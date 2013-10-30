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
#include "drv/x_spi_ioctl.h"
#include "drv/x_spi_cfg.h"
#include "drv/x_spi_lld.h"
#include "drv/x_spi.h"
#include "port/port.h"
#include "dbg/dbg.h"
#include "log/log.h"

/*=========================================================  LOCAL MACRO's  ==*/

#define DEF_DEVCTX_SIGNATURE            0xdeadbeefu

#define XSPI_ACTIVITY_IDLE              0u
#define XSPI_ACTIVITY_RUNNIG               (!XSPI_ACTIVITY_IDLE)
#define XSPI_ACTIVITY_ID(chn)           (0x01u << (chn))

#define CFG_ARG_IS_VALID(argv, min, max)                                        \
    (((min) <= (argv)) && ((max) >= (argv)))

/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static int handleOpen(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    int                 oflag);

static int handleClose(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr);

static int handleIOctl(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    unsigned int        req,
    void __user *       arg);

static ssize_t handleRd(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    void *              dst,
    size_t              bytes);

static ssize_t handleWr(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    const void *        src,
    size_t              bytes);

/*=======================================================  LOCAL VARIABLES  ==*/

DECL_MODULE_INFO(DEF_DRV_NAME, DEF_DRV_DESCRIPTION, DEF_DRV_AUTHOR);

static struct rtdm_device * Devs[CFG_MAX_DEVICES];

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

static struct devCtx * getDevCtx(
    struct rtdm_dev_context * rtdmDevCtx) {

    return ((struct devCtx *)&rtdmDevCtx->dev_private[0]);
}

static void unitCtxInit(
    struct unitCtx *    unitCtx) {

}

static int32_t ctxInit(
    struct rtdm_dev_context * ctx) {

    struct devCtx *     devCtx;
    uint32_t            i;
    int32_t             ret;

    ret = 0;
    devCtx = getDevCtx(
        ctx);

    for (i = 0u; i < DEF_CHN_COUNT; i++) {
        devCtx->chn[i].online = FALSE;

        if (TRUE == portChnIsOnline(ctx->device, i)) {
            devCtx->chn[i].online = TRUE;
        }
    }
    rtdm_lock_init(&devCtx->lock);
    devCtx->activity    = 0u;
    ES_DBG_API_OBLIGATION(devCtx->signature = DEF_DEVCTX_SIGNATURE);

    return (ret);
}

static void ctxTerm(
    struct rtdm_dev_context * ctx) {

}

/* NOTE: rtdm_sem_down() requires to be called from preemptable code section,
 *       therefore we must temporarily disable global locks. This function will
 *       block if a configuration operation is ongoing.
 */
static void activitySetRunningI(
    struct rtdm_dev_context * ctx,
    rtdm_lockctx_t *    lockCtx) {

    struct devCtx *     devCtx;

    devCtx = getDevCtx(
        ctx);
    devCtx->activity++;

    if (1u == devCtx->activity) {
        rtdm_lock_put_irqrestore(&devCtx->lock, *lockCtx);
        rtdm_sem_down(
            &devCtx->idleLock);
        rtdm_lock_get_irqsave(&devCtx->lock, *lockCtx);
    }
}

static void activitySetIdleI(
    struct rtdm_dev_context * ctx) {

    struct devCtx *     devCtx;

    devCtx = getDevCtx(
        ctx);
    devCtx->activity--;

    if (0u == devCtx->activity) {
        rtdm_sem_up(
            &devCtx->idleLock);
    }
}

/*
 * TODO: Nešto što će da čeka na slobodne resurse
 */

static int32_t cfgApply(
    struct rtdm_dev_context * ctx) {

    struct devCtx *     devCtx;

    devCtx = getDevCtx(
        ctx);
    lldReset(ctx->device);
    lldModeSet(ctx->device, devCtx->cfg.mode);

    return (0);
}

static int32_t cfgChnSet(
    struct rtdm_dev_context * ctx,
    enum xspiChn        chn) {

    struct devCtx *     devCtx;
    rtdm_lockctx_t      lockCtx;

    LOG_DBG("CFG: set current channel to %d", chn);

    if (!CFG_ARG_IS_VALID(chn, XSPI_CHN_0, XSPI_CHN_3)) {

        return (-EINVAL);
    }

    if (FALSE == portChnIsOnline(ctx->device, chn)) {

        return (-EIDRM);
    }
    devCtx = getDevCtx(
        ctx);
    rtdm_lock_get_irqsave(&devCtx->lock, lockCtx);
    devCtx->cfg.chn = chn;
    rtdm_lock_put_irqrestore(&devCtx->lock, lockCtx);

    return (0);
}

