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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#include "display.h"
#include "packet.h"
#include "lmp.h"

#define COLOR_OPCODE		COLOR_MAGENTA
#define COLOR_OPCODE_UNKNOWN	COLOR_WHITE_BG

#define ESC4(x) ((127 << 8) | (x))

struct lmp_data {
	uint16_t opcode;
	const char *str;
	void (*func) (const void *data, uint8_t size);
	uint8_t size;
	bool fixed;
};

static const struct lmp_data lmp_table[] = {
	{  1, "LMP_name_req" },
	{  2, "LMP_name_res" },
	{  3, "LMP_accepted" },
	{  4, "LMP_not_accepted" },
	{  5, "LMP_clkoffset_req" },
	{  6, "LMP_clkoffset_res" },
	{  7, "LMP_detach" },
	{  8, "LMP_in_rand" },
	{  9, "LMP_comb_key" },
	{ 10, "LMP_unit_key" },
	{ 11, "LMP_au_rand" },
	{ 12, "LMP_sres" },
	{ 13, "LMP_temp_rand" },
	{ 14, "LMP_temp_key" },
	{ 15, "LMP_encryption_mode_req" },
	{ 16, "LMP_encryption_key_size_req" },
	{ 17, "LMP_start_encryption_req" },
	{ 18, "LMP_stop_encryption_req" },
	{ 19, "LMP_switch_req" },
	{ 20, "LMP_hold" },
	{ 21, "LMP_hold_req" },
	{ 22, "LMP_sniff" },
	{ 23, "LMP_sniff_req" },
	{ 24, "LMP_unsniff_req" },
	{ 25, "LMP_park_req" },
	{ 26, "LMP_park" },
	{ 27, "LMP_set_broadcast_scan_window" },
	{ 28, "LMP_modify_beacon" },
	{ 29, "LMP_unpark_BD_ADDR_req" },
	{ 30, "LMP_unpark_PM_ADDR_req" },
	{ 31, "LMP_incr_power_req" },
	{ 32, "LMP_decr_power_req" },
	{ 33, "LMP_max_power" },
	{ 34, "LMP_min_power" },
	{ 35, "LMP_auto_rate" },
	{ 36, "LMP_preferred_rate" },
	{ 37, "LMP_version_req" },
	{ 38, "LMP_version_res" },
	{ 39, "LMP_features_req" },
	{ 40, "LMP_features_res" },
	{ 41, "LMP_quality_of_service" },
	{ 42, "LMP_quality_of_service_req" },
	{ 43, "LMP_SCO_link_req" },
	{ 44, "LMP_remove_SCO_link_req" },
	{ 45, "LMP_max_slot" },
	{ 46, "LMP_max_slot_req" },
	{ 47, "LMP_timing_accuracy_req" },
	{ 48, "LMP_timing_accuracy_res" },
	{ 49, "LMP_setup_complete" },
	{ 50, "LMP_use_semi_permanent_key" },
	{ 51, "LMP_host_connection_req" },
	{ 52, "LMP_slot_offset" },
	{ 53, "LMP_page_mode_req" },
	{ 54, "LMP_Page_scan_mode_req" },
	{ 55, "LMP_supervision_timeout" },
	{ 56, "LMP_test_activate" },
	{ 57, "LMP_test_control" },
	{ 58, "LMP_encryption_key_size_mask_req" },
	{ 59, "LMP_encryption_key_size_mask_res" },
	{ 60, "LMP_set_AFH" },
	{ 61, "LMP_encapsulated_header" },
	{ 62, "LMP_encapsulated_payload" },
	{ 63, "LMP_simple_pairing_confirm" },
	{ 64, "LMP_simple_pairing_number" },
	{ 65, "LMP_DHkey_check" },
	{ 66, "LMP_pause_encryption_aes_req" },
	{ ESC4(1),  "LMP_accepted_ext" },
	{ ESC4(2),  "LMP_not_accepted_ext" },
	{ ESC4(3),  "LMP_features_req_ext" },
	{ ESC4(4),  "LMP_features_res_ext" },
	{ ESC4(5),  "LMP_clk_adj" },
	{ ESC4(6),  "LMP_clk_adj_ack" },
	{ ESC4(7),  "LMP_clk_adj_req" },
	{ ESC4(11), "LMP_packet_type_table" },
	{ ESC4(12), "LMP_eSCO_link_req" },
	{ ESC4(13), "LMP_remove_eSCO_link_req" },
	{ ESC4(16), "LMP_channel_classification_req" },
	{ ESC4(17), "LMP_channel_classification" },
	{ ESC4(21), "LMP_sniff_subrating_req" },
	{ ESC4(22), "LMP_sniff_subrating_res" },
	{ ESC4(23), "LMP_pause_encryption_req" },
	{ ESC4(24), "LMP_resume_encryption_req" },
	{ ESC4(25), "LMP_IO_capability_req" },
	{ ESC4(26), "LMP_IO_capability_res" },
	{ ESC4(27), "LMP_numeric_comparision_failed" },
	{ ESC4(28), "LMP_passkey_failed" },
	{ ESC4(29), "LMP_oob_failed" },
	{ ESC4(30), "LMP_keypress_notification" },
	{ ESC4(31), "LMP_power_control_req" },
	{ ESC4(32), "LMP_power_control_res" },
	{ ESC4(33), "LMP_ping_req" },
	{ ESC4(34), "LMP_ping_res" },
	{ }
};


void lmp_packet(const void *data, uint8_t size)
{
	const struct lmp_data *lmp_data = NULL;
	const char *opcode_color, *opcode_str;
	uint16_t opcode;
	uint8_t tid, off;
	int i;

	tid = ((const uint8_t *) data)[0] & 0x01;
	opcode = (((const uint8_t *) data)[0] & 0xfe) >> 1;

	switch (opcode) {
	case 127:
		opcode = ESC4(((const uint8_t *) data)[1]);
		off = 2;
		break;
	case 126:
	case 125:
	case 124:
		return;
	default:
		off = 1;
		break;
	}

	for (i = 0; lmp_table[i].str; i++) {
		if (lmp_table[i].opcode == opcode) {
			lmp_data = &lmp_table[i];
			break;
		}
	}

	if (lmp_data) {
		if (lmp_data->func)
			opcode_color = COLOR_OPCODE;
		else
			opcode_color = COLOR_OPCODE_UNKNOWN;
		opcode_str = lmp_data->str;
	} else {
		opcode_color = COLOR_OPCODE_UNKNOWN;
		opcode_str = "Unknown";
	}

	if (opcode & 0xff00)
		print_indent(6, opcode_color, "", opcode_str, COLOR_OFF,
			" (%d/%d) TID %d", opcode >> 8, opcode & 0xff, tid);
	else
		print_indent(6, opcode_color, "", opcode_str, COLOR_OFF,
					" (%d) TID %d", opcode, tid);

	if (!lmp_data || !lmp_data->func) {
		packet_hexdump(data + off, size - off);
		return;
	}

	if (lmp_data->fixed) {
		if (size - 1 != lmp_data->size) {
			print_text(COLOR_ERROR, "invalid packet size");
			packet_hexdump(data + off, size - off);
			return;
		}
	} else {
		if (size - 1 < lmp_data->size) {
			print_text(COLOR_ERROR, "too short packet");
			packet_hexdump(data + off, size - off);
			return;
		}
	}

	lmp_data->func(data + off, size - off);
}
