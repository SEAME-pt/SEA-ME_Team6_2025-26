root@d61610b30f6f:/local/workspace/shared_with_docker# scp /local/workspace/shared_with_docker/hef/yolov8n_h8.hef root@10.21.220.191:/data/
scp /local/workspace/shared_with_docker/hef/yolov8n_seg_h8.hef root@10.21.220.191:/data/
scp /local/workspace/shared_with_docker/hef/yolo26n_seg_320_h8_no_nms.hef root@10.21.220.191:/data/
yolov8n_h8.hef                                                                    100% 4260KB   9.1MB/s   00:00    
yolov8n_seg_h8.hef                                                                100% 7476KB   8.1MB/s   00:00    
yolo26n_seg_320_h8_no_nms.hef                                                     100% 8299KB   9.0MB/s   00:00    
root@d61610b30f6f:/local/workspace/shared_with_docker# history
    1  cd /local/workspace/hailo_model_zoo/hailo_models/custom_models
    2  hailomz compile yolov8n   --har yolov8n_simplified.har
    3  clear
    4  hailomz convert     --onnx yolov8n_simplified.onnx     --har yolov8n_simplified.har
    5  cd ..
    6  ls
    7  cd .. 
    8  ls
    9  cd hailo_models
   10  ls
   11  cd custom_models/
   12  ls
   13  cd ..
   14  ls
   15  cd ..
   16  ls
   17  hailo_model_zoo
   18  cd hailo_model_zoo
   19  ls
   20  cd ..
   21  clear
   22  cd ..
   23  ls
   24  cd shared_with_docker/
   25  ls
   26  cd doc/
   27  ls
   28  cd COCO
   29  ls -lh
   30  cd ..
   31  ls -lh
   32  cd COCO/
   33  ls
   34  cd val2017
   35  ls
   36  clear
   37  cd /local/workspace/shared_with_docker
   38  hailomz compile yolov8n   --onnx yolov8n_simplified.onnx   --har yolov8n_simplified.har
   39  clear
   40  hailomz parse yolov8n --onnx yolov8n_simplified.onnx
   41  clear
   42  ls
   43  cd /local/workspace/shared_with_docker
   44  hailomz compile yolov8n_simplified   --calibration /local/workspace/shared_with_docker/.hailomz/models_files/coco/2021-06-18/coco_calib2017.tfrecord
   45  ls
   46  clear
   47  nano yolov8n_simplified.yaml
   48  vim yolov8n_simplified.yaml
   49  cat yolov8n_simplified.
   50  cat yolov8n_simplified.yaml 
   51  cd .hailomz/models_files/coco/2021-06-18/
   52  ls
   53  cd ..
   54  cd ../../..
   55  hailomz compile   --yaml yolov8n_simplified.yaml   --ckpt yolov8n_simplified.onnx   --calib-path .hailomz/models_files/coco/2021-06-18/coco_calib2017.tfrecord   --har yolov8n_simplified.har
   56  ls -lh
   57  clear
   58  ls
   59  vim yolov8n_simplified.yaml 
   60  hailomz compile   --yaml yolov8n_simplified.yaml   --ckpt yolov8n_simplified.onnx   --calib-path .hailomz/models_files/coco/2021-06-18/coco_calib2017.tfrecord   --har yolov8n_simplified.har
   61  vim yolov8n_simplified.yaml 
   62  hailomz compile   --yaml yolov8n_simplified.yaml   --ckpt yolov8n_simplified.onnx   --calib-path .hailomz/models_files/coco/2021-06-18/coco_calib2017.tfrecord   --har yolov8n_simplified.har
   63  clear
   64  hailomz compile   --yaml yolov8n_simplified.yaml   --ckpt yolov8n_simplified.onnx   --calib-path .hailomz/models_files/coco/2021-06-18/coco_calib2017.tf
   65  clear
   66  hailomz compile   --yaml yolov8n_simplified.yaml   --ckpt yolov8n_simplified.onnx   --calib-path .hailomz/models_files/coco/2021-06-18/coco_calib2017.tfrecord
   67  clear
   68  ls
   69  cat yolov8n_simplified.yaml 
   70  nano yolov8n_simplified.yaml 
   71  vim  yolov8n_simplified.yaml 
   72  cat yolov8n_simplified.yaml 
   73  hailomz compile   --yaml yolov8n_simplified.yaml   --ckpt yolov8n_simplified.onnx   --calib-path .hailomz/models_files/coco/2021-06-18/coco_calib2017.tfrecord   --har yolov8n_simplified.har
   74  clearr
   75  clear
   76  hailomz compile   --yaml yolov8n_simplified.yaml   --ckpt yolov8n_simplified.onnx   --calib-path .hailomz/models_files/coco/2021-06-18/coco_calib2017.tfrecord
   77  hailomz compile   --yaml yolov8n_simplified.yaml   --ckpt yolov8n_simplified.onnx   --calib-path .hailomz/models_files/coco/2021-06-18/coco_calib2017.tfrecord   --har ./yolov8n_simplified.har
   78  hailomz compile   --yaml /local/workspace/shared_with_docker/yolov8n_simplified.yaml   --ckpt /local/workspace/shared_with_docker/yolov8n_simplified.onnx   --calib-path /local/workspace/shared_with_docker/.hailomz/models_files/coco/2021-06-18/coco_calib2017.tfrecord   --har /local/workspace/shared_with_docker/yolov8n_simplified.har
   79  clear
   80  hailomz compile   --yaml /local/workspace/shared_with_docker/yolov8n_simplified.yaml   --ckpt /local/workspace/shared_with_docker/yolov8n_simplified.onnx   --calib-path /local/workspace/shared_with_docker/.hailomz/models_files/coco/2021-06-18/coco_calib2017.tfrecord
   81  clear
   82  cat yolov8n_simplified.yaml 
   83  vim yolov8n_simplified.yaml 
   84  hailomz compile   --yaml /local/workspace/shared_with_docker/yolov8n_simplified.yaml   --ckpt /local/workspace/shared_with_docker/yolov8n_simplified.onnx   --calib-path /local/workspace/shared_with_docker/.hailomz/models_files/coco/2021-06-18/coco_calib2017.tfrecord
   85  hailomz compile   --yaml /local/workspace/shared_with_docker/yolov8n_simplified.yaml   --ckpt /local/workspace/shared_with_docker/yolov8n_simplified.onnx   --calib-path /local/workspace/shared_with_docker/.hailomz/models_files/coco/2021-06-18/coco_calib2017.tfrecord
   86  clear
   87  vim yolov8n_simplified.yaml 
   88  cat yolov8n_simplified.yaml 
   89  vim yolov8n_simplified.yaml 
   90  python3 - << 'EOF'
   91  import onnx
   92  model = onnx.load("/local/workspace/shared_with_docker/yolov8n_simplified.onnx")
   93  print("INPUTS:")
   94  for i in model.graph.input:
   95      print(i.name)
   96  print("\nOUTPUTS:")
   97  for o in model.graph.output:
   98      print(o.name)
   99  EOF
  100  cat yolov8n_simplified.yaml 
  101  hailomz compile   --yaml /local/workspace/shared_with_docker/yolov8n_simplified.yaml   --ckpt /local/workspace/shared_with_docker/yolov8n_simplified.onnx   --calib-path /local/workspace/shared_with_docker/.hailomz/models_files/coco/2021-06-18/coco_calib2017.tfrecord
  102  clear
  103  vim yolov8n_simplified.yaml 
  104  hailomz compile   --yaml /local/workspace/shared_with_docker/yolov8n_simplified.yaml   --ckpt /local/workspace/shared_with_docker/yolov8n_simplified.onnx   --calib-path /local/workspace/shared_with_docker/.hailomz/models_files/coco/2021-06-18/coco_calib2017.tfrecord
  105  cat yolov8n_simplified.yaml clear
  106  clear
  107  vim yolov8n_simplified.yaml 
  108  cat yolov8n_simplified.yaml 
  109  hailomz compile   --yaml /local/workspace/shared_with_docker/yolov8n_simplified.yaml   --ckpt /local/workspace/shared_with_docker/yolov8n_simplified.onnx   --calib-path /local/workspace/shared_with_docker/.hailomz/models_files/coco/2021-06-18/coco_calib2017.tfrecord
  110  clear
  111  python3 - << 'EOF'
  112  import onnx
  113  model = onnx.load("yolov8n_simplified.onnx")
  114  onnx.checker.check_model(model)
  115  print("ONNX OK")
  116  EOF
  117  clear
  118  exit
  119  cd shared_with_docker/
  120  vim yolov8n_simplified.yaml 
  121  ls
  122  vim run_onnx_cpu.py 
  123  import os
  124  import cv2
  125  import torch
  126  import onnxruntime as ort
  127  import numpy as np
  128  # --- Configurações ---
  129  model_path = "yolov8n.onnx"
  130  images_dir = "COCO/val2017"
  131  output_dir = "outputs"
  132  os.makedirs(output_dir, exist_ok=True)
  133  # --- Carregar modelo ONNX ---
  134  ort_sess = ort.InferenceSession(model_path, providers=['CPUExecutionProvider'])
  135  input_name = ort_sess.get_inputs()[0].name
  136  # --- Listar imagens ---
  137  img_files = [f for f in os.listdir(images_dir) if f.lower().endswith((".jpg", ".png"))]
  138  print(f"Found {len(img_files)} images in {images_dir}/")
  139  # --- Função para pré-processamento ---
  140  def preprocess_image(img_path):
  141  # --- Rodar inferência e salvar imagens ---
  142  for img_file in img_files:;     img_path = os.path.join(images_dir, img_file)
  143  print("Inference completed for all images.")
  144  import os
  145  import cv2
  146  import torch
  147  import onnxruntime as ort
  148  import numpy as np
  149  # --- Configurações ---
  150  model_path = "yolov8n.onnx"
  151  images_dir = "COCO/val2017"
  152  output_dir = "outputs"
  153  os.makedirs(output_dir, exist_ok=True)
  154  # --- Carregar modelo ONNX ---
  155  ort_sess = ort.InferenceSession(model_path, providers=['CPUExecutionProvider'])
  156  input_name = ort_sess.get_inputs()[0].name
  157  # --- Listar imagens ---
  158  img_files = [f for f in os.listdir(images_dir) if f.lower().endswith((".jpg", ".png"))]
  159  print(f"Found {len(img_files)} images in {images_dir}/")
  160  # --- Função para pré-processamento ---
  161  def preprocess_image(img_path):
  162  # --- Rodar inferência e salvar imagens ---
  163  for img_file in img_files:;     img_path = os.path.join(images_dir, img_file)
  164  print("Inference completed for all images.")
  165  clear
  166  vim run_onnx_cpu.py 
  167  python3 run_onnx_cpu.py
  168  clear
  169  cd .. 
  170  cd hailo_model_zoo/hailo_models/custom_models/
  171  ls
  172  cd output_test/
  173  ls
  174  # Copiar modelo ONNX do shared_with_docker (host) para dentro do container
  175  cp /local/workspace/shared_with_docker/yolov8n_simplified.onnx    /local/workspace/hailo_model_zoo/hailo_models/custom_models/
  176  # Certifica-te que o output_test existe (já existe, mas não faz mal repetir)
  177  mkdir -p /local/workspace/hailo_model_zoo/hailo_models/custom_models/output_test
  178  # Verificar
  179  ls /local/workspace/hailo_model_zoo/hailo_models/custom_models/
  180  ls
  181  cd..
  182  ls
  183  cd ..
  184  ls
  185  cd /local/workspace/hailo_model_zoo/hailo_models/custom_models
  186  hailo parser onnx yolov8n_simplified.onnx
  187  ls
  188  cd ..
  189  cd shared_with_docker/
  190  ls
  191  cd scripts/
  192  ls
  193  cd ..
  194  python3 run_onnx_cpu.py 
  195  ls
  196  cd outputs/
  197  ls
  198  cd ..
  199  ls
  200  hailomz compile yolov8n   --ckpt /app/workspace/yolov8n.onnx   --hw-arch hailo8   --calib-path /app/workspace/calib   --classes 80
  201  exit
  202  ls -lh /local/workspace/shared/yolov8n_seg.hef
  203  clear
  204  ls
  205  cd shared_with_docker/
  206  ls
  207  cd ..
  208  clear
  209  ls
  210  cd shared_with_docker/
  211  ls
  212  exit
  213  # dentro do container (atenção ao caminho correto: shared_with_docker)
  214  mkdir -p /local/workspace/shared_with_docker/logs
  215  hailomz compile yolov8n_seg   --hw-arch hailo8l   --calib-path /local/workspace/hailo_model_zoo/hailo_models/custom_models/calibration_images   2>&1 | tee /local/workspace/shared_with_docker/logs/compile_yolov8n_seg_fallback.log
  216  ls -lh /local/workspace/shared_with_docker/yolov8n_seg.hef
  217  ls -lh /local/workspace/shared_with_docker/yolov8n_seg.har
  218  exit
  219  mkdir -p /local/workspace/shared_with_docker/logs
  220  hailomz compile yolov8n_seg   --ckpt /local/workspace/shared_with_docker/yolo26n-seg.onnx   --hw-arch hailo8l   --calib-path /local/workspace/shared_with_docker/calibration_images   2>&1 | tee /local/workspace/shared_with_docker/logs/compile_yolo26n_seg.log
  221  clear
  222  exit
  223  hailomz compile -h | grep -Ei "start|end|node"
  224  python3 - << 'PY'
  225  import onnx
  226  m = onnx.load('/local/workspace/shared_with_docker/yolo26n-seg.onnx')
  227  print("INPUTS:")
  228  for i in m.graph.input:
  229      print(" ", i.name)
  230  print("OUTPUTS:")
  231  for o in m.graph.output:
  232      print(" ", o.name)
  233  PY
  234  hailomz compile yolov8n_seg   --ckpt /local/workspace/shared_with_docker/yolo26n-seg.onnx   --hw-arch hailo8l   --calib-path /local/workspace/shared_with_docker/calibration_images   --start-node-names <INPUT_REAL>   --end-node-names <OUT1> <OUT2> <OUT3> <OUT4> \
  235  hailomz compile yolov8n_seg   --ckpt /local/workspace/shared_with_docker/yolo26n-seg.onnx   --hw-arch hailo8l   --calib-path /local/workspace/shared_with_docker/calibration_images   --start-node-names <INPUT_REAL>   --end-node-names <OUT1> <OUT2> <OUT3> <OUT4> \
  236  clear
  237  hailomz compile -h | grep -Ei "start|end|node"
  238  python3 - << 'PY'
  239  import onnx
  240  m = onnx.load('/local/workspace/shared_with_docker/yolo26n-seg.onnx')
  241  print("INPUTS:")
  242  for i in m.graph.input:
  243      print(" ", i.name)
  244  print("OUTPUTS:")
  245  for o in m.graph.output:
  246      print(" ", o.name)
  247  PY
  248  hailomz compile yolov8n_seg   --ckpt /local/workspace/shared_with_docker/yolo26n-seg.onnx   --hw-arch hailo8l   --calib-path /local/workspace/shared_with_docker/calibration_images   --start-node-names images   --end-node-names output0 output1   2>&1 | tee /local/workspace/shared_with_docker/logs/compile_yolo26n_seg_try2.log
  249  clear
  250  exit
  251  python3 - << 'PY'
  252  import onnx
  253  m=onnx.load('/local/workspace/shared_with_docker/yolo26n-seg_512.onnx')
  254  print([i.name for i in m.graph.input])
  255  print([o.name for o in m.graph.output])
  256  PY
  257  hailomz compile yolov8n_seg   --ckpt /local/workspace/shared_with_docker/yolo26n-seg_512.onnx   --hw-arch hailo8l   --calib-path /local/workspace/shared_with_docker/calibration_images   --start-node-names images   --end-node-names output0 output1   2>&1 | tee /local/workspace/shared_with_docker/logs/compile_yolo26n_seg_512.log
  258  # no alvo (AGL/RPi)
  259  hailortcli scan
  260  exit
  261  hailomz compile yolov8n   --ckpt /local/workspace/shared/yolov8n.onnx   --hw-arch hailo8   --calib-path /local/workspace/shared/calibration_images   2>&1 | tee /local/workspace/shared/logs/compile_yolov8n_h8.log
  262  clear
  263  mkdir -p /local/workspace/shared/logs
  264  ls -lah /local/workspace/shared
  265  find /local/workspace -type f -name "*yolov8n*.onnx" 2>/dev/null
  266  clear
  267  mkdir -p /local/workspace/shared_with_docker/logs
  268  hailomz compile yolov8n   --ckpt /local/workspace/shared_with_docker/yolov8n.onnx   --hw-arch hailo8   --calib-path /local/workspace/shared_with_docker/calibration_images   2>&1 | tee /local/workspace/shared_with_docker/logs/compile_yolov8n_h8.log
  269  ls -lah /local/workspace/*.hef
  270  ls -lah /local/workspace/shared_with_docker/logs/compile_yolov8n_h8.log
  271  ls -lah /local/workspace/shared_with_docker
  272  cler
  273  clear
  274  mkdir -p /local/workspace/shared_with_docker/logs /local/workspace/shared_with_docker/hef
  275  hailomz compile yolov8n   --hw-arch hailo8   --calib-path /local/workspace/shared_with_docker/calibration_images   2>&1 | tee /local/workspace/shared_with_docker/logs/compile_yolov8n_h8_recipe.log
  276  cp -f /local/workspace/yolov8n.hef /local/workspace/shared_with_docker/hef/yolov8n_h8.hef
  277  hailomz compile yolov8n_seg   --hw-arch hailo8   --calib-path /local/workspace/shared_with_docker/calibration_images   2>&1 | tee /local/workspace/shared_with_docker/logs/compile_yolov8n_seg_h8_recipe.log
  278  cp -f /local/workspace/yolov8n_seg.hef /local/workspace/shared_with_docker/hef/yolov8n_seg_h8.hef
  279  hailomz compile yolov8n_seg   --ckpt /local/workspace/shared_with_docker/yolo26n-seg_512.onnx   --hw-arch hailo8   --calib-path /local/workspace/shared_with_docker/calibration_images   --start-node-names images   --end-node-names output0 output1   2>&1 | tee /local/workspace/shared_with_docker/logs/compile_yolo26n_seg512_h8.log
  280  clear
  281  cd /local/workspace/shared_with_docker
  282  mkdir -p logs hef
  283  python - <<'PY'
from ultralytics import YOLO
m = YOLO("yolo26n-seg.pt")
m.export(format="onnx", imgsz=384, dynamic=False, simplify=True, opset=13)
m.export(format="onnx", imgsz=320, dynamic=False, simplify=True, opset=13)
PY

  284  ls -lah *.onnx | grep -E "yolo26n-seg|yolo26n_seg|384|320"
  285  clear
  286  python -m pip install -U pip wheel setuptools
  287  python -m pip install ultralytics onnx onnxsim
  288  python - <<'PY'
from ultralytics import YOLO
m = YOLO("/local/workspace/shared_with_docker/yolo26n-seg.pt")
m.export(format="onnx", imgsz=384, dynamic=False, simplify=True, opset=13, name="yolo26n-seg_384")
m.export(format="onnx", imgsz=320, dynamic=False, simplify=True, opset=13, name="yolo26n-seg_320")
PY

  289  ls -lah /local/workspace/shared_with_docker/*384*.onnx /local/workspace/shared_with_docker/*320*.onnx
  290  clear
  291  cd /local/workspace/shared_with_docker
  292  python - <<'PY'
from ultralytics import YOLO
m = YOLO("/local/workspace/shared_with_docker/yolo26n-seg.pt")
m.export(format="onnx", imgsz=384, dynamic=False, simplify=True, opset=13)
PY

  293  cp -f /local/workspace/shared_with_docker/yolo26n-seg.onnx /local/workspace/shared_with_docker/yolo26n-seg_384.onnx
  294  python - <<'PY'
from ultralytics import YOLO
m = YOLO("/local/workspace/shared_with_docker/yolo26n-seg.pt")
m.export(format="onnx", imgsz=320, dynamic=False, simplify=True, opset=13)
PY

  295  cp -f /local/workspace/shared_with_docker/yolo26n-seg.onnx /local/workspace/shared_with_docker/yolo26n-seg_320.onnx
  296  ls -lah /local/workspace/shared_with_docker/yolo26n-seg_384.onnx /local/workspace/shared_with_docker/yolo26n-seg_320.onnx
  297  hailomz compile yolov8n_seg   --ckpt /local/workspace/shared_with_docker/yolo26n-seg_384.onnx   --hw-arch hailo8   --calib-path /local/workspace/shared_with_docker/calibration_images   --start-node-names images   --end-node-names output0 output1   2>&1 | tee /local/workspace/shared_with_docker/logs/compile_yolo26n_seg_384_h8.log
  298  tail -n 120 /local/workspace/shared_with_docker/logs/compile_yolo26n_seg_384_h8.log
  299  # ou _320_h8.log, conforme o que estava a correr
  300  clear
  301  hailomz compile -h | grep -Ei "model-script|alls|script"
  302  cp /local/workspace/hailo_model_zoo/hailo_model_zoo/cfg/alls/generic/yolov8n_seg.alls    /local/workspace/shared_with_docker/yolov8n_seg_no_nms.alls
  303  grep -v "nms_postprocess" /local/workspace/shared_with_docker/yolov8n_seg_no_nms.alls   > /local/workspace/shared_with_docker/yolov8n_seg_no_nms.tmp && mv /local/workspace/shared_with_docker/yolov8n_seg_no_nms.tmp /local/workspace/shared_with_docker/yolov8n_seg_no_nms.alls
  304  hailomz compile yolov8n_seg   --ckpt /local/workspace/shared_with_docker/yolo26n-seg_320.onnx   --hw-arch hailo8   --calib-path /local/workspace/shared_with_docker/calibration_images   --start-node-names images   --end-node-names output0 output1   --model-script /local/workspace/shared_with_docker/yolov8n_seg_no_nms.alls   2>&1 | tee /local/workspace/shared_with_docker/logs/compile_yolo26n_seg_320_h8_no_nms.log
  305  clear
  306  root@d61610b30f6f:/local/workspace/shared_with_docker# hailomz compile -h | grep -Ei "model-script|alls|script"
  307  root@d61610b30f6f:/local/workspace/shared_with_docker# cp /local/workspace/hailo_model_zoo/hailo_model_zoo/cfg/alls/generic/yolov8n_seg.alls    /local/workspace/shared_with_docker/yolov8n_seg_no_nms.alls
  308  grep -v "nms_postprocess" /local/workspace/shared_with_docker/yolov8n_seg_no_nms.alls   > /local/workspace/shared_with_docker/yolov8n_seg_no_nms.tmp && mv /local/workspace/shared_with_docker/yolov8n_seg_no_nms.tmp /local/workspace/shared_with_docker/yolov8n_seg_no_nms.alls
  309  root@d61610b30f6f:/local/workspace/shared_with_docker# hailomz compile yolov8n_seg   --ckpt /local/workspace/shared_with_docker/yolo26n-seg_320.onnx   --hw-arch hailo8   --calib-path /local/workspace/shared_with_docker/calibration_images   --start-node-names images   --end-node-names output0 output1   --model-script /local/workspace/shared_with_docker/yolov8n_seg_no_nms.alls   2>&1 | tee /local/workspace/shared_with_docker/logs/compile_yolo26n_seg_320_h8_no_nms.log
  310  [info] No GPU chosen and no suitable GPU found, falling back to CPU.
  311  <Hailo Model Zoo INFO> Start run for network yolov8n_seg ...
  312  <Hailo Model Zoo INFO> Initializing the hailo8 runner...
  313  [info] Translation started on ONNX model yolov8n_seg
  314  [info] Restored ONNX model yolov8n_seg (completion time: 00:00:00.05)
  315  [info] Extracted ONNXRuntime meta-data for Hailo model (completion time: 00:00:00.18)
  316  [info] Simplified ONNX model for a parsing retry attempt (completion time: 00:00:00.95)
  317  [info] According to recommendations, retrying parsing with end node names: ['/model.23/proto/cv3/act/Mul', '/model.23/Transpose'].
  318  [info] Translation started on ONNX model yolov8n_seg
  319  [info] Restored ONNX model yolov8n_seg (completion time: 00:00:00.03)
  320  [info] Extracted ONNXRuntime meta-data for Hailo model (completion time: 00:00:00.17)
  321  [info] Start nodes mapped from original model: 'images': 'yolov8n_seg/input_layer1'.
  322  [info] End nodes mapped from original model: '/model.23/proto/cv3/act/Mul', '/model.23/Transpose'.
  323  [info] Translation completed on ONNX model yolov8n_seg (completion time: 00:00:01.19)
  324  [info] Saved HAR to: /local/workspace/shared_with_docker/yolov8n_seg.har
  325  <Hailo Model Zoo INFO> Preparing calibration data...
  326  [info] Loading model script commands to yolov8n_seg from /local/workspace/shared_with_docker/yolov8n_seg_no_nms.alls
  327  [info] Found model with 3 input channels, using real RGB images for calibration instead of sampling random data.
  328  [info] Starting Model Optimization
  329  [warning] Reducing optimization level to 0 (the accuracy won't be optimized and compression won't be used) because there's less data than the recommended amount (1024), and there's no available GPU
  330  [warning] Running model optimization with zero level of optimization is not recommended for production use and might lead to suboptimal accuracy results
  331  [info] Model received quantization params from the hn
  332  [info] MatmulDecompose skipped
  333  [info] Starting Mixed Precision
  334  [info] Model Optimization Algorithm Mixed Precision is done (completion time is 00:00:00.82)
  335  [info] LayerNorm Decomposition skipped
  336  [info] Starting Statistics Collector
  337  [info] Using dataset with 64 entries for calibration
  338  Calibration: 100%|██████████| 64/64 [00:33<00:00,  1.93entries/s]
  339  [info] Model Optimization Algorithm Statistics Collector is done (completion time is 00:00:34.29)
  340  [info] Starting Fix zp_comp Encoding
  341  [info] Model Optimization Algorithm Fix zp_comp Encoding is done (completion time is 00:00:00.00)
  342  [info] Starting Matmul Equalization
  343  [info] Model Optimization Algorithm Matmul Equalization is done (completion time is 00:00:00.06)
  344  [info] Starting MatmulDecomposeFix
  345  [info] Model Optimization Algorithm MatmulDecomposeFix is done (completion time is 00:00:00.00)
  346  [info] activation fitting started for yolov8n_seg/reduce_sum_softmax1/act_op
  347  [info] activation fitting started for yolov8n_seg/reduce_sum_softmax2/act_op
  348  [info] Finetune encoding skipped
  349  [info] Bias Correction skipped
  350  [info] Adaround skipped
  351  [info] Quantization-Aware Fine-Tuning skipped
  352  [info] Layer Noise Analysis skipped
  353  [info] Model Optimization is done
  354  [info] Saved HAR to: /local/workspace/shared_with_docker/yolov8n_seg.har
  355  [info] Loading model script commands to yolov8n_seg from /local/workspace/shared_with_docker/yolov8n_seg_no_nms.alls
  356  [info] To achieve optimal performance, set the compiler_optimization_level to "max" by adding performance_param(compiler_optimization_level=max) to the model script. Note that this may increase compilation time.
  357  [info] Loading network parameters
  358  [info] Starting Hailo allocation and compilation flow
  359  [info] Building optimization options for network layers...
  360  [info] Successfully built optimization options - 5s 60ms
  361  [info] Trying to compile the network in a single context
  362  [info] Single context flow failed: Recoverable single context error
  363  [info] Building optimization options for network layers...
  364  [info] Successfully built optimization options - 6s 568ms
  365  [info] Using Multi-context flow
  366  [info] Resources optimization params: max_control_utilization=60%, max_compute_utilization=60%, max_compute_16bit_utilization=60%, max_memory_utilization (weights)=60%, max_input_aligner_utilization=60%, max_apu_utilization=60%
  367  [info] Finding the best partition to contexts...
  368  [info] Found valid partition to 3 contexts
  369  [info] Iteration #4 - 3 contexts,
  370  [info] Searching for a better partition...
  371  [info] Found valid partition to 4 contexts, Performance improved by 0.8%
  372  [info] Iteration #30 - 4 contexts,
  373  [info] Searching for a better partition...
  374  [info] Found valid partition to 4 contexts, Performance improved by 2.4%
  375  [info] Iteration #37 - 4 contexts,
  376  [info] Searching for a better partition...
  377  [info] Found valid partition to 4 contexts, Performance improved by 0.2%
  378  [info] Iteration #40 - 4 contexts,
  379  [info] Searching for a better partition...
  380  [info] Found valid partition to 4 contexts, Performance improved by 0.7%
  381  [info] Iteration #42 - 4 contexts,
  382  [info] Searching for a better partition...
  383  [info] Found valid partition to 4 contexts, Performance improved by 6.1%
  384  [info] Iteration #47 - 4 contexts,
  385  [info] Searching for a better partition...
  386  [info] Found valid partition to 4 contexts, Performance improved by 0.3%
  387  [info] Iteration #48 - 4 contexts,
  388  [info] Searching for a better partition...
  389  [info] Found valid partition to 4 contexts, Performance improved by 0.2%
  390  [info] Iteration #52 - 4 contexts,
  391  [info] Searching for a better partition...
  392  [info] Found valid partition to 4 contexts, Performance improved by 4.7%
  393  [info] Iteration #54 - 4 contexts,
  394  [info] Searching for a better partition...
  395  [info] Found valid partition to 4 contexts, Performance improved by 1.1%
  396  [info] Iteration #59 - 4 contexts,
  397  [info] Searching for a better partition...
  398  [info] Found valid partition to 4 contexts, Performance improved by 3.3%
  399  [info] Iteration #61 - 4 contexts,
  400  [info] Searching for a better partition...
  401  [<==>....................................] 
  402  [info] Partition to contexts finished successfully
  403  [info] Partitioner finished after 201 iterations, Time it took: 7m 0s 808ms
  404  [info] Applying selected partition to 4 contexts...
  405  [info] Validating layers feasibility
  406  [info] input_layer1: Pass
  407  [info] conv2_sd2: Pass
  408  [info] conv2_sd0: Pass
  409  [info] conv2_ws: Pass
  410  [info] conv_feature_splitter1_1: Pass
  411  [info] conv2_sdc: Pass
  412  [info] conv1_ws: Pass
  413  [info] conv_feature_splitter2_1: Pass
  414  [info] conv1_sd0: Pass
  415  [info] conv5: Pass
  416  [info] ew_add1: Pass
  417  [info] conv4: Pass
  418  [info] conv2_sd3: Pass
  419  [info] normalization1: Pass
  420  [info] conv9: Pass
  421  [info] conv1_sd2: Pass
  422  [info] conv_feature_splitter2_2: Pass
  423  [info] conv1_sdc: Pass
  424  [info] concat1: Pass
  425  [info] conv2_sd1: Pass
  426  [info] conv_feature_splitter1_2: Pass
  427  [info] conv1_sd1: Pass
  428  [info] conv7_sd1: Pass
  429  [info] conv7_sdc: Pass
  430  [info] conv10: Pass
  431  [info] conv7_sd2: Pass
  432  [info] concat2: Pass
  433  [info] conv6: Pass
  434  [info] conv7_sd0: Pass
  435  [info] conv7_ws: Pass
  436  [info] ew_add2: Pass
  437  [info] conv11: Pass
  438  [info] conv12_sd2: Pass
  439  [info] conv12_sd0: Pass
  440  [info] conv12_sd1: Pass
  441  [info] conv12_sdc: Pass
  442  [info] conv_feature_splitter3_2: Pass
  443  [info] conv12_ws: Pass
  444  [info] conv_feature_splitter3_1: Pass
  445  [info] conv14: Pass
  446  [info] conv16: Pass
  447  [info] conv17: Pass
  448  [info] conv15: Pass
  449  [info] ew_add3: Pass
  450  [info] conv18: Pass
  451  [info] conv21: Pass
  452  [info] conv19: Pass
  453  [info] conv_feature_splitter4_1: Pass
  454  [info] concat3: Pass
  455  [info] conv20: Pass
  456  [info] ew_add4: Pass
  457  [info] concat4: Pass
  458  [info] conv24: Pass
  459  [info] conv_feature_splitter4_2: Pass
  460  [info] conv25: Pass
  461  [info] concat6: Pass
  462  [info] conv27: Pass
  463  [info] conv26: Pass
  464  [info] conv30: Pass
  465  [info] conv22: Pass
  466  [info] conv32: Pass
  467  [info] concat5: Pass
  468  [info] maxpool2: Pass
  469  [info] ew_add6: Pass
  470  [info] yolov8n_seg_yolov8n_seg_context_0_to_yolov8n_seg_context_1_2757: Pass
  471  [info] conv28: Pass
  472  [info] conv29: Pass
  473  [info] conv31: Pass
  474  [info] yolov8n_seg_yolov8n_seg_context_0_to_yolov8n_seg_context_1_2759: Pass
  475  [info] yolov8n_seg_yolov8n_seg_context_0_to_yolov8n_seg_context_1_2761: Pass
  476  [info] yolov8n_seg_yolov8n_seg_context_0_to_yolov8n_seg_context_1_2767: Pass
  477  [info] yolov8n_seg_yolov8n_seg_context_0_to_yolov8n_seg_context_1_2763: Pass
  478  [info] yolov8n_seg_yolov8n_seg_context_0_to_yolov8n_seg_context_1_2769: Pass
  479  [info] maxpool1: Pass
  480  [info] ew_add5: Pass
  481  [info] conv_feature_splitter6_1: Pass
  482  [info] auto_reshape_from_input_layer1_to_normalization1: Pass
  483  [info] conv33: Pass
  484  [info] conv_feature_splitter6_2: Pass
  485  [info] maxpool3: Pass
  486  [info] conv_feature_splitter6_3: Pass
  487  [info] conv_feature_splitter5_2: Pass
  488  [info] conv_feature_splitter5_1: Pass
  489  [info] ew_add7: Pass
  490  [info] concat7: Pass
  491  [info] mux_conv_feature_splitter5_2_conv_feature_splitter6_1_to_conv36_matmul1: Pass
  492  [info] conv39: Pass
  493  [info] concat9: Pass
  494  [info] matmul2_reshape: Pass
  495  [info] dw1_defuse_reshape_hxf_to_w_transposed: Pass
  496  [info] conv37: Pass
  497  [info] conv38: Pass
  498  [info] concat8: Pass
  499  [info] dw1_defuse_width_feature_reshape: Pass
  500  [info] conv43: Pass
  501  [info] conv42: Pass
  502  [info] conv44: Pass
  503  [info] concat10: Pass
  504  [info] conv46: Pass
  505  [info] ew_mult_softmax1: Pass
  506  [info] conv_feature_splitter7_1: Pass
  507  [info] conv41: Pass
  508  [info] matmul2: Pass
  509  [info] matmul1: Pass
  510  [info] activation1: Pass
  511  [info] conv45: Pass
  512  [info] conv48: Pass
  513  [info] ew_add11: Pass
  514  [info] reduce_sum_softmax1: Pass
  515  [info] ew_sub_softmax1: Pass
  516  [info] reduce_max_softmax1: Pass
  517  [info] conv_feature_splitter7_2: Pass
  518  [info] resize1: Pass
  519  [info] ew_add12: Pass
  520  [info] concat14: Pass
  521  [info] concat11: Pass
  522  [info] conv36: Pass
  523  [info] resize2: Pass
  524  [info] conv50: Pass
  525  [info] concat13: Pass
  526  [info] conv57: Pass
  527  [info] dw1_defuse_1x1: Pass
  528  [info] ew_add13: Pass
  529  [info] conv51: Pass
  530  [info] conv63: Pass
  531  [info] conv47: Pass
  532  [info] conv55: Pass
  533  [info] conv61: Pass
  534  [info] conv64: Pass
  535  [info] conv53: Pass
  536  [info] conv52: Pass
  537  [info] ew_add14: Pass
  538  [info] conv_feature_splitter8_2: Pass
  539  [info] conv_feature_splitter9_2: Pass
  540  [info] conv54: Pass
  541  [info] conv56: Pass
  542  [info] conv58: Pass
  543  [info] concat15: Pass
  544  [info] conv60: Pass
  545  [info] sh_from_conv57_to_conv58-60_dw2_1: Pass
  546  [info] conv59: Pass
  547  [info] conv_feature_splitter8_1: Pass
  548  [info] conv62: Pass
  549  [info] concat12: Pass
  550  [info] dw2_sd0: Pass
  551  [info] conv71: Pass
  552  [info] conv72: Pass
  553  [info] conv_feature_splitter9_1: Pass
  554  [info] conv69: Pass
  555  [info] conv70: Pass
  556  [info] dw2_sdc: Pass
  557  [info] dw3_d0: Pass
  558  [info] conv67: Pass
  559  [info] conv66: Pass
  560  [info] yolov8n_seg_yolov8n_seg_context_0_to_yolov8n_seg_context_1_2760: Pass
  561  [info] yolov8n_seg_yolov8n_seg_context_1_to_yolov8n_seg_context_2_2774: Pass
  562  [info] dw2_sd1: Pass
  563  [info] dw3_d1: Pass
  564  [info] format_conversion5: Pass
  565  [info] dw3_fs: Pass
  566  [info] yolov8n_seg_yolov8n_seg_context_1_to_yolov8n_seg_context_2_2776: Pass
  567  [info] yolov8n_seg_yolov8n_seg_context_1_to_yolov8n_seg_context_2_2772: Pass
  568  [info] conv65: Pass
  569  [info] dw3_dc: Pass
  570  [info] yolov8n_seg_yolov8n_seg_context_1_to_yolov8n_seg_context_2_2780: Pass
  571  [info] yolov8n_seg_yolov8n_seg_context_1_to_yolov8n_seg_context_2_2778: Pass
  572  [info] yolov8n_seg_yolov8n_seg_context_1_to_yolov8n_seg_context_2_2784: Pass
  573  [info] yolov8n_seg_yolov8n_seg_context_1_to_yolov8n_seg_context_2_2782: Pass
  574  [info] yolov8n_seg_yolov8n_seg_context_0_to_yolov8n_seg_context_1_2764: Pass
  575  [info] yolov8n_seg_yolov8n_seg_context_1_to_yolov8n_seg_context_3_2786: Pass
  576  [info] format_conversion4: Pass
  577  [info] yolov8n_seg_yolov8n_seg_context_1_to_yolov8n_seg_context_3_2788: Pass
  578  [info] yolov8n_seg_yolov8n_seg_context_1_to_yolov8n_seg_context_3_2790: Pass
  579  [info] yolov8n_seg_yolov8n_seg_context_0_to_yolov8n_seg_context_1_2768: Pass
  580  [info] yolov8n_seg_yolov8n_seg_context_0_to_yolov8n_seg_context_1_2758: Pass
  581  [info] yolov8n_seg_yolov8n_seg_context_0_to_yolov8n_seg_context_1_2762: Pass
  582  [info] yolov8n_seg_yolov8n_seg_context_0_to_yolov8n_seg_context_1_2770: Pass
  583  [info] format_conversion6: Pass
  584  [info] demux_conv_feature_splitter5_2_conv_feature_splitter6_1_to_conv36_matmul1: Pass
  585  [info] sh_from_sh_from_conv57_to_conv58-60_dw2_1_to_dw2_sd0-1_2806: Pass
  586  [info] sh_from_conv76_to_conv77-80_2: Pass
  587  [info] conv80: Pass
  588  [info] concat18: Pass
  589  [info] conv81: Pass
  590  [info] conv82: Pass
  591  [info] concat16: Pass
  592  [info] conv77: Pass
  593  [info] conv75: Pass
  594  [info] ew_add16: Pass
  595  [info] conv74: Pass
  596  [info] conv89: Pass
  597  [info] dw4_sdc: Pass
  598  [info] conv90: Pass
  599  [info] dw4_sd0: Pass
  600  [info] conv76: Pass
  601  [info] conv84: Pass
  602  [info] ew_add15: Pass
  603  [info] conv86: Pass
  604  [info] conv83: Pass
  605  [info] concat17: Pass
  606  [info] conv_feature_splitter11_1: Pass
  607  [info] dw4_sd1: Pass
  608  [info] conv73: Pass
  609  [info] format_conversion7_defuse_width_feature_reshape: Pass
  610  [info] conv78: Pass
  611  [info] dw5: Pass
  612  [info] conv_feature_splitter10_1: Pass
  613  [info] ew_add18: Pass
  614  [info] conv85: Pass
  615  [info] conv_feature_splitter11_2: Pass
  616  [info] conv_feature_splitter10_2: Pass
  617  [info] conv87: Pass
  618  [info] format_conversion8_defuse_width_feature_reshape: Pass
  619  [info] format_conversion9_defuse_width_feature_reshape: Pass
  620  [info] conv79: Pass
  621  [info] format_conversion7_defuse_reshape_hxf_to_w_transposed: Pass
  622  [info] format_conversion9_defuse_reshape_hxf_to_w_transposed: Pass
  623  [info] format_conversion8_defuse_reshape_hxf_to_w_transposed: Pass
  624  [info] activation2: Pass
  625  [info] reduce_sum_softmax2: Pass
  626  [info] ew_mult_softmax2: Pass
  627  [info] reduce_max_softmax2: Pass
  628  [info] matmul4_reshape: Pass
  629  [info] ew_sub_softmax2: Pass
  630  [info] matmul4: Pass
  631  [info] matmul3: Pass
  632  [info] dw6_defuse_width_feature_reshape: Pass
  633  [info] conv93: Pass
  634  [info] conv94: Pass
  635  [info] dw6_defuse_reshape_hxf_to_w_transposed: Pass
  636  [info] conv_feature_splitter11_3: Pass
  637  [info] dw6_defuse_1x1: Pass
  638  [info] concat19: Pass
  639  [info] dw7: Pass
  640  [info] conv92: Pass
  641  [info] conv96: Pass
  642  [info] conv102: Pass
  643  [info] conv100: Pass
  644  [info] dw8: Pass
  645  [info] conv95: Pass
  646  [info] format_conversion13_defuse_reshape_hxf_to_w_transposed: Pass
  647  [info] conv104: Pass
  648  [info] conv98: Pass
  649  [info] conv101: Pass
  650  [info] format_conversion13_defuse_width_feature_reshape: Pass
  651  [info] conv103: Pass
  652  [info] conv97: Pass
  653  [info] resize4_transposed_input: Pass
  654  [info] conv99: Pass
  655  [info] yolov8n_seg_yolov8n_seg_context_2_to_yolov8n_seg_context_3_2792: Pass
  656  [info] conv105: Pass
  657  [info] yolov8n_seg_yolov8n_seg_context_2_to_yolov8n_seg_context_3_2794: Pass
  658  [info] yolov8n_seg_yolov8n_seg_context_1_to_yolov8n_seg_context_2_2777: Pass
  659  [info] format_conversion14_defuse_width_feature_reshape: Pass
  660  [info] resize3: Pass
  661  [info] yolov8n_seg_yolov8n_seg_context_1_to_yolov8n_seg_context_2_2779: Pass
  662  [info] yolov8n_seg_yolov8n_seg_context_1_to_yolov8n_seg_context_2_2785: Pass
  663  [info] yolov8n_seg_yolov8n_seg_context_2_to_yolov8n_seg_context_3_2796: Pass
  664  [info] yolov8n_seg_yolov8n_seg_context_1_to_yolov8n_seg_context_2_2781: Pass
  665  [info] format_conversion14_defuse_reshape_hxf_to_w_transposed: Pass
  666  [info] yolov8n_seg_yolov8n_seg_context_2_to_yolov8n_seg_context_3_2798: Pass
  667  [info] yolov8n_seg_yolov8n_seg_context_1_to_yolov8n_seg_context_2_2773: Pass
  668  [info] yolov8n_seg_yolov8n_seg_context_2_to_yolov8n_seg_context_3_2800: Pass
  669  [info] yolov8n_seg_yolov8n_seg_context_2_to_yolov8n_seg_context_3_2802: Pass
  670  [info] yolov8n_seg_yolov8n_seg_context_2_to_yolov8n_seg_context_3_2804: Pass
  671  [info] yolov8n_seg_yolov8n_seg_context_1_to_yolov8n_seg_context_2_2775: Pass
  672  [info] resize4_transposed: Pass
  673  [info] ew_add17: Pass
  674  [info] yolov8n_seg_yolov8n_seg_context_1_to_yolov8n_seg_context_2_2783: Pass
  675  [info] ew_add23: Pass
  676  [info] output_layer1: Pass
  677  [info] format_conversion15_defuse_width_feature_reshape: Pass
  678  [info] conv106_sdc: Pass
  679  [info] ew_mult1: Pass
  680  [info] ew_add22: Pass
  681  [info] output_layer2: Pass
  682  [info] conv106_ws: Pass
  683  [info] const_input1: Pass
  684  [info] ew_sub1: Pass
  685  [info] concat22: Pass
  686  [info] const_input2: Pass
  687  [info] const_input3: Pass
  688  [info] concat23: Pass
  689  [info] deconv1_defuse_fi0_d0: Pass
  690  [info] conv107_ws: Pass
  691  [info] concat21: Pass
  692  [info] feature_splitter12: Pass
  693  [info] deconv1_defuse_fi0_d1: Pass
  694  [info] conv106_sd1: Pass
  695  [info] conv107_sdc: Pass
  696  [info] deconv1_defuse_conv: Pass
  697  [info] activation3: Pass
  698  [info] format_conversion15_defuse_reshape_hxf_to_w_transposed: Pass
  699  [info] deconv1_defuse_fi0_dc: Pass
  700  [info] conv107_sd1: Pass
  701  [info] concat20: Pass
  702  [info] conv108_sd0: Pass
  703  [info] conv107_sd0: Pass
  704  [info] auto_reshape_from_format_conversion16_to_output_layer1: Pass
  705  [info] auto_reshape_from_conv109_to_output_layer2: Pass
  706  [info] conv106_sd0: Pass
  707  [info] concat_w_from_conv108_sd0-3_to_conv108_sdc: Pass
  708  [info] yolov8n_seg_yolov8n_seg_context_1_to_yolov8n_seg_context_3_2791: Pass
  709  [info] concat24: Pass
  710  [info] yolov8n_seg_yolov8n_seg_context_2_to_yolov8n_seg_context_3_2793: Pass
  711  [info] conv108_sdc: Pass
  712  [info] yolov8n_seg_yolov8n_seg_context_2_to_yolov8n_seg_context_3_2795: Pass
  713  [info] yolov8n_seg_yolov8n_seg_context_2_to_yolov8n_seg_context_3_2803: Pass
  714  [info] conv108_sd1: Pass
  715  [info] yolov8n_seg_yolov8n_seg_context_1_to_yolov8n_seg_context_3_2787: Pass
  716  [info] conv108_sd9: Pass
  717  [info] ws_from_conv108_ws_to_conv108_sd0-3_182: Pass
  718  [info] conv109: Pass
  719  [info] conv108_sd2: Pass
  720  [info] conv108_sd7: Pass
  721  [info] yolov8n_seg_yolov8n_seg_context_2_to_yolov8n_seg_context_3_2797: Pass
  722  [info] format_conversion16: Pass
  723  [info] ws_from_conv108_ws_to_conv108_sd4-7_191: Pass
  724  [info] conv108_sd8: Pass
  725  [info] conv108_sd3: Pass
  726  [info] yolov8n_seg_yolov8n_seg_context_2_to_yolov8n_seg_context_3_2801: Pass
  727  [info] conv108_sd6: Pass
  728  [info] yolov8n_seg_yolov8n_seg_context_1_to_yolov8n_seg_context_3_2789: Pass
  729  [info] concat_w_from_conv108_sd4-7_to_conv108_sdc: Pass
  730  [info] yolov8n_seg_yolov8n_seg_context_2_to_yolov8n_seg_context_3_2805: Pass
  731  [info] conv108_sd4: Pass
  732  [info] conv108_ws: Pass
  733  [info] yolov8n_seg_yolov8n_seg_context_2_to_yolov8n_seg_context_3_2799: Pass
  734  [info] conv108_sd5: Pass
  735  [info] Layers feasibility validated successfully
  736  [info] Running resources allocation (mapping) flow, time per context: 59m 59s
  737  [info] Context:0/0 Iteration 0: Mapping prepost...          
  738  [info] Context:0/3 Iteration 0: Trying parallel splits...   
  739  [info] Context:0/3 Iteration 4: Trying parallel mapping...  
  740  [info] Context:1/3 Iteration 0: Trying parallel splits...   
  741  [info] Context:1/3 Iteration 4: Trying parallel mapping...  
  742  [info] Context:2/3 Iteration 0: Trying parallel splits...   
  743  [info] Context:2/3 Iteration 4: Trying parallel mapping...  
  744  [info] Context:3/3 Iteration 0: Trying parallel splits...   
  745  [info] Context:3/3 Iteration 4: Trying parallel mapping...  
  746  [info] yolov8n_seg_context_0 (yolov8n_seg_context_0):
  747  Iterations: 4
  748  Reverts on cluster mapping: 0
  749  Reverts on inter-cluster connectivity: 1
  750  Reverts on pre-mapping validation: 0
  751  Reverts on split failed: 0
  752  [info] yolov8n_seg_context_1 (yolov8n_seg_context_1):
  753  Iterations: 4
  754  Reverts on cluster mapping: 0
  755  Reverts on inter-cluster connectivity: 0
  756  Reverts on pre-mapping validation: 0
  757  Reverts on split failed: 0
  758  [info] yolov8n_seg_context_2 (yolov8n_seg_context_2):
  759  Iterations: 4
  760  Reverts on cluster mapping: 0
  761  Reverts on inter-cluster connectivity: 0
  762  Reverts on pre-mapping validation: 0
  763  Reverts on split failed: 0
  764  [info] yolov8n_seg_context_3 (yolov8n_seg_context_3):
  765  Iterations: 4
  766  Reverts on cluster mapping: 0
  767  Reverts on inter-cluster connectivity: 0
  768  Reverts on pre-mapping validation: 0
  769  Reverts on split failed: 0
  770  [info] yolov8n_seg_context_0 utilization: 
  771  [info] +-----------+---------------------+---------------------+--------------------+
  772  [info] | Cluster   | Control Utilization | Compute Utilization | Memory Utilization | [info] +-----------+---------------------+---------------------+--------------------+
  773  [info] | cluster_0 | 56.3%               | 23.4%               | 23.4%              | [info] | cluster_1 | 87.5%               | 34.4%               | 43.8%              | [info] | cluster_2 | 37.5%               | 31.3%               | 27.3%              | [info] | cluster_3 | 31.3%               | 18.8%               | 18%                | [info] | cluster_4 | 87.5%               | 37.5%               | 41.4%              | [info] | cluster_5 | 81.3%               | 31.3%               | 34.4%              | [info] | cluster_6 | 18.8%               | 9.4%                | 10.9%              | [info] | cluster_7 | 87.5%               | 46.9%               | 42.2%              | [info] +-----------+---------------------+---------------------+--------------------+
  774  [info] | Total     | 60.9%               | 29.1%               | 30.2%              | [info] +-----------+---------------------+---------------------+--------------------+
  775  [info] yolov8n_seg_context_1 utilization: 
  776  [info] +-----------+---------------------+---------------------+--------------------+
  777  [info] | Cluster   | Control Utilization | Compute Utilization | Memory Utilization | [info] +-----------+---------------------+---------------------+--------------------+
  778  [info] | cluster_0 | 37.5%               | 10.9%               | 18.8%              | [info] | cluster_1 | 62.5%               | 17.2%               | 21.1%              | [info] | cluster_2 | 81.3%               | 26.6%               | 35.2%              | [info] | cluster_3 | 31.3%               | 9.4%                | 19.5%              | [info] | cluster_4 | 56.3%               | 25%                 | 25%                | [info] | cluster_5 | 50%                 | 18.8%               | 21.9%              | [info] | cluster_6 | 68.8%               | 17.2%               | 30.5%              | [info] | cluster_7 | 100%                | 50%                 | 35.9%              | [info] +-----------+---------------------+---------------------+--------------------+
  779  [info] | Total     | 60.9%               | 21.9%               | 26%                | [info] +-----------+---------------------+---------------------+--------------------+
  780  [info] yolov8n_seg_context_2 utilization: 
  781  [info] +-----------+---------------------+---------------------+--------------------+
  782  [info] | Cluster   | Control Utilization | Compute Utilization | Memory Utilization | [info] +-----------+---------------------+---------------------+--------------------+
  783  [info] | cluster_0 | 81.3%               | 31.3%               | 28.1%              | [info] | cluster_1 | 75%                 | 28.1%               | 24.2%              | [info] | cluster_2 | 81.3%               | 28.1%               | 36.7%              | [info] | cluster_3 | 56.3%               | 14.1%               | 17.2%              | [info] | cluster_4 | 81.3%               | 23.4%               | 32%                | [info] | cluster_6 | 100%                | 57.8%               | 46.9%              | [info] +-----------+---------------------+---------------------+--------------------+
  784  [info] | Total     | 59.4%               | 22.9%               | 23.1%              | [info] +-----------+---------------------+---------------------+--------------------+
  785  [info] yolov8n_seg_context_3 utilization: 
  786  [info] +-----------+---------------------+---------------------+--------------------+
  787  [info] | Cluster   | Control Utilization | Compute Utilization | Memory Utilization | [info] +-----------+---------------------+---------------------+--------------------+
  788  [info] | cluster_0 | 50%                 | 60.9%               | 43.8%              | [info] | cluster_2 | 43.8%               | 54.7%               | 43%                | [info] | cluster_3 | 6.3%                | 1.6%                | 3.1%               | [info] | cluster_4 | 93.8%               | 76.6%               | 63.3%              | [info] | cluster_5 | 25%                 | 6.3%                | 7.8%               | [info] | cluster_6 | 56.3%               | 35.9%               | 65.6%              | [info] | cluster_7 | 6.3%                | 1.6%                | 1.6%               | [info] +-----------+---------------------+---------------------+--------------------+
  789  [info] | Total     | 35.2%               | 29.7%               | 28.5%              | [info] +-----------+---------------------+---------------------+--------------------+
  790  [info] Successful Mapping (allocation time: 8m 6s)
  791  [info] Compiling kernels of yolov8n_seg_context_0...
  792  [info] Compiling kernels of yolov8n_seg_context_1...
  793  [info] Compiling kernels of yolov8n_seg_context_2...
  794  [info] Compiling kernels of yolov8n_seg_context_3...
  795  [info] Bandwidth of model inputs: 2.34375 Mbps, outputs: 3.42102 Mbps (for a single frame)
  796  [info] Bandwidth of DDR buffers: 0.0 Mbps (for a single frame)
  797  [info] Bandwidth of inter context tensors: 13.4662 Mbps (for a single frame)
  798  [info] Building HEF...
  799  [info] Successful Compilation (compilation time: 6s)
  800  [info] Saved HAR to: /local/workspace/shared_with_docker/yoloclear
  801  clear
  802  cp -f /local/workspace/shared_with_docker/yolov8n_seg.hef       /local/workspace/shared_with_docker/hef/yolo26n_seg_320_h8_no_nms.hef
  803  ls -lah /local/workspace/shared_with_docker/hef
  804  scp /local/workspace/shared_with_docker/hef/yolov8n_h8.hef root@sh root@10.21.220.191:/data/
  805  scp /local/workspace/shared_with_docker/hef/yolov8n_seg_h8.hef root@sh root@10.21.220.191:/data/
  806  scp /local/workspace/shared_with_docker/hef/yolo26n_seg_320_h8_no_nms.hef root@sh root@10.21.220.191 :/data/
  807  clear
  808  scp /local/workspace/shared_with_docker/hef/yolov8n_h8.hef root@10.21.220.191:/data/
  809  scp /local/workspace/shared_with_docker/hef/yolov8n_seg_h8.hef root@10.21.220.191:/data/
  810  scp /local/workspace/shared_with_docker/hef/yolo26n_seg_320_h8_no_nms.hef root@10.21.220.191:/data/
  811  history
root@d61610b30f6f:/local/workspace/shared_with_docker# 
