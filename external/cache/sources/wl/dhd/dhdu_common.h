/*
 * Linux port of dhd command line utility, hacked from wl utility.
 *
 * Copyright (C) 2013, Broadcom Corporation
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 *
 * $Id: dhdu_common.h 281524 2011-09-02 17:09:25Z $
 */

/* Common header file for dhdu_linux.c and dhdu_ndis.c */

#ifndef _dhdu_common_h
#define _dhdu_common_h

/* DHD utility function declarations */
extern int dhd_check(void *dhd);
extern int dhd_atoip(const char *a, struct ipv4_addr *n);
extern int dhd_option(char ***pargv, char **pifname, int *phelp);
void dhd_usage(cmd_t *port_cmds);

/* Remote DHD declarations */
int remote_type = NO_REMOTE;
extern char *g_rwl_buf_mac;
extern char* g_rwl_device_name_serial;
unsigned short g_rwl_servport;
char *g_rwl_servIP = NULL;
unsigned short defined_debug = DEBUG_ERR | DEBUG_INFO;


static int process_args(struct ifreq* ifr, char **argv);

#define dtoh32(i) i
#define dtoh16(i) i

#endif  /* _dhdu_common_h_ */
