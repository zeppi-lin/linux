/*
    EDM DISPLAY CONTROL FRAMEWORK.
    Copyright (C) 2012,2013,2014 TechNexion Ltd.
    Edward Lin <edward.lin@technexion.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/edm.h>
#include <linux/edm_display.h>


/****************************************************************************
 *
 * Display Control
 *
 ****************************************************************************/

#define memalloc(a) kmalloc(a, GFP_KERNEL)
#define dbg_print	printk

#define EDM_DISPLAY_DEBUG	0

enum disp_control_types {
	ENUM_DEV = 0,
	ENUM_MODE,
	ENUM_TIMING,
	ENUM_IF,
	ENUM_BPP,
	ENUM_CHAR,
	ENUM_SPACE,
	ENUM_DISPLAY0,
	ENUM_DISPLAY1,
	ENUM_DISPLAY2,
	ENUM_DISPLAY3,
	ENUM_DISPLAY4,
	ENUM_DISPLAY5,
	ENUM_DISPLAY6,
	ENUM_DISPLAY7,
	ENUM_INVALID /*also number of display item types*/
};

struct edm_disp_ctrl_items {
	char *str;
	enum disp_control_types type;
};

static struct edm_disp_ctrl_items disp_ctrls[ENUM_INVALID] = {
	{
		.str = "dev=",
		.type = ENUM_DEV,
	}, {
		.str = "mode=",
		.type = ENUM_MODE,
	}, {
		.str = "timing=",
		.type = ENUM_TIMING,
	}, {
		.str = "if=",
		.type = ENUM_IF,
	}, {
		.str = "bpp=",
		.type = ENUM_BPP,
	}, {
		.str = "signalchar=",
		.type = ENUM_CHAR,
	}, {
		.str = " ",
		.type = ENUM_SPACE,
	}, {
		.str = "display0=",
		.type = ENUM_DISPLAY0,
	}, {
		.str = "display1=",
		.type = ENUM_DISPLAY1,
	}, {
		.str = "display2=",
		.type = ENUM_DISPLAY2,
	}, {
		.str = "display3=",
		.type = ENUM_DISPLAY3,
	}, {
		.str = "display4=",
		.type = ENUM_DISPLAY4,
	}, {
		.str = "display5=",
		.type = ENUM_DISPLAY5,
	}, {
		.str = "display6=",
		.type = ENUM_DISPLAY6,
	}, {
		.str = "display7=",
		.type = ENUM_DISPLAY7,
	}
};

void edm_disp_str_to_timing(struct edm_video_timing *timing, char * timing_str)
{
	char *ep;

	if ((timing == NULL) ||
		(timing_str == NULL)) {
		dbg_print("%s : NULL inputs!!\n", __FUNCTION__);
		return;
	}

	timing->pixclock = simple_strtoul(timing_str, &ep, 10);
	if (*ep == ',')
		timing->hres = simple_strtoul(++ep, &ep, 10);
	else
		goto err_out;

	if (*ep == ',')
		timing->hfp = simple_strtoul(++ep, &ep, 10);
	else
		goto err_out;

	if (*ep == ',')
		timing->hbp = simple_strtoul(++ep, &ep, 10);
	else
		goto err_out;

	if (*ep == ',')
		timing->hsw = simple_strtoul(++ep, &ep, 10);
	else
		goto err_out;

	if (*ep == ',')
		timing->vres = simple_strtoul(++ep, &ep, 10);
	else
		goto err_out;

	if (*ep == ',')
		timing->vfp = simple_strtoul(++ep, &ep, 10);
	else
		goto err_out;

	if (*ep == ',')
		timing->vbp = simple_strtoul(++ep, &ep, 10);
	else
		goto err_out;

	if (*ep == ',')
		timing->vsw = simple_strtoul(++ep, &ep, 10);
	else
		goto err_out;

#if EDM_DISPLAY_DEBUG
	dbg_print("Timing info :\n");
	dbg_print("PCLK %d, XRES %d, YRES, %d\n",
			timing->pixclock, timing->hres, timing->vres);
	dbg_print("Horizontal : HFP %d, HBP %d, HSW %d\n",
			timing->hfp, timing->hbp, timing->hsw);
	dbg_print("Vertical : VFP %d, VBP %d, VSW %d\n\n",
			timing->vfp, timing->vbp, timing->vsw);
#endif
	return;

err_out:
	dbg_print("%s : Incorrect input timing_str, zero all outputs!!\n",
								__FUNCTION__);
	timing->pixclock	= 0;
	timing->hres		= 0;
	timing->hfp		= 0;
	timing->hbp		= 0;
	timing->hsw		= 0;
	timing->vres		= 0;
	timing->vfp		= 0;
	timing->vbp		= 0;
	timing->vsw		= 0;

	return;
}
EXPORT_SYMBOL(edm_disp_str_to_timing);

