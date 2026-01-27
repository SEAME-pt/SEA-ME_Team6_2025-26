# Combined Test - DC Motors and Servo Control

---

## Table of Contents
1. [Introduction](#introduction)
2. [Custom Meta-Layer (meta-seame)](#custom-meta-layer-meta-seame)
3. [local.conf Configuration](#localconf-configuration)
4. [Installed Components](#installed-components)
5. [Network Configuration](#network-configuration)
6. [U-Boot Bypass](#u-boot-bypass)
7. [Automatic WiFi Support](#automatic-wifi-support)
8. [USB/Joystick Support](#usbjoystick-support)
9. [Build and Deploy](#build-and-deploy)
10. [Troubleshooting](#troubleshooting)

---

## Introduction
This script implements a combined test that simultaneously controls two DC motors (through the TB6612FNG driver) and a servo motor during a period of 20 minutes. The servo moves continuously between minimum and maximum positions while the motors rotate forward.
Implementation
Main Loop Integration
The test is called in the main program loop:


## Required Configurations
Constants (define before main)
```
#define SERVO_MIN_PULSE  500   // Servo minimum position
#define SERVO_MID_PULSE  1500  // Servo center position
#define SERVO_MAX_PULSE  2500  // Servo maximum position
```


## Inside while(1)
```
printf("\r\n>>> Combined test (wheels + servo) for 20 minutes <<<\r\n");
TestCombined();
HAL_Delay(2000);
```
## Main Function
```
void TestCombined(void)
{
    HAL_StatusTypeDef statusA, statusB;
    uint32_t startTime;
    int servoDirection = 1;  // 1 = para max, -1 = para min
    uint16_t servoPos = SERVO_MID_PULSE;
    uint16_t servoStep = 50;  // Incremento por iteração

    printf("\r\n========================================\r\n");
    printf("Teste COMBINADO: Rodas + Servo (10 segundos)\r\n");
    printf("========================================\r\n");

    /* Liga os dois motores para a frente */
    printf("-> Ligando motores para a frente...\r\n");
    statusA = TB6612FNG_MotorRun(&motorDriver, MOTOR_CHA, 255);
    HAL_Delay(10);  // Delay entre comandos I2C
    statusB = TB6612FNG_MotorRun(&motorDriver, MOTOR_CHB, 255);
    printf("   Motor A: %s, Motor B: %s\r\n",
           (statusA == HAL_OK) ? "OK" : "ERRO",
           (statusB == HAL_OK) ? "OK" : "ERRO");

    startTime = HAL_GetTick();

    /* Loop durante 10 segundos */
    while ((HAL_GetTick() - startTime) < 1200000)
    {
        /* Actualiza posição do servo */
        servoPos += servoDirection * servoStep;

        /* Inverte direcção nos limites */
        if (servoPos >= SERVO_MAX_PULSE) {
            servoPos = SERVO_MAX_PULSE;
            servoDirection = -1;
        } else if (servoPos <= SERVO_MIN_PULSE) {
            servoPos = SERVO_MIN_PULSE;
            servoDirection = 1;
        }

        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, servoPos);

        /* Pequeno delay para movimento suave */
        HAL_Delay(50);
    }

    /* Para tudo */
    printf("-> 10 segundos terminados! A parar...\r\n");
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, SERVO_MID_PULSE);
    TB6612FNG_MotorBrake(&motorDriver, MOTOR_CHA);
    HAL_Delay(10);
    TB6612FNG_MotorBrake(&motorDriver, MOTOR_CHB);

    printf("Teste combinado completo!\r\n");
}
```