static void cfgChnGet(
    struct rtdm_dev_context * ctx,
    enum xspiChn *      chn) {

    struct devCtx *     devCtx;

    devCtx = getDevCtx(
        ctx);
    *chn = devCtx->cfg.chn;

    LOG_DBG("CFG: current channel is %d", devCtx->cfg.chn);
}

static int32_t cfgFIFOChnSet(
    struct rtdm_dev_context * ctx,
    enum xspiFifoChn    chn) {

    struct devCtx *     devCtx;
    rtdm_lockctx_t      lockCtx;

    LOG_DBG("CFG: set FIFO channel to %d", chn);

    if (!CFG_ARG_IS_VALID(chn, XSPI_FIFO_CHN_DISABLED, XSPI_FIFO_CHN_3)) {

        return (-EINVAL);
    }

    if (XSPI_FIFO_CHN_DISABLED != chn) {

        if (FALSE == portChnIsOnline(ctx->device, (enum xspiChn)chn)) {

            return (-EIDRM);
        }
    }
    devCtx = getDevCtx(
        ctx);
    rtdm_lock_get_irqsave(&devCtx->lock, lockCtx);

    if (XSPI_ACTIVITY_RUNNIG == devCtx->activity) {
        rtdm_lock_put_irqrestore(&devCtx->lock, lockCtx);

        return (-EAGAIN);
    }

    if (XSPI_FIFO_CHN_DISABLED != devCtx->cfg.fifoChn) {
        lldFIFOChnDisable(
            ctx->device,
            devCtx->cfg.fifoChn);
    }
    devCtx->cfg.fifoChn = chn;

    if (XSPI_FIFO_CHN_DISABLED != devCtx->cfg.fifoChn) {
        lldFIFOChnEnable(
            ctx->device,
            devCtx->cfg.fifoChn);
    }
    rtdm_lock_put_irqrestore(&devCtx->lock, lockCtx);

    return (0);
}

static void cfgFIFOChnGet(
    struct rtdm_dev_context * ctx,
    enum xspiFifoChn *  chn) {

    struct devCtx *     devCtx;

    devCtx = getDevCtx(
        ctx);

    LOG_DBG("CFG: FIFO channel is %d", devCtx->cfg.fifoChn);

    *chn = devCtx->cfg.fifoChn;
}

static int32_t cfgCsModeSet(
    struct rtdm_dev_context * ctx,
    enum xspiCsMode     csMode) {

    struct devCtx *     devCtx;
    rtdm_lockctx_t      lockCtx;

    LOG_DBG("CFG: set CS mode to %d", csMode);

    if (!CFG_ARG_IS_VALID(csMode, XSPI_CS_MODE_ENABLED, XSPI_CS_MODE_DISABLED)) {

        return (-EINVAL);
    }
    devCtx = getDevCtx(
        ctx);
    rtdm_lock_get_irqsave(&devCtx->lock, lockCtx);

    if (XSPI_ACTIVITY_RUNNIG == devCtx->activity) {
        rtdm_lock_put_irqrestore(&devCtx->lock, lockCtx);

        return (-EAGAIN);
    }
    devCtx->cfg.csMode = csMode;
    lldCsModeSet(
        ctx->device,
        (int32_t)csMode);
    rtdm_lock_put_irqrestore(&devCtx->lock, lockCtx);

    return (0);
}

static void cfgCsModeGet(
    struct rtdm_dev_context * ctx,
    enum xspiCsMode *   csMode) {

    struct devCtx *     devCtx;

    devCtx = getDevCtx(
        ctx);

    LOG_DBG("CFG: CS mode is %d", devCtx->cfg.csMode);

    *csMode = devCtx->cfg.csMode;
}

