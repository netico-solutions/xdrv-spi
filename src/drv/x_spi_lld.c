/*
 * This file is part of x_spi
 *
 * Copyright (C) 2011, 2012 - Nenad Radulovic
 *
 * x_spi is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * x_spi is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * x_spi; if not, write to the Free Software Foundation, Inc., 51 Franklin St,
 * Fifth Floor, Boston, MA  02110-1301  USA
 *
 * web site:    http://blueskynet.dyndns-server.com
 * e-mail  :    blueskyniss@gmail.com
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Low Level Driver implementation
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include "linux/io.h"

#include "port/port.h"
#include "drv/x_spi_lld.h"
#include "drv/x_spi.h"
#include "log/log.h"
#include "dbg/dbg.h"

/*=========================================================  LOCAL MACRO's  ==*/

/*-- Common registers bit definitions ----------------------------------------*/
#define MCSPI_REVISION_X_MAJOR_Pos      (8u)
#define MCSPI_REVISION_X_MAJOR_Mask     (0x03u << MCSPI_REVISION_X_MAJOR_Pos)
#define MCSPI_REVISION_Y_MINOR_Pos      (0u)
#define MCSPI_REVISION_Y_MINOR_Mask     (0x1fu << MCSPI_REVISION_Y_MINOR_Pos)
#define MCSPI_SYSCONFIG_SOFTRESET_Pos   (1u)
#define MCSPI_SYSCONFIG_SOFTRESET_Mask  (0x01u << MCSPI_SYSCONFIG_SOFTRESET_Pos)
#define MCSPI_SYSSTATUS_RESETDONE_Pos   (0u)
#define MCSPI_SYSSTATUS_RESETDONE_Mask  (0x01u << MCSPI_SYSSTATUS_RESETDONE_Pos)

/*-- Channel registers bit definitions ---------------------------------------*/
#define MCSPI_MODULCTRL_PIN32_Pos       (1u)
#define MCSPI_MODULCTRL_PIN32_Mask      (0x01u << MCSPI_MODULCTRL_PIN32_Pos)

#define MCSPI_CH_CONF_CLKG_Pos          (29u)
#define MCSPI_CH_CONF_CLKG_Mask         (0x01u << MCSPI_CH_CONF_CLKG_Pos)
#define MCSPI_CH_CONF_FFER_Pos          (28u)
#define MCSPI_CH_CONF_FFER_Mask         (0x01u << MCSPI_CH_CONF_FFER_Pos)
#define MCSPI_CH_CONF_FFEW_Pos          (27u)
#define MCSPI_CH_CONF_FFEW_Mask         (0x01u << MCSPI_CH_CONF_FFEW_Pos)
#define MCSPI_CH_CONF_TCS_Pos           (25u)
#define MCSPI_CH_CONF_TCS_Mask          (0x03u << MCSPI_CH_CONF_TCS_Pos)
#define MCSPI_CH_CONF_SBPOL_Pos         (24u)
#define MCSPI_CH_CONF_SBPOL_Mask        (0x01u << MCSPI_CH_CONF_SBPOL_Pos)
#define MCSPI_CH_CONF_SBE_Pos           (23u)
#define MCSPI_CH_CONF_SBE_Mask          (0x01u << MCSPI_CH_CONF_SBE_Pos)
#define MCSPI_CH_CONF_SPIENSLV_Pos      (21u)
#define MCSPI_CH_CONF_SPIENSLV_Mask     (0x03u << MCSPI_CH_CONF_SPIENSLV_Pos)
#define MCSPI_CH_CONF_FORCE_Pos         (20u)
#define MCSPI_CH_CONF_FORCE_Mask        (0x01u << MCSPI_CH_CONF_FORCE_Pos)
#define MCSPI_CH_CONF_TURBO_Pos         (19u)
#define MCSPI_CH_CONF_TURBO_Mask        (0x01u << MCSPI_CH_CONF_TURBO_Pos)
#define MCSPI_CH_CONF_IS_Pos            (18u)
#define MCSPI_CH_CONF_IS_Mask           (0x01u << MCSPI_CH_CONF_IS_Pos)
#define MCSPI_CH_CONF_DPE1_Pos          (17u)
#define MCSPI_CH_CONF_DPE1_Mask         (0x01u << MCSPI_CH_CONF_DPE1_Pos)
#define MCSPI_CH_CONF_DPE0_Pos          (16u)
#define MCSPI_CH_CONF_DPE0_Mask         (0x01u << MCSPI_CH_CONF_DPE0_Pos)
#define MCSPI_CH_CONF_DMAR_Pos          (15u)
#define MCSPI_CH_CONF_DMAR_Mask         (0x01u << MCSPI_CH_CONF_DMAR_Pos)
#define MCSPI_CH_CONF_DMAW_Pos          (14u)
#define MCSPI_CH_CONF_DMAW_Mask         (0x01u << MCSPI_CH_CONF_DMAW_Pos)
#define MCSPI_CH_CONF_TRM_Pos           (13u)
#define MCSPI_CH_CONF_TRM_Mask          (0x01u << MCSPI_CH_CONF_TRM_Pos)
#define MCSPI_CH_CONF_WL_Pos            (7u)
#define MCSPI_CH_CONF_WL_Mask           (0x1fu << MCSPI_CH_CONF_WL_Pos)
#define MCSPI_CH_CONF_EPOL_Pos          (6u)
#define MCSPI_CH_CONF_EPOL_Mask         (0x01u << MCSPI_CH_CONF_EPOL_Pos)
#define MCSPI_CH_CONF_CLKD_Pos          (2u)
#define MCSPI_CH_CONF_CLKD_Mask         (0x0fu << MCSPI_CH_CONF_CLKD_Pos)
#define MCSPI_CH_CONF_POL_Pos           (1u)
#define MCSPI_CH_CONF_POL_Mask          (0x01u << MCSPI_CH_CONF_POL_Pos)
#define MCSPI_CH_CONF_PHA_Pos           (0u)
#define MCSPI_CH_CONF_PHA_Mask          (0x01u << MCSPI_CH_CONF_PHA_Pos)

