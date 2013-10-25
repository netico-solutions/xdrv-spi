/*
 * This file is part of eSolid
 *
 * Copyright (C) 2011, 2012 - Nenad Radulovic
 *
 * eSolid is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * eSolid is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * eSolid; if not, write to the Free Software Foundation, Inc., 51 Franklin St,
 * Fifth Floor, Boston, MA  02110-1301  USA
 *
 * web site:    http://blueskynet.dyndns-server.com
 * e-mail  :    blueskyniss@gmail.com
 *//***********************************************************************//**
 * @file
 * @author      Nenad Radulovic
 * @brief       Configuration of Debug.
 * @addtogroup  dbg_cfg
 *********************************************************************//** @{ */

#if !defined(DBG_CFG_H_)
#define DBG_CFG_H_

/*=========================================================  INCLUDE FILES  ==*/

/*===============================================================  DEFINES  ==*/
/** @cond */

/** @endcond */
/*==============================================================  SETTINGS  ==*/

/**@brief       Enable/disable Debug module
 * @details     Possible values:
 *              - 0U - Debug is disabled
 *              - 1U - Debug is enabled
 */
#if !defined(CFG_DBG_ENABLE)
# define CFG_DBG_ENABLE                 1u
#endif

/**@brief       Enable/disable API arguments validation
 * @details     Possible values:
 *              - 0U - API validation is disabled
 *              - 1U - API validation is enabled
 * @note        This option is enabled only if @ref CFG_DBG_ENABLE is enabled,
 *              too.
 */
#if !defined(CFG_DBG_API_VALIDATION)
# define CFG_DBG_API_VALIDATION         1u
#endif

/**@brief       Enable/disable internal checks
 * @details     Possible values:
 *              - 0U - API validation is disabled
 *              - 1U - API validation is enabled
 * @note        This option is enabled only if @ref CFG_DBG_ENABLE is enabled,
 *              too.
 */
#if !defined(CFG_DBG_INTERNAL_CHECK)
# define CFG_DBG_INTERNAL_CHECK         0u
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/

#if ((1U != CFG_DBG_ENABLE) && (0U != CFG_DBG_ENABLE))
# error "eSolid RT Kernel: Configuration option CFG_DBG_ENABLE is out of range."
#endif

#if ((1U != CFG_DBG_API_VALIDATION) && (0U != CFG_DBG_API_VALIDATION))
# error "eSolid RT Kernel: Configuration option CFG_DBG_API_VALIDATION is out of range."
#endif

#if ((1U != CFG_DBG_INTERNAL_CHECK) && (0U != CFG_DBG_INTERNAL_CHECK))
# error "eSolid RT Kernel: Configuration option CFG_DBG_INTERNAL_CHECK is out of range."
#endif

/** @endcond *//** @} *//******************************************************
 * END of err_cfg.h
 ******************************************************************************/
#endif /* DBG_CFG_H_ */
