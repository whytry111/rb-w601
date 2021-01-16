/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-11     MurphyZhao   first implementation
 */

#include <rtthread.h>
//#include <stdio.h>
#include "iotb_sensor.h"
#include "iotb_event.h"
#include "iotb_workqeue.h"
#include "iotb_lcd_process.h"
#include "iotb_key_process.h"
#include "board.h"
#include "drv_lcd.h"

#define IOTB_MAIN_DEBUG

#define DBG_TAG "main"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

#if 1
int main(void)
{
	float tem_val, hum_val;
	char tem_ch[64];
	char hum_ch[64];

    iotb_lcd_show_startup_page();
	rt_thread_mdelay(1000);

	/* 初始化温湿度传感器 */
	iotb_sensor_aht10_init();
    rt_thread_mdelay(100);
	iotb_lcd_show_main_page();

	while(1) {
		iotb_sensor_aht10_read(0, &tem_val);
		iotb_sensor_aht10_read(1, &hum_val);

		sprintf(tem_ch, "temperature:%.2f", tem_val);
		sprintf(hum_ch, "humidity:%.2f", hum_val);

		lcd_show_string(25, 80, 24, tem_ch);
		lcd_show_string(25, 110, 24, hum_ch);

#if 0	/* test */	
		printf("temperature: %f, %s\n", tem_val, tem_ch);
		printf("humidity: %f, %s\n", hum_val, hum_ch);
#endif 
		
		rt_thread_mdelay(500);
	}

	

	

	/* 显示温湿度值，每500ms更新一次显示 */

	

}

#else	/* demo */
int main(void)
{
    /* 显示启动页 */
    iotb_lcd_show_startup_page();

    /* 在 SD 卡上挂载文件系统 */
    if (iotb_sensor_sdcard_fs_init() != RT_EOK)
    {
        LOG_E("Init sdcard fs failed!");
    }

    /* 初始化 WIFI */
    if (iotb_sensor_wifi_init() != RT_EOK)
    {
        if (iotb_sdcard_wifi_image_upgrade() != RT_EOK)
        {
            /* 使用 'ymodem start' 命令升级 WIFI 固件 */
            LOG_E("sdcard upgrad 'wifi image' failed!");
            LOG_E("Input 'ymodem_start' cmd to try to upgrade!");
            lcd_set_color(BLACK, WHITE);
            lcd_clear(BLACK);
            lcd_show_string(0, 120 - 26 - 26, 24,  "SDCard upgrade wifi");
            lcd_show_string(0, 120 - 26, 24,  " image failed!");
            lcd_show_string(0, 120, 24,  "Input 'ymodem_start'");
            lcd_show_string(0, 120 + 26, 24,  "cmd to upgrade");
            return 0;
        }
    }

    LOG_E("iotb_workqueue_start!");
    /* 启动工作队列，异步处理耗时任务 */
    if (iotb_workqueue_start() != RT_EOK)
    {
        return -RT_ERROR;
    }

    iotb_workqueue_dowork(iotb_init, RT_NULL);


    /* 启动 LCD 线程，用于接收处理 menu 事件 */
    iotb_lcd_start();
    /* 启动事件处理器 */
    iotb_event_start();

    /* 启动按键处理线程 */
    iotb_key_process_start();
    LOG_E("Init key process start ok!");
    return 0;
}
#endif