static int32_t cfgModeSet(
    struct rtdm_dev_context * ctx,
    enum xspiMode       mode) {

    struct devCtx *     devCtx;
    rtdm_lockctx_t      lockCtx;

    LOG_DBG("CFG: set SPI mode to %d", mode);

    if (!CFG_ARG_IS_VALID(mode, XSPI_MODE_MASTER, XSPI_MODE_SLAVE)) {

        return (-EINVAL);
    }
    devCtx = getDevCtx(
        ctx);
    rtdm_lock_get_irqsave(&devCtx->lock, lockCtx);

    if (XSPI_ACTIVITY_RUNNIG == devCtx->activity) {
        rtdm_lock_put_irqrestore(&devCtx->lock, lockCtx);

        return (-EAGAIN);
    }
    devCtx->cfg.mode = mode;
    cfgApply(
        ctx);
    rtdm_lock_put_irqrestore(&devCtx->lock, lockCtx);

    return (0);
}

static void cfgModeGet(
    struct rtdm_dev_context * ctx,
    enum xspiMode *     mode) {

    struct devCtx *     devCtx;

    devCtx = getDevCtx(
        ctx);

    LOG_DBG("CFG: CS mode is %d", devCtx->cfg.csMode);

    *mode = devCtx->cfg.csMode;
}

static int32_t cfgChannelModeSet(
    struct rtdm_dev_context * ctx,
    enum xspiChannelMode channelMode) {

    struct devCtx *     devCtx;
    rtdm_lockctx_t      lockCtx;

    LOG_DBG("CFG: set initial delay to %d", channelMode);

    if (!CFG_ARG_IS_VALID(channelMode, XSPI_CHANNEL_MODE_MULTI, XSPI_CHANNEL_MODE_SINGLE)) {

        return (-EINVAL);
    }
    devCtx = getDevCtx(
        ctx);
    rtdm_lock_get_irqsave(&devCtx->lock, lockCtx);

    if (XSPI_ACTIVITY_RUNNIG == devCtx->activity) {
        rtdm_lock_put_irqrestore(&devCtx->lock, lockCtx);

        return (-EAGAIN);
    }
    devCtx->cfg.delay = channelMode;
    lldChannelModeSet(
        ctx->device,
        (uint32_t)channelMode);
    rtdm_lock_put_irqrestore(&devCtx->lock, lockCtx);

    return (0);
}

static void cfgChannelModeGet(
    struct rtdm_dev_context * ctx,
    enum xspiChannelMode * channelMode) {

    struct devCtx *     devCtx;

    devCtx = getDevCtx(
        ctx);

    LOG_DBG("CFG: channel mode is %d", devCtx->cfg.channelMode);

    *channelMode = devCtx->cfg.channelMode;
}

static int32_t cfgInitialDelaySet(
    struct rtdm_dev_context * ctx,
    enum xspiInitialDelay delay) {

    struct devCtx *     devCtx;
    rtdm_lockctx_t      lockCtx;

    LOG_DBG("CFG: set initial delay to %d", delay);

    if (!CFG_ARG_IS_VALID(delay, XSPI_INITIAL_DELAY_0, XSPI_INITIAL_DELAY_32)) {

        return (-EINVAL);
    }
    devCtx = getDevCtx(
        ctx);
    rtdm_lock_get_irqsave(&devCtx->lock, lockCtx);

    if (XSPI_ACTIVITY_RUNNIG == devCtx->activity) {
        rtdm_lock_put_irqrestore(&devCtx->lock, lockCtx);

        return (-EAGAIN);
    }
    devCtx->cfg.delay = delay;
    lldInitialDelaySet(
        ctx->device,
        (uint32_t)delay);
    rtdm_lock_put_irqrestore(&devCtx->lock, lockCtx);

    return (0);
}

static void cfgInitialDelayGet(
    struct rtdm_dev_context * ctx,
    enum xspiInitialDelay * delay) {

    struct devCtx *     devCtx;

    devCtx = getDevCtx(
        ctx);

    LOG_DBG("CFG: initial delay is %d", devCtx->cfg.delay);

    *delay = devCtx->cfg.delay;
}

