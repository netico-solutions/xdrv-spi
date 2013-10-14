/*
 * This file is part of x_spi
 *
 * Copyright (C) 2011, 2012 - Nenad Radulovic
 *
 * x_spi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * x_spi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with x_spi; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 *
 * web site:    http://blueskynet.dyndns-server.com
 * e-mail  :    blueskyniss@gmail.com
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Driver main file
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include "linux/module.h"
#include "linux/printk.h"

#include "arch/compiler.h"
#include "drv/x_spi_cfg.h"
#include "drv/x_spi_lld.h"
#include "drv/x_spi.h"
#include "port/port.h"
#include "dbg/dbg.h"
#include "log/log.h"

/*=========================================================  LOCAL MACRO's  ==*/

#define DEF_MAX_DEVICES                 10

/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

int handleOpen(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    int                 oflag);

int handleClose(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr);

int handleIOctl(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    unsigned int        req,
    void __user *       arg);

ssize_t handleRd(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    void *              dst,
    size_t              bytes);

ssize_t handleWr(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    const void *        src,
    size_t              bytes);

/*=======================================================  LOCAL VARIABLES  ==*/

DECL_MODULE_INFO(DEF_DRV_NAME, DEF_DRV_DESCRIPTION, DEF_DRV_AUTHOR);

static struct rtdm_device * Devs[DEF_MAX_DEVICES];

static const struct rtdm_device DevTemplate = {
    .struct_version     = RTDM_DEVICE_STRUCT_VER,
    .device_flags       = RTDM_NAMED_DEVICE | RTDM_EXCLUSIVE,
    .context_size       = sizeof(struct devCtx),
    .device_name        = DEF_DRV_NAME,
    .protocol_family    = 0,
    .socket_type        = 0,
    .open_rt            = NULL,
    .open_nrt           = handleOpen,
    .socket_rt          = NULL,
    .socket_nrt         = NULL,
    .ops                = {
        .close_rt           = NULL,
        .close_nrt          = handleClose,
        .ioctl_rt           = handleIOctl,
        .ioctl_nrt          = handleIOctl,
        .select_bind        = NULL,
        .read_rt            = handleRd,
        .read_nrt           = NULL,
        .write_rt           = handleWr,
        .write_nrt          = NULL,
        .recvmsg_rt         = NULL,
        .recvmsg_nrt        = NULL,
        .sendmsg_rt         = NULL,
        .sendmsg_nrt        = NULL
    },
    .device_class       = RTDM_CLASS_SERIAL,
    .device_sub_class   = 0,
    .profile_version    = 0,
    .driver_name        = DEF_DRV_NAME,
    .driver_version     = RTDM_DRIVER_VER(DEF_DRV_VERSION_MAJOR, DEF_DRV_VERSION_MINOR, DEF_DRV_VERSION_PATCH),
    .peripheral_name    = DEF_DRV_SUPP_DEVICE,
    .provider_name      = DEF_DRV_AUTHOR,
    .proc_name          = DEF_DRV_NAME,
    .device_id          = 0,
    .device_data        = NULL
};

/*======================================================  GLOBAL VARIABLES  ==*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DEF_DRV_AUTHOR);
MODULE_DESCRIPTION(DEF_DRV_DESCRIPTION);
MODULE_SUPPORTED_DEVICE(DEF_DRV_SUPP_DEVICE);

/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

#if (0u == CFG_DMA_MODE)

int handleOpen(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    int                 oflag) {

    int                 retval;

    retval = 0;

    return (retval);
}

int handleClose(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr) {

    int                 retval;

    retval = 0;

    return (retval);
}

int handleIOctl(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    unsigned int        req,
    void __user *       arg) {

    int                 retval;

    retval = 0;

    return (retval);
}

ssize_t handleRd(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    void *              dst,
    size_t              bytes) {

    ssize_t             read;

    read = 0;

    return (read);
}

ssize_t handleWr(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    const void *        src,
    size_t              bytes) {

    ssize_t             write;

    write = 0;

    return (write);
}

#elif (1u == CFG_DMA_MODE)

int handleOpen(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    int                 oflag) {

    int                 retval;

    retval = 0;

    return (retval);
}

int handleClose(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr) {

    int                 retval;

    retval = 0;

    return (retval);
}

int handleIOctl(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    unsigned int        req,
    void __user *       arg) {

    int                 retval;

    retval = 0;

    return (retval);
}

ssize_t handleRd(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    void *              dst,
    size_t              bytes) {

    ssize_t             read;

    read = 0;

    return (read);
}

ssize_t handleWr(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    const void *        src,
    size_t              bytes) {

    ssize_t             write;

    write = 0;

    return (write);
}

#elif (2u == CFG_DMA_MODE)

int handleOpen(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    int                 oflag) {

    int                 retval;

    retval = 0;

    return (retval);
}

int handleClose(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr) {

    int                 retval;

    retval = 0;

    return (retval);
}

int handleIOctl(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    unsigned int        req,
    void __user *       arg) {

    int                 retval;

    retval = 0;

    return (retval);
}

ssize_t handleRd(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    void *              dst,
    size_t              bytes) {

    ssize_t             read;

    read = 0;

    return (read);
}

ssize_t handleWr(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    const void *        src,
    size_t              bytes) {

    ssize_t             write;

    write = 0;

    return (write);
}

#endif

/*===================================  GLOBAL PRIVATE FUNCTION DEFINITIONS  ==*/
/*====================================  GLOBAL PUBLIC FUNCTION DEFINITIONS  ==*/

