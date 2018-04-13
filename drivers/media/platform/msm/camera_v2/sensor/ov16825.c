/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 and
* only version 2 as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
*/
#include "msm_sensor.h"
#define OV16825_SENSOR_NAME "ov16825"
DEFINE_MSM_MUTEX(ov16825_mut);

static struct msm_sensor_ctrl_t ov16825_s_ctrl;

static struct msm_sensor_power_setting ov16825_power_setting[] = {
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_VDIG,   //SUB DVDD GPIO POWER
		.config_val = GPIO_OUT_HIGH,
		.delay = 5,
	}, 
	{
		.seq_type = SENSOR_VREG,
		.seq_val = CAM_VANA,
		.config_val = 0,
		.delay = 1,    
	},
	{
		.seq_type = SENSOR_VREG,
		.seq_val = CAM_VIO,
		.config_val = 0,
		.delay = 5,
	},

	{
		.seq_type = SENSOR_VREG,
		.seq_val = CAM_VAF,
		.config_val = 0,
		.delay = 5,
	},
		{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_AF_PWDM,  
		.config_val = GPIO_OUT_HIGH,
		.delay = 5,
	},
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_RESET,
		.config_val = GPIO_OUT_LOW,
		.delay = 1,
	},
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_RESET,
		.config_val = GPIO_OUT_HIGH,
		.delay = 30,
	},
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_STANDBY,
		.config_val = GPIO_OUT_HIGH,
		.delay = 1,
	},
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_STANDBY,
		.config_val = GPIO_OUT_LOW,
		.delay = 30,
	},
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_RESET,
		.config_val = GPIO_OUT_HIGH,
		.delay = 20,    
	},
	{
		.seq_type = SENSOR_CLK,
		.seq_val = SENSOR_CAM_MCLK,
		.config_val = 0,
		.delay = 20,    
	},
	{
		.seq_type = SENSOR_I2C_MUX,
		.seq_val = 0,
		.config_val = 0,
		.delay = 0,
	},
};

static struct v4l2_subdev_info ov16825_subdev_info[] = {
	{
		.code  = V4L2_MBUS_FMT_SBGGR10_1X10,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.fmt  = 1,
		.order  = 0,
	},
};

static const struct i2c_device_id ov16825_i2c_id[] = {
	{OV16825_SENSOR_NAME, (kernel_ulong_t)&ov16825_s_ctrl},
	{ }
};


static int32_t msm_ov16825_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	return msm_sensor_i2c_probe(client, id, &ov16825_s_ctrl);
}
static struct i2c_driver ov16825_i2c_driver = {
	.id_table = ov16825_i2c_id,
	.probe = msm_ov16825_i2c_probe,
	.driver = {
		.name = OV16825_SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client ov16825_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

static const struct of_device_id ov16825_dt_match[] = {
	{.compatible = "qcom,ov16825", .data = &ov16825_s_ctrl},
	{}
};

MODULE_DEVICE_TABLE(of, ov16825_dt_match);

static struct platform_driver ov16825_platform_driver = {
	.driver = {
		.name = "qcom,ov16825",
		.owner = THIS_MODULE,
		.of_match_table = ov16825_dt_match,
	},
};

static int32_t ov16825_platform_probe(struct platform_device *pdev)
{
	int32_t rc = 0;
	const struct of_device_id *match;
	match = of_match_device(ov16825_dt_match, &pdev->dev);
	pr_info("%s:%d\n", __func__, __LINE__);
	rc = msm_sensor_platform_probe(pdev, match->data);
	return rc;
}

static int __init ov16825_init_module(void)
{
	int32_t rc = 0;
	pr_info("%s:%d\n", __func__, __LINE__);
	rc = platform_driver_probe(&ov16825_platform_driver,
	ov16825_platform_probe);
	if (!rc)
		return rc;
	return i2c_add_driver(&ov16825_i2c_driver);
}

static void __exit ov16825_exit_module(void)
{
	pr_info("%s:%d\n", __func__, __LINE__);
	if (ov16825_s_ctrl.pdev) {
		msm_sensor_free_sensor_data(&ov16825_s_ctrl);
		platform_driver_unregister(&ov16825_platform_driver);
	} else {
		i2c_del_driver(&ov16825_i2c_driver);
	}
}

static struct msm_sensor_ctrl_t ov16825_s_ctrl = {
	.sensor_i2c_client = &ov16825_sensor_i2c_client,
	.power_setting_array.power_setting = ov16825_power_setting,
	.power_setting_array.size = ARRAY_SIZE(ov16825_power_setting),
	.msm_sensor_mutex = &ov16825_mut,
	.sensor_v4l2_subdev_info = ov16825_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(ov16825_subdev_info),
};

module_init(ov16825_init_module);
module_exit(ov16825_exit_module);
MODULE_DESCRIPTION("ov16825");
MODULE_LICENSE("GPL v2");
