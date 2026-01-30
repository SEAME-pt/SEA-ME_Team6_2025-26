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


#ifndef RING_FBUFFER_VM_H_
#define RING_FBUFFER_VM_H_

#include "htgd_platform.h"

#ifdef __cplusplus
extern "C" {
#endif
    
/** @defgroup misc_ring_buffer Ring Float Buffer
 *  @brief    Simple ring buffer implementation (float)
    @par Description
    Ring buffer is implemented as a static array of integers of size #RFBVM_MAX_SIZE. The functional size of the ring buffer is
    programmable. When the ring_buffer is full, new elements added are replacing the older elements.
 *  @ingroup misc
 *  @{  
 */

/** Ring Buffer maximum size */
#define RFBVM_MAX_SIZE 128

/**
 * @struct ring_buffer
 * @brief Simple ring buffer of int with a programmable size (max size is #RFBVM_MAX_SIZE)
 */    
typedef struct
{
    float  buffer[RFBVM_MAX_SIZE];
    float* buffer_end;
    float* data_start;
    float* data_end;
    int count;
    int size;
} ring_fbuffer_vm;

/**
 * @brief Initialize Ring Buffer
 * @param rb  Ring Buffer pointer
 * @param size Number of int elements (max size is #RFBVM_MAX_SIZE, min size is 1)
 * @return 0 on success or -1 if size is greater than #RFBVM_MAX_SIZE or smaller than 1
 */
int RFBVM_init(ring_fbuffer_vm* rb, int size);

/**
 * @brief Push one element in Ring Buffer (after the last element)
 * @par Function Description
 * If ring buffer is full, added element is replacing the oldest element in the ring buffer
 * @param rb  Ring Buffer pointer
 * @param data Element to add
 * @return 0 on success
 */
int RFBVM_push(ring_fbuffer_vm* rb, float data);

/**
 * @brief Fills buffer with data
 * @par Function Description
 * Fills from data start to data end, only changes the values, doesn't add elements
 * @param rb  Ring Buffer pointer
 * @param data  Element to fill buffer with
 * @return 0 if successful
 */
int RFBVM_fill(ring_fbuffer_vm* rb, float data);

/**
 * @brief Check if ring buffer is full
 * @param rb  Ring Buffer pointer
 * @return true if full else false
 */
bool RFBVM_full(ring_fbuffer_vm* rb);

/**
 * @brief Return the sum of elements in the ring buffer
 * @param rb  Ring Buffer pointer
 * @return The sum
 */
float RFBVM_sum(ring_fbuffer_vm* rb);

/**
 * @brief Return the mean of all elements in the ring buffer
 * @param rb  Ring Buffer pointer
 * @return The mean (rounded to integer)
 */
float RFBVM_mean(ring_fbuffer_vm* rb);

/**
 * @brief Return the standard deviation of all elements in the ring buffer
 * @param rb  Ring Buffer pointer
 * @return The stdev (in float)
 */
float RFBVM_stdev(ring_fbuffer_vm* rb);

/**
 * @brief Return the value of the element at index. Index goes from 0 to rb->count - 1
 * @param rb  Ring Buffer pointer
 * @param index  Index of the element to be read in the ring buffer
 * @return The element value
 */
float RFBVM_get_element_value(ring_fbuffer_vm* rb, int index);

/**
 * @brief Sets the value of the element at index. Index goes from 0 to rb->count - 1
 * @param rb  Ring Buffer pointer
 * @param index  Index of the element to be changed in the ring buffer
 * @param value  Value to write
 * @return 0 if successful
 */
int RFBVM_set_element_value(ring_fbuffer_vm* rb, int index, float value);

/**
 * @brief Return the pointer on the element at index. Index goes from 0 to rb->count - 1
 * @param rb  Ring Buffer pointer
 * @param index  Index of the element to be read in the ring buffer
 * @return The element pointer
 */
float* RFBVM_get_element_pointer(ring_fbuffer_vm* rb, int index);

 /** @}  */

//For debug
//void RFBVM_trace(ring_fbuffer_vm* rb);

#ifdef __cplusplus
}					
#endif	
#endif /* RING_FBUFFER_VM_H_ */
