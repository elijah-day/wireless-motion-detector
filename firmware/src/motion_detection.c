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

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "motion_detection.h"
#include "stdbool.h"

/*
 * NOTE: Normally md_sample_size and md_variance would be assigned to macros.
 * However, one potential goal is to make them adjustable in the future (through
 * UART, WiFi, or otherwise).
 */

static adc_oneshot_unit_handle_t unit_handle;
static int adc_value;
static int ir_curr;
static int ir_prev;
static int md_sample_size;
static int md_variance;
static const char *tag = "motion_detection.c";

/*
 * Function takes an IR reading and compares it to a previous IR reading
 * "snapshot".  If the currently read value exceeds the previous snapshot by
 * some variance, motion is considered to be detected and an alert is sent.
 */
bool watch_md(void)
{
	bool md_status = false;

	/*
	 * Check to see if the current IR value exceeds the previous IR value by
	 * md_variance (absolute value).  If so, send a motion detection alert.
	 */

	if(ir_curr > ir_prev + md_variance || ir_curr < ir_prev - md_variance)
	{
		ESP_LOGI(tag, "Motion detected!!");
		md_status = true;
		ir_prev = ir_curr;
	}

	/* Zero out ir_curr for reading. */

	ir_curr = 0;

	/* Take ADC readings of count md_sample_size.  Average them into ir_curr. */

	for(int i = 0; i < md_sample_size; i++)
	{
		adc_oneshot_read(unit_handle, ADC_CHANNEL_4, &adc_value);
		ir_curr += adc_value;
		adc_value = 0;
	}

	ir_curr /= md_sample_size;

	return md_status;
}

/* Function initializes everything needed for motion detection. */
void init_md(void)
{
	/* Configure GPIO. */

	gpio_config_t gpio_cfg;

	gpio_cfg.intr_type = GPIO_INTR_DISABLE;
	gpio_cfg.pull_down_en = 0;
	gpio_cfg.pull_up_en = 0;

	gpio_cfg.mode = GPIO_MODE_INPUT;
	gpio_cfg.pin_bit_mask = 1ULL << GPIO_NUM_4;
	gpio_config(&gpio_cfg);

	gpio_cfg.mode = GPIO_MODE_OUTPUT;
	gpio_cfg.pin_bit_mask = 1ULL << GPIO_NUM_6;
	gpio_config(&gpio_cfg);

	/* Register and configure a new unit for ADC1. */

	adc_oneshot_unit_init_cfg_t unit_init_cfg =
	{
		.ulp_mode = ADC_ULP_MODE_DISABLE,
		.unit_id = ADC_UNIT_1
	};

	ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_init_cfg, &unit_handle));

	/* Configure Channel 4 for the new unit. */

	adc_oneshot_chan_cfg_t chan_cfg =
	{
		.atten = ADC_ATTEN_DB_0,
		.bitwidth = ADC_BITWIDTH_DEFAULT
	};

	ESP_ERROR_CHECK
	(
		adc_oneshot_config_channel
		(
			unit_handle,
			ADC_CHANNEL_4,
			&chan_cfg
		)
	);

	/* Set initial values for motion detection. */

	adc_value = 0;
	ir_curr = 0;
	ir_prev = 0;
	md_sample_size = MD_SAMPLE_SIZE_DEFAULT;
	md_variance = MD_VARIANCE_DEFAULT;

	/* Turn the IR LED on. */
	gpio_set_level(GPIO_NUM_6, 1);
}