static int32_t cfgChnTransferModeSet(
    struct rtdm_dev_context * ctx,
    enum xspiTransferMode transferMode) {

    struct devCtx *     devCtx;
    rtdm_lockctx_t      lockCtx;

    LOG_DBG("CFG: set transfer mode to %d", transferMode);

    if (!CFG_ARG_IS_VALID(transferMode, XSPI_TRANSFER_MODE_TX_AND_RX, XSPI_TRANSFER_MODE_TX_ONLY)) {

        return (-EINVAL);
    }
    devCtx = getDevCtx(
        ctx);
    rtdm_lock_get_irqsave(&devCtx->lock, lockCtx);

    if (XSPI_ACTIVITY_RUNNIG == devCtx->activity) {
        rtdm_lock_put_irqrestore(&devCtx->lock, lockCtx);

        return (-EAGAIN);
    }
    devCtx->chn[devCtx->cfg.chn].cfg.transferMode = transferMode;
    lldChnTransferModeSet(
        ctx->device,
        devCtx->cfg.chn,
        (uint32_t)transferMode);
    rtdm_lock_put_irqrestore(&devCtx->lock, lockCtx);

    return (0);
}

static void cfgChnTransferModeGet(
    struct rtdm_dev_context * ctx,
    enum xspiTransferMode * transferMode) {

    struct devCtx *     devCtx;

    devCtx = getDevCtx(
        ctx);

    LOG_DBG("CFG: transfer mode is %d", devCtx->chn[devCtx->cfg.chn].cfg.transferMode);

    *transferMode = devCtx->chn[devCtx->cfg.chn].cfg.transferMode;
}

static int32_t cfgChnPinLayoutSet(
    struct rtdm_dev_context * ctx,
    enum xspiPinLayout  pinLayout) {

    struct devCtx *     devCtx;
    rtdm_lockctx_t      lockCtx;

    LOG_DBG("CFG: set pin layout to %d", pinLayout);

    if (!CFG_ARG_IS_VALID(pinLayout, XSPI_PIN_LAYOUT_TX_RX, XSPI_PIN_LAYOUT_RX_TX)) {

        return (-EINVAL);
    }
    devCtx = getDevCtx(
        ctx);
    rtdm_lock_get_irqsave(&devCtx->lock, lockCtx);

    if (XSPI_ACTIVITY_RUNNIG == devCtx->activity) {
        rtdm_lock_put_irqrestore(&devCtx->lock, lockCtx);

        return (-EAGAIN);
    }
    devCtx->chn[devCtx->cfg.chn].cfg.pinLayout = pinLayout;
    lldChnPinLayoutSet(
        ctx->device,
        devCtx->cfg.chn,
        (uint32_t)pinLayout);
    rtdm_lock_put_irqrestore(&devCtx->lock, lockCtx);

    return (0);
}

static void cfgChnPinLayoutGet(
    struct rtdm_dev_context * ctx,
    enum xspiPinLayout * pinLayout) {

    struct devCtx *     devCtx;

    devCtx = getDevCtx(
        ctx);

    LOG_DBG("CFG: pin layout is %d", devCtx->chn[devCtx->cfg.chn].cfg.pinLayout);

    *pinLayout = devCtx->chn[devCtx->cfg.chn].cfg.pinLayout;
}

static int32_t cfgChnWordLengthSet(
    struct rtdm_dev_context * ctx,
    uint32_t            length) {

    struct devCtx *     devCtx;
    rtdm_lockctx_t      lockCtx;

    LOG_DBG("CFG: set word length to %d", length);

    if (!CFG_ARG_IS_VALID(length, 4u, 32u)) {

        return (-EINVAL);
    }
    devCtx = getDevCtx(
        ctx);
    rtdm_lock_get_irqsave(&devCtx->lock, lockCtx);

    if (XSPI_ACTIVITY_RUNNIG == devCtx->activity) {
        rtdm_lock_put_irqrestore(&devCtx->lock, lockCtx);

        return (-EAGAIN);
    }
    devCtx->chn[devCtx->cfg.chn].cfg.wordLength = length;
    lldChnCsDelaySet(
        ctx->device,
        devCtx->cfg.chn,
        (uint32_t)length);
    rtdm_lock_put_irqrestore(&devCtx->lock, lockCtx);

    return (0);
}

static void cfgChnWordLengthGet(
    struct rtdm_dev_context * ctx,
    uint32_t *          length) {

    struct devCtx *     devCtx;

    devCtx = getDevCtx(
        ctx);

    LOG_DBG("CFG: word length is %d", devCtx->chn[devCtx->cfg.chn].cfg.wordLength);

    *length = devCtx->chn[devCtx->cfg.chn].cfg.wordLength;
}

