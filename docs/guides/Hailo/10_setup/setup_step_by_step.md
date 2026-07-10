# Setup Step by Step

Este guia explica como preparar o ambiente Hailo de ponta a ponta. A regra principal é: o `hailomz` não se instala como pacote independente no host; ele vem dentro do Hailo AI Software Suite e é usado dentro do container.

## 1) Obter o Hailo AI Software Suite
- Entrar no portal oficial da Hailo com a conta da equipa.
- Fazer download do pacote da suite compatível com o projeto, por exemplo `hailo8_ai_sw_suite_2025-10_docker.zip` ou a versão validada pela equipa.
- Guardar o ficheiro no Lenovo, idealmente numa pasta dedicada ao workspace Hailo.

## 2) Preparar o host Lenovo
- Confirmar que o Docker está instalado e responde corretamente.
- Confirmar que o utilizador pertence ao grupo `docker`.
- Validar rapidamente o ambiente:

```bash
docker --version
docker run hello-world
```

## 3) Carregar a image da Hailo
- Descompactar o pacote descarregado, se necessário.
- Carregar a imagem para o Docker local.
- Confirmar que a image aparece na lista de imagens.

```bash
docker load < hailo8_ai_sw_suite_2025-10.tar.gz
docker images | grep -i hailo
```

## 4) Criar a estrutura de trabalho
- Usar uma pasta estável no host para evitar paths diferentes entre runs.
- Criar subpastas para modelos, calibração, logs e outputs.

```bash
mkdir -p "$HOME/Documents/AI/hailo/shared_with_docker"
mkdir -p "$HOME/Documents/AI/hailo/shared_with_docker/{models,calibration_images,logs,outputs,scripts,hef}"
cd "$HOME/Documents/AI/hailo/shared_with_docker"
```

Estrutura mínima esperada:

```text
shared_with_docker/
├── calibration_images/
├── hef/
├── logs/
├── models/
├── outputs/
└── scripts/
```

## 5) Iniciar o container Hailo
- Montar a pasta partilhada no container.
- Entrar no shell da image Hailo.
- Confirmar que o volume está visível.

```bash
docker run -it --rm \
	-v "$HOME/Documents/AI/hailo/shared_with_docker:/local/workspace/shared" \
	hailo_ai_suite:latest
```

Dentro do container:

```bash
cd /local/workspace/shared
pwd
ls -la
which hailomz
hailomz --help | head
```

Se `hailomz` não aparecer, o problema é a image/ambiente do container, não o projeto.

## 6) Confirmar os inputs do projeto
- Verificar se os modelos fonte existem em `models/`.
- Confirmar se a pasta de calibração tem imagens reais e representativas.
- Começar com um conjunto pequeno e estável para debug; para validação final, usar um subconjunto maior.

Exemplo para imagens de calibração:

```bash
find calibration_images -type f | wc -l
find calibration_images -type f | head -20
```

Recomendação prática:
- usar um conjunto inicial de 64 imagens para primeiros testes rápidos;
- usar 256+ imagens para compilação/validação final, quando o tempo permitir;
- garantir mistura de reta, curva, sombra, passadeira e sinais, se o dataset já estiver etiquetado assim.

## 7) Validar o target AGL
- Confirmar que o target com Hailo está acessível por SSH.
- Garantir que a pasta `/data` ou equivalente está pronta para receber artefactos.
- Validar rapidamente que o runtime reconhece o `HEF`.

```bash
ssh root@<AGL_IP>
cd /data
hailortcli --help | head
```

## 8) Usar o `hailomz` na prática
O fluxo normal é:
1. abrir o container;
2. verificar o `hailomz`;
3. apontar para o modelo e calibração;
4. compilar;
5. guardar logs e outputs no mount partilhado.

Exemplo de verificação do help:

```bash
hailomz compile --help | head -40
```

## 9) Check final antes de converter
- Modelo fonte presente.
- Calibração preparada.
- Paths estáveis.
- Logs e outputs a escrever no mount partilhado.
- Target AGL pronto para receber o `HEF`.

## 10) Resultado esperado
- Host preparado para trabalhar com Docker e Hailo.
- Container com `hailomz` operacional.
- Estrutura de pastas normalizada.
- Target pronto para validar `HEF` e inferência.

## Ligações úteis
- `README.md`
- `environment_setup_checklist.md`
- `../context/context.md`
- Base documental: `../backup/legacy_2026-04-14/sw_suite_detailed_conversion_tutorial.md`, `../backup/legacy_2026-04-14/sw_suite_short_conversion_tutorial.md`, `../backup/legacy_2026-04-14/convert_models_from_scratch_tutorial.md`
