/****************************************************************************
 *
 * Copyright (C) 2004 Synopsys, Inc. All Rights Reserved.
 * Portions Copyright (C) 2004 Accellera Organization, Inc.
 * All Rights Reserved. Used with Permission.
 *
 *                     Chronologic VCS (TM)
 *
 * SystemVerilog 3.1 DPI (Direct Programming Interface).
 *
 * Source-level compatibility include file "svdpi_src.h"
 *
 * Revision:
 *  as in Annex F of SystemVerilog 3.1a draft 5
 *  Language Reference Manual
 *  Accellera's Extensions to Verilog (R)
 *
 *
 * This file contains the macro definitions for declaring variables
 * to represent SystemVerilog packed arrays of type bit or logic.
 *
 * The actual definitions are implementation specific.
 * This version is for VCS 7.2.
 *
 * The file should be included with all C functions that use SystemVerilog
 * Direct Programming Interface and need to define data structures
 * corresponding to SystemVerilog data structures involving packed arrays.
 *
 ****************************************************************************/

/* $Revision$ */

#ifndef SVDPI_SRC_H /* allow multiple inclusions */
#define SVDPI_SRC_H

/*---------------------------------------------------------------------------*/
/*------------------------------- definitions -------------------------------*/
/*---------------------------------------------------------------------------*/

#if defined(__cplusplus)
extern "C" {
#endif


/* Macros for declaring variables to represent SystemVerilog */
/* packed arrays of type bit or logic.                       */
/* WIDTH = number of bits,                                   */
/* NAME  = name of a declared field/variable                 */

#define SV_BIT_PACKED_ARRAY(WIDTH,NAME) \
    svBitVec32 NAME [ SV_CANONICAL_SIZE(WIDTH) ]

#define SV_LOGIC_PACKED_ARRAY(WIDTH,NAME) \
    svLogicVec32 NAME [ SV_CANONICAL_SIZE(WIDTH) ]


#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif /* SVDPI_SRC_H */
