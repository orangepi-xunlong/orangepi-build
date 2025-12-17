/*
 * $Copyright Open Broadcom Corporation$
 *
 * $Id: epivers.h.in,v 13.33 2010-09-08 22:08:53 csm Exp $
 *
*/

#ifndef _epivers_h_
#define _epivers_h_

#define	EPI_MAJOR_VERSION	6

#define	EPI_MINOR_VERSION	37

#define	EPI_RC_NUMBER		32

#define	EPI_INCREMENTAL_NUMBER	0

#define EPI_BUILD_NUMBER	1

#define	EPI_VERSION		6, 37, 32, 0

#define	EPI_VERSION_NUM		0x06252000

#define EPI_VERSION_DEV		6.37.32

/* Driver Version String, ASCII, 32 chars max */
#ifdef BCMINTERNAL
#define	EPI_VERSION_STR		"6.37.32 (TOB) (r410874 BCMINT)"
#else
#ifdef WLTEST
#define	EPI_VERSION_STR		"6.37.32 (TOB) (r410874 WLTEST)"
#else
#define	EPI_VERSION_STR		"6.37.32 (TOB) (r410874)"
#endif
#endif /* BCMINTERNAL */

#endif /* _epivers_h_ */
