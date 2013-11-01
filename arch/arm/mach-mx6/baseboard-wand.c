
#include <asm/mach/arch.h>

#include <linux/clk.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/regulator/fixed.h>
#include <linux/regulator/machine.h>

#include <mach/common.h>
#include <mach/devices-common.h>

#include "edm.h"

/****************************************************************************
 *                                                                          
 * SGTL5000 Audio Codec
 *                                                                          
 ****************************************************************************/

static struct regulator_consumer_supply wandbase_sgtl5000_consumer_vdda = {
	.supply = "VDDA",
	.dev_name = "0-000a", /* Modified load time */
};

/* ------------------------------------------------------------------------ */

static struct regulator_consumer_supply wandbase_sgtl5000_consumer_vddio = {
	.supply = "VDDIO",
	.dev_name = "0-000a", /* Modified load time */
};

/* ------------------------------------------------------------------------ */

static struct regulator_init_data wandbase_sgtl5000_vdda_reg_initdata = {
	.num_consumer_supplies = 1,
	.consumer_supplies = &wandbase_sgtl5000_consumer_vdda,
};

/* ------------------------------------------------------------------------ */

static struct regulator_init_data wandbase_sgtl5000_vddio_reg_initdata = {
	.num_consumer_supplies = 1,
	.consumer_supplies = &wandbase_sgtl5000_consumer_vddio,
};

/* ------------------------------------------------------------------------ */

static struct fixed_voltage_config wandbase_sgtl5000_vdda_reg_config = {
	.supply_name		= "VDDA",
	.microvolts		= 2500000,
	.gpio			= -1,
	.init_data		= &wandbase_sgtl5000_vdda_reg_initdata,
};

/* ------------------------------------------------------------------------ */

static struct fixed_voltage_config wandbase_sgtl5000_vddio_reg_config = {
	.supply_name		= "VDDIO",
	.microvolts		= 3300000,
	.gpio			= -1,
	.init_data		= &wandbase_sgtl5000_vddio_reg_initdata,
};

/* ------------------------------------------------------------------------ */

static struct platform_device wandbase_sgtl5000_vdda_reg_devices = {
	.name	= "reg-fixed-voltage",
	.id	= 0,
	.dev	= {
		.platform_data = &wandbase_sgtl5000_vdda_reg_config,
	},
};

/* ------------------------------------------------------------------------ */

static struct platform_device wandbase_sgtl5000_vddio_reg_devices = {
	.name	= "reg-fixed-voltage",
	.id	= 1,
	.dev	= {
		.platform_data = &wandbase_sgtl5000_vddio_reg_config,
	},
};

/* ------------------------------------------------------------------------ */

static struct platform_device wandbase_audio_device = {
	.name = "imx-sgtl5000",
};

/* ------------------------------------------------------------------------ */

static const struct i2c_board_info wandbase_sgtl5000_i2c_data __initdata = {
        I2C_BOARD_INFO("sgtl5000", 0x0a)
};

/* ------------------------------------------------------------------------ */

static char wandbase_sgtl5000_dev_name[8] = "0-000a";

static __init int wandbase_init_sgtl5000(void) {

	wandbase_sgtl5000_dev_name[0] = '0' + edm_i2c[1];
	wandbase_sgtl5000_consumer_vdda.dev_name = wandbase_sgtl5000_dev_name;
	wandbase_sgtl5000_consumer_vddio.dev_name = wandbase_sgtl5000_dev_name;
        
	wandbase_audio_device.dev.platform_data = (struct mxc_audio_platform_data *)edm_analog_audio_platform_data;
	platform_device_register(&wandbase_audio_device);
        
	i2c_register_board_info(edm_i2c[1], &wandbase_sgtl5000_i2c_data, 1);
	platform_device_register(&wandbase_sgtl5000_vdda_reg_devices);
	platform_device_register(&wandbase_sgtl5000_vddio_reg_devices);
        return 0;
}

