# CAN implementation

![CAN Scheme - New update.png](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/demos/CAN%20Scheme%20-%20New%20update.png)
Scheme of our wiring

The objective is to communicate via CAN from the STM32 to the Raspberry Pi (and vice versa). 
To do that follow the following steps:

1. Connect the wiring like the image above
2. Connect micro-usb to CN8 (STLINK)
3. Open STM32CubeIDE
	1.  1. Go to 'File' -> 'New' -> 'STM32 Project'
	2. Select the Board on the 'Board Selector'
	3. Common problems at this stage:
	    1. ST-Link not connected: Unplug the MCU and plug it back in :)
	    2. GDB not encountered: Go to STM32CubeIDE → Preferences -> STM32Cube → Toolchain Manager (it will update automatically the libraries that are needed)
4. Configure Pinout in the STM32CubeIDE
	1. Double click the .ioc file
	2. For the speedometer:
		1. Select the PB0 pin and select mode: GPIO_EXTI
		2. On that pin, select: External Interrupt Mode with **Rising edge** trigger and **Pull:** **Pull-Down**
		3. In the NVIC tab select EXTI line interrupts -> enable
	3. For the MCP2515 (CAN module):
		1. Pinout -> Connectivity -> SPI1: Mode: Full Duplex Master
		2. Configuration tab -> SPI1 -> verify if:
			   - Mode: Full Duplex Master
			   - Hardware NSS Signal: Disabled (use manual CS via GPIO)
		3. Parameters:
			1. Prescaler: SPI_BAUDRATEPRESCALER_16
			2. **Clock Polarity (CPOL):** `Low`
			3. **Clock Phase (CPHA):** `1 Edge`
			4. **CRC Calculation:** _Disabled_
			5. **Data Size:** `8 bits`
		4. NSS Signal Type: Software (NSS = Soft)
		5. GPIO for CS -> Click on PE12
			1. Mode: GPIO Output
			2. Go to 'System Core' in the left tab -> GPIO
			3. **GPIO Output Level:** “High” (so CS is inactive by default)
			4. **GPIO Pull-up/Pull-down:** “No pull-up and no pull-down”
			5. **Maximum Output Speed:** “Low” (fine for CS)
			6. **User Label**: CS_PIN
			7. Problems:
				1. If there isn't #define GPIO_PIN_12 / GPIOE (search in main.h)
				2. You need to define it yourself
				3. #define CS_PIN_Pin GPIO_PIN_12
				4. #define CS_PIN_GPIO_Port GPIOE
		6. INT in PE7
			1. Mode: GPIO EXTI7
			2. External Interrupt Mode with Falling Edge
			3. Pull: No pull-down and no pull-up
			4. In the NVIC tab select EXTI line interrupts -> enable
	4. Configure CNF - time
		1. CAN module 8 MHz -> 500 kbps (this is done by code)
```	
			//Setting for MCP2515 @ 8MHz, 500 kbits/s:
			CNF1 = 0x00
			CNF2 = 0x90
			CNF3 = 0x02
```
5. Need to manually configure the SPI1 pins in the STM32
	1. This includes assigning the following pins in the Pinout View in CubeMX:
	2. PE13: SPI1_SCK
	3. PE14: SPI1_MISO
	4. PE15: SPI1_MOSI
	5. PE12: GPIO_Output 
6. Click in **Generate Code** (it will create files needed (.c/.h) for the running test on the microcontroller)  
	1. Problems at this stage:
	    1. If it doesn't create .h files (like gpio.h), it is needed to go to configurations -> create the pair (.h/.c) files
7. Create `mcp2515.h` and `mcp2515.c` (the code will be attached) 
8. After this, you will need to make sure that:
	1. `main.c` includes `mcp2515.h`
	2. `mcp2515.h` includes `stm32u5xx_hal_conf.h`
	3. `mcp2515.c` includes `mcp2515.h`
	4. `main.h` includes `stm32u5xx_hal.h`
