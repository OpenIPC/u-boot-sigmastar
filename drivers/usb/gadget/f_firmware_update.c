/*
 * f_mass_storage.c -- Mass Storage USB Composite Function
 *
 * Copyright (C) 2003-2008 Alan Stern
 * Copyright (C) 2009 Samsung Electronics
 *                    Author: Michal Nazarewicz <m.nazarewicz@samsung.com>
 * All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0+    BSD-3-Clause
 */

/*
 * The Mass Storage Function acts as a USB Mass Storage device,
 * appearing to the host as a disk drive or as a CD-ROM drive.  In
 * addition to providing an example of a genuinely useful composite
 * function for a USB device, it also illustrates a technique of
 * double-buffering for increased throughput.
 *
 * Function supports multiple logical units (LUNs).  Backing storage
 * for each LUN is provided by a regular file or a block device.
 * Access for each LUN can be limited to read-only.  Moreover, the
 * function can indicate that LUN is removable and/or CD-ROM.  (The
 * later implies read-only access.)
 *
 * MSF is configured by specifying a fsg_config structure.  It has the
 * following fields:
 *
 *  nluns       Number of LUNs function have (anywhere from 1
 *              to FSG_MAX_LUNS which is 8).
 *  luns        An array of LUN configuration values.  This
 *              should be filled for each LUN that
 *              function will include (ie. for "nluns"
 *              LUNs).  Each element of the array has
 *              the following fields:
 *  ->filename  The path to the backing file for the LUN.
 *              Required if LUN is not marked as
 *              removable.
 *  ->ro        Flag specifying access to the LUN shall be
 *              read-only.  This is implied if CD-ROM
 *              emulation is enabled as well as when
 *              it was impossible to open "filename"
 *              in R/W mode.
 *  ->removable Flag specifying that LUN shall be indicated as
 *              being removable.
 *  ->cdrom     Flag specifying that LUN shall be reported as
 *              being a CD-ROM.
 *
 *  lun_name_format A printf-like format for names of the LUN
 *              devices.  This determines how the
 *              directory in sysfs will be named.
 *              Unless you are using several MSFs in
 *              a single gadget (as opposed to single
 *              MSF in many configurations) you may
 *              leave it as NULL (in which case
 *              "lun%d" will be used).  In the format
 *              you can use "%d" to index LUNs for
 *              MSF's with more than one LUN.  (Beware
 *              that there is only one integer given
 *              as an argument for the format and
 *              specifying invalid format may cause
 *              unspecified behaviour.)
 *  thread_name Name of the kernel thread process used by the
 *              MSF.  You can safely set it to NULL
 *              (in which case default "file-storage"
 *              will be used).
 *
 *  vendor_name
 *  product_name
 *  release     Information used as a reply to INQUIRY
 *              request.  To use default set to NULL,
 *              NULL, 0xffff respectively.  The first
 *              field should be 8 and the second 16
 *              characters or less.
 *
 *  can_stall   Set to permit function to halt bulk endpoints.
 *              Disabled on some USB devices known not
 *              to work correctly.  You should set it
 *              to true.
 *
 * If "removable" is not set for a LUN then a backing file must be
 * specified.  If it is set, then NULL filename means the LUN's medium
 * is not loaded (an empty string as "filename" in the fsg_config
 * structure causes error).  The CD-ROM emulation includes a single
 * data track and no audio tracks; hence there need be only one
 * backing file per LUN.  Note also that the CD-ROM block length is
 * set to 512 rather than the more common value 2048.
 *
 *
 * MSF includes support for module parameters.  If gadget using it
 * decides to use it, the following module parameters will be
 * available:
 *
 *  file=filename[,filename...]
 *          Names of the files or block devices used for
 *              backing storage.
 *  ro=b[,b...] Default false, boolean for read-only access.
 *  removable=b[,b...]
 *          Default true, boolean for removable media.
 *  cdrom=b[,b...]  Default false, boolean for whether to emulate
 *              a CD-ROM drive.
 *  luns=N      Default N = number of filenames, number of
 *              LUNs to support.
 *  stall       Default determined according to the type of
 *              USB device controller (usually true),
 *              boolean to permit the driver to halt
 *              bulk endpoints.
 *
 * The module parameters may be prefixed with some string.  You need
 * to consult gadget's documentation or source to verify whether it is
 * using those module parameters and if it does what are the prefixes
 * (look for FSG_MODULE_PARAMETERS() macro usage, what's inside it is
 * the prefix).
 *
 *
 * Requirements are modest; only a bulk-in and a bulk-out endpoint are
 * needed.  The memory requirement amounts to two 16K buffers, size
 * configurable by a parameter.  Support is included for both
 * full-speed and high-speed operation.
 *
 * Note that the driver is slightly non-portable in that it assumes a
 * single memory/DMA buffer will be useable for bulk-in, bulk-out, and
 * interrupt-in endpoints.  With most device controllers this isn't an
 * issue, but there may be some with hardware restrictions that prevent
 * a buffer from being used by more than one endpoint.
 *
 *
 * The pathnames of the backing files and the ro settings are
 * available in the attribute files "file" and "ro" in the lun<n> (or
 * to be more precise in a directory which name comes from
 * "lun_name_format" option!) subdirectory of the gadget's sysfs
 * directory.  If the "removable" option is set, writing to these
 * files will simulate ejecting/loading the medium (writing an empty
 * line means eject) and adjusting a write-enable tab.  Changes to the
 * ro setting are not allowed when the medium is loaded or if CD-ROM
 * emulation is being used.
 *
 * When a LUN receive an "eject" SCSI request (Start/Stop Unit),
 * if the LUN is removable, the backing file is released to simulate
 * ejection.
 *
 *
 * This function is heavily based on "File-backed Storage Gadget" by
 * Alan Stern which in turn is heavily based on "Gadget Zero" by David
 * Brownell.  The driver's SCSI command interface was based on the
 * "Information technology - Small Computer System Interface - 2"
 * document from X3T9.2 Project 375D, Revision 10L, 7-SEP-93,
 * available at <http://www.t10.org/ftp/t10/drafts/s2/s2-r10l.pdf>.
 * The single exception is opcode 0x23 (READ FORMAT CAPACITIES), which
 * was based on the "Universal Serial Bus Mass Storage Class UFI
 * Command Specification" document, Revision 1.0, December 14, 1998,
 * available at
 * <http://www.usb.org/developers/devclass_docs/usbmass-ufi10.pdf>.
 */

/*
 *              Driver Design
 *
 * The MSF is fairly straightforward.  There is a main kernel
 * thread that handles most of the work.  Interrupt routines field
 * callbacks from the controller driver: bulk- and interrupt-request
 * completion notifications, endpoint-0 events, and disconnect events.
 * Completion events are passed to the main thread by wakeup calls.  Many
 * ep0 requests are handled at interrupt time, but SetInterface,
 * SetConfiguration, and device reset requests are forwarded to the
 * thread in the form of "exceptions" using SIGUSR1 signals (since they
 * should interrupt any ongoing file I/O operations).
 *
 * The thread's main routine implements the standard command/data/status
 * parts of a SCSI interaction.  It and its subroutines are full of tests
 * for pending signals/exceptions -- all this polling is necessary since
 * the kernel has no setjmp/longjmp equivalents.  (Maybe this is an
 * indication that the driver really wants to be running in userspace.)
 * An important point is that so long as the thread is alive it keeps an
 * open reference to the backing file.  This will prevent unmounting
 * the backing file's underlying filesystem and could cause problems
 * during system shutdown, for example.  To prevent such problems, the
 * thread catches INT, TERM, and KILL signals and converts them into
 * an EXIT exception.
 *
 * In normal operation the main thread is started during the gadget's
 * fsg_bind() callback and stopped during fsg_unbind().  But it can
 * also exit when it receives a signal, and there's no point leaving
 * the gadget running when the thread is dead.  At of this moment, MSF
 * provides no way to deregister the gadget when thread dies -- maybe
 * a callback functions is needed.
 *
 * To provide maximum throughput, the driver uses a circular pipeline of
 * buffer heads (struct fsg_buffhd).  In principle the pipeline can be
 * arbitrarily long; in practice the benefits don't justify having more
 * than 2 stages (i.e., double buffering).  But it helps to think of the
 * pipeline as being a long one.  Each buffer head contains a bulk-in and
 * a bulk-out request pointer (since the buffer can be used for both
 * output and input -- directions always are given from the host's
 * point of view) as well as a pointer to the buffer and various state
 * variables.
 *
 * Use of the pipeline follows a simple protocol.  There is a variable
 * (fsg->next_buffhd_to_fill) that points to the next buffer head to use.
 * At any time that buffer head may still be in use from an earlier
 * request, so each buffer head has a state variable indicating whether
 * it is EMPTY, FULL, or BUSY.  Typical use involves waiting for the
 * buffer head to be EMPTY, filling the buffer either by file I/O or by
 * USB I/O (during which the buffer head is BUSY), and marking the buffer
 * head FULL when the I/O is complete.  Then the buffer will be emptied
 * (again possibly by USB I/O, during which it is marked BUSY) and
 * finally marked EMPTY again (possibly by a completion routine).
 *
 * A module parameter tells the driver to avoid stalling the bulk
 * endpoints wherever the transport specification allows.  This is
 * necessary for some UDCs like the SuperH, which cannot reliably clear a
 * halt on a bulk endpoint.  However, under certain circumstances the
 * Bulk-only specification requires a stall.  In such cases the driver
 * will halt the endpoint and set a flag indicating that it should clear
 * the halt in software during the next device reset.  Hopefully this
 * will permit everything to work correctly.  Furthermore, although the
 * specification allows the bulk-out endpoint to halt when the host sends
 * too much data, implementing this would cause an unavoidable race.
 * The driver will always use the "no-stall" approach for OUT transfers.
 *
 * One subtle point concerns sending status-stage responses for ep0
 * requests.  Some of these requests, such as device reset, can involve
 * interrupting an ongoing file I/O operation, which might take an
 * arbitrarily long time.  During that delay the host might give up on
 * the original ep0 request and issue a new one.  When that happens the
 * driver should not notify the host about completion of the original
 * request, as the host will no longer be waiting for it.  So the driver
 * assigns to each ep0 request a unique tag, and it keeps track of the
 * tag value of the request associated with a long-running exception
 * (device-reset, interface-change, or configuration-change).  When the
 * exception handler is finished, the status-stage response is submitted
 * only if the current ep0 request tag is equal to the exception request
 * tag.  Thus only the most recently received ep0 request will get a
 * status-stage response.
 *
 * Warning: This driver source file is too long.  It ought to be split up
 * into a header file plus about 3 separate .c files, to handle the details
 * of the Gadget, USB Mass Storage, and SCSI protocols.
 */

