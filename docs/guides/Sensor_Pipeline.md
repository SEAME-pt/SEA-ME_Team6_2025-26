  # Do Fio ao Dashboard — Pipeline Completo de um Sensor

  **Projeto SEA:ME | Team 6 — Formação Interna**

  Este documento explica o fluxo completo de dados de um sensor físico até à sua utilização em
  software.
  Usa o SRF08 (sensor ultrassónico de distância) como exemplo principal, mas o padrão aplica-se a
  qualquer sensor do sistema.

  Depois de ler este documento, qualquer membro da equipa deve conseguir:
  - Perceber como os dados fluem do sensor até ao dashboard
  - Adicionar um novo sensor ao sistema seguindo o mesmo padrão
  - Consumir dados de sensores existentes via CAN bus para HMI, datalogger, ou scripts

  ---

  ## 1. Visão Geral do Pipeline


  <img width="802" height="1024" alt="image" src="https://github.com/user-attachments/assets/74134b2f-24f4-4c1b-882a-98594e609550" />


  **Princípio-chave:** A interface de leitura do sensor varia (I2C, SPI, onboard), mas a partir do
  momento em que tens um valor numérico no STM32, o pipeline é sempre o mesmo:
  leitura → empacotar → CAN TX → CAN RX → consumo.

  ---

  ## 2. Hardware — Tipologias de Ligação

  Esta secção cobre as três formas de ligar um sensor ao STM32 B-U585I-IOT02A.

  ### 2.1 Sensor Externo via I2C (Caso Principal: SRF08)

  #### O que é I2C (resumo de 30 segundos)

  I2C é um protocolo de comunicação série que usa apenas dois fios:
  - **SDA** (Serial Data) — dados bidirecionais
  - **SCL** (Serial Clock) — clock gerado pelo master (STM32)

  Cada dispositivo no barramento tem um **endereço único** (7 bits). O STM32 (master) inicia a
  comunicação dizendo "quero falar com o dispositivo no endereço 0x70", e apenas esse dispositivo
  responde. Isto permite ter múltiplos sensores no mesmo par de fios.

  #### Ligação física do SRF08

  <img width="1024" height="371" alt="image" src="https://github.com/user-attachments/assets/ced7988e-09f1-45c8-83ae-d129d1d5af35" />


  > ⚠️  **Atenção ao barramento:** O SRF08 está em **I2C1** (hi2c1), partilhado com o INA226 (0x40)
  > e o motor driver Grove TB6612FNG (0x28). Os sensores onboard (IMU, pressão, luz) estão em
  > **I2C2** (PH4/PH5). Todos os acessos I2C1 são protegidos por `ctx->i2c1_mutex` (ThreadX).


  **Notas importantes:**
  - O SRF08 opera a **5V** mas as linhas I2C são tolerantes a 3.3V — funciona ligado diretamente
    ao STM32 (3.3V logic) sem level shifter
  - Endereço I2C de fábrica (7-bit): **0x70** → no HAL da ST usa-se o endereço 8-bit: **`0xE0`**
    (= `0x70 << 1`). O HAL espera o endereço em formato `addr[7:1]`, ou seja, shifted left.
    No código: `#define SRF08_DEFAULT_ADDR 0xE0`
  - O SRF08 também tem um sensor de luz (registo 0x01) — bónus grátis
  - Pull-ups em SDA e SCL: o SRF08 inclui pull-ups internos; se tiveres problemas de comunicação,
    verifica com osciloscópio

  #### Confirmar que o sensor está ligado (I2C scan)

  O seguinte padrão, extraído de `task_srf08_init()`, confirma a presença do sensor:

  ```c
  // Verifica se o dispositivo responde no endereço 0xE0
  tx_mutex_get(&ctx->i2c1_mutex, TX_WAIT_FOREVER);
  HAL_StatusTypeDef st = HAL_I2C_IsDeviceReady(&hi2c1, SRF08_DEFAULT_ADDR, 3, 100);
  tx_mutex_put(&ctx->i2c1_mutex);

  if (st != HAL_OK) {
      sys_log(ctx, "[SRF08] IsDeviceReady FAIL st=%d", st);
      return;
  }
  // Resultado esperado: st == HAL_OK → "Device found at 0xE0 (7-bit: 0x70)"
  ```

  Para um I2C scan genérico (varrer todos os endereços), o projeto tem a função
  I2C_Scan(&hi2c2, "I2C2") chamável com #define ENABLE_I2C_SCAN 1 em
  task_environment.c.

  ### 2.2 Sensor Externo via SPI

  #### O que é SPI (resumo de 30 segundos)

  SPI é outro protocolo série, mais rápido que I2C. Usa 4 fios:
  - MOSI (Master Out, Slave In) — dados do STM32 para o dispositivo
  - MISO (Master In, Slave Out) — dados do dispositivo para o STM32
  - SCK (Serial Clock) — clock gerado pelo master
  - CS/SS (Chip Select) — o STM32 põe LOW para selecionar com qual dispositivo quer falar

  A diferença prática para I2C:
  - Sem endereços — em vez disso, cada dispositivo tem o seu próprio fio CS
  - Full-duplex — envia e recebe dados ao mesmo tempo
  - Mais rápido — mas usa mais pinos

  Exemplo no projeto: MCP2515 (CAN Controller)

  O controlador CAN do sistema é um MCP2515 (não MCP2518FD), ligado via SPI1:

  <img width="1024" height="371" alt="image" src="https://github.com/user-attachments/assets/571ac988-ddfc-4d56-a0f5-836eb7090716" />


  Configuração SPI1 no código (spi.c):
  - Mode: MASTER, 2LINES, 8-bit, CPOL=LOW, CPHA=1EDGE (SPI Mode 0)
  - NSS: Software (CS controlado por GPIO manualmente)
  - BaudRate: SYSCLK/16
  - FirstBit: MSB

  Diferenças-chave vs I2C:
  - No código: HAL_SPI_Transmit() / HAL_SPI_Transmit() em vez de HAL_I2C_Mem_Read/Write()
  - CS é controlado manualmente: MCP_CS_L() / MCP_CS_H() antes/depois de cada transação
  - Protegido por ctx->spi1_mutex
  - O resto do pipeline (empacotar → CAN → consumo) é exactamente igual

  ### 2.3 Sensores Onboard do B-U585I-IOT02A

  Tabela de Mapeamento de Sensores e Endereços

