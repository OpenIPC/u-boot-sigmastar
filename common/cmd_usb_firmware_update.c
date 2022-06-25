/*
 * Copyright (C) 2011 Samsung Electronics
 * Lukasz Majewski <l.majewski@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <errno.h>
#include <common.h>
#include <command.h>
#include <g_dnl.h>
#include <part.h>
#include <usb.h>
#include <usb_mass_storage.h>

int do_ufu(cmd_tbl_t *cmdtp, int flag,
			       int argc, char * const argv[])
{
	int rc;
	int cable_ready_timeout __maybe_unused;

	fsg_init(NULL);
	rc = g_sstar_dnl_register("usb_dnl_ufu");
	if (rc) {
		error("g_sstar_dnl_register failed");
		return CMD_RET_FAILURE;
	}

	/* Timeout unit: seconds */
	cable_ready_timeout = UMS_CABLE_READY_TIMEOUT;

	if (!g_sstar_dnl_board_usb_cable_connected()) {
		/*
		 * Won't execute if we don't know whether the cable is
		 * connected.
		 */
		puts("Please connect USB cable.\n");

		while (!g_sstar_dnl_board_usb_cable_connected()) {
			if (ctrlc()) {
				puts("\rCTRL+C - Operation aborted.\n");
				goto exit;
			}
			if (!cable_ready_timeout) {
				puts("\rUSB cable not detected.\n" \
				     "Command exit.\n");
				goto exit;
			}

			printf("\rAuto exit in: %.2d s.", cable_ready_timeout);
			mdelay(1000);
			cable_ready_timeout--;
		}
		puts("\r\n");
	}

	while (1) {
		usb_gadget_handle_interrupts();

		rc = fsg_main_thread(NULL);
		if (rc) {
			printf("fsg_main_thread err: %d\n", rc);
			/* Check I/O error */
			if (rc == -EIO)
				printf("\rCheck USB cable connection\n");

			/* Check CTRL+C */
			if (rc == -EPIPE)
				printf("\rCTRL+C - Operation aborted\n");

			/* Check invalud command */
			if (rc == -EINVAL) {
				printf("\rInvalid command\n");
			}

			goto exit;
		}
	}
exit:
	g_sstar_dnl_unregister();
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(ufu, 1, 1, do_ufu,
	"Use the UFU [USB Firmware Update]",
	"ufu\n"
);
