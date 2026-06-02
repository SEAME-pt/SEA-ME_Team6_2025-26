#include "system_ctx.h"
#include "can_id.h"

static SystemCtx g_ctx; // Global system context instance. Serve como o "coração" do sistema, armazenando o estado global, mutexes e outras informações compartilhadas entre as threads. E como faz isso? Criando uma instância global de SystemCtx chamada g_ctx, que é acessível por todas as threads. As threads podem acessar e modificar o estado do sistema através dessa estrutura, usando os mutexes para garantir a sincronização adequada. Esta instancia está definida em system_ctx.c, e é composta por variáveis como system_state, drive_mode, error_flags, e mutexes para proteger o acesso a recursos compartilhados como printf, SPI, I2C, etc. As threads podem acessar essa estrutura global usando a função system_ctx(), que retorna um ponteiro para g_ctx.

void system_ctx_init(void)
{
    tx_mutex_create(&g_ctx.printf_mutex, "printf", TX_INHERIT);
    tx_mutex_create(&g_ctx.spi1_mutex,   "spi1",   TX_INHERIT);
    tx_mutex_create(&g_ctx.state_mutex,  "state",  TX_INHERIT);
    tx_mutex_create(&g_ctx.sys_mutex,    "sys",    TX_INHERIT);
    tx_mutex_create(&g_ctx.i2c1_mutex,   "i2c1",   TX_INHERIT);
    tx_mutex_create(&g_ctx.i2c2_mutex,   "i2c2",   TX_INHERIT);


    g_ctx.system_state = SYSTEM_STATE_INIT;
    g_ctx.drive_mode   = DRIVE_MODE_IDLE;
    g_ctx.error_flags  = 0;
}
