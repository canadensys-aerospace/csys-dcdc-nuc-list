#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "usb.h"

#define VID			0x04d8
#define PID			0xd006
#define XFER_SIZE	32
#define TIMEOUT		100

int get_config(struct usb_device *device, unsigned char *config)
{
	struct usb_dev_handle *handle;
	int ret;
    unsigned char temp_buf[65536];
	unsigned char out_buf[XFER_SIZE];
	unsigned char in_buf[XFER_SIZE];
	int i;

	handle = usb_open(device);
	if (!handle)
	{
		fprintf(stderr, "Error opening device\n");
		return -1;
	}

	if (usb_get_driver_np(handle, 0, (char*)temp_buf, sizeof(temp_buf)) == 0 && usb_detach_kernel_driver_np(handle, 0) != 0)
	{
		fprintf(stderr, "Error detaching kernel driver\n");
		usb_close(handle);
		return -1;
	}

	if (usb_set_configuration(handle, 1) != 0)
	{
		fprintf(stderr, "Error setting configuration 1\n");
		usb_close(handle);
		return -1;
	}

	usleep(1000);

	if (usb_claim_interface(handle, 0) != 0)
	{
		fprintf(stderr, "Error claiming interface\n");
		usb_close(handle);
		return -1;
	}

	if (usb_set_altinterface(handle, 0) != 0)
	{
		fprintf(stderr, "Error setting alternate configuration\n");
		usb_close(handle);
		return -1;
	}

	memset(out_buf, 0, sizeof(out_buf));
	out_buf[0] = 0xa1;
	out_buf[1] = 0x30;
	out_buf[2] = 0x30;
	out_buf[3] = 0x00;
	out_buf[4] = 0x10;
	ret = usb_interrupt_write(handle, USB_ENDPOINT_OUT + 1, (char*)out_buf, sizeof(out_buf), TIMEOUT);
	if (ret != sizeof(out_buf))
	{
		fprintf(stderr, "Error sending OUT packet: %s\n", strerror(errno));
		usb_close(handle);
		return -1;
	}

	memset(in_buf, 0, sizeof(in_buf));
	ret = usb_interrupt_read(handle, USB_ENDPOINT_IN + 1, (char*)in_buf, sizeof(in_buf), TIMEOUT);
	if (ret != sizeof(in_buf))
	{
		fprintf(stderr, "Error receiving IN packet: %s\n", strerror(errno));
		usb_close(handle);
		return -1;
	}
	if (in_buf[0] != 0xa2)
	{
		fprintf(stderr, "Invalid response header byte %d (0x%02x != 0x%02x)\n", 0, in_buf[0], 0xa2);
		usb_close(handle);
		return -1;
	}
	for (i = 2; i <= 4; i++)
	{
		if (in_buf[i] != out_buf[i])
		{
			fprintf(stderr, "Invalid response header byte %d (0x%02x != 0x%02x)\n", i, in_buf[i], out_buf[i]);
			usb_close(handle);
			return -1;
		}
	}

	config[0] = in_buf[9];
	config[1] = in_buf[11];

	usb_close(handle);
	return 0;
}

int main(int argc, char *argv[])
{
	struct usb_bus *bus;
	struct usb_device *device;
	unsigned char config[2];
	char startup_enabled;

	usb_init();
	usb_set_debug(0);
	usb_find_busses();
	usb_find_devices();

	for (bus = usb_get_busses(); bus != NULL; bus = bus->next)
	{
		for (device = bus->devices; device != NULL; device = device->next)
		{
			if (device->descriptor.idVendor == VID && device->descriptor.idProduct == PID)
			{
				if (get_config(device, config) == 0)
				{
					startup_enabled = (config[0] & 0x02) ? '1' : '0';
				}
				else
				{
					startup_enabled = '?';
				}
				printf("%d,%d,%c\n", bus->location, atoi(device->filename), startup_enabled);
			}
		}
	}

	return 0;
}