/* #define VERBOSE_DEBUG */
/* #define DUMP_MSGS */

#include <config.h>
#include <malloc.h>
#include <common.h>
#include <u-boot/md5.h>
#include <g_sstar_dnl.h>

#include <linux/err.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
#include <usb_mass_storage.h>

#include <asm/unaligned.h>
#include <linux/usb/gadget.h>
#include <linux/usb/gadget.h>
#include <linux/usb/composite.h>
#include <usb/lin_gadget_compat.h>

/*------------------------------------------------------------------------*/

#define FSG_DRIVER_DESC "Firmware Update Function"
#define FSG_DRIVER_VERSION  "2020/05/04"

static const char fsg_string_interface[] = "Firmware Update";

#define FSG_NO_INTR_EP 1
#define FSG_NO_DEVICE_STRINGS    1
#define FSG_NO_OTG               1
#define FSG_NO_INTR_EP           1

#include "storage_common.c"

#define SC_VENDOR_SSTAR     0xE8
/* SCSI VENDOR SSTAR (0xE8) sub-code */
#define SSTAR_USB_DOWNLOAD_KEEP     0x01
#define SSTAR_USB_GET_RESULT        0x02
#define SSTAR_USB_DOWNLOAD_END      0x04
#define SSTAR_USB_LOAD_INFO         0x05
#define SSTAR_USB_RUN_CMD           0x06

/*-------------------------------------------------------------------------*/

#define GFP_ATOMIC ((gfp_t) 0)
#define PAGE_CACHE_SHIFT    12
#define PAGE_CACHE_SIZE     (1 << PAGE_CACHE_SHIFT)
#define kthread_create(...) __builtin_return_address(0)
#define wait_for_completion(...) do {} while (0)

struct kref {int x; };
struct completion {int x; };

inline void set_bit(int nr, volatile void *addr)
{
    int mask;
    unsigned int *a = (unsigned int *) addr;

    a += nr >> 5;
    mask = 1 << (nr & 0x1f);
    *a |= mask;
}

inline void clear_bit(int nr, volatile void *addr)
{
    int mask;
    unsigned int *a = (unsigned int *) addr;

    a += nr >> 5;
    mask = 1 << (nr & 0x1f);
    *a &= ~mask;
}

struct fsg_dev;
struct fsg_common;

/* Data shared by all the FSG instances. */
struct fsg_common {
    struct usb_gadget   *gadget;
    struct fsg_dev      *fsg, *new_fsg;

    struct usb_ep       *ep0;       /* Copy of gadget->ep0 */
    struct usb_request  *ep0req;    /* Copy of cdev->req */
    unsigned int        ep0_req_tag;

    struct fsg_buffhd   *next_buffhd_to_fill;
    struct fsg_buffhd   *next_buffhd_to_drain;
    struct fsg_buffhd   buffhds[FSG_NUM_BUFFERS];

    int         cmnd_size;
    u8          cmnd[MAX_COMMAND_SIZE];

    unsigned int        nluns;
    unsigned int        lun;
    struct fsg_lun          luns[FSG_MAX_LUNS];

    unsigned int        bulk_out_maxpacket;
    enum fsg_state      state;      /* For exception handling */
    unsigned int        exception_req_tag;

    enum data_direction data_dir;
    u32         data_size;
    u32         data_size_from_cmnd;
    u32         tag;
    u32         residue;
    u32         usb_amount_left;

    unsigned int        can_stall:1;
    unsigned int        free_storage_on_release:1;
    unsigned int        phase_error:1;
    unsigned int        short_packet_received:1;
    unsigned int        bad_lun_okay:1;
    unsigned int        running:1;

    int         thread_wakeup_needed;
    struct completion   thread_notifier;
    struct task_struct  *thread_task;

    /* Callback functions. */
    const struct fsg_operations *ops;
    /* Gadget's private data. */
    void            *private_data;

    const char *vendor_name;        /*  8 characters or less */
    const char *product_name;       /* 16 characters or less */
    u16 release;

    /* Vendor (8 chars), product (16 chars), release (4
     * hexadecimal digits) and NUL byte */
    char inquiry_string[8 + 16 + 4 + 1];

    struct kref     ref;
};

struct fsg_config {
    unsigned nluns;
    struct fsg_lun_config {
        const char *filename;
        char ro;
        char removable;
        char cdrom;
        char nofua;
    } luns[FSG_MAX_LUNS];

    /* Callback functions. */
    const struct fsg_operations     *ops;
    /* Gadget's private data. */
    void            *private_data;

    const char *vendor_name;        /*  8 characters or less */
    const char *product_name;       /* 16 characters or less */

    char            can_stall;
};

struct fsg_dev {
    struct usb_function function;
    struct usb_gadget   *gadget;    /* Copy of cdev->gadget */
    struct fsg_common   *common;

    u16         interface_number;

    unsigned int        bulk_in_enabled:1;
    unsigned int        bulk_out_enabled:1;

    unsigned long       atomic_bitflags;
#define IGNORE_BULK_OUT     0

    struct usb_ep       *bulk_in;
    struct usb_ep       *bulk_out;
};

enum ufu_errcode {
    UFU_SUCCESS = 0,
    UFU_ERR_MD5,
    UFU_ERR_INVALID_PARAM,
    UFU_ERR_RUNCMD,
    UFU_ERR_IMG_FMT,
    UFU_ERR_NOT_IMPLEMENT,
    UFU_ERR_MAX
};

struct ufu_loadinfo {
    u32                 addr;
    u32                 size;
    u8                  md5[16];
};

struct ufu {
    char                cmd[1024];
    struct ufu_loadinfo loadinfo;
    u8                  *cur;
    enum ufu_errcode    err;
};

static inline int __fsg_is_set(struct fsg_common *common,
                   const char *func, unsigned line)
{
    if (common->fsg)
        return 1;
    ERROR(common, "common->fsg is NULL in %s at %u\n", func, line);
    WARN_ON(1);
    return 0;
}

#define fsg_is_set(common) likely(__fsg_is_set(common, __func__, __LINE__))


static inline struct fsg_dev *fsg_from_func(struct usb_function *f)
{
    return container_of(f, struct fsg_dev, function);
}


typedef void (*fsg_routine_t)(struct fsg_dev *);

static int exception_in_progress(struct fsg_common *common)
{
    return common->state > FSG_STATE_IDLE;
}

/* Make bulk-out requests be divisible by the maxpacket size */
static void set_bulk_out_req_length(struct fsg_common *common,
        struct fsg_buffhd *bh, unsigned int length)
{
    //unsigned int    rem;

    bh->bulk_out_intended_length = length;
    //rem = length % common->bulk_out_maxpacket;
    //if (rem > 0)
    //    length += common->bulk_out_maxpacket - rem;
    bh->outreq->length = length;
}

/*-------------------------------------------------------------------------*/

struct ufu ufu;
struct ums *ums;
struct fsg_common *the_fsg_common;

static int fsg_set_halt(struct fsg_dev *fsg, struct usb_ep *ep)
{
    const char  *name;

    if (ep == fsg->bulk_in)
        name = "bulk-in";
    else if (ep == fsg->bulk_out)
        name = "bulk-out";
    else
        name = ep->name;
    DBG(fsg, "%s set halt\n", name);
    return usb_ep_set_halt(ep);
}

/*-------------------------------------------------------------------------*/

/* These routines may be called in process context or in_irq */

/* Caller must hold fsg->lock */
static void wakeup_thread(struct fsg_common *common)
{
    common->thread_wakeup_needed = 1;
}

static void raise_exception(struct fsg_common *common, enum fsg_state new_state)
{
    /* Do nothing if a higher-priority exception is already in progress.
     * If a lower-or-equal priority exception is in progress, preempt it
     * and notify the main thread by sending it a signal. */
    if (common->state <= new_state) {
        common->exception_req_tag = common->ep0_req_tag;
        common->state = new_state;
        common->thread_wakeup_needed = 1;
    }
}

/*-------------------------------------------------------------------------*/

static int ep0_queue(struct fsg_common *common)
{
    int rc;

    rc = usb_ep_queue(common->ep0, common->ep0req, GFP_ATOMIC);
    common->ep0->driver_data = common;
    if (rc != 0 && rc != -ESHUTDOWN) {
        /* We can't do much more than wait for a reset */
        WARNING(common, "error in submission: %s --> %d\n",
            common->ep0->name, rc);
    }
    return rc;
}