/*======================================================  LOCAL DATA TYPES  ==*/

enum mcspiRegs {
    MCSPI_REVISION      = 0x000u,
    MCSPI_SYSCONFIG     = 0x110u,
    MCSPI_SYSSTATUS     = 0x114u,
    MCSPI_IRQSTATUS     = 0x118u,
    MCSPI_IRQENABLE     = 0x11cu,
    MCSPI_SYST          = 0x124u,
    MCSPI_MODULCTRL     = 0x128u,
    MCSPI_CHANNEL_BASE  = 0x12cu,
    MCSPI_XFERLEVEL     = 0x17cu,
    MCSPI_DAFTX         = 0x180u,
    MCSPI_DAFRX         = 0x1a0u
};

enum mcspiChnRegs {
    MCSPI_CH_CONF       = 0x00u,
    MCSPI_CH_STAT       = 0x04u,
    MCSPI_CH_CTRL       = 0x08u,
    MCSPI_CH_TX         = 0x0cu,
    MCSPI_CH_RX         = 0x10u,
    MCSPI_CHANNEL_SIZE  = 0x14u
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

static inline void regWrite(
    volatile uint8_t *  io,
    enum mcspiRegs      reg,
    uint32_t            val);

static inline uint32_t regRead(
    volatile uint8_t *  io,
    enum mcspiRegs      reg);

static inline void regChnWrite(
    volatile uint8_t *  io,
    uint32_t            chn,
    enum mcspiChnRegs   reg,
    uint32_t            val);

static inline uint32_t regChnRead(
    volatile uint8_t *  io,
    uint32_t            chn,
    enum mcspiChnRegs   reg);

static struct devData * getDevData(
    struct rtdm_device * dev);

static int32_t shadowCreate(
    struct rtdm_device * dev);

static void shadowUpdate(
    struct rtdm_device * dev);

static inline void shadowWrite(
    struct rtdm_device * dev,
    enum mcspiRegs      reg,
    uint32_t            val);

static inline uint32_t shadowRead(
    struct rtdm_device * dev,
    enum mcspiRegs      reg);

static inline uint32_t shadowReadUpdate(
    struct rtdm_device * dev,
    enum mcspiRegs      reg);

static inline void shadowChnWrite(
    struct rtdm_device * dev,
    uint32_t            chn,
    enum mcspiChnRegs   reg,
    uint32_t            val);

static inline uint32_t shadowChnRead(
    struct rtdm_device * dev,
    uint32_t            chn,
    enum mcspiChnRegs   reg);

static inline uint32_t shadowChnReadUpdate(
    struct rtdm_device * dev,
    uint32_t            chn,
    enum mcspiChnRegs   reg);

/*=======================================================  LOCAL VARIABLES  ==*/

DECL_MODULE_INFO("x_spi_lld", "Low-level device driver", DEF_DRV_AUTHOR);

/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

static inline void regWrite(
    volatile uint8_t *  io,
    enum mcspiRegs      reg,
    uint32_t            val) {

    LOG_DBG("UART wr: %p, %x = %x", io, reg, val);

    iowrite32(val, &io[reg]);
}

static inline uint32_t regRead(
    volatile uint8_t *  io,
    enum mcspiRegs      reg) {

