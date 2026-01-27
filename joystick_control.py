#!/usr/bin/env python3
"""
Joystick CAN Control - Tudo-em-um
Controla o veículo via CAN usando joystick SHANWAN ou testes manuais

Configuração:
  - CAN Interface: can1
  - Joystick: /dev/input/event4 (SHANWAN Android Gamepad)
  - CAN ID: 0x500
  - Bitrate: 500kbps

Uso:
  sudo python3 joystick_control.py
"""

import can
import struct
import time
import sys
import select
import termios
import tty

# Tentar importar evdev (só necessário para modo joystick)
try:
    import evdev
    from evdev import ecodes
    EVDEV_AVAILABLE = True
except ImportError:
    EVDEV_AVAILABLE = False

# Configuração
CAN_INTERFACE = 'can1'
JOYSTICK_DEVICE = '/dev/input/event4'
CAN_ID_JOYSTICK = 0x500

class Colors:
    RED = '\033[0;31m'
    GREEN = '\033[0;32m'
    BLUE = '\033[0;34m'
    YELLOW = '\033[1;33m'
    CYAN = '\033[0;36m'
    BOLD = '\033[1m'
    NC = '\033[0m'

class VehicleController:
    def __init__(self, can_interface=CAN_INTERFACE):
        self.can_interface = can_interface
        self.steering = 0
        self.throttle = 0

        # Inicializar CAN bus
        try:
            self.can_bus = can.interface.Bus(
                channel=can_interface,
                bustype='socketcan',
                bitrate=500000
            )
            print(f"{Colors.GREEN}✓ CAN {can_interface} inicializado (500kbps){Colors.NC}")
        except Exception as e:
            print(f"{Colors.RED}✗ Erro ao inicializar CAN: {e}{Colors.NC}")
            print(f"\nExecute antes:")
            print(f"  sudo ip link set {can_interface} type can bitrate 500000")
            print(f"  sudo ip link set {can_interface} up")
            sys.exit(1)

    def send_command(self, steering, throttle):
        """Envia comando de controlo via CAN"""
        # Limitar valores
        steering = max(-100, min(100, steering))
        throttle = max(-100, min(100, throttle))

        # Empacotar como int16 little-endian
        data = struct.pack('<hh', steering, throttle)

        # Criar e enviar mensagem
        msg = can.Message(
            arbitration_id=CAN_ID_JOYSTICK,
            data=data,
            is_extended_id=False
        )

        try:
            self.can_bus.send(msg)
            return True
        except can.CanError as e:
            print(f"{Colors.RED}Erro CAN: {e}{Colors.NC}")
            return False

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

        steer_bar = make_bar(steering)
        throt_bar = make_bar(throttle)

        print(f"\r{Colors.CYAN}Steering[{steer_bar}]{steering:4d}  "
              f"Throttle[{throt_bar}]{throttle:4d}{Colors.NC}  ",
              end='', flush=True)

    def stop(self):
        """Para o veículo"""
        self.send_command(0, 0)
        print(f"\n{Colors.YELLOW}→ STOP enviado{Colors.NC}")

    def cleanup(self):
        """Limpar recursos"""
        self.stop()
        self.can_bus.shutdown()
        print(f"{Colors.GREEN}✓ CAN fechado{Colors.NC}")

# =============================================================================
# MODO 1: JOYSTICK REAL
# =============================================================================

def modo_joystick(controller):
    """Controlo com joystick real"""

    if not EVDEV_AVAILABLE:
        print(f"{Colors.RED}✗ Biblioteca 'evdev' não instalada!{Colors.NC}")
        print("Instale com: pip3 install evdev")
        return

    # Encontrar joystick
    try:
        joystick = evdev.InputDevice(JOYSTICK_DEVICE)
        print(f"{Colors.GREEN}✓ Joystick: {joystick.name}{Colors.NC}")
        print(f"  Device: {JOYSTICK_DEVICE}")
    except Exception as e:
        print(f"{Colors.RED}✗ Joystick não encontrado em {JOYSTICK_DEVICE}{Colors.NC}")
        print("\nDispositivos disponíveis:")
        for path in evdev.list_devices():
            dev = evdev.InputDevice(path)
            print(f"  {path}: {dev.name}")
        return

    print(f"\n{Colors.BOLD}=== CONTROLO POR JOYSTICK ==={Colors.NC}")
    print("Controles:")
    print("  Stick Direito (horizontal) → Steering")
    print("  Stick Esquerdo (vertical)   → Throttle")
    print("  Ctrl+C para sair\n")

    dead_zone = 5
    steering = 0
    throttle = 0

    def normalize_axis(raw_value, invert=False):
        """Normaliza eixo do SHANWAN (0..255, centro=127) para -100..100
        SHANWAN usa 8-bit unsigned:
          - Mínimo: 0
          - Centro: 127
          - Máximo: 255
        """
        normalized = int(((raw_value - 127) / 127.0) * 100)
        if abs(normalized) < dead_zone:
            normalized = 0
        if invert:
            normalized = -normalized
        return max(-100, min(100, normalized))

    try:
        for event in joystick.read_loop():
            if event.type == ecodes.EV_ABS:

                # Eixo X (esquerda/direita) - Steering
                if event.code == ecodes.ABS_Z:
                    steering = normalize_axis(event.value)
                    controller.send_command(steering, throttle)
                    controller.print_status(steering, throttle)

                # Eixo Y (cima/baixo) - Throttle
                elif event.code == ecodes.ABS_Y:
                    throttle = normalize_axis(event.value, invert=True)
                    controller.send_command(steering, throttle)
                    controller.print_status(steering, throttle)

    except KeyboardInterrupt:
        print(f"\n{Colors.GREEN}✓ Joystick desconectado{Colors.NC}")

