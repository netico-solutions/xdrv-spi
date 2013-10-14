/*
 * This file is part of x_spi
 *
 * Copyright (C) 2013 - Nenad Radulovic
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
 * @brief       Interface of arm-none-eabi-gcc port.
 *********************************************************************//** @{ */

#if !defined(ARM_NONE_EABI_GCC_H_)
#define ARM_NONE_EABI_GCC_H_

/*=========================================================  INCLUDE FILES  ==*/
/*===============================================================  MACRO's  ==*/

/*------------------------------------------------------------------------*//**
 * @name        Compiler provided macros
 * @{ *//*--------------------------------------------------------------------*/

/**@brief       C extension - make a function inline
 */
#define PORT_C_INLINE                   __inline__

/**@brief       C extension - make a function inline - always
 */
#define PORT_C_INLINE_ALWAYS            __inline__ __attribute__((__always_inline__))

/**@brief       Omit function prologue/epilogue sequences
 */
#define PORT_C_NAKED                    __attribute__((naked))

/**@brief       Provides function name for assert macros
 */
#define PORT_C_FUNC                     __FUNCTION__

#define PORT_C_FILE                     __FILE__

#define PORT_C_LINE                     __LINE__

/**@brief       Declare a weak function
 */
#define PORT_C_WEAK                     __attribute__((weak))

/**@brief       Declare a function that will never return
 */
#define PORT_C_NORETURN                 __attribute__((noreturn))

#define PORT_C_UNUSED                   __attribute__((unused))

#define PORT_C_ROM

#define PORT_C_ROM_VAR

/**@brief       This attribute specifies a minimum alignment (in bytes) for
 *              variables of the specified type.
 */
#define PORT_C_ALIGNED(expr)            __attribute__((aligned (expr)))

/**@brief       A standardized way of properly setting the value of HW register
 * @param       reg
 *              Register which will be written to
 * @param       mask
 *              The bit mask which will be applied to register and @c val
 *              argument
 * @param       val
 *              Value to be written into the register
 */
#define PORT_HWREG_SET(reg, mask, val)                                          \
    do {                                                                        \
        portReg_T tmp;                                                          \
        tmp = (reg);                                                            \
        tmp &= ~(mask);                                                         \
        tmp |= ((mask) & (val));                                                \
        (reg) = tmp;                                                            \
    } while (0U)

/** @} *//*---------------------------------------------  C++ extern begin  --*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================  DATA TYPES  ==*/

/*------------------------------------------------------------------------*//**
 * @name        Compiler provided data types
 * @brief       All required data types are found in @c stdint.h and @c stddef.h
 * @{ *//*--------------------------------------------------------------------*/

/**@brief       Bool data type
 */
typedef enum boolType {
    TRUE = 1U,                                                                  /**< TRUE                                                   */
    FALSE = 0U                                                                  /**< FALSE                                                  */
} bool_T;

#if defined(__KERNEL__)
#include <linux/stddef.h>
#include <linux/types.h>
typedef uint32_t uint_fast8_t;
#else
#include <stdint.h>
#endif

typedef unsigned int  portReg_T;

/** @} *//*-------------------------------------------------------------------*/
/*======================================================  GLOBAL VARIABLES  ==*/
/*===================================================  FUNCTION PROTOTYPES  ==*/
/*--------------------------------------------------------  C++ extern end  --*/
#ifdef __cplusplus
}
#endif

/*================================*//** @cond *//*==  CONFIGURATION ERRORS  ==*/
/** @endcond *//** @} *//******************************************************
 * END of arm-none-eabi-gcc.h
 ******************************************************************************/
#endif /* ARM_NONE_EABI_GCC_H_ */
