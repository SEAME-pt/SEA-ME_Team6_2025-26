/*
 * thread_config.h
 *
 *  Created on: Dec 18, 2025
 *      Author: rcosta-c
 *
 *  Description: Centralized configuration for all ThreadX threads
 *               - Thread priorities (lower number = higher priority)
 *               - Stack sizes
 */

#ifndef INC_THREAD_CONFIG_H_
#define INC_THREAD_CONFIG_H_

/* ===========================================================================
 * THREAD PRIORITIES
 * ===========================================================================
 * ThreadX Priority Levels:
 *   - Priority 0 = Highest priority (most urgent)
 *   - Priority 31 = Lowest priority (least urgent)
 *   - Lower number = Higher priority
 *
 * Recommended Priority Assignment:
 *   - Critical/Safety: 0-5
 *   - High Priority:   6-10
 *   - Medium Priority: 11-15
 *   - Low Priority:    16-20
 *   - Background:      21-31
 */

/* Critical Threads */
#define HEARTBEAT_THREAD_PRIORITY     10    /* System heartbeat monitoring */

/* High Priority Threads */
#define CAN_RX_THREAD_PRIORITY        11    /* CAN message reception and actuation */
#define SRF08_THREAD_PRIORITY         11    /* SRF08 ultrasonic distance sensor - CRITICAL SAFETY */
#define SPEED_THREAD_PRIORITY         12    /* Vehicle speed measurement */

/* Medium Priority Threads */
#define IMU_THREAD_PRIORITY           13    /* IMU sensor (accel + gyro) */
#define TOF_THREAD_PRIORITY           14    /* Time-of-Flight distance sensor */
#define TEMP_THREAD_PRIORITY          15    /* Temperature sensor */

/* Cruise Control Thread */
#define CC_THREAD_PRIORITY            12    /* Same as speed — needs fresh speed data */

/* Low Priority Threads */
#define INDICATOR_THREAD_PRIORITY     16    /* KS0064 indicator lights */

/* Background Threads */
/* Add background threads here if needed */


/* ===========================================================================
 * THREAD STACK SIZES
 * ===========================================================================
 * Stack sizes in bytes. Adjust based on thread complexity and local variables.
 * Typical ranges:
 *   - Simple threads: 512-1024 bytes
 *   - Medium threads: 1024-2048 bytes
 *   - Complex threads: 2048-4096 bytes
 */

#define HEARTBEAT_THREAD_STACK_SIZE   2048
#define CAN_RX_THREAD_STACK_SIZE      2048
#define TEMP_THREAD_STACK_SIZE        2048
#define SPEED_THREAD_STACK_SIZE       2048
#define IMU_THREAD_STACK_SIZE         2048
#define TOF_THREAD_STACK_SIZE         4096
#define SRF08_THREAD_STACK_SIZE       2048
#define CC_THREAD_STACK_SIZE          2048  /* float printf needs ~600B; was 1024 (too small) */
#define AEB_THREAD_STACK_SIZE         2048  /* float printf in task_aeb; was 1024 hardcoded */
#define BATTERY_THREAD_STACK_SIZE     2048  /* INA226 I2C + printf; was 1024 hardcoded */
#define INDICATOR_THREAD_STACK_SIZE   1024


/* ===========================================================================
 * THREAD TIME SLICES (optional)
 * ===========================================================================
 * Time slice in ticks for round-robin scheduling.
 * Set to TX_NO_TIME_SLICE for FIFO scheduling (default).
 */

/* Currently using TX_NO_TIME_SLICE for all threads */


#endif /* INC_THREAD_CONFIG_H_ */
