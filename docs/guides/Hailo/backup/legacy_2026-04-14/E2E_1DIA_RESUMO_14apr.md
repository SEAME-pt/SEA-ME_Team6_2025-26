# E2E Comparativo 1 Dia — Resumo Executivo (14 April 2026)

**Objetivo:** Comparar 3 pipelines E2E em tempo real:
- **A** (colega): UFLDv2 + YOLOv8s
- **B** (João): UFLDv2 + YOLO26n-seg (split hybrid: backbone Hailo + head CPU)
- **C** (João): UFLDv2 + YOLOv8n-seg (tudo Hailo)

**Duração:** 9h30 (8h00–17h30)  
**Critério gate:** p95 latência E2E <= 50 ms, CPU avg <= 30%, FN passadeira <= 1%

---

## Resumo do Planeamento (Blocos)

| Bloco | O quê | Tempo | Quem | Status |
|-------|-------|-------|------|--------|
| **0** | Pré-requisitos (protocolo, dataset, skeleton) | 0h30 | colega + João + colega | ✅ PRONTO |
| **1** | Montar E2E YOLO26n-seg (B) | 3h00 | João + colega | ⚠️ INÍCIO 8h30 |
| **2** | Montar E2E YOLOv8n-seg (C) | 2h00 | João + colega | ⚠️ INÍCIO 11h30 |
| **3** | Correr 2 cenários (Normal + Degradado) | 1h45 | João | ⚠️ INÍCIO 13h30 |
| **4** | Tabela final + decisão | 1h30 | João + colega + colega | ⚠️ INÍCIO 15h15 |
| **Buffer** | Debug / SegFormer (Opção D) / reunião | 1h00 | Todos | ⚠️ INÍCIO 16h45 |

---

## Dependências Críticas (ANTES DE COMEÇAR)

✅ **HEFs já validados em Hailo:**
```
yolov8n_seg_h8.hef (C)
yolo26n_seg_320_h8_no_nms.hef (B, split hybrid)
```

✅ **Dataset colega:**
- 50+ imagens pista real com labels (passadeiras, setas, sinais)
- Separadas em: Normal (25) + Degradado/Curva+Sombra (25)

✅ **Skeleton E2E colega:**
- Decode → preprocess → infer → postprocess → render
- Já pronto; João copia e adapta para B e C

---

## 3 Pontos-Chave (Mudanças vs Plano Anterior)

### 1️⃣ **Benchmark anterior (11D.11) foi infer-only, não E2E**
- hailortcli benchmark usa dummy frames sintéticos
- FPS/latência hw_only são válidos, mas não incluem preprocess real, host-NMS, render
- **Aqui:** medem E2E real com dataset colega

### 2️⃣ **YOLO26n-seg (full INT8) — novo candidato**
- Nunca foi testado em Hailo
- Se compilar, latência ~9-10ms (sem overhead host)
- Bloco 1.5: 30 min para tentar; se falhar, revert para split hybrid (11D.10) garantido

### 3️⃣ **Split hybrid confirmado (11D.10)**
- Backbone INT8 na Hailo (~8-10ms) + head float32 CPU (~1-3ms)
- Host-NMS overhead: ~2-3ms adicional
- Latência E2E esperada: ~12-15ms + preprocess + render

---

## Ordem Recomendada (Porquê)

```
B (YOLO26n-seg) ANTES de C (YOLOv8n-seg):
├─ B é mais complexo (host-NMS, split hybrid)
├─ C é simpler, mais estável (fallback garantido)
└─ Se B passar → melhor desempenho; se falhar → fallback rápido para C

Dentro de B:
├─ Tentar full INT8 primeiro (0h30, novo)
├─ Se falhar → split hybrid (3h, já validado)
└─ Se split falhar → fallback automático para C
```

---

## Gates de Decisão Final

**GO Produção:**
- p95 latência E2E <= 50 ms
- CPU avg <= 30%, CPU max <= 60%
- FN em passadeira/setas <= 1%

**GO Experimental:**
- p95 latência E2E <= 60 ms (com observação)
- CPU avg <= 40%, CPU max <= 75%
- FN em passadeira/setas <= 2%

**NO-GO:**
- p95 latência > 60 ms, ou CPU max > 75%, ou FN > 2%
- Compilação/runtime bloqueada
- → recuar para A (colega baseline)

---

## Como Começar (AGORA)

### Pre-check (5 min)

```bash
# Terminal 1 (Lenovo)
ls -lh ~/Documents/AI/hailo/shared_with_docker/hef/
# OUTPUT: yolov8n_seg_h8.hef, yolo26n_seg_320_h8_no_nms.hef

# Terminal 2 (AGL/Rasp5)
ssh root@<IP_AGL>
hailortcli parse-hef /data/yolov8n_seg_h8.hef
hailortcli parse-hef /data/yolo26n_seg_320_h8_no_nms.hef
# OUTPUT: "Model parsed successfully" ambos
```

### Começar Bloco 0 (8h00)

```bash
# Terminal 1 (Lenovo)
cd ~/Documents/AI/hailo/shared_with_docker

# Ler este ficheiro
cat docs/guides/Hailo/E2E_1DAY_CHECKLIST_14apr2026.md

# Começar Bloco 0, Task 0.1
# (congelar protocolo, preparar dataset, skeleton)
```

---

## Ficheiros Críticos (para consultar durante o dia)

| Ficheiro | Propósito |
|----------|-----------|
| `AI_implementation_planning.md` | Contexto técnico completo (seção 11E novo) |
| `E2E_1DAY_CHECKLIST_14apr2026.md` | **← Usar isto como guia hora-a-hora** |
| `E2E_1dia_resumo_14apr.md` | Este ficheiro (quick reference) |

---

## Contatos & Escalação

- **João:** E2E YOLO26n-seg + YOLOv8n-seg (infer→postprocess→render)
- **colega:** UFLDv2 + coordenação protocolo
- **colega:** Dataset + pós-processamento + render validation

**Se bloquear:** ping mentores (Paulo?) ou escalate para Dia 2.

---

## Última Nota

Este planeamento é **ambicioso mas exequível** em 1 dia agressivo. Se qualquer bloco ficar para trás >30 min, skip para próxima e registar como "bloqueado" no report final. **Prioridade: completar B + C vs perfeição absoluta.**

---

**Status:** ✅ PRONTO PARA EXECUTAR  
**Data:** 14 April 2026 (8h00 start)  
**Próximo:** Confirmar pré-check em 2 min, depois Bloco 0
