/*
 * Copyright © 2024 Elijah Day
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "motion_detection.h"
#include "net.h"
#include "nvs_flash.h"
#include "wifi.h"

static const char *tag = "main.c";

void app_main(void)
{
	/* Initialize NVS. */

	esp_err_t esp_err;
	esp_err = nvs_flash_init();

	if
	(
		esp_err == ESP_ERR_NVS_NO_FREE_PAGES ||
		esp_err == ESP_ERR_NVS_NEW_VERSION_FOUND
	)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		esp_err = nvs_flash_init();
	}

	ESP_ERROR_CHECK(esp_err);

	/* Initialize motion detection. */

	init_md();

	/* Initialize and start WiFi. */

	init_wifi();
	register_wifi_events();
	start_wifi();
	check_wifi_bits();
	unregister_wifi_events();

	/* Start the device server. */

	init_net();

	/* Begin the main loop. */

	while(1)
	{
		if(watch_md())
		{
			send_net_md_msg();
		}

		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}