static int32_t cfgChnCsDelaySet(
    struct rtdm_dev_context * ctx,
    enum xspiCsDelay    delay) {

    struct devCtx *     devCtx;
    rtdm_lockctx_t      lockCtx;

    LOG_DBG("CFG: set CS delay to %d", delay);

    if (!CFG_ARG_IS_VALID(delay, XSPI_CS_DELAY_0_5, XSPI_CS_DELAY_3_5)) {

        return (-EINVAL);
    }
    devCtx = getDevCtx(
        ctx);
    rtdm_lock_get_irqsave(&devCtx->lock, lockCtx);

    if (XSPI_ACTIVITY_RUNNIG == devCtx->activity) {
        rtdm_lock_put_irqrestore(&devCtx->lock, lockCtx);

        return (-EAGAIN);
    }
    devCtx->chn[devCtx->cfg.chn].cfg.csDelay = delay;
    lldChnCsDelaySet(
        ctx->device,
        devCtx->cfg.chn,
        (uint32_t)delay);
    rtdm_lock_put_irqrestore(&devCtx->lock, lockCtx);

    return (0);
}

static void cfgChnCsDelayGet(
    struct rtdm_dev_context * ctx,
    enum xspiCsDelay *  delay) {

    struct devCtx *     devCtx;

    devCtx = getDevCtx(
        ctx);

    LOG_DBG("CFG: CS delay is %d", devCtx->chn[devCtx->cfg.chn].cfg.csDelay);

    *delay = devCtx->chn[devCtx->cfg.chn].cfg.csDelay;
}

static int32_t cfgChnCsPolaritySet(
    struct rtdm_dev_context * ctx,
    enum xspiCsPolarity csPolarity) {

    struct devCtx *     devCtx;
    rtdm_lockctx_t      lockCtx;

    LOG_DBG("CFG: set CS polarity to %d", csPolarity);

    if (!CFG_ARG_IS_VALID(csPolarity, XSPI_CS_POLARITY_ACTIVE_HIGH, XSPI_CS_POLAROTY_ACTIVE_LOW)) {

        return (-EINVAL);
    }
    devCtx = getDevCtx(
        ctx);
    rtdm_lock_get_irqsave(&devCtx->lock, lockCtx);

    if (XSPI_ACTIVITY_RUNNIG == devCtx->activity) {
        rtdm_lock_put_irqrestore(&devCtx->lock, lockCtx);

        return (-EAGAIN);
    }
    devCtx->chn[devCtx->cfg.chn].cfg.wordLength = csPolarity;
    lldChnCsPolaritySet(
        ctx->device,
        devCtx->cfg.chn,
        (uint32_t)csPolarity);
    rtdm_lock_put_irqrestore(&devCtx->lock, lockCtx);

    return (0);
}

static void cfgChnCsPolarityGet(
    struct rtdm_dev_context * ctx,
    enum xspiCsPolarity * csPolarity) {

    struct devCtx *     devCtx;

    devCtx = getDevCtx(
        ctx);

    LOG_DBG("CFG: CS polarity is %d", devCtx->chn[devCtx->cfg.chn].cfg.csPolarity);

    *csPolarity =  devCtx->chn[devCtx->cfg.chn].cfg.csPolarity;
}

static int32_t cfgChnCsStateSet(
    struct rtdm_dev_context * ctx,
    enum xspiCsState    state) {

    struct devCtx *     devCtx;
    rtdm_lockctx_t      lockCtx;
    int32_t             ret;

    LOG_DBG("CFG: set CS state to %d", state);

    if (!CFG_ARG_IS_VALID(state, XSPI_CS_STATE_INACTIVE, XSPI_CS_STATE_ACTIVE)) {

        return (-EINVAL);
    }
    devCtx = getDevCtx(
        ctx);
    rtdm_lock_get_irqsave(&devCtx->lock, lockCtx);

    if (XSPI_ACTIVITY_RUNNIG == devCtx->activity) {
        rtdm_lock_put_irqrestore(&devCtx->lock, lockCtx);

        return (-EAGAIN);
    }
    devCtx->chn[devCtx->cfg.chn].cfg.csState = state;
    ret = lldChnCsStateSet(
        ctx->device,
        devCtx->cfg.chn,
        (uint32_t)state);
    rtdm_lock_put_irqrestore(&devCtx->lock, lockCtx);

    return (ret);
}

