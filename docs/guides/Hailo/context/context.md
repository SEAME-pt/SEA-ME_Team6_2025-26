# Project Context (SEA:ME Team 6)

## 1) Programa e enquadramento
O projeto integra o programa **SEA:ME — Software Engineering in Automotive and Mobility Ecosystems**, com foco em engenharia de software automóvel de nível industrial.

Princípios e standards relevantes:
- ISO 26262 (functional safety)
- Automotive SPICE
- Arquiteturas distribuídas de veículo
- Comunicação CAN
- Sistemas de tempo real
- OTA update infrastructure

## 2) Estado atual do projeto
- Módulo 0: concluído
- Módulo 1: concluído
- Módulo 2 (ADAS): em curso (refinamento técnico)
- OTA (Goal 6): parcialmente implementado e documentado
- Estratégia atual: melhorar o sistema existente, não recomeçar do zero

## 3) Missão ADAS nesta fase
O carro deve circular de forma automática numa pista com variações desafiantes:
- pista preta com linhas brancas e linha amarela,
- sinais verticais,
- sinais horizontais (`passadeira`, `STOP`, virar à esquerda/direita),
- sombras e alterações de iluminação,
- curvas/contracurvas,
- curvas apertadas,
- curvas a 90º e cruzamentos,
- alterações de cor/contraste no pavimento.

## 4) Arquitetura de hardware
Plataforma principal:
- PiRacer chassis
- Raspberry Pi 5 (vehicle computer)
- Raspberry Pi 4 (instrument cluster)
- STM32U585AI + B-U585I-IOT02A
- Hailo AI Hat
- câmara e sensores ADAS
- servo (steering), DC motor (propulsion), speed sensor
- MCP2515, MCP2518, transceiver CAN
- display, BMS, step-down converters

Mudança arquitetural relevante:
- o STM32 passou a controlar motores e power management (substituindo a expansion board original nesse papel).

## 5) Arquitetura de software
### STM32 (MCU)
- MCU: STM32U585AI
- RTOS: ThreadX (Azure RTOS)
- Responsabilidades:
  - leitura de velocidade
  - controlo de steering e propulsion
  - processamento de sinais em tempo real
  - envio de sinais por CAN

### Raspberry Pi 5 (AGL)
- OS: Automotive Grade Linux 20.0.2
- Componentes: CAN interface, VSS, KUKSA Databroker
- Responsabilidades:
  - receber sinais do STM32 via CAN
  - mapear para VSS
  - expor sinais via KUKSA (porta 55555)

Nota importante:
- parte relevante da configuração existe diretamente no filesystem AGL (`/etc`, `/opt`, `/data`, serviços `systemd`) e não está totalmente versionada no repositório.

### Raspberry Pi 4 (Cluster)
- OS: Debian Bookworm
- Responsabilidades:
  - app Qt de instrument cluster
  - ligação ao KUKSA
  - UI de bordo

## 6) Fluxo de comunicação CAN (resumo)
STM32
-> MCP2515 (transceiver)
-> MCP2518 (CAN Hat)
-> Raspberry Pi 5 (AGL)
-> KUKSA + VSS
-> Qt Cluster (Raspberry Pi 4)
-> Display

## 7) Decisões técnicas relevantes
- Não usar `uProtocol`.
- Usar `COVESA VSS` + `KUKSA Databroker`.
- Separar claramente benchmark `infer-only` de validação `E2E`.

## 8) Objetivo documental
Este ficheiro existe para centralizar contexto estável do projeto e reduzir ambiguidade durante:
- planeamento,
- conversão de modelos,
- benchmark,
- validação E2E,
- discussão de CI/CD e OTA.

## 9) Referências no repositório
- `docs/sprints/`
- `docs/guides/`
- `docs/guides/OTA/`
- `docs/guides/Car_Architecture/`
- `src/`
- `docs/contents_2025/README.md`
