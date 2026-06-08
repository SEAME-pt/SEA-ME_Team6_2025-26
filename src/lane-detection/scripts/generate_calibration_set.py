#!/usr/bin/env python3
"""
generate_calibration_set.py — SEAME Team 6
Gera imagens de calibração para compilação do HEF com Hailo DFC.

As imagens são processadas exactamente como durante o treino:
  1. Resize para 640×320
  2. Crop inferior para 640×160
  3. Normalização ImageNet (mean/std)
  4. Guardadas como numpy .npy para o Hailo DFC

Uso:
    python3 generate_calibration_set.py \
        --images_dir combined_dataset/clips/seame/train \
        --output_dir calibration_set \
        --num_images 64 \
        --input_w 640 --input_h 160 \
        --crop_ratio 0.5
"""

import argparse
import random
import numpy as np
from pathlib import Path

try:
    from PIL import Image
except ImportError:
    raise SystemExit("Instala Pillow: pip install Pillow")

# ImageNet normalisation (mesmo que o treino)
MEAN = np.array([0.485, 0.456, 0.406], dtype=np.float32)
STD  = np.array([0.229, 0.224, 0.225], dtype=np.float32)


def preprocess(img_path, input_w, input_h, crop_ratio):
    """
    Replica o preprocessing do DALI:
    1. Resize para input_w × (input_h / crop_ratio)
    2. Crop inferior para input_w × input_h
    3. Normaliza com ImageNet mean/std
    Retorna array float32 shape (3, input_h, input_w) — formato CHW
    """
    resize_h = int(input_h / crop_ratio)  # 160 / 0.5 = 320

    with Image.open(img_path).convert('RGB') as img:
        # Resize
        img = img.resize((input_w, resize_h), Image.BILINEAR)

        # Crop inferior
        top = resize_h - input_h  # 320 - 160 = 160
        img = img.crop((0, top, input_w, resize_h))

        # Converte para float32 e normaliza
        arr = np.array(img, dtype=np.float32) / 255.0  # HWC, 0-1
        arr = (arr - MEAN) / STD                        # normaliza

        # HWC → CHW
        arr = arr.transpose(2, 0, 1)

    return arr  # shape: (3, 160, 640)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--images_dir', default='combined_dataset/clips/seame/train',
                        help='Pasta com imagens de calibração (preferencialmente SEAME)')
    parser.add_argument('--output_dir', default='calibration_set')
    parser.add_argument('--num_images', type=int, default=64,
                        help='Número de imagens de calibração (64-256 recomendado)')
    parser.add_argument('--input_w',    type=int, default=640)
    parser.add_argument('--input_h',    type=int, default=160)
    parser.add_argument('--crop_ratio', type=float, default=0.5)
    args = parser.parse_args()

    output_dir = Path(args.output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    # Recolhe imagens
    images_dir = Path(args.images_dir)
    all_images = list(images_dir.rglob('*.jpg')) + list(images_dir.rglob('*.png'))

    if len(all_images) == 0:
        raise SystemExit(f"Nenhuma imagem encontrada em {images_dir}")

    print(f"Imagens disponíveis: {len(all_images)}")

    # Selecciona aleatoriamente
    random.seed(42)
    selected = random.sample(all_images, min(args.num_images, len(all_images)))
    print(f"Imagens seleccionadas: {len(selected)}")
    print(f"Preprocessing: resize {args.input_w}×{int(args.input_h/args.crop_ratio)} → crop {args.input_w}×{args.input_h}")

    # Processa e guarda
    all_arrays = []
    failed = 0
    for i, img_path in enumerate(selected):
        try:
            arr = preprocess(img_path, args.input_w, args.input_h, args.crop_ratio)
            all_arrays.append(arr)

            # Guarda individualmente (formato aceite pelo Hailo DFC)
            out_path = output_dir / f'calib_{i:04d}.npy'
            np.save(out_path, arr)

        except Exception as e:
            print(f"  ERRO em {img_path.name}: {e}")
            failed += 1

    # Guarda também como batch único (alternativa para o DFC)
    if all_arrays:
        batch = np.stack(all_arrays, axis=0)  # (N, 3, H, W)
        np.save(output_dir / 'calibration_batch.npy', batch)

    # Resumo
    print(f"\n{'='*50}")
    print(f"Output: {output_dir}/")
    print(f"  {len(all_arrays)} ficheiros calib_XXXX.npy  — shape (3, {args.input_h}, {args.input_w})")
    print(f"  calibration_batch.npy               — shape ({len(all_arrays)}, 3, {args.input_h}, {args.input_w})")
    if failed:
        print(f"  {failed} imagens falharam")
    print(f"\nValores de exemplo (primeira imagem):")
    if all_arrays:
        a = all_arrays[0]
        print(f"  shape: {a.shape}, dtype: {a.dtype}")
        print(f"  min: {a.min():.3f}, max: {a.max():.3f}, mean: {a.mean():.3f}")
    print(f"{'='*50}")
    print(f"\nPara usar no Hailo DFC:")
    print(f"  hailo optimize --har model.har \\")
    print(f"      --calib-set-path {output_dir}/ \\")
    print(f"      --output-har model_quantized.har")


if __name__ == '__main__':
    main()
