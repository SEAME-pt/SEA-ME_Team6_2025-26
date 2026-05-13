#!/usr/bin/env python3
"""
Joystick Control v2 — UNIX socket client para ADAS Manager

Protocolo (UNIX DGRAM → /tmp/adas_joystick.sock):
  J <steering> <throttle>\n   — frame de joystick (50 Hz, modo MANUAL)
  T\n                          — toggle MANUAL ↔ AUTONOMOUS

Lógica de modo (modo joystick real):
  - Arranca em MANUAL: envia J frames ao ADAS Manager
  - Botão START → T → ADAS Manager usa PID autónomo
  - Qualquer eixo em AUTONOMOUS → volta a MANUAL (envia T de regresso)

Uso:
  sudo python3 joystick_control_v2.py
"""

import socket
import struct
import time
import sys
import select
import termios
import tty

try:
    import evdev
    from evdev import ecodes
    EVDEV_AVAILABLE = True
except ImportError:
    EVDEV_AVAILABLE = False

# Configuração
JOYSTICK_DEVICE = '/dev/input/event4'
SOCKET_PATH     = '/tmp/adas_joystick.sock'
SEND_INTERVAL   = 0.02   # 50 Hz (20ms)
DEAD_ZONE       = 5      # percentagem — ignora ruído do stick


class Colors:
    RED    = '\033[0;31m'
    GREEN  = '\033[0;32m'
    BLUE   = '\033[0;34m'
    YELLOW = '\033[1;33m'
    CYAN   = '\033[0;36m'
    BOLD   = '\033[1m'
    NC     = '\033[0m'


class VehicleController:
    def __init__(self, socket_path=SOCKET_PATH):
        self.socket_path = socket_path
        self.mode        = 'MANUAL'
        self.sock        = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
        print(f"{Colors.GREEN}✓ Socket {socket_path} pronto (UNIX DGRAM){Colors.NC}")
        print(f"  ADAS Manager deve estar à escuta em {socket_path}")

    def send_command(self, steering, throttle):
        """Envia frame J ao ADAS Manager"""
        steering = max(-100, min(100, steering))
        throttle = max(-100, min(100, throttle))
        msg = f"J {int(steering)} {int(throttle)}\n"
        try:
            self.sock.sendto(msg.encode(), self.socket_path)
            return True
        except (FileNotFoundError, OSError):
            return False

    def send_toggle(self):
        """Envia T — togla MANUAL ↔ AUTONOMOUS no ADAS Manager"""
        try:
            self.sock.sendto(b"T\n", self.socket_path)
            self.mode = 'AUTONOMOUS' if self.mode == 'MANUAL' else 'MANUAL'
            print(f"\n{Colors.YELLOW}[MODE] → {self.mode}{Colors.NC}")
        except (FileNotFoundError, OSError):
            pass

    def set_manual(self):
        """Força regresso a MANUAL (envia T se necessário)"""
        if self.mode != 'MANUAL':
            self.mode = 'MANUAL'
            try:
                self.sock.sendto(b"T\n", self.socket_path)
            except (FileNotFoundError, OSError):
                pass
            print(f"\n{Colors.CYAN}[MODE] → MANUAL (joystick activo){Colors.NC}")

    def print_status(self, steering, throttle):
        """Imprime status formatado"""
        def make_bar(value, width=15):
            center = width // 2
            if value > 0:
                filled = int((value / 100.0) * center)
                return ' ' * center + '█' * filled + ' ' * (center - filled)
            else:
                filled = int((abs(value) / 100.0) * center)
                return ' ' * (center - filled) + '█' * filled + ' ' * center

        mode_tag = f"{Colors.CYAN}[{self.mode}]{Colors.NC}" if self.mode == 'MANUAL' \
                   else f"{Colors.YELLOW}[{self.mode}]{Colors.NC}"
        print(f"\r{mode_tag} Steering[{make_bar(steering)}]{steering:4d}  "
              f"Throttle[{make_bar(throttle)}]{throttle:4d}  ",
              end='', flush=True)

    def stop(self):
        """Para o veículo"""
        self.send_command(0, 0)
        print(f"\n{Colors.YELLOW}→ STOP enviado{Colors.NC}")

    def cleanup(self):
        """Limpar recursos"""
        self.stop()
        self.sock.close()
        print(f"{Colors.GREEN}✓ Socket fechado{Colors.NC}")


# =============================================================================
# MODO 1: JOYSTICK REAL
# =============================================================================

