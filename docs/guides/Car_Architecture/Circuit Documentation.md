# **Circuit Documentation**

## **Summary**

This document provides a detailed overview of a complex electronic circuit designed to manage power distribution, control motors, and interface with various sensors and communication modules. The circuit includes a battery management system, a Raspberry Pi for processing, motor drivers, sensors, and communication interfaces. The circuit is designed to be powered by a 12.6V battery pack and includes step-down converters to supply 5V to various components.

## **Component List**

1. **12.6V 3S 20A 18650 BMS Protection Board Module**  
   * Description: Manages and protects a 3-cell lithium battery pack.  
   * Pins: GND, 4.2V, 8.4V, \+, \-, 12.6V  
2. **Toggle Switch**  
   * Description: A simple on/off switch to control power flow.  
   * Pins: L1, COM, L2  
3. **Relay Module 3V 1 Channel**  
   * Description: Allows control of a high-power circuit with a low-power signal.  
   * Pins: NC, COM, NO, VCC, IN, GND  
4. **2.1mm Barrel Jack with Terminal Block**  
   * Description: Provides a connection point for external power supply.  
   * Pins: POS, NEG  
5. **INA 226**  
   * Description: A current and power monitor with I2C interface.  
   * Pins: VCC, GND, SDA, SCL, ALERT, V IN, V OUT  
6. **Raspberry Pi 5**  
   * Description: A single-board computer for processing and control.  
   * Pins: Type-C, Micro HDMI 1, Micro HDMI 2, Camera 1, Camera 2, PoE, Fan, PCIe, USB 3.0, USB 2.0, Ethernet, 5V, GND, 3.3v, GPIO 14, GPIO 15, GPIO 18, GPIO 23, GPIO 24, GPIO 25, GPIO 8, GPIO 7, GPIO 1, GPIO 12, GPIO 16, GPIO 20, GPIO 21, GPIO 2, GPIO 3, GPIO 4, GPIO 17, GPIO 27, GPIO 22, GPIO 10, GPIO 9, GPIO 11, GPIO 0, GPIO 5, GPIO 6, GPIO 13, GPIO 19, GPIO 26  
7. **MCP2515**  
   * Description: A CAN controller with SPI interface.  
   * Pins: INT, SCK, SI, SO, CS, GND, VCC, H, L, can H, can L  
8. **Step-down 12 To 5V 5A XL4015**  
   * Description: Converts 12V to 5V for powering 5V components.  
   * Pins: OUT \-, OUT \+, IN \-, IN \+  
9. **DC Motor\_small**  
   * Description: A small DC motor for mechanical movement.  
   * Pins: pin 1, pin 2  
10. **Servo**  
    * Description: A servo motor for precise control of angular position.  
    * Pins: GND, VCC, PWM  
11. **2-Channel CAN-BUS(FD) HAT (MCP2518FD)**  
    * Description: A CAN bus interface for communication.  
    * Pins: 1\_H, 1\_L, GND, 0\_H, 0\_L, VIN  
12. **Grove \- I2C Motor Driver (TB6612FNG)**  
    * Description: Controls two DC motors via I2C interface.  
    * Pins: Motor\_B1, Motor\_B2, Motor\_A1, Motor\_A2, VM, GND, VCC, SDA, SCL, OUT4, OUT3, OUT2, OUT1  
13. **USB C to 2 fils**  
    * Description: Provides power connection via USB-C.  
    * Pins: USB C, \+, \-  
14. **Raspi Cam 3**  
    * Description: A camera module for Raspberry Pi.  
    * Pins: GND, CAM1\_DN0, CAM1\_DP0, CAM1\_DN1, CAM1\_CN, CAM1\_CP, CAM1\_DP1, CAM\_GPIO, CAM\_CLK, SDA0, \+3.3V, SCL0  
15. **Sensor Optocoupler Disk Speed**  
    * Description: Measures rotational speed using an optocoupler.  
    * Pins: OUT, GND, 5V  
16. **Gravity: I2C HUB**  
    * Description: Expands I2C connections for multiple devices.  
    * Pins: SDA, SCL, GND, VCC  
17. **splicing connector 6**  
    * Description: Connects multiple wires together.  
    * Pins: 1, 2, 3, 4, 5, 6  
18. **stepdown 24v to 5v**  
    * Description: Converts 24V to 5V for powering 5V components.  
    * Pins: IN+, IN-, OUT+, OUT-  
19. **3x 3.7V Battery**  
    * Description: A battery pack consisting of three 3.7V cells.  
    * Pins: Batt1+, Batt3-, Batt2+, Batt3+, Batt2-, Batt1-  
20. **SRF08 Ultrasonic Range Finder**  
    * Description: Measures distance using ultrasonic waves.  
    * Pins: GND, SCL, SDA, \+5V  
21. **Keyestudio KS0064 \- Matrix LED 8x8 I2C (HT16K33)**  
    * Description: An 8x8 LED matrix display with I2C interface.  
    * Pins: VCC, GND, SDA, SCL  
