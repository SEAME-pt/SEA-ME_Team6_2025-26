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


#ifndef SEN_H_
#define SEN_H_

#include "htgd_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SEN_VERSION "1.0.2"

/** @brief Sensor description for VL53L5 or similar
 *  @par Description
    This is a placeholder for key sensor parameters and data along with helper functions
 *  @{
 */

#define M_PI	3.14159265358979323846

/* Default Sensor constants
 *
 * Field of view is in degrees
 * x and y axis are relative to the orientation 0 for res and fov
 *
 * Sensor orientation:
     0    1    2    3    4    5    6    7
          ||   __   ||              _   _
    [oo] [oo] [  ] [__] =[8] [8]= =[_] [_]=
     ||        ||
 */
#define SEN_FOV_X 			43  //L5 = 43 / L7 = 60
#define SEN_FOV_Y 			43  //L5 = 43 / L7 = 60
#define SEN_ORIENTATION		1
#define SEN_MAX_ZONES		64

/**
 * @struct Sensor Info
 * @brief Key parameters of the sensor
 */
typedef struct {
    
	int res_x;
	int res_y;
    int nb_zones;
    
    float freq;

    float fov_x;
    float fov_y;
    
    char orientation;
	
} SEN_info_t;

/**
 * @struct Sensor data
 * @brief Place holder for sensor data, contains the latest values and parameters
 */
typedef struct {

	SEN_info_t info;

	long timestamp_ms;

    float ranging[SEN_MAX_ZONES];
    float peak[SEN_MAX_ZONES];
    bool  valid[SEN_MAX_ZONES];

} SEN_data_t;

/**
 * @brief Initialize Sensor data with default values and unknowns at 0
 * @param pData  Sensor data pointer
 * @return 0 on success or -2 if pointer is NULL
 */
int SEN_init(SEN_data_t* pData);

/**
 * @brief Sets sensor orientation, choses default for invalid values
 * @param pData  Sensor data pointer
 * @param orientation Wanted orientation
 * @return 0 on success, -2 if pointer is NULL
 */
int SEN_set_orientation(SEN_data_t* pData, int orientation);

/**
 * @brief Sets sensor frequency, returns error for invalid values
 * @param pData  Sensor data pointer
 * @param freq Sensor freq
 * @return 0 on success, -2 if pointer is NULL or -6 if frequency is negative or zero
 */
int SEN_set_frequency(SEN_data_t* pData, float freq);

/**
 * @brief Set actual resolution of the sensor, allocates placeholders accordingly
 * @param pData  Sensor data pointer
 * @param res_x  Resolution along x axis (for orientation 0)
 * @param res_y  Resolution along y axis (for orientation 0)
 * @return 0 on success, -2 if pointer is NULL or -4 if resolution given is flawed
 */
int SEN_set_resolution(SEN_data_t* pData, int res_x, int res_y);

/**
 * @brief Overload of set_resolution for square sensors
 * @param pData  Sensor data pointer
 * @param nb_zones  Total number of zones
 * @return 0 on success, -2 if pointer is NULL or -4 if resolution given is flawed
 */
int SEN_set_square_resolution(SEN_data_t* pData, int nb_zones);

/**
 * @brief Stores the values in the buffers at given index
 * @param pData  Sensor data pointer
 * @param idx Index to store the values at
 * @param range Range value to store
 * @param peak Peak value to store
 * @param valid Valid value to store
 * @return 0 on success, -2 if pointer is NULL, -3 if index is invalid
 */
int SEN_set_data(SEN_data_t* pData, int idx, float range, float peak, bool valid);

/**
 * @brief Computes the correct coordinates from an index depending on the orientation
 * @param col_ret  Return pointer of column
 * @param row_ret  Return pointer of row
 * @param idx  Index of element
 * @param pInfo  Sensor information
 * @return 0 on success, -1 for unknown orientation, -2 if pointer is NULL, -3 if index is invalid or -4 if resolution given is invalid
 */
int SEN_idx_to_col_row(int* col_ret, int* row_ret, int idx, SEN_info_t* pInfo);

