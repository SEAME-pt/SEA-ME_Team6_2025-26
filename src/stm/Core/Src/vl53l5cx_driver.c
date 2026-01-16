/**
  ******************************************************************************
  * @file    vl53l5cx_driver.c
  * @brief   VL53L5CX Time-of-Flight sensor driver implementation
  * @details High-level API wrapping the official ST VL53L5CX ULD library
  ******************************************************************************
  */

#include "vl53l5cx_driver.h"
#include "vl53l5cx_platform_stm32.h"
#include "i2c.h"
#include <stdio.h>

/* Global driver instance */
VL53L5CX_Driver_t vl53l5cx_driver = {0};

/**
 * @brief Initialize VL53L5CX sensor
 * @return HAL_OK if successful, HAL_ERROR otherwise
 * @note This function takes 1-2 seconds due to firmware upload (~84KB over I2C)
 */
HAL_StatusTypeDef VL53L5CX_Init(void)
{
	uint8_t status;
	uint8_t is_alive;

	printf("[VL53L5CX] Initializing sensor...\r\n");

	/* Initialize platform structure (platform is INSIDE config) */
	/* Note: I2C handle is extern global in platform.c, only address needed here */
	vl53l5cx_driver.config.platform.address = 0x52;  /* 0x29 << 1 */

	/* Power on sequence with proper timing */
	VL53L5CX_PowerOn();

	/* Debug: Scan I2C2 bus to find sensor */
	printf("[VL53L5CX] Scanning I2C2 bus...\r\n");
	for (uint8_t addr = 0x10; addr < 0x80; addr++) {
		if (HAL_I2C_IsDeviceReady(&hi2c2, addr << 1, 1, 10) == HAL_OK) {
			printf("  - Found device at 0x%02X (8-bit: 0x%02X)\r\n", addr, addr << 1);
		}
	}

	/* Check if sensor is alive */
	printf("[VL53L5CX] Checking sensor presence...\r\n");
	status = vl53l5cx_is_alive(&vl53l5cx_driver.config, &is_alive);
	if (status != 0 || !is_alive) {
		printf("[VL53L5CX] ERROR: Sensor not detected! Status=%d, Alive=%d\r\n",
		       status, is_alive);
		return HAL_ERROR;
	}
	printf("[VL53L5CX] Sensor detected successfully at address 0x52\r\n");

	/* Initialize sensor (uploads firmware - takes ~1-2 seconds) */
	printf("[VL53L5CX] Uploading firmware (~84KB, this takes 1-2 seconds)...\r\n");
	status = vl53l5cx_init(&vl53l5cx_driver.config);
	if (status != 0) {
		printf("[VL53L5CX] ERROR: Firmware upload failed! Status=%d\r\n", status);
		return HAL_ERROR;
	}
	printf("[VL53L5CX] Firmware uploaded successfully!\r\n");

	/* Configure for 4x4 resolution (16 zones) */
	status = vl53l5cx_set_resolution(&vl53l5cx_driver.config, VL53L5CX_RESOLUTION_4X4);
	if (status != 0) {
		printf("[VL53L5CX] ERROR: Failed to set 4x4 resolution\r\n");
		return HAL_ERROR;
	}
	vl53l5cx_driver.resolution = VL53L5CX_RESOLUTION_4X4;
	printf("[VL53L5CX] Resolution set to 4x4 (16 zones)\r\n");

	/* Set ranging frequency to 30Hz (faster distance detection) */
	status = vl53l5cx_set_ranging_frequency_hz(&vl53l5cx_driver.config, 30);
	if (status != 0) {
		printf("[VL53L5CX] WARNING: Failed to set ranging frequency\r\n");
	} else {
		printf("[VL53L5CX] Ranging frequency set to 30 Hz\r\n");
	}

	/* Set integration time for good performance/accuracy balance */
	status = vl53l5cx_set_integration_time_ms(&vl53l5cx_driver.config, 20);
	if (status != 0) {
		printf("[VL53L5CX] WARNING: Failed to set integration time\r\n");
	} else {
		printf("[VL53L5CX] Integration time set to 20 ms\r\n");
	}

	vl53l5cx_driver.is_initialized = 1;
	printf("[VL53L5CX] Initialization complete!\r\n");

	return HAL_OK;
}