| Sensor | Função | HAL Addr (8-bit) | 7-bit Addr | CAN ID |
| :--- | :--- | :--- | :--- | :--- |
| **ISM330DHCX** | Acelerómetro + Giroscópio | `0xD6 (0x6B << 1, SDO=VCC)` | `0x6B` | `0x400/0x401` |
| **IIS2MDC** | Magnetómetro | `0x3C` | `0x1E` | `0x402` |
| **LPS22HH** | Pressão barométrica + Temperatura | `0xBA (0x5D << 1)` | `0x5D` | `0x420` |
| **HTS221** | Temperatura + Humidade | `0xBE` | `0x5F` | `0x420` |
| **VEML6030** | Luz ambiente (lux) | `0x20 (0x10 << 1)` | `0x10` | `0x420` |
| **VL53L5CX** | ToF 8×8 (distância multi-zona) | `0x52` | `0x29` | `0x422` |


  Diferenças em relação a sensores externos

  A ligação física já está feita no PCB. A abordagem de software é HAL direto em todos os casos:
  HAL_I2C_Mem_Read(&hi2c2, addr, reg, ...) — mesmo padrão dos sensores externos, bus diferente.

  Drivers implementados:
  - ism330dhcx.c/h — acelerómetro + giroscópio via hi2c2
  - iis2mdc.c/h — magnetómetro via hi2c2
  - lps22hh.c/h — pressão + temperatura via hi2c2
  - hts221.c/h — humidade via hi2c2
  - veml6030.c/h — luz ambiente via hi2c2
  - vl53l5cx_driver.c/h + ULD library — ToF via hi2c2, interrupt-driven

  O rest do pipeline é igual: leitura → empacotar CAN → enviar → consumo.

  ---
  ### 3. STM32 — Detectar e Ler o Sensor

  Esta secção usa o SRF08 como walkthrough completo.

  #### 3.1 Configuração no CubeMX

  I2C1 (para SRF08, INA226, motor driver):
  1. Periférico: I2C1
  2. Pinos: PB8 → I2C1_SCL, PB9 → I2C1_SDA
  3. Speed: Standard Mode (timing 0x30909DEC no código)
  4. Analog filter: ENABLE, Digital filter: 0
  5. Verificar que clock GPIOB e I2C1 estão activos (gerado automaticamente pelo CubeMX)

  ▎ O SRF08 não suporta Fast Mode (400kHz). Standard Mode (100kHz) é obrigatório.

  #### 3.2 I2C Scan — Confirmar Detecção

  Usar HAL_I2C_IsDeviceReady() para confirmar que o sensor está no barramento:

  // Sempre proteger com mutex em ambiente ThreadX
  tx_mutex_get(&ctx->i2c1_mutex, TX_WAIT_FOREVER);
  HAL_StatusTypeDef st = HAL_I2C_IsDeviceReady(&hi2c1, SRF08_DEFAULT_ADDR, 3, 100);
  //                                                    ^^^^^^^^^^^^^^^^
  //                                                    0xE0 (= 0x70 << 1)
  tx_mutex_put(&ctx->i2c1_mutex);

  if (st == HAL_OK) {
      // Sensor encontrado em 0x70 (7-bit) / 0xE0 (HAL)
  } else {
      // Verificar ligações físicas, pull-ups, alimentação 5V
  }

  Para scan completo (debug), o projeto inclui I2C_Scan(&hi2c1, "I2C1") —
  activar com #define ENABLE_I2C_SCAN 1.

  #### 3.3 Como o SRF08 Funciona

  O SRF08 mede distância por ultrassons em dois passos:

  1. Trigger: Escrevemos 0x51 (ranging em cm) no registo 0x00
  2. Esperar: O sensor emite um pulso ultrassónico e mede o tempo do eco (~65ms máx.)
  3. Ler: A distância em cm está nos registos 0x02 (byte alto) e 0x03 (byte baixo) como uint16

 Registos do SRF08:

