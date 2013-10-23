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
 * @brief   	Driver main configuration
 *********************************************************************//** @{ */

#if !defined(X_SPI_CFG_H_)
#define X_SPI_CFG_H_

/*=========================================================  INCLUDE FILES  ==*/
/*===============================================================  DEFINES  ==*/
/** @cond */

/** @endcond */
/*==============================================================  SETTINGS  ==*/

/**@brief       DMA mode selection
 * @details     0 - no DMA mode
 */
#define CFG_DMA_MODE                    0u

/**@brief       Maximum number of devices
 */
#define CFG_MAX_DEVICES                 10u

/**@brief       Maximum number of channels per device
 */
#define CFG_MAX_CHN                     4u

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of x_spi_cfg.h
 ******************************************************************************/
#endif /* X_SPI_CFG_H_ */