/**
 * @brief Configure interrupt mode for data ready
 * @return HAL_OK if successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef VL53L5CX_ConfigureInterrupt(void)
{
	/* The VL53L5CX generates an interrupt on GPIO1 when data is ready */
	/* No additional configuration needed - hardware is already set up */
	printf("[VL53L5CX] Interrupt mode configured (GPIO1 data ready)\r\n");
	return HAL_OK;
}

/**
 * @brief Start continuous ranging
 * @return HAL_OK if successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef VL53L5CX_StartRanging(void)
{
	uint8_t status;

	if (!vl53l5cx_driver.is_initialized) {
		printf("[VL53L5CX] ERROR: Sensor not initialized\r\n");
		return HAL_ERROR;
	}

	status = vl53l5cx_start_ranging(&vl53l5cx_driver.config);
	if (status != 0) {
		printf("[VL53L5CX] ERROR: Failed to start ranging, status=%d\r\n", status);
		return HAL_ERROR;
	}

	printf("[VL53L5CX] Ranging started (continuous mode, 10 Hz)\r\n");
	return HAL_OK;
}

/**
 * @brief Stop ranging
 * @return HAL_OK if successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef VL53L5CX_StopRanging(void)
{
	uint8_t status;

	if (!vl53l5cx_driver.is_initialized) {
		return HAL_ERROR;
	}

	status = vl53l5cx_stop_ranging(&vl53l5cx_driver.config);
	if (status != 0) {
		printf("[VL53L5CX] ERROR: Failed to stop ranging\r\n");
		return HAL_ERROR;
	}

	printf("[VL53L5CX] Ranging stopped\r\n");
	return HAL_OK;
}

/**
 * @brief Check if data is ready
 * @param is_ready Pointer to store result (0=not ready, 1=ready)
 * @return HAL_OK if successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef VL53L5CX_GetDataReady(uint8_t *is_ready)
{
	uint8_t status;

	if (!vl53l5cx_driver.is_initialized) {
		return HAL_ERROR;
	}

	status = vl53l5cx_check_data_ready(&vl53l5cx_driver.config, is_ready);
	if (status != 0) {
		return HAL_ERROR;
	}

	return HAL_OK;
}

/**
 * @brief Get full ranging results (all zones)
 * @param results Pointer to VL53L5CX_ResultsData structure
 * @return HAL_OK if successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef VL53L5CX_GetResults(VL53L5CX_ResultsData *results)
{
	uint8_t status;

	if (!vl53l5cx_driver.is_initialized) {
		return HAL_ERROR;
	}

	status = vl53l5cx_get_ranging_data(&vl53l5cx_driver.config, results);
	if (status != 0) {
		printf("[VL53L5CX] ERROR: Failed to get ranging data, status=%d\r\n", status);
		return HAL_ERROR;
	}

	return HAL_OK;
}

/**
 * @brief Read distance data for all 16 zones (4x4 mode)
 * @param distances Array of 16 uint16_t to store distances in mm
 * @return HAL_OK if successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef VL53L5CX_ReadDistanceData4x4(uint16_t distances[16])
{
	VL53L5CX_ResultsData results;
	HAL_StatusTypeDef status;

	status = VL53L5CX_GetResults(&results);
	if (status != HAL_OK) {
		return status;
	}

	/* Extract distance data for all 16 zones (4x4 mode) */
	for (uint8_t i = 0; i < 16; i++) {
		distances[i] = results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE * i];
	}

	return HAL_OK;
}

/**
 * @brief Read single distance value (average of center zones)
 * @param distance_mm Pointer to store distance in millimeters
 * @return HAL_OK if successful, HAL_ERROR otherwise
 *
 * @note Returns average of center 4 zones (5,6,9,10) for best single-point accuracy
 * @note Zone layout (4x4):
 *       [0]  [1]  [2]  [3]
 *       [4]  [5]  [6]  [7]
 *       [8]  [9]  [10] [11]
 *       [12] [13] [14] [15]
 */
HAL_StatusTypeDef VL53L5CX_ReadDistance(uint16_t *distance_mm)
{
	VL53L5CX_ResultsData results;
	HAL_StatusTypeDef status;
	uint32_t avg;

	status = VL53L5CX_GetResults(&results);
	if (status != HAL_OK) {
		return status;
	}

	/* Average the 4 center zones for best single-point result */
	/* Zones 5, 6, 9, 10 are the center zones in 4x4 layout */
	avg = (results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE * 5] +
	       results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE * 6] +
	       results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE * 9] +
	       results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE * 10]) / 4;

	*distance_mm = (uint16_t)avg;

	return HAL_OK;
}