static void cfgChnCsStateGet(
    struct rtdm_dev_context * ctx,
    enum xspiCsState *  state) {

    struct devCtx *     devCtx;

    devCtx = getDevCtx(
        ctx);

    LOG_DBG("CFG: CS state is %d", devCtx->chn[devCtx->cfg.chn].cfg.csState);

    *state = devCtx->chn[devCtx->cfg.chn].cfg.csState;
}

/*
 * Rest
 */

/******************************************************************************
 * DMA MODE 0
 ******************************************************************************/
#if (0u == CFG_DMA_MODE)

static int handleOpen(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    int                 oflag) {

    int                 retval;

    retval = 0;

    return (retval);
}

static int handleClose(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr) {

    int                 retval;

    retval = 0;

    return (retval);
}

static int handleIOctl(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    unsigned int        req,
    void __user *       arg) {

    int                 retval;

    retval = 0;

    switch (req) {
/*-- XSPI_IOC_SET_CURRENT_CHN ------------------------------------------------*/
        case XSPI_IOC_SET_CURRENT_CHN : {
            retval = (int)cfgChnSet(
                ctx,
                (enum xspiChn)arg);

            break;
        }

/*-- XSPI_IOC_GET_CURRENT_CHN ------------------------------------------------*/
        case XSPI_IOC_GET_CURRENT_CHN : {
            enum xspiChn chn;

            cfgChnGet(
                ctx,
                &chn);

            if (NULL != usr) {
                retval = rtdm_safe_copy_to_user(
                    usr,
                    arg,
                    &chn,
                    sizeof(int));
            } else {
                *(int *)arg = (int)chn;
            }

            break;
        }

/*-- XSPI_IOC_SET_FIFO_MODE --------------------------------------------------*/
        case XSPI_IOC_SET_FIFO_CHN : {
            retval = (int)cfgFIFOChnSet(
                ctx,
                (enum xspiFifoChn)arg);

            break;
        }

/*-- XSPI_IOC_GET_FIFO_MODE --------------------------------------------------*/
        case XSPI_IOC_GET_FIFO_CHN : {
            enum xspiFifoChn fifoChn;

            cfgFIFOChnGet(
                ctx,
                &fifoChn);

            if (NULL != usr) {
                retval = rtdm_safe_copy_to_user(
                    usr,
                    arg,
                    &fifoChn,
                    sizeof(int));
            } else {
                *(int *)arg = (int)fifoChn;
            }

            break;
        }

/*-- XSPI_IOC_SET_CS_MODE ----------------------------------------------------*/
        case XSPI_IOC_SET_CS_MODE : {
            retval = (int)cfgCsModeSet(
                ctx,
                (enum xspiCsMode)arg);

            break;
        }

/*-- XSPI_IOC_GET_CS_MODE ----------------------------------------------------*/
        case XSPI_IOC_GET_CS_MODE : {
            enum xspiCsMode csMode;

            cfgCsModeGet(
                ctx,
                &csMode);

            if (NULL != usr) {
                retval = rtdm_safe_copy_to_user(
                    usr,
                    arg,
                    &csMode,
                    sizeof(int));
            } else {
                *(int *)arg = (int)csMode;
            }

            break;
        }

/*-- XSPI_IOC_SET_MODE -------------------------------------------------------*/
        case XSPI_IOC_SET_MODE : {
            retval = (int)cfgModeSet(
                ctx,
                (enum xspiMode)arg);

            break;
        }

/*-- XSPI_IOC_GET_MODE -------------------------------------------------------*/
        case XSPI_IOC_GET_MODE : {
            enum xspiMode mode;

            cfgModeGet(
                ctx,
                &mode);

            if (NULL != usr) {
                retval = rtdm_safe_copy_to_user(
                    usr,
                    arg,
                    &mode,
                    sizeof(int));
            } else {
                *(int *)arg = (int)mode;
            }

            break;
        }

/*-- XSPI_IOC_SET_CHANNEL_MODE -----------------------------------------------*/
        case XSPI_IOC_SET_CHANNEL_MODE : {
            retval = (int)cfgChannelModeSet(
                ctx,
                (enum xspiChannelMode)arg);

            break;
        }

/*-- XSPI_IOC_GET_CHANNEL_MODE -----------------------------------------------*/
        case XSPI_IOC_GET_CHANNEL_MODE : {
            enum xspiChannelMode channelMode;

            cfgChannelModeGet(
                ctx,
                &channelMode);

            if (NULL != usr) {
                retval = rtdm_safe_copy_to_user(
                    usr,
                    arg,
                    &channelMode,
                    sizeof(int));
            } else {
                *(int *)arg = (int)channelMode;
            }

            break;
        }

/*-- XSPI_IOC_SET_INITIAL_DELAY ----------------------------------------------*/
        case XSPI_IOC_SET_INITIAL_DELAY : {
            retval = (int)cfgInitialDelaySet(
                ctx,
                (enum xspiInitialDelay)arg);

            break;
        }

/*-- XSPI_IOC_GET_INITIAL_DELAY ----------------------------------------------*/
        case XSPI_IOC_GET_INITIAL_DELAY : {
            enum xspiInitialDelay initialDelay;

            cfgInitialDelayGet(
                ctx,
                &initialDelay);

            if (NULL != usr) {
                retval = rtdm_safe_copy_to_user(
                    usr,
                    arg,
                    &initialDelay,
                    sizeof(int));
            } else {
                *(int *)arg = (int)initialDelay;
            }

            break;
        }

/*-- XSPI_IOC_SET_TRANSFER_MODE ----------------------------------------------*/
        case XSPI_IOC_SET_TRANSFER_MODE : {
            retval = (int)cfgChnTransferModeSet(
                ctx,
                (enum xspiTransferMode)arg);

            break;
        }

/*-- XSPI_IOC_GET_TRANSFER_MODE ----------------------------------------------*/
        case XSPI_IOC_GET_TRANSFER_MODE : {
            enum xspiTransferMode transferMode;

            cfgChnTransferModeGet(
                ctx,
                &transferMode);

            if (NULL != usr) {
                retval = rtdm_safe_copy_to_user(
                    usr,
                    arg,
                    &transferMode,
                    sizeof(int));
            } else {
                *(int *)arg = (int)transferMode;
            }

            break;
        }

/*-- XSPI_IOC_SET_PIN_LAYOUT -------------------------------------------------*/
        case XSPI_IOC_SET_PIN_LAYOUT : {
            retval = (int)cfgChnPinLayoutSet(
                ctx,
                (enum xspiPinLayout)arg);

            break;
        }

/*-- XSPI_IOC_GET_PIN_LAYOUT -------------------------------------------------*/
        case XSPI_IOC_GET_PIN_LAYOUT : {
            enum xspiPinLayout pinLayout;

            cfgChnPinLayoutGet(
                ctx,
                &pinLayout);

            if (NULL != usr) {
                retval = rtdm_safe_copy_to_user(
                    usr,
                    arg,
                    &pinLayout,
                    sizeof(int));
            } else {
                *(int *)arg = (int)pinLayout;
            }

            break;
        }

/*-- XSPI_IOC_SET_WORD_LENGTH ------------------------------------------------*/
        case XSPI_IOC_SET_WORD_LENGTH : {
            retval = (int)cfgChnWordLengthSet(
                ctx,
                (uint32_t)arg);

            break;
        }

/*-- XSPI_IOC_GET_WORD_LENGTH ------------------------------------------------*/
        case XSPI_IOC_GET_WORD_LENGTH : {
            uint32_t    wordLength;

            cfgChnWordLengthGet(
                ctx,
                &wordLength);

            if (NULL != usr) {
                retval = rtdm_safe_copy_to_user(
                    usr,
                    arg,
                    &wordLength,
                    sizeof(int));
            } else {
                *(int *)arg = (int)wordLength;
            }

            break;
        }

/*-- XSPI_IOC_SET_CS_DELAY ---------------------------------------------------*/
        case XSPI_IOC_SET_CS_DELAY : {
            retval = (int)cfgChnCsDelaySet(
                ctx,
                (enum xspiCsDelay)arg);

            break;
        }

/*-- XSPI_IOC_GET_CS_DELAY ---------------------------------------------------*/
        case XSPI_IOC_GET_CS_DELAY : {
            enum xspiCsDelay csDelay;

            cfgChnCsDelayGet(
                ctx,
                &csDelay);

            if (NULL != usr) {
                retval = rtdm_safe_copy_to_user(
                    usr,
                    arg,
                    &csDelay,
                    sizeof(int));
            } else {
                *(int *)arg = (int)csDelay;
            }

            break;
        }

/*-- XSPI_IOC_SET_CS_POLARITY ------------------------------------------------*/
        case XSPI_IOC_SET_CS_POLARITY : {
            retval = (int)cfgChnCsPolaritySet(
                ctx,
                (enum xspiCsPolarity)arg);

            break;
        }

/*-- XSPI_IOC_GET_CS_POLARITY ------------------------------------------------*/
        case XSPI_IOC_GET_CS_POLARITY : {
            enum xspiCsPolarity csPolarity;

            cfgChnCsPolarityGet(
                ctx,
                &csPolarity);

            if (NULL != usr) {
                retval = rtdm_safe_copy_to_user(
                    usr,
                    arg,
                    &csPolarity,
                    sizeof(int));
            } else {
                *(int *)arg = (int)csPolarity;
            }

            break;
        }

/*-- XSPI_IOC_SET_CS_STATE ---------------------------------------------------*/
        case XSPI_IOC_SET_CS_STATE : {
            retval = (int)cfgChnCsStateSet(
                ctx,
                (enum xspiCsState)arg);

            break;
        }

/*-- XSPI_IOC_GET_CS_STATE ---------------------------------------------------*/
        case XSPI_IOC_GET_CS_STATE : {
            enum xspiCsState csState;

            cfgChnCsStateGet(
                ctx,
                &csState);

            if (NULL != usr) {
                retval = rtdm_safe_copy_to_user(
                    usr,
                    arg,
                    &csState,
                    sizeof(int));
            } else {
                *(int *)arg = (int)csState;
            }

            break;
        }

/*-- XSPI_IOC_SET_CLOCK_FREQ -------------------------------------------------*/
        case XSPI_IOC_SET_CLOCK_FREQ : {

        }

/*-- XSPI_IOC_GET_CLOCK_FREQ -------------------------------------------------*/
        case XSPI_IOC_GET_CLOCK_FREQ : {

        }

/*-- Unhandled request -------------------------------------------------------*/
        default : {
            LOG_DBG("IOC: unknown request (%d) received", req);
            retval = -EINVAL;
        }
    }

    if (0 != retval) {
        LOG_INFO("IOC: failed to execute IO request, err: %d", -retval);
    }

    return (retval);
}

