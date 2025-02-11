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

#ifndef WIFI_H
#define WIFI_H

#define DEFAULT_WIFI_SSID "wmd"
#define DEFAULT_WIFI_PASSWORD "wmd"
#define EVENT_BIT_WIFI_FAILURE BIT1
#define EVENT_BIT_WIFI_SUCCESS BIT0
#define WIFI_RETRY_MAX_DEFAULT 10

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "string.h"
#include "wifi.h"

void check_wifi_bits(void);
void init_wifi(void);
void register_wifi_events(void);
void start_wifi(void);
void unregister_wifi_events(void);

void wifi_event_handler
(
	void *event_handler_arg,
	esp_event_base_t event_base,
	int32_t event_id,
	void *event_data
);

#endif /* WIFI_H */
