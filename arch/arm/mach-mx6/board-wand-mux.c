
#include "board-wand.h"

/****************************************************************************
 *                                                                          
 * SD init
 *
 * SD1 is routed to EDM connector (external SD on wand baseboard)
 * SD2 is WiFi
 * SD3 is boot SD on the module
 *                                                                          
 ****************************************************************************/
void wand_mux_pads_init_sdmmc(unsigned int port, int speed)
{
	unsigned int index;

	if(speed==200)
		index = 2;
	else if(speed==100)
		index = 1;
	else
		index = 0;

	switch(port)
	{
		case 0:
			EDM_SET_PAD(PAD_SD1_CLK__USDHC1_CLK_50MHZ_40OHM);
			EDM_SET_PAD(PAD_SD1_CMD__USDHC1_CMD_50MHZ_40OHM);
			EDM_SET_PAD(PAD_SD1_DAT0__USDHC1_DAT0_50MHZ_40OHM);
			EDM_SET_PAD(PAD_SD1_DAT1__USDHC1_DAT1_50MHZ_40OHM);
			EDM_SET_PAD(PAD_SD1_DAT2__USDHC1_DAT2_50MHZ_40OHM);
			EDM_SET_PAD(PAD_SD1_DAT3__USDHC1_DAT3_50MHZ_40OHM);
			break;
		case 1:
			EDM_SET_PAD(PAD_SD2_CLK__USDHC2_CLK);
			EDM_SET_PAD(PAD_SD2_CMD__USDHC2_CMD);
			EDM_SET_PAD(PAD_SD2_DAT0__USDHC2_DAT0);
			EDM_SET_PAD(PAD_SD2_DAT1__USDHC2_DAT1);
			EDM_SET_PAD(PAD_SD2_DAT2__USDHC2_DAT2);
			EDM_SET_PAD(PAD_SD2_DAT3__USDHC2_DAT3);
			break;
		case 2:
			EDM_SET_PAD(PAD_SD3_CLK__USDHC3_CLK_50MHZ);
			EDM_SET_PAD(PAD_SD3_CMD__USDHC3_CMD_50MHZ);
			EDM_SET_PAD(PAD_SD3_DAT0__USDHC3_DAT0_50MHZ);
			EDM_SET_PAD(PAD_SD3_DAT1__USDHC3_DAT1_50MHZ);
			EDM_SET_PAD(PAD_SD3_DAT2__USDHC3_DAT2_50MHZ);
			EDM_SET_PAD(PAD_SD3_DAT3__USDHC3_DAT3_50MHZ);
			break;
		default:
			break;
	}
}

/****************************************************************************
 *                                                                          
 * I2C
 *                                                                          
 ****************************************************************************/

void wand_mux_pads_init_i2c(int port)
{
	switch(port)
	{
		case 0:
			EDM_SET_PAD(PAD_EIM_D21__I2C1_SCL);
			EDM_SET_PAD(PAD_EIM_D28__I2C1_SDA);
			break;
		case 1:
			EDM_SET_PAD(PAD_KEY_COL3__I2C2_SCL);
			EDM_SET_PAD(PAD_KEY_ROW3__I2C2_SDA);
			break;
		case 2:
			EDM_SET_PAD(PAD_GPIO_5__I2C3_SCL);
			EDM_SET_PAD(PAD_GPIO_16__I2C3_SDA);
			break;
		default:
			printk("Invalid Port Selection : %d\n",port);
	}
}
		

/****************************************************************************
 *                                                                          
 * Initialize debug console (UART1)
 *                                                                          
 ****************************************************************************/

void wand_mux_pads_init_uart(void)
{
	/* UART1 (debug console) */
        EDM_SET_PAD(PAD_CSI0_DAT10__UART1_TXD);
        EDM_SET_PAD(PAD_CSI0_DAT11__UART1_RXD);
        EDM_SET_PAD(PAD_EIM_D19__UART1_CTS);
        EDM_SET_PAD(PAD_EIM_D20__UART1_RTS);
}
	

/****************************************************************************
 *                                                                          
 * Initialize sound (SSI, ASRC, AUD3 channel and S/PDIF)
 *                                                                          
 ****************************************************************************/
void wand_mux_pads_init_audio(void)
{
        EDM_SET_PAD(PAD_CSI0_DAT4__AUDMUX_AUD3_TXC);
        EDM_SET_PAD(PAD_CSI0_DAT5__AUDMUX_AUD3_TXD);
        EDM_SET_PAD(PAD_CSI0_DAT6__AUDMUX_AUD3_TXFS);
        EDM_SET_PAD(PAD_CSI0_DAT7__AUDMUX_AUD3_RXD);
        EDM_SET_PAD(PAD_GPIO_0__CCM_CLKO);
}

/*****************************************************************************
 *                                                                           
 * Init FEC and AR8031 PHY
 *                                                                            
 *****************************************************************************/
