/*
 * BCM common config options
 *
 * $Copyright Open Broadcom Corporation$
 *
 * $Id: bcm_cfg.h 294399 2011-11-07 03:31:22Z hharte $
 */

#ifndef _bcm_cfg_h_
#define _bcm_cfg_h_
#if defined(__NetBSD__) || defined(__FreeBSD__)
#if defined(_KERNEL)
#include <opt_bcm.h>
#endif /* defined(_KERNEL) */
#endif /* defined(__NetBSD__) || defined(__FreeBSD__) */
#endif /* _bcm_cfg_h_ */
