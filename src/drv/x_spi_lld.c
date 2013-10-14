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
 * @brief       Low Level Driver implementation
 *********************************************************************//** @{ */

/*=========================================================  INCLUDE FILES  ==*/

#include "drv/x_spi_lld.h"
#include "drv/x_spi.h"
#include "dbg/dbg.h"

/*=========================================================  LOCAL MACRO's  ==*/
/*======================================================  LOCAL DATA TYPES  ==*/
/*=============================================  LOCAL FUNCTION PROTOTYPES  ==*/
/*=======================================================  LOCAL VARIABLES  ==*/

DECL_MODULE_INFO("x_spi_lld", "Low-level device driver", DEF_DRV_AUTHOR);

/*======================================================  GLOBAL VARIABLES  ==*/
/*============================================  LOCAL FUNCTION DEFINITIONS  ==*/
/*===================================  GLOBAL PRIVATE FUNCTION DEFINITIONS  ==*/
/*====================================  GLOBAL PUBLIC FUNCTION DEFINITIONS  ==*/

int32_t lldDevInit(
    struct rtdm_device * dev) {

    int32_t             retval;

    retval = 0;

    return (retval);
}

void lldDevTerm(
    struct rtdm_device * dev) {

}

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of x_spi_lld.c
 ******************************************************************************/
