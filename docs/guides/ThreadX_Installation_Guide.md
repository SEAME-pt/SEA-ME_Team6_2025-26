# 🧵 Installing ThreadX on an STM32 – Complete Guide
---------------------------
# ✅ 1. What You Need
  # Software
  - STM32CubeIDE.
  - STM32CubeMX.
    
  # Hardware
  - An STM32 development board.
---------------------------
# 🧱 2. Enable ThreadX Using STM32CubeMX
  **Step 1 — Create a New Project**
  1. Open **STM32CubeMX**
  2. Select your **MCU** or **board**
  3. Configure your clocks (or accept defaults)
     
  **Step 2 — Enable Azure RTOS / ThreadX**
  1. Go to Middleware in the left sidebar
  2. Enable Azure RTOS → ThreadX

  **Step 3 — Configure ThreadX**
  
  Under Middleware → ThreadX, configure:
  - Tick rate (usually 1000 Hz).
  - Preemption threshold.
  - Timer task priority.

# 🧰 3. Build and Run in STM32CubeIDE

  1. Open the generated project in STM32CubeIDE.
  2. Build the project.
  3. Flash it to your STM32 using the Run button.

# 🧪 4. Verify ThreadX Is Running

  1. Enable a GPIO Output for the LED.
  - Pick a pin.
  - Mode: **GPIO_Output**
  - Name: LED_Pin

  2. Add a ThreadX Application Thread.

      In CubeMX → ThreadX → Application\
       Create a thread:
     - Name: blink_thread
     - Entry function: blink_thread_entry
     - Priority: 1
     - Stack size: 1024
       
  3. Implement the Blink Thread
     
      in app_threadx.c
        ```c
        void blink_thread_entry(ULONG thread_input)
        {
            while (1)
            {
                HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
                tx_thread_sleep(100); // 100 ticks → ~100 ms blink
            }
        }

This makes the LED toggle every ~0.1 seconds.

  4. Ensure ThreadX starts in main.c

     CubeMX does this automatically:
      ```c
        int main(void)
        {
            HAL_Init();
            SystemClock_Config();
            MX_GPIO_Init();
            MX_ThreadX_Init();    // IMPORTANT: starts ThreadX kernel
        
            while (1)
            {
            }
        }
  If everything is working:

✔️ ThreadX starts.

✔️ The scheduler runs your thread.

✔️ You see the LED blinking on the board.
