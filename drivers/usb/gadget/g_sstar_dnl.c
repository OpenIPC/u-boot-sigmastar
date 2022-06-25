/*
 * g_dnl.c -- USB Downloader Gadget
 *
 * Copyright (C) 2012 Samsung Electronics
 * Lukasz Majewski  <l.majewski@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <malloc.h>

#include <mmc.h>
#include <part.h>

#include <g_sstar_dnl.h>
#include <usb_mass_storage.h>
#include <dfu.h>
#include <thor.h>

#include "gadget_chips.h"
#include <linux/usb/composite.h>

/*
 * One needs to define the following:
 * CONFIG_G_DNL_VENDOR_NUM
 * CONFIG_G_DNL_PRODUCT_NUM
 * at e.g. ./include/configs/<board>.h
 */

/* Number of supported configurations */
#define CONFIGURATION_NUMBER    1
#define CONFIG_VENDOR_NUM       0x1B20
#define CONFIG_PRODUCT_NUM      0x0300

static struct usb_device_descriptor device_desc = {
	.bLength = sizeof device_desc,
	.bDescriptorType = USB_DT_DEVICE,

	.bcdUSB = __constant_cpu_to_le16(0x0200),
	.bDeviceClass = USB_CLASS_PER_INTERFACE,
	.bDeviceSubClass = 0x00, /*0x02:CDC-modem , 0x00:CDC-serial*/

	.idVendor = __constant_cpu_to_le16(CONFIG_VENDOR_NUM),
	.idProduct = __constant_cpu_to_le16(CONFIG_PRODUCT_NUM),
	.iProduct = 0,
	.iSerialNumber = 0,
	.bNumConfigurations = 1,
};

static int g_dnl_unbind(struct usb_composite_dev *cdev)
{
	struct usb_gadget *gadget = cdev->gadget;

	free(cdev->config);
	cdev->config = NULL;
	debug("%s: calling usb_gadget_disconnect for "
			"controller '%s'\n", __func__, gadget->name);
	usb_gadget_disconnect(gadget);

	return 0;
}

static inline struct g_dnl_bind_callback *g_dnl_bind_callback_first(void)
{
	return ll_entry_start(struct g_dnl_bind_callback,
				g_dnl_bind_callbacks);
}

static inline struct g_dnl_bind_callback *g_dnl_bind_callback_end(void)
{
	return ll_entry_end(struct g_dnl_bind_callback,
				g_dnl_bind_callbacks);
}

static int g_dnl_do_config(struct usb_configuration *c)
{
	const char *s = c->cdev->driver->name;
	struct g_dnl_bind_callback *callback = g_dnl_bind_callback_first();

	debug("%s: configuration: 0x%p composite dev: 0x%p\n",
	      __func__, c, c->cdev);

	for (; callback != g_dnl_bind_callback_end(); callback++)
		if (!strcmp(s, callback->usb_function_name))
			return callback->fptr(c);
	return -ENODEV;
}

static int g_dnl_config_register(struct usb_composite_dev *cdev)
{
	struct usb_configuration *config;
	const char *name = "usb_dnload";

	config = memalign(CONFIG_SYS_CACHELINE_SIZE, sizeof(*config));
	if (!config)
		return -ENOMEM;

	memset(config, 0, sizeof(*config));

	config->label = name;
	config->bmAttributes = USB_CONFIG_ATT_ONE | USB_CONFIG_ATT_SELFPOWER;
	config->bConfigurationValue = CONFIGURATION_NUMBER;
	config->iConfiguration = 0;
	config->bind = g_dnl_do_config;

	return usb_add_config(cdev, config);
}

__weak int g_sstar_dnl_board_usb_cable_connected(void)
{
	return -EOPNOTSUPP;
}

static bool g_dnl_detach_request;

bool g_sstar_dnl_detach(void)
{
	return g_dnl_detach_request;
}

void g_sstar_dnl_trigger_detach(void)
{
	g_dnl_detach_request = true;
}

void g_sstar_dnl_clear_detach(void)
{
	g_dnl_detach_request = false;
}

static int g_dnl_bind(struct usb_composite_dev *cdev)
{
	struct usb_gadget *gadget = cdev->gadget;
	int ret;

	debug("%s: gadget: 0x%p cdev: 0x%p\n", __func__, gadget, cdev);

	device_desc.iManufacturer = 0;
	device_desc.iProduct = 0;
	device_desc.iSerialNumber = 0;

	ret = g_dnl_config_register(cdev);
	if (ret)
		goto error;

	device_desc.bcdDevice = cpu_to_le16(0x100);

	debug("%s: calling usb_gadget_connect for "
			"controller '%s'\n", __func__, gadget->name);
	usb_gadget_connect(gadget);

	return 0;

 error:
	g_dnl_unbind(cdev);
	return -ENOMEM;
}

static struct usb_composite_driver g_dnl_driver = {
	.name = NULL,
	.dev = &device_desc,

	.bind = g_dnl_bind,
	.unbind = g_dnl_unbind,
};

/*
 * NOTICE:
 * Registering via USB function name won't be necessary after rewriting
 * g_dnl to support multiple USB functions.
 */
int g_sstar_dnl_register(const char *name)
{
	int ret;

	debug("%s: g_dnl_driver.name = %s\n", __func__, name);
	g_dnl_driver.name = name;

	ret = usb_composite_register(&g_dnl_driver);
	if (ret) {
		printf("%s: failed!, error: %d\n", __func__, ret);
		return ret;
	}
	return 0;
}

void g_sstar_dnl_unregister(void)
{
	usb_composite_unregister(&g_dnl_driver);
}
