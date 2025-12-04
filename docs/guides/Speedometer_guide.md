# FULL ROADMAP ON HOW TO SET UP THE SPEEDOMETER TO COMMUNICATE WITH THE MICROCONTROLLER
----------------------------------------------------------------------------------------

# 1 STEP - Create a comprehensive pinout documentation and select a GPIO that corresponds to a timer channel (IC) in capture mode.
  - The timer channel in capture mode means that whenever an edge arrives on this GPIO pin, the timer value will be recorded.
  
  So now we choose a GPIO pin:

<img width="626" height="531" alt="image" src="https://github.com/user-attachments/assets/ee23bad9-25e3-4fff-bd28-8a159768b407" /> 
<img width="626" height="431" alt="image" src="https://github.com/user-attachments/assets/32673730-5d09-4e90-baa8-e182aad148c9" />

In this case, I will choose the PA8 (D9) with its timer channel set to TIM1_CH1.
  - Why did I choose this one? There are more pins to choose such as PB6, PE0, and PB2. I chose PA8 simply because the TIM1 is the highest grade timer on the MCU, making PA8 the best choice for measurement.
      - Being the highest grade timer, it gives you some perks:
          - Higher Resolution.
          - Better noise immunity.
          - More prescaler options.

# 2 STEP - Electrical connections.
  In this case, our Speedometer sensor is 3.3V, so it connects directly to the STM32 Board.
  
    STM32 Board 3.3v -> Sensor vcc.
  As for the Grounding, the sensor must share a common ground with the STM32.
  
     Sensor GND  ────┐
                      ├─── STM32 GND
    Power Supply GND ┘

  And finally, the OUT connects to the PA8(TIM1_CH1)
  
    Sensor OUT → PA8 (TIM1_CH1)

  With all said and done, make sure all the cables are stable to prevent damage and interference.

  Why 3.3v?, well:  
  - MCU-safe: STM32U585 GPIOs are 3.3 V devices. If the sensor is powered at 3.3V, you can connect its output directly to the MCU without level shifting.
  - Less hardware & lower risk: No level shifters, no resistor dividers, fewer failure modes.
  - Lower power consumption: Slightly less current draw in many sensors.
  - Sufficient logic margin: A 3.3 V logic high easily meets STM32 input high thresholds (typically ≈0.7·VDD).
  
# 3 STEP - Configure Timer Input Capture.
  In STM32CubeMX:
  
  1. Select TIM1 -> Channel 1 -> Input capture(Direct Mode).
  2. Set input polarity (rising edge) 
  3. Set Prescaler so timer resolution fits speed range.
  4. Enable NVIC interrupt for TIM1.
  5. Generate code and verify if it compiles correctly.

# 4 Step - Implement Pulse Measurement Logic
  # TO BE CONTINUED
