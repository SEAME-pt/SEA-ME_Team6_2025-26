# Conversion Step by Step

Este guia descreve o fluxo real para converter modelos para Hailo, incluindo exportação, calibração, compilação e validação. A ideia é chegar sempre a um artefacto rastreável: cada `HEF` deve dizer de que modelo veio, com que resolução, com que calibração e com que tipo de pós-processamento.

## 1) Escolher a rota de conversão
Antes de tocar em comandos, decidir qual destes caminhos está a ser seguido:
- **Baseline Model Zoo:** usar um modelo suportado diretamente pelo `hailomz` para validar ambiente e fluxo.
- **Modelo customizado:** partir de `.pt` ou `.onnx` do projeto e compilar com os nós de entrada/saída corretos.
- **Split / no-NMS:** compilar a parte de inferência e deixar pós-processamento ou NMS no host, quando for a melhor opção para latência ou compatibilidade.

## 2) Exportar o modelo para ONNX
Se o ponto de partida for `YOLO` em PyTorch, exportar primeiro para ONNX.

Exemplo com Ultralytics:

```bash
python -c "from ultralytics import YOLO; YOLO('yolo26n-seg.pt').export(format='onnx', imgsz=640, opset=11)"
```

ou, para YOLOv8n-seg:

```bash
python -c "from ultralytics import YOLO; YOLO('yolov8n-seg.pt').export(format='onnx', imgsz=640, opset=11)"
```

Boas práticas:
- manter o `imgsz` igual ao que será usado no E2E;
- confirmar que o `.onnx` gerado abre no Netron;
- guardar o ficheiro em `models/` com nome explícito.

## 3) Validar o ONNX antes da compilação
- Abrir o grafo no Netron ou ferramenta equivalente.
- Confirmar se os nós de entrada e saída fazem sentido.
- Anotar nomes de nós que possam ser necessários no `hailomz` (`--start-node-names` e `--end-node-names`).

Se o `ONNX` estiver mal exportado, o problema deve ser corrigido aqui — não na compilação.

## 4) Preparar o dataset de calibração
- Usar imagens representativas do domínio real da pista.
- Misturar cenários bons e degradados: reta, curva, sombra, passadeira, sinalização e cruzamentos.
- Começar com 64 imagens para debug rápido; subir para 256+ para validação mais séria.

Exemplo de organização:

```bash
calibration_images/
├── normal/
├── shadow/
├── curve/
├── crosswalk/
└── signs/
```

## 5) Compilar com `hailomz`
Dentro do container Hailo, usar a rota apropriada.

### 5.1 Baseline rápido para validar o ambiente

```bash
hailomz compile yolov8n_seg \
	--hw-arch hailo8 \
	--calib-path /local/workspace/shared/calibration_images \
	2>&1 | tee /local/workspace/shared/logs/compile_yolov8n_seg_baseline.log
```

Este passo serve para confirmar que o container, a calibração e o `hailomz` estão funcionais.

### 5.2 Compilar um modelo customizado

```bash
hailomz compile yolov8n_seg \
	--ckpt /local/workspace/shared/models/yolo26n-seg.onnx \
	--hw-arch hailo8 \
	--calib-path /local/workspace/shared/calibration_images \
	--start-node-names <start_node_name> \
	--end-node-names <end_node_name> \
	-o /local/workspace/shared/hef/yolo26n_seg_640_h8.hef \
	2>&1 | tee /local/workspace/shared/logs/compile_yolo26n_seg.log
```

### 5.3 Quando usar `no-NMS` ou split hybrid
Se o objetivo for manter NMS no host ou reduzir risco de compilação, documentar isso explicitamente no nome do artefacto e no log.

Exemplo de naming útil:
- `yolo26n_seg_640_h8_full_int8.hef`
- `yolo26n_seg_640_h8_no_nms.hef`
- `yolov8n_seg_640_h8.hef`

## 6) Verificar a saída da compilação
- Confirmar no log a mensagem de sucesso de compilação.
- Verificar tamanho e localização do `HEF`.
- Guardar sempre o log junto do artefacto.

```bash
ls -lh /local/workspace/shared/hef/
tail -40 /local/workspace/shared/logs/compile_yolo26n_seg.log
```

## 7) Validar o `HEF`
Antes de passar ao target, validar o ficheiro com `hailortcli`.

```bash
hailortcli parse-hef /data/yolo26n_seg_640_h8.hef
```

Se o objetivo for benchmark infer-only, usar depois `hailortcli benchmark`. Se o objetivo for E2E, passar o `HEF` para o pipeline completo e medir com preprocess + render incluídos.

## 8) Copiar para o target ou para artefactos finais
- Copiar o `HEF` para o mount partilhado ou para `/data` no target.
- Não apagar a versão anterior até confirmar a nova.
- Guardar o checksum ou pelo menos o nome completo do ficheiro.

## 9) Rastreabilidade mínima por tentativa
Cada tentativa deve ficar com:
- nome do modelo fonte;
- resolução;
- dataset de calibração usado;
- comando de compilação;
- log completo;
- `HEF` final;
- decisão: sucesso, fallback ou revisão necessária.

## 10) Resultado esperado
- `ONNX` validado.
- `HEF` compilado e identificado.
- Log de compilação guardado.
- Pronto para benchmark ou E2E.

## Ligações úteis
- `README.md`
- `conversion_pipeline.md`
- `../10_setup/setup_step_by_step.md`
- Base documental: `../backup/legacy_2026-04-14/convert_models_from_scratch_tutorial.md`, `../backup/legacy_2026-04-14/sw_suite_detailed_conversion_tutorial.md`, `../backup/legacy_2026-04-14/sw_suite_short_conversion_tutorial.md`
