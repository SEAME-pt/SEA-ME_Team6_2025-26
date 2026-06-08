import numpy as np
import os
from PIL import Image
from hailo_sdk_client import ClientRunner

calib_dir = "calibration_images"
images = []
for fname in sorted(os.listdir(calib_dir)):
    if fname.lower().endswith(('.jpg', '.jpeg', '.png')):
        img = Image.open(os.path.join(calib_dir, fname)).convert("RGB")
        img = img.resize((640, 640))
        arr = np.array(img, dtype=np.float32) / 255.0
        images.append(arr)

calib_data = np.array(images, dtype=np.float32)
print(f"Calibration images loaded: {calib_data.shape}")

runner = ClientRunner(hw_arch="hailo8")
runner.load_har("best.har")
runner.optimize(calib_data)
runner.save_har("best_quantized.har")
print("Optimization done ✅")
