/* Original EK-RA6M5 reference binding, FSP 5.6.0. Cold-start only.
 * Uses externally installed TinyUSB clock/BSP configuration, NOT its USB code.
 * MCU pin signals follow the official Renesas example; connector numbers and
 * physical operation still require board-revision verification. */
#include "ads1299_fsp_hal.h"
#include "r_spi.h"
#include "r_ioport.h"

void spi_rxi_isr(void);
void spi_txi_isr(void);
void spi_tei_isr(void);
void spi_eri_isr(void);
BSP_DONT_REMOVE BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS)
const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_MAX_ENTRIES] = {
    [0]=spi_rxi_isr, [1]=spi_txi_isr, [2]=spi_tei_isr, [3]=spi_eri_isr
};
const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_MAX_ENTRIES] = {
    [0]=ELC_EVENT_SPI1_RXI, [1]=ELC_EVENT_SPI1_TXI,
    [2]=ELC_EVENT_SPI1_TEI, [3]=ELC_EVENT_SPI1_ERI
};
/* Clock initialization is performed by FSP SystemInit. I/O is opened below. */
void bsp_init(void *args) { (void)args; }

static const ioport_pin_cfg_t pins[] = {
    {IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_SPI, BSP_IO_PORT_04_PIN_10},
    {IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_SPI, BSP_IO_PORT_04_PIN_11},
    {IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_SPI, BSP_IO_PORT_04_PIN_12},
    {IOPORT_CFG_PORT_DIRECTION_OUTPUT | IOPORT_CFG_PORT_OUTPUT_HIGH, BSP_IO_PORT_04_PIN_08},
    {IOPORT_CFG_PORT_DIRECTION_INPUT, BSP_IO_PORT_04_PIN_00},
    {IOPORT_CFG_PORT_DIRECTION_OUTPUT | IOPORT_CFG_PORT_OUTPUT_HIGH, BSP_IO_PORT_04_PIN_04},
    {IOPORT_CFG_PORT_DIRECTION_OUTPUT | IOPORT_CFG_PORT_OUTPUT_HIGH, BSP_IO_PORT_07_PIN_08},
    {IOPORT_CFG_PORT_DIRECTION_OUTPUT | IOPORT_CFG_PORT_OUTPUT_LOW, BSP_IO_PORT_01_PIN_12}
};
static const ioport_cfg_t io_cfg = {sizeof(pins)/sizeof(pins[0]), pins, NULL};
static ioport_instance_ctrl_t io_ctrl;
static const ioport_instance_t io = {&io_ctrl, &io_cfg, &g_ioport_on_ioport};
static spi_instance_ctrl_t spi_ctrl;
static spi_extended_cfg_t extension = {
    .spi_clksyn=SPI_SSL_MODE_CLK_SYN, .spi_comm=SPI_COMMUNICATION_FULL_DUPLEX,
    .ssl_polarity=SPI_SSLP_LOW, .ssl_select=SPI_SSL_SELECT_SSL0,
    .parity=SPI_PARITY_MODE_DISABLE, .byte_swap=SPI_BYTE_SWAP_DISABLE,
    .spck_delay=SPI_DELAY_COUNT_1, .ssl_negation_delay=SPI_DELAY_COUNT_1,
    .next_access_delay=SPI_DELAY_COUNT_1
};
static const spi_cfg_t spi_cfg = {
    .channel=1, .rxi_irq=0, .txi_irq=1, .tei_irq=2, .eri_irq=3,
    .rxi_ipl=5, .txi_ipl=5, .tei_ipl=5, .eri_ipl=5,
    .operating_mode=SPI_MODE_MASTER, .clk_phase=SPI_CLK_PHASE_EDGE_EVEN,
    .clk_polarity=SPI_CLK_POLARITY_LOW, .mode_fault=SPI_MODE_FAULT_ERROR_DISABLE,
    .bit_order=SPI_BIT_ORDER_MSB_FIRST, .p_extend=&extension
};
static const spi_instance_t spi = {&spi_ctrl, &spi_cfg, &g_spi_on_spi};
static ads1299_fsp_context_t context;
static uint32_t last_cycles, micros, cycle_remainder, cycles_per_us;

/* Polled DWT extension. Must run at least once per CYCCNT wrap (~21 s at
 * 200 MHz). Acquisition polls continuously. No clock switching or sleep here.
 * Debug halt pauses this reference clock; not a wall-clock recorder. */
static uint32_t now_us(void *user)
{
    (void)user;
    uint32_t current=DWT->CYCCNT;
    uint64_t total=(uint64_t)(uint32_t)(current-last_cycles)+cycle_remainder;
    last_cycles=current;
    micros+=(uint32_t)(total/cycles_per_us);
    cycle_remainder=(uint32_t)(total%cycles_per_us);
    return micros;
}
static void delay_us(void *user, uint32_t us)
{
    while (us) {
        uint32_t n=us>100000 ? 100000 : us;
        R_BSP_SoftwareDelay(n, BSP_DELAY_UNITS_MICROSECONDS);
        (void)now_us(user); us-=n;
    }
}
int board_ads1299_hal(ads1299_platform_hal_t *hal)
{
    if (!hal || context.opened || !SystemCoreClock || SystemCoreClock%1000000U)
        return -1;
    cycles_per_us=SystemCoreClock/1000000U;
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    __DSB(); __ISB();
    last_cycles=DWT->CYCCNT;
    R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MICROSECONDS);
    if (DWT->CYCCNT==last_cycles) return -1; /* unavailable/locked debug counter */
    last_cycles=DWT->CYCCNT;
    if (R_IOPORT_Open(&io_ctrl, &io_cfg)!=FSP_SUCCESS) return -1;
    if (R_SPI_CalculateBitrate(1000000U, &extension.spck_div)!=FSP_SUCCESS) return -1;
    context.spi=&spi; context.io=&io;
    context.pins[ADS1299_PIN_CS]=BSP_IO_PORT_04_PIN_08;
    context.pins[ADS1299_PIN_RESET]=BSP_IO_PORT_04_PIN_04;
    context.pins[ADS1299_PIN_PWDN]=BSP_IO_PORT_07_PIN_08;
    context.pins[ADS1299_PIN_START]=BSP_IO_PORT_01_PIN_12;
    context.pins[ADS1299_PIN_DRDY]=BSP_IO_PORT_04_PIN_00;
    context.now_us=now_us; context.delay_us=delay_us; context.timeout_us=10000;
    __enable_irq();
    return ads1299_fsp_open(&context,hal);
}
