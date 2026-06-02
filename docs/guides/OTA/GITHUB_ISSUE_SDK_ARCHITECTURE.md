# GitHub Issue - Copiar para: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/issues/new

---

## Título:
```
🐛 [OTA] SDK Architecture Mismatch - Binários 32-bit em sistema 64-bit (RPi5)
```

## Labels:
`bug`, `OTA`, `high-priority`

---

## Body (copiar abaixo):

## 🔴 Problema Crítico

Os binários gerados pelo CI/CD (GitHub Actions) são **ARM 32-bit**, mas o Raspberry Pi 5 corre um sistema **ARM 64-bit (aarch64)**. Isto significa que **o cluster nunca funcionou via OTA**.

---

## 📊 Evidência do Problema

### No Raspberry Pi 5 (AGL):
```bash
# Sistema é 64-bit
$ uname -m
aarch64

# Binário do OTA é 32-bit (ERRADO!)
$ file /opt/cluster/HelloQt6Qml
ELF 32-bit LSB pie executable, ARM, EABI5 version 1 (SYSV)...

# Binário KUKSA manual é 64-bit (CORRETO - foi instalado manualmente)
$ file /home/kuksa_RPi5/bin/can_to_kuksa_publisher
ELF 64-bit LSB pie executable, ARM aarch64...
```

### Sintomas observados:
- `cluster.service` em restart loop infinito (13,951+ tentativas)
- Erro `status=203/EXEC` - "Failed to execute: No such file or directory"
- O kernel não encontra o interpretador ELF correto para binários 32-bit

---

## 🔍 Causa Raiz

### 1. Docker Image usa SDK errado

O ficheiro `src/setup/Dockerfile.agl-sdk` usa:
```dockerfile
ARG SDK_INSTALLER=poky-agl-glibc-x86_64-agl-image-minimal-crosssdk-armv7vet2hf-neon-vfpv4-raspberrypi4-toolchain-${SDK_VERSION}.sh
```

| Componente | Valor Atual | Problema |
|------------|-------------|----------|
| Arquitetura | `armv7vet2hf` | ❌ 32-bit |
| Target | `raspberrypi4` | ❌ RPi4, não RPi5 |

### 2. Docker Hub image desatualizado

A imagem `souzitaaaa/team6-agl-sdk:latest` foi construída com o SDK 32-bit do RPi4.

### 3. Workflow CI/CD usa imagem errada

Em `.github/workflows/ota.yml`:
```yaml
run: docker pull souzitaaaa/team6-agl-sdk:latest  # ← SDK 32-bit!
```

---

## 🔴 Problema Secundário: Path KUKSA incorreto

O script `ota-update.sh` copiava para o caminho errado:

| Serviço | Path usado pelo systemd | Path onde OTA copiava |
|---------|------------------------|----------------------|
| KUKSA | `/home/kuksa_RPi5/` | `/home/root/kuksa_RPi5/` ❌ |

**Resultado:** O OTA nunca atualizava o KUKSA - usava sempre o binário manual antigo (que por acaso era 64-bit e funcionava).

**Status:** ✅ Já corrigido em `feature/OTA/implementation`

---

## 📁 Ficheiros Afetados

| Ficheiro | Problema | Correção Necessária |
|----------|----------|---------------------|
| `src/setup/Dockerfile.agl-sdk` | SDK 32-bit RPi4 | Criar versão para RPi5 aarch64 |
| `.github/workflows/ota.yml` | Usa imagem errada | Atualizar para imagem correta |
| `src/ota/scripts/ota-update.sh` | Path KUKSA errado | ✅ Já corrigido |

---

## ✅ Solução Proposta

### Opção A: Docker Images Separados (Recomendado para Multi-Platform)

Para suportar OTA em RPi4 **e** RPi5:

```bash
# Para RPi4 (32-bit) - manter existente
souzitaaaa/team6-agl-sdk:rpi4

# Para RPi5 (64-bit) - CRIAR NOVO
souzitaaaa/team6-agl-sdk:rpi5
```

Workflow CI/CD compila para ambos e cria releases separadas ou um pacote multi-arch.

### Opção B: Atualizar para RPi5 apenas

