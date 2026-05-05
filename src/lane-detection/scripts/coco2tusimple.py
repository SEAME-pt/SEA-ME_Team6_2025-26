import json
import numpy as np
import os
from collections import defaultdict
from scipy.interpolate import interp1d

# === CONFIG ===
ROBOFLOW_ROOT = '/home/seame/UFLDv2_tusimple/Seame_dataset_new'
OUTPUT_ROOT   = '/home/seame/UFLDv2_tusimple/seame_dataset'
EXCLUDE_CATS  = {'crosswalk', 'Lane-Detection'}
H_SAMPLES     = list(range(160, 720, 10))  # 56 anchors
IMG_W         = 1280
IMG_H         = 720
MIN_POINTS    = 3

os.makedirs(OUTPUT_ROOT, exist_ok=True)
clips_root = os.path.join(OUTPUT_ROOT, 'clips', 'seame')

def polygon_to_centerline(segmentation, h_samples):
    """
    Converte um polygon de lane (dois lados) para centerline interpolada nos h_samples.
    Estratégia: para cada h_sample y, encontra todos os x do polígono nesse y
    e calcula a média (centro da lane).
    """
    if not segmentation:
        return [-2] * len(h_samples)

    pts = np.array(segmentation[0]).reshape(-1, 2)
    n = len(pts)

    result = []
    for h in h_samples:
        intersections = []
        # Verifica interseção de cada aresta do polígono com a linha y=h
        for i in range(n):
            p1 = pts[i]
            p2 = pts[(i + 1) % n]
            y1, y2 = p1[1], p2[1]
            x1, x2 = p1[0], p2[0]

            if y1 == y2:
                continue  # aresta horizontal, ignora

            # Verifica se h está entre y1 e y2
            if min(y1, y2) <= h <= max(y1, y2):
                # Interpolação linear para encontrar x na interseção
                t = (h - y1) / (y2 - y1)
                x = x1 + t * (x2 - x1)
                intersections.append(x)

        if len(intersections) >= 2:
            # Centerline = média de todas as interseções
            cx = int(round(np.mean(intersections)))
            if 0 <= cx <= IMG_W:
                result.append(cx)
            else:
                result.append(-2)
        else:
            result.append(-2)

    return result

def process_split(split_name):
    anno_path = os.path.join(ROBOFLOW_ROOT, split_name, '_annotations.coco.json')
    if not os.path.exists(anno_path):
        print(f'  Skipping {split_name} — not found')
        return []

    with open(anno_path) as f:
        data = json.load(f)

    cat_map = {c['id']: c['name'] for c in data['categories']}
    img_map = {img['id']: img for img in data['images']}

    ann_by_img = defaultdict(list)
    for ann in data['annotations']:
        cat_name = cat_map[ann['category_id']]
        if cat_name in EXCLUDE_CATS:
            continue
        ann_by_img[ann['image_id']].append(ann)

    entries = []
    clips_split = os.path.join(clips_root, split_name)

    for img_id, anns in ann_by_img.items():
        img_info = img_map[img_id]
        fname = img_info['file_name']

        idx = len(entries)
        clip_dir = os.path.join(clips_split, str(idx))
        os.makedirs(clip_dir, exist_ok=True)

        src = os.path.join(ROBOFLOW_ROOT, split_name, fname)
        dst = os.path.join(clip_dir, '1.jpg')
        if not os.path.exists(dst):
            os.symlink(src, dst)

        raw_file = f'clips/seame/{split_name}/{idx}/1.jpg'

        lanes = []
        for ann in anns:
            lane = polygon_to_centerline(ann['segmentation'], H_SAMPLES)
            valid_pts = sum(1 for x in lane if x != -2)
            if valid_pts >= MIN_POINTS:
                lanes.append(lane)

        if not lanes:
            continue

        # Ordena por x médio (esquerda → direita)
        def mean_x(lane):
            xs = [x for x in lane if x != -2]
            return sum(xs) / len(xs) if xs else 0

        lanes.sort(key=mean_x)

        # Pad para 4 lanes
        empty = [-2] * len(H_SAMPLES)
        while len(lanes) < 4:
            lanes.append(empty[:])
        lanes = lanes[:4]

        entries.append({
            'lanes': lanes,
            'h_samples': H_SAMPLES,
            'raw_file': raw_file
        })

    print(f'  {split_name}: {len(entries)} entradas')
    return entries

print('A processar splits...')
train_entries = process_split('train')
valid_entries = process_split('valid')
test_entries  = process_split('test')

def write_jsonl(entries, path):
    with open(path, 'w') as f:
        for e in entries:
            f.write(json.dumps(e) + '\n')

def write_txt(entries, path):
    with open(path, 'w') as f:
        for e in entries:
            f.write(f"{e['raw_file']} seg_seame/placeholder.png 0\n")

write_jsonl(train_entries, os.path.join(OUTPUT_ROOT, 'label_train.json'))
write_jsonl(valid_entries, os.path.join(OUTPUT_ROOT, 'label_valid.json'))
write_jsonl(test_entries,  os.path.join(OUTPUT_ROOT, 'label_test.json'))
write_jsonl(test_entries,  os.path.join(OUTPUT_ROOT, 'test_label.json'))
write_txt(train_entries,   os.path.join(OUTPUT_ROOT, 'train_gt.txt'))
write_txt(test_entries,    os.path.join(OUTPUT_ROOT, 'test.txt'))

print()
print('=== RESUMO ===')
print(f'Train: {len(train_entries)} | Valid: {len(valid_entries)} | Test: {len(test_entries)}')
print(f'Output: {OUTPUT_ROOT}')

# Verificação visual da primeira anotação
print()
print('=== VERIFICAÇÃO ===')
with open('/home/seame/UFLDv2_tusimple/Seame_dataset_new/train/_annotations.coco.json') as f:
    data = json.load(f)
cat_map = {c['id']: c['name'] for c in data['categories']}
for ann in data['annotations'][:20]:
    if cat_map[ann['category_id']] in EXCLUDE_CATS:
        continue
    lane = polygon_to_centerline(ann['segmentation'], H_SAMPLES)
    valid = [(h, x) for h, x in zip(H_SAMPLES, lane) if x != -2]
    print(f"Category: {cat_map[ann['category_id']]}")
    print(f"Centerline points (y, x): {valid[:8]}...")
    break

print()
print('Primeiras 2 entradas train:')
for e in train_entries[:2]:
    valid_per_lane = [sum(1 for x in l if x != -2) for l in e['lanes']]
    xs_medios = []
    for l in e['lanes']:
        xs = [x for x in l if x != -2]
        xs_medios.append(round(sum(xs)/len(xs)) if xs else -1)
    print(f"  {e['raw_file']}: válidos={valid_per_lane}, x_médio={xs_medios}")