static void edm_disp_string_matcher(char *string, int *type, int *length)
{
	int i;
	int tmp_length = 0;
	int tmp_type = ENUM_INVALID;

	if (string == NULL)
		goto out;

	for (i = 0; i < ENUM_INVALID; i++) {
		int str_len = strlen(disp_ctrls[i].str);
		if (strncmp(string, disp_ctrls[i].str, str_len) == 0) {
			tmp_type = disp_ctrls[i].type;
			tmp_length = str_len;
			break;
		}
	}
out:
	if (type != NULL)
		*type = tmp_type;
	if (length != NULL)
		*length = tmp_length;
}


static int edm_disp_process_cmdline(char *cmdline, struct edm_display_device *dev)
{
	#define TEMP_STR_LEN	200
	char cmd_sub_str[TEMP_STR_LEN];
	int i = 0;
	int str_len = 0;
	int processed = 0;
	int ctrl_type = ENUM_INVALID;

	for (i = 0; i < TEMP_STR_LEN; i++)
		cmd_sub_str[i] = '\0';

	edm_disp_string_matcher(cmdline, &ctrl_type, &str_len);
	if ((ctrl_type != ENUM_INVALID) && (str_len != 0)) {
		char *temp_str;
#if EDM_DISPLAY_DEBUG
		dbg_print("%s:  %s  ", __FUNCTION__, disp_ctrls[ctrl_type].str);
#endif
		cmdline += str_len;
		processed += str_len;
		temp_str = cmdline;
		while (*temp_str != '\0') {
			int match_type = ENUM_INVALID;
			edm_disp_string_matcher(temp_str, &match_type, NULL);
			if (match_type != ENUM_INVALID)
				break;
			else
				temp_str++;
		}

		if ((temp_str - cmdline) < TEMP_STR_LEN) {
			int len = (temp_str - cmdline);
			int j = 0;
			processed += len;
			strncpy(cmd_sub_str, cmdline, len);
			/* Remove all '"'*/
			for (i = 0; i < TEMP_STR_LEN; i++) {
				if (cmd_sub_str[i] != '"') {
					cmd_sub_str[j] = cmd_sub_str[i];
					j++;
				}
			}
			cmd_sub_str[j] = '\0';

			len = strlen(cmd_sub_str);
			if (cmd_sub_str[len - 1] == ',')
				cmd_sub_str[len - 1] = '\0';
		}
#if EDM_DISPLAY_DEBUG
		dbg_print(" \t%s\n", cmd_sub_str);
#endif
	}

	if (ctrl_type == ENUM_DEV) {
		if (strncmp(cmd_sub_str, "hdmi0", strlen("hdmi0")) == 0)
			dev->disp_dev = EDM_HDMI0;
		else if (strncmp(cmd_sub_str, "hdmi1", strlen("hdmi1")) == 0)
			dev->disp_dev = EDM_HDMI1;
		else if (strncmp(cmd_sub_str, "lcd0", strlen("lcd0")) == 0)
			dev->disp_dev = EDM_LCD0;
		else if (strncmp(cmd_sub_str, "lcd1", strlen("lcd1")) == 0)
			dev->disp_dev = EDM_LCD1;
		else if (strncmp(cmd_sub_str, "lvds0", strlen("lvds0")) == 0)
			dev->disp_dev = EDM_LVDS0;
		else if (strncmp(cmd_sub_str, "lvds1", strlen("lvds1")) == 0)
			dev->disp_dev = EDM_LVDS1;
		else if (strncmp(cmd_sub_str, "lvds2", strlen("lvds2")) == 0)
			dev->disp_dev = EDM_LVDS2;
		else if (strncmp(cmd_sub_str, "lvds3", strlen("lvds3")) == 0)
			dev->disp_dev = EDM_LVDS3;
		else if (strncmp(cmd_sub_str, "lvdsd", strlen("lvdsd")) == 0)
			dev->disp_dev = EDM_LVDSD_0_1;
		else if (strncmp(cmd_sub_str, "lvdsd0", strlen("lvdsd0")) == 0)
			dev->disp_dev = EDM_LVDSD_0_1;
		else if (strncmp(cmd_sub_str, "lvdsd01", strlen("lvdsd01")) == 0)
			dev->disp_dev = EDM_LVDSD_0_1;
		else if (strncmp(cmd_sub_str, "lvdsd1", strlen("lvdsd1")) == 0)
			dev->disp_dev = EDM_LVDSD_2_3;
		else if (strncmp(cmd_sub_str, "lvdsd23", strlen("lvdsd23")) == 0)
			dev->disp_dev = EDM_LVDSD_2_3;
		else if (strncmp(cmd_sub_str, "dsi0", strlen("dsi0")) == 0)
			dev->disp_dev = EDM_DSI0;
		else if (strncmp(cmd_sub_str, "dsi1", strlen("dsi1")) == 0)
			dev->disp_dev = EDM_DSI1;
		else
			dev->disp_dev = EDM_DEV_INVALID;

	} else if (ctrl_type == ENUM_MODE) {
		int str_len = strlen(cmd_sub_str);
		dev->mode_string = (char *)memalloc((str_len + 1) * sizeof(char));
		strncpy(dev->mode_string, cmd_sub_str, (str_len + 1));
	} else if (ctrl_type == ENUM_TIMING) {
		edm_disp_str_to_timing(&dev->timing, cmd_sub_str);
	} else if (ctrl_type == ENUM_IF) {
		int str_len = strlen(cmd_sub_str);
		dev->if_fmt = (char *)memalloc((str_len + 1) * sizeof(char));
		strncpy(dev->if_fmt, cmd_sub_str, (str_len + 1));
	} else if (ctrl_type == ENUM_BPP) {
		if (strncmp(cmd_sub_str, "32", 2) == 0)
			dev->bpp = 32;
		else if (strncmp(cmd_sub_str, "24", 2) == 0)
			dev->bpp = 24;
		else if (strncmp(cmd_sub_str, "16", 2) == 0)
			dev->bpp = 16;
		else
			dev->bpp = 0;
	}
	return processed;
}

