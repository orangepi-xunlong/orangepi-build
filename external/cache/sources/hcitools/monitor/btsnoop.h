/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2011-2012  Intel Corporation
 *  Copyright (C) 2004-2010  Marcel Holtmann <marcel@holtmann.org>
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <stdint.h>
#include <sys/time.h>

#define BTSNOOP_TYPE_HCI		1001
#define BTSNOOP_TYPE_UART		1002
#define BTSNOOP_TYPE_BCSP		1003
#define BTSNOOP_TYPE_3WIRE		1004

#define BTSNOOP_TYPE_EXTENDED_HCI	2001
#define BTSNOOP_TYPE_EXTENDED_PHY	2002

#define BTSNOOP_OPCODE_NEW_INDEX	0
#define BTSNOOP_OPCODE_DEL_INDEX	1
#define BTSNOOP_OPCODE_COMMAND_PKT	2
#define BTSNOOP_OPCODE_EVENT_PKT	3
#define BTSNOOP_OPCODE_ACL_TX_PKT	4
#define BTSNOOP_OPCODE_ACL_RX_PKT	5
#define BTSNOOP_OPCODE_SCO_TX_PKT	6
#define BTSNOOP_OPCODE_SCO_RX_PKT	7

void btsnoop_create(const char *path, uint32_t type);
void btsnoop_write(struct timeval *tv, uint32_t flags,
					const void *data, uint16_t size);
void btsnoop_write_hci(struct timeval *tv, uint16_t index, uint16_t opcode,
					const void *data, uint16_t size);
void btsnoop_write_phy(struct timeval *tv, uint16_t frequency,
					const void *data, uint16_t size);
int btsnoop_open(const char *path, uint32_t *type);
int btsnoop_read_hci(struct timeval *tv, uint16_t *index, uint16_t *opcode,
						void *data, uint16_t *size);
int btsnoop_read_phy(struct timeval *tv, uint16_t *frequency,
						void *data, uint16_t *size);
void btsnoop_close(void);