/*-------------------------------------------------------------------------*/

/* Bulk and interrupt endpoint completion handlers.
 * These always run in_irq. */

static void bulk_in_complete(struct usb_ep *ep, struct usb_request *req)
{
    struct fsg_common   *common = ep->driver_data;
    struct fsg_buffhd   *bh = req->context;

    if (req->status || req->actual != req->length)
        DBG(common, "%s --> %d, %u/%u\n", __func__,
                req->status, req->actual, req->length);
    if (req->status == -ECONNRESET)     /* Request was cancelled */
        usb_ep_fifo_flush(ep);

    /* Hold the lock while we update the request and buffer states */
    bh->inreq_busy = 0;
    bh->state = BUF_STATE_EMPTY;
    wakeup_thread(common);
}

static void bulk_out_complete(struct usb_ep *ep, struct usb_request *req)
{
    struct fsg_common   *common = ep->driver_data;
    struct fsg_buffhd   *bh = req->context;

    dump_msg(common, "bulk-out", req->buf, req->actual);
    if (req->status || req->actual != bh->bulk_out_intended_length)
        DBG(common, "%s --> %d, %u/%u\n", __func__,
                req->status, req->actual,
                bh->bulk_out_intended_length);
    if (req->status == -ECONNRESET)     /* Request was cancelled */
        usb_ep_fifo_flush(ep);

    /* Hold the lock while we update the request and buffer states */
    bh->outreq_busy = 0;
    bh->state = BUF_STATE_FULL;
    wakeup_thread(common);
}

/*-------------------------------------------------------------------------*/

/* Ep0 class-specific handlers.  These always run in_irq. */

static int fsg_setup(struct usb_function *f,
        const struct usb_ctrlrequest *ctrl)
{
    struct fsg_dev      *fsg = fsg_from_func(f);
    struct usb_request  *req = fsg->common->ep0req;
    u16         w_index = get_unaligned_le16(&ctrl->wIndex);
    u16         w_value = get_unaligned_le16(&ctrl->wValue);
    u16         w_length = get_unaligned_le16(&ctrl->wLength);

    if (!fsg_is_set(fsg->common))
        return -EOPNOTSUPP;

    switch (ctrl->bRequest) {

    case USB_BULK_RESET_REQUEST:
        if (ctrl->bRequestType !=
            (USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE))
            break;
        if (w_index != fsg->interface_number || w_value != 0)
            return -EDOM;

        /* Raise an exception to stop the current operation
         * and reinitialize our state. */
        DBG(fsg, "bulk reset request\n");
        raise_exception(fsg->common, FSG_STATE_RESET);
        return DELAYED_STATUS;

    case USB_BULK_GET_MAX_LUN_REQUEST:
        if (ctrl->bRequestType !=
            (USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE))
            break;
        if (w_index != fsg->interface_number || w_value != 0)
            return -EDOM;
        VDBG(fsg, "get max LUN\n");
        *(u8 *) req->buf = fsg->common->nluns - 1;

        /* Respond with data/status */
        req->length = min((u16)1, w_length);
        return ep0_queue(fsg->common);
    }

    VDBG(fsg,
         "unknown class-specific control req "
         "%02x.%02x v%04x i%04x l%u\n",
         ctrl->bRequestType, ctrl->bRequest,
         get_unaligned_le16(&ctrl->wValue), w_index, w_length);
    return -EOPNOTSUPP;
}

/*-------------------------------------------------------------------------*/

/* All the following routines run in process context */

/* Use this for bulk or interrupt transfers, not ep0 */
static void start_transfer(struct fsg_dev *fsg, struct usb_ep *ep,
        struct usb_request *req, int *pbusy,
        enum fsg_buffer_state *state)
{
    int rc;

    if (ep == fsg->bulk_in)
        dump_msg(fsg, "bulk-in", req->buf, req->length);

    *pbusy = 1;
    *state = BUF_STATE_BUSY;
    rc = usb_ep_queue(ep, req, GFP_KERNEL);
    if (rc != 0) {
        *pbusy = 0;
        *state = BUF_STATE_EMPTY;

        /* We can't do much more than wait for a reset */

        /* Note: currently the net2280 driver fails zero-length
         * submissions if DMA is enabled. */
        if (rc != -ESHUTDOWN && !(rc == -EOPNOTSUPP &&
                        req->length == 0))
            WARNING(fsg, "error in submission: %s --> %d\n",
                    ep->name, rc);
    }
}

#define START_TRANSFER_OR(common, ep_name, req, pbusy, state)       \
    if (fsg_is_set(common))                     \
        start_transfer((common)->fsg, (common)->fsg->ep_name,   \
                   req, pbusy, state);          \
    else

#define START_TRANSFER(common, ep_name, req, pbusy, state)      \
    START_TRANSFER_OR(common, ep_name, req, pbusy, state) (void)0

/*
static void busy_indicator(void)
{
    static int state;

    switch (state) {
    case 0:
        puts("\r|"); break;
    case 1:
        puts("\r/"); break;
    case 2:
        puts("\r-"); break;
    case 3:
        puts("\r\\"); break;
    case 4:
        puts("\r|"); break;
    case 5:
        puts("\r/"); break;
    case 6:
        puts("\r-"); break;
    case 7:
        puts("\r\\"); break;
    default:
        state = 0;
    }
    if (state++ == 8)
        state = 0;
}
*/

static int sleep_thread(struct fsg_common *common)
{
    int rc = 0;
    int i = 0, k = 0;

    /* Wait until a signal arrives or we are woken up */
    for (;;) {
        if (common->thread_wakeup_needed)
            break;

        if (++i == 50000) {
            //busy_indicator();
            i = 0;
            k++;
        }

        if (k == 10) {
            /* Handle CTRL+C */
            if (ctrlc())
                return -EPIPE;

            /* Check cable connection */
            if (!g_sstar_dnl_board_usb_cable_connected())
                return -EIO;

            k = 0;
        }

        usb_gadget_handle_interrupts();
    }
    common->thread_wakeup_needed = 0;
    return rc;
}

/*-------------------------------------------------------------------------*/

static int do_inquiry(struct fsg_common *common, struct fsg_buffhd *bh)
{
    struct fsg_lun *curlun = &common->luns[common->lun];
    u8  *buf = (u8 *) bh->buf;

    if (!curlun) {      /* Unsupported LUNs are okay */
        common->bad_lun_okay = 1;
        memset(buf, 0, 36);
        buf[0] = 0x7f;      /* Unsupported, no device-type */
        buf[4] = 31;        /* Additional length */
        return 36;
    }

    memset(buf, 0, 36);
    buf[0] = TYPE_DISK;
    buf[1] = 0x80;  /* removable */
    buf[2] = 0;     /* ANSI SCSI level 2 */
    buf[3] = 0;     /* SCSI-2 INQUIRY data format */
    buf[4] = 31;    /* Additional length */
                    /* No special options */
    buf[8] = 'G';
    buf[9] = 'C';
    buf[10]= 'R';
    buf[11]= 'E';
    buf[12]= 'A';
    buf[13]= 'D';
    buf[14]= 'E';
    buf[15]= 'R';

    buf[16]= 'U';
    buf[17]= 'B';
    buf[18]= 'O';
    buf[19]= 'O';
    buf[20]= 'T';
    return 36;
}


static int do_request_sense(struct fsg_common *common, struct fsg_buffhd *bh)
{
    struct fsg_lun  *curlun = &common->luns[common->lun];
    u8      *buf = (u8 *) bh->buf;
    u32     sd, sdinfo;

    /*
     * From the SCSI-2 spec., section 7.9 (Unit attention condition):
     *
     * If a REQUEST SENSE command is received from an initiator
     * with a pending unit attention condition (before the target
     * generates the contingent allegiance condition), then the
     * target shall either:
     *   a) report any pending sense data and preserve the unit
     *  attention condition on the logical unit, or,
     *   b) report the unit attention condition, may discard any
     *  pending sense data, and clear the unit attention
     *  condition on the logical unit for that initiator.
     *
     * FSG normally uses option a); enable this code to use option b).
     */
#if 0
    if (curlun && curlun->unit_attention_data != SS_NO_SENSE) {
        curlun->sense_data = curlun->unit_attention_data;
        curlun->unit_attention_data = SS_NO_SENSE;
    }
#endif

    if (!curlun) {      /* Unsupported LUNs are okay */
        common->bad_lun_okay = 1;
        sd = SS_LOGICAL_UNIT_NOT_SUPPORTED;
        sdinfo = 0;
    } else {
        sd = curlun->sense_data;
        curlun->sense_data = SS_NO_SENSE;
        curlun->info_valid = 0;
    }

    memset(buf, 0, 18);
    buf[0] = 0x70;          /* Valid, current error */
    buf[2] = SK(sd);
    put_unaligned_be32(sdinfo, &buf[3]);    /* Sense information */
    buf[7] = 18 - 8;            /* Additional sense length */
    buf[12] = ASC(sd);
    buf[13] = ASCQ(sd);
    return 18;
}

static int do_read_capacity(struct fsg_common *common, struct fsg_buffhd *bh)
{
    u8      *buf = (u8 *) bh->buf;

    put_unaligned_be32(0, &buf[0]);
                        /* Max logical block */
    put_unaligned_be32(0, &buf[4]); /* Block length */
    return 8;
}

static int do_read_format_capacities(struct fsg_common *common,
            struct fsg_buffhd *bh)
{
    u8      *buf = (u8 *) bh->buf;

