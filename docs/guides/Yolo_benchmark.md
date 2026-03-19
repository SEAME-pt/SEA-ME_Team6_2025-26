# Yolo Benchmark

This file will help us see how Yolo performs on hailo and on carla, by checking fps, accuracy, mAP and energy consumption.

# Yolo benchmark on Hailo

1. so first off we need to get the pre compiled HEF, in our case we needed the Yolov8s, Yolov8m, Yolov11m, Yolov11s and Yolov11n.
2. Once you get the HEFs we need a script that lets us do the benchmark.


          import hailo_platform as hp
          import numpy as np
          import time
          import json
          
          def benchmark_hef(hef_path: str, n_frames: int = 500) -> dict:
              """Benchmark de throughput e latência no Hailo-8."""
              
              hef = hp.HEF(hef_path)
              target = hp.VDevice()
              
              network_group = target.configure(hef)[0]
              network_group_params = network_group.create_params()
              
              input_vstreams_params = hp.InputVStreamParams.make(
                  network_group, quantized=False,
                  format_type=hp.FormatType.FLOAT32
              )
              output_vstreams_params = hp.OutputVStreamParams.make(
                  network_group, quantized=False,
                  format_type=hp.FormatType.FLOAT32
              )
              
              input_info = hef.get_input_vstream_infos()[0]
              h, w, c = input_info.shape
              
              # Frame sintético (substitui por frames reais da câmara)
              dummy_frame = np.random.uint8(0, 255, (h, w, c)).astype(np.float32)
              
              latencies = []
              
              with hp.InferVStreams(
                  network_group, input_vstreams_params, output_vstreams_params
              ) as infer_pipeline:
                  with network_group.activate(network_group_params):
                      # Warm-up
                      for _ in range(20):
                          infer_pipeline.infer({input_info.name: dummy_frame[np.newaxis]})
                      
                      # Benchmark
                      t_start = time.perf_counter()
                      for _ in range(n_frames):
                          t0 = time.perf_counter()
                          infer_pipeline.infer({input_info.name: dummy_frame[np.newaxis]})
                          latencies.append((time.perf_counter() - t0) * 1000)
                      t_total = time.perf_counter() - t_start
              
              return {
                  "hef": hef_path,
                  "n_frames": n_frames,
                  "total_time_s": t_total,
                  "throughput_fps": n_frames / t_total,
                  "latency_avg_ms": np.mean(latencies),
                  "latency_p99_ms": np.percentile(latencies, 99),
                  "latency_min_ms": np.min(latencies),
              }
          
          # Executar benchmark
          models_to_test = [
              "hailo_model_zoo/models/yolov8/yolov8s.hef",
              "hailo_model_zoo/models/yolov8/yolov8m.hef",
          ]
          
          all_results = [benchmark_hef(m) for m in models_to_test]
          
          with open("/data/seame-configs/hailo_benchmark_results.json", "w") as f:
              json.dump(all_results, f, indent=2)
          
          for r in all_results:
              print(f"{r['hef']}: {r['throughput_fps']:.1f} FPS | "
                    f"lat avg={r['latency_avg_ms']:.1f}ms p99={r['latency_p99_ms']:.1f}ms")


3. After the script runs check your results, our results were:
   
**Yolov8m**

| Run ID | FPS   | Avg Latency (ms) | P50 (ms) | P95 (ms) | P99 (ms) | Min (ms) | Max (ms) | StdDev | CPU Avg (%) | CPU Max (%) |
| ------ | ----- | ---------------- | -------- | -------- | -------- | -------- | -------- | ------ | ----------- | ----------- |
| 0      | 29.16 | 34.29            | 34.29    | 34.48    | 34.85    | 32.85    | 35.12    | 0.16   | 6.9         | 7.6         |
| 1      | 29.05 | 34.42            | 34.41    | 34.64    | 35.20    | 32.98    | 35.33    | 0.20   | 7.0         | 8.8         |
| 2      | 29.16 | 34.29            | 34.29    | 34.46    | 35.07    | 33.02    | 35.20    | 0.19   | 7.2         | 9.3         |

**Yolov8s**

