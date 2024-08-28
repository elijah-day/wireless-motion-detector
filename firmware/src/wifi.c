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
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "string.h"
#include "wifi.h"

static EventGroupHandle_t event_group_handle;
static esp_event_handler_instance_t ip_event;
static esp_event_handler_instance_t wifi_event;
static wifi_config_t wifi_cfg;
static int wifi_retry_cnt;
static int wifi_retry_max;
static int wifi_status;
static const char *tag = "wifi.c";

void check_wifi_bits(void)
{
	/* Wait for the event bits. */
	EventBits_t event_bits = xEventGroupWaitBits
	(
		event_group_handle,
		EVENT_BIT_WIFI_SUCCESS | EVENT_BIT_WIFI_FAILURE,
		pdFALSE,
		pdFALSE,
		portMAX_DELAY
	);

	/* Check the event bits. */
	if(event_bits & EVENT_BIT_WIFI_SUCCESS)
	{
		ESP_LOGI(tag, "WiFi successfully connected!!");
		wifi_status = 1;
	}
	else if(event_bits & EVENT_BIT_WIFI_FAILURE)
	{
		ESP_LOGI(tag, "WiFi failed to connect!!");
		wifi_status = 0;
	}
	else
	{
		ESP_LOGI(tag, "WiFi failed to connect!!");
		wifi_status = 0;
	}
}

void init_wifi(void)
{
	/* Set the default values for the WiFi configuration. */

	strcpy((char *)wifi_cfg.sta.password, DEFAULT_WIFI_PASSWORD);
	strcpy((char *)wifi_cfg.sta.ssid, DEFAULT_WIFI_SSID);
	wifi_cfg.sta.pmf_cfg.capable = true;
	wifi_cfg.sta.pmf_cfg.required = false;
	wifi_cfg.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

	/* Set initial values for WiFi. */

	wifi_retry_cnt = 0;
	wifi_retry_max = WIFI_RETRY_MAX_DEFAULT;
	wifi_status = 0;

	/* Initialize the TCP/IP stack and create the default event loop. */

	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	/* Start the default WiFi initialization. */

	esp_netif_create_default_wifi_sta();

	/* Initialize ESP WiFi with the default configuration. */

	wifi_init_config_t wifi_init_cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_cfg));
}

void register_wifi_events(void)
{
	/* Create the WiFi event group handler. */
	event_group_handle = xEventGroupCreate();

	/* Register the event handler instances. */
	ESP_ERROR_CHECK
	(
		esp_event_handler_instance_register
		(
			WIFI_EVENT,
			ESP_EVENT_ANY_ID,
			&wifi_event_handler,
			NULL,
			&wifi_event
		)
	);

	ESP_ERROR_CHECK
	(
		esp_event_handler_instance_register
		(
			IP_EVENT,
			IP_EVENT_STA_GOT_IP,
			&wifi_event_handler,
			NULL,
			&ip_event
		)
	);
}

void start_wifi(void)
{
	/* Set the WiFi controller settings. */
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg));

	/* Start the WiFi driver. */
	ESP_ERROR_CHECK(esp_wifi_start());
}

void unregister_wifi_events(void)
{
	/* Unregister the event handler instances. */
	ESP_ERROR_CHECK
	(
		esp_event_handler_instance_unregister
		(
			WIFI_EVENT,
			ESP_EVENT_ANY_ID,
			wifi_event
		)
	);

	ESP_ERROR_CHECK
	(
		esp_event_handler_instance_unregister
		(
			IP_EVENT,
			IP_EVENT_STA_GOT_IP,
			ip_event
		)
	);

	/* Delete the event group handle. */
	vEventGroupDelete(event_group_handle);
}

void wifi_event_handler
(
	void *event_handler_arg,
	esp_event_base_t event_base,
	int32_t event_id,
	void *event_data
)
{
	if(event_base == IP_EVENT)
	{
		if(event_id == IP_EVENT_STA_GOT_IP)
		{
			ip_event_got_ip_t *ip_event_got_ip =
			(ip_event_got_ip_t *)event_data;

			ESP_LOGI
			(
				tag,
				"Obtained IP: " IPSTR,
				IP2STR(&ip_event_got_ip->ip_info.ip)
			);

			xEventGroupSetBits(event_group_handle, EVENT_BIT_WIFI_SUCCESS);
		}
	}

	else if(event_base == WIFI_EVENT)
	{
		if(event_id == WIFI_EVENT_STA_START)
		{
			esp_wifi_connect();
		}
		else if(event_id == WIFI_EVENT_STA_DISCONNECTED)
		{
			if(wifi_retry_cnt < wifi_retry_max)
			{
				esp_wifi_connect();
				wifi_retry_cnt += 1;
			}
			else
			{
				xEventGroupSetBits(event_group_handle, EVENT_BIT_WIFI_FAILURE);
			}
		}
	}
}
