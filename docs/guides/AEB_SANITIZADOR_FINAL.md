# AEB — Sanitização do SRF08: problema, implementação e validação

**Team 6 SEA:ME · fechado a 18 jul 2026**
Código: `team6/Core/Src/tasks/task_aeb.c` · Pacote de flash/validação: `/data/stm32/aeb-validation/` (no carro)

---

## 1. Resumo

O carro parava sozinho na pista por disparos falsos do AEB, mesmo sem
nada à frente. A causa era um **eco fantasma do chão** que o SRF08 devolve
a ~530 mm e que o firmware tratava como obstáculo real. A solução foi um
**sanitizador de distância** em software (não confiar em leituras dentro da
banda do eco, com regras para não perder obstáculos reais), flashado e
validado em bancada a 18 jul: o AEB passou de "obstáculo permanente a
0,5 m" para "via livre", mantendo todas as taxas de frames normais.

| | Antes (fw 8 jul) | Depois (fw 14 jul) |
|---|---|---|
| SRF08 cru (parado, chão livre) | ~570-800 mm | ~570-800 mm (igual — é o eco) |
| d_eff visto pelo AEB (0x002) | **cru − 100 mm** (≈470-700) | **5900 mm** (= longe) |
| Disparos falsos (replay sessão 14 jul, 23 min) | 66 | ~4 |
| Disparos reais (mesma sessão) | 18 | 18 (mantidos) |

## 2. O problema

- O SRF08 está montado a ~110 mm do chão com ~5° de inclinação; o cone do
  ultrassom apanha o chão e devolve um eco persistente: **~380-780 mm
  parado, ~300-880 mm em movimento** (o pitch do carro faz a leitura
  oscilar). Confirmado ao vivo por candump: cru 570 mm num dia, 800 mm
  noutro — deriva dentro da banda.
- Baixar o gain **não** suprime o eco (testado com gain=4 a 14 jul) e
  custa alcance útil.
- Com o firmware antigo, o AEB via `d_eff = cru − 100 mm` ≈ 0,4-0,7 m
  permanente. Em andamento isso significava: ARMED→WARN imediato, e nos
  mergulhos do eco (<~370 mm) entrava em BRAKING; o speed-limit
  cinemático caía abaixo de 20% → `Motor_Stop()`; 600 ms parado →
  LATCHED. Resultado: **o carro parava na pista sem obstáculo** e voltava
  a andar após o unlatch (0,5-5 s).
- Agravante do código antigo: leitura 0 ("sem eco") era tratada como
  ~2 cm → travagem instantânea com uma única amostra nula.
- Sessão de 14 jul (23 min): **66 disparos falsos**, 18 reais.

## 3. Diagnóstico (evidência)

1. `candump can1`: 0x423 (SRF08) cru ~570 mm **e ao mesmo tempo** 0x002
   (AEB) `d_eff = 470 mm` = cru − 100 → o AEB confiava no eco.
2. Dump da flash via openocd (ST-Link V3 no Pi): md5 **igual** ao
   `/root/team6.bin` de 8 jul → o firmware no carro era anterior ao
   sanitizador (escrito e compilado a 14 jul, nunca flashado).
3. Diff entre os dois binários: strings idênticas; única fonte alterada
   desde 8 jul = `task_aeb.c` → o flash só mudaria o módulo AEB.

## 4. O que foi implementado — o sanitizador

Camada entre o SRF08 e o AEB (`san_update()` em `task_aeb.c`), que corre
por amostra fresca do sensor (~70-100 ms), não no tick de 20 ms do AEB:

**Regras:**
1. **Fora da banda** [250, 900] mm → leitura confiável, aceite após
   **2 amostras consecutivas** (debounce contra spikes).
2. **Leitura 0 = "sem eco" = longe** (6000 mm) — em vez dos 2 cm antigos.
3. **Dentro da banda** [250, 900] mm → **não confiada por defeito**
   (é a zona do eco fantasma) → o AEB vê "longe". Exceções:
   - **Aproximação real vinda de fora**: se vínhamos a seguir um objeto
     logo acima da banda (900-1300 mm, há ≤0,5 s), continua-se a
     aproximação por **odometria** (`d_est −= v·dt`) enquanto as leituras
     acompanharem a previsão (±150 mm). Um objeto real emerge abaixo da
     banda antes de `d_est` chegar ao fundo; se não emergir ou a previsão
     divergir, era o eco → longe.
   - **Backstop**: abaixo de 250 mm é sempre confiável — um obstáculo
     mesmo em cima do sensor nunca é ignorado.