| Registo | Acesso | Conteúdo |
| :--- | :--- | :--- |
| `0x00` | W / R | Escrita: comando \| Leitura: SW version |
| `0x01` | W / R | Escrita: gain (0-31) \| Leitura: luz |
| `0x02` | W / R | Escrita: range max \| Leitura: dist Hi |
| `0x03` | R | Leitura: distância byte baixo (Lo) |

  Distância (cm) = (reg_0x02 << 8) | reg_0x03
  Range físico: 3 cm (dead zone) — 600 cm (~6m com range=140)

  Conversão para CAN frame: distance_mm = distance_cm * 10

  Parâmetros usados no projeto (srf08.h):
  #define SRF08_DEFAULT_ADDR        0xE0   // 8-bit HAL addr (7-bit: 0x70)
  #define SRF08_RECOMMENDED_GAIN    6      // Baixo: reduz falsos ecos do chão/paredes
  #define SRF08_RECOMMENDED_RANGE   140    // 140 × 43mm ≈ 6m
  #define SRF08_CMD_RANGE_CM        0x51   // Trigger ranging em centímetros

  #### 3.4 Leitura Periódica (ThreadX Task)

  Código real de task_srf08.c — função task_srf08_step():

  void task_srf08_step(SystemCtx* ctx)
  {
      // 1) Trigger: escreve 0x51 no registo 0x00
      HAL_StatusTypeDef ranging_status = SRF08_StartRanging(&s_srf.hsrf08, ctx);
      // Internamente: HAL_I2C_Mem_Write(hi2c, 0xE0, SRF08_REG_COMMAND, 1, &cmd_0x51, 1, 100)

      // 2) Esperar medição completar — sleep fixo 70 ticks (≈ 70ms)
      //    NOTA: SRF08_DISABLE_POLLING = 1 (sensor não suporta polling do CMD register)
      //    Usar tx_thread_sleep(), NUNCA HAL_Delay() — bloqueia o ThreadX scheduler
      tx_thread_sleep(70);  // SRF08_MEAS_WAIT_TICKS

      // 3) Ler distância: registos 0x02 e 0x03 (big-endian no sensor, 2 bytes)
      uint16_t distance_cm = SRF08_GetDistanceCm(&s_srf.hsrf08, ctx);
      // Internamente: HAL_I2C_Mem_Read(hi2c, 0xE0, SRF08_REG_RANGE_H, 1, data, 2, 100)
      // Retorna: (data[0] << 8) | data[1]

      // 4) Ler luz ambiente: registo 0x01
      uint8_t light = SRF08_GetLight(&s_srf.hsrf08, ctx);
      // Internamente: HAL_I2C_Mem_Read(hi2c, 0xE0, SRF08_REG_LIGHT, 1, &light, 1, 100)

      // 5) Converter cm → mm (unidade do frame CAN)
      uint16_t distance_mm_raw = (distance_cm == 0xFFFF) ? 0 : (uint16_t)(distance_cm * 10u);

      // 6) Filtro mediana (3 amostras, insertion sort) — rejeita spikes
      uint16_t distance_mm = srf08_apply_filter(distance_mm_raw, light);

      // 7) Enviar CAN frame 0x423 (SRF08Distance_t, 8 bytes)
      //    Condicional: só envia quando light > 0 (confirma que sensor leu algo)
      if (light > 0) {
          SRF08Distance_t srf08_frame;
          srf08_frame.distance_mm   = distance_mm;   // uint16, mm
          srf08_frame.light_level   = light;          // uint8, raw 0-255
          srf08_frame.gain          = 0;              // não preenchido
          srf08_frame.range_setting = 0;              // não preenchido
          srf08_frame.reserved[0]   = 0;
          srf08_frame.reserved[1]   = 0;
          srf08_frame.status        = 0x01;           // bit 0: valid reading

          mcp_send_message(CAN_ID_SRF08_DISTANCE,    // 0x423
                           (uint8_t*)&srf08_frame,
                           sizeof(srf08_frame));      // 8 bytes
      }

      // 8) Actualizar estado partilhado (para AEB task ler)
      tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
      ctx->state.srf08_distance_mm = distance_mm;
      ctx->state.srf08_light       = light;
      ctx->state.srf08_ts          = tx_time_get();
      ctx->state.srf08_valid       = (distance_cm != 0xFFFF) ? 1 : 0;
      tx_mutex_put(&ctx->state_mutex);

      // Frequência efectiva: ~14 Hz (limitada pelos 70ms de medição)
      // Não há sleep adicional — loop imediato após leitura
  }

  Thread info (thread_config.h):
  - Prioridade: 11 (SRF08_THREAD_PRIORITY) — mesma prioridade que CAN_RX (safety-critical)
  - Stack: 2048 bytes
  - Agendamento: TX_NO_TIME_SLICE (FIFO dentro da mesma prioridade)

  #### 3.5 Lições Aprendidas

  - Tempo de ranging: O SRF08 precisa de ~65ms. Se leres antes, apanhas o valor anterior.
  Usa tx_thread_sleep(70), nunca HAL_Delay() (bloqueia o ThreadX scheduler).
  - Polling desativado: O SRF08 deste projeto não suporta polling do registo CMD (fica em
  0x0B em vez de 0x00 quando termina). SRF08_DISABLE_POLLING = 1 usa sleep fixo.
  - Endereço I2C: O HAL da ST usa endereço 8-bit → sempre 0xE0 no código, não 0x70.
  - Unidade CAN: O frame CAN envia em mm, não cm. distance_mm = distance_cm * 10.
  - Filtro mediana: 3 amostras (insertion sort). Latência: ~214ms até estabilizar.
  Rejeita melhor spikes do que média simples.
  - Mutex obrigatório: Todos os acessos I2C1 levam ctx->i2c1_mutex — há outras tasks
  (Battery, MotorDriver) a partilhar o mesmo bus.
  - AEB não é responsabilidade do SRF08 task: O task_aeb.c lê ctx->state.srf08_distance_mm
  e decide autonomamente sobre travagem. O SRF08 task só fornece dados.

  ---
  ### 4. STM32 → CAN TX — Empacotar e Enviar

  #### 4.1 Estrutura do CAN Frame

  Cada frame CAN tem:
  - CAN ID (11 bits standard, SFF) — identifica o tipo de mensagem
  - DLC (0-8 bytes) — quantidade de dados
  - Data (até 8 bytes) — os valores empacotados em struct C __attribute__((packed))

  CAN ID do SRF08 no projeto

  // can_id.h
  #define CAN_ID_SRF08_DISTANCE   0x423

  // Frame: SRF08Distance_t (8 bytes, packed, little-endian)
  typedef struct __attribute__((packed)) {
      uint16_t distance_mm;    // Bytes 0-1: distância em mm (0–6000)
      uint8_t  light_level;    // Byte  2:   luz ambiente (0–255, raw)
      uint8_t  gain;           // Byte  3:   gain activo (0 = não preenchido)
      uint8_t  range_setting;  // Byte  4:   range activo (0 = não preenchido)
      uint8_t  reserved[2];    // Bytes 5-6: reservado
      uint8_t  status;         // Byte  7:   bit0=valid, bit1=init_error
  } SRF08Distance_t;

  // DLC = 8 bytes
  // Encoding: little-endian (Cortex-M, packed struct)
  // Condição de envio: só quando light_level > 0

  #### 4.2 Porquê este encoding

  - Milímetros: Maior precisão que cm, sem scaling — inteiro direto.
  - Little-endian: Convenção natural do Cortex-M33 com structs packed. Diferente de algumas
  convenções CAN automotive (big-endian) — atenção ao desempacotar no RPi5.
  - 8 bytes (frame completo): sizeof(SRF08Distance_t) = 8 — preenche o DLC máximo.
  - Luz incluída: Grátis no registo 0x01, incluída no byte 2.
  - Status byte: Permite ao receptor saber se a leitura é válida sem valor sentinel.

  #### 4.3 Código de Envio (mcp_send_message)

  Toda a transmissão CAN passa por mcp_send_message() em can_tx.c:

  void mcp_send_message(uint16_t id, uint8_t *data, uint8_t len)
  {
      uint8_t buf[6 + 8];

      // Cabeçalho MCP2515: Load TX Buffer command (0x40)
      buf[0] = MCP_LOAD_TX;               // 0x40
      buf[1] = (id >> 3) & 0xFF;          // SIDH: bits [10:3] do CAN ID
      buf[2] = (id & 0x07) << 5;          // SIDL: bits [2:0] nos bits [7:5]
      buf[3] = 0x00;                      // EID8 (extended ID, não usado)
      buf[4] = 0x00;                      // EID0 (extended ID, não usado)
      buf[5] = len & 0x0F;                // DLC

      for (uint8_t i = 0; i < len; i++)
          buf[6 + i] = data[i];

      // Envio via SPI1 com mutex
      tx_mutex_get(&ctx->spi1_mutex, TX_WAIT_FOREVER);

      MCP_CS_L();                                           // CS LOW
      HAL_SPI_Transmit(&hspi1, buf, 6 + len, HAL_MAX_DELAY);
      MCP_CS_H();                                           // CS HIGH

      uint8_t cmd = MCP_RTS_TXB0;                          // 0x81: Request To Send
      MCP_CS_L();
      HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
      MCP_CS_H();

      tx_mutex_put(&ctx->spi1_mutex);
  }

  
  ---
  
  ### 5. RPi5 CAN RX → VSS Bridge — Receber e Traduzir

  ▎ Estado: Esta camada ainda não está implementada neste codebase (Março 2026).
  ▎ O que existe actualmente são scripts Python directos (python-can) para testes e controlo manual.
  ▎ Esta secção descreve a arquitectura planeada.

  #### 5.1 O que será o CAN-VSS Bridge

  O bridge será um script Python no RPi5 (AGL) a traduzir entre:
  - CAN (frames binários com IDs numéricos, little-endian)
  - VSS (Vehicle Signal Specification, paths legíveis → Kuksa databroker)

  CAN Frame (0x423, 8 bytes LE)
      b[0..1]: 0x2D 0x01 → distance_mm = 0x012D = 301 mm
      b[2]:    0x8A      → light_level = 138
      │
      │ struct.unpack_from('<H', data, 0)  ← little-endian uint16
      ▼
      distance_mm = 301
      │
      │ kuksa-client gRPC
      ▼
  Kuksa.val: Vehicle.ADAS.ObstacleDetection.DistanceFront = 0.301  (metros)

  ▎ ⚠️  Atenção ao endianness: Os frames CAN do projecto são little-endian (structs C do
  ▎ Cortex-M). Usar struct.unpack_from('<H', ...) no Python (não >H).

  #### 5.2 Configuração do SocketCAN no AGL

  ##### Verificar que a interface CAN está ativa
  ip link show can0

  ##### Se não estiver configurada:
  ip link set can0 type can bitrate 500000
  ip link set can0 up

  ##### Testar receção (debug):
  candump can0

  ##### Filtrar apenas SRF08:
  candump can0,423:7FF

  #### 5.3 Estrutura do Bridge (a implementar)

  import can
  import struct

  # Mapeamento CAN ID → função de desempacotamento
  # ATENÇÃO: little-endian ('<' no struct)

  def unpack_srf08(data):
      distance_mm, light = struct.unpack_from('<HB', data, 0)
      return {
          'Vehicle.ADAS.ObstacleDetection.DistanceFront': distance_mm / 1000.0,  # metros
          'Vehicle.ADAS.ObstacleDetection.AmbientLight': light,
      }

  def unpack_wheel_speed(data):
      rpm, total_pulses, direction, status = struct.unpack_from('<iIBB', data, 0)
      speed_kmh = abs(rpm) * (66.75e-3 * 3.14159) * 60 / 1000  # aprox.
      return {
          'Vehicle.Speed': speed_kmh,
          'Vehicle.Powertrain.Transmission.CurrentGear': direction,
      }

  def unpack_battery(data):
      voltage_mv, current_ma, soc = struct.unpack_from('<Hhb', data, 0)
      return {
          'Vehicle.Powertrain.Battery.Voltage': voltage_mv / 1000.0,
          'Vehicle.Powertrain.Battery.CurrentCurrent': current_ma / 1000.0,
          'Vehicle.Powertrain.Battery.StateOfCharge.Current': soc,
      }

  CAN_HANDLERS = {
      0x423: unpack_srf08,
      0x403: unpack_wheel_speed,
      0x421: unpack_battery,
      # ... adicionar os restantes
  }

  bus = can.interface.Bus(channel='can0', bustype='socketcan')
  for msg in bus:
      handler = CAN_HANDLERS.get(msg.arbitration_id)
      if handler:
          values = handler(msg.data)
          # publicar valores no Kuksa via gRPC
          # client.set(path, value)

  #### 5.4 Mapping CAN → VSS (planeado)

  ┌────────┬────────┬─────────┬──────────────────────────────────────────────────┬───────────────┐
  │ CAN ID │ Bytes  │ Tipo LE │                     VSS Path                     │    Unidade    │
  ├────────┼────────┼─────────┼──────────────────────────────────────────────────┼───────────────┤
  │ 0x423  │ uint16 │ mm      │ Vehicle.ADAS.ObstacleDetection.DistanceFront     │ metros        │
  │ B0-1   │        │         │                                                  │ (÷1000)       │
  ├────────┼────────┼─────────┼──────────────────────────────────────────────────┼───────────────┤
  │ 0x423  │ uint8  │ raw     │ Vehicle.ADAS.ObstacleDetection.AmbientLight      │ raw 0-255     │
  │ B2     │        │         │                                                  │               │
  ├────────┼────────┼─────────┼──────────────────────────────────────────────────┼───────────────┤
  │ 0x421  │ uint16 │ mV      │ Vehicle.Powertrain.Battery.Voltage               │ V (÷1000)     │
  │ B0-1   │        │         │                                                  │               │
  ├────────┼────────┼─────────┼──────────────────────────────────────────────────┼───────────────┤
  │ 0x421  │ int16  │ mA      │ Vehicle.Powertrain.Battery.CurrentCurrent        │ A (÷1000)     │
  │ B2-3   │        │         │                                                  │               │
  ├────────┼────────┼─────────┼──────────────────────────────────────────────────┼───────────────┤
  │ 0x421  │ uint8  │ %       │ Vehicle.Powertrain.Battery.StateOfCharge.Current │ %             │
  │ B4     │        │         │                                                  │               │
  ├────────┼────────┼─────────┼──────────────────────────────────────────────────┼───────────────┤
  │ 0x403  │ int16  │ RPM     │ Vehicle.Powertrain.Transmission.CurrentGear      │ RPM           │
  │ B0-1   │        │         │ (proxy)                                          │               │
  ├────────┼────────┼─────────┼──────────────────────────────────────────────────┼───────────────┤
  │ 0x002  │ uint8  │ bool    │ Vehicle.ADAS.AEB.IsActive                        │ bool          │
  │ B0     │        │         │                                                  │               │
  ├────────┼────────┼─────────┼──────────────────────────────────────────────────┼───────────────┤
  │ 0x400  │ int16  │ milli-g │ Vehicle.Chassis.Accelerometer.Longitudinal       │ m/s²          │
  │ B0-1   │        │         │                                                  │ (×9.81/1000)  │
  ├────────┼────────┼─────────┼──────────────────────────────────────────────────┼───────────────┤
  │ 0x420  │ int16  │ 0.01°C  │ Vehicle.Exterior.AirTemperature                  │ °C (÷100)     │
  │ B0-1   │        │         │                                                  │               │
  └────────┴────────┴─────────┴──────────────────────────────────────────────────┴───────────────┘

  ---
  ### 6. Kuksa — Consumir os Dados

  ▎ Estado: Kuksa.val databroker não está instalado/configurado neste codebase.
  ▎ Esta secção descreve a arquitectura alvo.

  #### 6.1 Ligação ao Databroker (quando implementado)

  Host: <IP do RPi5>    (ex: 10.21.220.191)
  Port: 55555
  Protocolo: gRPC
  TLS: sim (certificado CA necessário)

  #### 6.2 Leitura Pontual (Python)

  from pathlib import Path
  from kuksa_client.grpc import VSSClient

  with VSSClient('10.21.220.191', 55555,
                 root_certificates=Path('ca.pem')) as client:
      values = client.get_current_values([
          'Vehicle.ADAS.ObstacleDetection.DistanceFront'
      ])
      distance_m = values['Vehicle.ADAS.ObstacleDetection.DistanceFront'].value
      print(f"Distance: {distance_m * 1000:.0f} mm")

  #### 6.3 Subscription — Atualizações em Tempo Real

  with VSSClient('10.21.220.191', 55555,
                 root_certificates=Path('ca.pem')) as client:
      for updates in client.subscribe_current_values([
          'Vehicle.ADAS.ObstacleDetection.DistanceFront'
      ]):
          for path, datapoint in updates.items():
              print(f"{path} = {datapoint.value}")

  #### 6.4 Teste Rápido com CLI

  # No RPi5, se kuksa-client estiver instalado:
  kuksa-client --ip 127.0.0.1 --port 55555 --protocol grpc --cacert /path/to/ca.pem

  # Dentro do client:
  getValue Vehicle.ADAS.ObstacleDetection.DistanceFront
  subscribe Vehicle.ADAS.ObstacleDetection.DistanceFront

  ---
  ### 7. Receita — Adicionar um Novo Sensor

  Checklist para integrar qualquer sensor novo no sistema:

  Hardware:
  - Ligar o sensor ao STM32 (I2C1 para sensores externos; I2C2 para onboard)
  - Confirmar alimentação (3.3V vs 5V) e pull-ups

  STM32:
  - Configurar periférico no CubeMX se necessário (já gerado para I2C1 e I2C2)
  - Confirmar detecção: HAL_I2C_IsDeviceReady() com ctx->i2c1_mutex
  - Escrever driver: sensor_init() + sensor_read() (HAL_I2C_Mem_Write/Read)
  - Criar ThreadX task em Core/Src/tasks/task_<sensor>.c
  - Definir prioridade em thread_config.h
  - Nunca usar HAL_Delay() em tasks — usar tx_thread_sleep()
  - Registar thread em app_threadx.c (App_ThreadX_Init)

  CAN:
  - Escolher CAN ID (ver tabela secção 4.4, respeitar prioridades; max 0x7FF)
  - Adicionar #define CAN_ID_<SENSOR> 0xXXX em can_id.h
  - Definir struct __attribute__((packed)) para o frame (sempre little-endian)
  - Enviar com mcp_send_message(CAN_ID_<SENSOR>, (uint8_t*)&frame, sizeof(frame))

  RPi5 / AGL:
  - Verificar receção: candump can0 | grep <CAN_ID_hex>
  - Adicionar handler em CAN-VSS bridge (desempacotar com struct.unpack_from('<...', data, 0))
  - Notar: little-endian ('<' no struct format, não '>')

  Kuksa (quando implementado):
  - Escolher VSS path (seguir standard COVESA VSS)
  - Publicar no Kuksa (client.set())
  - Testar end-to-end — valor no Kuksa CLI bate certo com medição real

  Documentação:
  - Atualizar tabela de CAN IDs (secção 4.4)
  - Atualizar mapping VSS (secção 5.4)
  - Atualizar tabela de threads se nova task criada

  ---
 #### Referência Rápida

  
  Buses I2C

  ┌───────────┬─────────────┬───────────────────────────────────────────────────────────────────┐
  │    Bus    │ Pinos STM32 │                           Dispositivos                            │
  ├───────────┼─────────────┼───────────────────────────────────────────────────────────────────┤
  │ I2C1      │ PB8=SCL,    │ SRF08 (0x70/0xE0), INA226 (0x40), Grove TB6612FNG (0x28)          │
  │ (hi2c1)   │ PB9=SDA     │                                                                   │
  ├───────────┼─────────────┼───────────────────────────────────────────────────────────────────┤
  │ I2C2      │ PH4=SCL,    │ ISM330DHCX (0x6B/0xD6), IIS2MDC (0x1E/0x3C), LPS22HH (0x5D/0xBA), │
  │ (hi2c2)   │ PH5=SDA     │  HTS221 (0x5F/0xBE), VEML6030 (0x10/0x20), VL53L5CX (0x29/0x52)   │
  └───────────┴─────────────┴───────────────────────────────────────────────────────────────────┘

  ▎ Endereços mostrados como 7-bit / 8-bit HAL (HAL = 7-bit << 1).
  ▎ Todos os acessos ao mesmo bus usam o mesmo mutex (i2c1_mutex / por extensão hi2c2).