    put_unaligned_be32(0, &buf[0]);
                        /* Number of blocks */
    put_unaligned_be32(0, &buf[4]); /* Block length */
    buf[4] = 0x02;              /* Current capacity */
    return 12;
}

static int verify_md5sum(void)
{
    u8 output[16];
    unsigned char *addr;
    int len, i;

    addr = (unsigned char *)ufu.loadinfo.addr;
    len = ufu.loadinfo.size;
    md5_wd(addr, len, output, CHUNKSZ_MD5);
    if (memcmp(output, ufu.loadinfo.md5, 16) != 0) {
        printf("md5 for %p ... %p ==> ", addr,
            addr + len - 1);
        for (i = 0; i < 16; i++)
            printf("%02x", output[i]);
        printf(" != ");
        for (i = 0; i < 16; i++)
            printf("%02x", ufu.loadinfo.md5[i]);
        printf(" ** ERROR **\n");
        return -EINVAL;
    }
    printf("md5sum pass\n");
    return 0;
}

static int do_vendor_sstar_bulk_out(struct fsg_common *common, void *out)
{
	struct fsg_buffhd	*bh;
	int			get_some_more;
	u32			amount_left_to_req, amount_left_to_write;
	unsigned int    amount;
	int			rc;
	u8          *buf = out;

	get_some_more = 1;
	amount_left_to_req = common->data_size_from_cmnd;
	amount_left_to_write = common->data_size_from_cmnd;

	while (amount_left_to_write > 0) {

		/* Queue a request for more data from the host */
		bh = common->next_buffhd_to_fill;
		if (bh->state == BUF_STATE_EMPTY && get_some_more) {

			amount = min(amount_left_to_req, FSG_BUFLEN);
			if (amount == 0) {
				get_some_more = 0;
				continue;
			}

			/* Get the next buffer */
			common->usb_amount_left -= amount;
			amount_left_to_req -= amount;
			if (amount_left_to_req == 0)
				get_some_more = 0;

			/* amount is always divisible by 512, hence by
			 * the bulk-out maxpacket size */
			bh->outreq->length = amount;
			bh->bulk_out_intended_length = amount;
			bh->outreq->short_not_ok = 1;
			START_TRANSFER_OR(common, bulk_out, bh->outreq,
					  &bh->outreq_busy, &bh->state)
				/* Don't know what to do if
				 * common->fsg is NULL */
				return -EIO;
			common->next_buffhd_to_fill = bh->next;
			continue;
		}

		/* Write the received data to the backing file */
		bh = common->next_buffhd_to_drain;
		if (bh->state == BUF_STATE_EMPTY && !get_some_more)
			break;			/* We stopped early */
		if (bh->state == BUF_STATE_FULL) {
			common->next_buffhd_to_drain = bh->next;
			bh->state = BUF_STATE_EMPTY;

			/* Did something go wrong with the transfer? */
			if (bh->outreq->status != 0) {
				printf("xfer failed\n");
				break;
			}

			amount = bh->outreq->actual;

			/* Perform the write */
			memcpy(buf, bh->buf, amount);
			//debug("\namount %d\n", amount);
			//debug("%2x %2x %2x %2x %2x %2x %2x %2x\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
		    //debug("%2x %2x %2x %2x %2x %2x %2x %2x\n", buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15]);
			amount_left_to_write -= amount;
			common->residue -= amount;

			/* Did the host decide to stop early? */
			if (bh->outreq->actual != bh->outreq->length) {
				common->short_packet_received = 1;
				break;
			}
			continue;
		}

		/* Wait for something to happen */
		rc = sleep_thread(common);
		if (rc)
			return rc;
	}

	return -EIO;		/* No default reply */
}

static int do_vendor_sstar_get_result(struct fsg_common *common, struct fsg_buffhd *bh)
{
    u8  *buf = (u8 *)bh->buf;

    buf[0] = 0x0D;
    buf[1] = ufu.err;
    buf[2] = 0x00;
    buf[3] = 0x00;

    return 4;
}

static int do_vendor_sstar_keep_dnl(struct fsg_common *common)
{
    int ret;

    ret = do_vendor_sstar_bulk_out(common, ufu.cur);
    ufu.cur += common->data_size_from_cmnd;

    return ret;
}

static int do_vendor_sstar_end_dnl(struct fsg_common *common)
{
    int ret;
    int dcache = dcache_status();

    ret = do_vendor_sstar_bulk_out(common, ufu.cur);
    ufu.cur += common->data_size_from_cmnd;
    /* enable d-cache for speedup */
    if (dcache == 0) {
        dcache_enable();
    }
    /* verify md5 checksum */
	ret = verify_md5sum();
	/* restore d-cache to disable */
    if (dcache == 0) {
        dcache_disable();
    }
	if (ret != 0) {
	    ret = -EINVAL;
	    ufu.err = UFU_ERR_MD5;
	}
	else {
	    ufu.err = UFU_SUCCESS;
	}
    return ret;
}

static int do_vendor_sstar_loadinfo(struct fsg_common *common)
{
    int ret;

    ret = do_vendor_sstar_bulk_out(common, &ufu.loadinfo);
    ufu.cur = (u8 *)ufu.loadinfo.addr;
    setenv_hex("filesize", ufu.loadinfo.size);
    printf("[UFU loadinfo] addr x%x, size %d\n", ufu.loadinfo.addr, ufu.loadinfo.size);
    return ret;
}

static int do_vendor_sstar_runcmd(struct fsg_common *common)
{
    int ret, result;

    if ((common->data_size_from_cmnd+1) >= sizeof(ufu.cmd)) {
        printf("UFU cmd is too long to support\n");
        ufu.err = UFU_ERR_INVALID_PARAM;
        return -EINVAL;
    }
    ret = do_vendor_sstar_bulk_out(common, ufu.cmd);
    ufu.cmd[common->data_size_from_cmnd] = '\0';
    printf("[UFU runcmd] %s\n", ufu.cmd);
    // run command
    result = run_command(ufu.cmd, 0);
    if (result != 0)
        ufu.err = UFU_ERR_RUNCMD;
    else
        ufu.err = UFU_SUCCESS;
    return ret;
}

static int do_vendor_sstar(struct fsg_common *common, struct fsg_buffhd *bh)
{
    int     reply = 0;

    //debug("cmd %x sub %x data_size %d size_from_cmnd %d\n", common->cmnd[0], common->cmnd[1],
    //                                                        common->data_size, common->data_size_from_cmnd);
    common->residue = common->data_size;
    common->usb_amount_left = common->data_size;

    switch(common->cmnd[1]) {
    case SSTAR_USB_DOWNLOAD_KEEP:
        reply = do_vendor_sstar_keep_dnl(common);
        break;
    case SSTAR_USB_GET_RESULT:
        reply = do_vendor_sstar_get_result(common, bh);
        break;
    case SSTAR_USB_DOWNLOAD_END:
        reply = do_vendor_sstar_end_dnl(common);
        break;
    case SSTAR_USB_LOAD_INFO:
        reply = do_vendor_sstar_loadinfo(common);
        break;
    case SSTAR_USB_RUN_CMD:
        reply = do_vendor_sstar_runcmd(common);
        break;
    default:
        reply = -EINVAL;
        break;
    }
    return reply;
}

/*-------------------------------------------------------------------------*/

static int halt_bulk_in_endpoint(struct fsg_dev *fsg)
{
    int rc;

    rc = fsg_set_halt(fsg, fsg->bulk_in);
    if (rc == -EAGAIN)
        VDBG(fsg, "delayed bulk-in endpoint halt\n");
    while (rc != 0) {
        if (rc != -EAGAIN) {
            WARNING(fsg, "usb_ep_set_halt -> %d\n", rc);
            rc = 0;
            break;
        }

        rc = usb_ep_set_halt(fsg->bulk_in);
    }
    return rc;
}

static int wedge_bulk_in_endpoint(struct fsg_dev *fsg)
{
    int rc;

    DBG(fsg, "bulk-in set wedge\n");
    rc = 0; /* usb_ep_set_wedge(fsg->bulk_in); */
    if (rc == -EAGAIN)
        VDBG(fsg, "delayed bulk-in endpoint wedge\n");
    while (rc != 0) {
        if (rc != -EAGAIN) {
            WARNING(fsg, "usb_ep_set_wedge -> %d\n", rc);
            rc = 0;
            break;
        }
    }
    return rc;
}

static int pad_with_zeros(struct fsg_dev *fsg)
{
    struct fsg_buffhd   *bh = fsg->common->next_buffhd_to_fill;
    u32         nkeep = bh->inreq->length;
    u32         nsend;
    int         rc;

    bh->state = BUF_STATE_EMPTY;        /* For the first iteration */
    fsg->common->usb_amount_left = nkeep + fsg->common->residue;
    while (fsg->common->usb_amount_left > 0) {

        /* Wait for the next buffer to be free */
        while (bh->state != BUF_STATE_EMPTY) {
            rc = sleep_thread(fsg->common);
            if (rc)
                return rc;
        }

        nsend = min(fsg->common->usb_amount_left, FSG_BUFLEN);
        memset(bh->buf + nkeep, 0, nsend - nkeep);
        bh->inreq->length = nsend;
        bh->inreq->zero = 0;
        start_transfer(fsg, fsg->bulk_in, bh->inreq,
                &bh->inreq_busy, &bh->state);
        bh = fsg->common->next_buffhd_to_fill = bh->next;
        fsg->common->usb_amount_left -= nsend;
        nkeep = 0;
    }
    return 0;
}

