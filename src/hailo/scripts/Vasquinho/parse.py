import os
from hailo_sdk_client import ClientRunner

runner = ClientRunner(hw_arch="hailo8")

default_end_nodes = [
    "/model.22/cv2.0/cv2.0.2/Conv",
    "/model.22/cv3.0/cv3.0.2/Conv",
    "/model.22/cv2.1/cv2.1.2/Conv",
    "/model.22/cv3.1/cv3.1.2/Conv",
    "/model.22/cv2.2/cv2.2.2/Conv",
    "/model.22/cv3.2/cv3.2.2/Conv",
]

raw_end_nodes = os.environ.get("END_NODE_NAMES", "").strip()
end_nodes = [n.strip() for n in raw_end_nodes.split(",") if n.strip()] if raw_end_nodes else default_end_nodes

runner.translate_onnx_model(
    "best.onnx",
    "best_model",
    end_node_names=end_nodes,
    net_input_shapes={"images": [1, 3, 640, 640]}
)

runner.save_har("best.har")
print("Parsing done ✅")