Se o projeto só usa RPi5:
1. Obter SDK: `poky-agl-glibc-x86_64-agl-image-minimal-crosssdk-aarch64-raspberrypi5-toolchain-20.0.2.sh`
2. Atualizar `Dockerfile.agl-sdk` para usar este SDK
3. Reconstruir e push para Docker Hub
4. Criar nova release (v1.8.0) com binários 64-bit

---

## 📋 Tasks

- [ ] Obter SDK aarch64 para RPi5 (verificar com @souzitaaaa se já existe)
- [ ] Criar `Dockerfile.agl-sdk-rpi5` ou atualizar existente
- [ ] Construir e push nova imagem Docker
- [ ] Atualizar `.github/workflows/ota.yml`
- [ ] Criar release v1.8.0 com binários 64-bit corretos
- [ ] Testar OTA completo no RPi5
- [ ] (Opcional) Configurar multi-platform build para RPi4 + RPi5

---

## 🔗 Contexto

- **Branch:** `feature/OTA/implementation`
- **Descoberto durante:** Teste de rollback OTA com release v1.7.0
- **Relacionado com:** Implementação de A/B updates com RAUC para RPi4 e RPi5

---

## 🧠 Explicação Simples do Problema (para discussão com equipa)

### O que encontrámos:

O nome do SDK no Dockerfile diz tudo:
```
poky-agl-glibc-x86_64-agl-image-minimal-crosssdk-armv7vet2hf-neon-vfpv4-raspberrypi4-toolchain-20.0.2.sh
                                                 ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑         ↑↑↑↑↑↑↑↑↑↑↑↑
                                                 ARM 32-bit (ARMv7)            Raspberry Pi 4
```

**Mas o nosso RPi5 corre em ARM 64-bit (aarch64)!**

### O que já existe no projeto:

| Componente | Ficheiro | SDK usado | Usado no CI? |
|------------|----------|-----------|--------------|
| `build-deploy.sh` | Local | `aarch64-raspberrypi5` (64-bit) ✅ | ❌ Não |
| `ota.yml` | CI/CD | `armv7-raspberrypi4` (32-bit) ❌ | ✅ Sim |

**Problema:** O CI/CD usa a imagem Docker errada (`souzitaaaa/team6-agl-sdk:latest`) que foi construída com SDK 32-bit!

### O que descobrimos:

1. **O SDK 64-bit para RPi5 JÁ está referenciado** no `.gitignore`:
   ```
   src/setup/poky-agl-glibc-x86_64-agl-image-minimal-crosssdk-aarch64-raspberrypi5-toolchain-20.0.2.sh
   ```

2. **O script `qt6-build.sh` já tenta usar aarch64 primeiro:**
   ```bash
   if [ -f /opt/agl-sdk/environment-setup-aarch64-agl-linux ]; then
       source /opt/agl-sdk/environment-setup-aarch64-agl-linux
   ```

3. **O `build-deploy.sh` espera o SDK RPi5:**
   ```bash
   SDK_INSTALLER=$(ls setup/poky-agl-glibc-x86_64-...-aarch64-raspberrypi5-toolchain-*.sh ...)
   ```

**Conclusão:** O Diogo provavelmente já tem o SDK 64-bit localmente (ficheiro ~1GB no gitignore), mas a imagem Docker no Docker Hub foi construída com o SDK errado.

---

## ❓ Perguntas para Diogo e David

### Pergunta 1: Onde está o SDK do RPi5?

O ficheiro `poky-agl-glibc-x86_64-agl-image-minimal-crosssdk-aarch64-raspberrypi5-toolchain-20.0.2.sh` existe algures?
- Se sim, onde? (Google Drive, servidor, etc.)
- Se não, como podemos obtê-lo? (Build Yocto/AGL?)

### Pergunta 2: Qual a estratégia para multi-platform?

Queremos OTA em **RPi4 E RPi5**. Qual opção preferem?

#### Opção A: Dois Docker Images Separados (Recomendado)
```bash
souzitaaaa/team6-agl-sdk:rpi4    # ARM 32-bit
souzitaaaa/team6-agl-sdk:rpi5    # ARM 64-bit (aarch64)
```
- ✅ Mais simples de manter
- ✅ Imagens mais pequenas
- ❌ Dois builds no CI/CD

#### Opção B: Um Docker Image com Ambos os SDKs
```bash
souzitaaaa/team6-agl-sdk:latest  # Contém ambos os SDKs
```
- ✅ Uma só imagem
- ❌ Imagem muito grande (~2GB+)
- ❌ Mais complexo de configurar

