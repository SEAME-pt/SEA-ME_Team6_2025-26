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


#ifndef GW_H_
#define GW_H_

#include "htgd_platform.h"

#include "sensor.h"
#include "htmz.h"
#include "ring_fbuffer_vm.h"

#include "opti_math.h"

#ifdef __cplusplus
extern "C" {
#endif


#define GW_VERSION "1.9.4"


#define DOUBLE_TAP_DETECTION_ALGO

//#define IGNORE_OTHER_MOTION_IF_ASSOCIATED_WITH_TAP

/** @brief Gesture Prediction utility for untoggled detection
 *  @par Description
    Predicts linear hand gestures based on speed
 *  @{
 */

/* Gesture Wild default parameters */
#define GW_ANALYSIS_TIME_MS     200
#define GW_DEAD_TIME_MS         1000
#define GW_SCREENING_TIME_MS    600
#define GW_CLOSER_DELTA_MM      20
#define GW_MIN_SPEED_X_MM_S     500
#define GW_MIN_SPEED_Y_MM_S     600
#define GW_MIN_SPEED_Z_MM_S     600
#define GW_MAX_SPEED_MM_S       2000
#define GW_MAX_DISTANCE_MM		850
#define GW_MIN_VX_ON_VY         (float)(0.417)
#define GW_MIN_VX_ON_VZ         (float)(1)
#define GW_MIN_VY_ON_VX         (float)(1.621)
#define GW_MIN_VY_ON_VZ         (float)(1.2)
#define GW_MIN_VZ_ON_VX         (float)(2)
#define GW_MIN_VZ_ON_VY         (float)(1.118)
#define GW_PRECISE_SPEED        0

#define LC_STABLE_THRESHOLD 		(50 * 50)
#define LC_STABLE_TIME_THRESHOLD 	(1000)
#define LC_MAXDISTANCE_MM			(400)
#define LC_MINDISTANCE_MM			(100)
#ifdef DOUBLE_TAP_DETECTION_ALGO
#define DOUBLE_TAP_TS_DIFF_THRESHOLD 250
#endif


#define GW_LABEL_SINGLE_ONLY    1
#define GW_LABEL_IGNORE_MASK    47	//0b00101111
#define GW_LABEL_X_MASK         3	//0b000011
#define GW_LABEL_Y_MASK	        12	//0b001100
#define GW_LABEL_Z_MASK	        48	//0b110000

#define GW_DETECTION_LIMITATION 95 // = 0x5F = 0x0101 1111b = DOUBLETAP | 0 | TOWARD | UP | DOWN | RIGHT | LEFT
#define GW_DEAD_ZONE 50

#define GW_MIN_USER_FILTERING_MM 	300
#define GW_MAX_USER_FILTERING_MM 	GW_MAX_DISTANCE_MM + GW_FILTERING_AREA_MM
#define GW_FILTERING_AREA_MM		150


/**
 * @enum GestureWild state
 * @brief In which state the predictor is
 */
typedef enum {
	GW_DETECTION = 100,  // Looking for a hand
	GW_TRACKING  = 200   // Tracking hand
} GW_state_t;

/**
 * @enum GestureWild gesture labels
 * @brief To use as prediction, can be combined, filtered through masks
 */
typedef enum {
	GW_NONE   		= 0,	//No gesture
	GW_LEFT   		= 1,	//Swipe to the left
	GW_RIGHT  		= 2,	//Swipe to the right
	GW_DOWN   		= 3,	//Swipe down
	GW_UP     		= 4,	//Swipe up
	GW_TOWARD 		= 5,	//Tap (Toward the sensor)
	GW_AWAY   		= 6,	//Away from the sensor
	GW_DOUBLETAP   	= 7,	//Double Tap (Toward - Away - Toward)
} GW_label_t;


/**
 * @enum Level Control states
 * @brief To use as condition
 */
typedef enum {
	LC_NONE   			= 0,  // 0   No hand found
	LC_HAND_STABLE     	= 1,  // 1   Hand found and stable over the time
	LC_UNDER_CONTROL    = 2,  // 2   Level under controlled
} GW_LevelControlState_t;


/**
 * @struct GestureWild parameters
 * @brief Key parameters of the predictor
 */
typedef struct {

	int detectionLimitation;// Gesture mode
	int detectionMask;
	int analysis_ms;        // Time window to compute speed in ms
	int analysis_periods;   // Time window to compute speed in iterations
	int dead_ms;            // Duration which no new gesture can be predicted
	int screening_ms;       // Duration needed with no other prediction to lock current
	int closer_mm;          // Distance difference to raise closer flag
	int min_speed_x_mm_s;   // Min speed of hand on x axis for a gesture
	int min_speed_y_mm_s;   // Min speed of hand on y axis for a gesture
	int min_speed_z_mm_s;   // Min speed of hand on z axis for a gesture
	int max_speed_mm_s;     // Max speed of hand for a gesture
	int max_distance_mm;	// Max distance on Z-axis for a gesture
	float min_ratio_vx[2];  // Ratios of vx/vy and vx/vz that vx need to exceed
	float min_ratio_vy[2];  // Ratios of vy/vx and vy/vz that vy need to exceed
	float min_ratio_vz[2];  // Ratios of vz/vx and vz/vy that vz need to exceed
	bool precise_speed;     // Compute with timestamps (slower, precise), or with constant frequency assumption (faster)
	//User Filtering
	int min_user_filtering_mm;		// min distance to locate the user
	int max_user_filtering_mm;		// max distance to locate the user
	int filtering_area_mm;			// area ahead the user where gestures will be filtered

	//Level Control
	float lc_stable_threshold;
	long lc_stable_time_threshold;
	int lc_maxDistance_mm;  // Distance associated to 100% level
	int lc_minDistance_mm;  // Distance associated to   0% level
#ifdef DOUBLE_TAP_DETECTION_ALGO
	long double_tap_ts_threshold;
#endif
} GW_params_t;

/**
 * @struct GestureWild computer
 * @brief Intermediate variables used to predict
 */
typedef struct {

	ring_fbuffer_vm _buff_hand_x;  // Hand x path to compute speed
	ring_fbuffer_vm _buff_hand_y;  // Hand y path to compute speed
	ring_fbuffer_vm _buff_hand_z;  // Hand z path to compute speed
	ring_fbuffer_vm _buff_time;    // Hand timestamps to compute speed

	GW_label_t _label;             // Prediction label
	float _distance_mm;            // Prediction distance
	float _user_distance;		   // Current User distance
	bool _closer;                  // Prediction was closer
	bool _consistent;              // Prediction was consistent
	bool _dead;                    // Current was dead
	bool _ignore;                  // Prediction was ignored
	bool _same;                    // Prediction was same
	bool _single;                  // Prediction was singular
	bool _valid;                   // Current is valid

	//Level Control
	ring_fbuffer_vm lc_levelControlList; //ring of level control values
	float_t lc_gain;                     // gain to map [minDistance_mm:maxDistance_mm] to [0%:100%]
	ring_fbuffer_vm lc_PolarlevelControlList_r; //ring of level control Polar r values
	ring_fbuffer_vm lc_PolarlevelControlList_theta; //ring of level control Polar theta values

} GW_comp_t;

/**
 * @struct GestureWild gesture predicted
 * @brief Result of a prediction
 */
typedef struct {

	GW_label_t label;    // Label of gesture

	long start_time_ms;  // Time when gesture started
	long end_time_ms;    // Time when gesture ended

	bool current;        // Is the gesture ongoing
	bool probation;      // Is the gesture screened
	bool ready;          // Is the gesture to be used
	float distance_mm;   // Distance at which the gesture was made
	float user_distance;// User distance when Gesture was made

	//Level Control
	uint32_t lc_currentLevel;	// Current level value in percent
	float lc_polar_r;			// r in polar coordonates
	float lc_polar_theta;

} GW_gesture_t;

/**
 * @struct GestureWild processor
 * @brief Result of a prediction
 */
typedef struct {

	GW_params_t params;            // Predictor parameters

	GW_state_t state;              // Current state of predictor

	float speed_x_mm_s;            // Hand speed on x-axis
	float speed_y_mm_s;            // Hand speed on y-axis
	float speed_z_mm_s;            // Hand speed on z-axis

	GW_comp_t _computer;           // Results of computations

	GW_gesture_t gesture;          // Current predicted gesture

	GW_LevelControlState_t lc_state;	// Level control state
	long lc_start_time;					// Level control timestamp


#ifdef DOUBLE_TAP_DETECTION_ALGO
	long ts_current; //Current Frame timestamp
	long ts_toward; //Timestamp of Toward Motion
	long ts_away; //Timestamp of Away Motion
	long ts_diff_toward_away; //Timestamp difference between Toward and Away
	int away_label_is_true; //Away Motion detected
	int double_tap_is_true; //Double Tap detected
#endif


} GW_proc_t;


/****************************
 ****                    ****
 **     MAIN INTERFACE	   **
 ****                    ****
 ****************************/

/**
 * @brief Initialize Gesture Predictor
 * @param pProc   GestureWild processor pointer
 * @param pTrack  HandTracker processor pointer
 * @param pData   Sensor data pointer
 * @return 0 on success, -2 if pointer is NULL
 */
int GW_init(GW_proc_t* pProc, HT_proc_t* pTrack, SEN_data_t* pData);

/**
 * @brief Predict gesture from data ranging
 * @param pProc   GestureWild processor pointer
 * @param pTrack  HandTracker processor pointer
 * @param pData   Sensor data pointer
 * @return 0 on success, -2 if pointer is NULL
 */
int GW_run(GW_proc_t* pProc, HT_proc_t* pTrack, SEN_data_t* pData);


/****************************
 ****                    ****
 **         SETTERS 	   **
 ****                    ****
 ****************************/

/**
 * @brief Sets the values of the processor parameters, sets to default if invalid
 * @param pProc    GestureWild processor pointer
 * @param detectionLimitation	Gesture detection limitation
 * @param analysis_ms       Time window to compute speed
 * @param dead_ms           Duration after a gesture when we can't predict anything
 * @param screening_ms      Duration which we
 * @param min_speed_x_mm_s  Minimum speed on x axis for a gesture
 * @param min_speed_y_mm_s  Minimum speed on y axis for a gesture
 * @param min_speed_z_mm_s  Minimum speed on z axis for a gesture
 * @param max_speed_mm_s    Maximum speed for a gesture
 * @param max_distance_mm   Maximum distance for a gesture
 * @param min_vx_vy         Min VX/VY ratio to detect horizontal swipe
 * @param min_vx_vz         Min VX/VZ ratio to detect horizontal swipe
 * @param min_vy_vx         Min VY/VX ratio to detect vertical swipe
 * @param min_vy_vz         Min VY/VZ ratio to detect vertical swipe
 * @param min_vz_vx         Min VZ/VX ratio to detect depth movement
 * @param min_vz_vy         Min VZ/VY ratio to detect depth movement
 * @param precise_speed     Do we use timestamps for speed computation
 * @return 0 on success, -2 if pointer is NULL
 */
int GW_set_params(GW_proc_t* pProc, int detectionLimitation, int analysis_ms, int dead_ms, int screening_ms, int closer_mm,
									int min_speed_x_mm_s, int min_speed_y_mm_s, int min_speed_z_mm_s, int max_speed_mm_s,
									int max_distance_mm,
									float min_vx_vy, float min_vx_vz,
									float min_vy_vx, float min_vy_vz,
									float min_vz_vx, float min_vz_vy,
									int precise_speed,
									int min_user_filtering_mm,
									int max_user_filtering_mm,
									int filtering_area_mm,
									float lc_stable_threshold,
									long lc_stable_time_threshold,
									int lc_maxDistance_mm,
									int lc_minDistance_mm,
									long double_tap_ts_threshold);
/**
 * @brief Sets frequency and initializes buffer sizes
 * @param pProc    GestureWild processor pointer
 * @param pTrack   HandTracker processor pointer
 * @param pData    Sensor data pointer
 * @param freq     Frequency of ranging
 * @return 0 on success, -2 if pointer is NULL, -6 if frequency is invalid
 */
int GW_set_frequency(GW_proc_t* pProc, HT_proc_t* pTrack, SEN_data_t* pData, float freq);

/**
 * @brief Initializes buffers, converts times to iterations
 * @param pProc    GestureWild processor pointer
 * @param pInfo    Sensor info pointer
 * @return 0 on success, -2 if pointer is NULL, -8 if we can't write values to buffers
 */
int GW_set_buffer_length(GW_proc_t* pProc, SEN_info_t* pInfo);


/****************************
 ****                    ****
 **    HELPER FUNCTIONS    **
 ****                    ****
 ****************************/

/**
 * @brief Saves hand position to speed computation buffers
 * @param pProc    GestureWild processor pointer
 * @param pTrack   HandTracker processor pointer
 * @param pInfo    Sensor info pointer
 * @return 0 on success, -2 if pointer is NULL
 */
int GW_update_hand_buffers(GW_proc_t* pProc, HT_proc_t* pTrack, SEN_info_t* pInfo);

/**
 * @brief Computes speeds of hands in buffers
 * @param pProc    GestureWild processor pointer
 * @param pInfo    Sensor info pointer
 * @return 0 on success, -2 if pointer is NULL
 */
int GW_compute_speeds(GW_proc_t* pProc, SEN_info_t* pInfo);

/**
 * @brief Assigns a gesture label depending on the speeds provided
 * @param pProc       GestureWild processor pointer
 * @return 0 on success, -2 if pointer is NULL
 */
int GW_interpret_speeds(GW_proc_t* pProc);

/**
 * @brief Post-Process and filter of predictions
 * @param pProc       GestureWild processor pointer
 * @param pTrack   HandTracker processor pointer
 * @param timestamp_ms timestamp of prediction
 * @return 0 on success, -2 if pointer is NULL
 */
int GW_update_prediction(GW_proc_t* pProc, HT_proc_t* pTrack, long timestamp_ms);

/**
 * @brief Filter of predictions according user body
 * @param pProc     GestureWild processor pointer
 * @param pTrack   HandTracker processor pointer
 * @param pData    	Sensor data pointer
 * @return 0 on success, -2 if pointer is NULL
 */
int GW_filter_prediction(GW_proc_t* pProc, HT_proc_t* pTrack, SEN_data_t* pData);

/**
 * @brief Level Control predictions
 * @param pProc       GestureWild processor pointer
 * @param pTrack   HandTracker processor pointer
 * @param timestamp_ms timestamp of prediction
 * @return 0 on success, -2 if pointer is NULL
 */
int GW_level_control(GW_proc_t* pProc, HT_proc_t* pTrack, long timestamp_ms);

// LEVEL CONTROL //
void setLevelController(GW_proc_t* pProc, HT_proc_t* pTrack);
void setPolarLevelController(GW_proc_t* pProc, HT_proc_t* pTrack);

#ifdef __cplusplus
}					
#endif	
#endif
