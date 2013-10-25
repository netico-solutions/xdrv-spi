/*
 * This file is part of x_spi
 *
 * Copyright (C) 2011, 2012 - Nenad Radulovic
 *
 * x_spi is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or
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
 * @brief       Interface of Low-level driver
 *********************************************************************//** @{ */

#if !defined(X_SPI_LLD_H_)
#define X_SPI_LLD_H_

/*=========================================================  INCLUDE FILES  ==*/

#include <rtdm/rtdm_driver.h>

#include "arch/compiler.h"

/*===============================================================  MACRO's  ==*/
/*------------------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/

/**@brief       Initialize device
 * @param       dev
 *              RT device descriptor
 * @return      Operation status:
 *              0 - SUCCESS
 *              !0 - standard Linux error define
 */
int32_t lldDevInit(
    struct rtdm_device * dev);

/**@brief       Terminate device
 * @param       dev
 *              RT device descriptor
 */
void lldDevTerm(
    struct rtdm_device * dev);

/**@brief       Returns IO remap address
 * @param       dev
 *              RT device descriptor
 * @return      IO remap address
 */
volatile uint8_t * lldRemapGet(
    struct rtdm_device * dev);

/**@brief       Reset device module
 * @param       dev
 *              RT device descriptor
 */
void lldDevReset(
    struct rtdm_device * dev);

/**@brief       Enable FIFO on specified channel
 * @param       dev
 *              RT device descriptor
 * @param       chn
 *              Channel
 */
void lldFIFOChnEnable(
    struct rtdm_device * dev,
    uint32_t            chn);

/**@brief       Disable FIFO on specified channel.
 * @param       dev
 *              RT device descriptor
 * @param       chn
 *              Channel
 */
void lldFIFOChnDisable(
    struct rtdm_device * dev,
    uint32_t            chn);

/**@brief       Set CS mode
 * @param       dev
 *              RT device descriptor
 * @param       chn
 *
 * @param mode
 */
void lldCsModeSet(
    struct rtdm_device * dev,
    int32_t             chn,
    uint32_t            mode);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of x_spi_lld.h
 ******************************************************************************/
#endif /* X_SPI_LLD_H_ */