**Constantes** (em `task_aeb.c`): `SAN_BAND_LO=250` `SAN_BAND_HI=900`
`SAN_ENTRY_MAX=1300` `SAN_ENTRY_FRESH=500ms` `SAN_CONSIST=±150mm`
`SAN_FAR=6000` `SAN_DEBOUNCE=2`.

**Retune dos parâmetros AEB** (mesmo commit): `ttc_brake` 0,3→0,5 s,
margens warn/brake 0,05/0,02→0,30/0,10 m, `stop_hold` 300→600 ms —
mais margem física, menos LATCH por abrandamento momentâneo.

**O que NÃO mudou**: máquina de estados do AEB (OFF/ARMED/WARN/BRAKING/
LATCHED), curva cinemática `v=√(2·0,6·d_eff)` com `d_limit=2,5 m`,
atuação em `task_can_rx.c`, frame 0x002, caminho CAN/MCP2515, joystick,
cruise control — binários com strings idênticas, +728 bytes de código.

## 5. Validação

**a) Replay offline (14 jul)** — sessão de 23 min re-processada com o
sanitizador: 66 → ~4 disparos falsos; os 18 reais e os tempos de
travagem em aproximações sintéticas (0,4-1,2 m/s) mantidos.

**b) Pré-flash (17 jul)** — `aeb_validate.py --test parado` no carro:
taxas todas PASS; sanitizador FAIL **como esperado** (d_eff=700 = cru
800 − 100) → o validador deteta corretamente o firmware antigo.
Baseline de 20 s de CAN guardado (`baseline_preflash.log`).

**c) Pós-flash (18 jul)** — flash do `team6_jul14_sanitizer.bin`
(md5 `35fffde8...`) via openocd; `aeb_validate.py --test parado`:

```
Taxas: 0x002=46.9Hz 0x423=14.1Hz 0x213=9.0Hz 0x701=10.0Hz
SRF08 cru (mediana): 800 mm | d_eff AEB (mediana): 5900 mm
✅ TUDO PASS (7/7)
```

O sensor continua a ver o eco (800 mm) mas o AEB reporta 5900 mm — o
fantasma deixou de existir para a lógica de travagem, sem regressão nas
taxas de frames.

**d) Pendente:**
- [ ] Bancada: `--test obstaculo` com caixa a ~1,2-1,5 m (d_eff ≈ cru−100)
- [ ] Bancada: aproximar caixa até <30 cm → limit desce, estados sobem
- [ ] Pista: reta livre sem paragens espontâneas
      (`candump can1 | grep 0x002 | grep -v "00 00 00 64"`)
- [ ] Pista: travagem real com obstáculo → BRAKING→LATCHED→unlatch

## 6. Flash, rollback e ficheiros

Tudo em `/data/stm32/aeb-validation/` no carro (detalhe no
`FLASH_E_VALIDACAO_AEB.md`):

| Ficheiro | O quê | md5 |
|---|---|---|
| `team6_jul14_sanitizer.bin` | firmware novo (flashado 18 jul) | `35fffde87408b5891dfea1f7558e1bf2` |
| `team6_jul8_flashed.bin` | firmware antigo (rollback) | `8f7100016f82c44c5daa80ad0111522c` |
| `flash_dump_209164.bin` | dump real da flash pré-mudança | idem jul8 |
| `aeb_validate.py` | validador (`--test parado\|obstaculo\|rates`) | — |

Flash/rollback (no Pi, ST-Link V3 já ligado):
```bash
openocd -f interface/stlink-dap.cfg -f target/stm32u5x.cfg \
  -c "init" -c "halt" -c "program <BIN> 0x08000000 verify" \
  -c "reset run" -c "shutdown"
```

## 7. Limitações conhecidas

- **Objeto parado que "nasce" dentro da banda** (250-900 mm): se um
  obstáculo for colocado à frente do carro já dentro dessa zona (sem o
  carro se aproximar dele vindo de fora), o SRF08 sanitizado reporta
  "longe" até o objeto entrar abaixo de 250 mm (backstop) — é o preço de
  rejeitar o eco. Mitigação: a câmara (obj detection do Manager) cobre
  esse cenário; em aproximação normal em movimento a âncora 900→1300 mm
  resolve.
- A banda foi calibrada para a montagem atual do sensor (110 mm, ~5°).
  **Se a montagem mudar, recalibrar `SAN_BAND_*`** (ver geometria em
  `srf08.h`).
- O Manager (Modular8) ainda não consome a frame 0x002; e se o OA voltar
  a ser ligado, o `gap_cm` do 0x213 já vem sanitizado — o risco de
  BLOCKED permanente por eco desapareceu com este flash.
