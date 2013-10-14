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
 * @brief       Interface of log.
 *********************************************************************//** @{ */

#if !defined(LOG_H_)
#define LOG_H_

/*=========================================================  INCLUDE FILES  ==*/

#include <rtdm/rtdm_driver.h>

#include "drv/x_spi.h"
#include "log/log_cfg.h"

/*===============================================================  MACRO's  ==*/

#if (1U == CFG_LOG_DBG_ENABLE)
#define LOG_DBG(msg, ...)                                                       \
    rtdm_printk(KERN_INFO DEF_DRV_NAME "(DBG): " msg "\n", ##__VA_ARGS__);
#else
#define LOG_DBG(msg, ...)
#endif

#define LOG(msg, ...)                                                           \
    rtdm_printk(KERN_INFO DEF_DRV_NAME ": " msg "\n", ##__VA_ARGS__)

#if (1U == CFG_LOG_INFO_ENABLE)
#define LOG_INFO(msg, ...)                                                      \
    rtdm_printk(KERN_INFO DEF_DRV_NAME ": " msg "\n", ##__VA_ARGS__)
#else
#define LOG_INFO(msg, ...)
#endif

#define LOG_WARN(msg, ...)                                                      \
    rtdm_printk(KERN_WARNING DEF_DRV_NAME " <WARN>: line %d: " msg "\n", __LINE__, ##__VA_ARGS__)

#define LOG_ERR(msg, ...)                                                       \
    rtdm_printk(KERN_ERR DEF_DRV_NAME " <ERR>: line %d: " msg "\n", __LINE__, ##__VA_ARGS__)

#define LOG_VAR(var)                                                            \
    rtdm_printk(KERN_INFO DEF_DRV_NAME " VAR " #var " : %d\n", var )

#define LOG_PVAR(var)                                                           \
    rtdm_printk(KERN_INFO DEF_DRV_NAME " PTR " #var " : %p\n", var )

/*============================================================  DATA TYPES  ==*/
/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/
/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of log.h
 ******************************************************************************/
#endif /* LOG_H_ */
