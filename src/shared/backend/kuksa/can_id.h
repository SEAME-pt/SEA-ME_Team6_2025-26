/*
 * can_frames.h
 *
 * Same Header File for STM32 and AGL - Common definitions for AGL <-> STM32
 *
 * Created on: Dec 28, 2025
 *     Author: rcosta-c
 */

#ifndef INC_CAN_FRAMES_BAK_
#define INC_CAN_FRAMES_BAK_

#include <stdint.h>


/* CAN IDs*/

#define CAN_ID_EMERGENCY_STOP	0x001

/* Commands (AGL -> STM32) */
#define CAN_ID_MOTOR_CMD        0x200
#define CAN_ID_CONFIG_CMD       0x201

/* Status (STM32 -> AGL) */
#define CAN_ID_MOTOR_STATUS     0x210
#define CAN_ID_SERVO_STATUS     0x211
#define CAN_ID_SYSTEM_STATE     0x212

/* Sensors Fast (STM32 -> AGL) */
#define CAN_ID_IMU_ACCEL        0x400
#define CAN_ID_IMU_GYRO         0x401
#define CAN_ID_IMU_MAG          0x402
#define CAN_ID_WHEEL_SPEED      0x403

/* Sensors Slow (STM32 -> AGL) */
#define CAN_ID_ENVIRONMENT      0x420
#define CAN_ID_BATTERY          0x421
#define CAN_ID_TOF_DISTANCE     0x422

/* Joystick (temporary)*/
#define CAN_ID_JOYSTICK			0x500

/* Heartbeat */
#define CAN_ID_HEARTBEAT_AGL    0x700
#define CAN_ID_HEARTBEAT_STM32  0x701

/* Diagnostics */
#define CAN_ID_DIAG_REQUEST     0x7E0
#define CAN_ID_DIAG_RESPONSE    0x7E8

/* Others */
#define CAN_ID_CMD_RELAY		0x801


/*============================================================================
 * Enums
 *============================================================================*/

typedef enum {
    DRIVE_MODE_IDLE = 0,
    DRIVE_MODE_MANUAL = 1,
    DRIVE_MODE_AUTONOMOUS = 2,
    DRIVE_MODE_CALIBRATION = 3
} DriveMode_t;

typedef enum {
    SYSTEM_STATE_BOOT = 0,
    SYSTEM_STATE_INIT = 1,
    SYSTEM_STATE_READY = 2,
    SYSTEM_STATE_RUNNING = 3,
    SYSTEM_STATE_ERROR = 4,
    SYSTEM_STATE_SAFE = 5
} SystemState_t;

/* Error flags (bitfield) */
#define ERROR_FLAG_CAN_TIMEOUT    (1 << 0)
#define ERROR_FLAG_SENSOR_FAULT   (1 << 1)
#define ERROR_FLAG_MOTOR_FAULT    (1 << 2)
#define ERROR_FLAG_WATCHDOG       (1 << 3)
#define ERROR_FLAG_OVERVOLTAGE    (1 << 4)
#define ERROR_FLAG_UNDERVOLTAGE   (1 << 5)

/* Command flags (bitfield) */
#define CMD_FLAG_ENABLE           (1 << 0)
#define CMD_FLAG_BRAKE            (1 << 1)
#define CMD_FLAG_EMERGENCY_STOP   (1 << 2)


/* Timing Configuration */

#define CAN_PERIOD_MOTOR_CMD_MS      20   /* 50 Hz */
#define CAN_PERIOD_MOTOR_STATUS_MS   10   /* 100 Hz */
#define CAN_PERIOD_IMU_FAST_MS       10   /* 100 Hz */
#define CAN_PERIOD_IMU_MAG_MS        20   /* 50 Hz */
#define CAN_PERIOD_WHEEL_SPEED_MS    100  /* 10 Hz */
#define CAN_PERIOD_TOF_MS            20   /* 50 Hz */
#define CAN_PERIOD_ENVIRONMENT_MS    1000 /* 1 Hz */
#define CAN_PERIOD_BATTERY_MS        500  /* 2 Hz */
#define CAN_PERIOD_HEARTBEAT_MS      100  /* 10 Hz */

#define CAN_TIMEOUT_COMMAND_MS       50   /* Safe state after 50ms */
#define CAN_TIMEOUT_HEARTBEAT_MS     300  /* Node dead after 300ms */


/* Frame Structures */

/* Emergency Stop (0x001) - Bi-directional */
typedef struct __attribute__((packed)) {
    uint8_t  active;        /* 1 = Emergency active, 0 = Clear */
    uint8_t  source;        /* 0=ToF, 1=AGL, 2=Manual, 3=Watchdog */
    uint16_t distance_mm;   /* Distance to obstacle (if ToF) */
    uint8_t  reason;        /* Reason code */
    uint8_t  reserved[2];
    uint8_t  crc;
} EmergencyStop_t;


