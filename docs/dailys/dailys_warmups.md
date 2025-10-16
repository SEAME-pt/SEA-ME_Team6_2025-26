# 🕒 Daily Scrum - 16/10/2025

**Equipa:** Team 6  
**Scrum Master:** Joao Silva  
**Participantes:** Equipa  

---

## 👤 Atualizações e Planeamento

| Membro | Objetivos do dia | O que fiz ontem | Impedimentos / bloqueios / dúvidas | O que falta fazer / Próximos passos |
|:--|:--|:--|:--|:--|
| **Equipa** | 1️⃣ Integrar e testar os principais componentes do set (Raspberry Pi, Hailo Hat, DSI, CSI, Gamepad)<br>2️⃣ Clarificar dúvidas logísticas e técnicas na reunião das 16h<br>3️⃣ Criar template de Pull Request e template para Daily<br>4️⃣ Continuar implementação do **TSF** no GitHub<br>5️⃣ Continuar análise das **GitHub Actions**<br>6️⃣ Prosseguir com implementação da **Qt Application**<br>7️⃣ Atualizar board (cada membro deve atualizar o estado das suas tasks) | - Set quase todo pronto e testado:<br>• Raspberry Pi OS (Bookworm) instalado<br>• Hailo Hat 8 instalado e funcional ([rpicam-apps](https://github.com/raspberrypi/rpicam-apps))<br>• CSI ligado e câmara funcional com script Python de deteção ([hailo-rpi5-examples](https://github.com/hailo-ai/hailo-rpi5-examples))<br>• DSI ligado e a funcionar<br>• Gamepad e dongle ligados e funcionais, dependências instaladas | - Falta de material (cabeçalhos, carregador 5V/5A, cabos DSI, adaptadores HDMI, dissipadores)<br>- Wi-Fi da Raspberry Pi com possíveis problemas de configuração (como no grupo do Miguel)<br>- Dependência de clarificação com David e Filipe (reuniões de hoje e amanhã)<br>- Decisão pendente entre AGL/ThreadX/FreeRTOS/Azure para arquitetura final<br>- Dividir tarefas grandes em várias *sprints* e *epics*<br>- Confirmar que todas as *tasks* estão bem definidas (relacionadas com o sprint)<br>- Criar *traceability matrix* (PB1 → L0 → L1 → L2 → L3 → Test Cases)<br>- Confirmar com Dana sessão sobre o TSF<br>- Validar escolha final: **AGL (Raspberry)** + **ThreadX (microcontrolador)**<br>- Planeamento futuro: ligar motores ao microcontrolador e criar camada de comunicação bidirecional (uProtocol)<br>- Fazer estimativa de consumo energético (via osciloscópio ou cálculo teórico)<br>- Falar com Vítor sobre permissões e workflows (pull requests, Git Actions)<br>- Confirmar necessidade de documentação no final de cada *task* (na issue + traceability backup) | - Ligar todos os componentes à placa de expansão (aguardar compra de cabeçalhos)<br>- Preparar inventário dos componentes necessários para todos os grupos<br>- Enviar inventário ao Filipe até amanhã<br>- Dividir tarefas grandes em subtasks / epics<br>- Continuar desenvolvimento da **Qt Application** e **TSF** |



# 🕒 Daily Scrum - 17/10/2025

**Equipa:** Team 6  
**Scrum Master:** Joao Silva
**Participantes:** Equipa

---

## 🧩 1. O que é para fazer / Objetivo do dia 
> 1 - Fazer Assembly do carro já com os componentes: Placa expansão, RP5, Halo, Câmara.
2 - Testar com esse novo set para verificar se está tudo a funcionar, e se estiver pensar em migrar para novo set onde servomotor e DCmotor estão ligados ao microcontrolador e nao à placa de expansão.
3 - Fazer template PR, template p/a daily.
4 - Continuar com TSF implementação no github. 
5 - Continuar a ver github actions.
6 - Continuar com a implementação da QT aplication.
7 - Atualizar board (cada elemento deve atualizar o estado das suas tasks).

---

## 👤 2. Atualizações e Planeamento

| Membro | O que fiz ontem | Impedimentos / bloqueios / dúvidas | O que vou fazer hoje |
|:--|:--|:--|:--|
| **Equipa** | - Set quase todo pronto e testado:<br>• Raspberry Pi OS (Bookworm) instalado<br>• Hailo Hat 8 instalado e funcional ([rpicam-apps](https://github.com/raspberrypi/rpicam-apps))<br>• CSI ligado e câmara funcional com script Python de deteção ([hailo-rpi5-examples](https://github.com/hailo-ai/hailo-rpi5-examples))<br>• DSI ligado e a funcionar<br>• Gamepad e bundle ligados e funcionais, dependências instaladas | - Falta de material (cabeçalhos, carregador 5V/5A, cabos DSI, adaptadores HDMI, dissipadores)<br>- Wi-Fi da Raspberry Pi com possíveis problemas de configuração (como no grupo do Miguel)<br>- Dependência de clarificação com David e Filipe (reuniões de hoje e amanhã)<br>- Decisão pendente entre AGL/ThreadX/FreeRTOS/Azure para arquitetura final<br>- Dividir tarefas grandes em várias *sprints* e *epics*<br>- Confirmar que todas as *tasks* estão bem definidas (relacionadas com o sprint)<br>- Criar *traceability matrix* (PB1 → L0 → L1 → L2 → L3 → Test Cases)<br>- Confirmar com Dana sessão sobre o TSF<br>- Validar escolha final: **AGL (Raspberry)** + **ThreadX (microcontrolador)**<br>- Planeamento futuro: ligar motores ao microcontrolador e criar camada de comunicação bidirecional (uProtocol)<br>- Fazer estimativa de consumo energético (via osciloscópio ou cálculo teórico)<br>- Falar com Vítor sobre permissões e workflows (pull requests, Git Actions)<br>- Confirmar necessidade de documentação no final de cada *task* (na issue + traceability backup) | - Ligar todos os componentes à placa de expansão (aguardar compra de cabeçalhos)<br>- Preparar inventário dos componentes necessários para todos os grupos<br>- Enviar inventário ao Filipe até amanhã |