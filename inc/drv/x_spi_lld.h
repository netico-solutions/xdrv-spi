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
void lldReset(
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
 *              Channel
 * @param       mode
 *              CS mode:
 *              0 - enabled
 *              1 - disabled
 */
void lldCsModeSet(
    struct rtdm_device * dev,
    uint32_t            mode);

/**@brief       Set Master/Slave mode
 * @param       dev
 *              RT device descriptor
 * @param       mode
 *              0 - Master
 *              1 - Slave
 */
void lldModeSet(
    struct rtdm_device * dev,
    uint32_t            mode);

/**@brief       Set channel mode to single or multichannel
 * @param       dev
 *              RT device descriptor
 * @param       chnMode
 *              0 - Multichannel mode
 *              1 - Single channel mode
 */
void lldChannelModeSet(
    struct rtdm_device * dev,
    uint32_t            chnMode);

/**@brief       Initial SPI delay for first transfer
 * @param       dev
 *              RT device descriptor
 * @param       delay
 *              0 - no delay for first transfer
 *              1 - 4 SPI clocks
 *              2 - 8 SPI clocks
 *              3 - 16 SPI clocks
 *              4 - 32 SPI clocks
 */
void lldInitialDelaySet(
    struct rtdm_device * dev,
    uint32_t            delay);

/**@brief       Transmit/receive modes
 * @param       dev
 *              RT device descriptor
 * @param       mode
 *              0 - Transmit and receive mode
 *              1 - Receive only mode
 *              2 - Transmit only mode
 */
void lldChnTransferModeSet(
    struct rtdm_device * dev,
    uint32_t            chn,
    uint32_t            mode);

/**@brief       Set pin layout
 * @param       dev
 *              RT device descriptor
 * @param       chn
 *              Selected channel
 * @param       layout
 *              0 - Rx = SPIDAT[1], Tx = SPIDAT[0]
 *              1 - Rx = SPIDAT[0], Tx = SPIDAT[1]
 */
void lldChnPinLayoutSet(
    struct rtdm_device * dev,
    uint32_t            chn,
    uint32_t            layout);

/**@brief       Set data word length
 * @param       dev
 *              RT device descriptor
 * @param       chn
 *              Selected channel
 * @param       wordLength
 *              Data word length in bits. Valid range is from 4 to 32 bits.
 */
void lldChnWordLengthSet(
    struct rtdm_device * dev,
    uint32_t            chn,
    uint32_t            wordLength);

/**@brief       Set CS delay
 * @param       dev
 *              RT device descriptor
 * @param       chn
 *              Selected channel
 * @param       delay
 *              0 - 0.5 clock cycles
 *              1 - 1.5 clock cycles
 *              2 - 2.5 clock cycles
 *              3 - 3.5 clock cycles
 */
void lldChnCsDelaySet(
    struct rtdm_device * dev,
    uint32_t            chn,
    uint32_t            delay);

/**@brief       Set active CS polarity
 * @param       dev
 *              RT device descriptor
 * @param       chn
 *              Selected channel
 * @param       polarity
 *              0 - Active high
 *              1 - Active low
 */
void lldChnCsPolaritySet(
    struct rtdm_device * dev,
    uint32_t            chn,
    uint32_t            polarity);

/**@brief       Force CS state to given state parameter
 * @param       dev
 *              RT device descriptor
 * @param       chn
 *              Selected channel
 * @param       state
 *              0 - Inactive state
 *              1 - Active state
 * @return      Operation status
 *              0 - success
 *              !0 - standard Linux error define
 */
int32_t lldChnCsStateSet(
    struct rtdm_device * dev,
    uint32_t            chn,
    uint32_t            state);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of x_spi_lld.h
 ******************************************************************************/
#endif /* X_SPI_LLD_H_ */
