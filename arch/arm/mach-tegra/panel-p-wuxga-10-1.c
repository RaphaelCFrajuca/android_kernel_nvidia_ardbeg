/*
 * arch/arm/mach-tegra/panel-p-wuxga-10-1.c
 *
 * Copyright (c) 2013, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <mach/dc.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/tegra_pwm_bl.h>
#include <linux/regulator/consumer.h>
#include <linux/pwm_backlight.h>
#include <linux/max8831_backlight.h>
#include <linux/leds.h>
#include <linux/ioport.h>
#include <linux/export.h>
#include <linux/mutex.h>

#include <generated/mach-types.h>

#include "board.h"
#include "board-panel.h"
#include "devices.h"
#include "gpio-names.h"
#include "tegra11_host1x_devices.h"

#define TEGRA_DSI_GANGED_MODE	0

#define DSI_PANEL_RESET		1

#define DC_CTRL_MODE	TEGRA_DC_OUT_CONTINUOUS_MODE
#define DSI_PANEL_BL_PWM	TEGRA_GPIO_PH1
#define en_vdd_bl	TEGRA_GPIO_PG0
#define lvds_en		TEGRA_GPIO_PG3
#define S818_LCD_DSI_40PIN 1
//#define S818_LCD_DSI_31PIN 1
extern void disable_ft5x06(void);
extern void enable_ft5x06(void);
extern void disable_gsensor(void);
extern void enable_gsensor(void);
extern void turn_off_all_led(void);
extern void turn_on_working_status_led(void);



#ifdef	S818_LCD_DSI_40PIN 
static bool reg_requested;
static bool gpio_requested;
static struct platform_device *disp_device;
static struct regulator *avdd_lcd_3v3;
static struct regulator *vdd_lcd_bl;
static struct regulator *vdd_lcd_bl_en;
static struct regulator *dvdd_lcd_1v8;
static struct regulator *vdd_ds_1v8;

static struct regulator *vdd_en_ds_1v8;

int early_suspend_flag=0;

extern void suspend_max1160(void);
extern void resume_max1160(void);
extern void get_suspend_flag(void);
extern void put_suspend_flag(void);


#define en_vdd_bl	TEGRA_GPIO_PG0
#define lvds_en		TEGRA_GPIO_PG3

static struct tegra_dc_sd_settings dsi_p_wuxga_10_1_sd_settings = {
	.enable = 1, /* enabled by default. */
	.use_auto_pwm = false,
	.hw_update_delay = 0,
	.bin_width = -1,
	.aggressiveness = 5,
	.use_vid_luma = false,
	.phase_in_adjustments = 0,
	.k_limit_enable = true,
	.k_limit = 200,
	.sd_window_enable = false,
	.soft_clipping_enable = true,
	/* Low soft clipping threshold to compensate for aggressive k_limit */
	.soft_clipping_threshold = 128,
	.smooth_k_enable = true,
	.smooth_k_incr = 4,
	/* Default video coefficients */
	.coeff = {5, 9, 2},
	.fc = {0, 0},
	/* Immediate backlight changes */
	.blp = {1024, 255},
	/* Gammas: R: 2.2 G: 2.2 B: 2.2 */
	/* Default BL TF */
	.bltf = {
			{
				{57, 65, 73, 82},
				{92, 103, 114, 125},
				{138, 150, 164, 178},
				{193, 208, 224, 241},
			},
		},
	/* Default LUT */
	.lut = {
			{
				{255, 255, 255},
				{199, 199, 199},
				{153, 153, 153},
				{116, 116, 116},
				{85, 85, 85},
				{59, 59, 59},
				{36, 36, 36},
				{17, 17, 17},
				{0, 0, 0},
			},
		},
	.sd_brightness = &sd_brightness,
	.use_vpulse2 = true,
};

static tegra_dc_bl_output dsi_p_wuxga_10_1_bl_output_measured = {
	0, 0, 1, 2, 3, 4, 5, 6,
	7, 8, 9, 9, 10, 11, 12, 13,
	13, 14, 15, 16, 17, 17, 18, 19,
	20, 21, 22, 22, 23, 24, 25, 26,
	27, 27, 28, 29, 30, 31, 32, 32,
	33, 34, 35, 36, 37, 37, 38, 39,
	40, 41, 42, 42, 43, 44, 45, 46,
	47, 48, 48, 49, 50, 51, 52, 53,
	54, 55, 56, 57, 57, 58, 59, 60,
	61, 62, 63, 64, 65, 66, 67, 68,
	69, 70, 71, 71, 72, 73, 74, 75,
	76, 77, 77, 78, 79, 80, 81, 82,
	83, 84, 85, 87, 88, 89, 90, 91,
	92, 93, 94, 95, 96, 97, 98, 99,
	100, 101, 102, 103, 104, 105, 106, 107,
	108, 109, 110, 111, 112, 113, 115, 116,
	117, 118, 119, 120, 121, 122, 123, 124,
	125, 126, 127, 128, 129, 130, 131, 132,
	133, 134, 135, 136, 137, 138, 139, 141,
	142, 143, 144, 146, 147, 148, 149, 151,
	152, 153, 154, 155, 156, 157, 158, 158,
	159, 160, 161, 162, 163, 165, 166, 167,
	168, 169, 170, 171, 172, 173, 174, 176,
	177, 178, 179, 180, 182, 183, 184, 185,
	186, 187, 188, 189, 190, 191, 192, 194,
	195, 196, 197, 198, 199, 200, 201, 202,
	203, 204, 205, 206, 207, 208, 209, 210,
	211, 212, 213, 214, 215, 216, 217, 219,
	220, 221, 222, 224, 225, 226, 227, 229,
	230, 231, 232, 233, 234, 235, 236, 238,
	239, 240, 241, 242, 243, 244, 245, 246,
	247, 248, 249, 250, 251, 252, 253, 255
};


static u8 init_cmd1[] = {0xB3,0x14,0x08,0x00,0x22,0x00};
static u8 init_cmd2[] = {0xB4,0x0C};
static u8 init_cmd3[] = {0xB6,0x3A,0xC3};
static u8 init_cmd4[] = {0x2A,0x00,0x00,0x04,0xAF};
static u8 init_cmd5[] = {0x2B,0x00,0x00,0x07,0x7F};


static struct tegra_dsi_cmd dsi_p_wuxga_10_1_init_cmd[] = {

	DSI_CMD_SHORT(0x05, 0x01, 0x00),
	DSI_DLY_MS(5),
    DSI_CMD_SHORT(0x23, 0xB0, 0x00),
    DSI_DLY_MS(3),
    DSI_CMD_LONG(0x29,  init_cmd1),
    DSI_DLY_MS(3),
	DSI_CMD_LONG(0x29,  init_cmd2),	
    DSI_CMD_LONG(0x29,  init_cmd3),
    DSI_CMD_SHORT(0x15, 0x51, 0xE6),
    DSI_CMD_SHORT(0x15, 0x53, 0x2C),
  //  DSI_CMD_SHORT(0x15, 0x3A, 0x77),

