import onnx
import numpy as np

model = onnx.load('tusimple_res34_finetune_seame.onnx')

weights = {}
for init in model.graph.initializer:
    weights[init.name] = onnx.numpy_helper.to_array(init)
    print(f"{init.name}: {weights[init.name].shape}")

np.savez('tusimple_postprocess_weights.npz', **weights)
print("Guardado.")
