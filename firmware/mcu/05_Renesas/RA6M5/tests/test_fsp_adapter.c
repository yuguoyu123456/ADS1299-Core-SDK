/* Executed as ARM instructions in Unicorn with fake peripherals, not hardware. */
#include "ads1299_fsp_hal.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)

void *memcpy(void *dst, const void *src, size_t n)
{ unsigned char *d = dst; const unsigned char *s = src; while (n--) *d++ = *s++; return dst; }
void *memset(void *dst, int value, size_t n)
{ unsigned char *d = dst; while (n--) *d++ = (unsigned char)value; return dst; }

static ads1299_fsp_context_t ctx;
static ads1299_platform_hal_t hal;
static spi_cfg_t config;
static int ctrl, io_ctrl, extension;
static spi_cfg_t const *active_config;
static uint32_t tick, due, calls, closes, writes;
static int event_mode, pending, submit_fail, close_fail, gpio_fail, frozen_clock;
static uint8_t const *pending_tx;
static uint8_t *pending_rx;
static uint32_t pending_len;
static bsp_io_level_t levels[5];

static fsp_err_t fake_open(spi_ctrl_t *p, spi_cfg_t const *const cfg)
{ (void)p; active_config = cfg; return 0; }
static fsp_err_t fake_close(spi_ctrl_t *const p)
{ (void)p; ++closes; if (close_fail) return -1; pending = 0; return 0; }
static fsp_err_t fake_transfer(spi_ctrl_t *const p, void const *tx, void *rx,
                             uint32_t const n, spi_bit_width_t const width)
{
    (void)p; ++calls;
    if (submit_fail || width != SPI_BIT_WIDTH_8_BITS || tx != ctx.tx || rx != ctx.rx || n > 32)
        return -1;
    pending_tx = tx; pending_rx = rx; pending_len = n;
    due = tick + 3; pending = 1; return 0;
}
static uint32_t now(void *user)
{
    (void)user; if (!frozen_clock) ++tick;
    if (pending && event_mode && tick == due) {
        for (uint32_t i = 0; i < pending_len; ++i) pending_rx[i] = pending_tx[i] ^ 0xa5;
        spi_callback_args_t args = {0, (spi_event_t)event_mode, active_config->p_context};
        pending = 0; active_config->p_callback(&args);
    }
    return tick;
}
static void delay_us(void *user, uint32_t us) { (void)user; tick += us; }
static fsp_err_t write_pin(ioport_ctrl_t *const p, bsp_io_port_pin_t pin, bsp_io_level_t level)
{ (void)p; ++writes; if (gpio_fail || pin >= 5) return -1; levels[pin] = level; return 0; }
static fsp_err_t read_pin(ioport_ctrl_t *const p, bsp_io_port_pin_t pin, bsp_io_level_t *level)
{ (void)p; if (gpio_fail || pin >= 5) return -1; *level = levels[pin]; return 0; }
static const spi_api_t spi_api = {.open=fake_open, .close=fake_close, .writeRead=fake_transfer};
static const ioport_api_t io_api = {.pinWrite=write_pin, .pinRead=read_pin};
static const spi_instance_t spi = {&ctrl, &config, &spi_api};
static const ioport_instance_t io = {&io_ctrl, 0, &io_api};

static void prepare(void)
{
    memset(&ctx, 0, sizeof(ctx)); memset(&config, 0, sizeof(config));
    ctx.spi=&spi; ctx.io=&io; ctx.now_us=now; ctx.delay_us=delay_us; ctx.timeout_us=10;
    for (unsigned i=0; i<5; ++i) ctx.pins[i]=i;
    config.operating_mode=SPI_MODE_MASTER; config.clk_phase=SPI_CLK_PHASE_EDGE_EVEN;
    config.clk_polarity=SPI_CLK_POLARITY_LOW; config.bit_order=SPI_BIT_ORDER_MSB_FIRST;
    config.p_extend=&extension;
    event_mode=SPI_EVENT_TRANSFER_COMPLETE; pending=0; submit_fail=0; close_fail=0; gpio_fail=0;
    calls=closes=writes=tick=0; frozen_clock=0;
}