def modo_joystick(controller):
    """Controlo com joystick real — suporta toggle MANUAL/AUTONOMOUS"""

    if not EVDEV_AVAILABLE:
        print(f"{Colors.RED}✗ Biblioteca 'evdev' não instalada!{Colors.NC}")
        print("Instale com: pip3 install evdev")
        return

    try:
        joystick = evdev.InputDevice(JOYSTICK_DEVICE)
        print(f"{Colors.GREEN}✓ Joystick: {joystick.name}{Colors.NC}")
        print(f"  Device: {JOYSTICK_DEVICE}")
    except Exception:
        print(f"{Colors.RED}✗ Joystick não encontrado em {JOYSTICK_DEVICE}{Colors.NC}")
        print("\nDispositivos disponíveis:")
        for path in evdev.list_devices():
            dev = evdev.InputDevice(path)
            print(f"  {path}: {dev.name}")
        return

    toggle_button = ecodes.BTN_START

    print(f"\n{Colors.BOLD}=== CONTROLO POR JOYSTICK (socket → ADAS Manager) ==={Colors.NC}")
    print("Controles:")
    print("  Stick Direito (horizontal) → Steering")
    print("  Stick Esquerdo (vertical)  → Throttle")
    print("  Botão START                → Toggle MANUAL / AUTONOMOUS")
    print("  Ctrl+C para sair\n")
    print(f"  Modo inicial: {Colors.CYAN}{controller.mode}{Colors.NC}\n")

    steering      = 0
    throttle      = 0
    last_send     = 0.0

    def normalize_axis(raw_value, invert=False):
        normalized = int(((raw_value - 127) / 127.0) * 100)
        if abs(normalized) < DEAD_ZONE:
            normalized = 0
        if invert:
            normalized = -normalized
        return max(-100, min(100, normalized))

    try:
        for event in joystick.read_loop():

            # ── Eixos ──────────────────────────────────────────────────────────
            if event.type == ecodes.EV_ABS:
                if event.code == ecodes.ABS_Z:
                    steering = normalize_axis(event.value)
                elif event.code == ecodes.ABS_Y:
                    throttle = normalize_axis(event.value, invert=True)

                # Movimento em AUTONOMOUS → volta a MANUAL automaticamente
                if controller.mode == 'AUTONOMOUS':
                    if abs(steering) > DEAD_ZONE or abs(throttle) > DEAD_ZONE:
                        controller.set_manual()

                # Rate-limited send a 50 Hz — só em MANUAL
                now = time.monotonic()
                if controller.mode == 'MANUAL' and (now - last_send) >= SEND_INTERVAL:
                    controller.send_command(steering, throttle)
                    controller.print_status(steering, throttle)
                    last_send = now

            # ── Botões ─────────────────────────────────────────────────────────
            elif event.type == ecodes.EV_KEY:
                if event.code == toggle_button and event.value == 1:
                    controller.send_toggle()

    except KeyboardInterrupt:
        print(f"\n{Colors.GREEN}✓ Joystick desconectado{Colors.NC}")


# =============================================================================
# MODO 2: TESTES AUTOMÁTICOS
# =============================================================================

def modo_testes(controller):
    """Executa sequência de testes automáticos via socket"""

    # Garantir modo MANUAL antes de enviar comandos de teste
    controller.set_manual()

    print(f"\n{Colors.BOLD}=== TESTES AUTOMÁTICOS (socket → ADAS Manager) ==={Colors.NC}\n")

    # Teste 1: Neutro
    print(f"{Colors.YELLOW}[1/5] Posição neutra...{Colors.NC}")
    controller.send_command(0, 0)
    controller.print_status(0, 0)
    time.sleep(2)

    # Teste 2: Steering
    print(f"\n{Colors.YELLOW}[2/5] Teste de steering...{Colors.NC}")
    for angle in [-100, -50, 0, 50, 100, 0]:
        print(f"\n  → Steering: {angle}")
        controller.send_command(angle, 0)
        controller.print_status(angle, 0)
        time.sleep(1)

    # Teste 3: Throttle
    print(f"\n{Colors.YELLOW}[3/5] Teste de throttle...{Colors.NC}")
    for speed in [25, 50, 0, -25, 0]:
        print(f"\n  → Throttle: {speed}")
        controller.send_command(0, speed)
        controller.print_status(0, speed)
        time.sleep(2)

    # Teste 4: Curvas
    print(f"\n{Colors.YELLOW}[4/5] Teste de curvas...{Colors.NC}")
    curvas = [
        (-50, 50,  "Frente + Esquerda"),
        ( 50, 50,  "Frente + Direita"),
        (-50, -50, "Trás + Esquerda"),
        ( 50, -50, "Trás + Direita"),
    ]
    for steer, throt, desc in curvas:
        print(f"\n  → {desc}")
        controller.send_command(steer, throt)
        controller.print_status(steer, throt)
        time.sleep(2)

    # Teste 5: Padrão em 8
    print(f"\n{Colors.YELLOW}[5/5] Padrão em '8'...{Colors.NC}")
    for angle in [-100, -50, 0, 50, 100, 50, 0, -50, -100]:
        controller.send_command(angle, 40)
        controller.print_status(angle, 40)
        time.sleep(0.5)

    # Finalizar
    controller.stop()
    print(f"\n\n{Colors.GREEN}✓ Testes concluídos!{Colors.NC}")


# =============================================================================
# MODO 3: CONTROLO INTERATIVO (TECLADO)
# =============================================================================

