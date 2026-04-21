 hailo_demo_common.py serve como biblioteca utilitária partilhada pelos teus scripts de inferência.

Para que ele existe

Evitar duplicação: em vez de repetir o mesmo código em inference_camera_scalercrop_*.py.
Unificar comportamento: câmara, pasta de imagens, gravação de vídeo, e parsing de argumentos ficam iguais em todos os scripts.
O que ele fornece

CameraFrameSource: lê frames da câmara (Picamera2) com ScalerCrop.
ImageFolderSource: lê imagens de uma pasta (--images-dir) para modo offline.
AsyncVideoWriter: grava vídeo em background sem bloquear o loop de inferência.
build_arg_parser(...): argumentos padrão (duration, --save, --images-dir, --loop, --output).
make_frame_source(...): escolhe automaticamente entre câmara ou pasta.
resize_for_output(...): garante resolução de saída consistente.
Em resumo: ele é o “módulo comum de I/O” dos demos Hailo. Sem ele, cada script teria muito código repetido.

