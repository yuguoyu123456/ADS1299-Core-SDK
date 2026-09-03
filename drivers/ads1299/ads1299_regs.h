#ifndef ADS1299_REGS_H
#define ADS1299_REGS_H

#include <stdint.h>

/* Commands */
#define ADS1299_CMD_WAKEUP   0x02u
#define ADS1299_CMD_STANDBY  0x04u
#define ADS1299_CMD_RESET    0x06u
#define ADS1299_CMD_START    0x08u
#define ADS1299_CMD_STOP     0x0Au
#define ADS1299_CMD_RDATAC   0x10u
#define ADS1299_CMD_SDATAC   0x11u
#define ADS1299_CMD_RDATA    0x12u
#define ADS1299_CMD_RREG     0x20u
#define ADS1299_CMD_WREG     0x40u

/* Register map */
#define ADS1299_REG_ID           0x00u
#define ADS1299_REG_CONFIG1      0x01u
#define ADS1299_REG_CONFIG2      0x02u
#define ADS1299_REG_CONFIG3      0x03u
#define ADS1299_REG_LOFF         0x04u
#define ADS1299_REG_CH1SET       0x05u
#define ADS1299_REG_CH2SET       0x06u
#define ADS1299_REG_CH3SET       0x07u
#define ADS1299_REG_CH4SET       0x08u
#define ADS1299_REG_CH5SET       0x09u
#define ADS1299_REG_CH6SET       0x0Au
#define ADS1299_REG_CH7SET       0x0Bu
#define ADS1299_REG_CH8SET       0x0Cu
#define ADS1299_REG_BIAS_SENSP   0x0Du
#define ADS1299_REG_BIAS_SENSN   0x0Eu
#define ADS1299_REG_LOFF_SENSP   0x0Fu
#define ADS1299_REG_LOFF_SENSN   0x10u
#define ADS1299_REG_LOFF_FLIP    0x11u
#define ADS1299_REG_LOFF_STATP   0x12u
#define ADS1299_REG_LOFF_STATN   0x13u
#define ADS1299_REG_GPIO         0x14u
#define ADS1299_REG_MISC1        0x15u
#define ADS1299_REG_MISC2        0x16u
#define ADS1299_REG_CONFIG4      0x17u

#define ADS1299_CHANNEL_COUNT      8u
#define ADS1299_STATUS_BYTES       3u
#define ADS1299_BYTES_PER_CHANNEL  3u
#define ADS1299_FRAME_BYTES        27u

/* CONFIG1: reserved bits must retain datasheet-defined values. */
#define ADS1299_CONFIG1_RESERVED_BASE 0x90u
#define ADS1299_CONFIG1_DAISY_EN      0x40u /* 0=daisy-chain, 1=multiple readback */
#define ADS1299_CONFIG1_CLK_EN        0x20u
#define ADS1299_CONFIG1_DR_MASK       0x07u

/* CONFIG1 DR[2:0] values */
#define ADS1299_DR_16KSPS 0x00u
#define ADS1299_DR_8KSPS  0x01u
#define ADS1299_DR_4KSPS  0x02u
#define ADS1299_DR_2KSPS  0x03u
#define ADS1299_DR_1KSPS  0x04u
#define ADS1299_DR_500SPS 0x05u
#define ADS1299_DR_250SPS 0x06u

/* CONFIG2: test/calibration signal generation. */
#define ADS1299_CONFIG2_RESERVED_BASE 0xC0u
#define ADS1299_CONFIG2_INT_CAL       0x10u
#define ADS1299_CONFIG2_CAL_AMP       0x04u
#define ADS1299_CONFIG2_CAL_FREQ_MASK 0x03u
#define ADS1299_TEST_FREQ_FCLK_2_21   0x00u
#define ADS1299_TEST_FREQ_FCLK_2_20   0x01u
#define ADS1299_TEST_FREQ_RESERVED    0x02u
#define ADS1299_TEST_FREQ_DC          0x03u

/* Readable aliases used by application code. */
#define ADS1299_TEST_FREQ_FCLK_DIV_2_21 ADS1299_TEST_FREQ_FCLK_2_21
#define ADS1299_TEST_FREQ_FCLK_DIV_2_20 ADS1299_TEST_FREQ_FCLK_2_20

/* CHnSET */
#define ADS1299_CH_POWER_DOWN   0x80u
#define ADS1299_CH_GAIN_MASK    0x70u
#define ADS1299_CH_SRB2         0x08u
#define ADS1299_CH_MUX_MASK     0x07u

#define ADS1299_GAIN_1   0x00u
#define ADS1299_GAIN_2   0x10u
#define ADS1299_GAIN_4   0x20u
#define ADS1299_GAIN_6   0x30u
#define ADS1299_GAIN_8   0x40u
#define ADS1299_GAIN_12  0x50u
#define ADS1299_GAIN_24  0x60u

#define ADS1299_MUX_NORMAL      0x00u
#define ADS1299_MUX_SHORTED     0x01u
#define ADS1299_MUX_BIAS_MEAS   0x02u
#define ADS1299_MUX_MVDD        0x03u
#define ADS1299_MUX_TEMP        0x04u
#define ADS1299_MUX_TEST        0x05u
#define ADS1299_MUX_BIAS_DRP    0x06u
#define ADS1299_MUX_BIAS_DRN    0x07u

#endif