    uint32_t            ret;

    ret = ioread32(&io[reg]);

    LOG_DBG("UART rd: %p, %x : %x", io, reg, ret);

    return (ret);
}

static inline void regChnWrite(
    volatile uint8_t *  io,
    uint32_t            chn,
    enum mcspiChnRegs   reg,
    uint32_t            val) {

    regWrite(io, MCSPI_CHANNEL_BASE + (chn * MCSPI_CHANNEL_SIZE) + reg, val);
}

static inline uint32_t regChnRead(
    volatile uint8_t *  io,
    uint32_t            chn,
    enum mcspiChnRegs   reg) {

    uint32_t            ret;

    ret = regRead(io, MCSPI_CHANNEL_BASE + (chn * MCSPI_CHANNEL_SIZE) + reg);

    return (ret);
}

static struct devData * getDevData(
    struct rtdm_device * dev) {

    return ((struct devData *)dev->device_data);
}

static int32_t shadowCreate(
    struct rtdm_device * dev) {

    struct devData *    devData;

    devData = getDevData(
        dev);
    devData->shadow = kmalloc(
        devData->addr.size,
        GFP_KERNEL);

    if (NULL == devData->shadow) {
        LOG_DBG("failed to initialize local registry shadow, err: %d", ENOMEM);

        return (-ENOMEM);
    }
    shadowUpdate(
        dev);

    return (0);
}

static void shadowUpdate(
    struct rtdm_device * dev) {

    struct devData *    devData;
    volatile uint8_t *  io;

    devData = getDevData(
        dev);
    io = lldRemapGet(
        dev);
    memcpy_fromio(
        devData->shadow,
        io,
        devData->addr.size);
}

static inline void shadowWrite(
    struct rtdm_device * dev,
    enum mcspiRegs      reg,
    uint32_t            val) {

    volatile uint8_t *  io;
    struct devData *    devData;

    io = lldRemapGet(
        dev);
    devData = getDevData(
        dev);
    *((uint32_t *)&devData->shadow[reg]) = val;
    regWrite(
        io,
        reg,
        val);
}

static inline uint32_t shadowRead(
    struct rtdm_device * dev,
    enum mcspiRegs      reg) {

    uint32_t            ret;
    struct devData *    devData;

    devData = getDevData(
        dev);
    ret = *((uint32_t *)&devData->shadow[reg]);
    LOG_DBG("UART rd shadow: %x : %x", reg, ret);

    return (ret);
}

static inline uint32_t shadowReadUpdate(
    struct rtdm_device * dev,
    enum mcspiRegs      reg) {

    uint32_t            ret;
    volatile uint8_t *  io;
    struct devData *    devData;

    io = lldRemapGet(
        dev);
    devData = getDevData(
        dev);
    ret = regRead(
        io,
        reg);
    *((uint32_t *)&devData->shadow[reg]) = ret;

    return (ret);
}

static inline void shadowChnWrite(
    struct rtdm_device * dev,
    uint32_t            chn,
    enum mcspiChnRegs   reg,
    uint32_t            val) {

    shadowWrite(
        dev,
        MCSPI_CHANNEL_BASE + (chn * MCSPI_CHANNEL_SIZE) + reg,
        val);
}

static inline uint32_t shadowChnRead(
    struct rtdm_device * dev,
    uint32_t            chn,
    enum mcspiChnRegs   reg) {