/* Module entry                                                               */
int __init moduleInit(
    void) {

    int                 retval;
    uint32_t            i;

    retval = 0;

    LOG(DEF_DRV_DESCRIPTION " v%d.%d.%d", DEF_DRV_VERSION_MAJOR, DEF_DRV_VERSION_MINOR, DEF_DRV_VERSION_PATCH);

    for (i = 0u; i < DEF_MAX_DEVICES; i++) {

        if (TRUE == portDevIsReady(i)) {
            struct rtdm_device * dev;
            int32_t     ret;

            LOG_INFO("building SPI device: %d", i);
            ret = portDevCreate(
                &dev,
                &DevTemplate,
                i);

            if (0 != ret) {
                LOG_ERR("failed to build device: %d, err: %d", i, -ret);
                retval = (int)ret;
                break;
            }
            LOG_INFO("initializing SPI device: %d", i);
            ret = lldDevInit(
                dev);

            if (0 != ret) {
                LOG_ERR("failed to initialize device: %d, err: %d", i, -ret);
                portDevDestroy(
                    dev);
                retval = (int)ret;
                break;
            }
            LOG_INFO("registering SPI device: %d", i);
            retval = rtdm_dev_register(
                dev);

            if (0 != retval) {
                LOG_ERR("failed to register device: %d, err: %d", i, -retval);
                lldDevTerm(
                    dev);
                portDevDestroy(
                    dev);
                break;
            }
            LOG_INFO("SPI device %d successfully brought online", i);
            Devs[i] = dev;
        } else {
            LOG_DBG("skipping SPI device: %d", i);
        }
    }

    return (retval);
}

/* Module exit                                                                */
void __exit moduleTerm(
    void) {

    uint32_t            i;

    for (i = 0u; i < DEF_MAX_DEVICES; i++) {
        struct rtdm_device * dev;

        dev = Devs[i];

        if (NULL != dev) {
            int         retval;

            retval = rtdm_dev_unregister(
                dev,
                2000u);

            if (0 != retval) {
                LOG_WARN("SPI device %d failed to unregister cleanly, err: %d", i, -retval);
            }
            lldDevTerm(
                dev);
            portDevDestroy(
                dev);
        }
    }
}

void userAssert(
    const struct esDbgReport * dbgReport) {

    printk(KERN_ERR "\n ----\n");
    printk(KERN_ERR DEF_DRV_DESCRIPTION " ASSERTION FAILED\n");
    printk(KERN_ERR " Module name: %s\n", dbgReport->modName);
    printk(KERN_ERR " Module desc: %s\n", dbgReport->modDesc);
    printk(KERN_ERR " Module file: %s\n", dbgReport->modFile);
    printk(KERN_ERR " Module author: %s\n", dbgReport->modAuthor);
    printk(KERN_ERR " --\n");
    printk(KERN_ERR " Function   : %s\n", dbgReport->fnName);
    printk(KERN_ERR " Line       : %d\n", dbgReport->line);
    printk(KERN_ERR " Expression : %s\n", dbgReport->expr);
    printk(KERN_ERR " --\n");
    printk(KERN_ERR " Msg num    : %d\n", dbgReport->msgNum);
    printk(KERN_ERR " Msg text   : %s\n", dbgReport->msgText);
    printk(KERN_ERR " ----\n");
}

/* Module entry/exit declarations                                             */
module_init(moduleInit);
module_exit(moduleTerm);

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of x_spi.c
 ******************************************************************************/
