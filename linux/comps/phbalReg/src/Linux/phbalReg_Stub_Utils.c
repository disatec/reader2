/*
 * phbalReg_Stub_Utils.c
 *
 *  Created on: 26 Jan 2016
 *      Author: donatien
 */

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <poll.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "phbalReg_Stub_Utils.h"

#define CFG_GPIO_DIR "/sys/class/gpio"
#define CFG_GPIO_SYS_FILE_EXPORTED_TIME_IN_100MS 10

int gpio_is_exported(size_t gpio)
{
	int fd = 0;
	int i;
	char buf[64] = {0};
	size_t len = 0;

	len = snprintf(buf, sizeof(buf), CFG_GPIO_DIR "/gpio%lu/direction", gpio);
	fd = open(buf, O_WRONLY);
	if (fd >= 0) {
		close(fd);
		//OK, GPIO is exported
		return 0;
	}

	return -1;
}


int gpio_export(size_t gpio)
{
	int fd = 0;
	int i;
	char buf[64] = {0};
	size_t len = 0;

	if( gpio_is_exported(gpio) == 0 )
	{
		return 0; //No need to re-export
	}

	fd = open(CFG_GPIO_DIR "/export", O_WRONLY);
	if( fd < 0 )
	{
		return -1;
	}

	len = snprintf(buf, sizeof(buf), "%lu", gpio);
	write(fd, buf, len);
	close(fd);

	/* wait until file is actually available in user space */
	for (i = 0; i < CFG_GPIO_SYS_FILE_EXPORTED_TIME_IN_100MS; i++)
	{
		if( gpio_is_exported(gpio) == 0 )
		{
			return 0; //GPIO is present in user space
		}
		usleep(100 * 1000);
	}

	return -1;
}

int gpio_unexport(size_t gpio)
{
	int fd = 0;
	char buf[64] = {0};
	size_t len = 0;

	fd = open(CFG_GPIO_DIR "/unexport", O_WRONLY);
	if( fd < 0 )
	{
		return -1;
	}

	len = snprintf(buf, sizeof(buf), "%lu", gpio);
	write(fd, buf, len);
	close(fd);

	return 0;
}

int gpio_set_direction(size_t gpio, bool output)
{
	int fd = 0;
	char buf[64] = {0};
	size_t len = 0;

	len = snprintf(buf, sizeof(buf), CFG_GPIO_DIR "/gpio%lu/direction", gpio);

	fd = open(buf, O_WRONLY);
	if( fd < 0 )
	{
		return -1;
	}

	if(output)
	{
		write(fd, "out", 3);
	}
	else
	{
		write(fd, "in", 2);
	}

	close(fd);

	return 0;
}

int gpio_set_edge(size_t gpio, bool rising, bool falling)
{
	int fd = 0;
	char buf[64] = {0};
	size_t len = 0;

	len = snprintf(buf, sizeof(buf), CFG_GPIO_DIR "/gpio%lu/edge", gpio);

	fd = open(buf, O_WRONLY);
	if( fd < 0 )
	{
		return -1;
	}

	if(rising && falling)
	{
		write(fd, "both", 4);
	}
	else if(rising)
	{
		write(fd, "rising", 6);
	}
	else
	{
		write(fd, "falling", 7);
	}

	close(fd);

	return 0;
}

int gpio_open(size_t gpio, int mode)
{
	int fd = 0;
	char buf[64] = {0};
	size_t len = 0;

	len = snprintf(buf, sizeof(buf), CFG_GPIO_DIR "/gpio%lu/value", gpio);

	fd = open(buf, mode | O_NONBLOCK);
	if( fd < 0 )
	{
		return -1;
	}

	return fd;
}