| Run ID | FPS   | Avg Latency (ms) | P50 (ms) | P95 (ms) | P99 (ms) | Min (ms) | Max (ms) | StdDev | CPU Avg (%) | CPU Max (%) |
| ------ | ----- | ---------------- | -------- | -------- | -------- | -------- | -------- | ------ | ----------- | ----------- |
| 0      | 75.76 | 13.20            | 13.17    | 13.46    | 13.78    | 12.34    | 13.99    | 0.17   | 15.3        | 18.5        |
| 1      | 75.65 | 13.22            | 13.19    | 13.47    | 13.67    | 12.22    | 14.06    | 0.17   | 15.1        | 15.6        |
| 2      | 75.36 | 13.27            | 13.24    | 13.55    | 13.85    | 12.30    | 14.08    | 0.18   | 15.0        | 16.7        |

**Yolov11m**

| Run ID | FPS   | Avg Latency (ms) | P50 (ms) | P95 (ms) | P99 (ms) | Min (ms) | Max (ms) | StdDev | CPU Avg (%) | CPU Max (%) |
| ------ | ----- | ---------------- | -------- | -------- | -------- | -------- | -------- | ------ | ----------- | ----------- |
| 0      | 23.12 | 43.25            | 43.25    | 43.85    | 44.03    | 41.87    | 44.50    | 0.28   | 5.6         | 7.5         |
| 1      | 23.12 | 43.26            | 43.25    | 43.84    | 44.22    | 41.08    | 44.72    | 0.29   | 5.7         | 7.6         |
| 2      | 23.12 | 43.26            | 43.25    | 43.92    | 44.30    | 40.36    | 45.10    | 0.35   | 6.0         | 7.3         |

**Yolov11n**

| Run ID | FPS   | Avg Latency (ms) | P50 (ms) | P95 (ms) | P99 (ms) | Min (ms) | Max (ms) | StdDev | CPU Avg (%) | CPU Max (%) |
| ------ | ----- | ---------------- | -------- | -------- | -------- | -------- | -------- | ------ | ----------- | ----------- |
| 0      | 69.04 | 14.48            | 14.49    | 14.72    | 15.13    | 13.56    | 15.38    | 0.18   | 14.2        | 15.5        |
| 1      | 69.31 | 14.43            | 14.44    | 14.69    | 14.96    | 12.86    | 15.26    | 0.20   | 13.9        | 14.7        |
| 2      | 68.00 | 14.70            | 14.71    | 14.97    | 15.27    | 13.52    | 15.67    | 0.19   | 13.9        | 16.2        |

**Yolov11s**

| Run ID | FPS   | Avg Latency (ms) | P50 (ms) | P95 (ms) | P99 (ms) | Min (ms) | Max (ms) | StdDev | CPU Avg (%) | CPU Max (%) |
| ------ | ----- | ---------------- | -------- | -------- | -------- | -------- | -------- | ------ | ----------- | ----------- |
| 0      | 42.93 | 23.29            | 23.39    | 23.59    | 24.08    | 20.85    | 24.26    | 0.32   | 9.9         | 11.3        |
| 1      | 43.03 | 23.24            | 23.39    | 23.62    | 24.19    | 20.91    | 24.47    | 0.40   | 10.0        | 11.8        |
| 2      | 43.02 | 23.24            | 23.40    | 23.58    | 24.02    | 20.93    | 24.37    | 0.37   | 10.1        | 11.3        |

4. After you get your results check them against the minimun aceptance criteria:

- Throughput: > 20 FPS (pipeline completo câmara → inferência → CAN)
- Latência p99: < 50ms (requisito AEB)
- mAP50: > 40% (COCO)
- Consumo Hailo-8: < 8W
   
5. after you compare it to the requisists choose whats the best for you project, in our case is **Yolov8s**.


# Yolo benchmark on CARLA

  - Step by step on how to do a benchmark on carla of Yolo models

**Prerequisites**

  - Python 3.8+
  - Pip
  - Carla simulator

