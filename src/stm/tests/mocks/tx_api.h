/**
  * Mock do tx_api.h para testes
  * Simula o ThreadX RTOS
  */

#ifndef TX_API_H
#define TX_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// ThreadX wait options
#define TX_WAIT_FOREVER     0xFFFFFFFFUL
#define TX_NO_WAIT          0x00000000UL

// ThreadX return codes
#define TX_SUCCESS          0x00
#define TX_DELETED          0x01
#define TX_NO_MEMORY        0x10
#define TX_POOL_ERROR       0x02
#define TX_PTR_ERROR        0x03
#define TX_WAIT_ERROR       0x04
#define TX_SIZE_ERROR       0x05
#define TX_GROUP_ERROR      0x06
#define TX_NO_EVENTS        0x07
#define TX_OPTION_ERROR     0x08
#define TX_QUEUE_ERROR      0x09
#define TX_QUEUE_EMPTY      0x0A
#define TX_QUEUE_FULL       0x0B
#define TX_SEMAPHORE_ERROR  0x0C
#define TX_NO_INSTANCE      0x0D
#define TX_THREAD_ERROR     0x0E
#define TX_PRIORITY_ERROR   0x0F
#define TX_START_ERROR      0x10
#define TX_DELETE_ERROR     0x11
#define TX_RESUME_ERROR     0x12
#define TX_CALLER_ERROR     0x13
#define TX_SUSPEND_ERROR    0x14
#define TX_TIMER_ERROR      0x15
#define TX_TICK_ERROR       0x16
#define TX_ACTIVATE_ERROR   0x17
#define TX_THRESH_ERROR     0x18
#define TX_SUSPEND_LIFTED   0x19
#define TX_WAIT_ABORTED     0x1A
#define TX_WAIT_ABORT_ERROR 0x1B
#define TX_MUTEX_ERROR      0x1C
#define TX_NOT_AVAILABLE    0x1D
#define TX_NOT_OWNED        0x1E
#define TX_INHERIT_ERROR    0x1F
#define TX_NOT_DONE         0x20
#define TX_CEILING_EXCEEDED 0x21

// Mutex structure (mock)
typedef struct TX_MUTEX_STRUCT
{
    uint32_t tx_mutex_id;
    char *tx_mutex_name;
    uint32_t tx_mutex_ownership_count;
    void *tx_mutex_owner;
    uint32_t tx_mutex_inherit;
} TX_MUTEX;

// Mock functions
uint32_t tx_mutex_get(TX_MUTEX *mutex_ptr, uint32_t wait_option);
uint32_t tx_mutex_put(TX_MUTEX *mutex_ptr);

#ifdef __cplusplus
}
#endif

#endif /* TX_API_H */