    uint32_t            ret;

    ret = shadowRead(
        dev,
        MCSPI_CHANNEL_BASE + (chn * MCSPI_CHANNEL_SIZE) + reg);

    return (ret);
}

static inline uint32_t shadowChnReadUpdate(
    struct rtdm_device * dev,
    uint32_t            chn,
    enum mcspiChnRegs   reg) {

    uint32_t            ret;

    ret = shadowReadUpdate(
        dev,
        MCSPI_CHANNEL_BASE + (chn * MCSPI_CHANNEL_SIZE) + reg);

    return (ret);
}

/*===================================  GLOBAL PRIVATE FUNCTION DEFINITIONS  ==*/
/*====================================  GLOBAL PUBLIC FUNCTION DEFINITIONS  ==*/

int32_t lldDevInit(
    struct rtdm_device * dev) {

    int32_t             ret;
    uint32_t            revision;

    ret = shadowCreate(
        dev);

    if (0 != ret) {

        return (ret);
    }
    lldDevReset(
        dev);
    revision = shadowRead(
        dev,
        MCSPI_REVISION);                                                        /* Read revision info                                       */
    LOG_INFO("hardware version: %d.%d",
        (revision & MCSPI_REVISION_X_MAJOR_Mask) >> MCSPI_REVISION_X_MAJOR_Pos,
        (revision & MCSPI_REVISION_Y_MINOR_Mask) >> MCSPI_REVISION_Y_MINOR_Pos);

    return (ret);
}

void lldDevTerm(
    struct rtdm_device * dev) {

    lldDevReset(
        dev);
}

volatile uint8_t * lldRemapGet(
    struct rtdm_device * dev) {

    struct devData *    devData;

    devData = getDevData(
        dev);

    return (devData->addr.remap);
}

void lldDevReset(
    struct rtdm_device * dev) {

    volatile uint8_t *  io;
    uint32_t            sysconfig;

    io = lldRemapGet(
        dev);

    sysconfig = regRead(
        io,
        MCSPI_SYSCONFIG);
    regWrite(
        io,
        MCSPI_SYSCONFIG,
        sysconfig | MCSPI_SYSCONFIG_SOFTRESET_Mask);                            /* Reset device module                                      */

    while (0u == (regRead(io, MCSPI_SYSSTATUS) & MCSPI_SYSSTATUS_RESETDONE_Mask));  /* Wait a few cycles for reset procedure                */
    shadowUpdate(
        dev);
}

void lldFIFOChnEnable(
    struct rtdm_device * dev,
    uint32_t            chn) {

    uint32_t            reg;

    ES_DBG_API_REQUIRE(ES_DBG_USAGE_FAILURE, TRUE == portChnIsOnline(dev, chn));

    reg = shadowChnRead(
        dev,
        chn,
        MCSPI_CH_CONF);
    reg |= MCSPI_CH_CONF_FFER_Mask | MCSPI_CH_CONF_FFEW_Mask;
    shadowChnWrite(
        dev,
        chn,
        MCSPI_CH_CONF,
        reg);
}

void lldFIFOChnDisable(
    struct rtdm_device * dev,
    uint32_t            chn) {

    uint32_t            reg;

    ES_DBG_API_REQUIRE(ES_DBG_USAGE_FAILURE, TRUE == portChnIsOnline(dev, chn));

    reg = shadowChnRead(
        dev,
        chn,
        MCSPI_CH_CONF);
    reg &= ~(MCSPI_CH_CONF_FFER_Mask | MCSPI_CH_CONF_FFEW_Mask);
    shadowChnWrite(
        dev,
        chn,
        MCSPI_CH_CONF,
        reg);
}

void lldCsModeSet(
    struct rtdm_device * dev,
    int32_t             chn,
    uint32_t            mode) {

    uint32_t            reg;

    reg = shadowRead(
        dev,
        MCSPI_MODULCTRL);
    reg &= ~MCSPI_MODULCTRL_PIN32_Mask;
    reg |= mode & MCSPI_MODULCTRL_PIN32_Mask;
    shadowWrite(
        dev,
        MCSPI_MODULCTRL,
        reg);
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of x_spi_lld.c
 ******************************************************************************/