22. **B-U585I-IOT02A**  
    * Description: A microcontroller board for IoT applications.  
    * Pins: CN13\_10, CN13\_9, CN13\_8, CN13\_7, CN13\_6, CN13\_5, CN13\_4, CN13\_3, CN13\_2, CN13\_1, CN14\_1, CN14\_2, CN14\_3, CN14\_4, CN14\_5, CN14\_6, CN14\_7, CN14\_8, CN16\_1, CN16\_2, CN16\_3, CN16\_4, CN16\_5, CN16\_6, CN17\_8, CN17\_7, CN17\_6, CN17\_5, CN17\_4, CN17\_3, CN17\_2, CN17\_1

## ---

**Wiring Details**

### **12.6V 3S 20A 18650 BMS Protection Board Module**

* **GND** connected to **Batt3-** of 3x 3.7V Battery.  
* **4.2V** connected to **Batt2+** and **Batt3+** of 3x 3.7V Battery.  
* **8.4V** connected to **Batt2-** and **Batt1-** of 3x 3.7V Battery.  
* **12.6V** connected to **Batt1+** of 3x 3.7V Battery.  
* **\+** connected to **COM** of Toggle Switch and **POS** of 2.1mm Barrel Jack with Terminal Block.  
* **\-** connected to **V OUT** of INA 226 and **1** of splicing connector 6\.

### **Toggle Switch**

* **COM** connected to **\+** of 12.6V 3S 20A 18650 BMS Protection Board Module.  
* **L2** connected to **V IN** and **GND** of INA 226\.

### **Relay Module 3V 1 Channel**

* **COM** connected to **OUT+** of stepdown 24v to 5v.  
* **GND** connected to **CN17\_7** of B-U585I-IOT02A.  
* **VCC** connected to **CN17\_4** of B-U585I-IOT02A.  
* **IN** connected to **CN14\_8** of B-U585I-IOT02A.  
* **NO** connected to **VCC** of Grove \- I2C Motor Driver (TB6612FNG).

### **2.1mm Barrel Jack with Terminal Block**

* **POS** connected to **\+** of 12.6V 3S 20A 18650 BMS Protection Board Module.  
* **NEG** connected to **1** of splicing connector 6 and **V OUT** of INA 226\.

### **INA 226**

* **V IN** connected to **L2** of Toggle Switch.  
* **GND** connected to **5** of splicing connector 6\.  
* **SDA** connected to **SDA** of Gravity: I2C HUB.  
* **SCL** connected to **SCL** of Gravity: I2C HUB.  
* **V OUT** connected to **\-** of 12.6V 3S 20A 18650 BMS Protection Board Module and **1** of splicing connector 6\.

### **Raspberry Pi 5**

* **Type-C** connected to **USB C** of USB C to 2 fils.  
* **Camera 1** connected to **CAM1\_CP** of Raspi Cam 3\.

### **MCP2515**

* **L** connected to **0\_H** of 2-Channel CAN-BUS(FD) HAT (MCP2518FD).  
* **H** connected to **0\_L** of 2-Channel CAN-BUS(FD) HAT (MCP2518FD).  
* **INT** connected to **CN14\_5** of B-U585I-IOT02A.  
* **SCK** connected to **CN13\_6** of B-U585I-IOT02A.  
* **SI** connected to **CN13\_4** of B-U585I-IOT02A.  
* **SO** connected to **CN13\_5** of B-U585I-IOT02A.  
* **CS** connected to **CN13\_3** of B-U585I-IOT02A.  
* **GND** connected to **CN17\_6** of B-U585I-IOT02A.  
* **VCC** connected to **CN17\_5** of B-U585I-IOT02A.

### **Step-down 12 To 5V 5A XL4015**

* **OUT \+** connected to **VCC** of INA 226, **\+** of USB C to 2 fils, and **VCC** of Servo.  
* **OUT \-** connected to **\-** of USB C to 2 fils.  
* **IN \+** connected to **GND** of INA 226, **IN+** of stepdown 24v to 5v, and **VM** of Grove \- I2C Motor Driver (TB6612FNG).  
* **IN \-** connected to **2** of splicing connector 6\.

### **DC Motor\_small**

* **pin 1** connected to **Motor\_B1** of Grove \- I2C Motor Driver (TB6612FNG).  
* **pin 2** connected to **Motor\_B2** of Grove \- I2C Motor Driver (TB6612FNG).

### **Servo**

* **GND** connected to **6** of splicing connector 6\.  
* **VCC** connected to **OUT \+** of Step-down 12 To 5V 5A XL4015.  
* **PWM** connected to **CN13\_2** of B-U585I-IOT02A.

### **2-Channel CAN-BUS(FD) HAT (MCP2518FD)**

* **0\_H** connected to **L** of MCP2515.  
* **0\_L** connected to **H** of MCP2515.  
* **GND** connected to **CN13\_7** of B-U585I-IOT02A.

### **Grove \- I2C Motor Driver (TB6612FNG)**

