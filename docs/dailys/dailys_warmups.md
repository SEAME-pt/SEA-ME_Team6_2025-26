# 🕒 Daily Scrum - Team 6

**Scrum Master:** Joao Silva  
**Participantes:** Equipa  
1️⃣2️⃣3️⃣4️⃣5️⃣6️⃣7️⃣8️⃣9️⃣🔟
1️⃣1️⃣ 1️⃣2️⃣ 1️⃣3️⃣ 1️⃣4️⃣ 1️⃣5️⃣ 1️⃣6️⃣ 1️⃣7️⃣ 1️⃣8️⃣ 1️⃣9️⃣ 2️⃣0️⃣
---

## 👤 Atualizações e Planeamento

| Data | Membro | Objetivos do dia / O que falta fazer do dia anterior | O que fiz | Impedimentos / bloqueios / dúvidas |
|:--|:--|:--|:--|:--|:--|
| 06/10/2025 | **Joao** | Apresentação do SEA:ME e constituição das equipas | Objetivo atingido | Não há blockers| 
| 08/10/2025 | **Joao_PRÉ-SPRINTPLANNING** | Primeira reuniao semanal com pré-planeamento do próximo sprint | Objetivo atingido | Não há blockers| 
| 10/10/2025 | **Joao** | Criação do github TEAM&_2025/2025 e da board do projeto no github do SEA:ME | Em progresso sem questões | Autorizacoes por parte do Victor no que toca a github actions e PR |