def modo_interativo(controller):
    """Controlo via teclado (WASD) com toggle MANUAL/AUTONOMOUS"""

    # Garantir modo MANUAL
    controller.set_manual()

    print(f"\n{Colors.BOLD}=== MODO INTERATIVO (WASD) → socket → ADAS Manager ==={Colors.NC}")
    print("Controles:")
    print("  W/S - Throttle (frente/trás)")
    print("  A/D - Steering (esquerda/direita)")
    print("  T   - Toggle MANUAL / AUTONOMOUS")
    print("  Espaço - STOP")
    print("  Q - Sair\n")
    print(f"  Modo inicial: {Colors.CYAN}{controller.mode}{Colors.NC}\n")

    steering = 0
    throttle = 0
    step     = 10

    old_settings = termios.tcgetattr(sys.stdin)
    try:
        tty.setcbreak(sys.stdin.fileno())

        while True:
            if select.select([sys.stdin], [], [], 0.1)[0]:
                key = sys.stdin.read(1).lower()

                if key == 't':
                    controller.send_toggle()
                    # Reset valores ao entrar em AUTONOMOUS
                    if controller.mode == 'AUTONOMOUS':
                        steering = 0
                        throttle = 0
                    continue

                if key == 'q':
                    break

                # WASD em AUTONOMOUS → volta a MANUAL automaticamente
                if controller.mode == 'AUTONOMOUS':
                    if key in ('w', 'a', 's', 'd', ' '):
                        controller.set_manual()

                if key == 'w':
                    throttle = min(100, throttle + step)
                elif key == 's':
                    throttle = max(-100, throttle - step)
                elif key == 'a':
                    steering = max(-100, steering - step)
                elif key == 'd':
                    steering = min(100, steering + step)
                elif key == ' ':
                    steering = 0
                    throttle = 0
                else:
                    continue

                # Só envia J em MANUAL
                if controller.mode == 'MANUAL':
                    controller.send_command(steering, throttle)
                    controller.print_status(steering, throttle)

    finally:
        termios.tcsetattr(sys.stdin, termios.TCSADRAIN, old_settings)
        print(f"\n{Colors.GREEN}✓ Modo interativo encerrado{Colors.NC}")


# =============================================================================
# MODO 4: EMERGENCY STOP
# =============================================================================

def modo_emergency_stop(controller):
    """Para tudo imediatamente"""
    print(f"\n{Colors.RED}{Colors.BOLD}EMERGENCY STOP{Colors.NC}")
    for _ in range(3):
        controller.send_command(0, 0)
    print(f"{Colors.GREEN}✓ J 0 0 enviado 3x{Colors.NC}")
    time.sleep(0.5)


# =============================================================================
# MENU PRINCIPAL
# =============================================================================

def menu_principal(mode):
    mode_color = Colors.CYAN if mode == 'MANUAL' else Colors.YELLOW
    print("\n" + "=" * 60)
    print(f"{Colors.BOLD}      JOYSTICK CONTROL v2 — ADAS Manager Socket{Colors.NC}")
    print("=" * 60)
    print(f"Socket  : {Colors.CYAN}{SOCKET_PATH}{Colors.NC}")
    print(f"Joystick: {Colors.CYAN}{JOYSTICK_DEVICE}{Colors.NC} (SHANWAN)")
    print(f"Modo    : {mode_color}{mode}{Colors.NC}")
    print("=" * 60)
    print("\nEscolhe um modo:")
    print(f"  {Colors.GREEN}1{Colors.NC}) Joystick Real (SHANWAN + toggle AUTO/MANUAL)")
    print(f"  {Colors.YELLOW}2{Colors.NC}) Testes Automáticos")
    print(f"  {Colors.CYAN}3{Colors.NC}) Modo Interativo (Teclado WASD)")
    print(f"  {Colors.RED}4{Colors.NC}) EMERGENCY STOP")
    print("  9) Sair")
    print("")


def main():
    import os
    if os.geteuid() != 0:
        print(f"{Colors.YELLOW}⚠ Aviso: execute como root (acesso a /dev/input){Colors.NC}\n")

    controller = VehicleController(SOCKET_PATH)

    try:
        while True:
            menu_principal(controller.mode)

            try:
                opcao = input(f"{Colors.BOLD}Opção: {Colors.NC}").strip()

                if opcao == '1':
                    modo_joystick(controller)

                elif opcao == '2':
                    modo_testes(controller)
                    input(f"\n{Colors.YELLOW}Pressiona ENTER para continuar...{Colors.NC}")

                elif opcao == '3':
                    modo_interativo(controller)

                elif opcao == '4':
                    modo_emergency_stop(controller)
                    input(f"\n{Colors.YELLOW}Pressiona ENTER para continuar...{Colors.NC}")

                elif opcao == '9':
                    print(f"\n{Colors.GREEN}A sair...{Colors.NC}")
                    break

                else:
                    print(f"{Colors.RED}Opção inválida!{Colors.NC}")
                    time.sleep(1)

            except KeyboardInterrupt:
                print(f"\n{Colors.YELLOW}Use opção 9 para sair{Colors.NC}")

    finally:
        controller.cleanup()


if __name__ == '__main__':
    print(f"\n{Colors.CYAN}Joystick Control v2 — socket → ADAS Manager{Colors.NC}")
    print(f"Socket: {SOCKET_PATH}\n")
    main()