/**
 * @brief Computes the correct index from coordinates depending on the orientation
 * @param idx_ret  Return pointer of index
 * @param col  Column of the element
 * @param row  Row of the element
 * @param pInfo  Sensor information
 * @return 0 on success, -1 for unknown orientation, -2 if pointer is NULL, -4 if resolution given is invalid
 */
int SEN_col_row_to_idx(int* idx_ret, int col, int row, SEN_info_t* pInfo);

/**
 * @brief Computes the spatial coordinates from grid coordinates, center is in the middle of the grid
 * @param x_ret  Return pointer of the Abscissa
 * @param y_ret  Return pointer of the Ordinate
 * @param idx Index of the element
 * @param z  Distance of the element
 * @param pInfo  Sensor information
 * @return 0 on success, -1 if orientation is invalid, -2 if pointer is NULL, -3 if index is invalid,  -4 if resolution given is invalid
 */
int SEN_idx_to_mm(float* x_ret, float* y_ret, int idx, float z, SEN_info_t* pInfo);

/**
 * @brief Computes the spatial coordinates from grid coordinates, center is in the middle of the grid
 * @param x_ret  Return pointer of the Abscissa
 * @param y_ret  Return pointer of the Ordinate
 * @param col  Column of the element
 * @param row  Row of the element
 * @param z  Distance of the element
 * @param pInfo  Sensor information
 * @return 0 on success, -2 if pointer is NULL, -4 if resolution given is invalid
 */
int SEN_col_row_to_mm(float* x_ret, float* y_ret, int col, int row, float z, SEN_info_t* pInfo);

/**
 * @brief Computes the approximate grid coordinates from spatial coordinates
 * @param col_ret  Return pointer of column
 * @param row_ret  Return pointer of row
 * @param x  Abscissa of the element
 * @param y  Ordinate of the element
 * @param z  Distance of the element
 * @param pInfo  Sensor information
 * @return 0 on success, -2 if pointer is NULL, -3 if resulting coordinates are invalid, -4 if resolution given is invalid
 */
int SEN_mm_to_col_row(float* col_ret, float* row_ret, float x, float y, float z, SEN_info_t* pInfo);

/**
 * @brief Computes the index containing the spatial coordinates
 * @param idx_ret Index of element
 * @param x  Abscissa of the element
 * @param y  Ordinate of the element
 * @param z  Distance of the element
 * @param pInfo  Sensor information
 * @return 0 on success, -2 if pointer is NULL, -3 if resulting coordinates are invalid, -4 if resolution given is invalid
 */
int SEN_mm_to_idx(int* idx_ret, float x, float y, float z, SEN_info_t* pInfo);

/**
 * @brief Computes width and height of a zone at a distance
 * @param width_ret  Return pointer of width
 * @param height_ret  Return pointer of height
 * @param z  Distance of zone
 * @param pInfo  Sensor information
 * @return 0 on success, -2 if pointer is NULL or -4 if resolution given is invalid
 */
int SEN_zone_size(float* width_ret, float* height_ret, float z, SEN_info_t* pInfo);

/**
 * @brief Computes the actual resolution along x and y depending on orientation
 * @param res_x_ret  Return pointer of x axis resolution
 * @param res_y_ret  Return pointer of y axis resolution
 * @param pInfo  Sensor information
 * @return 0 on success or -2 if pointer is NULL
 */
int SEN_actual_res(int* res_x_ret, int* res_y_ret, SEN_info_t* pInfo);

/**
 * @brief Computes the actual resolution along x and y depending on orientation
 * @param radius_ret  Return pointer of radius
 * @param theta_ret  Return pointer of theta, angle in radian between y-z plane and point
 * @param phi_ret  Return pointer of phi, angle in radian between x-z plane and point
 * @param x  Abscissa of the element
 * @param y  Ordinate of the element
 * @param z  Distance of the element
 * @return 0 on success
 */
int SEN_cart_to_sphere(float* radius_ret, float* theta_ret, float* phi_ret, float x, float y, float z);

#ifdef __cplusplus
}					
#endif	
#endif
