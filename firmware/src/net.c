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

#include "esp_log.h"
#include "net.h"
#include "netinet/in.h"
#include "sys/socket.h"
#include "unistd.h"

static char buf[BUF_LEN]; /* Socket buffer */
static int sock_cl; /* Client socket */
static int sock_sv; /* Server socket */
static uint16_t port; /* Server port */
static struct sockaddr_in addr; /*  */
static const char *tag = "net.c";


void init_net(void)
{
	/* Create a generic return value integer. */
	int ret_val;
	socklen_t addr_len;

	/* Set the default port. */
	port = PORT_DEFAULT;

	/* Create the server. */
	sock_sv = socket(AF_INET, SOCK_STREAM, 0);

	if(sock_sv < 0)
	{
		ESP_LOGI(tag, "Unable to create the server socket!!");
		return;
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	addr_len = sizeof(addr);

	ret_val = bind(sock_sv, (struct sockaddr *)&addr, addr_len);

	if(ret_val < 0)
	{
		ESP_LOGI(tag, "Unable to bind the server socket!!");
		return;
	}

	listen(sock_sv, 2);

	sock_cl = accept
	(
		sock_sv,
		(struct sockaddr *)&addr,
		&addr_len
	);

	read(sock_cl, buf, BUF_LEN - 1);
	ESP_LOGI(tag, "%s\n", buf);
	send(sock_cl, "h\n", 3, 0);

	ESP_LOGI(tag, "CONTACT MADE!!\n");
}

void send_net_md_msg(void)
{
	/*
	 * Message to send when motion is detected.  (This is a temporary message
	 * for testing purposes.)
	 */
	send(sock_cl, "m\n", 3, 0);
}
