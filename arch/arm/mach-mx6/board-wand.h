
#ifndef _BOARD_WAND_H
#define _BOARD_WAND_H

#include <mach/gpio.h>
#include <mach/iomux-mx6dl.h>
#include <mach/iomux-mx6q.h>

/* Syntactic sugar for pad configuration */
#define EDM_SET_PAD(p) \
        if (cpu_is_mx6q()) \
                mxc_iomux_v3_setup_pad(MX6Q_##p);\
        else \
                mxc_iomux_v3_setup_pad(MX6DL_##p)

extern void wand_mux_pads_init_spi(void);
extern void wand_mux_pads_init_external_gpios(void);
extern void wand_mux_pads_init_bluetooth(void);
extern void wand_mux_pads_init_wifi(void);
extern void wand_mux_pads_init_usb(void);
extern void wand_mux_pads_init_ethernet(void);
extern void wand_mux_pads_init_audio(void);
extern void wand_mux_pads_init_uart(void);
extern void wand_mux_pads_init_i2c(int port);
extern void wand_mux_pads_init_sdmmc(unsigned int port, int speed);
extern void wand_mux_pads_init_lcdif(void);
extern void wand_mux_pads_init_lvds(void);

#endif