	DSI_CMD_SHORT(0x05, 0x29, 0x00),
   	DSI_DLY_MS(15),
	DSI_CMD_SHORT(0x05, 0x11, 0x00),
	DSI_DLY_MS(120),

};

static struct tegra_dsi_cmd dsi_p_1920x1200_suspend_cmd[] = {
    DSI_CMD_SHORT(0x05, 0x28, 0x00),
	DSI_DLY_MS(20),
    DSI_CMD_SHORT(0x05, 0x10, 0x00),
	DSI_DLY_MS(80),

};


static struct tegra_dsi_out dsi_p_wuxga_10_1_pdata = {

	.controller_vs = DSI_VS_1,
	.n_data_lanes = 4,
	.video_burst_mode = TEGRA_DSI_VIDEO_NONE_BURST_MODE_WITH_SYNC_END,

	.pixel_format = TEGRA_DSI_PIXEL_FORMAT_24BIT_P,
	.refresh_rate = 60,
	.virtual_channel = TEGRA_DSI_VIRTUAL_CHANNEL_0,

	.dsi_instance = DSI_INSTANCE_0,

	.panel_reset = 1,
	.power_saving_suspend = true,
	.video_data_type = TEGRA_DSI_VIDEO_TYPE_VIDEO_MODE,
	.video_clock_mode = TEGRA_DSI_VIDEO_CLOCK_CONTINUOUS,
	.dsi_init_cmd = dsi_p_wuxga_10_1_init_cmd,
	.n_init_cmd = ARRAY_SIZE(dsi_p_wuxga_10_1_init_cmd),
	.dsi_suspend_cmd = dsi_p_1920x1200_suspend_cmd,
	.n_suspend_cmd = ARRAY_SIZE(dsi_p_1920x1200_suspend_cmd),
};

static int dalmore_dsi_regulator_get(struct device *dev)
{
	int err = 0;

	if (reg_requested)
		return 0;
	avdd_lcd_3v3 = regulator_get(dev, "avdd_lcd");
	if (IS_ERR(avdd_lcd_3v3)) {
		pr_err("avdd_lcd regulator get failed\n");
		err = PTR_ERR(avdd_lcd_3v3);
		avdd_lcd_3v3 = NULL;
		goto fail;
	}

	dvdd_lcd_1v8 = regulator_get(dev, "dvdd_lcd");
	if (IS_ERR(dvdd_lcd_1v8)) {
		pr_err("dvdd_lcd_1v8 regulator get failed\n");
		err = PTR_ERR(dvdd_lcd_1v8);
		dvdd_lcd_1v8 = NULL;
		goto fail;
	}

	vdd_lcd_bl = regulator_get(dev, "vdd_lcd_bl");
	if (IS_ERR(vdd_lcd_bl)) {
		pr_err("vdd_lcd_bl regulator get failed\n");
		err = PTR_ERR(vdd_lcd_bl);
		vdd_lcd_bl = NULL;
		goto fail;
	}

	vdd_lcd_bl_en = regulator_get(dev, "vdd_lcd_bl_en");
	if (IS_ERR(vdd_lcd_bl_en)) {
		pr_err("vdd_lcd_bl_en regulator get failed\n");
		err = PTR_ERR(vdd_lcd_bl_en);
		vdd_lcd_bl_en = NULL;
		goto fail;
	}
	reg_requested = true;
	return 0;
fail:
	return err;
}

static int dalmore_dsi_gpio_get(void)
{
	int err = 0;

	if (gpio_requested)
		return 0;

	err = gpio_request(dsi_p_wuxga_10_1_pdata.dsi_panel_rst_gpio,
		"panel rst");
	if (err < 0) {
		pr_err("panel reset gpio request failed\n");
		goto fail;
	}

	/* free pwm GPIO */
	err = gpio_request(dsi_p_wuxga_10_1_pdata.dsi_panel_bl_pwm_gpio,
		"panel pwm");
	if (err < 0) {
		pr_err("panel pwm gpio request failed\n");
		goto fail;
	}
	gpio_free(dsi_p_wuxga_10_1_pdata.dsi_panel_bl_pwm_gpio);
	gpio_requested = true;
	return 0;
fail:
	return err;
}

static int macallan_dsi_regulator_get(struct device *dev)
{
	int err = 0;

	if (reg_requested)
		return 0;
	avdd_lcd_3v3 = regulator_get(dev, "avdd_lcd");
	if (IS_ERR(avdd_lcd_3v3)) {
		pr_err("avdd_lcd regulator get failed\n");
		err = PTR_ERR(avdd_lcd_3v3);
		avdd_lcd_3v3 = NULL;
		goto fail;
	}

	vdd_lcd_bl_en = regulator_get(dev, "vdd_lcd_bl_en");
	if (IS_ERR(vdd_lcd_bl_en)) {
		pr_err("vdd_lcd_bl_en regulator get failed\n");
		err = PTR_ERR(vdd_lcd_bl_en);
		vdd_lcd_bl_en = NULL;
		goto fail;
	}
	reg_requested = true;
	return 0;
fail:
	return err;
}

static int macallan_dsi_gpio_get(void)
{
	int err = 0;

	if (gpio_requested)
		return 0;

	err = gpio_request(dsi_p_wuxga_10_1_pdata.dsi_panel_rst_gpio,
		"panel rst");
	if (err < 0) {
		pr_err("panel reset gpio request failed\n");
		goto fail;
	}

	/* free pwm GPIO */
	err = gpio_request(dsi_p_wuxga_10_1_pdata.dsi_panel_bl_pwm_gpio,
		"panel pwm");
	if (err < 0) {
		pr_err("panel pwm gpio request failed\n");
		goto fail;
	}
	gpio_free(dsi_p_wuxga_10_1_pdata.dsi_panel_bl_pwm_gpio);
	gpio_requested = true;
	return 0;
fail:
	return err;
}






static int dsi_p_wuxga_10_1_enable(struct device *dev)
{
	int err = 0;
	get_suspend_flag();
	early_suspend_flag = 0;
	put_suspend_flag();

	resume_max1160();
	enable_gsensor();
	enable_ft5x06();
	turn_on_working_status_led();
	//printk("11111111111111111111\n");
	gpio_direction_output(TEGRA_GPIO_PH3,1);

	vdd_en_ds_1v8 = regulator_get(NULL, "ldoen");
	if (IS_ERR(vdd_en_ds_1v8)) {
		pr_err("ldoen regulator get failed\n");
		err = PTR_ERR(vdd_en_ds_1v8);
		vdd_en_ds_1v8 = NULL;
		goto fail;
	}
	err = regulator_enable(vdd_en_ds_1v8);
	if (err < 0) {
		pr_err("ldoen regulator enable failed\n");
		goto fail;
	}

#if DSI_PANEL_RESET
	mdelay(20);
	gpio_direction_output(TEGRA_GPIO_PR4,1);	// vs
	mdelay(20);
	gpio_direction_output(TEGRA_GPIO_PH3,1);  // reset
	mdelay(20);
	gpio_direction_output(TEGRA_GPIO_PR4,0);
	mdelay(20);
	gpio_direction_output(TEGRA_GPIO_PR4,1);	// vs
	mdelay(20);
#endif
	gpio_direction_output(TEGRA_GPIO_PH2,1);    // blacklight enable
	

	return 0;
fail:
	return err;
}




