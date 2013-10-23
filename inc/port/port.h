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
 * @author  	Nenad Radulovic
 * @brief       Interface of port.
 *********************************************************************//** @{ */

#if !defined(PORT_H_)
#define PORT_H_

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

/**@brief       Create port side of device driver
 * @param       dev
 *              RT device descriptor which will be initialized by this function
 * @param       devTemplate
 *              RT device template descriptor
 * @param       devId
 *              Unique device ID number (reffer to IC datasheet for IDs)
 * @return      Operation status:
 *              0 - SUCCESS
 *              !0 - standard Linux error define
 * @details     This function will create port side of device driver according
 *              to RT device template descriptor and will fill unique device
 *              members to appropriate values. This function is also responsible
 *              for allocation of private data structures.
 */
int32_t portDevCreate(
    struct rtdm_device ** dev,
    const struct rtdm_device * devTemplate,
    uint32_t            devId);

/**@brief       Destroy port side of device driver
 * @param       dev
 *              RT device descriptor which was previously initialized by
 *              portDevCreate()
 * @details     This function will release all resources allocated by
 *              portDevCreate() function
 */
void portDevDestroy(
    struct rtdm_device * dev);

/**@brief       Enable port side device driver
 * @param       dev
 *              RT device descriptor
 * @return      Operation status:
 *              0 - SUCCESS
 *              !0 - standard Linux error define
 * @details     If port device has PM functionality then this function will put
 *              the device into enabled/running state which means that it will
 *              turn on all clocks needed by the device and it's data/address
 *              busses.
 */
int32_t portDevEnable(
    struct rtdm_device * dev);

/**@brief       Disable port side device driver
 * @param       dev
 *              RT device descriptor
 * @return      Operation status:
 *              0 - SUCCESS
 *              !0 - standard Linux error define
 * @details     Put the device into sleep state if applicable.
 */
int32_t portDevDisable(
    struct rtdm_device * dev);

/**@brief       Returns if device with specified ID can be managed by this
 *              driver
 * @param       num
 *              ID of the device
 * @return      Device status:
 *              TRUE - device can be managed
 *              FALSE - device can't be managed
 */
bool_T portDevIsReady(
    uint32_t            num);

/**@brief       Returns if channel with specified ID can be managed by this
 *              driver
 * @param       dev
 *              RT device descriptor
 * @param       chn
 *              Channel number
 * @return      Channel status:
 *              TRUE - channel can be managed
 *              FALSE - channel can't be managed
 */
uint32_t portChnIsOnline(
    struct rtdm_device * dev,
    uint32_t            chn);

/**@brief       Returns IO remap address
 * @param       dev
 *              RT device descriptor
 * @return      IO remap address
 */
volatile uint8_t * portRemapGet(
    struct rtdm_device * dev);

/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of port.h
 ******************************************************************************/
#endif /* PORT_H_ */