/* Result
Display Device 0:
Transmitter	: hdmi0
Mode		: 1280x720MR-16@60
Connection	: RGB24
ColorDepth	: 32

Display Device 1:
Transmitter	: lcd1
Mode		: SEIKO-WVGA
Connection	: RGB666
ColorDepth	: 16

Display Device 2:
Transmitter	: lvds0
Mode		: LDB-CUSTOM
Timing
	PCLK	: 33260
	HRES	: 800
	HFP	: 210
	HBP	: 46
	HSW	: 1
	VRES	: 480
	VFP	: 22
	VBP	: 23
	VSW	: 1
	LINES	: 24
	BPP	: 32
Connection	: RGB565
ColorDepth	: 16
*/

void edm_disp_list_valid_devs(struct edm_display_device *display_devices,
						int num_displays)
{
	int i;

	if ((display_devices == NULL)) {
		dbg_print("%s : NULL inputs !!\n", __FUNCTION__);
		return;
	}

	for (i = 0; i < num_displays; i++) {
		struct edm_display_device *dev;
		dev = &display_devices[i];
		if (dev == NULL) {
			i = num_displays;
			break;
		}
		dbg_print("Display Device %d:\n", i);
		dbg_print("Transmitter\t: ");
		switch (dev->disp_dev) {
		case EDM_HDMI0:
			dbg_print("hdmi0");
			break;
		case EDM_HDMI1:
			dbg_print("hdmi1");
			break;
		case EDM_LVDS0:
			dbg_print("lvds0");
			break;
		case EDM_LVDS1:
			dbg_print("lvds1");
			break;
		case EDM_LVDS2:
			dbg_print("lvds2");
			break;
		case EDM_LVDS3:
			dbg_print("lvds3");
			break;
		case EDM_LVDSD_0_1:
			dbg_print("lvdsd_0_1");
			break;
		case EDM_LVDSD_2_3:
			dbg_print("lvdsd_2_3");
			break;
		case EDM_LCD0:
			dbg_print("lcd0");
			break;
		case EDM_LCD1:
			dbg_print("lcd1");
			break;
		case EDM_DSI0:
			dbg_print("dsi0");
			break;
		case EDM_DSI1:
			dbg_print("dsi1");
			break;
		default:
			dbg_print("Unknown");
			break;
		}
		dbg_print("\n");
		dbg_print("Mode\t\t: %s\n", dev->mode_string);
		if (dev->timing.pixclock) {
			dbg_print("Timing\n");
			dbg_print("\tPCLK\t: %d\n", dev->timing.pixclock);
			dbg_print("\tHRES\t: %d\n", dev->timing.hres);
			dbg_print("\tHFP\t: %d\n", dev->timing.hfp);
			dbg_print("\tHBP\t: %d\n", dev->timing.hbp);
			dbg_print("\tHSW\t: %d\n", dev->timing.hsw);
			dbg_print("\tVRES\t: %d\n", dev->timing.vres);
			dbg_print("\tVFP\t: %d\n", dev->timing.vfp);
			dbg_print("\tVBP\t: %d\n", dev->timing.vbp);
			dbg_print("\tVSW\t: %d\n", dev->timing.vsw);
		}
		dbg_print("Connection\t: %s\n", dev->if_fmt);
		dbg_print("ColorDepth\t: %d\n", dev->bpp);
		dbg_print("\n");
	}
}
EXPORT_SYMBOL(edm_disp_list_valid_devs);