static int dsi_p_wuxga_10_1_disable(void)
{
	//printk("000000000000000000000000000\n");
	get_suspend_flag();
	early_suspend_flag = 1;
	put_suspend_flag();
	gpio_direction_output(TEGRA_GPIO_PH2,0);
	if (vdd_en_ds_1v8)
		regulator_disable(vdd_en_ds_1v8);
	gpio_direction_output(TEGRA_GPIO_PH3,0);
	gpio_direction_output(TEGRA_GPIO_PR4,0);
	suspend_max1160();
	disable_ft5x06();
	disable_gsensor();
	turn_off_all_led();

	return 0;
}

static int dsi_p_wuxga_10_1_postsuspend(void)
{
//	printk("sssssssssssssssssssss\n");
	return 0;
}

static struct tegra_dc_mode dsi_p_wuxga_10_1_modes[] = {
	{
		.pclk = 154700000,
		.h_ref_to_sync = 4,
		.v_ref_to_sync = 1,
		.h_sync_width = 16,
		.v_sync_width = 2,
		.h_back_porch = 32,
		.v_back_porch = 16,
		.h_active = 1200,
		.v_active = 1920,
		.h_front_porch = 120,
		.v_front_porch = 17,
	},
};

#ifdef CONFIG_TEGRA_DC_CMU
static struct tegra_dc_cmu dsi_p_wuxga_10_1_cmu = {
	/* lut1 maps sRGB to linear space. */
	{
	0,    1,    2,    4,    5,    6,    7,    9,
	10,   11,   12,   14,   15,   16,   18,   20,
	21,   23,   25,   27,   29,   31,   33,   35,
	37,   40,   42,   45,   48,   50,   53,   56,
	59,   62,   66,   69,   72,   76,   79,   83,
	87,   91,   95,   99,   103,  107,  112,  116,
	121,  126,  131,  136,  141,  146,  151,  156,
	162,  168,  173,  179,  185,  191,  197,  204,
	210,  216,  223,  230,  237,  244,  251,  258,
	265,  273,  280,  288,  296,  304,  312,  320,
	329,  337,  346,  354,  363,  372,  381,  390,
	400,  409,  419,  428,  438,  448,  458,  469,
	479,  490,  500,  511,  522,  533,  544,  555,
	567,  578,  590,  602,  614,  626,  639,  651,
	664,  676,  689,  702,  715,  728,  742,  755,
	769,  783,  797,  811,  825,  840,  854,  869,
	884,  899,  914,  929,  945,  960,  976,  992,
	1008, 1024, 1041, 1057, 1074, 1091, 1108, 1125,
	1142, 1159, 1177, 1195, 1213, 1231, 1249, 1267,
	1286, 1304, 1323, 1342, 1361, 1381, 1400, 1420,
	1440, 1459, 1480, 1500, 1520, 1541, 1562, 1582,
	1603, 1625, 1646, 1668, 1689, 1711, 1733, 1755,
	1778, 1800, 1823, 1846, 1869, 1892, 1916, 1939,
	1963, 1987, 2011, 2035, 2059, 2084, 2109, 2133,
	2159, 2184, 2209, 2235, 2260, 2286, 2312, 2339,
	2365, 2392, 2419, 2446, 2473, 2500, 2527, 2555,
	2583, 2611, 2639, 2668, 2696, 2725, 2754, 2783,
	2812, 2841, 2871, 2901, 2931, 2961, 2991, 3022,
	3052, 3083, 3114, 3146, 3177, 3209, 3240, 3272,
	3304, 3337, 3369, 3402, 3435, 3468, 3501, 3535,
	3568, 3602, 3636, 3670, 3705, 3739, 3774, 3809,
	3844, 3879, 3915, 3950, 3986, 4022, 4059, 4095,
	},
	/* csc */
	{
		0x138, 0x3Ba, 0x00D, /* 1.220 -0.270 0.050 */
		0x3F5, 0x120, 0x3E6, /* 0.038 1.124 0.100 */
		0x3FE, 0x3F8, 0x0E9, /* -0.002 -0.026 0.913 */
	},
	/* lut2 maps linear space to sRGB */
	{
		0, 1, 2, 3, 4, 5, 6, 6,
		7, 8, 9, 10, 11, 11, 12, 13,
		13, 14, 15, 15, 16, 17, 17, 18,
		18, 19, 19, 20, 20, 21, 21, 22,
		22, 23, 23, 23, 24, 24, 24, 25,
		25, 25, 26, 26, 26, 27, 27, 27,
		28, 28, 28, 28, 29, 29, 29, 29,
		30, 30, 30, 30, 31, 31, 31, 31,
		32, 32, 32, 32, 33, 33, 33, 33,
		34, 34, 34, 35, 35, 35, 35, 36,
		36, 36, 37, 37, 37, 37, 38, 38,
		38, 39, 39, 39, 39, 40, 40, 40,
		41, 41, 41, 41, 42, 42, 42, 43,
		43, 43, 43, 44, 44, 44, 45, 45,
		45, 45, 46, 46, 46, 46, 47, 47,
		47, 47, 48, 48, 48, 48, 49, 49,
		49, 49, 50, 50, 50, 50, 50, 51,
		51, 51, 51, 52, 52, 52, 52, 52,
		53, 53, 53, 53, 53, 53, 54, 54,
		54, 54, 54, 55, 55, 55, 55, 55,
		55, 56, 56, 56, 56, 56, 56, 57,
		57, 57, 57, 57, 57, 57, 58, 58,
		58, 58, 58, 58, 59, 59, 59, 59,
		59, 59, 59, 60, 60, 60, 60, 60,
		60, 60, 61, 61, 61, 61, 61, 61,
		61, 62, 62, 62, 62, 62, 62, 62,
		63, 63, 63, 63, 63, 63, 63, 64,
		64, 64, 64, 64, 64, 64, 65, 65,
		65, 65, 65, 65, 66, 66, 66, 66,
		66, 66, 66, 67, 67, 67, 67, 67,
		67, 68, 68, 68, 68, 68, 68, 69,
		69, 69, 69, 69, 69, 70, 70, 70,
		70, 70, 70, 71, 71, 71, 71, 71,
		71, 72, 72, 72, 72, 72, 72, 73,
		73, 73, 73, 73, 73, 74, 74, 74,
		74, 74, 74, 74, 75, 75, 75, 75,
		75, 75, 76, 76, 76, 76, 76, 76,
		77, 77, 77, 77, 77, 77, 77, 78,
		78, 78, 78, 78, 78, 79, 79, 79,
		79, 79, 79, 79, 80, 80, 80, 80,
		80, 80, 80, 80, 81, 81, 81, 81,
		81, 81, 81, 82, 82, 82, 82, 82,
		82, 82, 82, 83, 83, 83, 83, 83,
		83, 83, 83, 83, 84, 84, 84, 84,
		84, 84, 84, 84, 85, 85, 85, 85,
		85, 85, 85, 85, 85, 85, 86, 86,
		86, 86, 86, 86, 86, 86, 86, 86,
		87, 87, 87, 87, 87, 87, 87, 87,
		87, 87, 88, 88, 88, 88, 88, 88,
		88, 88, 88, 88, 88, 88, 89, 89,
		89, 89, 89, 89, 89, 89, 89, 89,
		89, 89, 90, 90, 90, 90, 90, 90,
		90, 90, 90, 90, 90, 90, 91, 91,
		91, 91, 91, 91, 91, 91, 91, 91,
		91, 91, 91, 92, 92, 92, 92, 92,
		92, 92, 92, 92, 92, 92, 92, 92,
		93, 93, 93, 93, 93, 93, 93, 93,
		93, 93, 93, 93, 93, 93, 94, 94,
		94, 94, 94, 94, 94, 94, 94, 94,
		94, 94, 94, 94, 95, 95, 95, 95,
		95, 95, 95, 95, 95, 95, 95, 95,
		95, 96, 96, 96, 96, 96, 96, 96,
		96, 96, 96, 96, 96, 96, 97, 97,
		97, 97, 97, 97, 97, 97, 97, 97,
		98, 99, 99, 100, 101, 101, 102, 103,
		103, 104, 105, 105, 106, 107, 107, 108,
		109, 110, 110, 111, 112, 112, 113, 114,
		114, 115, 115, 116, 117, 117, 118, 119,
		119, 120, 120, 121, 121, 122, 123, 123,
		124, 124, 125, 125, 126, 126, 127, 128,
		128, 129, 129, 130, 130, 131, 131, 132,
		132, 133, 133, 134, 134, 135, 135, 136,
		136, 137, 138, 138, 139, 139, 140, 140,
		141, 141, 142, 142, 143, 143, 144, 144,
		144, 145, 145, 146, 146, 147, 147, 148,
		148, 149, 149, 150, 150, 151, 151, 152,
		152, 153, 153, 153, 154, 154, 155, 155,
		156, 156, 157, 157, 157, 158, 158, 159,
		159, 160, 160, 160, 161, 161, 162, 162,
		162, 163, 163, 164, 164, 164, 165, 165,
		165, 166, 166, 167, 167, 167, 168, 168,
		168, 169, 169, 169, 170, 170, 171, 171,
		171, 172, 172, 172, 173, 173, 173, 174,
		174, 174, 175, 175, 175, 176, 176, 176,
		177, 177, 177, 178, 178, 178, 179, 179,
		179, 180, 180, 180, 181, 181, 181, 182,
		182, 182, 183, 183, 183, 184, 184, 184,
		185, 185, 185, 185, 186, 186, 186, 187,
		187, 187, 188, 188, 188, 189, 189, 189,
		190, 190, 190, 191, 191, 191, 191, 192,
		192, 192, 193, 193, 193, 194, 194, 194,
		195, 195, 195, 195, 196, 196, 196, 197,
		197, 197, 198, 198, 198, 199, 199, 199,
		199, 200, 200, 200, 201, 201, 201, 202,
		202, 202, 203, 203, 203, 203, 204, 204,
		204, 205, 205, 205, 206, 206, 206, 207,
		207, 207, 208, 208, 208, 208, 209, 209,
		209, 210, 210, 210, 211, 211, 211, 212,
		212, 212, 213, 213, 213, 214, 214, 214,
		215, 215, 215, 215, 216, 216, 216, 217,
		217, 217, 218, 218, 218, 219, 219, 219,
		220, 220, 220, 220, 221, 221, 221, 222,
		222, 222, 222, 223, 223, 223, 224, 224,
		224, 224, 225, 225, 225, 226, 226, 226,
		226, 227, 227, 227, 227, 228, 228, 228,
		229, 229, 229, 229, 230, 230, 230, 230,
		230, 231, 231, 231, 231, 232, 232, 232,
		232, 233, 233, 233, 233, 234, 234, 234,
		234, 234, 235, 235, 235, 235, 236, 236,
		236, 236, 236, 237, 237, 237, 237, 238,
		238, 238, 238, 238, 239, 239, 239, 239,
		239, 240, 240, 240, 240, 240, 241, 241,
		241, 241, 241, 242, 242, 242, 242, 243,
		243, 243, 243, 243, 244, 244, 244, 244,
		244, 245, 245, 245, 245, 245, 246, 246,
		246, 246, 246, 247, 247, 247, 247, 248,
		248, 248, 248, 248, 249, 249, 249, 249,
		250, 250, 250, 250, 251, 251, 251, 251,
		251, 252, 252, 252, 253, 253, 253, 253,
		254, 254, 254, 254, 255, 255, 255, 255,
	},
};
#endif

