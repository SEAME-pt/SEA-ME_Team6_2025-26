#include "system_ctx.h"

static SystemCtx g_ctx;
SystemCtx* system_ctx(void) { return &g_ctx; }
