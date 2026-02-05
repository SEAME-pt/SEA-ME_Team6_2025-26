#include "system_ctx.h"
#include "can_id.h"

static SystemCtx g_ctx;
SystemCtx* system_ctx(void) { return &g_ctx; }

void system_ctx_init(void)
{
    tx_mutex_create(&g_ctx.printf_mutex, "printf", TX_INHERIT);
    tx_mutex_create(&g_ctx.state_mutex,  "state",  TX_INHERIT);
    tx_mutex_create(&g_ctx.sys_mutex,    "sys",    TX_INHERIT);

    g_ctx.system_state = SYSTEM_STATE_INIT;
    g_ctx.drive_mode   = DRIVE_MODE_IDLE;
    g_ctx.error_flags  = 0;
}