static int dsi_p_wuxga_10_1_bl_notify(struct device *unused, int brightness)
{
	int cur_sd_brightness = atomic_read(&sd_brightness);

	/* SD brightness is a percentage */
	brightness = (brightness * cur_sd_brightness) / 255;

	/* Apply any backlight response curve */
	if (brightness > 255)
		pr_info("Error: Brightness > 255!\n");
	else
		brightness = dsi_p_wuxga_10_1_bl_output_measured[brightness];

	return brightness;
}

static int dsi_p_wuxga_10_1_check_fb(struct device *dev, struct fb_info *info)
{
	return info->device == &disp_device->dev;
}

static struct platform_pwm_backlight_data dsi_p_wuxga_10_1_bl_data = {
	.pwm_id		= 1,
	.max_brightness	= 255,//max backlight 80% of  absolute maximum ratings.
	.dft_brightness	= 128,
	//.lth_brightness = 30,//min backlight brightness
	.lth_brightness = 50,//min backlight brightness
	.pwm_period_ns	= 40000,
	.pwm_gpio = DSI_PANEL_BL_PWM,
	.notify		= dsi_p_wuxga_10_1_bl_notify,

	/* Only toggle backlight on fb blank notifications for disp1 */
	.check_fb	= dsi_p_wuxga_10_1_check_fb,
};

static struct platform_device __maybe_unused
		dsi_p_wuxga_10_1_bl_device = {
	.name	= "pwm-backlight",
	.id	= -1,
	.dev	= {
		.platform_data = &dsi_p_wuxga_10_1_bl_data,
	},
};

static struct platform_device __maybe_unused
			*dsi_p_wuxga_10_1_bl_devices[] __initdata = {
	&tegra_pwfm_device,
	&dsi_p_wuxga_10_1_bl_device,
};

