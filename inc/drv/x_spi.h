/*
 * This file is part of x-spi
 *
 * Copyright (C) 2011, 2012 - Nenad Radulovic
 *
 * x-spi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * x-spi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with x-spi; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 *
 * web site:    http://blueskynet.dyndns-server.com
 * e-mail  :    blueskyniss@gmail.com
 *//***********************************************************************//**
 * @file
 * @author  	Nenad Radulovic
 * @brief       Driver interface
 *********************************************************************//** @{ */

#if !defined(X_SPI_H_)
#define X_SPI_H_

/*=========================================================  INCLUDE FILES  ==*/

#include "rtdm/rtdm_driver.h"

#include "drv/x_spi_cfg.h"

/*===============================================================  MACRO's  ==*/

#define DEF_DRV_VERSION_MAJOR           1
#define DEF_DRV_VERSION_MINOR           0
#define DEF_DRV_VERSION_PATCH           0
#define DEF_DRV_DESCRIPTION             "Real-time SPI driver"
#define DEF_DRV_AUTHOR                  "Nenad Radulovic"
#define DEF_DRV_NAME                    "xspi"
#define DEF_DRV_NAME_LEN                RTDM_MAX_DEVNAME_LEN
#define DEF_DRV_SUPP_DEVICE             "xspi"

/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

struct devCtx {
    struct unit {
        rtdm_event_t        opr;
        rtdm_sem_t          acc;
    }                   tx, rx;
    struct cache {
        volatile uint8_t *  io;
    }                   cache;
};

/*======================================================  GLOBAL VARIABLES  ==*/

/*------------------------------------------------------------------------*//**
 * @name        Variables group
 * @brief       brief description
 * @{ *//*--------------------------------------------------------------------*/

/** @} *//*-------------------------------------------------------------------*/
/*===================================================  FUNCTION PROTOTYPES  ==*/

/*------------------------------------------------------------------------*//**
 * @name        Function group
 * @brief       brief description
 * @{ *//*--------------------------------------------------------------------*/

/** @} *//*-------------------------------------------------------------------*/
/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of x-spi.h
 ******************************************************************************/
#endif /* X_SPI_H_ */
