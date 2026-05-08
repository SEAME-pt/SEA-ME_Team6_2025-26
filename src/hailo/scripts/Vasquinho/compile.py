from hailo_sdk_client import ClientRunner

runner = ClientRunner(hw_arch="hailo8")
runner.load_har("best_quantized.har")
runner.compile()

if hasattr(runner, "save_hef"):
	runner.save_hef("best.hef")
else:
	hef_bytes = runner.hef
	if not isinstance(hef_bytes, (bytes, bytearray)):
		hef_bytes = bytes(hef_bytes)
	with open("best.hef", "wb") as f:
		f.write(hef_bytes)

print("Compilation done ✅")
