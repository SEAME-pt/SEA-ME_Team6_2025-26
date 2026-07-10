# 🗺️ STM32 ThreadX — Guia Completo (Arquitetura + Padrões C)

---

## 📋 Índice

1. [Boot Sequence (Ordem Cronológica)](#-boot-sequence-ordem-cronológica)
2. [Threads em Regime Estacionário](#-threads-em-regime-estacionário)
3. [Recursos de Sincronização (Mutexes)](#-recursos-de-sincronização-mutexes)
4. [Fluxo de Dados (Sensor → CAN → RPi5)](#-fluxo-de-dados-sensor--can--rpi5)
5. [Prioridades — Visualização](#-prioridades--visualização)
6. [Fluxo Real de Execução (Detalhado)](#fluxo-real-de-execução-ordem-de-execução)
7. [O que Cada Thread Faz](#o-que-cada-thread-faz-e-porquê-foi-criada)
8. [Sincronização e Determinismo](#sincronização-e-determinismo-threadx-no-vosso-caso)
9. [Notas Importantes do Código](#notas-importantes-que-vi-no-código)
10. [Bare-Metal e HAL_Init](#bare-metal-e-hal_init-explicados)
11. [NVIC, PendSV e SysTick](#nvic-pendsv-e-systick--explicação-detalhada)
12. [Padrões C — Análise Detalhada](#padrões-c-no-projeto)
13. [Tabela-Resumo de Conceitos](#resumo-numa-tabela)

---

## ① Boot Sequence (Ordem Cronológica)

```
┌─────────────────────────────────────────────────────────────────────────────┐
│  POWER ON / RESET                                                           │
│                                                                             │
│  startup_stm32u585aiixq.s  ← Assembly: inicializa stack pointer,           │
│                               copia .data para RAM, zera .bss,             │
│                               chama main()                                 │
└───────────────────────────────────┬─────────────────────────────────────────┘
                                    │
                                    ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│  main()   [Core/Src/main.c]   ← BARE-METAL (sem RTOS ainda)                │
│                                                                             │
│  HAL_Init()           → configura SysTick (1ms tick), NVIC, watchdog       │
│  SystemPower_Config() → SMPS regulator                                     │
│  SystemClock_Config() → PLL → 160 MHz                                      │
│                                                                             │
│  MX_GPIO_Init()       → pinos IN/OUT                                       │
│  MX_I2C1/2_Init()     → sensores (SRF08, IMU, ToF, Battery, Indicator)     │
│  MX_SPI1/2_Init()     → MCP2515 (CAN), SPI2                                │
│  MX_TIM1_Init()       → PWM servo (PA8, 50Hz)                              │
│  MX_TIM4_Init()       → Input Capture speed sensor (EXTI pulsos)           │
│  MX_UART4/USART1()    → debug serial (printf → UART)                       │
│                                                                             │
│  HAL_TIM_IC_Start_IT() → inicia captura de pulsos (speed sensor)           │
│  I2C scan (debug)                                                           │
│  matrix_init() / PWM servo → posição central 90°                           │
│                                                                             │
│  MX_ThreadX_Init()    ← entrega controlo ao RTOS (não retorna)             │
└───────────────────────────────────┬─────────────────────────────────────────┘
                                    │
                                    ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│  MX_ThreadX_Init()  [Core/Src/app_threadx.c]                               │
│                                                                             │
│  tx_kernel_enter()  ← kernel ThreadX arranca                               │
│       │                                                                    │
│       └─► tx_application_define()  [AZURE_RTOS/App/app_azure_rtos.c]      │
│               tx_byte_pool_create()  → pool de memória estática 20 KB      │
│               App_ThreadX_Init()    ← cria recursos e threads              │
└───────────────────────────────────┬─────────────────────────────────────────┘
                                    │
                                    ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│  App_ThreadX_Init()  [Core/Src/app_threadx.c]                              │
│                                                                             │
│  system_ctx_init()   → cria mutexes globais:                               │
│                         printf_mutex, spi1_mutex, i2c1_mutex,              │
│                         state_mutex, sys_mutex                              │
│                                                                             │
│  tx_thread_create() × 10  → cria todas as threads (TX_AUTO_START)          │
│                                                                             │
│  Scheduler arranca → executa thread de maior prioridade disponível         │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## ② Threads em Regime Estacionário

| # | Thread | Ficheiro | Prio | Freq | CAN Frame enviado | O que faz |
|---|--------|----------|------|------|-------------------|-----------|
| 1 | **HeartBeat** | `task_heartbeat.c` | **10** | ~1 Hz | `CAN_ID_HEARTBEAT_STM32` | Inicializa MCP2515; envia estado + uptime + erros |
| 2 | **CAN RX** | `task_can_rx.c` | **11** | ~100 Hz (10ms) | `CAN_ID_MOTOR_STATUS` | Recebe comandos AGL → motor/servo/relay/indicadores; aplica AEB/Emergency |
| 3 | **SRF08** | `task_srf08.c` | **11** | ~14 Hz (70ms) | `CAN_ID_SRF08_DISTANCE` | Ultrassónico frontal; filtra mediana; publica distância |
| 4 | **Speed** | `task_speed.c` | **12** | ~10 Hz | `CAN_ID_WHEEL_SPEED` | Conta pulsos EXTI → RPM/m·h; low-pass; publica velocidade |
| 5 | **IMU** | `task_imu.c` | **13** | ~20 Hz | `CAN_ID_IMU_ACCEL/GYRO/MAG` | Lê accel + gyro + mag; publica 3 frames |
| 6 | **ToF** | `task_tof.c` | **14** | ~15 Hz (66ms) | `CAN_ID_TOF_DISTANCE` | VL53L5CX 8×8; distância mínima + zona nearest |
| 7 | **Environment** | `task_environment.c` | **15** | ~1 Hz | `CAN_ID_ENVIRONMENT` | LPS22HH + HTS221 + VEML6030 → temp/hum/pressão/luz |
| 8 | **AEB** | `task_aeb.c` | **11** | 50 Hz (20ms) | `CAN_ID_AEB_STOP` | Máquina de estados: OFF→ARMED→WARN→BRAKING→LATCHED; TTC + dist paragem |
| 9 | **Battery** | `task_battery.c` | **15** | ~0.5 Hz | `CAN_ID_BATTERY` | INA226 → tensão/corrente/SOC; flags sub/sobre-tensão |
| 10 | **Indicator** | `task_indicator.c` | **16** | 20 Hz (50ms) | *(sem CAN — I2C direto)* | KS0064 LEDs: pisca esq/dir, faróis, alerta |

---

## ③ Recursos de Sincronização (Mutexes)

```
┌──────────────┬──────────────────────────────────────────────────────────────┐
│ Mutex        │ O que protege                                                │
├──────────────┼──────────────────────────────────────────────────────────────┤
│ spi1_mutex   │ Bus SPI ↔ MCP2515 (CAN tx e rx não colidem)                 │
│ i2c1_mutex   │ Bus I2C1 (SRF08, IMU, Battery, ToF XSHUT, Indicator)        │
│ state_mutex  │ VehicleState snapshot (lido por AEB e CAN_RX, escrito pelas  │
│              │ tasks de sensores) — padrão "snapshot seguro"                │
│ sys_mutex    │ system_state, drive_mode, error_flags                        │
│ printf_mutex │ UART serial (sem mistura de logs de várias threads)          │
└──────────────┴──────────────────────────────────────────────────────────────┘
```

---

## ④ Fluxo de Dados (Sensor → CAN → RPi5)

```
Speed Sensor (EXTI pulso)
        │ ISR conta pulsos
        ▼
  [Speed Thread] ──────────────────────────────► CAN_ID_WHEEL_SPEED (0x3xx)
        │ state.speed_mh / rpm
        ▼
  [AEB Thread] ← lê state.speed_mh + state.srf08_distance_mm
        │ calcula TTC / d_stop
        │ escreve: aeb_stop_active, aeb_speed_limit, aeb_state
        ▼
  [CAN_RX Thread] ← recebe MotorCmd do RPi5
        │ lê AEB flags → impõe limite de throttle
        │ Motor_Forward/Backward/Stop + Servo_SetAngle
        ▼
  Motor TB6612FNG + Servo MG996R

SRF08 (I2C) ──► [SRF08 Thread] ──► state.srf08_distance_mm ──► AEB
VL53L5CX   ──► [ToF Thread]   ──► CAN_ID_TOF_DISTANCE
IMU        ──► [IMU Thread]   ──► CAN_ID_IMU_ACCEL/GYRO/MAG
INA226     ──► [Battery Thread] ► CAN_ID_BATTERY

Todos os CAN frames ──► MCP2515 ──► MCP2518 Hat ──► RPi5 (AGL/KUKSA)
```

---

## ⑤ Prioridades — Visualização

```
Prioridade  0 ────────────────────────────────── 31
            (mais urgente)              (menos urgente)

  10  ■ HeartBeat        (monitorização sistema + init CAN)
  11  ■ CAN_RX           (actuation — motor/servo)
  11  ■ SRF08            (safety — obstáculos frontais)
  11  ■ AEB              (safety — tomada de decisão)
  12  ■ Speed            (velocidade real-time)
  13  ■ IMU              (accel/gyro/mag)
  14  ■ ToF              (distância multi-zona)
  15  ■ Environment      (temp/hum/pressão)
  15  ■ Battery          (tensão/corrente/SOC)
  16  ■ Indicator        (LEDs — background)
```

---

# Fluxo Real de Execução (Ordem de Execução)

## Boot Bare-Metal Primeiro

Em `src/stm/Core/Src/main.c`, faz `HAL_Init`, clock/power, init de periféricos (`GPIO`, `I2C`, `SPI`, `TIM`, `UART`...), alguns testes de arranque (I2C scan, matriz, PWM servo), e depois chama `MX_ThreadX_Init()`.

---

## Entrada no Kernel ThreadX

`MX_ThreadX_Init()` (em `src/stm/Core/Src/app_threadx.c`) chama `tx_kernel_enter()`.

---

## Definição da App ThreadX

O kernel entra em `tx_application_define()` (`src/stm/AZURE_RTOS/App/app_azure_rtos.c`), cria byte pool (`tx_byte_pool_create`) e chama `App_ThreadX_Init(...)`.

---

## Inicialização de Contexto Partilhado

`App_ThreadX_Init` começa por `system_ctx_init()`; em `src/stm/Core/Src/system_ctx.c` cria mutexes (`spi1`, `i2c1`, `state`, `sys`, `printf`) e estado global inicial.

---

## Criação das Threads (TX_AUTO_START)

Em `App_ThreadX_Init` são criadas, por esta ordem:
1. **HeartBeat**
2. **CAN_RX**
3. **Temperature/Environment**
4. **Speed**
5. **IMU**
6. **ToF**
7. **SRF08**
8. **AEB**
9. **Battery**
10. **Indicator**

---

## Scheduler Entra em Regime Contínuo

Não há "fim" — o sistema fica em execução permanente com loops `while(1)` em cada thread.

---

# O que Cada Thread Faz (e Porquê foi Criada)

## HeartBeat
Inicializa MCP2515 (`mcp_init`), muda estado sistema para `READY/RUNNING`, envia heartbeat CAN periódico. Serve para "vida" do ECU e health monitoring.

## CAN_RX
Recebe frames CAN, valida CRC, aplica comandos (motor/servo/relay/indicadores), impõe regras de segurança (`emergency_stop` e flags AEB), envia `MotorStatus`. É o "atuador central".

## Temperature/Environment
Lê LPS22HH/HTS221/VEML6030 e publica frame CAN ambiental. Telemetria de ambiente.

## Speed
Usa pulsos do speed sensor, calcula RPM/velocidade, atualiza `VehicleState`, envia `CAN_ID_WHEEL_SPEED`. Base para controlo e AEB.

## IMU
Lê accel/gyro/mag, envia CAN IMU, atualiza snapshot do estado. Perceção de dinâmica do veículo.

## ToF
Inicializa VL53L5CX, lê distância mínima/zona, envia CAN ToF, escreve `tof_distance_mm` no estado partilhado.

## SRF08
Mede ultrassónico frontal, filtra (mediana), envia CAN SRF08 e atualiza `state.srf08_*`; hoje já não decide travagem diretamente (isso foi movido para AEB).

## AEB
Máquina de estados (`OFF/ARMED/WARN/BRAKING/LATCHED`), calcula TTC/distância de paragem e publica `aeb_stop_active`, `aeb_warn`, `aeb_speed_limit` no `VehicleState`.

## Battery
Lê INA226, estima SOC/status, envia frame CAN de bateria.

## Indicator
Controla KS0064 (pisca/faróis/alerta), com mutex próprio + `i2c1_mutex`.

---

# Sincronização e Determinismo (ThreadX no nosso caso)

## Mutexes
- `spi1_mutex` evita corrupção de bus SPI (MCP2515 partilhado)
- `i2c1_mutex` serializa sensores/atuadores I2C
- `state_mutex` protege snapshot `VehicleState`

## Prioridades
Configuradas em `src/stm/Core/Inc/thread_config.h` (número menor = maior prioridade):
- heartbeat 10
- CAN_RX/SRF08 11
- speed 12
- ...

## Pacing Temporal
Cada task usa `tx_thread_sleep(...)` com períodos específicos (ou sleep interno no próprio `task_*_step`) para manter taxa estável e evitar busy-loop.

---

# Notas Importantes

- Há `thread_relay_entry` e `can_rx_queue` declarados em `app_threadx.c`, mas **existe `tx_queue_create` nem thread relay criada** no fluxo ativo atual (parece legado/inativo).
- O "fim" do fluxo é estado estacionário: scheduler ThreadX + threads periódicas + CAN contínuo.

---

---

# Bare-Metal e HAL_Init Explicados

## O que é Bare-Metal?

"Bare-metal" significa programar diretamente o hardware sem nenhum sistema operativo. Não há scheduler, não há threads, não há abstração de tarefas — apenas tu e os registos do microcontrolador.

### Analogia:
"Bare-metal é como construir uma casa — só tu e os tijolos. O HAL é como ter ferramentas elétricas já calibradas. O ThreadX é o arquiteto que depois organiza as equipas de trabalho em paralelo. Antes de o arquiteto chegar, tens de ter a obra preparada."

```
┌───────────────────────────────────────────┐
│  Bare-Metal                               │
│  O nosso código                           │
│  ─────────────────────────────────────    │
│  Hardware (periféricos, registos, CPU)    │
└───────────────────────────────────────────┘

┌───────────────────────────────────────────┐
│  Com RTOS (o nosso caso)                  │
│  O nosso código (tasks)                   │
│  ThreadX (scheduler, mutexes, timers)     │
│  HAL (abstração de periféricos STM32)     │
│  Hardware                                 │
└───────────────────────────────────────────┘
```

No nosso `main.c`, a fase antes de `MX_ThreadX_Init()` é bare-metal puro — estás a configurar hardware diretamente, sem qualquer RTOS ativo. Só depois de `tx_kernel_enter()` é que o sistema passa a ter preemption, scheduling e concorrência.

---

## O que faz HAL_Init() em detalhe?

HAL = Hardware Abstraction Layer — a biblioteca da STMicroelectronics que abstrai os registos do STM32 em funções C portáteis.

`HAL_Init()` faz 4 coisas concretas:

### 1 — Configura o SysTick (1 ms tick)

```c
// Internamente, HAL_Init() chama:
HAL_InitTick(TICK_INT_PRIORITY);
// → configura SysTick para disparar interrupção a cada 1 ms
// → o contador `uwTick` incrementa: base de tempo para HAL_Delay(), HAL_GetTick()
```

No nosso código, o `HAL_GetTick()` é usado em todas as tasks para timestamps (ex: `s_rx.last_motor_status_tick`).

### 2 — Configura o NVIC (interrupt controller)

```c
// Define prioridade de grupo para interrupções
HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
// → 4 bits de prioridade, 0 bits de sub-prioridade
// → 16 níveis de prioridade para IRQs do hardware
```

Isto é importante: o ThreadX usa o PendSV e SysTick com prioridades específicas, e o grupo NVIC tem de ser compatível.

#### Por que têm de ser compatíveis com o NVIC?

O NVIC no Cortex-M tem um sistema de Priority Grouping — configura quantos bits são para prioridade e quantos para sub-prioridade:

```c
// HAL_Init() no STM32:
HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
// → 4 bits de prioridade (0-15), 0 bits de sub-prioridade
```

O ThreadX no STM32 exige uma configuração específica:

| Interrupção | Prioridade configurada | Porquê |
|---|---|---|
| SysTick | baixa (ex: 15) | Se fosse alta, interrompia ISRs de hardware no meio → corromperia drivers |
| PendSV | a mais baixa possível (15) | Tem de executar depois de todas as ISRs de hardware terminarem |
| ISRs de hardware (SPI, I2C...) | 0–14 | Têm de ser mais altas que PendSV |

**O problema se não forem compatíveis:**

Cenário errado (PendSV prioridade alta):

```
  Thread A corre SPI_Transmit()
       │
  SysTick → scheduler decide trocar para Thread B
       │
  PendSV executa IMEDIATAMENTE (prioridade alta)
       │
  Context switch no MEIO de uma transação SPI
       │
  Thread B começa — SPI fica num estado inválido
       │
  CRASH / dados corrompidos no CAN bus
```

Cenário correto (PendSV prioridade 15):

```
  Thread A corre SPI_Transmit()
       │
  SysTick → scheduler sinaliza PendSV
       │
  SPI_Transmit() termina normalmente
       │
  Nenhuma outra ISR ativa → agora sim PendSV executa
       │
  Context switch limpo e seguro
```

### 3 — Inicializa o Flash prefetch/cache

```c
// Permite ao CPU buscar instruções de forma preditiva
// → reduz latência de execução a 160 MHz com Flash mais lento
```

### 4 — Prepara estruturas internas do HAL

```c
// Zera handles internos
// Configura callbacks de erro por omissão
```

---

## Por que é que main.c tem de fazer tudo isso antes de MX_ThreadX_Init()?

Porque o ThreadX precisa de encontrar o hardware já funcional quando o scheduler arrancar. Se a thread HeartBeat tentar usar SPI antes de `MX_SPI1_Init()` ter sido chamado — crash garantido.

**ORDEM OBRIGATÓRIA:**

1. `HAL_Init()`          ← base do HAL (tick + NVIC)
2. Clock config        ← CPU a 160 MHz — SPI/I2C/UART dependem disto
3. `MX_GPIO/SPI/I2C/TIM` ← periféricos prontos
4. `MX_ThreadX_Init()`   ← agora sim, threads podem usar tudo

---

---

# NVIC, PendSV e SysTick — Explicação Detalhada

## Resumo

- **NVIC** = gestor de interrupções do ARM — decide qual ISR executa com base em prioridade
- **SysTick** = timer do kernel, base de tempo do ThreadX (tx_thread_sleep conta tiques SysTick)
- **PendSV** = mecanismo de context switch — executa sempre por último (prio máx. baixa) para nunca cortar ISRs de hardware no meio
- **Compatibilidade** = PendSV e SysTick têm de ter prioridade menor que os drivers de hardware, senão fazem context switch no meio de transações SPI/I2C → corrupção de dados

---

## O que é o NVIC?

NVIC = Nested Vectored Interrupt Controller — é o "gestor de interrupções" do ARM Cortex-M. É um bloco de hardware dentro do próprio CPU que:

- Recebe sinais de interrupção de todos os periféricos (SPI, I2C, TIM, GPIO, CAN...)
- Decide qual executa primeiro (baseado em prioridade)
- Guarda o estado do CPU quando interrompe uma tarefa ("nesting" = podem encadear-se)
- Restaura o estado quando a ISR termina

```
┌─────────────────────────────────────────────────────────────────────────────┐
│  STM32U585AI — Cortex-M33                                                  │
│                                                                             │
│  ┌──────────┐    IRQ linha 0  ──► ┌─────────────────────────────────────┐  │
│  │  SPI1    │    IRQ linha 1  ──► │          N V I C                    │  │
│  │  I2C1    │    IRQ linha 2  ──► │  (compara prioridades de todas)     │  │
│  │  TIM4    │    IRQ linha 3  ──► │  → escala a ISR de maior prioridade │  │
│  │  GPIO    │    ...          ──► │  → salva contexto CPU se necessário │  │
│  │  SysTick │    ─────────────►   └──────────────────┬──────────────────┘  │
│  │  PendSV  │                                        │                     │
│  └──────────┘                                        ▼                     │
│                                               CPU executa ISR              │
└─────────────────────────────────────────────────────────────────────────────┘
```

**"Nested" (aninhado):** uma ISR de prioridade 3 pode ser interrompida por outra de prioridade 1, que termina primeiro — o CPU retoma a de 3 automaticamente. Hardware faz tudo.

---

## O que é o SysTick?

SysTick = System Tick Timer — um timer de 24 bits dentro do núcleo ARM, independente dos timers periféricos (TIM1, TIM4...).

- Conta down de um valor até zero, dispara interrupção, recomeça
- Configurado para disparar a cada 1 ms pelo `HAL_Init()`
- É a base de tempo do HAL: `HAL_GetTick()`, `HAL_Delay()`

```c
// O que acontece a cada 1ms (SysTick ISR no nosso projeto):
void HAL_TIM_PeriodElapsedCallback(...) {
    if (htim->Instance == TIM6) {
        HAL_IncTick();   // uwTick++
    }
}
// (No nosso main.c usa TIM6 para o tick do HAL em vez do SysTick nativo,
//  porque o ThreadX reserva o SysTick para si próprio)
```

### Porque é que o ThreadX "rouba" o SysTick?

O ThreadX usa o SysTick como o seu clock interno — cada tick do SysTick é um "tique" do scheduler. Quando fazes `tx_thread_sleep(100)`, o scheduler conta 100 tiques de SysTick para acordar a thread. Por isso o HAL teve de usar TIM6 como fallback para o seu próprio tick.

---

## O que é o PendSV?

PendSV = Pendable Service Call — uma interrupção especial do ARM, de prioridade mais baixa possível, usada especificamente para fazer context switch (troca de threads).

**Funciona assim:**

```
┌──────────────────────────────────────────────────────────────────────────┐
│  PORQUE EXISTE O PendSV?                                                 │
│                                                                          │
│  Thread A está a correr                                                  │
│       │                                                                  │
│  SysTick dispara (1ms tick)  ← IRQ de alta prioridade                   │
│       │                                                                  │
│  Scheduler ThreadX avalia:                                               │
│    "Thread B tem maior prioridade e está ready"                          │
│    → NÃO faz context switch aqui dentro do SysTick                      │
│    → Sinaliza o PendSV (coloca flag "pendente")                          │
│       │                                                                  │
│  SysTick ISR termina                                                     │
│       │                                                                  │
│  Como PendSV é a prioridade MAIS BAIXA possível,                        │
│  só executa quando não há mais nenhuma ISR ativa                         │
│       │                                                                  │
│  PendSV executa:                                                         │
│    1. Salva registos de Thread A na sua stack                            │
│    2. Carrega registos de Thread B da sua stack                          │
│    3. CPU continua a executar Thread B                                   │
└──────────────────────────────────────────────────────────────────────────┘
```

---

## Diferença entre SysTick e PendSV

Em poucas palavras: o SysTick **decide** quem fica, o PendSV **executa** a troca. Esta separação garante que o context switch nunca acontece no meio de uma ISR de hardware.

---

# Padrões C no Projeto

## `volatile`

**Usam? Sim.**

```c
// app_threadx.c
volatile uint8_t emergency_stop_active = 0;
volatile uint8_t srf08_speed_limit = 100;

// task_tof.c
volatile uint8_t vlx_ready = 0;

// system_ctx.h (via vehicle_state.h — não marcado mas devia ser)
```

**Quando e porquê:**
`volatile` diz ao compilador *"não otimizes o acesso a esta variável — o seu valor pode mudar fora do teu controlo"*. Os vossos casos são:

- `emergency_stop_active` — escrita por uma ISR ou thread de alta prioridade, lida por outra thread
- `vlx_ready` — setada na ISR do VL53L5CX, lida no loop da task ToF
- Sem `volatile`, o compilador podia fazer *register caching*: ler uma vez para um registo do CPU e nunca mais ler da RAM — a ISR atualiza a RAM mas a thread vê sempre o valor antigo

```c
// Sem volatile — compilador pode otimizar para:
bool cached = vlx_ready;  // lê uma vez
while (!cached) { ... }   // nunca sai → bug silencioso

// Com volatile — força leitura da RAM em cada acesso:
while (!vlx_ready) { ... } // sempre vê o valor atual
```

---

## `static` (variáveis locais e globais de ficheiro)

**Usam? Sim — muito.**

```c
// task_can_rx.c
static TaskCanRx s_rx;           // estado privado da task

// task_srf08.c
static TaskSRF08 s_srf;          // idem

// task_aeb.c
static AebCtx s_aeb;             // máquina de estados AEB
static const AebParams P = {...}; // parâmetros imutáveis

// system_ctx.c
static SystemCtx g_ctx;          // instância única global
```

**Dois usos distintos:**

| `static` onde | Efeito | Porquê no vosso projeto |
|---|---|---|
| Dentro de função | Variável persiste entre chamadas | ex: `static uint8_t log_counter` nas tasks |
| Fora de função (ficheiro) | Visibilidade limitada ao ficheiro `.c` | Encapsulamento — `s_srf`, `s_aeb` só acessíveis internamente |

**`static` local em step functions:**
```c
void task_tof_step(SystemCtx* ctx) {
    static uint8_t log_counter = 0;  // persiste entre chamadas da task
    if (++log_counter >= 15) {
        log_counter = 0;
        // log a cada ~1s
    }
}
```

---

## Variáveis Atómicas (`_Atomic` / `stdatomic.h`)

**Usam? Não diretamente.**

O vosso projeto usa **mutexes ThreadX** em vez de atómicas para proteger dados partilhados. Em C11 existe `_Atomic` mas num RTOS como o ThreadX o padrão correto é mesmo usar `tx_mutex_get/put` — as atómicas não protegem sequências de operações (ex: read-modify-write em structs).

```c
// O que fazem em vez de atomics:
tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
ctx->state.speed_mh = speed_mh;   // escrita protegida
tx_mutex_put(&ctx->state_mutex);
```

**Quando atómicas fariam sentido:** variável simples inteira, sem struct, sem operação composta — ex: flag de 1 bit entre ISR e thread. Mas mesmo assim `volatile` + desabilitar interrupções é mais comum em ARM.

---

## Variáveis Globais

**Usam? Sim, com disciplina.**

```c
// system_ctx.c — a única instância global real
static SystemCtx g_ctx;
SystemCtx* system_ctx(void) { return &g_ctx; }
```

Todas as threads acedem ao estado via `system_ctx()` — um *accessor* que devolve ponteiro para a struct global. Isto é o padrão **singleton em C**.

```c
// Qualquer task acede assim:
SystemCtx* ctx = system_ctx();
tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
ctx->state.rpm = ...;
tx_mutex_put(&ctx->state_mutex);
```

**Porquê não fazer `extern SystemCtx g_ctx` em todo o lado?** Porque assim qualquer ficheiro poderia escrever sem mutex → race condition. O accessor força o uso do padrão correto.

---

## `const`

**Usam? Sim.**

```c
// task_aeb.c — parâmetros de tuning imutáveis
static const AebParams P = {
    .d_offset_m    = 0.10f,
    .ttc_warn_s    = 0.8f,
    .ttc_brake_s   = 0.5f,
    .lp_alpha      = 0.70f,
    ...
};
```

**Porquê `const` aqui:** o compilador pode colocar `P` em Flash (ROM) em vez de RAM — num MCU com 256 KB RAM isso importa. Também documenta intenção: *"ninguém deve mudar estes parâmetros em runtime"*.

```c
// Parâmetros passados como const pointer → sem cópia, sem modificação
static void handle_motor_cmd(SystemCtx* ctx,
                              const CAN_Message_t* rx_msg,   // const: só leitura
                              const VehicleState* snap) {    // const: snapshot imutável
```

---

## `constexpr`

**Usam? Não — é C++, o projeto é C.**

Em C usam `#define` ou `static const` para o mesmo efeito:
```c
// Equivalentes C ao constexpr:
#define SERVO_MID_PULSE  1500
static const uint32_t TX_APP_MEM_POOL_SIZE = 20480;
```

---

## Herança e Polimorfismo

**Usam? Não — projeto em C puro.**

Se fosse C++, a `SystemCtx` podia ser uma classe base com `VehicleState` como interface. Em C simulam encapsulamento com:
- structs privadas por ficheiro (`static TaskCanRx s_rx`)
- funções `init/step` por módulo (padrão que imita métodos de classe)
- accessor `system_ctx()` (padrão singleton)

---

## Access Specifiers (`public/private/protected`)

**Usam? Não — C puro.**

Simulam com convenções:
- `static` no ficheiro `.c` = **private** (não visível fora)
- declarado no `.h` = **public**

```c
// task_srf08.c — "private" (não declarado no .h)
static TaskSRF08 s_srf;
static uint16_t srf08_apply_filter(...) { ... }

// task_srf08.h — "public"
void task_srf08_init(SystemCtx* ctx);
void task_srf08_step(SystemCtx* ctx);
```

---

## Getters/Setters

**Usam? Sim — `task_indicator` tem um par clássico.**

```c
// task_indicator.c — setter thread-safe
void task_indicator_set_state(IndicatorState_t state) {
    tx_mutex_get(&s_mutex, TX_WAIT_FOREVER);
    s_state = state;
    tx_mutex_put(&s_mutex);
}

// getter thread-safe
IndicatorState_t task_indicator_get_state(void) {
    IndicatorState_t s;
    tx_mutex_get(&s_mutex, TX_WAIT_FOREVER);
    s = s_state;
    tx_mutex_put(&s_mutex);
    return s;
}
```

**Porquê:** `s_state` é `static volatile` — múltiplas threads podem ler/escrever. O getter/setter encapsula o mutex para que o chamador não precise de saber da implementação interna.

---

## ISR vs Polling

**Usam os dois, com lógica clara de quando usar cada um.**

### ISR no vosso projeto:
```c
// main.c — speed sensor via Input Capture Interrupt (TIM4)
HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);
// → cada pulso do sensor gera IRQ → ISR incrementa contador
// → task_speed.c lê o contador periodicamente

// VL53L5CX — GPIO interrupt quando dados prontos
volatile uint8_t vlx_ready = 0;
// ISR externa seta vlx_ready = 1 → task_tof acorda
```

### Polling no vosso projeto:
```c
// task_can_rx.c — polling do MCP2515
uint8_t has_msg = MCP2515_CheckReceive();  // polling ativo
if (has_msg) read_ok = MCP2515_ReadMessage(&rx_msg);
tx_thread_sleep(10);  // cede CPU entre polls

// task_srf08.c — polling do sensor ultrassónico
#define SRF08_DISABLE_POLLING 1  // até desabilitaram o polling
// porque o SRF08 não suporta interrupt → sleep fixo 70ms
```

**Regra geral seguida no nosso projeto:**

| Situação | Escolha | Porquê |
|---|---|---|
| Evento imprevisível, latência crítica | **ISR** | Speed sensor, VL53L5CX data ready |
| Sensor periódico, sem interrupt pin | **Polling + sleep** | SRF08, MCP2515 rx |
| Dados chegam em qualquer momento | **ISR + queue/flag** | padrão clássico |

---

## Prioridade, Scheduler e Preemption

**Usamos intensivamente — é o coração do ThreadX.**

```
Scheduler ThreadX (preemptivo, baseado em prioridade):

  Thread AEB    (prio 11) ──► RUNNING
        │
  SysTick tick → scheduler verifica
        │
  CAN_RX (prio 11) fica READY
        │
  Mesmo nível → round-robin? Não: TX_NO_TIME_SLICE
  → AEB continua até fazer tx_thread_sleep()
        │
  HeartBeat (prio 10 — MAIS ALTA) fica READY
        │
  PREEMPTION IMEDIATA → HeartBeat interrompe AEB
```

```c
// thread_config.h — as vossas prioridades:
#define HEARTBEAT_THREAD_PRIORITY  10   // crítico: init CAN
#define CAN_RX_THREAD_PRIORITY     11   // actuation
#define SRF08_THREAD_PRIORITY      11   // safety
#define AEB_THREAD_PRIORITY        11   // safety
#define SPEED_THREAD_PRIORITY      12   // real-time
#define IMU_THREAD_PRIORITY        13
#define TOF_THREAD_PRIORITY        14
#define TEMP_THREAD_PRIORITY       15
#define INDICATOR_THREAD_PRIORITY  16   // background
```

**Preemption na prática:** se um obstáculo aparece, a SRF08 thread (prio 11) escreve `state.srf08_distance_mm`, a AEB (prio 11) processa, e se fizer `aeb_stop_active=1`, a CAN_RX (prio 11) na próxima iteração chama `Motor_Stop()`. Threads de prio 12-16 ficam à espera — não interferem.

---

## Dangling Pointer — Como Previnem

**Dangling pointer = ponteiro que aponta para memória já libertada ou inválida.**

No vosso projeto, **a abordagem é evitar o problema por design:**

**1 — Sem `malloc/free` → sem dangling pointers de heap:**
```c
// Tudo é alocação estática:
static uint8_t heartbeat_thread_stack[HEARTBEAT_THREAD_STACK_SIZE];
static TaskCanRx s_rx;
static AebCtx s_aeb;
// → nada é libertado em runtime → ponteiros nunca ficam "dangling"
```

**2 — Padrão snapshot para partilha de dados:**
```c
// Em vez de passar ponteiro para VehicleState (que outra thread pode modificar):
VehicleState snap;                          // cópia LOCAL na stack
tx_mutex_get(&ctx->state_mutex, ...);
snap = ctx->state;                          // copia tudo
tx_mutex_put(&ctx->state_mutex);
// → trabalha com 'snap' — imutável, na stack, não pode ficar dangling
```

**3 — `const` pointer em parâmetros:**
```c
static void handle_motor_cmd(SystemCtx* ctx,
                              const CAN_Message_t* rx_msg,
                              const VehicleState* snap)
// → compilador avisa se tentares libertar ou reatribuir rx_msg
```

**4 — Ponteiros de periféricos como `extern`:**
```c
extern SPI_HandleTypeDef hspi1;  // sempre válido — lifetime = programa inteiro
extern TIM_HandleTypeDef htim1;
// → nunca são libertados → nunca ficam dangling
```

---

# Resumo numa Tabela

| Conceito | Usam? | Onde / Como |
|---|---|---|
| `volatile` | ✅ | Flags entre ISR↔thread (`vlx_ready`, `emergency_stop_active`) |
| `static` local | ✅ | Contadores persistentes em `_step()` |
| `static` global de ficheiro | ✅ | Estado privado de cada task (`s_rx`, `s_srf`, `s_aeb`) |
| Variáveis atómicas | ❌ | Substituídas por mutexes ThreadX |
| Globais | ✅ | `g_ctx` via accessor `system_ctx()` |
| `const` | ✅ | Parâmetros AEB, ponteiros de entrada em funções |
| `constexpr` | ❌ | C puro — usam `#define` / `static const` |
| Herança/Polimorfismo | ❌ | C puro — simulado com struct + function pointers |
| Access specifiers | ❌ | C puro — simulado com `static` no `.c` |
| Getters/Setters | ✅ | `task_indicator_get/set_state()` com mutex interno |
| Pipes | ❌ | Não usados — comunicação via CAN/mutexes/shared state |
| ISR | ✅ | Speed sensor (TIM4 IC), VL53L5CX data ready |
| Polling | ✅ | MCP2515 CAN RX, SRF08 |
| Prioridade/Scheduler/Preemption | ✅ | `thread_config.h` — 10 threads, prio 10-16 |
| Dangling Pointer prevenção | ✅ | Sem malloc, padrão snapshot, `const` params, `extern` handles |

---

> **Ficheiros-chave para referência rápida:**
> - Arranque RTOS: `AZURE_RTOS/App/app_azure_rtos.c`
> - Criação das threads: `Core/Src/app_threadx.c`
> - Prioridades/stacks: `Core/Inc/thread_config.h`
> - Estado partilhado: `Core/Inc/system_ctx.h` + `Core/Inc/vehicle_state.h`
> - CAN TX: `Core/Src/can_tx.c` → `mcp_send_message()`

> **Outros Ficheiros para ler:*
* ThreadXGuide.md: prioridades/preempção/determinismo.
* ThreadX_Installation_Guide.md: sequência main.c -> MX_ThreadX_Init -> tx_kernel_enter.
* AGL_and_ThreadX_benefits_Guide.md: divisão AGL (alto nível) vs ThreadX (tempo real no STM32).






ENGLISH VERSION

﻿# STM32 ThreadX — Detailed Guide (Architecture + C Patterns)

> Note: This file contains two versions of the same documentation. The English translation appears first, followed by the original Portuguese version below. Both versions are kept for reference.

---

## Table of Contents

1. [Boot Sequence (chronological)](#boot-sequence-chronological)
2. [Steady-State Threads](#steady-state-threads)
3. [Synchronization Resources (Mutexes)](#synchronization-resources-mutexes)
4. [Data Flow (Sensor → CAN → RPi5)](#data-flow-sensor--can--rpi5)
5. [Priorities — Visualization](#priorities--visualization)
6. [Actual Execution Flow (detailed)](#actual-execution-flow-detailed)
7. [What Each Thread Does](#what-each-thread-does)
8. [Synchronization and Determinism (ThreadX in this project)](#synchronization-and-determinism-threadx-in-this-project)
9. [Important Code Notes](#important-code-notes)
10. [Bare‑Metal and HAL_Init Explained](#bare-metal-and-hal_init-explained)
11. [NVIC, PendSV and SysTick — Detailed Explanation](#nvic-pendsv-and-systick-detailed-explanation)
12. [C Patterns in the Project](#c-patterns-in-the-project)
13. [Concept Summary Table](#concept-summary-table)

---

## 1. Boot Sequence (chronological)

```
┌─────────────────────────────────────────────────────────────────────────────┐
│  POWER ON / RESET                                                           │
│                                                                             │
│  startup_stm32u585aiixq.s  ← Assembly: sets stack pointer, copies .data to  │
│                               RAM, zeroes .bss, then calls main()          │
└───────────────────────────────────┬─────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│  main()   [Core/Src/main.c]   ← BARE‑METAL (no RTOS yet)                    │
│                                                                             │
│  HAL_Init()           → configures SysTick (1 ms tick), NVIC, watchdog     │
│  SystemPower_Config() → SMPS regulator                                      │
│  SystemClock_Config() → PLL → 160 MHz                                       │
│                                                                             │
│  MX_GPIO_Init()       → GPIO pins                                            │
│  MX_I2C1/2_Init()     → sensors (SRF08, IMU, ToF, Battery, Indicator)       │
│  MX_SPI1/2_Init()     → MCP2515 (CAN), SPI2                                 │
│  MX_TIM1_Init()       → PWM servo (PA8, 50 Hz)                              │
│  MX_TIM4_Init()       → Input Capture for speed sensor (EXTI pulses)        │
│  MX_UART4/USART1()    → debug serial (printf → UART)                        │
│                                                                             │
│  HAL_TIM_IC_Start_IT() → start input-capture interrupt (speed sensor)       │
│  I2C scan (debug)                                                           │
│  matrix_init() / PWM servo → center position 90°                            │
│                                                                             │
│  MX_ThreadX_Init()    ← hand control to the RTOS (does not return)          │
└───────────────────────────────────┬─────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│  MX_ThreadX_Init()  [Core/Src/app_threadx.c]                               │
│                                                                             │
│  tx_kernel_enter()  ← ThreadX kernel starts                                 │
│       │                                                                    │
│       └─► tx_application_define()  [AZURE_RTOS/App/app_azure_rtos.c]       │
│               tx_byte_pool_create()  → static memory pool (20 KB)          │
│               App_ThreadX_Init()    ← create resources and threads         │
└───────────────────────────────────┬─────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│  App_ThreadX_Init()  [Core/Src/app_threadx.c]                              │
│                                                                             │
│  system_ctx_init()   → create global mutexes:                              │
│                         printf_mutex, spi1_mutex, i2c1_mutex,              │
│                         state_mutex, sys_mutex                              │
│                                                                             │
│  tx_thread_create() × 10  → create all threads (TX_AUTO_START)             │
│                                                                             │
│  Scheduler starts → executes highest‑priority ready thread                 │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 2. Steady‑State Threads

| # | Thread | File | Prio | Freq | CAN Frame sent | Purpose |
|---|--------|------|------|------|----------------|---------|
| 1 | HeartBeat | `task_heartbeat.c` | 10 | ~1 Hz | `CAN_ID_HEARTBEAT_STM32` | Init MCP2515; publish health, uptime, error flags |
| 2 | CAN RX | `task_can_rx.c` | 11 | ~100 Hz (10 ms) | `CAN_ID_MOTOR_STATUS` | Receive AGL commands → motor/servo/relay/indicators; enforce AEB/emergency |
| 3 | SRF08 | `task_srf08.c` | 11 | ~14 Hz (70 ms) | `CAN_ID_SRF08_DISTANCE` | Front ultrasonic; median filter; publish distance |
| 4 | Speed | `task_speed.c` | 12 | ~10 Hz | `CAN_ID_WHEEL_SPEED` | Count EXTI pulses → RPM; low‑pass filter; publish speed |
| 5 | IMU | `task_imu.c` | 13 | ~20 Hz | `CAN_ID_IMU_ACCEL/GYRO/MAG` | Read accel/gyro/mag; publish frames |
| 6 | ToF | `task_tof.c` | 14 | ~15 Hz (66 ms) | `CAN_ID_TOF_DISTANCE` | VL53L5CX 8×8 → min distance + nearest zone |
| 7 | Environment | `task_environment.c` | 15 | ~1 Hz | `CAN_ID_ENVIRONMENT` | LPS22HH + HTS221 + VEML6030 → temp/hum/pressure/light |
| 8 | AEB | `task_aeb.c` | 11 | 50 Hz (20 ms) | `CAN_ID_AEB_STOP` | State machine: OFF→ARMED→WARN→BRAKING→LATCHED; TTC + stopping distance |
| 9 | Battery | `task_battery.c` | 15 | ~0.5 Hz | `CAN_ID_BATTERY` | INA226 → voltage/current/SOC; under/over voltage flags |
|10 | Indicator | `task_indicator.c` | 16 | 20 Hz (50 ms) | (no CAN — I2C direct) | KS0064 LEDs: blink L/R, headlights, warning |

---

## 3. Synchronization Resources (Mutexes)

```
┌──────────────┬──────────────────────────────────────────────────────────────┐
│ Mutex        │ What it protects                                             │
├──────────────┼──────────────────────────────────────────────────────────────┤
│ spi1_mutex   │ SPI bus ↔ MCP2515 (ensure CAN tx/rx don't collide)           │
│ i2c1_mutex   │ I2C1 bus (SRF08, IMU, Battery, ToF XSHUT, Indicator)         │
│ state_mutex  │ VehicleState snapshot (read by AEB/CAN_RX, written by sensor │
│              │ tasks) — snapshot safety pattern                            │
│ sys_mutex    │ system_state, drive_mode, error_flags                        │
│ printf_mutex │ UART serial (avoid interleaved logs from threads)            │
└──────────────┴──────────────────────────────────────────────────────────────┘
```

---

## 4. Data Flow (Sensor → CAN → RPi5)

```
Speed Sensor (EXTI pulse)
       │ ISR counts pulses
       ▼
  [Speed Thread] ──────────────────────────────► CAN_ID_WHEEL_SPEED (0x3xx)
       │ state.speed_mh / rpm
       ▼
  [AEB Thread] ← reads state.speed_mh + state.srf08_distance_mm
       │ computes TTC / d_stop
       │ writes: aeb_stop_active, aeb_speed_limit, aeb_state
       ▼
  [CAN_RX Thread] ← receives MotorCmd from RPi5
       │ reads AEB flags → enforces throttle limit
       │ Motor_Forward/Backward/Stop + Servo_SetAngle
       ▼
  Motor TB6612FNG + Servo MG996R

SRF08 (I2C) ──► [SRF08 Thread] ──► state.srf08_distance_mm ──► AEB
VL53L5CX   ──► [ToF Thread]   ──► CAN_ID_TOF_DISTANCE
IMU        ──► [IMU Thread]   ──► CAN_ID_IMU_ACCEL/GYRO/MAG
INA226     ──► [Battery Thread] ► CAN_ID_BATTERY

All CAN frames ──► MCP2515 ──► MCP2518 Hat ──► RPi5 (AGL/KUKSA)
```

---

## 5. Priorities — Visualization

```
Priority  0 ────────────────────────────────── 31
         (most urgent)                 (least urgent)

 10  ■ HeartBeat        (system monitoring + CAN init)
 11  ■ CAN_RX           (actuation — motor/servo)
 11  ■ SRF08            (safety — frontal obstacle)
 11  ■ AEB              (safety — decision making)
 12  ■ Speed            (real‑time velocity)
 13  ■ IMU              (accel/gyro/mag)
 14  ■ ToF              (multi‑zone distance)
 15  ■ Environment      (temp/hum/pressure)
 15  ■ Battery          (voltage/current/SOC)
 16  ■ Indicator        (LEDs — background)
```

---

## Actual Execution Flow (order of execution)

### Bare‑Metal Boot First

In `src/stm/Core/Src/main.c` the code performs `HAL_Init`, clock/power setup, peripheral init (`GPIO`, `I2C`, `SPI`, `TIM`, `UART`), some startup checks (I2C scan, matrix, PWM), and then calls `MX_ThreadX_Init()`.

---

### Entering the ThreadX Kernel

`MX_ThreadX_Init()` (in `src/stm/Core/Src/app_threadx.c`) calls `tx_kernel_enter()`.

---

### Application Definition for ThreadX

The kernel enters `tx_application_define()` (`src/stm/AZURE_RTOS/App/app_azure_rtos.c`), creates the byte pool (`tx_byte_pool_create`) and calls `App_ThreadX_Init(...)`.

---

### Shared Context Initialization

`App_ThreadX_Init` starts with `system_ctx_init()`; in `src/stm/Core/Src/system_ctx.c` it creates mutexes (`spi1`, `i2c1`, `state`, `sys`, `printf`) and initializes the global state.

---

### Thread Creation (TX_AUTO_START)

`App_ThreadX_Init` creates threads in this order:
1. HeartBeat
2. CAN_RX
3. Temperature/Environment
4. Speed
5. IMU
6. ToF
7. SRF08
8. AEB
9. Battery
10. Indicator

---

### Scheduler Runs Continuously

There is no "end" — the system runs indefinitely with `while(1)` loops inside each thread.

---

## What Each Thread Does (and why it exists)

### HeartBeat
Initializes MCP2515 (`mcp_init`), sets system to `READY/RUNNING`, and sends periodic heartbeat CAN frames. Used for ECU liveness and health monitoring.

### CAN_RX
Receives CAN frames, validates, applies commands (motor/servo/relay/indicators), enforces safety (`emergency_stop` and AEB flags), and sends `MotorStatus`. Central actuator.

### Temperature/Environment
Reads LPS22HH/HTS221/VEML6030 and publishes environment CAN frames.

### Speed
Uses speed sensor pulses, computes RPM/velocity, updates `VehicleState`, sends `CAN_ID_WHEEL_SPEED`. Fundamental for control and AEB.

### IMU
Reads accel/gyro/mag, publishes IMU CAN frames, updates state snapshot for dynamics.

### ToF
Initializes VL53L5CX, reads min distance/zone, publishes CAN ToF, writes `tof_distance_mm` into shared state.

### SRF08
Measures front ultrasonic, applies median filter, publishes SRF08 CAN and updates `state.srf08_*`. Braking decision moved to AEB.

### AEB
State machine (`OFF/ARMED/WARN/BRAKING/LATCHED`), computes TTC/stopping distance and publishes `aeb_stop_active`, `aeb_warn`, `aeb_speed_limit` to `VehicleState`.

### Battery
Reads INA226, estimates SOC/status and publishes battery CAN frame.

### Indicator
Controls KS0064 (blink/headlights/warning) via I2C and a dedicated mutex.

---

## 8. Synchronization and Determinism (ThreadX in this project)

### Mutexes
- `spi1_mutex` prevents SPI bus corruption (shared MCP2515)
- `i2c1_mutex` serializes I2C sensors/actuators
- `state_mutex` protects the `VehicleState` snapshot

### Priorities
Configured in `src/stm/Core/Inc/thread_config.h` (lower numeric value = higher priority).

### Temporal Pacing
Each task uses `tx_thread_sleep(...)` with a fixed period (or an internal sleep in `task_*_step`) to keep rates stable and avoid busy loops.

---

## 9. Important Code Notes

- There are `thread_relay_entry` and `can_rx_queue` declared in `app_threadx.c`, but no `tx_queue_create` nor a relay thread active — appears legacy/inactive.
- The runtime steady state is the ThreadX scheduler with periodic threads and continuous CAN.

---

## 10. Bare‑Metal and HAL_Init Explained

### What is Bare‑Metal?

"Bare‑metal" means programming hardware directly without an operating system. No scheduler, no threads — just your code and MCU registers.

Analogy: Bare‑metal is like building a house by hand; HAL is the power tools; ThreadX is the architect who organizes parallel teams. Before the architect arrives, the site must be prepared.

```
┌───────────────────────────────────────────┐
│  Bare‑Metal                               │
│  Our code                                 │
│  ─────────────────────────────────────    │
│  Hardware (peripherals, registers, CPU)   │
└───────────────────────────────────────────┘

┌───────────────────────────────────────────┐
│  With RTOS (our case)                     │
│  Our code (tasks)                         │
│  ThreadX (scheduler, mutexes, timers)     │
│  HAL (STM32 peripheral abstraction)       │
│  Hardware                                 │
└───────────────────────────────────────────┘
```

In `main.c` the phase before `MX_ThreadX_Init()` is pure bare‑metal — hardware is initialized directly. After `tx_kernel_enter()` the system gains preemption, scheduling and concurrency.

### What does HAL_Init() do in detail?

HAL = Hardware Abstraction Layer — the ST library that wraps STM32 registers in portable C functions.

`HAL_Init()` performs four concrete tasks:

1) Configure SysTick (1 ms tick)

```c
// Internally, HAL_Init() calls:
HAL_InitTick(TICK_INT_PRIORITY);
// → configures SysTick to fire every 1 ms
// → increments uwTick: base for HAL_Delay()/HAL_GetTick()
```

The project uses `HAL_GetTick()` for timestamps (e.g., `s_rx.last_motor_status_tick`).

2) Configure the NVIC (interrupt controller)

```c
HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
// → 4 bits of priority, 0 bits of sub‑priority
```

This must be compatible with ThreadX's expectations (PendSV and SysTick priorities).

3) Initialize Flash prefetch/cache

```c
// Helps reduce instruction fetch latency at 160 MHz
```

4) Prepare HAL internal structures

```c
// Zero internal handles and set default error callbacks
```

### Why must main.c do all that before MX_ThreadX_Init()?

Because ThreadX expects hardware to be operational when the scheduler starts. If HeartBeat thread uses SPI before `MX_SPI1_Init()` was called — crash is likely.

Required order:
1. `HAL_Init()`
2. Clock config (CPU @ 160 MHz)
3. `MX_GPIO/SPI/I2C/TIM` peripheral init
4. `MX_ThreadX_Init()` — now threads may safely use peripherals

---

## 11. NVIC, PendSV and SysTick — Detailed Explanation

### Summary

- NVIC = ARM interrupt controller — determines which ISR runs by priority
- SysTick = kernel timer; ThreadX uses it as the scheduler clock (tx_thread_sleep counts SysTick ticks)
- PendSV = the context switch mechanism — runs last (lowest priority) so it never preempts hardware ISRs
- Compatibility: PendSV and SysTick must have lower priority than hardware ISRs or context switches could occur mid SPI/I2C transaction and corrupt data

---

### What is the NVIC?

NVIC (Nested Vectored Interrupt Controller) receives peripheral IRQs, prioritizes them, saves/restores CPU state and supports nested ISRs.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│  STM32U585AI — Cortex‑M33                                                   │
│                                                                             │
│  ┌──────────┐    IRQ line 0  ──► ┌─────────────────────────────────────┐  │
│  │  SPI1    │    IRQ line 1  ──► │          N V I C                    │  │
│  │  I2C1    │    IRQ line 2  ──► │  (compares priorities of all)       │  │
│  │  TIM4    │    IRQ line 3  ──► │  → schedules highest priority ISR   │  │
│  │  GPIO    │    ...          ──► │  → saves CPU context if needed      │  │
│  │  SysTick │    ─────────────►   └──────────────────┬──────────────────┘  │
│  │  PendSV  │                                        │                     │
│  └──────────┘                                        ▼                     │
│                                               CPU executes ISR            │
└─────────────────────────────────────────────────────────────────────────────┘
```

Nested ISRs are handled automatically by hardware.

---

### What is SysTick?

SysTick is a 24‑bit core timer used here as the ThreadX scheduler clock, typically configured for 1 ms ticks by HAL_Init(). The project uses TIM6 as a fallback HAL tick because ThreadX reserves SysTick.

---

### What is PendSV?

PendSV (Pendable Service Call) is a special, very low priority interrupt used to perform a context switch. ThreadX sets a pending flag and PendSV performs the register save/restore when no other ISRs are active.

---

### SysTick vs PendSV

SysTick decides scheduling events; PendSV performs the actual switch. This ensures context switches don't happen inside high‑priority hardware ISRs.

---

## 12. C Patterns in the Project

### `volatile`

Used for flags/variables updated by ISRs or different contexts, e.g. `volatile uint8_t emergency_stop_active` or `volatile uint8_t vlx_ready`. `volatile` prevents compiler caching optimizations and forces memory reads.

### `static` (file scope and function local)

Widely used for module‑private state, e.g. `static TaskCanRx s_rx;`. Inside functions `static` makes the variable persist between calls (e.g., logging counters).

### Atomics (`_Atomic`)

Not used directly — the project relies on ThreadX mutexes (`tx_mutex_get/put`) to protect shared structures. Atomics would be useful for single integer flags but not for complex read‑modify‑write sequences.

### Globals and Accessors

One global instance `static SystemCtx g_ctx;` is exposed via `SystemCtx* system_ctx(void) { return &g_ctx; }`. Access to shared state is protected by `state_mutex`.

### `const`

Used for immutable tuning parameters (placed in Flash), e.g. `static const AebParams P = { ... };`.

### C vs C++ features

No C++ features used — no inheritance/polymorphism. Encapsulation is achieved by `static` module variables and `init/step` function pairs.

### ISR vs Polling

Both are used: ISR for latency‑sensitive signals (speed sensor input capture, VL53L5CX data‑ready), polling for sensors without interrupts (SRF08, MCP2515 receive polling).

### Priority / Preemption

ThreadX is preemptive and priority‑based; higher priority threads preempt lower ones. The project uses TX_NO_TIME_SLICE (no round‑robin) so threads run until they sleep or block.

Example priorities (in `thread_config.h`):
```c
#define HEARTBEAT_THREAD_PRIORITY  10
#define CAN_RX_THREAD_PRIORITY     11
#define SRF08_THREAD_PRIORITY      11
#define AEB_THREAD_PRIORITY        11
#define SPEED_THREAD_PRIORITY      12
#define IMU_THREAD_PRIORITY        13
#define TOF_THREAD_PRIORITY        14
#define TEMP_THREAD_PRIORITY       15
#define INDICATOR_THREAD_PRIORITY  16
```

---

## 13. Concept Summary Table

See the Portuguese section below for the full original text and diagrams.

---