/* Motor Command (0x200) - AGL -> STM32 */
typedef struct __attribute__((packed)) {
    int8_t   throttle;      /* -100 to +100 (%) */
    int8_t   steering;      /* -100 to +100 (%) */
    uint8_t  flags;         /* CMD_FLAG_* */
    uint8_t  mode;          /* DriveMode_t */
    uint8_t  reserved[2];
    uint8_t  counter;       /* Rolling 0-14 */
    uint8_t  crc;           /* CRC-8 */
} MotorCmd_t;

/* Motor Status (0x210) - STM32 -> AGL */
typedef struct __attribute__((packed)) {
    int8_t   actual_throttle;   /* -100 to +100 (%) applied */
    int8_t   actual_steering;   /* -100 to +100 (%) applied */
    uint16_t motor_current_ma;  /* Motor current in mA */
    int8_t   driver_temp;       /* TB6612 temperature °C */
    uint8_t  pwm_duty;          /* Actual PWM duty 0-255 */
    uint8_t  counter;
    uint8_t  crc;
} MotorStatus_t;

/* IMU Accelerometer (0x400) - STM32 -> AGL */
typedef struct __attribute__((packed)) {
    int16_t  acc_x;         /* milli-g */
    int16_t  acc_y;         /* milli-g */
    int16_t  acc_z;         /* milli-g */
    uint8_t  reserved;
    uint8_t  status;        /* Sensor status flags */
} ImuAccel_t;

/* IMU Gyroscope (0x401) - STM32 -> AGL */
typedef struct __attribute__((packed)) {
    int16_t  gyro_x;        /* 0.1 °/s */
    int16_t  gyro_y;        /* 0.1 °/s */
    int16_t  gyro_z;        /* 0.1 °/s */
    uint8_t  reserved;
    uint8_t  status;
} ImuGyro_t;

/* IMU Magnetometer (0x402) - STM32 -> AGL */
typedef struct __attribute__((packed)) {
    int16_t  mag_x;         /* milli-Gauss */
    int16_t  mag_y;         /* milli-Gauss */
    int16_t  mag_z;         /* milli-Gauss */
    uint8_t  reserved;
    uint8_t  status;
} ImuMag_t;

/* Wheel Speed (0x403) - STM32 -> AGL */
typedef struct __attribute__((packed)) {
    int16_t  rpm;           /* Signed RPM */
    uint32_t total_pulses;  /* Odometry counter */
    uint8_t  direction;     /* 0=forward, 1=reverse */
    uint8_t  status;
} WheelSpeed_t;

/* Environment (0x420) - STM32 -> AGL */
typedef struct __attribute__((packed)) {
    int16_t  temperature;       /* 0.01 °C */
    uint8_t  humidity;          /* 0-100 % */
    uint8_t  ambient_light_x100;/* Ambient light x100 (0-255 -> 0-25500 lux) */
    uint32_t pressure : 24;     /* Pa */
    uint8_t  status : 8;        /* Sensor status flags */
} Environment_t;

/* Battery Status (0x421) - STM32 -> AGL */
typedef struct __attribute__((packed)) {
    uint16_t voltage_mv;    /* mV */
    int16_t  current_ma;    /* mA (negative = discharge) */
    uint8_t  soc;           /* State of charge % */
    int8_t   temperature;   /* °C */
    uint8_t  cycles;        /* Charge cycles */
    uint8_t  status;
} BatteryStatus_t;

/* ToF Distance (0x422) - STM32 -> AGL */
typedef struct __attribute__((packed)) {
    uint16_t min_distance_mm;   /* Minimum distance detected (mm) */
    uint8_t  nearest_zone;      /* Zone index where min was detected (0-63, 255=none) */
    uint8_t  target_status;     /* VL53L5CX target status (5/9=valid) */
    uint8_t  detection_count;   /* Number of valid targets detected */
    uint8_t  reserved[2];
    uint8_t  status;            /* Sensor status flags */
} ToFDistance_t;

/* Heartbeat (0x700/0x701) */
typedef struct __attribute__((packed)) {
    uint8_t  state;         /* SystemState_t */
    uint32_t uptime_ms;     /* Milliseconds since boot */
    uint8_t  errors;        /* ERROR_FLAG_* */
    uint8_t  mode;          /* DriveMode_t */
    uint8_t  crc;
} Heartbeat_t;





#endif /* INC_CAN_FRAMES_BAK_ */