| 13/10/2025 | **Joao_SPRINTPLANNING** | 1. hardware setup 2. software setup 3. Github actions 4. TSF 5. QT e crosscompliation ... com colocacao do projeto no git e elaboraçao da board projects | Em progresso sem questões | | 10/10/2025 | **Joao** | Criação do github TEAM&_2025/2025 e da board do projeto no github do SEA:ME | Em progresso sem questões | Autorizacoes por parte do Victor no que toca a github actions e PR |
| 14/10/2025 | **Joao** | Continuamos implementação dos pontos referidos no dia anterior | Em progresso sem questões | Autorizacoes por parte do Victor no que toca a github actions e PR |
| 15/10/2025 | **Joao_REUNIAOSEMANAL** | 1️⃣ - Fazer setup 2️⃣ -Instalar Halo 3️⃣ - Ligar CSI 4️⃣ - Ligar DSI 5️⃣ - Ligar comando gamepad e instalar dependencias | 1️⃣ - Set quase pronto e quase todo testado:
2️⃣ - raspberry bookworm  instalado
3️⃣ - hallo hat 8 instalado https://github.com/raspberrypi/rpicam-apps
4️⃣ - CSI ligado/camera instalada e a funcionar com script python de deteção (pessoas, tlms, etc) https://github.com/hailo-ai/hailo-rpi5-examples
5️⃣ - DSI ligado e a funcionar
6️⃣ - gamepad (e bundlle do gamepad) ligado no raspeberry pi e a funcionar, já com depencias instaladas
7️⃣ - Falta: 
- Ligar tudo à placa de expansão...temos de comprar cabeçalhos. |
Dúvidas para David perguntar na reunião das 16h?
1️⃣ - coisas para o carro que temos de comprar - ela arranja ou compramos nós e apresentamos nota de despesa? Temos de comprar cabeçalhos, carregador original de Raspeberry (34€) 5V/5A, cabos DSI mais compridos, adaptadores microHDMI (vários, para os grupos todos), dissipadores/ventoinhas Raspeberry Pi 5
2️⃣ - fiquei encarregue de fazer um inventário daquilo que todos os grupos precisam e enviar amanhã ao Filipe. Ele irá reunir amanhã com a Maria e tratar disso. 
3️⃣ - dividir as tarefas grandes por várias sprints
-epics Sim. Tasks tem de ser relacionadas com aquele sprint. Se assim não for, então a task está mal feita e deverá ser dividida em pedaços mais pequenos. 
4️⃣ - no TSF é para fazer uma "traceability matrix" (PB1->L0->L1->L2->L3->Test Cases)?
5️⃣ - é isto mesmo 👆 estamos a ir no caminho certo. O Filipe diz que a Dana vai preparar sessão (diz que fala com ela amanha). 
6️⃣ - devemos escolher entre o AGL (Android Grade Linux) ou Android Automotive OS? ou podemos escolher outro como o Eclipse ThreadX? AGL ou ThreadX? ou o FreeRTOS ou Azure ?
7️⃣ -devemos escolher AGL(raspberry) e ThreadX(microcontrolador). O Pedro recomendou tb tentarmos ligar motores ao microcontrolador. Possivelmente mais à frente (por agora deixamos estar ligados como estão), mas mais pa frente seria interessante pensar nisso (e preparar isso já). Assim tudo o que seria power train ficaria ligado ao microcontrolador. Depois poderemos (e devemos) criar uma layer de Comunicação entre a rasp e o micro (usando por exemplo uprotocol). Podemos pensar em bidirecional em vez de unidirecional. 
8️⃣ - em que semana estamos do warmup?
-a semana anterior foi a 0 (ou - 1) e esta é a primeira semana do primeiro sprint.sao 2 sprints Para este modulo 0.portanto vamos mt adiantados(o que referiram ser mt bom). 
. Outras questões:
9️⃣ - os problemas de power podem ser resolvidos como estávamos a pensar (2 sets de baterias, um para cada coisa) ou podemos usar reguladores de tensão (nota: dps temos de ver como... Ou se alguém tiver uma ideia de como podemos usar esta solução, que diga pf)
🔟 - ainda sobre o tópico anterior, o hardware tem especificações (nomeadamente ao nível de energia). Portanto ou usamos o osciloscópio ou podemos calcular (teoricamente) o qt precisa o carro no total (somando as necessidades das partes/dos seus vários componentes,uma vez que cada componente tem especificações de consumo). 
1️⃣1️⃣ - temos de usar o TSF, portanto convém usar nomenclatura do mesmo. Portanto temos de usar a traceability matrix, assim com L0, L1, etc
1️⃣2️⃣ - vão falar com o Vítor por causa das autorizações/permissões para: regras para pull request, workflows para git actions, etc
1️⃣3️⃣ - o grupo do Miguel não consegue ligar o Wi-Fi, apesar de estar enabled na placa. se quisermos usar o Wi-Fi da raspberry mt possivelmente vamos ter os problemas de config(kernel) q o grupo do Miguel está a ter. Há que resolvê-los. 
1️⃣4️⃣ - scrum: nós é que temos que decidir o que queremos fazer com base na dinâmica do grupo. Isto é, se achamos que faz sentido uma daily e escrever no final do dia, podemos fazê-lo. Devemos fazer o que nos sentimos mais à vontade... Sem complicar (KISS).
1️⃣5️⃣ - perguntei se no final de cada task teriamos de ter documentação. Disseram que sim. E como tínhamos pensado... Na issue. Sugeri doc traceability backup  adicional(o q ja tinha proposto aqui a vocês...qd puderem respondam ao que oetuntei sobre este tópico @~Ruben Carvalho @Sea:me Vasco Soares Vasferre ). Disseram que sim. |
| 16/10/2025 | **Joao** | 1️⃣ Integrar e testar os principais componentes do set (Raspberry Pi, Hailo Hat, DSI, CSI, Gamepad)<br>2️⃣ Clarificar dúvidas logísticas e técnicas na reunião das 16h<br>3️⃣ Criar template de Pull Request e template para Daily<br>4️⃣ Continuar implementação do **TSF** no GitHub<br>5️⃣ Continuar análise das **GitHub Actions**<br>6️⃣ Prosseguir com implementação da **Qt Application**<br>7️⃣ Atualizar board | 1️⃣ - Set quase todo pronto e testado:<br>• Raspberry Pi OS (Bookworm) instalado<br>• Hailo Hat 8 instalado e funcional ([rpicam-apps](https://github.com/raspberrypi/rpicam-apps))<br>• CSI ligado e câmara funcional com script Python de deteção ([hailo-rpi5-examples](https://github.com/hailo-ai/hailo-rpi5-examples))<br>• DSI ligado e a funcionar<br>• Gamepad e dongle ligados e funcionais, dependências instaladas<br>2️⃣ - Templates de Pull Request e template para Daily e Taskly criados<br>3️⃣ - TSF: implementação iniciada<br>4️⃣ - GitHub Actions: em standby<br>5️⃣ - Qt Application: em progresso<br>6️⃣ - Board atualização: em progresso | - Falta de material (cabeçalhos, carregador 5V/5A, cabos DSI, adaptadores HDMI, dissipadores)<br>- Wi-Fi da Raspberry Pi com possíveis problemas de configuração<br>- Dependência de validação com David e Filipe<br>- Decisão pendente entre AGL/ThreadX/FreeRTOS/Azure<br>- Necessidade de dividir tarefas grandes em subtasks/epics<br>- Confirmar nomenclatura e uso da traceability matrix no TSF<br>- Autorização/permissões para pull requests e GitHub Actions |
| 17/10/2025 | **Joao** | 1️⃣ Ligar todos os componentes à placa de expansão / fazer assembly do carro<br>2️⃣ Preparar inventário dos componentes necessários e enviar ao Filipe<br>3️⃣ Validar template PR, template para Daily e template Taskly (João e David)<br>4️⃣ Continuar implementação do TSF no GitHub<br>5️⃣ Continuar análise das GitHub Actions<br>6️⃣ Continuar implementação da Qt Application<br>7️⃣ Atualizar board | Falta fazer tudo | - Falta de integração completa do hardware<br>- Dependência de inventário e validação de templates<br>- TSF e Qt ainda em desenvolvimento<br>- GitHub Actions em standby |

| 20/10/2025 | **Joao** | 1️⃣  - Ligar todos os componentes à placa de expansão e testar novo set<br>2️⃣  - Preparar inventário dos componentes necessários e enviar ao Filipe<br>3️⃣ - Validar templates<br>4️⃣ - Continuar implementação TSF<br>5️⃣ - Continuar análise GitHub Actions<br>6️⃣ - Continuar implementação Qt Application<br>7️⃣ - Atualizar board | Tudo feito exceto o ponto 5 - TSF e ponto 6 - QT | Nao há blockers nem dúvidas|
| 21/10/2025 | **Joao** | Continuamos implementação dos pontos referidos no dia anterior e que ficaram por fazer | Em progresso sem questões | Nao há blockers nem dúvidas|
| 22/10/2025 | **Rubén_REUNIAOSEMANAL** | 1️⃣ Continuação do trabalho a ser desenvolvido e Preparação da reunião semanal | Tudo feito |
1️⃣ - Git (review's,rule set, e outros) : Ficou de ser falado e resolvido com o Vitor da 42
2️⃣-QT : neste modulo querem ver o mais basico, apenas um hello world ja está bom, mas se quisermos mostrar uma primeira versao do nosso design tambem esta porreiro.
3️⃣-Problemas Wireless DNS: A ser resolvido pelo Vitor da 42.
4️⃣-AGL: pesquisar o que é o YOCTO(construtor de distribuições de linux)
5️⃣-Comunicação CAN: Usar CAN e não CAN-FD
6️⃣-Comunicação CAN entre hardware: RASP->MF2515-> CABO CAN -> MF2515 -> STM32
7️⃣-Bibliotecas de I2C/SPI/GPIO: pesquisar e utilizar o Eclipse uProtocol
8️⃣-Gestão de energia: Pesquisar mais e planificar como queremos contruir este sistema(Battery Managment System + Regulador Buck-Boost + Power Distribution Board com fusíveis independentes)
9️⃣Meetings:
TSF -> 6 Novembro 
ThreadX-> 4/11/14 Novembro(a definir ainda)
Architecture Blueprints Eclipse -> a definir data
Workshop OpenHardware -> a definir data |
| 23/10/2025 | **Joao** | Estudo dos assuntos falados durante a reuniao semanal e continuacao dos temas: 1. hardware setup 2. software setup 3. Github actions 4. TSF 5. QT e crosscompliation| Não há blockers para já |
| 24/10/2025 | **Joao** | Continuamos implementação dos pontos referidos no dia anterior e que ficaram por fazer e Preparação da retrospective | Em progresso sem questões | Nao há blockers nem dúvidas |
| 27/10/2025 | **Joao_SPRINTPLANNING** | 1️⃣ - Atualizacao das branches: colocar development e main iguais. 2️⃣- Definir metodologia de trabalho para o próximo sprint (em relação às dailys, tasklys, etc) 3️⃣ - Preparação do sprint planning e definicao do sprint: 3️⃣1️⃣ - Definicao Arquitetura (software + hardaware)...se é AGL, ThreadX, Canbus, Uprotocol...se retiramos placa expansao e ligamos motores diretamente ao multicontrolador com novo esquema independente para energia e power supply 3️⃣2️⃣ Implementar TSF no github 3️⃣3️⃣ QT e Cross Compiltation 3️⃣4️⃣ Estudar próximo módulo e ligação com o ponto 1 deste módulo 3️⃣5️⃣ Finalizar tarefas passadas | Na reunião foi-nos aconselhado a: 1️⃣ Colocar "engineering process refinement" em vez de "finalizar tarefas passadas" (input do PAULO) 2️⃣ Retirar nºs dos Epics (input do PAULO) 3️⃣ Rever o que são EPICS e SPIKES, em que Epics podem ser transversais a vários sprints e spikes é algo que ainda vamos estudar (input do PEDRO) 4️⃣ Alguns exemplos de epics poderão ser: 1 - Instrument Cluster, 2 - UI Display 3 - Communication Archicteture 4 - Motion Control 5️⃣ - Estudar o que é arquitectura monolítica e arquitectura zonal (zonal é dividir a arquitectura por zonas dum carro para encapsular algumas das funcionalidades que dps comunicam entre si - explo: isolar travao da direcao/ou conducao autonoma, q ficam separadas por zonas do carro q dps comunicam entre si/entre zonas - zonas critical safe) | Um dos blockers é a retirada da placa de expansão. Outro está a ser perceber como será feita a crosscompilation do QT. Outro está a ser como será feita a introducao do QT dentro do DSI | 