void wand_mux_pads_init_ethernet(void)
{
        EDM_SET_PAD(PAD_ENET_MDIO__ENET_MDIO);
        EDM_SET_PAD(PAD_ENET_MDC__ENET_MDC);
        
        EDM_SET_PAD(PAD_ENET_REF_CLK__ENET_TX_CLK);
        
	EDM_SET_PAD(PAD_RGMII_TXC__ENET_RGMII_TXC);
	EDM_SET_PAD(PAD_RGMII_TD0__ENET_RGMII_TD0);
	EDM_SET_PAD(PAD_RGMII_TD1__ENET_RGMII_TD1);
	EDM_SET_PAD(PAD_RGMII_TD2__ENET_RGMII_TD2);
	EDM_SET_PAD(PAD_RGMII_TD3__ENET_RGMII_TD3);
	EDM_SET_PAD(PAD_RGMII_TX_CTL__ENET_RGMII_TX_CTL);
	EDM_SET_PAD(PAD_RGMII_RXC__ENET_RGMII_RXC);
	EDM_SET_PAD(PAD_RGMII_RD0__ENET_RGMII_RD0);
	EDM_SET_PAD(PAD_RGMII_RD1__ENET_RGMII_RD1);
	EDM_SET_PAD(PAD_RGMII_RD2__ENET_RGMII_RD2);
	EDM_SET_PAD(PAD_RGMII_RD3__ENET_RGMII_RD3);
	EDM_SET_PAD(PAD_RGMII_RX_CTL__ENET_RGMII_RX_CTL);
                
        EDM_SET_PAD(PAD_ENET_TX_EN__GPIO_1_28);
        EDM_SET_PAD(PAD_EIM_D29__GPIO_3_29);
}

/****************************************************************************
 *                                                                          
 * USB
 *                                                                          
 ****************************************************************************/
void wand_mux_pads_init_usb(void)
{
        EDM_SET_PAD(PAD_GPIO_9__GPIO_1_9);
        EDM_SET_PAD(PAD_GPIO_1__USBOTG_ID);
        EDM_SET_PAD(PAD_EIM_D22__GPIO_3_22);
        EDM_SET_PAD(PAD_EIM_D30__GPIO_3_30);
}       

/****************************************************************************
 *                                                                          
 * WiFi
 *                                                                          
 ****************************************************************************/
void wand_mux_pads_init_wifi(void)
{
        /* ref_on, enable 32k clock */
        EDM_SET_PAD(PAD_EIM_EB1__GPIO_2_29);
        /* Wifi reset (resets when low!) */
        EDM_SET_PAD(PAD_EIM_A25__GPIO_5_2);
        /* reg on, signal to FDC6331L */
        EDM_SET_PAD(PAD_ENET_RXD1__GPIO_1_26);
        /* host wake */
        EDM_SET_PAD(PAD_ENET_TXD1__GPIO_1_29);
        /* wl wake - nc */
        EDM_SET_PAD(PAD_ENET_TXD0__GPIO_1_30);
}
	

/****************************************************************************
 *                                                                          
 * Bluetooth
 *                                                                          
 ****************************************************************************/
void wand_mux_pads_init_bluetooth(void)
{
        /* BT_ON, BT_WAKE and BT_HOST_WAKE */
        EDM_SET_PAD(PAD_EIM_DA13__GPIO_3_13);
        EDM_SET_PAD(PAD_EIM_DA14__GPIO_3_14);
        EDM_SET_PAD(PAD_EIM_DA15__GPIO_3_15);

        /* AUD5 channel goes to BT */
        EDM_SET_PAD(PAD_KEY_COL0__AUDMUX_AUD5_TXC);
        EDM_SET_PAD(PAD_KEY_ROW0__AUDMUX_AUD5_TXD);
        EDM_SET_PAD(PAD_KEY_COL1__AUDMUX_AUD5_TXFS);
        EDM_SET_PAD(PAD_KEY_ROW1__AUDMUX_AUD5_RXD);        
        
        /* Bluetooth is on UART3*/
        EDM_SET_PAD(PAD_EIM_D23__UART3_CTS);
        EDM_SET_PAD(PAD_EIM_D24__UART3_TXD);
        EDM_SET_PAD(PAD_EIM_D25__UART3_RXD);
        EDM_SET_PAD(PAD_EIM_EB3__UART3_RTS);
}

/****************************************************************************
 *                                                                          
 * Expansion pin header GPIOs
 *                                                                          
 ****************************************************************************/
void wand_mux_pads_init_external_gpios(void)
{
	EDM_SET_PAD(PAD_EIM_DA11__GPIO_3_11);
	EDM_SET_PAD(PAD_EIM_D27__GPIO_3_27);
	EDM_SET_PAD(PAD_EIM_BCLK__GPIO_6_31);
	EDM_SET_PAD(PAD_ENET_RX_ER__GPIO_1_24);
	EDM_SET_PAD(PAD_SD3_RST__GPIO_7_8);
	EDM_SET_PAD(PAD_EIM_D26__GPIO_3_26);
	EDM_SET_PAD(PAD_EIM_DA8__GPIO_3_8);
	EDM_SET_PAD(PAD_GPIO_19__GPIO_4_5);
}


/****************************************************************************
 *                                                                          
 * SPI - while not used on the Wandboard, the pins are routed out
 *                                                                          
 ****************************************************************************/
void wand_mux_pads_init_spi(void)
{
	EDM_SET_PAD(PAD_EIM_D16__ECSPI1_SCLK);
	EDM_SET_PAD(PAD_EIM_D17__ECSPI1_MISO);
	EDM_SET_PAD(PAD_EIM_D18__ECSPI1_MOSI);
	EDM_SET_PAD(PAD_EIM_EB2__GPIO_2_30);

	EDM_SET_PAD(PAD_EIM_CS0__ECSPI2_SCLK);
	EDM_SET_PAD(PAD_EIM_CS1__ECSPI2_MOSI);
	EDM_SET_PAD(PAD_EIM_OE__ECSPI2_MISO);
	EDM_SET_PAD(PAD_EIM_RW__GPIO_2_26);
	EDM_SET_PAD(PAD_EIM_LBA__GPIO_2_27);
}