static ssize_t handleRd(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    void *              dst,
    size_t              bytes) {

    ssize_t             read;

    read = 0;

    return (read);
}

static ssize_t handleWr(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    const void *        src,
    size_t              bytes) {

    ssize_t             write;

    write = 0;

    return (write);
}

/******************************************************************************
 * DMA MODE 1
 ******************************************************************************/
#elif (1u == CFG_DMA_MODE)

static int handleOpen(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    int                 oflag) {

    int                 retval;

    retval = 0;

    return (retval);
}

static int handleClose(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr) {

    int                 retval;

    retval = 0;

    return (retval);
}

static int handleIOctl(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    unsigned int        req,
    void __user *       arg) {

    int                 retval;

    retval = 0;

    return (retval);
}

static ssize_t handleRd(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    void *              dst,
    size_t              bytes) {

    ssize_t             read;

    read = 0;

    return (read);
}

static ssize_t handleWr(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    const void *        src,
    size_t              bytes) {

    ssize_t             write;

    write = 0;

    return (write);
}

#elif (2u == CFG_DMA_MODE)

static int handleOpen(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    int                 oflag) {

    int                 retval;

    retval = 0;

    return (retval);
}

static int handleClose(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr) {

    int                 retval;

    retval = 0;

    return (retval);
}

static int handleIOctl(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    unsigned int        req,
    void __user *       arg) {

    int                 retval;

    retval = 0;

    return (retval);
}

static ssize_t handleRd(
    struct rtdm_dev_context * ctx,
    rtdm_user_info_t *  usr,
    void *              dst,
    size_t              bytes) {

    ssize_t             read;

    read = 0;

    return (read);
}

static ssize_t handleWr(
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

    for (i = 0u; i < CFG_MAX_DEVICES; i++) {

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
            portDevEnable(
                dev);
            LOG_INFO("initializing SPI device: %d", i);
            ret = lldDevInit(
                dev);

            if (0 != ret) {
                LOG_ERR("failed to initialize device: %d, err: %d", i, -ret);
                portDevDisable(
                    dev);
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
                portDevDisable(
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

    for (i = 0u; i < CFG_MAX_DEVICES; i++) {
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
            portDevDisable(
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