static int throw_away_data(struct fsg_common *common)
{
    struct fsg_buffhd   *bh;
    u32         amount;
    int         rc;

    for (bh = common->next_buffhd_to_drain;
         bh->state != BUF_STATE_EMPTY || common->usb_amount_left > 0;
         bh = common->next_buffhd_to_drain) {

        /* Throw away the data in a filled buffer */
        if (bh->state == BUF_STATE_FULL) {
            bh->state = BUF_STATE_EMPTY;
            common->next_buffhd_to_drain = bh->next;

            /* A short packet or an error ends everything */
            if (bh->outreq->actual != bh->outreq->length ||
                    bh->outreq->status != 0) {
                raise_exception(common,
                        FSG_STATE_ABORT_BULK_OUT);
                return -EINTR;
            }
            continue;
        }

        /* Try to submit another request if we need one */
        bh = common->next_buffhd_to_fill;
        if (bh->state == BUF_STATE_EMPTY
         && common->usb_amount_left > 0) {
            amount = min(common->usb_amount_left, FSG_BUFLEN);

            /* amount is always divisible by 512, hence by
             * the bulk-out maxpacket size */
            bh->outreq->length = amount;
            bh->bulk_out_intended_length = amount;
            bh->outreq->short_not_ok = 1;
            START_TRANSFER_OR(common, bulk_out, bh->outreq,
                      &bh->outreq_busy, &bh->state)
                /* Don't know what to do if
                 * common->fsg is NULL */
                return -EIO;
            common->next_buffhd_to_fill = bh->next;
            common->usb_amount_left -= amount;
            continue;
        }

        /* Otherwise wait for something to happen */
        rc = sleep_thread(common);
        if (rc)
            return rc;
    }
    return 0;
}


static int finish_reply(struct fsg_common *common)
{
    struct fsg_buffhd   *bh = common->next_buffhd_to_fill;
    int         rc = 0;

    switch (common->data_dir) {
    case DATA_DIR_NONE:
        break;          /* Nothing to send */

    /* If we don't know whether the host wants to read or write,
     * this must be CB or CBI with an unknown command.  We mustn't
     * try to send or receive any data.  So stall both bulk pipes
     * if we can and wait for a reset. */
    case DATA_DIR_UNKNOWN:
        if (!common->can_stall) {
            /* Nothing */
        } else if (fsg_is_set(common)) {
            fsg_set_halt(common->fsg, common->fsg->bulk_out);
            rc = halt_bulk_in_endpoint(common->fsg);
        } else {
            /* Don't know what to do if common->fsg is NULL */
            rc = -EIO;
        }
        break;

    /* All but the last buffer of data must have already been sent */
    case DATA_DIR_TO_HOST:
        if (common->data_size == 0) {
            /* Nothing to send */

        /* If there's no residue, simply send the last buffer */
        } else if (common->residue == 0) {
            bh->inreq->zero = 0;
            START_TRANSFER_OR(common, bulk_in, bh->inreq,
                      &bh->inreq_busy, &bh->state)
                return -EIO;
            common->next_buffhd_to_fill = bh->next;

        /* For Bulk-only, if we're allowed to stall then send the
         * short packet and halt the bulk-in endpoint.  If we can't
         * stall, pad out the remaining data with 0's. */
        } else if (common->can_stall) {
            bh->inreq->zero = 1;
            START_TRANSFER_OR(common, bulk_in, bh->inreq,
                      &bh->inreq_busy, &bh->state)
                /* Don't know what to do if
                 * common->fsg is NULL */
                rc = -EIO;
            common->next_buffhd_to_fill = bh->next;
            if (common->fsg)
                rc = halt_bulk_in_endpoint(common->fsg);
        } else if (fsg_is_set(common)) {
            rc = pad_with_zeros(common->fsg);
        } else {
            /* Don't know what to do if common->fsg is NULL */
            rc = -EIO;
        }
        break;

    /* We have processed all we want from the data the host has sent.
     * There may still be outstanding bulk-out requests. */
    case DATA_DIR_FROM_HOST:
        if (common->residue == 0) {
            /* Nothing to receive */

        /* Did the host stop sending unexpectedly early? */
        } else if (common->short_packet_received) {
            raise_exception(common, FSG_STATE_ABORT_BULK_OUT);
            rc = -EINTR;

        /* We haven't processed all the incoming data.  Even though
         * we may be allowed to stall, doing so would cause a race.
         * The controller may already have ACK'ed all the remaining
         * bulk-out packets, in which case the host wouldn't see a
         * STALL.  Not realizing the endpoint was halted, it wouldn't
         * clear the halt -- leading to problems later on. */
#if 0
        } else if (common->can_stall) {
            if (fsg_is_set(common))
                fsg_set_halt(common->fsg,
                         common->fsg->bulk_out);
            raise_exception(common, FSG_STATE_ABORT_BULK_OUT);
            rc = -EINTR;
#endif

        /* We can't stall.  Read in the excess data and throw it
         * all away. */
        } else {
            rc = throw_away_data(common);
        }
        break;
    }
    return rc;
}


static int send_status(struct fsg_common *common)
{
    struct fsg_lun      *curlun = &common->luns[common->lun];
    struct fsg_buffhd   *bh;
    struct bulk_cs_wrap *csw;
    int         rc;
    u8          status = USB_STATUS_PASS;
    u32         sd, sdinfo = 0;

    /* Wait for the next buffer to become available */
    bh = common->next_buffhd_to_fill;
    while (bh->state != BUF_STATE_EMPTY) {
        rc = sleep_thread(common);
        if (rc)
            return rc;
    }

    if (curlun)
        sd = curlun->sense_data;
    else if (common->bad_lun_okay)
        sd = SS_NO_SENSE;
    else
        sd = SS_LOGICAL_UNIT_NOT_SUPPORTED;

    if (common->phase_error) {
        DBG(common, "sending phase-error status\n");
        status = USB_STATUS_PHASE_ERROR;
        sd = SS_INVALID_COMMAND;
    } else if (sd != SS_NO_SENSE) {
        DBG(common, "sending command-failure status\n");
        status = USB_STATUS_FAIL;
        VDBG(common, "  sense data: SK x%02x, ASC x%02x, ASCQ x%02x;"
            "  info x%x\n",
            SK(sd), ASC(sd), ASCQ(sd), sdinfo);
    }

    /* Store and send the Bulk-only CSW */
    csw = (void *)bh->buf;

    csw->Signature = cpu_to_le32(USB_BULK_CS_SIG);
    csw->Tag = common->tag;
    csw->Residue = cpu_to_le32(common->residue);
    csw->Status = status;

    bh->inreq->length = USB_BULK_CS_WRAP_LEN;
    bh->inreq->zero = 0;
    START_TRANSFER_OR(common, bulk_in, bh->inreq,
              &bh->inreq_busy, &bh->state)
        /* Don't know what to do if common->fsg is NULL */
        return -EIO;
    //printf("status SK %02x, ASC %02x\n", SK(sd), ASC(sd));
    common->next_buffhd_to_fill = bh->next;
    return 0;
}


/*-------------------------------------------------------------------------*/

/* Check whether the command is properly formed and whether its data size
 * and direction agree with the values we already have. */
static int check_command(struct fsg_common *common, int cmnd_size,
        enum data_direction data_dir, unsigned int mask,
        int needs_medium, const char *name)
{
    int         i;
    int         lun = common->cmnd[1] >> 5;
    static const char   dirletter[4] = {'u', 'o', 'i', 'n'};
    char            hdlen[20];
    struct fsg_lun      *curlun;

    hdlen[0] = 0;
    if (common->data_dir != DATA_DIR_UNKNOWN)
        sprintf(hdlen, ", H%c=%u", dirletter[(int) common->data_dir],
                common->data_size);
    VDBG(common, "SCSI command: %s;  Dc=%d, D%c=%u;  Hc=%d%s\n",
         name, cmnd_size, dirletter[(int) data_dir],
         common->data_size_from_cmnd, common->cmnd_size, hdlen);

    /* We can't reply at all until we know the correct data direction
     * and size. */
    if (common->data_size_from_cmnd == 0)
        data_dir = DATA_DIR_NONE;
    if (common->data_size < common->data_size_from_cmnd) {
        /* Host data size < Device data size is a phase error.
         * Carry out the command, but only transfer as much as
         * we are allowed. */
        common->data_size_from_cmnd = common->data_size;
        common->phase_error = 1;
    }
    common->residue = common->data_size;
    common->usb_amount_left = common->data_size;

    /* Conflicting data directions is a phase error */
    if (common->data_dir != data_dir
     && common->data_size_from_cmnd > 0) {
        common->phase_error = 1;
        return -EINVAL;
    }

    /* Verify the length of the command itself */
    if (cmnd_size != common->cmnd_size) {

        /* Special case workaround: There are plenty of buggy SCSI
         * implementations. Many have issues with cbw->Length
         * field passing a wrong command size. For those cases we
         * always try to work around the problem by using the length
         * sent by the host side provided it is at least as large
         * as the correct command length.
         * Examples of such cases would be MS-Windows, which issues
         * REQUEST SENSE with cbw->Length == 12 where it should
         * be 6, and xbox360 issuing INQUIRY, TEST UNIT READY and
         * REQUEST SENSE with cbw->Length == 10 where it should
         * be 6 as well.
         */
        if (cmnd_size <= common->cmnd_size) {
            DBG(common, "%s is buggy! Expected length %d "
                "but we got %d\n", name,
                cmnd_size, common->cmnd_size);
            cmnd_size = common->cmnd_size;
        } else {
            common->phase_error = 1;
            return -EINVAL;
        }
    }

    /* Check that the LUN values are consistent */
    if (common->lun != lun)
        DBG(common, "using LUN %d from CBW, not LUN %d from CDB\n",
            common->lun, lun);

    /* Check the LUN */
    if (common->lun >= 0 && common->lun < common->nluns) {
        curlun = &common->luns[common->lun];
        if (common->cmnd[0] != SC_REQUEST_SENSE) {
            curlun->sense_data = SS_NO_SENSE;
            curlun->info_valid = 0;
        }
    } else {
        curlun = NULL;
        common->bad_lun_okay = 0;

        /* INQUIRY and REQUEST SENSE commands are explicitly allowed
         * to use unsupported LUNs; all others may not. */
        if (common->cmnd[0] != SC_INQUIRY &&
            common->cmnd[0] != SC_REQUEST_SENSE) {
            DBG(common, "unsupported LUN %d\n", common->lun);
            return -EINVAL;
        }
    }
#if 0
    /* If a unit attention condition exists, only INQUIRY and
     * REQUEST SENSE commands are allowed; anything else must fail. */
    if (curlun && curlun->unit_attention_data != SS_NO_SENSE &&
            common->cmnd[0] != SC_INQUIRY &&
            common->cmnd[0] != SC_REQUEST_SENSE) {
        curlun->sense_data = curlun->unit_attention_data;
        curlun->unit_attention_data = SS_NO_SENSE;
        return -EINVAL;
    }
#endif
    /* Check that only command bytes listed in the mask are non-zero */
    common->cmnd[1] &= 0x1f;            /* Mask away the LUN */
    for (i = 1; i < cmnd_size; ++i) {
        if (common->cmnd[i] && !(mask & (1 << i))) {
            if (curlun)
                curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
            return -EINVAL;
        }
    }

    return 0;
}