static int  __init dsi_p_wuxga_10_1_register_bl_dev(void)
{
	int err = 0;

	err = platform_add_devices(dsi_p_wuxga_10_1_bl_devices,
				ARRAY_SIZE(dsi_p_wuxga_10_1_bl_devices));
	if (err) {
		pr_err("disp1 bl device registration failed");
		return err;
	}
	return err;
}

static void dsi_p_wuxga_10_1_set_disp_device(
	struct platform_device *display_device)
{
	disp_device = display_device;
}

static void dsi_p_wuxga_10_1_dc_out_init(struct tegra_dc_out *dc)
{
	dc->dsi = &dsi_p_wuxga_10_1_pdata;
	dc->parent_clk = "pll_d_out0";
	dc->modes = dsi_p_wuxga_10_1_modes;
	dc->n_modes = ARRAY_SIZE(dsi_p_wuxga_10_1_modes);
	dc->enable = dsi_p_wuxga_10_1_enable;
	dc->disable = dsi_p_wuxga_10_1_disable;
	dc->postsuspend	= dsi_p_wuxga_10_1_postsuspend,
	dc->width = 217;
	dc->height = 135;
	dc->flags = DC_CTRL_MODE;
	
}

static void dsi_p_wuxga_10_1_fb_data_init(struct tegra_fb_data *fb)
{
	fb->xres = dsi_p_wuxga_10_1_modes[0].h_active;
	fb->yres = dsi_p_wuxga_10_1_modes[0].v_active;
}

static void
dsi_p_wuxga_10_1_sd_settings_init(struct tegra_dc_sd_settings *settings)
{
	*settings = dsi_p_wuxga_10_1_sd_settings;
	settings->bl_device_name = "pwm-backlight";
}

static void dsi_p_wuxga_10_1_cmu_init(struct tegra_dc_platform_data *pdata)
{
	pdata->cmu = &dsi_p_wuxga_10_1_cmu;
}
#endif


#ifdef	S818_LCD_DSI_31PIN
static bool reg_requested;
static bool gpio_requested;
static struct platform_device *disp_device;
static struct regulator *avdd_lcd_3v3;
static struct regulator *vdd_lcd_bl;
static struct regulator *vdd_lcd_bl_en;
static struct regulator *dvdd_lcd_1v8;
static struct regulator *vdd_ds_1v8;

static struct regulator *vdd_en_ds_1v8;
static struct regulator *vdd_rst_3v3;

int early_suspend_flag=0;

extern void suspend_max1160(void);
extern void resume_max1160(void);
extern void get_suspend_flag(void);
extern void put_suspend_flag(void);


#define en_vdd_bl	TEGRA_GPIO_PG0
#define lvds_en		TEGRA_GPIO_PG3

static struct tegra_dc_sd_settings dsi_p_wuxga_10_1_sd_settings = {
	.enable = 1, /* enabled by default. */
	.use_auto_pwm = false,
	.hw_update_delay = 0,
	.bin_width = -1,
	.aggressiveness = 5,
	.use_vid_luma = false,
	.phase_in_adjustments = 0,
	.k_limit_enable = true,
	.k_limit = 200,
	.sd_window_enable = false,
	.soft_clipping_enable = true,
	/* Low soft clipping threshold to compensate for aggressive k_limit */
	.soft_clipping_threshold = 128,
	.smooth_k_enable = true,
	.smooth_k_incr = 4,
	/* Default video coefficients */
	.coeff = {5, 9, 2},
	.fc = {0, 0},
	/* Immediate backlight changes */
	.blp = {1024, 255},
	/* Gammas: R: 2.2 G: 2.2 B: 2.2 */
	/* Default BL TF */
	.bltf = {
			{
				{57, 65, 73, 82},
				{92, 103, 114, 125},
				{138, 150, 164, 178},
				{193, 208, 224, 241},
			},
		},
	/* Default LUT */
	.lut = {
			{
				{255, 255, 255},
				{199, 199, 199},
				{153, 153, 153},
				{116, 116, 116},
				{85, 85, 85},
				{59, 59, 59},
				{36, 36, 36},
				{17, 17, 17},
				{0, 0, 0},
			},
		},
	.sd_brightness = &sd_brightness,
	.use_vpulse2 = true,
};

static tegra_dc_bl_output dsi_p_wuxga_10_1_bl_output_measured = {
	0, 0, 1, 2, 3, 4, 5, 6,
	7, 8, 9, 9, 10, 11, 12, 13,
	13, 14, 15, 16, 17, 17, 18, 19,
	20, 21, 22, 22, 23, 24, 25, 26,
	27, 27, 28, 29, 30, 31, 32, 32,
	33, 34, 35, 36, 37, 37, 38, 39,
	40, 41, 42, 42, 43, 44, 45, 46,
	47, 48, 48, 49, 50, 51, 52, 53,
	54, 55, 56, 57, 57, 58, 59, 60,
	61, 62, 63, 64, 65, 66, 67, 68,
	69, 70, 71, 71, 72, 73, 74, 75,
	76, 77, 77, 78, 79, 80, 81, 82,
	83, 84, 85, 87, 88, 89, 90, 91,
	92, 93, 94, 95, 96, 97, 98, 99,
	100, 101, 102, 103, 104, 105, 106, 107,
	108, 109, 110, 111, 112, 113, 115, 116,
	117, 118, 119, 120, 121, 122, 123, 124,
	125, 126, 127, 128, 129, 130, 131, 132,
	133, 134, 135, 136, 137, 138, 139, 141,
	142, 143, 144, 146, 147, 148, 149, 151,
	152, 153, 154, 155, 156, 157, 158, 158,
	159, 160, 161, 162, 163, 165, 166, 167,
	168, 169, 170, 171, 172, 173, 174, 176,
	177, 178, 179, 180, 182, 183, 184, 185,
	186, 187, 188, 189, 190, 191, 192, 194,
	195, 196, 197, 198, 199, 200, 201, 202,
	203, 204, 205, 206, 207, 208, 209, 210,
	211, 212, 213, 214, 215, 216, 217, 219,
	220, 221, 222, 224, 225, 226, 227, 229,
	230, 231, 232, 233, 234, 235, 236, 238,
	239, 240, 241, 242, 243, 244, 245, 246,
	247, 248, 249, 250, 251, 252, 253, 255
};


static u8 init_cmd1[] = {0xB3,0x04,0x08,0x00,0x22,0x00};
static u8 init_cmd2[] = {0xB4,0x0C};
static u8 init_cmd3[] = {0xB6,0x3A,0xC3};
static u8 init_cmd4[] = {0x2A,0x00,0x00,0x04,0xAF};
static u8 init_cmd5[] = {0x2B,0x00,0x00,0x07,0x7F};

