# STM32 + MCP2551 + Speed Sensor


1. Installed STM32CubeIDE
	1. Install STLINK and other dependencies in the installation
2. Connect micro-usb to CN8 (STLINK)
3. Open STM32CubeIDE
	1. Go to 'File' -> 'New' -> 'STM32 Project'
	2. Select the Board on the 'Board Selector'
4. Common problems at this stage:
	1. ST-Link not connected: Unplug the MCU and plug it back in :)
	2. GDB not encountered: Go to STM32CubeIDE → Preferences -> STM32Cube → Toolchain Manager (it will update automatically the libraries that are needed)
5. Connect Speed sensor to MCU
	1. VCC to 3.3V; GND to GND; DO to PB0;
6. Configure Pinout in the STM32CubeIDE
	1. Double click the .ioc file
	2. Select the PB0 pin and select mode: GPIO_EXTI
	3. On that pin, select: External Interrupt Mode with **Rising edge** trigger and **Pull:** **Pull-Down**
	4. In the NVIC tab select EXTI line interrupts -> enable
	5. Click in **Generate Code** (it will create files needed (.c/.h) for the running test on the microcontroller)
    <br>Problems at this stage:
        1. If it doesn't create .h files (like gpio.h), it is needed to go to configurations -> create the pair (.h/.c) files