static int do_scsi_command(struct fsg_common *common)
{
    struct fsg_buffhd   *bh;
    int         rc;
    int         reply = -EINVAL;
    struct fsg_lun      *curlun = &common->luns[common->lun];

    dump_cdb(common);

    /* Wait for the next buffer to become available for data or status */
    bh = common->next_buffhd_to_fill;
    common->next_buffhd_to_drain = bh;
    while (bh->state != BUF_STATE_EMPTY) {
        rc = sleep_thread(common);
        if (rc)
            return rc;
    }
    common->phase_error = 0;
    common->short_packet_received = 0;

    down_read(&common->filesem);    /* We're using the backing file */
    //printf("sc cmd %X\n", common->cmnd[0]);
    switch (common->cmnd[0]) {

    case SC_INQUIRY:
        common->data_size_from_cmnd = common->cmnd[4];
        reply = check_command(common, 6, DATA_DIR_TO_HOST,
                      (1<<4), 0,
                      "INQUIRY");
        if (reply == 0)
            reply = do_inquiry(common, bh);
        break;

    case SC_READ_CAPACITY:
        common->data_size_from_cmnd = 8;
        reply = check_command(common, 10, DATA_DIR_TO_HOST,
                      (0xf<<2) | (1<<8), 1,
                      "READ CAPACITY");
        if (reply == 0)
            reply = do_read_capacity(common, bh);
        break;

    case SC_READ_FORMAT_CAPACITIES:
        common->data_size_from_cmnd =
            get_unaligned_be16(&common->cmnd[7]);
        reply = check_command(common, 10, DATA_DIR_TO_HOST,
                      (3<<7), 1,
                      "READ FORMAT CAPACITIES");
        if (reply == 0)
            reply = do_read_format_capacities(common, bh);
        break;

    case SC_REQUEST_SENSE:
        common->data_size_from_cmnd = common->cmnd[4];
        reply = check_command(common, 6, DATA_DIR_TO_HOST,
                      (1<<4), 0,
                      "REQUEST SENSE");
        if (reply == 0)
            reply = do_request_sense(common, bh);
        break;

    case SC_VENDOR_SSTAR:
        common->data_size_from_cmnd =
            get_unaligned_be32(&common->cmnd[6]);
        reply = do_vendor_sstar(common, bh);
        if (reply < 0) {
            common->data_size_from_cmnd = 0;
            curlun->sense_data = SS_INVALID_FIELD_IN_CDB;
            reply = -EINVAL;
        }
        break;

    case SC_MODE_SELECT_6:
    case SC_MODE_SELECT_10:
    case SC_MODE_SENSE_6:
    case SC_MODE_SENSE_10:
    case SC_PREVENT_ALLOW_MEDIUM_REMOVAL:
    case SC_READ_6:
    case SC_READ_10:
    case SC_READ_12:
    case SC_READ_HEADER:
    case SC_READ_TOC:
    case SC_START_STOP_UNIT:
    case SC_SYNCHRONIZE_CACHE:
    case SC_TEST_UNIT_READY:
    case SC_VERIFY:
    case SC_WRITE_6:
    case SC_WRITE_10:
    case SC_WRITE_12:
    /* Some mandatory commands that we recognize but don't implement.
     * They don't mean much in this setting.  It's left as an exercise
     * for anyone interested to implement RESERVE and RELEASE in terms
     * of Posix locks. */
    case SC_FORMAT_UNIT:
    case SC_RELEASE:
    case SC_RESERVE:
    case SC_SEND_DIAGNOSTIC:
        /* Fall through */
    default:
        common->data_size_from_cmnd = 0;
        debug("\nUnknown x%02x\n", common->cmnd[0]);
        curlun->sense_data = SS_MEDIUM_NOT_PRESENT;
        reply = -EINVAL;
        break;
    }
    up_read(&common->filesem);

    if (reply == -EINTR)
        return -EINTR;

    /* Set up the single reply buffer for finish_reply() */
    if (reply == -EINVAL)
        reply = 0;      /* Error reply length */
    if (reply >= 0 && common->data_dir == DATA_DIR_TO_HOST) {
        reply = min((u32) reply, common->data_size_from_cmnd);
        bh->inreq->length = reply;
        bh->state = BUF_STATE_FULL;
        common->residue -= reply;
    }               /* Otherwise it's already set */

    return 0;
}

/*-------------------------------------------------------------------------*/

static int received_cbw(struct fsg_dev *fsg, struct fsg_buffhd *bh)
{
    struct usb_request  *req = bh->outreq;
    struct fsg_bulk_cb_wrap *cbw = req->buf;
    struct fsg_common   *common = fsg->common;

    /* Was this a real packet?  Should it be ignored? */
    if (req->status || test_bit(IGNORE_BULK_OUT, &fsg->atomic_bitflags))
        return -EINVAL;

    /* Is the CBW valid? */
    if (req->actual != USB_BULK_CB_WRAP_LEN ||
            cbw->Signature != cpu_to_le32(
                USB_BULK_CB_SIG)) {
        printf("invalid CBW: len %u %u sig 0x%x\n",
                req->length,
                req->actual,
                le32_to_cpu(cbw->Signature));

        /* The Bulk-only spec says we MUST stall the IN endpoint
         * (6.6.1), so it's unavoidable.  It also says we must
         * retain this state until the next reset, but there's
         * no way to tell the controller driver it should ignore
         * Clear-Feature(HALT) requests.
         *
         * We aren't required to halt the OUT endpoint; instead
         * we can simply accept and discard any data received
         * until the next reset. */
        wedge_bulk_in_endpoint(fsg);
        set_bit(IGNORE_BULK_OUT, &fsg->atomic_bitflags);
        return -EINVAL;
    }

    /* Is the CBW meaningful? */
    if (cbw->Lun >= FSG_MAX_LUNS || cbw->Flags & ~USB_BULK_IN_FLAG ||
            cbw->Length <= 0 || cbw->Length > MAX_COMMAND_SIZE) {
        printf("non-meaningful CBW: lun = %u, flags = 0x%x, "
                "cmdlen %u\n",
                cbw->Lun, cbw->Flags, cbw->Length);

        /* We can do anything we want here, so let's stall the
         * bulk pipes if we are allowed to. */
        if (common->can_stall) {
            fsg_set_halt(fsg, fsg->bulk_out);
            halt_bulk_in_endpoint(fsg);
        }
        return -EINVAL;
    }

    /* Save the command for later */
    common->cmnd_size = cbw->Length;
    memcpy(common->cmnd, cbw->CDB, common->cmnd_size);
    if (cbw->Flags & USB_BULK_IN_FLAG)
        common->data_dir = DATA_DIR_TO_HOST;
    else
        common->data_dir = DATA_DIR_FROM_HOST;
    common->data_size = le32_to_cpu(cbw->DataTransferLength);
    if (common->data_size == 0)
        common->data_dir = DATA_DIR_NONE;
    common->lun = cbw->Lun;
    common->tag = cbw->Tag;
    return 0;
}


static int get_next_command(struct fsg_common *common)
{
    struct fsg_buffhd   *bh;
    int         rc = 0;

    /* Wait for the next buffer to become available */
    bh = common->next_buffhd_to_fill;
    while (bh->state != BUF_STATE_EMPTY) {
        rc = sleep_thread(common);
        if (rc)
            return rc;
    }

    /* Queue a request to read a Bulk-only CBW */
    set_bulk_out_req_length(common, bh, USB_BULK_CB_WRAP_LEN);
    bh->outreq->short_not_ok = 1;
    START_TRANSFER_OR(common, bulk_out, bh->outreq,
              &bh->outreq_busy, &bh->state)
        /* Don't know what to do if common->fsg is NULL */
        return -EIO;

    /* We will drain the buffer in software, which means we
     * can reuse it for the next filling.  No need to advance
     * next_buffhd_to_fill. */

    /* Wait for the CBW to arrive */
    while (bh->state != BUF_STATE_FULL) {
        rc = sleep_thread(common);
        if (rc)
            return rc;
    }

    rc = fsg_is_set(common) ? received_cbw(common->fsg, bh) : -EIO;
    bh->state = BUF_STATE_EMPTY;

    return rc;
}


