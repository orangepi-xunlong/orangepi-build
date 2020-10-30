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
#include <stdbool.h>
#include <sys/time.h>

#define PACKET_FILTER_SHOW_INDEX	(1 << 0)
#define PACKET_FILTER_SHOW_DATE		(1 << 1)
#define PACKET_FILTER_SHOW_TIME		(1 << 2)
#define PACKET_FILTER_SHOW_TIME_OFFSET	(1 << 3)
#define PACKET_FILTER_SHOW_ACL_DATA	(1 << 4)
#define PACKET_FILTER_SHOW_SCO_DATA	(1 << 5)

void packet_set_filter(unsigned long filter);
void packet_add_filter(unsigned long filter);
void packet_del_filter(unsigned long filter);

void packet_select_index(uint16_t index);

void packet_hexdump(const unsigned char *buf, uint16_t len);
void packet_print_version(const char *label, uint8_t version,
				const char *sublabel, uint16_t subversion);
void packet_print_company(const char *label, uint16_t company);
void packet_print_addr(const char *label, const void *data, bool random);
void packet_print_ad(const void *data, uint8_t size);
void packet_print_features_ll(const uint8_t *features);
void packet_print_channel_map_ll(const uint8_t *map);

void packet_control(struct timeval *tv, uint16_t index, uint16_t opcode,
					const void *data, uint16_t size);
void packet_monitor(struct timeval *tv, uint16_t index, uint16_t opcode,
					const void *data, uint16_t size);
void packet_simulator(struct timeval *tv, uint16_t frequency,
					const void *data, uint16_t size);

void packet_new_index(struct timeval *tv, uint16_t index, const char *label,
				uint8_t type, uint8_t bus, const char *name);
void packet_del_index(struct timeval *tv, uint16_t index, const char *label);

void packet_hci_command(struct timeval *tv, uint16_t index,
					const void *data, uint16_t size);
void packet_hci_event(struct timeval *tv, uint16_t index,
					const void *data, uint16_t size);
void packet_hci_acldata(struct timeval *tv, uint16_t index, bool in,
					const void *data, uint16_t size);
void packet_hci_scodata(struct timeval *tv, uint16_t index, bool in,
					const void *data, uint16_t size);