# =============================================================================
# MODO 2: TESTES AUTOMÁTICOS
# =============================================================================

def modo_testes(controller):
    """Executa sequência de testes automáticos"""

    print(f"\n{Colors.BOLD}=== TESTES AUTOMÁTICOS ==={Colors.NC}\n")

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
        (-50, 50, "Frente + Esquerda"),
        (50, 50, "Frente + Direita"),
        (-50, -50, "Trás + Esquerda"),
        (50, -50, "Trás + Direita"),
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
    """Controlo via teclado (WASD)"""

    print(f"\n{Colors.BOLD}=== MODO INTERATIVO (WASD) ==={Colors.NC}")
    print("Controles:")
    print("  W/S - Throttle (frente/trás)")
    print("  A/D - Steering (esquerda/direita)")
    print("  Espaço - STOP")
    print("  Q - Sair\n")

    steering = 0
    throttle = 0
    step = 10

    # Configurar terminal
    old_settings = termios.tcgetattr(sys.stdin)
    try:
        tty.setcbreak(sys.stdin.fileno())

        while True:
            # Ler tecla
            if select.select([sys.stdin], [], [], 0.1)[0]:
                key = sys.stdin.read(1).lower()

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
                elif key == 'q':
                    break
                else:
                    continue

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
    print(f"\n{Colors.RED}{Colors.BOLD}🛑 EMERGENCY STOP 🛑{Colors.NC}")
    for _ in range(3):
        controller.send_command(0, 0)
    print(f"{Colors.GREEN}✓ Comando STOP enviado 3x{Colors.NC}")
    time.sleep(0.5)

# =============================================================================
# MENU PRINCIPAL
# =============================================================================

def menu_principal():
    """Menu principal"""

    print("\n" + "="*60)
    print(f"{Colors.BOLD}      JOYSTICK CAN CONTROL - Team6 Project{Colors.NC}")
    print("="*60)
    print(f"CAN Interface: {Colors.CYAN}{CAN_INTERFACE}{Colors.NC}")
    print(f"Joystick:      {Colors.CYAN}{JOYSTICK_DEVICE}{Colors.NC} (SHANWAN)")
    print(f"CAN ID:        {Colors.CYAN}0x{CAN_ID_JOYSTICK:03X}{Colors.NC}")
    print("="*60)
    print("\nEscolhe um modo:")
    print(f"  {Colors.GREEN}1{Colors.NC}) Joystick Real (SHANWAN Android Gamepad)")
    print(f"  {Colors.YELLOW}2{Colors.NC}) Testes Automáticos")
    print(f"  {Colors.CYAN}3{Colors.NC}) Modo Interativo (Teclado WASD)")
    print(f"  {Colors.RED}4{Colors.NC}) EMERGENCY STOP")
    print(f"  {Colors.BLUE}5{Colors.NC}) Monitor CAN")
    print("  9) Sair")
    print("")

def modo_monitor(controller):
    """Monitor de mensagens CAN"""
    print(f"\n{Colors.BLUE}Monitorando CAN (Ctrl+C para sair)...{Colors.NC}\n")

    try:
        while True:
            msg = controller.can_bus.recv(timeout=1.0)
            if msg and msg.arbitration_id == CAN_ID_JOYSTICK:
                if len(msg.data) >= 4:
                    steering, throttle = struct.unpack('<hh', msg.data[:4])
                    print(f"{Colors.CYAN}[0x{msg.arbitration_id:03X}]{Colors.NC} "
                          f"Steering={steering:4d}, Throttle={throttle:4d} "
                          f"Data: {msg.data.hex().upper()}")
    except KeyboardInterrupt:
        print(f"\n{Colors.GREEN}Monitor encerrado{Colors.NC}")

def main():
    """Loop principal"""

    # Criar controller
    controller = VehicleController(CAN_INTERFACE)

    try:
        while True:
            menu_principal()

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

                elif opcao == '5':
                    modo_monitor(controller)

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
    print(f"\n{Colors.CYAN}Joystick CAN Control - Team6{Colors.NC}")
    print(f"Interface: {CAN_INTERFACE} | Joystick: {JOYSTICK_DEVICE}\n")

    # Verificar se está a correr como root
    import os
    if os.geteuid() != 0:
        print(f"{Colors.YELLOW}⚠ Aviso: Execute como root para garantir acesso ao CAN e joystick{Colors.NC}")
        print(f"  sudo python3 {sys.argv[0]}\n")

    main()