static struct tegra_dsi_cmd dsi_p_wuxga_10_1_init_cmd[] = {
	DSI_CMD_SHORT(0x05, 0x01, 0x00),
	DSI_DLY_MS(5),
	DSI_CMD_SHORT(0x05, 0x11, 0x00),
	DSI_DLY_MS(100),
    DSI_CMD_SHORT(0x23, 0xB0, 0x00),
	DSI_DLY_MS(3),
    DSI_CMD_LONG(0x29,  init_cmd1),
	DSI_DLY_MS(3),
	DSI_CMD_LONG(0x29,  init_cmd2),	
	DSI_DLY_MS(3),
    DSI_CMD_LONG(0x29,  init_cmd3),
	DSI_DLY_MS(3),
    DSI_CMD_SHORT(0x15, 0x51, 0xE6),
	DSI_DLY_MS(3),
    DSI_CMD_SHORT(0x15, 0x53, 0x2C),
	DSI_DLY_MS(3),
    DSI_CMD_SHORT(0x15, 0x3A, 0x77),
	DSI_DLY_MS(3),
	DSI_CMD_LONG(0x39,	init_cmd4),
	DSI_DLY_MS(3),
	DSI_CMD_LONG(0x39,	init_cmd5),
	DSI_DLY_MS(3),
	DSI_CMD_SHORT(0x05, 0x29, 0x00),
   	DSI_DLY_MS(150),
	DSI_CMD_SHORT(0x05, 0x11, 0x00),
	DSI_DLY_MS(120),
	
//	DSI_CMD_LONG(0x29,  init_cmd1),
};

static struct tegra_dsi_cmd dsi_p_1920x1200_suspend_cmd[] = {
    DSI_CMD_SHORT(0x05, 0x28, 0x00),
	DSI_DLY_MS(20),
    DSI_CMD_SHORT(0x05, 0x10, 0x00),
	DSI_DLY_MS(80),

};


static struct tegra_dsi_out dsi_p_wuxga_10_1_pdata = {

	.controller_vs = DSI_VS_1,
	.n_data_lanes = 4,
	.video_burst_mode = TEGRA_DSI_VIDEO_NONE_BURST_MODE_WITH_SYNC_END,

	.pixel_format = TEGRA_DSI_PIXEL_FORMAT_24BIT_P,
	.refresh_rate = 60,
	.virtual_channel = TEGRA_DSI_VIRTUAL_CHANNEL_0,

	.dsi_instance = DSI_INSTANCE_0,

	.panel_reset = 0,
	.power_saving_suspend = true,
	.video_data_type = TEGRA_DSI_VIDEO_TYPE_COMMAND_MODE,
	.video_clock_mode = TEGRA_DSI_VIDEO_CLOCK_CONTINUOUS,
	.dsi_init_cmd = dsi_p_wuxga_10_1_init_cmd,
	.n_init_cmd = ARRAY_SIZE(dsi_p_wuxga_10_1_init_cmd),
	.dsi_suspend_cmd = dsi_p_1920x1200_suspend_cmd,
	.n_suspend_cmd = ARRAY_SIZE(dsi_p_1920x1200_suspend_cmd),
};

static int dsi_p_wuxga_10_1_enable(struct device *dev)
{
	int err = 0;
	get_suspend_flag();
	early_suspend_flag = 0;
	put_suspend_flag();

	resume_max1160();
	printk("11111111111111111111\n");
	
	//gpio_direction_output(169,0);
	gpio_direction_output(67,1);
	gpio_direction_output(TEGRA_GPIO_PH3,1);

	vdd_en_ds_1v8 = regulator_get(NULL, "ldoen");
	if (IS_ERR(vdd_en_ds_1v8)) {
		pr_err("ldoen regulator get failed\n");
		err = PTR_ERR(vdd_en_ds_1v8);
		vdd_en_ds_1v8 = NULL;
		goto fail;
	}
	err = regulator_enable(vdd_en_ds_1v8);
	if (err < 0) {
		pr_err("ldoen regulator enable failed\n");
		goto fail;
	}
	vdd_rst_3v3= regulator_get(NULL, "avdd_lcd");
	if (IS_ERR(vdd_rst_3v3)) {
		pr_err("ldoen regulator get failed\n");
		err = PTR_ERR(vdd_rst_3v3);
		vdd_rst_3v3 = NULL;
		goto fail;
	}
	
	err = regulator_enable(vdd_rst_3v3);
	if (err < 0) {
		pr_err("ldoen vdd_rst_3v3 regulator enable failed\n");
		goto fail;
	}
#if DSI_PANEL_RESET
//	mdelay(10);
//	gpio_direction_output(TEGRA_GPIO_PR4,0);
//	gpio_direction_output(TEGRA_GPIO_PH3,1);  // reset
	

	mdelay(20);
#endif
	
	gpio_direction_output(TEGRA_GPIO_PR4,1);	// vs
	mdelay(20);
	//gpio_direction_output(169,1);
	mdelay(200);
	gpio_direction_output(TEGRA_GPIO_PH2,1);    // blacklight enable
	mdelay(30);

	return 0;
fail:
	return err;
}




static int dsi_p_wuxga_10_1_disable(void)
{
	printk("000000000000000000000000000\n");
	get_suspend_flag();
	early_suspend_flag = 1;
	put_suspend_flag();
	gpio_direction_output(TEGRA_GPIO_PH2,0);
	if (vdd_en_ds_1v8)
		regulator_disable(vdd_en_ds_1v8);
	if (vdd_rst_3v3)
		regulator_disable(vdd_rst_3v3);

	mdelay(20);
	//gpio_direction_output(TEGRA_GPIO_PH3,0);
	gpio_direction_output(TEGRA_GPIO_PR4,0);
	//gpio_direction_output(169,0);
	suspend_max1160();
	return 0;
}

static int dsi_p_wuxga_10_1_postsuspend(void)
{
//	printk("sssssssssssssssssssss\n");
	return 0;
}

static struct tegra_dc_mode dsi_p_wuxga_10_1_modes[] = {
	{
		.pclk = 154700000,
		.h_ref_to_sync = 2,
		.v_ref_to_sync = 2,
		.h_sync_width = 12,
		.v_sync_width = 2,
		.h_back_porch = 60,
		.v_back_porch = 8,
		.h_active = 1200,
		.v_active = 1920,
		.h_front_porch = 120,
		.v_front_porch = 8,
	},
};

