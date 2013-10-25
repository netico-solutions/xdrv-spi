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
 * @brief       Platform port implementation
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <plat/omap_device.h>
#include <plat/mcspi.h>
#include <mach/edma.h>

#include "drv/x_spi.h"
#include "port/port.h"
#include "log/log.h"
#include "plat_omap2.h"

/*=========================================================  LOCAL MACRO's  ==*/

#define DEF_HWMOD_NAME                  "spi"
#define DEF_HWMOD_CLASS_NAME            "omap2_mcspi"
#define DEF_HWMOD_DMA_TX                "tx"
#define DEF_HWMOD_DMA_RX                "rx"

/*======================================================  LOCAL DATA TYPES  ==*/

struct dmaData {
    struct {
        int32_t             chn;
        int32_t             sync;
    }                   tx, rx;
};

struct privDevData {
    struct devData      public;
    struct platform_device * pDev;
#if (0u != CFG_DMA_MODE)
    struct dmaData *    dma;                                                    /* Array of DMA channels                                    */
#endif
    uint32_t            online;                                                 /* Number of CS signals                                     */
    bool_T              devBuilt;
    bool_T              devActv;
};

/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/

/**@brief       Get device data from RT device descriptor
 */
static inline struct privDevData * getPrivDevData(
    struct rtdm_device * dev);

/**@brief       Get device Id from RT device data
 */
static inline uint32_t getDevId(
    struct rtdm_device * dev);

static int32_t resRequest(
    struct privDevData *    devData);

static void resRelease(
    struct privDevData *    devData);

/*=======================================================  LOCAL VARIABLES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/

static inline struct privDevData * getPrivDevData(
    struct rtdm_device * dev) {

    return ((struct privDevData *)dev->device_data);
}

static inline uint32_t getDevId(
    struct rtdm_device * dev) {

    struct privDevData *    devData;

    devData = getPrivDevData(
        dev);

    return ((uint32_t)devData->pDev->id);
}

static int32_t resRequest(
    struct privDevData *    devData) {

    int32_t             ret;
    struct resource *   res;
#if (0u != CFG_DMA_MODE)
    uint32_t            cnt;
#endif

    LOG_DBG("get resource mem");
    res = platform_get_resource(
        devData->pDev,
        IORESOURCE_MEM,
        0);

    if (NULL == res) {
        LOG_DBG("failed to get memory resource");

        return (-ENODEV);
    }
    devData->public.addr.phy = (volatile uint8_t *)res->start;
    devData->public.addr.size = (size_t)(res->end - res->start);
    LOG_DBG("phy addr 0x%p", devData->public.addr.phy);
    LOG_DBG("phy size 0x%x", devData->public.addr.size);

    if (NULL == request_mem_region((resource_size_t)devData->public.addr.phy,
                                   (resource_size_t)devData->public.addr.size,
                                   dev_name(&devData->pDev->dev))) {
        LOG_DBG("failed to request memory region");

        return (-EBUSY);
    }
    devData->public.addr.remap = ioremap(
        (unsigned long)devData->public.addr.phy,
        devData->public.addr.size);
    LOG_DBG("vm  addr 0x%p", devData->public.addr.remap);

    if (NULL == devData->public.addr.remap) {
        LOG_DBG("failed to remap memory");
        release_mem_region(
            (resource_size_t)devData->public.addr.phy,
            devData->public.addr.size);

        return (-ENOMEM);
    }
    ret = 0;
#if (0u != CFG_DMA_MODE)
    devData->dma = kcalloc(
        devData->online,
        sizeof(struct dmaData),
        GFP_KERNEL);                                                            /* Storage for DMA structures                               */

    if (NULL == devData->dma) {
        LOG_DBG("failed to request memory for DMA management");
        iounmap(
            devData->addr.remap);
        release_mem_region(
            (resource_size_t)devData->addr.phy,
            devData->addr.size);

        return (-ENOMEM);
    }


    for (cnt = 0u; cnt < devData->online; cnt++) {
        char            resName[DEF_DRV_NAME_LEN];

        LOG_DBG("CS num %d", cnt);
        snprintf(
            resName,
            DEF_DRV_NAME_LEN,
            DEF_HWMOD_DMA_TX "%d",
            cnt);
        LOG_DBG("get resource DMA %s", resName);
        res = platform_get_resource_byname(
            devData->pDev,
            IORESOURCE_DMA,
            resName);

        if (NULL == res) {
            LOG_DBG("failed to get DMA Tx channel info");
            ret = -ENODEV;
            break;
        }
        devData->dma[cnt].tx.chn = EDMA_CHANNEL_ANY;
        devData->dma[cnt].tx.sync = res->start;
        LOG_DBG("DMA resource is %d", res->start);
        snprintf(
            resName,
            DEF_DRV_NAME_LEN,
            DEF_HWMOD_DMA_RX "%d",
            cnt);
        LOG_DBG("get resource DMA %s", resName);
        res = platform_get_resource_byname(
            devData->pDev,
            IORESOURCE_DMA,
            resName);

        if (NULL == res) {
            LOG_DBG("failed to get DMA Rx channel info");
            ret = - ENODEV;
            break;
        }
        devData->dma[cnt].rx.chn = EDMA_CHANNEL_ANY;
        devData->dma[cnt].rx.sync = res->start;
        LOG_DBG("DMA resource is %d", res->start);
    }

    if (0 > ret) {
        LOG_DBG("failed to get DMA info");
        iounmap(
            devData->addr.remap);
        release_mem_region(
            (resource_size_t)devData->addr.phy,
            devData->addr.size);
        kfree(
            devData->dma);
    }
