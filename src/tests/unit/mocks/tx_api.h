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
// ... (outros códigos omitidos para brevidade - não usados nos testes)

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
