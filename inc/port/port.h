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
 * @author  	Nenad Radulovic
 * @brief       Interface of port.
 *********************************************************************//** @{ */

#if !defined(PORT_H_)
#define PORT_H_

/*=========================================================  INCLUDE FILES  ==*/

#include <rtdm/rtdm_driver.h>

#include "arch/compiler.h"

/*===============================================================  MACRO's  ==*/

/*------------------------------------------------------------------------*//**
 * @name        Macro group
 * @brief       brief description
 * @{ *//*--------------------------------------------------------------------*/

/** @} *//*-------------------------------------------------------------------*/
/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

/*------------------------------------------------------------------------*//**
 * @name        Data types group
 * @brief       brief description
 * @{ *//*--------------------------------------------------------------------*/

/** @} *//*-------------------------------------------------------------------*/
/*======================================================  GLOBAL VARIABLES  ==*/

/*------------------------------------------------------------------------*//**
 * @name        Variables group
 * @brief       brief description
 * @{ *//*--------------------------------------------------------------------*/

/** @} *//*-------------------------------------------------------------------*/
/*===================================================  FUNCTION PROTOTYPES  ==*/

int32_t portDevCreate(
    struct rtdm_device ** dev,
    const struct rtdm_device * devTemplate,
    uint32_t            devId);

void portDevDestroy(
    struct rtdm_device * dev);

bool_T portDevIsReady(
    uint32_t            num);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of port.h
 ******************************************************************************/
#endif /* PORT_H_ */