/****************************************************************************
 *                                                                          
 * PRISM Touch
 *                                                                          
 ****************************************************************************/

#include <mach/gpio.h>
#include <linux/delay.h>
static struct i2c_board_info wandbase_prism_i2c_data[] = {
        {
		I2C_BOARD_INFO("prism", 0x10),
		.irq	= -EINVAL,
		.flags = I2C_CLIENT_WAKE,
        },
};

static __init int wandbase_init_prism(void) {
	unsigned prism_reset;
	unsigned prism_irq;

	prism_reset = edm_external_gpio[9];
	prism_irq = edm_external_gpio[8];

	gpio_direction_output(prism_reset, 0);
	gpio_set_value(prism_reset, 0);
	mdelay(50);
	gpio_set_value(prism_reset, 1);
	wandbase_prism_i2c_data[0].irq = gpio_to_irq(prism_irq);
	gpio_direction_input(prism_irq);
	i2c_register_board_info(edm_i2c[1], &wandbase_prism_i2c_data[0], 1);
	return 0;
}

/****************************************************************************
 *
 * GPIO_BUTTON
 *
 ****************************************************************************/
#if defined(CONFIG_KEYBOARD_GPIO) || defined(CONFIG_KEYBOARD_GPIO_MODULE)
static struct gpio_keys_button wandbase_gpio_buttons[] = {
        {
                .code                   = KEY_POWER,
                .desc                   = "btn power-key",
                .wakeup                 = 1,
                .active_low             = 0,
        }, {
                .code                   = KEY_F8,
                .desc                   = "btn home-key",
                .wakeup                 = 0,
                .active_low             = 0,
        }, {
                .code                   = KEY_ESC,
                .desc                   = "btn back-key",
                .wakeup                 = 0,
                .active_low             = 0,
        }, {
                .code                   = KEY_F1,
                .desc                   = "btn menu-key",
                .wakeup                 = 0,
                .active_low             = 0,
        },
};

static struct gpio_keys_platform_data wandbase_gpio_key_info = {
        .buttons        = wandbase_gpio_buttons,
        .nbuttons       = ARRAY_SIZE(wandbase_gpio_buttons),
};

static struct platform_device wandbase_keys_gpio = {
        .name   = "gpio-keys",
        .id     = -1,
        .dev    = {
                .platform_data  = &wandbase_gpio_key_info,
        },
};

static int __init wandbase_gpio_keys_init(void)
{
	gpio_free(edm_external_gpio[3]);
	gpio_free(edm_external_gpio[4]);
	gpio_free(edm_external_gpio[5]);
	gpio_free(edm_external_gpio[7]);
	wandbase_gpio_buttons[0].gpio = edm_external_gpio[3];
	wandbase_gpio_buttons[1].gpio = edm_external_gpio[4];
	wandbase_gpio_buttons[2].gpio = edm_external_gpio[5];
	wandbase_gpio_buttons[3].gpio = edm_external_gpio[7];
	platform_device_register(&wandbase_keys_gpio);
	return 0;
}
#else
static inline int __init wandbase_gpio_keys_init(void) { return 0; }
#endif
/****************************************************************************
 *                                                                          
 * main-function for wand baseboard
 *                                                                          
 ****************************************************************************/

static __init int wandbase_init(void) {
	int ret = 0;
	ret += wandbase_init_sgtl5000();
	ret += wandbase_init_prism();
	ret += wandbase_gpio_keys_init();
	return ret;
}
subsys_initcall(wandbase_init);

static __exit void wandbase_exit(void) {
	/* Actually, this cannot be unloaded. Or loaded as a module..? */
} 
module_exit(wandbase_exit);

MODULE_DESCRIPTION("Wand baseboard driver");
MODULE_AUTHOR("Tapani <tapani@vmail.me>");
MODULE_LICENSE("GPL");