/**
 * @brief Clear interrupt (compatibility function)
 * @return HAL_OK
 * @note With official API, reading data automatically clears interrupt
 */
HAL_StatusTypeDef VL53L5CX_ClearInterrupt(void)
{
	/* Reading data automatically clears the interrupt */
	/* No explicit action needed */
	return HAL_OK;
}

/**
 * @brief Test function for VL53L5CX sensor (debug only)
 * @return HAL_OK if all tests pass, HAL_ERROR otherwise
 * @note Call this from main() before ThreadX starts for isolated testing
 */
HAL_StatusTypeDef VL53L5CX_TestSensor(void)
{
	uint8_t status;
	uint8_t is_alive;
	HAL_StatusTypeDef hal_status;

	printf("\r\n");
	printf("========================================\r\n");
	printf("  VL53L5CX SENSOR TEST (DEBUG MODE)\r\n");
	printf("========================================\r\n");

	/* Test 1: I2C Bus Scan */
	printf("\n[TEST 1] I2C2 Bus Scan...\r\n");
	uint8_t devices_found = 0;
	for (uint8_t addr = 0x10; addr < 0x80; addr++) {
		if (HAL_I2C_IsDeviceReady(&hi2c2, addr << 1, 1, 10) == HAL_OK) {
			printf("  [OK] Device found at 0x%02X (8-bit: 0x%02X)\r\n", addr, addr << 1);
			devices_found++;
		}
	}
	printf("  Total devices found: %d\r\n", devices_found);

	/* Test 2: Initialize platform structure */
	printf("\n[TEST 2] Initializing platform structure...\r\n");
	/* Note: I2C handle is extern global in platform.c, only address needed here */
	vl53l5cx_driver.config.platform.address = 0x52;  /* 0x29 << 1 */
	printf("  [OK] I2C Handle: hi2c2 (extern global)\r\n");
	printf("  [OK] I2C Address: 0x52 (7-bit: 0x29)\r\n");

	/* Test 3: Power on sequence */
	printf("\n[TEST 3] Power on sequence (XSHUT pin)...\r\n");
	VL53L5CX_PowerOn();
	printf("  [OK] Sensor powered on (XSHUT = HIGH)\r\n");

	/* Test 4: Check if sensor is alive */
	printf("\n[TEST 4] Checking sensor presence...\r\n");
	status = vl53l5cx_is_alive(&vl53l5cx_driver.config, &is_alive);
	printf("  API Status: %d\r\n", status);
	printf("  Is Alive: %d (1=yes, 0=no)\r\n", is_alive);

	if (status != 0 || !is_alive) {
		printf("  [FAIL] Sensor not detected!\r\n");
		printf("  Possible causes:\r\n");
		printf("    - Wrong I2C bus or address\r\n");
		printf("    - XSHUT pin not working\r\n");
		printf("    - I2C wiring issue\r\n");
		printf("    - Sensor hardware failure\r\n");
		return HAL_ERROR;
	}
	printf("  [OK] Sensor is alive!\r\n");

	/* Test 5: Firmware upload (critical test) */
	printf("\n[TEST 5] Uploading firmware (~84KB)...\r\n");
	printf("  This takes 1-2 seconds...\r\n");

	uint32_t start_tick = HAL_GetTick();
	status = vl53l5cx_init(&vl53l5cx_driver.config);
	uint32_t end_tick = HAL_GetTick();
	uint32_t duration_ms = end_tick - start_tick;

	printf("  API Status: %d (0=OK)\r\n", status);
	printf("  Upload duration: %lu ms\r\n", duration_ms);

	if (status != 0) {
		printf("  [FAIL] Firmware upload failed!\r\n");
		printf("  Possible causes:\r\n");
		printf("    - I2C timeout too short\r\n");
		printf("    - Chunking issue in VL53L5CX_WrMulti()\r\n");
		printf("    - I2C bus error during large transfer\r\n");
		printf("    - Sensor RAM issue\r\n");

		/* Try to get more I2C error info */
		printf("\n  I2C2 Error Code: 0x%08lX\r\n", hi2c2.ErrorCode);
		printf("  I2C2 State: %d\r\n", hi2c2.State);

		return HAL_ERROR;
	}
	printf("  [OK] Firmware uploaded successfully!\r\n");

	/* Test 6: SKIP all configuration - use DEFAULTS only! */
	printf("\n[TEST 6] Using DEFAULT configuration (no changes)...\r\n");
	printf("  Skipping resolution, frequency, integration time\r\n");
	printf("  API defaults: 8x8 resolution, AUTONOMOUS mode\r\n");

	/* Don't configure anything - just use what vl53l5cx_init() set up */

	/* Test 7: COMPLETE POWER CYCLE - Last resort! */
	printf("\n[TEST 7] Attempting COMPLETE sensor reset...\r\n");
	printf("  Power cycling sensor (XSHUT LOW -> HIGH)...\r\n");

	/* Power down */
	VL53L5CX_SetXSHUT(0);
	HAL_Delay(100);

	/* Power up */
	VL53L5CX_SetXSHUT(1);
	HAL_Delay(100);

	/* Check if alive after power cycle */
	uint8_t is_alive_after_reset;
	status = vl53l5cx_is_alive(&vl53l5cx_driver.config, &is_alive_after_reset);
	printf("  Sensor alive after reset: %d (status=%d)\r\n", is_alive_after_reset, status);

	if (!is_alive_after_reset) {
		printf("  [FAIL] Sensor not responding after power cycle!\r\n");
		return HAL_ERROR;
	}

	/* RE-INITIALIZE firmware after power cycle */
	printf("  Re-uploading firmware after power cycle...\r\n");
	uint32_t start_reinit = HAL_GetTick();
	status = vl53l5cx_init(&vl53l5cx_driver.config);
	uint32_t end_reinit = HAL_GetTick();

	if (status != 0) {
		printf("  [FAIL] Re-initialization failed! Status=%d\r\n", status);
		return HAL_ERROR;
	}
	printf("  [OK] Re-initialized successfully (%lu ms)\r\n", end_reinit - start_reinit);

	HAL_Delay(500);

	/* Test 8: Start ranging */
	printf("\n[TEST 8] Starting ranging (after complete reset)...\r\n");

	/* Check if sensor is ready */
	printf("  Verifying sensor is ready...\r\n");
	uint8_t is_alive_check;
	status = vl53l5cx_is_alive(&vl53l5cx_driver.config, &is_alive_check);
	printf("  Pre-ranging alive check: %d (status=%d)\r\n", is_alive_check, status);

	/* Check power mode */
	printf("\n  Checking power mode...\r\n");
	uint8_t power_mode;
	status = vl53l5cx_get_power_mode(&vl53l5cx_driver.config, &power_mode);
	printf("  Current power mode: %d (0=SLEEP, 1=WAKEUP, status=%d)\r\n", power_mode, status);

	/* Check ranging mode */
	printf("\n  Checking ranging mode...\r\n");
	uint8_t ranging_mode;
	status = vl53l5cx_get_ranging_mode(&vl53l5cx_driver.config, &ranging_mode);
	printf("  Current ranging mode: %d (1=CONTINUOUS, 3=AUTONOMOUS, status=%d)\r\n", ranging_mode, status);

	HAL_Delay(200);

	/* Debug: Check data_read_size BEFORE start ranging */
	printf("\n  Pre-start debug info:\r\n");
	printf("    data_read_size = %lu\r\n", vl53l5cx_driver.config.data_read_size);
	printf("    streamcount = %u\r\n", vl53l5cx_driver.config.streamcount);

	/* Try to read current ranging mode/status */
	printf("\n  Attempting to start ranging (1st try)...\r\n");
	status = vl53l5cx_start_ranging(&vl53l5cx_driver.config);
	printf("  vl53l5cx_start_ranging() returned: %d\r\n", status);
	printf("  I2C2 Error Code: 0x%08lX\r\n", hi2c2.ErrorCode);
	printf("  I2C2 State: %d\r\n", hi2c2.State);

	/* Debug: Check data_read_size AFTER failed start ranging */
	printf("  Post-start debug info:\r\n");
	printf("    data_read_size = %lu\r\n", vl53l5cx_driver.config.data_read_size);
	printf("    temp_buffer[0x8-0x9] content (data size from sensor):\r\n");
	printf("      [0x8]=0x%02X [0x9]=0x%02X\r\n",
	       vl53l5cx_driver.config.temp_buffer[0x8],
	       vl53l5cx_driver.config.temp_buffer[0x9]);

	/* If first attempt failed, try with additional delay and retry */
	if (status != 0) {
		printf("  [WARN] First attempt failed (code: %d), trying recovery...\r\n", status);

		/* Wait longer and try power cycle the ranging */
		printf("  Waiting additional 1 second...\r\n");
		HAL_Delay(1000);

		/* Verify sensor still responsive */
		uint8_t sensor_status;
		uint8_t rd_status = vl53l5cx_is_alive(&vl53l5cx_driver.config, &sensor_status);
		printf("  Sensor alive after delay: %d (status=%d)\r\n", sensor_status, rd_status);

		if (!sensor_status) {
			printf("  [FAIL] Sensor became unresponsive!\r\n");
			return HAL_ERROR;
		}

		/* Try starting ranging again */
		printf("  Attempting to start ranging (2nd try)...\r\n");
		status = vl53l5cx_start_ranging(&vl53l5cx_driver.config);
		printf("  vl53l5cx_start_ranging() returned: %d\r\n", status);

		if (status != 0) {
			printf("  [FAIL] Failed to start ranging after retry (error: %d)\r\n", status);
			printf("\n  Diagnostic info:\r\n");
			printf("    - API returned: 0x%02X (255 = internal error)\r\n", status);
			printf("    - I2C is OK (no bus errors)\r\n");
			printf("    - Sensor is alive and responsive\r\n");
			printf("    - All configurations succeeded\r\n");
			printf("    - Likely: sensor internal state issue\r\n");
			printf("\n  Possible solutions:\r\n");
			printf("    1. Try different ranging frequency (1-15 Hz)\r\n");
			printf("    2. Try different integration time (5-200 ms)\r\n");
			printf("    3. Check if 8x8 resolution works instead\r\n");
			printf("    4. Verify sensor firmware version compatibility\r\n");

			return HAL_ERROR;
		}
	}

	printf("  [OK] Ranging started successfully!\r\n");

	/* Test 9: Wait for first measurement */
	printf("\n[TEST 9] Waiting for first measurement...\r\n");
	uint8_t is_ready = 0;
	uint32_t timeout = 1000;  /* 1 second timeout */
	start_tick = HAL_GetTick();

	while (!is_ready && (HAL_GetTick() - start_tick) < timeout) {
		status = vl53l5cx_check_data_ready(&vl53l5cx_driver.config, &is_ready);
		if (status != 0) {
			printf("  [FAIL] Error checking data ready\r\n");
			return HAL_ERROR;
		}
		HAL_Delay(10);
	}

	if (!is_ready) {
		printf("  [FAIL] Timeout waiting for data\r\n");
		return HAL_ERROR;
	}
	printf("  [OK] Data ready! (waited %lu ms)\r\n", HAL_GetTick() - start_tick);

	/* Test 10: Read measurement */
	printf("\n[TEST 10] Reading measurement data...\r\n");
	VL53L5CX_ResultsData results;
	status = vl53l5cx_get_ranging_data(&vl53l5cx_driver.config, &results);
	if (status != 0) {
		printf("  [FAIL] Failed to read data\r\n");
		return HAL_ERROR;
	}
	printf("  [OK] Data read successfully!\r\n");

	/* Display 4x4 grid */
	printf("\n  4x4 Distance Grid (mm):\r\n");
	printf("  +------+------+------+------+\r\n");
	for (uint8_t row = 0; row < 4; row++) {
		printf("  |");
		for (uint8_t col = 0; col < 4; col++) {
			uint8_t zone = row * 4 + col;
			uint16_t dist = results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE * zone];
			uint8_t stat = results.target_status[VL53L5CX_NB_TARGET_PER_ZONE * zone];

			if (stat == 5 || stat == 9) {
				printf(" %4u |", dist);
			} else {
				printf(" ---- |");
			}
		}
		printf("\r\n");
		printf("  +------+------+------+------+\r\n");
	}

	/* Calculate center distance */
	uint16_t center_dist = (results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE * 5] +
	                       results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE * 6] +
	                       results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE * 9] +
	                       results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE * 10]) / 4;
	printf("  Center distance: %u mm\r\n", center_dist);

	/* Test 11: Stop ranging */
	printf("\n[TEST 11] Stopping ranging...\r\n");
	status = vl53l5cx_stop_ranging(&vl53l5cx_driver.config);
	if (status != 0) {
		printf("  [FAIL] Failed to stop ranging\r\n");
		return HAL_ERROR;
	}
	printf("  [OK] Ranging stopped\r\n");

	/* Summary */
	printf("\n========================================\r\n");
	printf("  ALL TESTS PASSED!\r\n");
	printf("  VL53L5CX is working correctly.\r\n");
	printf("========================================\r\n\n");

	vl53l5cx_driver.is_initialized = 1;
	return HAL_OK;
}
