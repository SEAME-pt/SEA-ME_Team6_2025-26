#!/usr/bin/env python3
"""
Wrapper script para execução offline de yolov8s sem dependências de Picamera2 hardware.
"""
import sys
import os

# Mock Picamera2 para evitar erros de imports
class MockPickamera2:
    pass

sys.modules['picamera2'] = MockPickamera2()
sys.modules['picamera2.picamera2'] = MockPickamera2()

# Agora importa o script original
exec(open('/root/yolo_scripts/inference_camera_scalercrop_yolov8s.py').read())