static void edm_disp_check_lvds(struct edm_display_device *display_devices,
						int *num_displays)
{
	int lvds0 = 0;
	int lvds1 = 0;
	int lvds2 = 0;
	int lvds3 = 0;
	int lvdsd0 = 0;
	int lvdsd1 = 0;
	int i;

	for (i = 0; i < *num_displays; i++) {
		struct edm_display_device *dev;
		dev = &display_devices[i];
		switch (dev->disp_dev) {
		case EDM_LVDS0:
			lvds0 = 1;
			if (lvdsd0) {
				*num_displays = i;
				return;
			}
			break;
		case EDM_LVDS1:
			lvds1 = 1;
			if (lvdsd0) {
				*num_displays = i;
				return;
			}
			break;
		case EDM_LVDS2:
			lvds2 = 1;
			if (lvdsd1) {
				*num_displays = i;
				return;
			}
			break;
		case EDM_LVDS3:
			lvds3 = 1;
			if (lvdsd1) {
				*num_displays = i;
				return;
			}
			break;
		case EDM_LVDSD_0_1:
			lvdsd0 = 1;
			if (lvds0 || lvds1) {
				*num_displays = i;
				return;
			}
			break;
		case EDM_LVDSD_2_3:
			lvdsd1 = 1;
			if (lvds2 || lvds3) {
				*num_displays = i;
				return;
			}
			break;
		default:
			break;
		}
	}
}

#define MAX_DISPLAYS	8
char *disp_dev_str[MAX_DISPLAYS] = {NULL};

unsigned int edm_display_init(char *cmdline,
			struct edm_display_device *display_devices,
					unsigned int max_displays)
{
	int num_displays = 0;
	int i;

	cmdline  = cmdline;

	if ((max_displays == 0) ||
		(display_devices == NULL) ||
		(cmdline == NULL)) {
		return 0;
	}

	for (i = 0; i < max_displays; i++) {
		display_devices[i].disp_dev = EDM_DEV_INVALID;
		display_devices[i].mode_string = NULL;
		display_devices[i].timing.pixclock = 0;
		display_devices[i].if_fmt = NULL;
		display_devices[i].bpp = 0;
	}

	for (i = 0; i < MAX_DISPLAYS; i++)
		disp_dev_str[i] = NULL;

	if (strstr(cmdline, "display0=") == NULL)
		goto err_out;

	disp_dev_str[0] = strstr(cmdline, "display0=");
	disp_dev_str[0] += strlen("display0=");

	if (strstr(cmdline, "display1=") != NULL) {
		disp_dev_str[1] = strstr(cmdline, "display1=");
		disp_dev_str[1] += strlen("display1=");
	}

	if (strstr(cmdline, "display2=") != NULL) {
		disp_dev_str[2] = strstr(cmdline, "display2=");
		disp_dev_str[2] += strlen("display2=");
	}

	if (strstr(cmdline, "display3=") != NULL) {
		disp_dev_str[3] = strstr(cmdline, "display3=");
		disp_dev_str[3] += strlen("display3=");
	}

	if (strstr(cmdline, "display4=") != NULL) {
		disp_dev_str[4] = strstr(cmdline, "display4=");
		disp_dev_str[4] += strlen("display4=");
	}

	if (strstr(cmdline, "display5=") != NULL) {
		disp_dev_str[5] = strstr(cmdline, "display5=");
		disp_dev_str[5] += strlen("display5=");
	}

	if (strstr(cmdline, "display6=") != NULL) {
		disp_dev_str[6] = strstr(cmdline, "display6=");
		disp_dev_str[6] += strlen("display6=");
	}

	if (strstr(cmdline, "display7=") != NULL) {
		disp_dev_str[7] = strstr(cmdline, "display7=");
		disp_dev_str[7] += strlen("display7=");
	}

#if EDM_DISPLAY_DEBUG
	for (i = 0; i < max_displays; i++) {
		if (disp_dev_str[i] != NULL)
			dbg_print("%d : %s\n", i, disp_dev_str[i]);
	}
#endif

	for (i = 0; i < max_displays; i++) {
		if (disp_dev_str[i] != NULL) {
			char *str = disp_dev_str[i];
			struct edm_display_device *dev = display_devices + i;
			int prc_len = 0;

			num_displays++;

			while ((*str != '\0') && (*str != ' ')) {
				prc_len = edm_disp_process_cmdline(str, dev);
				if (prc_len != 0)
					str += prc_len;
				else if (*str == ',')
					str++;
				else
					str++;
			}
		} else {
			i = max_displays;
		}
	}

	edm_disp_check_lvds(display_devices, &num_displays);
#if EDM_DISPLAY_DEBUG
	edm_disp_list_valid_devs(display_devices, num_displays);
#endif

err_out:
	return num_displays;

}
EXPORT_SYMBOL(edm_display_init);