#ifdef CONFIG_TEGRA_DC_CMU
static struct tegra_dc_cmu dsi_p_wuxga_10_1_cmu = {
	/* lut1 maps sRGB to linear space. */
	{
	0,    1,    2,    4,    5,    6,    7,    9,
	10,   11,   12,   14,   15,   16,   18,   20,
	21,   23,   25,   27,   29,   31,   33,   35,
	37,   40,   42,   45,   48,   50,   53,   56,
	59,   62,   66,   69,   72,   76,   79,   83,
	87,   91,   95,   99,   103,  107,  112,  116,
	121,  126,  131,  136,  141,  146,  151,  156,
	162,  168,  173,  179,  185,  191,  197,  204,
	210,  216,  223,  230,  237,  244,  251,  258,
	265,  273,  280,  288,  296,  304,  312,  320,
	329,  337,  346,  354,  363,  372,  381,  390,
	400,  409,  419,  428,  438,  448,  458,  469,
	479,  490,  500,  511,  522,  533,  544,  555,
	567,  578,  590,  602,  614,  626,  639,  651,
	664,  676,  689,  702,  715,  728,  742,  755,
	769,  783,  797,  811,  825,  840,  854,  869,
	884,  899,  914,  929,  945,  960,  976,  992,
	1008, 1024, 1041, 1057, 1074, 1091, 1108, 1125,
	1142, 1159, 1177, 1195, 1213, 1231, 1249, 1267,
	1286, 1304, 1323, 1342, 1361, 1381, 1400, 1420,
	1440, 1459, 1480, 1500, 1520, 1541, 1562, 1582,
	1603, 1625, 1646, 1668, 1689, 1711, 1733, 1755,
	1778, 1800, 1823, 1846, 1869, 1892, 1916, 1939,
	1963, 1987, 2011, 2035, 2059, 2084, 2109, 2133,
	2159, 2184, 2209, 2235, 2260, 2286, 2312, 2339,
	2365, 2392, 2419, 2446, 2473, 2500, 2527, 2555,
	2583, 2611, 2639, 2668, 2696, 2725, 2754, 2783,
	2812, 2841, 2871, 2901, 2931, 2961, 2991, 3022,
	3052, 3083, 3114, 3146, 3177, 3209, 3240, 3272,
	3304, 3337, 3369, 3402, 3435, 3468, 3501, 3535,
	3568, 3602, 3636, 3670, 3705, 3739, 3774, 3809,
	3844, 3879, 3915, 3950, 3986, 4022, 4059, 4095,
	},
	/* csc */
	{
		0x138, 0x3Ba, 0x00D, /* 1.220 -0.270 0.050 */
		0x3F5, 0x120, 0x3E6, /* 0.038 1.124 0.100 */
		0x3FE, 0x3F8, 0x0E9, /* -0.002 -0.026 0.913 */
	},
	/* lut2 maps linear space to sRGB */
	{
		0, 1, 2, 3, 4, 5, 6, 6,
		7, 8, 9, 10, 11, 11, 12, 13,
		13, 14, 15, 15, 16, 17, 17, 18,
		18, 19, 19, 20, 20, 21, 21, 22,
		22, 23, 23, 23, 24, 24, 24, 25,
		25, 25, 26, 26, 26, 27, 27, 27,
		28, 28, 28, 28, 29, 29, 29, 29,
		30, 30, 30, 30, 31, 31, 31, 31,
		32, 32, 32, 32, 33, 33, 33, 33,
		34, 34, 34, 35, 35, 35, 35, 36,
		36, 36, 37, 37, 37, 37, 38, 38,
		38, 39, 39, 39, 39, 40, 40, 40,
		41, 41, 41, 41, 42, 42, 42, 43,
		43, 43, 43, 44, 44, 44, 45, 45,
		45, 45, 46, 46, 46, 46, 47, 47,
		47, 47, 48, 48, 48, 48, 49, 49,
		49, 49, 50, 50, 50, 50, 50, 51,
		51, 51, 51, 52, 52, 52, 52, 52,
		53, 53, 53, 53, 53, 53, 54, 54,
		54, 54, 54, 55, 55, 55, 55, 55,
		55, 56, 56, 56, 56, 56, 56, 57,
		57, 57, 57, 57, 57, 57, 58, 58,
		58, 58, 58, 58, 59, 59, 59, 59,
		59, 59, 59, 60, 60, 60, 60, 60,
		60, 60, 61, 61, 61, 61, 61, 61,
		61, 62, 62, 62, 62, 62, 62, 62,
		63, 63, 63, 63, 63, 63, 63, 64,
		64, 64, 64, 64, 64, 64, 65, 65,
		65, 65, 65, 65, 66, 66, 66, 66,
		66, 66, 66, 67, 67, 67, 67, 67,
		67, 68, 68, 68, 68, 68, 68, 69,
		69, 69, 69, 69, 69, 70, 70, 70,
		70, 70, 70, 71, 71, 71, 71, 71,
		71, 72, 72, 72, 72, 72, 72, 73,
		73, 73, 73, 73, 73, 74, 74, 74,
		74, 74, 74, 74, 75, 75, 75, 75,
		75, 75, 76, 76, 76, 76, 76, 76,
		77, 77, 77, 77, 77, 77, 77, 78,
		78, 78, 78, 78, 78, 79, 79, 79,
		79, 79, 79, 79, 80, 80, 80, 80,
		80, 80, 80, 80, 81, 81, 81, 81,
		81, 81, 81, 82, 82, 82, 82, 82,
		82, 82, 82, 83, 83, 83, 83, 83,
		83, 83, 83, 83, 84, 84, 84, 84,
		84, 84, 84, 84, 85, 85, 85, 85,
		85, 85, 85, 85, 85, 85, 86, 86,
		86, 86, 86, 86, 86, 86, 86, 86,
		87, 87, 87, 87, 87, 87, 87, 87,
		87, 87, 88, 88, 88, 88, 88, 88,
		88, 88, 88, 88, 88, 88, 89, 89,
		89, 89, 89, 89, 89, 89, 89, 89,
		89, 89, 90, 90, 90, 90, 90, 90,
		90, 90, 90, 90, 90, 90, 91, 91,
		91, 91, 91, 91, 91, 91, 91, 91,
		91, 91, 91, 92, 92, 92, 92, 92,
		92, 92, 92, 92, 92, 92, 92, 92,
		93, 93, 93, 93, 93, 93, 93, 93,
		93, 93, 93, 93, 93, 93, 94, 94,
		94, 94, 94, 94, 94, 94, 94, 94,
		94, 94, 94, 94, 95, 95, 95, 95,
		95, 95, 95, 95, 95, 95, 95, 95,
		95, 96, 96, 96, 96, 96, 96, 96,
		96, 96, 96, 96, 96, 96, 97, 97,
		97, 97, 97, 97, 97, 97, 97, 97,
		98, 99, 99, 100, 101, 101, 102, 103,
		103, 104, 105, 105, 106, 107, 107, 108,
		109, 110, 110, 111, 112, 112, 113, 114,
		114, 115, 115, 116, 117, 117, 118, 119,
		119, 120, 120, 121, 121, 122, 123, 123,
		124, 124, 125, 125, 126, 126, 127, 128,
		128, 129, 129, 130, 130, 131, 131, 132,
		132, 133, 133, 134, 134, 135, 135, 136,
		136, 137, 138, 138, 139, 139, 140, 140,
		141, 141, 142, 142, 143, 143, 144, 144,
		144, 145, 145, 146, 146, 147, 147, 148,
		148, 149, 149, 150, 150, 151, 151, 152,
		152, 153, 153, 153, 154, 154, 155, 155,
		156, 156, 157, 157, 157, 158, 158, 159,
		159, 160, 160, 160, 161, 161, 162, 162,
		162, 163, 163, 164, 164, 164, 165, 165,
		165, 166, 166, 167, 167, 167, 168, 168,
		168, 169, 169, 169, 170, 170, 171, 171,
		171, 172, 172, 172, 173, 173, 173, 174,
		174, 174, 175, 175, 175, 176, 176, 176,
		177, 177, 177, 178, 178, 178, 179, 179,
		179, 180, 180, 180, 181, 181, 181, 182,
		182, 182, 183, 183, 183, 184, 184, 184,
		185, 185, 185, 185, 186, 186, 186, 187,
		187, 187, 188, 188, 188, 189, 189, 189,
		190, 190, 190, 191, 191, 191, 191, 192,
		192, 192, 193, 193, 193, 194, 194, 194,
		195, 195, 195, 195, 196, 196, 196, 197,
		197, 197, 198, 198, 198, 199, 199, 199,
		199, 200, 200, 200, 201, 201, 201, 202,
		202, 202, 203, 203, 203, 203, 204, 204,
		204, 205, 205, 205, 206, 206, 206, 207,
		207, 207, 208, 208, 208, 208, 209, 209,
		209, 210, 210, 210, 211, 211, 211, 212,
		212, 212, 213, 213, 213, 214, 214, 214,
		215, 215, 215, 215, 216, 216, 216, 217,
		217, 217, 218, 218, 218, 219, 219, 219,
		220, 220, 220, 220, 221, 221, 221, 222,
		222, 222, 222, 223, 223, 223, 224, 224,
		224, 224, 225, 225, 225, 226, 226, 226,
		226, 227, 227, 227, 227, 228, 228, 228,
		229, 229, 229, 229, 230, 230, 230, 230,
		230, 231, 231, 231, 231, 232, 232, 232,
		232, 233, 233, 233, 233, 234, 234, 234,
		234, 234, 235, 235, 235, 235, 236, 236,
		236, 236, 236, 237, 237, 237, 237, 238,
		238, 238, 238, 238, 239, 239, 239, 239,
		239, 240, 240, 240, 240, 240, 241, 241,
		241, 241, 241, 242, 242, 242, 242, 243,
		243, 243, 243, 243, 244, 244, 244, 244,
		244, 245, 245, 245, 245, 245, 246, 246,
		246, 246, 246, 247, 247, 247, 247, 248,
		248, 248, 248, 248, 249, 249, 249, 249,
		250, 250, 250, 250, 251, 251, 251, 251,
		251, 252, 252, 252, 253, 253, 253, 253,
		254, 254, 254, 254, 255, 255, 255, 255,
	},
};
#endif