/*-------------------------------------------------------------------------*/

static int enable_endpoint(struct fsg_common *common, struct usb_ep *ep,
        const struct usb_endpoint_descriptor *d)
{
    int rc;

    ep->driver_data = common;
    rc = usb_ep_enable(ep, d);
    if (rc)
        ERROR(common, "can't enable %s, result %d\n", ep->name, rc);
    return rc;
}

static int alloc_request(struct fsg_common *common, struct usb_ep *ep,
        struct usb_request **preq)
{
    *preq = usb_ep_alloc_request(ep, GFP_ATOMIC);
    if (*preq)
        return 0;
    ERROR(common, "can't allocate request for %s\n", ep->name);
    return -ENOMEM;
}

/* Reset interface setting and re-init endpoint state (toggle etc). */
static int do_set_interface(struct fsg_common *common, struct fsg_dev *new_fsg)
{
    const struct usb_endpoint_descriptor *d;
    struct fsg_dev *fsg;
    int i, rc = 0;

    if (common->running)
        DBG(common, "reset interface\n");

reset:
    /* Deallocate the requests */
    if (common->fsg) {
        fsg = common->fsg;

        for (i = 0; i < FSG_NUM_BUFFERS; ++i) {
            struct fsg_buffhd *bh = &common->buffhds[i];

            if (bh->inreq) {
                usb_ep_free_request(fsg->bulk_in, bh->inreq);
                bh->inreq = NULL;
            }
            if (bh->outreq) {
                usb_ep_free_request(fsg->bulk_out, bh->outreq);
                bh->outreq = NULL;
            }
        }

        /* Disable the endpoints */
        if (fsg->bulk_in_enabled) {
            usb_ep_disable(fsg->bulk_in);
            fsg->bulk_in_enabled = 0;
        }
        if (fsg->bulk_out_enabled) {
            usb_ep_disable(fsg->bulk_out);
            fsg->bulk_out_enabled = 0;
        }

        common->fsg = NULL;
        /* wake_up(&common->fsg_wait); */
    }

    common->running = 0;
    if (!new_fsg || rc)
        return rc;

    common->fsg = new_fsg;
    fsg = common->fsg;

    /* Enable the endpoints */
    d = fsg_ep_desc(common->gadget,
            &fsg_fs_bulk_in_desc, &fsg_hs_bulk_in_desc);
    rc = enable_endpoint(common, fsg->bulk_in, d);
    if (rc)
        goto reset;
    fsg->bulk_in_enabled = 1;

    d = fsg_ep_desc(common->gadget,
            &fsg_fs_bulk_out_desc, &fsg_hs_bulk_out_desc);
    rc = enable_endpoint(common, fsg->bulk_out, d);
    if (rc)
        goto reset;
    fsg->bulk_out_enabled = 1;
    common->bulk_out_maxpacket =
                le16_to_cpu(get_unaligned(&d->wMaxPacketSize));
    clear_bit(IGNORE_BULK_OUT, &fsg->atomic_bitflags);

    /* Allocate the requests */
    for (i = 0; i < FSG_NUM_BUFFERS; ++i) {
        struct fsg_buffhd   *bh = &common->buffhds[i];

        rc = alloc_request(common, fsg->bulk_in, &bh->inreq);
        if (rc)
            goto reset;
        rc = alloc_request(common, fsg->bulk_out, &bh->outreq);
        if (rc)
            goto reset;
        bh->inreq->buf = bh->outreq->buf = bh->buf;
        bh->inreq->context = bh->outreq->context = bh;
        bh->inreq->complete = bulk_in_complete;
        bh->outreq->complete = bulk_out_complete;
    }

    common->running = 1;

    return rc;
}


/****************************** ALT CONFIGS ******************************/


static int fsg_set_alt(struct usb_function *f, unsigned intf, unsigned alt)
{
    struct fsg_dev *fsg = fsg_from_func(f);
    fsg->common->new_fsg = fsg;
    raise_exception(fsg->common, FSG_STATE_CONFIG_CHANGE);
    return 0;
}

static void fsg_disable(struct usb_function *f)
{
    struct fsg_dev *fsg = fsg_from_func(f);
    fsg->common->new_fsg = NULL;
    raise_exception(fsg->common, FSG_STATE_CONFIG_CHANGE);
}

/*-------------------------------------------------------------------------*/

static void handle_exception(struct fsg_common *common)
{
    int         i;
    struct fsg_buffhd   *bh;
    enum fsg_state      old_state;
    struct fsg_lun      *curlun;
    unsigned int        exception_req_tag;

    /* Cancel all the pending transfers */
    if (common->fsg) {
        for (i = 0; i < FSG_NUM_BUFFERS; ++i) {
            bh = &common->buffhds[i];
            if (bh->inreq_busy)
                usb_ep_dequeue(common->fsg->bulk_in, bh->inreq);
            if (bh->outreq_busy)
                usb_ep_dequeue(common->fsg->bulk_out,
                           bh->outreq);
        }

        /* Wait until everything is idle */
        for (;;) {
            int num_active = 0;
            for (i = 0; i < FSG_NUM_BUFFERS; ++i) {
                bh = &common->buffhds[i];
                num_active += bh->inreq_busy + bh->outreq_busy;
            }
            if (num_active == 0)
                break;
            if (sleep_thread(common))
                return;
        }

        /* Clear out the controller's fifos */
        if (common->fsg->bulk_in_enabled)
            usb_ep_fifo_flush(common->fsg->bulk_in);
        if (common->fsg->bulk_out_enabled)
            usb_ep_fifo_flush(common->fsg->bulk_out);
    }

    /* Reset the I/O buffer states and pointers, the SCSI
     * state, and the exception.  Then invoke the handler. */

    for (i = 0; i < FSG_NUM_BUFFERS; ++i) {
        bh = &common->buffhds[i];
        bh->state = BUF_STATE_EMPTY;
    }
    common->next_buffhd_to_fill = &common->buffhds[0];
    common->next_buffhd_to_drain = &common->buffhds[0];
    exception_req_tag = common->exception_req_tag;
    old_state = common->state;

    if (old_state == FSG_STATE_ABORT_BULK_OUT)
        common->state = FSG_STATE_STATUS_PHASE;
    else {
        for (i = 0; i < common->nluns; ++i) {
            curlun = &common->luns[i];
            curlun->sense_data = SS_NO_SENSE;
            curlun->info_valid = 0;
        }
        common->state = FSG_STATE_IDLE;
    }

    /* Carry out any extra actions required for the exception */
    switch (old_state) {
    case FSG_STATE_ABORT_BULK_OUT:
        send_status(common);

        if (common->state == FSG_STATE_STATUS_PHASE)
            common->state = FSG_STATE_IDLE;
        break;

    case FSG_STATE_RESET:
        /* In case we were forced against our will to halt a
         * bulk endpoint, clear the halt now.  (The SuperH UDC
         * requires this.) */
        if (!fsg_is_set(common))
            break;
        if (test_and_clear_bit(IGNORE_BULK_OUT,
                       &common->fsg->atomic_bitflags))
            usb_ep_clear_halt(common->fsg->bulk_in);

        if (common->ep0_req_tag == exception_req_tag)
            ep0_queue(common);  /* Complete the status stage */

        break;

    case FSG_STATE_CONFIG_CHANGE:
        do_set_interface(common, common->new_fsg);
        break;

    case FSG_STATE_EXIT:
    case FSG_STATE_TERMINATED:
        do_set_interface(common, NULL);     /* Free resources */
        common->state = FSG_STATE_TERMINATED;   /* Stop the thread */
        break;

    case FSG_STATE_INTERFACE_CHANGE:
    case FSG_STATE_DISCONNECT:
    case FSG_STATE_COMMAND_PHASE:
    case FSG_STATE_DATA_PHASE:
    case FSG_STATE_STATUS_PHASE:
    case FSG_STATE_IDLE:
        break;
    }
}

/*-------------------------------------------------------------------------*/

int fsg_main_thread(void *common_)
{
    int ret;
    struct fsg_common   *common = the_fsg_common;
    /* The main loop */
    do {
        if (exception_in_progress(common)) {
            handle_exception(common);
            continue;
        }

        if (!common->running) {
            ret = sleep_thread(common);
            if (ret)
                return ret;

            continue;
        }

        ret = get_next_command(common);
        if (ret)
            return ret;

        if (!exception_in_progress(common))
            common->state = FSG_STATE_DATA_PHASE;

        if (do_scsi_command(common) || finish_reply(common))
            continue;

        if (!exception_in_progress(common))
            common->state = FSG_STATE_STATUS_PHASE;

        if (send_status(common))
            continue;

        if (!exception_in_progress(common))
            common->state = FSG_STATE_IDLE;
    } while (0);

    common->thread_task = NULL;

    return 0;
}

static void fsg_common_release(struct kref *ref);

static struct fsg_common *fsg_common_init(struct fsg_common *common,
                      struct usb_composite_dev *cdev)
{
    struct usb_gadget *gadget = cdev->gadget;
    struct fsg_buffhd *bh;
    struct fsg_lun *curlun;
    int nluns, i, rc;

    /* Find out how many LUNs there should be */
    nluns = 1;
    if (nluns < 1 || nluns > FSG_MAX_LUNS) {
        printf("invalid number of LUNs: %u\n", nluns);
        return ERR_PTR(-EINVAL);
    }