#### Opção C: Só RPi5 (abandonar RPi4)
```bash
souzitaaaa/team6-agl-sdk:latest  # Só aarch64
```
- ✅ Mais simples
- ❌ Perde suporte RPi4

### Pergunta 3: Quem reconstrói a imagem Docker?

Depois de decidir a estratégia:
1. Quem tem acesso para push no Docker Hub (`souzitaaaa`)?
2. Quem vai reconstruir e publicar a nova imagem?

---

## 📊 Resumo do Estado Atual

| Componente | Estado Atual | Problema |
|------------|-------------|----------|
| `Dockerfile.agl-sdk` | Usa SDK armv7-rpi4 | ❌ 32-bit, RPi4 |
| Docker Hub image | `souzitaaaa/team6-agl-sdk:latest` | ❌ Construído com SDK errado |
| CI/CD workflow | Usa a imagem errada | ❌ Gera binários 32-bit |
| `build-deploy.sh` | Espera SDK aarch64-rpi5 | ✅ Correto (mas não usado no CI) |
| SDK RPi5 | Existe mas está no .gitignore | ⚠️ Não publicado/partilhado |

---

## 🎯 Passos para Resolver (após decisão da equipa)

### Se escolherem Opção A (dois images):

```bash
# 1. Obter os dois SDK installers
poky-agl-...-armv7vet2hf-...-raspberrypi4-toolchain-20.0.2.sh  # Já existe
poky-agl-...-aarch64-raspberrypi5-toolchain-20.0.2.sh          # Precisa ser partilhado

# 2. Criar dois Docker images
docker build -f Dockerfile.agl-sdk-rpi4 -t souzitaaaa/team6-agl-sdk:rpi4 .
docker build -f Dockerfile.agl-sdk-rpi5 -t souzitaaaa/team6-agl-sdk:rpi5 .

# 3. Push para Docker Hub
docker push souzitaaaa/team6-agl-sdk:rpi4
docker push souzitaaaa/team6-agl-sdk:rpi5

# 4. Atualizar ota.yml para usar a imagem correta (ou ambas)
```

### Se escolherem Opção C (só RPi5):

```bash
# 1. Obter SDK RPi5
poky-agl-...-aarch64-raspberrypi5-toolchain-20.0.2.sh

# 2. Atualizar Dockerfile.agl-sdk para usar este SDK

# 3. Reconstruir e push
docker build -f Dockerfile.agl-sdk -t souzitaaaa/team6-agl-sdk:latest .
docker push souzitaaaa/team6-agl-sdk:latest

# 4. Criar release v1.8.0 com binários 64-bit
```

---

## 📝 Notas Importantes

### O SDK para RPi5 já está referenciado no projeto:

1. **`.gitignore`** menciona:
   ```
   src/setup/poky-agl-glibc-x86_64-agl-image-minimal-crosssdk-aarch64-raspberrypi5-toolchain-20.0.2.sh
   ```

2. **`build-deploy.sh`** espera este ficheiro:
   ```bash
   SDK_INSTALLER=$(ls setup/poky-agl-glibc-x86_64-agl-image-minimal-crosssdk-aarch64-raspberrypi5-toolchain-*.sh ...)
   ```

3. **`qt6-build.sh`** já tenta usar aarch64 primeiro:
   ```bash
   if [ -f /opt/agl-sdk/environment-setup-aarch64-agl-linux ]; then
       source /opt/agl-sdk/environment-setup-aarch64-agl-linux
   ```

O ficheiro SDK é grande (~1GB) por isso está no `.gitignore`. Precisa ser:
- Partilhado via cloud storage (Google Drive, etc.)
- Ou obtido do build AGL (Yocto)

---

## 🕐 Timeline do Problema

| Data | Evento |
|------|--------|
| Antes | Docker image criado com SDK RPi4 32-bit |
| v1.5.0, v1.6.0 | Releases criadas - binários 32-bit |
| 10/02/2026 | OTA "funcionou" mas cluster em restart loop |
| 11/02/2026 | Descoberto: arquitetura incompatível |

---

## 📎 Referências

- Workflow OTA: `.github/workflows/ota.yml`
- Dockerfile SDK: `src/setup/Dockerfile.agl-sdk`
- Script OTA: `src/ota/scripts/ota-update.sh`
- Build script: `src/build-deploy.sh`