static int dsi_p_wuxga_10_1_bl_notify(struct device *unused, int brightness)
{
	int cur_sd_brightness = atomic_read(&sd_brightness);

	/* SD brightness is a percentage */
	brightness = (brightness * cur_sd_brightness) / 255;

	/* Apply any backlight response curve */
	if (brightness > 255)
		pr_info("Error: Brightness > 255!\n");
	else
		brightness = dsi_p_wuxga_10_1_bl_output_measured[brightness];

	return brightness;
}

static int dsi_p_wuxga_10_1_check_fb(struct device *dev, struct fb_info *info)
{
	return info->device == &disp_device->dev;
}

static struct platform_pwm_backlight_data dsi_p_wuxga_10_1_bl_data = {
	.pwm_id		= 1,
	.max_brightness	= 255,
	.dft_brightness	= 100,
	.lth_brightness = 100,
	.pwm_period_ns	= 40000,
	.pwm_gpio = DSI_PANEL_BL_PWM,
	.notify		= dsi_p_wuxga_10_1_bl_notify,

	/* Only toggle backlight on fb blank notifications for disp1 */
	.check_fb	= dsi_p_wuxga_10_1_check_fb,
};

static struct platform_device __maybe_unused
		dsi_p_wuxga_10_1_bl_device = {
	.name	= "pwm-backlight",
	.id	= -1,
	.dev	= {
		.platform_data = &dsi_p_wuxga_10_1_bl_data,
	},
};

static struct platform_device __maybe_unused
			*dsi_p_wuxga_10_1_bl_devices[] __initdata = {
	&tegra_pwfm_device,
	&dsi_p_wuxga_10_1_bl_device,
};

static int  __init dsi_p_wuxga_10_1_register_bl_dev(void)
{
	int err = 0;

	err = platform_add_devices(dsi_p_wuxga_10_1_bl_devices,
				ARRAY_SIZE(dsi_p_wuxga_10_1_bl_devices));
	if (err) {
		pr_err("disp1 bl device registration failed");
		return err;
	}
	return err;
}

static void dsi_p_wuxga_10_1_set_disp_device(
	struct platform_device *display_device)
{
	disp_device = display_device;
}

static void dsi_p_wuxga_10_1_dc_out_init(struct tegra_dc_out *dc)
{
	dc->dsi = &dsi_p_wuxga_10_1_pdata;
	dc->parent_clk = "pll_d_out0";
	dc->modes = dsi_p_wuxga_10_1_modes;
	dc->n_modes = ARRAY_SIZE(dsi_p_wuxga_10_1_modes);
	dc->enable = dsi_p_wuxga_10_1_enable;
	dc->disable = dsi_p_wuxga_10_1_disable;
	dc->postsuspend	= dsi_p_wuxga_10_1_postsuspend,
	dc->width = 217;
	dc->height = 135;
	dc->flags = DC_CTRL_MODE;
	
}

static void dsi_p_wuxga_10_1_fb_data_init(struct tegra_fb_data *fb)
{
	fb->xres = dsi_p_wuxga_10_1_modes[0].h_active;
	fb->yres = dsi_p_wuxga_10_1_modes[0].v_active;
}

static void
dsi_p_wuxga_10_1_sd_settings_init(struct tegra_dc_sd_settings *settings)
{
	*settings = dsi_p_wuxga_10_1_sd_settings;
	settings->bl_device_name = "pwm-backlight";
}

static void dsi_p_wuxga_10_1_cmu_init(struct tegra_dc_platform_data *pdata)
{
	pdata->cmu = &dsi_p_wuxga_10_1_cmu;
}

#endif

struct tegra_panel_ops dsi_p_wuxga_10_1_ops = {
	.enable = dsi_p_wuxga_10_1_enable,
	.disable = dsi_p_wuxga_10_1_disable,
	.postsuspend = dsi_p_wuxga_10_1_postsuspend,
};

struct tegra_panel __initdata dsi_p_wuxga_10_1 = {
	.init_sd_settings = dsi_p_wuxga_10_1_sd_settings_init,
	.init_dc_out = dsi_p_wuxga_10_1_dc_out_init,
	.init_fb_data = dsi_p_wuxga_10_1_fb_data_init,
	.register_bl_dev = dsi_p_wuxga_10_1_register_bl_dev,
	.init_cmu_data = dsi_p_wuxga_10_1_cmu_init,
	.set_disp_device = dsi_p_wuxga_10_1_set_disp_device,
};
EXPORT_SYMBOL(dsi_p_wuxga_10_1);