#endif

    return (ret);
}

static void resRelease(
    struct privDevData *    devData) {

    iounmap(
        devData->public.addr.remap);
    release_mem_region(
        (resource_size_t)devData->public.addr.phy,
        devData->public.addr.size);
#if (0u != CFG_DMA_MODE)
    kfree(
        devData->dma);
#endif
}

/*===================================  GLOBAL PRIVATE FUNCTION DEFINITIONS  ==*/
/*====================================  GLOBAL PUBLIC FUNCTION DEFINITIONS  ==*/

int32_t portDevCreate(
    struct rtdm_device ** dev,
    const struct rtdm_device * devTemplate,
    uint32_t            devId) {

    int32_t             ret;
    struct rtdm_device * dev_;
    struct privDevData *    devData;
    struct omap_hwmod * hwmod;
    char                hwmodName[DEF_DRV_NAME_LEN];

    dev_ = kmalloc(sizeof(struct rtdm_device), GFP_KERNEL);                     /* Storage for RT data                                      */

    if (NULL == dev_) {
        LOG_DBG("failed to alloc memory for RT data");

        return (-ENOMEM);
    }
    devData = kmalloc(sizeof(struct privDevData), GFP_KERNEL);                      /* Storage for PORT RT data                                 */

    if (NULL == devData) {
        LOG_DBG("failed to alloc memory for PORT RT data");
        kfree(
            dev_);

        return (-ENOMEM);
    }
    memcpy(
        dev_,
        devTemplate,
        sizeof(struct rtdm_device));                                            /* Copy template structure                                  */
    snprintf(
        &dev_->device_name[0],
        DEF_DRV_NAME_LEN,
        DEF_DRV_SUPP_DEVICE ".%d",
        devId);
    dev_->device_id = devId;
    dev_->device_data = (void *)devData;
    dev_->proc_name = dev_->device_name;
    LOG_DBG("device name: %s", &dev_->device_name[0]);
    snprintf(
        hwmodName,
        DEF_DRV_NAME_LEN,
        DEF_HWMOD_NAME "%d",
        devId);
    LOG_DBG("looking up %s", hwmodName);
    hwmod = omap_hwmod_lookup(
        hwmodName);                                                             /* Find OMAP device descriptor structure                    */

    if (NULL == hwmod) {
        LOG_DBG("failed to find HWMOD device");
        kfree(
            devData);
        kfree(
            dev_);

        return (-ENODEV);
    }
    devData->devBuilt = FALSE;

    if (NULL == hwmod->od) {
        devData->devBuilt = TRUE;
        LOG_DBG("building HWMOD device");
        devData->pDev = omap_device_build(
            DEF_DRV_NAME,
            devId,                                                              /* Stupid OMAP DEVICE needs +1 id number here               */
            hwmod,
            NULL,
            0,
            NULL,
            0,
            0);

        if (NULL == devData->pDev) {
            kfree(
                devData);
            kfree(
                dev_);

            return (-ENODEV);
        }
    }
    devData->pDev = hwmod->od->pdev;
    devData->online = ((struct omap2_mcspi_dev_attr *)hwmod->dev_attr)->num_chipselect;
    LOG_INFO("number of channels %d", devData->online);
    ret = resRequest(
        devData);

    if (0 > ret) {
        kfree(
            devData);
        kfree(
            dev_);

        return (ret);
    }
    pm_runtime_enable(&devData->pDev->dev);
    *dev = dev_;

    return (ret);
}

void portDevDestroy(
    struct rtdm_device * dev) {

    struct privDevData *    devData;

    devData = getPrivDevData(
        dev);
    pm_runtime_put_sync(&devData->pDev->dev);
    pm_runtime_disable(&devData->pDev->dev);

    if (TRUE == devData->devBuilt) {
        devData->devBuilt = FALSE;
        omap_device_delete(
            to_omap_device(devData->pDev));
        platform_device_put(
            devData->pDev);
    }
    resRelease(
        devData);
    kfree(
        devData);
    kfree(
        dev);
}

int32_t portDevEnable(
    struct rtdm_device * dev) {

    int                 retval;
    struct privDevData *    devData;

    devData = getPrivDevData(
        dev);
    retval = pm_runtime_get_sync(&devData->pDev->dev);

    return ((int32_t)retval);
}

int32_t portDevDisable(
    struct rtdm_device * dev) {

    int                 retval;
    struct privDevData *    devData;

    devData = getPrivDevData(
        dev);
    retval = pm_runtime_put_sync(&devData->pDev->dev);

    return ((uint32_t)retval);
}

bool_T portDevIsReady(
    uint32_t            num) {

    if (1u == num) {

        return (TRUE);
    } else {

        return (FALSE);
    }
}

bool_T portChnIsOnline(
    struct rtdm_device * dev,
    uint32_t            chn) {

    struct privDevData *    devData;

    devData = getPrivDevData(
        dev);

    if (chn < devData->online) {

        return (TRUE);
    } else {

        return (FALSE);
    }
}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of plat_omap2.c
 ******************************************************************************/