    /* Allocate? */
    if (!common) {
        common = calloc(sizeof(*common), 1);
        if (!common)
            return ERR_PTR(-ENOMEM);
        common->free_storage_on_release = 1;
    } else {
        memset(common, 0, sizeof(*common));
        common->free_storage_on_release = 0;
    }

    common->ops = NULL;
    common->private_data = NULL;

    common->gadget = gadget;
    common->ep0 = gadget->ep0;
    common->ep0req = cdev->req;

    fsg_intf_desc.iInterface = 0;

    /* Create the LUNs, open their backing files, and register the
     * LUN devices in sysfs. */
    curlun = calloc(nluns, sizeof *curlun);
    if (!curlun) {
        rc = -ENOMEM;
        goto error_release;
    }
    common->nluns = nluns;

    for (i = 0; i < nluns; i++) {
        common->luns[i].removable = 1;

        rc = fsg_lun_open(&common->luns[i], "");
        if (rc)
            goto error_luns;
    }
    common->lun = 0;

    /* Data buffers cyclic list */
    bh = common->buffhds;

    i = FSG_NUM_BUFFERS;
    goto buffhds_first_it;
    do {
        bh->next = bh + 1;
        ++bh;
buffhds_first_it:
        bh->inreq_busy = 0;
        bh->outreq_busy = 0;
        bh->buf = memalign(CONFIG_SYS_CACHELINE_SIZE, FSG_BUFLEN);
        if (unlikely(!bh->buf)) {
            rc = -ENOMEM;
            goto error_release;
        }
    } while (--i);
    bh->next = common->buffhds;

    snprintf(common->inquiry_string, sizeof common->inquiry_string,
         "%-8s%-16s%04x",
         "Linux   ",
         "File-Store Gadget",
         0xffff);

    /* Some peripheral controllers are known not to be able to
     * halt bulk endpoints correctly.  If one of them is present,
     * disable stalls.
     */

    /* Tell the thread to start working */
    common->thread_task =
        kthread_create(fsg_main_thread, common,
                   OR(cfg->thread_name, "file-storage"));
    if (IS_ERR(common->thread_task)) {
        rc = PTR_ERR(common->thread_task);
        goto error_release;
    }

#undef OR
    /* Information */
    INFO(common, FSG_DRIVER_DESC ", version: " FSG_DRIVER_VERSION "\n");
    INFO(common, "Number of LUNs=%d\n", common->nluns);

    return common;

error_luns:
    common->nluns = i + 1;
error_release:
    common->state = FSG_STATE_TERMINATED;   /* The thread is dead */
    /* Call fsg_common_release() directly, ref might be not
     * initialised */
    fsg_common_release(&common->ref);
    return ERR_PTR(rc);
}

static void fsg_common_release(struct kref *ref)
{
    struct fsg_common *common = container_of(ref, struct fsg_common, ref);

    /* If the thread isn't already dead, tell it to exit now */
    if (common->state != FSG_STATE_TERMINATED) {
        raise_exception(common, FSG_STATE_EXIT);
        wait_for_completion(&common->thread_notifier);
    }

    if (likely(common->luns)) {
        struct fsg_lun *lun = common->luns;
        unsigned i = common->nluns;

        /* In error recovery common->nluns may be zero. */
        for (; i; --i, ++lun)
            fsg_lun_close(lun);

        kfree(common->luns);
    }

    {
        struct fsg_buffhd *bh = common->buffhds;
        unsigned i = FSG_NUM_BUFFERS;
        do {
            kfree(bh->buf);
        } while (++bh, --i);
    }

    if (common->free_storage_on_release)
        kfree(common);
}


/*-------------------------------------------------------------------------*/

/**
 * usb_copy_descriptors - copy a vector of USB descriptors
 * @src: null-terminated vector to copy
 * Context: initialization code, which may sleep
 *
 * This makes a copy of a vector of USB descriptors.  Its primary use
 * is to support usb_function objects which can have multiple copies,
 * each needing different descriptors.  Functions may have static
 * tables of descriptors, which are used as templates and customized
 * with identifiers (for interfaces, strings, endpoints, and more)
 * as needed by a given function instance.
 */
struct usb_descriptor_header **
usb_copy_descriptors(struct usb_descriptor_header **src)
{
    struct usb_descriptor_header **tmp;
    unsigned bytes;
    unsigned n_desc;
    void *mem;
    struct usb_descriptor_header **ret;

    /* count descriptors and their sizes; then add vector size */
    for (bytes = 0, n_desc = 0, tmp = src; *tmp; tmp++, n_desc++)
        bytes += (*tmp)->bLength;
    bytes += (n_desc + 1) * sizeof(*tmp);

    mem = memalign(CONFIG_SYS_CACHELINE_SIZE, bytes);
    if (!mem)
        return NULL;

    /* fill in pointers starting at "tmp",
     * to descriptors copied starting at "mem";
     * and return "ret"
     */
    tmp = mem;
    ret = mem;
    mem += (n_desc + 1) * sizeof(*tmp);
    while (*src) {
        memcpy(mem, *src, (*src)->bLength);
        *tmp = mem;
        tmp++;
        mem += (*src)->bLength;
        src++;
    }
    *tmp = NULL;

    return ret;
}

static void fsg_unbind(struct usb_configuration *c, struct usb_function *f)
{
    struct fsg_dev      *fsg = fsg_from_func(f);

    DBG(fsg, "unbind\n");
    if (fsg->common->fsg == fsg) {
        fsg->common->new_fsg = NULL;
        raise_exception(fsg->common, FSG_STATE_CONFIG_CHANGE);
    }

    free(fsg->function.descriptors);
    free(fsg->function.hs_descriptors);
    kfree(fsg);
}

static int fsg_bind(struct usb_configuration *c, struct usb_function *f)
{
    struct fsg_dev      *fsg = fsg_from_func(f);
    struct usb_gadget   *gadget = c->cdev->gadget;
    int         i;
    struct usb_ep       *ep;
    fsg->gadget = gadget;

    /* New interface */
    i = usb_interface_id(c, f);
    if (i < 0)
        return i;
    fsg_intf_desc.bInterfaceNumber = i;
    fsg->interface_number = i;

    /* Find all the endpoints we will use */
    ep = usb_ep_autoconfig(gadget, &fsg_fs_bulk_in_desc);
    if (!ep)
        goto autoconf_fail;
    ep->driver_data = fsg->common;  /* claim the endpoint */
    fsg->bulk_in = ep;

    ep = usb_ep_autoconfig(gadget, &fsg_fs_bulk_out_desc);
    if (!ep)
        goto autoconf_fail;
    ep->driver_data = fsg->common;  /* claim the endpoint */
    fsg->bulk_out = ep;

    /* Copy descriptors */
    fsg_hs_bulk_out_desc.bInterval = 0;
    f->descriptors = usb_copy_descriptors(fsg_fs_function);
    if (unlikely(!f->descriptors))
        return -ENOMEM;

    if (gadget_is_dualspeed(gadget)) {
        /* Assume endpoint addresses are the same for both speeds */
        fsg_hs_bulk_in_desc.bEndpointAddress =
            fsg_fs_bulk_in_desc.bEndpointAddress;
        fsg_hs_bulk_out_desc.bEndpointAddress =
            fsg_fs_bulk_out_desc.bEndpointAddress;
        f->hs_descriptors = usb_copy_descriptors(fsg_hs_function);
        if (unlikely(!f->hs_descriptors)) {
            free(f->descriptors);
            return -ENOMEM;
        }
    }
    return 0;

autoconf_fail:
    ERROR(fsg, "unable to autoconfigure all endpoints\n");
    return -ENOTSUPP;
}


/****************************** ADD FUNCTION ******************************/

static struct usb_gadget_strings *fsg_strings_array[] = {
    NULL,
    &fsg_stringtab, // avoid build warning
};

static int fsg_bind_config(struct usb_composite_dev *cdev,
               struct usb_configuration *c,
               struct fsg_common *common)
{
    struct fsg_dev *fsg;
    int rc;

    fsg = calloc(1, sizeof *fsg);
    if (!fsg)
        return -ENOMEM;
    fsg->function.name        = FSG_DRIVER_DESC;
    fsg->function.strings     = fsg_strings_array;
    fsg->function.bind        = fsg_bind;
    fsg->function.unbind      = fsg_unbind;
    fsg->function.setup       = fsg_setup;
    fsg->function.set_alt     = fsg_set_alt;
    fsg->function.disable     = fsg_disable;

    fsg->common               = common;
    common->fsg               = fsg;
    /* Our caller holds a reference to common structure so we
     * don't have to be worry about it being freed until we return
     * from this function.  So instead of incrementing counter now
     * and decrement in error recovery we increment it only when
     * call to usb_add_function() was successful. */

    rc = usb_add_function(c, &fsg->function);

    if (rc)
        kfree(fsg);

    return rc;
}

int fsg_add(struct usb_configuration *c)
{
    struct fsg_common *fsg_common;

    fsg_common = fsg_common_init(NULL, c->cdev);

    fsg_common->vendor_name = 0;
    fsg_common->product_name = 0;
    fsg_common->release = 0xffff;

    fsg_common->ops = NULL;
    fsg_common->private_data = NULL;

    the_fsg_common = fsg_common;

    return fsg_bind_config(c->cdev, c, fsg_common);
}

int fsg_init(struct ums *ums_dev)
{
	memset(&ufu, 0, sizeof(ufu));

	return 0;
}

DECLARE_GADGET_BIND_CALLBACK(usb_dnl_ufu, fsg_add);