**Yolo models installation**

  - First we need to create a virtual environment.
    
          python -m venv yolo-env
          
          source yolo-env/bin/activate

  - Then we need the ultralytics peckage, this peckage will give us access to yolov8s and yolo26s.

          pip install --upgrade pip
          pip install ultralytics

  - Run inference with each model, this will give us the file for each model

    **Yolov8s**

          yolo predict model=yolov8s.pt source='https://ultralytics.com/images/bus.jpg'

    **Yolo26s**

          yolo predict model=yolo26s.pt source='https://ultralytics.com/images/bus.jpg'

**Carla Dataset**

  - Now we need to run carla and get a dataset of images so we can do our benchmark, we used a script to help us get the dataset


          #!/usr/bin/env python3
          """
          CARLA 0.9.15 - Dataset Collector for YOLO
          Optimized version with:
          - Town03 freeze skip
          - Smart spawn
          - Reduced traffic in Town03
          - Traffic Manager tuning
          - Safe actor destruction
          """
          
          import carla
          import cv2
          import numpy as np
          import os
          import random
          import time
          from pathlib import Path
          
          # ─────────────────────────────────────────
          #  CONFIG
          # ─────────────────────────────────────────
          CARLA_HOST = 'localhost'
          CARLA_PORT = 2000
          IMAGE_W    = 1280
          IMAGE_H    = 720
          FPS        = 10
          N_FRAMES   = 3000
          TRAIN_RATIO = 0.8
          VAL_RATIO   = 0.15
          
          MAPS = ['Town01', 'Town02', 'Town04', 'Town05']
          FRAMES_PER_MAP = N_FRAMES // len(MAPS)
          
          CLASSES = {
              'vehicle':       0,
              'pedestrian':    1,
              'traffic_light': 2,
          }
          
          # ─────────────────────────────────────────
          #  DATASET FOLDERS
          # ─────────────────────────────────────────
          BASE = Path('dataset')
          for split in ['train', 'val', 'test']:
              (BASE / 'images' / split).mkdir(parents=True, exist_ok=True)
              (BASE / 'labels' / split).mkdir(parents=True, exist_ok=True)
          
          print("✅ Dataset folders created.")
          
          # ─────────────────────────────────────────
          #  CAMERA UTILS
          # ─────────────────────────────────────────
          def build_projection_matrix(w, h, fov=90):
              f = w / (2.0 * np.tan(np.radians(fov / 2)))
              cx, cy = w / 2.0, h / 2.0
              return np.array([[f,0,cx],[0,f,cy],[0,0,1]])
          
          def world_to_camera(location, camera_transform):
              world_mat = np.array(camera_transform.get_matrix())
              inv_cam   = np.linalg.inv(world_mat)
              pt_world  = np.array([location.x, location.y, location.z, 1.0])
              pt_cam    = inv_cam @ pt_world
              return np.array([pt_cam[1], -pt_cam[2], pt_cam[0]])
          
          def project_3d_to_2d(pt_cam, K):
              if pt_cam[2] <= 0:
                  return None
              px = K[0,0] * pt_cam[0] / pt_cam[2] + K[0,2]
              py = K[1,1] * pt_cam[1] / pt_cam[2] + K[1,2]
              return px, py
          
          def get_bbox_2d(actor, camera_transform, K, img_w, img_h):
              bb = actor.bounding_box
              verts = bb.get_world_vertices(actor.get_transform())
              points_2d = []
          
              for v in verts:
                  pt_cam = world_to_camera(v, camera_transform)
                  pt_2d  = project_3d_to_2d(pt_cam, K)
                  if pt_2d is not None:
                      points_2d.append(pt_2d)
          
              if len(points_2d) < 4:
                  return None
          
              xs = [p[0] for p in points_2d]
              ys = [p[1] for p in points_2d]
              x1, x2 = max(0, min(xs)), min(img_w, max(xs))
              y1, y2 = max(0, min(ys)), min(img_h, max(ys))
          
              if x2 - x1 < 5 or y2 - y1 < 5:
                  return None
          
              xc = ((x1 + x2) / 2) / img_w
              yc = ((y1 + y2) / 2) / img_h
              bw = (x2 - x1) / img_w
              bh = (y2 - y1) / img_h
          
              return xc, yc, bw, bh
          
          def get_tl_bbox_2d(tl, camera_transform, K, img_w, img_h):
              loc = tl.get_location()
              loc.z += 0.5
          
              pt_cam = world_to_camera(loc, camera_transform)
              pt_2d  = project_3d_to_2d(pt_cam, K)
              if pt_2d is None:
                  return None
          
              # bounding box fixa e maior para semáforos
              xc = pt_2d[0] / img_w
              yc = pt_2d[1] / img_h
          
              # ignorar semáforos demasiado perto das bordas
              if not (0.01 < xc < 0.99 and 0.01 < yc < 0.99):
                  return None
          
              # caixas maiores e consistentes
              bw = 0.04   # largura 4% da imagem
              bh = 0.10   # altura 10% da imagem
          
              return xc, yc, bw, bh
          
          
          # ─────────────────────────────────────────
          #  SPLIT
          # ─────────────────────────────────────────
          def get_split(frame_idx, total):
              r = frame_idx / total
              if r < TRAIN_RATIO:
                  return 'train'
              elif r < TRAIN_RATIO + VAL_RATIO:
                  return 'val'
              else:
                  return 'test'
          
          # ─────────────────────────────────────────
          #  MAIN LOOP
          # ─────────────────────────────────────────
          def collect(map_name, frames_target, global_offset, total_frames):
              print(f"\n🗺  Loading map: {map_name}")
          
              client = carla.Client(CARLA_HOST, CARLA_PORT)
              client.set_timeout(30.0)
          
              world = client.load_world(map_name)
              time.sleep(2)
          
              # Synchronous mode
              settings = world.get_settings()
              settings.synchronous_mode = True
              settings.fixed_delta_seconds = 1.0 / FPS
              world.apply_settings(settings)
          
              blueprint_library = world.get_blueprint_library()
              traffic_manager   = client.get_trafficmanager(8000)
              traffic_manager.set_synchronous_mode(True)
          
              # ─────────────────────────────────────────
              #  SPECIAL HANDLING FOR TOWN03
              # ─────────────────────────────────────────
              # Traffic per map (safe values)
              TRAFFIC_CONFIG = {
              "Town01": {"vehicles": 80, "walkers": 80},
              "Town02": {"vehicles": 80, "walkers": 80},
              "Town04": {"vehicles": 75, "walkers": 80},
              "Town05": {"vehicles": 80, "walkers": 80},
          }
          
              # Apply traffic config
              cfg = TRAFFIC_CONFIG.get(map_name, {"vehicles": 30, "walkers": 20})
              n_vehicles = cfg["vehicles"]
              n_walkers  = cfg["walkers"]
          
              print(f"🚦 Traffic for {map_name}: {n_vehicles} vehicles, {n_walkers} walkers")
          
              # ─────────────────────────────────────────
              #  SPAWN VEHICLES (SMART)
              # ─────────────────────────────────────────
              spawn_points = world.get_map().get_spawn_points()
              random.shuffle(spawn_points)
          
              vehicle_bps = blueprint_library.filter('vehicle.*')
              npc_vehicles = []
          
              for sp in spawn_points[:n_vehicles]:
                  bp = random.choice(vehicle_bps)
                  if bp.has_attribute('number_of_wheels') and int(bp.get_attribute('number_of_wheels')) < 4:
                      continue
                  npc = world.try_spawn_actor(bp, sp)
                  if npc:
                      npc.set_autopilot(True)
                      npc_vehicles.append(npc)
          
              # ─────────────────────────────────────────
              #  SPAWN PEDESTRIANS
              # ─────────────────────────────────────────
              walker_bps     = blueprint_library.filter('walker.pedestrian.*')
              walker_ctrl_bp = blueprint_library.find('controller.ai.walker')
              walkers, walker_ctrls = [], []
          
              for _ in range(n_walkers):
                  loc = world.get_random_location_from_navigation()
                  if loc is None:
                      continue
                  bp  = random.choice(walker_bps)
                  w   = world.try_spawn_actor(bp, carla.Transform(loc))
                  if w:
                      ctrl = world.spawn_actor(walker_ctrl_bp, carla.Transform(), attach_to=w)
                      walkers.append(w)
                      walker_ctrls.append(ctrl)
          
              world.tick()
              for ctrl in walker_ctrls:
                  ctrl.start()
                  ctrl.go_to_location(world.get_random_location_from_navigation())
                  ctrl.set_max_speed(1.4)
          
              # ─────────────────────────────────────────
              #  EGO VEHICLE
              # ─────────────────────────────────────────
              ego_bp = blueprint_library.filter('vehicle.tesla.model3')[0]
              ego    = world.spawn_actor(ego_bp, spawn_points[-1])
              ego.set_autopilot(True, traffic_manager.get_port())
          
              traffic_manager.ignore_lights_percentage(ego, 20)
              traffic_manager.ignore_signs_percentage(ego, 20)
              traffic_manager.vehicle_percentage_speed_difference(ego, -20)
          
              # ─────────────────────────────────────────
              #  CAMERA
              # ─────────────────────────────────────────
              cam_bp = blueprint_library.find('sensor.camera.rgb')
              cam_bp.set_attribute('image_size_x', str(IMAGE_W))
              cam_bp.set_attribute('image_size_y', str(IMAGE_H))
              cam_bp.set_attribute('fov', '90')
              cam_tf = carla.Transform(carla.Location(x=2.0, z=1.4))
              camera = world.spawn_actor(cam_bp, cam_tf, attach_to=ego)
          
              K = build_projection_matrix(IMAGE_W, IMAGE_H, fov=90)
              frame_buffer = []
          
              def on_image(image):
                  arr = np.frombuffer(image.raw_data, dtype=np.uint8)
                  arr = arr.reshape((IMAGE_H, IMAGE_W, 4))[:, :, :3]
                  frame_buffer.append(arr.copy())
          
              camera.listen(on_image)
          
              saved   = 0
              tick_n  = 0
              save_every = max(1, FPS // 5)
          
              print(f"  🚗 {len(npc_vehicles)} vehicles | 🚶 {len(walkers)} pedestrians")
              print(f"  📸 Collecting {frames_target} frames...")
          
              try:
                  while saved < frames_target:
          
                      # ─────────────────────────────────────────
                      #  SAFE TICK WITH TIMEOUT (SKIP MAP IF FREEZE)
                      # ─────────────────────────────────────────
                      try:
                          world.tick()
                      except RuntimeError:
                          print(f"❌ Tick timeout in {map_name} → Skipping this map.")
                          break
          
                      tick_n += 1
          
                      if tick_n % save_every != 0:
                          continue
                      if not frame_buffer:
                          continue
          
                      img = frame_buffer[-1]
                      frame_buffer.clear()
          
                      cam_transform = camera.get_transform()
                      labels = []
          
                      # Vehicles
                      for actor in world.get_actors().filter('vehicle.*'):
                          if actor.id == ego.id:
                              continue
                          dist = ego.get_location().distance(actor.get_location())
                          if dist > 80:
                              continue
                          bb = get_bbox_2d(actor, cam_transform, K, IMAGE_W, IMAGE_H)
                          if bb:
                              labels.append(f"{CLASSES['vehicle']} {bb[0]:.6f} {bb[1]:.6f} {bb[2]:.6f} {bb[3]:.6f}")
          
                      # Pedestrians
                      for actor in world.get_actors().filter('walker.pedestrian.*'):
                          dist = ego.get_location().distance(actor.get_location())
                          if dist > 80:
                              continue
                          bb = get_bbox_2d(actor, cam_transform, K, IMAGE_W, IMAGE_H)
                          if bb:
                              labels.append(f"{CLASSES['pedestrian']} {bb[0]:.6f} {bb[1]:.6f} {bb[2]:.6f} {bb[3]:.6f}")
          
                      # Traffic lights
                      for actor in world.get_actors().filter('traffic.traffic_light*'):
                          dist = ego.get_location().distance(actor.get_location())
                          if dist > 80:
                              continue
                          bb = get_tl_bbox_2d(actor, cam_transform, K, IMAGE_W, IMAGE_H)
                          if bb:
                              labels.append(f"{CLASSES['traffic_light']} {bb[0]:.6f} {bb[1]:.6f} {bb[2]:.6f} {bb[3]:.6f}")
          
                      if not labels:
                          continue
          
                      global_idx = global_offset + saved
                      split = get_split(global_idx, frames_target)
                      fname      = f"frame_{global_idx:06d}"
          
                      img_path   = BASE / 'images' / split / f"{fname}.jpg"
                      lbl_path   = BASE / 'labels' / split / f"{fname}.txt"
          
                      cv2.imwrite(str(img_path), img, [cv2.IMWRITE_JPEG_QUALITY, 92])
                      with open(lbl_path, 'w') as f:
                          f.write('\n'.join(labels))
          
                      saved += 1
                      if saved % 100 == 0:
                          print(f"  ✅ {saved}/{frames_target} frames saved ({split})")
          
              finally:
                  # ── 1. Stop camera listener ──
                  try:
                      camera.stop()
                  except:
                      pass
          
                  # ── 2. Switch to async mode ──
                  try:
                      settings.synchronous_mode = False
                      world.apply_settings(settings)
                  except:
                      pass
          
                  time.sleep(0.5)
          
                  # ── 3. Destroy actors safely ──
                  alive_ids = []
                  try:
                      world_actors = world.get_actors()
                  except:
                      world_actors = []
          
                  all_actors = [camera, ego] + npc_vehicles + walker_ctrls + walkers
          
                  for actor in all_actors:
                      try:
                          if actor and world_actors.find(actor.id) is not None:
                              alive_ids.append(actor.id)
                      except:
                          continue
          
                  if alive_ids:
                      try:
                          client.apply_batch([carla.command.DestroyActor(aid) for aid in alive_ids])
                      except:
                          pass
          
                  time.sleep(1.0)
                  print(f"  🏁 Map {map_name} done — {saved} frames saved.")
          
              return saved
          
          # ─────────────────────────────────────────
          #  YAML
          # ─────────────────────────────────────────
          def write_yaml():
              yaml_content = f"""path: {BASE.resolve()}
          train: images/train
          val:   images/val
          test:  images/test
          
          nc: {len(CLASSES)}
          names:
            0: vehicle
            1: pedestrian
            2: traffic_light
          """
              with open(BASE / 'data.yaml', 'w') as f:
                  f.write(yaml_content)
              print(f"\n📄 data.yaml written → {BASE / 'data.yaml'}")
          
          # ─────────────────────────────────────────
          #  MAIN
          # ─────────────────────────────────────────
          if __name__ == '__main__':
              import sys
          
              # Escolhe o mapa pela linha de comando, ex:
              # python3 caradataset.py Town01
              if len(sys.argv) < 2:
                  print("Usage: python3 caradataset.py Town01")
                  sys.exit(1)
          
              map_name = sys.argv[1]
              if map_name not in MAPS:
                  print(f"Map {map_name} not in MAPS list: {MAPS}")
                  sys.exit(1)
          
              # frames por mapa (podes ajustar)
              frames_target = FRAMES_PER_MAP
              total_frames  = N_FRAMES
          
              saved = collect(map_name, frames_target, 0, total_frames)
              write_yaml()
          
              print("\nDone.")
          

    **Run the benchmark**

      - After getting the dataset we need to run the yolo command that allows us to run the benchmark

              yolo detect train data=dataset/data.yaml model=yolov8s.pt imgsz=1280 epochs=50 batch=2 workers=2

      After running the benchmark it will appear a folder with the results.


    **Yolo Benchmark results**

    **Yolov8s**

    all:          mAP50 = 0.587
    vehicle:      mAP50 = 0.780
    pedestrian:   mAP50 = 0.104
    traffic_light mAP50 = 0.877

    
    <img width="2400" height="1200" alt="results" src="https://github.com/user-attachments/assets/285e9ac2-4dd7-4118-a0cd-df8ee327449a" />

    **Yolo26s**

    all:            mAP50 = 0.595
    vehicle:        mAP50 = 0.730
    pedestrian:     mAP50 = 0.163
    traffic_light:  mAP50 = 0.893

    <img width="2400" height="1200" alt="results" src="https://github.com/user-attachments/assets/7033f276-ed03-492c-b851-d9e341eb5186" />


    **Conclusion**

    We decided to pick Yolov8s because between the benchmark on hailo and on carla was the one with the better results.
