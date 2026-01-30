/********************************************************************************
*
* Copyright (c) 2018 STMicroelectronics - All Rights Reserved
*
* License terms: STMicroelectronics Proprietary in accordance with licensing
* terms at www.st.com/sla0081
*
* STMicroelectronics confidential
* Reproduction and Communication of this document is strictly prohibited unless
* specifically authorized in writing by STMicroelectronics.
*
*******************************************************************************/


#ifndef HT_H_
#define HT_H_

#include "htgd_platform.h"

#include "sensor.h"
#include "ring_fbuffer_vm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HT_VERSION "1.1.0"

/** @brief Hand Tracker utility for multi-zone ToF sensors
 *  @par Description
    Extracts a hand position (x,y,z) in the metric space from sensor rangings
 *  @{
 */

/* Hand Tracking default parameters */
#define HT_LIFETIME_MS 		1000
#define HT_LIFE_SUPPORT_MS	250
#define HT_BOX_MM			2000
#define HT_MIN_ZONES 		1
#define HT_OBJECT_GAP_MM	100
#define HT_FULL_FOV_MM		300 //250
#define HT_FOCUS_FINGERS	true

/**
 * @struct HandTracker params
 * @brief Key parameters of the tracker
 */
typedef struct {

	int lifetime_ms;        // Time during which we keep value in history
	int life_support_ms;    // Time during which we try to keep path integrity
	int detection_box_mm;   // Max distance for a zone to be included
	int min_zones;          // Min number of zones to be tracked
	int object_gap_mm;      // Min z distance between different object
	int fov_full_cover_mm;  // Distance where a hand covers full field of view
	bool focus_fingers;     // Should we apply the filter to focus on fingers

} HT_params_t;

/**
 * @struct HandTracker hand
 * @brief Information about an extracted hand
 */
typedef struct {

	float hand_x;                   // X position in mm, 0 is center of field of view
	float hand_y;                   // Y position in mm, 0 is center of field of view
	float hand_z;                   // Z position in mm, 0 is the sensor

	float ranging[SEN_MAX_ZONES];   // Ranging data which used for extraction
	bool  hand_map[SEN_MAX_ZONES];  // Which zones where part of the hand

	bool found;                     // Does the scene contain a hand
	bool subject_changed;           // Is the hand the same as the previous one found

	long timestamp_ms;              // When was the hand found

} HT_hand_t;

/**
 * @struct HandTracker path
 * @brief Hand movement history
 */
typedef struct {

	int length;              // Number of iterations we store
	int recovery;            // Max number of iterations we have to recover path
	int last_hand;           // Last time we found a hand

	ring_fbuffer_vm hand_x;  // History of X positions, most recent at the end
	ring_fbuffer_vm hand_y;  // History of Y positions, most recent at the end
	ring_fbuffer_vm hand_z;  // History of Z positions, most recent at the end

} HT_path_t;

/**
 * @struct HandTracker processor
 * @brief Container of all information necessary to track
 */
typedef struct {

	HT_params_t params;

	HT_hand_t hand;

	HT_path_t path;

} HT_proc_t;


/****************************
 ****                    ****
 **     MAIN INTERFACE	   **
 ****                    ****
 ****************************/

/**
 * @brief Initialize Hand Tracker
 * @param pProc  HandTracker processor pointer
 * @param pData  Sensor data pointer
 * @return 0 on success, -2 if pointer is NULL
 */
int HT_init(HT_proc_t* pProc, SEN_data_t* pData);

/**
 * @brief Find Hand in sensor data
 * @param pProc  HandTracker processor pointer
 * @param pData  Sensor data pointer
 * @return 0 on success, -2 if pointer is NULL
 */
int HT_run(HT_proc_t* pProc, SEN_data_t* pData);


/****************************
 ****                    ****
 **    HELPER FUNCTIONS    **
 ****                    ****
 ****************************/

/**
 * @brief Extracts hand from data and updates the processor's hand info
 * @param pProc  HandTracker processor pointer
 * @param pData  Sensor data pointer
 * @return 0 on success, -2 if pointer is NULL, -4 if there is no data
 */
int HT_find_hand(HT_proc_t* pProc, SEN_data_t* pSensorData);

/**
 * @brief Saves the new hand position into the buffer
 * @param pProc  HandTracker processor pointer
 * @return 0 on success, -2 if pointer is NULL
 */
int HT_update_path(HT_proc_t* pProc);

/**
 * @brief Recovers the path of hand, interpolating between last known position and newly found one
 * @param pPath  HandTracker path pointer
 * @param x      New hand x position
 * @param y      New hand y position
 * @param z      New hand z position
 * @return 0 on success, -2 if pointer is NULL, -8 if we can't write values to buffers
 */
int HT_path_interpolate(HT_path_t* pPath, float x, float y, float z);

/**
 * @brief Resets the hand data buffers before first use
 * @param pHand    HandTracker hand pointer
 * @param nb_zones Nb of zones to init
 * @return 0 on success, -2 if pointer is NULL, -4 if nb_zones is invalid
 */
int HT_struct_init(HT_hand_t* pHand, int nb_zones);


/****************************
 ****                    ****
 **         SETTERS 	   **
 ****                    ****
 ****************************/

/**
 * @brief Sets the values of the processor parameters, sets to default if invalid
 * @param pProc    HandTracker processor pointer
 * @param lifetime Time to save hand movements
 * @param life_support_time Time to recover path
 * @param detection_box Distance to take zone into account
 * @param min_zones Minimum number of zones for a hand
 * @param object_gap Gap between objects
 * @param fov_full_cover Distance for hand to cover the field of view
 * @param focus_fingers Do we focus on fingertips
 * @return 0 on success, -2 if pointer is NULL
 */
int HT_set_params(HT_proc_t* pProc, int lifetime,
									int life_support_time,
									int detection_box,
									int min_zones,
									int object_gap,
									int fov_full_cover,
									int focus_fingers);

/**
 * @brief Sets frequency and converts times into iteration numbers
 * @param pProc    HandTracker processor pointer
 * @param pData    Sensor data pointer
 * @param freq     Frequency of ranging
 * @return 0 on success, -2 if pointer is NULL, -6 if frequency is invalid, -7 if lifetime and lifesupport are contradictory
 */
int HT_set_frequency(HT_proc_t* pProc, SEN_data_t* pData, float freq);

/**
 * @brief Sets the hand paths history buffer length
 * @param pPath    HandTracker path pointer
 * @param length   Number of iterations to save
 * @return 0 on success, -2 if pointer is NULL, -4 if length is invalid
 */
int HT_set_path_length(HT_path_t* pPath, int length);

/**
 * @brief Sets the sensor resolution and initializes structures
 * @param pProc    HandTracker processor pointer
 * @param pData    Sensor data pointer
 * @param res_x    Horizontal resolution
 * @param res_y    Vertical resolution
 * @return 0 on success, -2 if pointer is NULL, -4 if any resolution is invalid
 */
int HT_set_resolution(HT_proc_t* pProc, SEN_data_t* pData, int res_x, int res_y);

/**
 * @brief Overloads the set_resolution function for square resolutions
 * @param pProc    HandTracker processor pointer
 * @param pData    Sensor data pointer
 * @param nb_zones Total number of zones
 * @return 0 on success, -2 if pointer is NULL, -4 if nb_zones is invalid
 */
int HT_set_square_resolution(HT_proc_t* pProc, SEN_data_t* pData, int nb_zones);


#ifdef __cplusplus
}					
#endif	
#endif