int test_main(void)
{
    uint8_t tx[65], rx[65];
    for (unsigned i=0;i<65;++i) tx[i]=(uint8_t)i;
    prepare(); CHECK(ads1299_fsp_open(&ctx,&hal)==0);
    CHECK(levels[0]==1 && levels[1]==1 && levels[2]==1 && levels[3]==0);
    CHECK(active_config==&ctx.config && config.p_callback==0);
    CHECK(ads1299_fsp_open(&ctx,&hal)!=0); /* prevent double open */
    CHECK(hal.spi_transfer(hal.user,tx,rx,65)==0 && calls==3 && writes==4);
    for (unsigned i=0;i<65;++i) CHECK(rx[i]==(uint8_t)(tx[i]^0xa5));
    CHECK(hal.spi_transfer(hal.user,0,rx,65)==0);
    for (unsigned i=0;i<65;++i) CHECK(rx[i]==0xa5);
    CHECK(hal.spi_transfer(hal.user,tx,0,65)==0);
    CHECK(hal.spi_transfer(hal.user,tx,rx,0)!=0);
    CHECK(hal.spi_transfer(hal.user,0,0,1)!=0);
    hal.pin_write(hal.user,ADS1299_PIN_DRDY,1); CHECK(writes==4);
    levels[4]=0; CHECK(hal.pin_read(hal.user,ADS1299_PIN_DRDY)==0);
    levels[4]=1; CHECK(hal.pin_read(hal.user,ADS1299_PIN_DRDY)==1);
    tick=UINT32_MAX-2; CHECK(hal.spi_transfer(hal.user,tx,rx,1)==0); /* clock wraps */
    event_mode=0; tick=UINT32_MAX-2;
    CHECK(hal.spi_transfer(hal.user,tx,rx,1)!=0 && closes==1 && !ctx.opened);
    CHECK(!pending && ctx.fault); CHECK(hal.spi_transfer(hal.user,tx,rx,1)!=0);
    hal.pin_write(hal.user,ADS1299_PIN_CS,1); CHECK(levels[0]==1);
    CHECK(ads1299_fsp_open(&ctx,&hal)==0); /* explicit reopen after quiesced close */
    for (int event=SPI_EVENT_TRANSFER_ABORTED;event<=SPI_EVENT_ERR_MODE_UNDERRUN;++event) {
        event_mode=event; CHECK(hal.spi_transfer(hal.user,tx,rx,1)!=0);
        CHECK(ctx.fault && !ctx.opened); CHECK(ads1299_fsp_open(&ctx,&hal)==0);
    }
    submit_fail=1; CHECK(hal.spi_transfer(hal.user,tx,rx,1)!=0 && !ctx.opened);
    prepare(); CHECK(ads1299_fsp_open(&ctx,&hal)==0); gpio_fail=1;
    CHECK(hal.pin_read(hal.user,ADS1299_PIN_DRDY)<0 && ctx.fault);
    CHECK(ads1299_fsp_close(&ctx)==0);
    prepare(); gpio_fail=1; CHECK(ads1299_fsp_open(&ctx,&hal)!=0 && !ctx.opened);
    prepare(); ctx.pins[4]=ctx.pins[0]; CHECK(ads1299_fsp_open(&ctx,&hal)!=0);
    prepare(); config.clk_phase=SPI_CLK_PHASE_EDGE_ODD; CHECK(ads1299_fsp_open(&ctx,&hal)!=0);
    prepare(); config.tei_irq=-1; CHECK(ads1299_fsp_open(&ctx,&hal)!=0);
    prepare(); config.p_transfer_rx=(void*)&ctrl; CHECK(ads1299_fsp_open(&ctx,&hal)!=0);
    prepare(); ctx.timeout_us=0; CHECK(ads1299_fsp_open(&ctx,&hal)!=0);
    prepare(); CHECK(ads1299_fsp_open(&ctx,&hal)==0); event_mode=0; close_fail=1;
    CHECK(hal.spi_transfer(hal.user,tx,rx,1)!=0 && ctx.opened && ctx.fault);
    CHECK(ads1299_fsp_open(&ctx,&hal)!=0); close_fail=0; CHECK(ads1299_fsp_close(&ctx)==0);
    spi_callback_args_t error={0,SPI_EVENT_ERR_OVERRUN,&ctx};
    spi_callback_args_t complete={0,SPI_EVENT_TRANSFER_COMPLETE,&ctx};
    ctx.event=0; ads1299_fsp_callback(&error); ads1299_fsp_callback(&complete);
    CHECK(ctx.event==SPI_EVENT_ERR_OVERRUN);
    prepare(); CHECK(ads1299_fsp_open(&ctx,&hal)==0); frozen_clock=1; event_mode=0;
    CHECK(hal.spi_transfer(hal.user,tx,rx,1)!=0 && !ctx.opened && ctx.fault);
    return 0;
}