* **Motor\_B1** connected to **pin 1** of DC Motor\_small.  
* **Motor\_B2** connected to **pin 2** of DC Motor\_small.  
* **Motor\_A1** connected to **pin 1** of another DC Motor\_small.  
* **Motor\_A2** connected to **pin 2** of another DC Motor\_small.  
* **VM** connected to **IN \+** of Step-down 12 To 5V 5A XL4015.  
* **GND** connected to **6** and **4** of splicing connector 6\.  
* **VCC** connected to **NO** of Relay Module 3V 1 Channel.  
* **SDA** connected to **SDA** of Gravity: I2C HUB.  
* **SCL** connected to **SCL** of Gravity: I2C HUB.

### **USB C to 2 fils**

* **USB C** connected to **Type-C** of Raspberry Pi 5\.  
* **\+** connected to **OUT \+** of Step-down 12 To 5V 5A XL4015.  
* **\-** connected to **OUT \-** of Step-down 12 To 5V 5A XL4015.

### **Raspi Cam 3**

* **CAM1\_CP** connected to **Camera 1** of Raspberry Pi 5\.

### **Sensor Optocoupler Disk Speed**

* **OUT** connected to **CN14\_7** of B-U585I-IOT02A.  
* **GND** connected to **4** of splicing connector 6\.  
* **5V** connected to **OUT+** of stepdown 24v to 5v.

### **Gravity: I2C HUB**

* **SDA** connected to **SDA** of Grove \- I2C Motor Driver (TB6612FNG), **SDA** of INA 226, **SDA** of SRF08 Ultrasonic Range Finder, **SDA** of Keyestudio KS0064 \- Matrix LED 8x8 I2C (HT16K33).  
* **SCL** connected to **SCL** of Grove \- I2C Motor Driver (TB6612FNG), **SCL** of INA 226, **SCL** of SRF08 Ultrasonic Range Finder, **SCL** of Keyestudio KS0064 \- Matrix LED 8x8 I2C (HT16K33).  
* **GND** connected to **1** of splicing connector 6\.  
* **VCC** connected to **OUT+** of stepdown 24v to 5v.

### **splicing connector 6**

* **1** connected to **NEG** of 2.1mm Barrel Jack with Terminal Block, **V OUT** of INA 226, **GND** of Gravity: I2C HUB.  
* **2** connected to **OUT-** of stepdown 24v to 5v, **IN \-** of Step-down 12 To 5V 5A XL4015.  
* **3** connected to **IN-** of stepdown 24v to 5v.  
* **4** connected to **GND** of Grove \- I2C Motor Driver (TB6612FNG), **GND** of Sensor Optocoupler Disk Speed.  
* **5** connected to **GND** of INA 226\.  
* **6** connected to **GND** of Servo, **GND** of Grove \- I2C Motor Driver (TB6612FNG).

### **stepdown 24v to 5v**

* **IN+** connected to **IN \+** of Step-down 12 To 5V 5A XL4015.  
* **IN-** connected to **3** of splicing connector 6\.  
* **OUT+** connected to **VCC** of Gravity: I2C HUB, **\+5V** of SRF08 Ultrasonic Range Finder, **VCC** of Keyestudio KS0064 \- Matrix LED 8x8 I2C (HT16K33), **COM** of Relay Module 3V 1 Channel, **5V** of Sensor Optocoupler Disk Speed, **VCC** of another Keyestudio KS0064 \- Matrix LED 8x8 I2C (HT16K33).  
* **OUT-** connected to **2** of splicing connector 6\.

### **3x 3.7V Battery**

* **Batt1+** connected to **12.6V** of 12.6V 3S 20A 18650 BMS Protection Board Module.  
* **Batt3-** connected to **GND** of 12.6V 3S 20A 18650 BMS Protection Board Module.  
* **Batt2+** connected to **4.2V** of 12.6V 3S 20A 18650 BMS Protection Board Module.  
* **Batt3+** connected to **4.2V** of 12.6V 3S 20A 18650 BMS Protection Board Module.  
* **Batt2-** connected to **8.4V** of 12.6V 3S 20A 18650 BMS Protection Board Module.  
* **Batt1-** connected to **8.4V** of 12.6V 3S 20A 18650 BMS Protection Board Module.

### **SRF08 Ultrasonic Range Finder**

* **GND** connected to **5** of splicing connector 6\.  
* **SCL** connected to **SCL** of Gravity: I2C HUB.  
* **SDA** connected to **SDA** of Gravity: I2C HUB.  
* **\+5V** connected to **OUT+** of stepdown 24v to 5v.

### **Keyestudio KS0064 \- Matrix LED 8x8 I2C (HT16K33)**

* **VCC** connected to **OUT+** of stepdown 24v to 5v.  
* **GND** connected to **1** of splicing connector 6\.  
* **SDA** connected to **SDA** of Gravity: I2C HUB.  
* **SCL** connected to **SCL** of Gravity: I2C HUB.

