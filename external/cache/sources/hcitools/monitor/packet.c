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
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <time.h>
#include <sys/time.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include "display.h"
#include "bt.h"
#include "ll.h"
#include "uuid.h"
#include "l2cap.h"
#include "control.h"
#include "btsnoop.h"
#include "vendor.h"
#include "packet.h"

#define COLOR_INDEX_LABEL		COLOR_WHITE
#define COLOR_TIMESTAMP			COLOR_YELLOW

#define COLOR_NEW_INDEX			COLOR_GREEN
#define COLOR_DEL_INDEX			COLOR_RED

#define COLOR_HCI_COMMAND		COLOR_BLUE
#define COLOR_HCI_COMMAND_UNKNOWN	COLOR_WHITE_BG

#define COLOR_HCI_EVENT			COLOR_MAGENTA
#define COLOR_HCI_EVENT_UNKNOWN		COLOR_WHITE_BG

#define COLOR_HCI_ACLDATA		COLOR_CYAN
#define COLOR_HCI_SCODATA		COLOR_YELLOW

#define COLOR_UNKNOWN_FEATURE_BIT	COLOR_WHITE_BG
#define COLOR_UNKNOWN_EVENT_MASK	COLOR_WHITE_BG
#define COLOR_UNKNOWN_LE_STATES		COLOR_WHITE_BG
#define COLOR_UNKNOWN_SERVICE_CLASS	COLOR_WHITE_BG

#define COLOR_PHY_PACKET		COLOR_BLUE

static time_t time_offset = ((time_t) -1);
static unsigned long filter_mask = 0;
static bool index_filter = false;
static uint16_t index_number = 0;
static uint16_t index_current = 0;

#define MAX_CONN 16

struct conn_data {
	uint16_t handle;
	uint8_t  type;
};

static struct conn_data conn_list[MAX_CONN];

static void assign_handle(uint16_t handle, uint8_t type)
{
	int i;

	for (i = 0; i < MAX_CONN; i++) {
		if (conn_list[i].handle == 0x0000) {
			conn_list[i].handle = handle;
			conn_list[i].type = type;
			break;
		}
	}
}

static void release_handle(uint16_t handle)
{
	int i;

	for (i = 0; i < MAX_CONN; i++) {
		if (conn_list[i].handle == handle) {
			conn_list[i].handle = 0x0000;
			conn_list[i].type = 0x00;
			break;
		}
	}
}

static uint8_t get_type(uint16_t handle)
{
	int i;

	for (i = 0; i < MAX_CONN; i++) {
		if (conn_list[i].handle == handle)
			return conn_list[i].type;
	}

	return 0xff;
}

void packet_set_filter(unsigned long filter)
{
	filter_mask = filter;
}

void packet_add_filter(unsigned long filter)
{
	if (index_filter)
		filter &= ~PACKET_FILTER_SHOW_INDEX;

	filter_mask |= filter;
}

void packet_del_filter(unsigned long filter)
{
	filter_mask &= ~filter;
}

void packet_select_index(uint16_t index)
{
	filter_mask &= ~PACKET_FILTER_SHOW_INDEX;

	index_filter = true;
	index_number = index;
}

#define print_space(x) printf("%*c", (x), ' ');

static void print_packet(struct timeval *tv, uint16_t index, char ident,
					const char *color, const char *label,
					const char *text, const char *extra)
{
	int col = num_columns();
	char line[256], ts_str[64];
	int n, ts_len = 0, ts_pos = 0, len = 0, pos = 0;

	if (filter_mask & PACKET_FILTER_SHOW_INDEX) {
		if (use_color()) {
			n = sprintf(ts_str + ts_pos, "%s", COLOR_INDEX_LABEL);
			if (n > 0)
				ts_pos += n;
		}

		n = sprintf(ts_str + ts_pos, " [hci%d]", index);
		if (n > 0) {
			ts_pos += n;
			ts_len += n;
		}
	}

	if (tv) {
		time_t t = tv->tv_sec;
		struct tm tm;

		localtime_r(&t, &tm);

		if (use_color()) {
			n = sprintf(ts_str + ts_pos, "%s", COLOR_TIMESTAMP);
			if (n > 0)
				ts_pos += n;
		}

		if (filter_mask & PACKET_FILTER_SHOW_DATE) {
			n = sprintf(ts_str + ts_pos, " %04d-%02d-%02d",
				tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
			if (n > 0) {
				ts_pos += n;
				ts_len += n;
			}
		}

		if (filter_mask & PACKET_FILTER_SHOW_TIME) {
			n = sprintf(ts_str + ts_pos, " %02d:%02d:%02d.%06lu",
				tm.tm_hour, tm.tm_min, tm.tm_sec, tv->tv_usec);
			if (n > 0) {
				ts_pos += n;
				ts_len += n;
			}
		}

		if (filter_mask & PACKET_FILTER_SHOW_TIME_OFFSET) {
			n = sprintf(ts_str + ts_pos, " %lu.%06lu",
					tv->tv_sec - time_offset, tv->tv_usec);
			if (n > 0) {
				ts_pos += n;
				ts_len += n;
			}
		}
	}

	if (use_color()) {
		n = sprintf(ts_str + ts_pos, "%s", COLOR_OFF);
		if (n > 0)
			ts_pos += n;
	}

	if (use_color()) {
		n = sprintf(line + pos, "%s", color);
		if (n > 0)
			pos += n;
	}

	n = sprintf(line + pos, "%c %s", ident, label);
	if (n > 0) {
		pos += n;
		len += n;
	}

	if (text) {
		int extra_len = extra ? strlen(extra) : 0;
		int max_len = col - len - extra_len - ts_len - 3;

		n = snprintf(line + pos, max_len + 1, ": %s", text);
		if (n > max_len) {
			line[pos + max_len - 1] = '.';
			line[pos + max_len - 2] = '.';
			if (line[pos + max_len - 3] == ' ')
				line[pos + max_len - 3] = '.';

			n = max_len;
		}

		if (n > 0) {
			pos += n;
			len += n;
		}
	}

	if (use_color()) {
		n = sprintf(line + pos, "%s", COLOR_OFF);
		if (n > 0)
			pos += n;
	}

	if (extra) {
		n = sprintf(line + pos, " %s", extra);
		if (n > 0) {
			pos += n;
			len += n;
		}
	}

	if (ts_len > 0) {
		printf("%s", line);
		if (len < col)
			print_space(col - len - ts_len - 1);
		printf("%s%s\n", use_color() ? COLOR_TIMESTAMP : "", ts_str);
	} else
		printf("%s\n", line);
}

static const struct {
	uint8_t error;
	const char *str;
} error2str_table[] = {
	{ 0x00, "Success"						},
	{ 0x01, "Unknown HCI Command"					},
	{ 0x02, "Unknown Connection Identifier"				},
	{ 0x03, "Hardware Failure"					},
	{ 0x04, "Page Timeout"						},
	{ 0x05, "Authentication Failure"				},
	{ 0x06, "PIN or Key Missing"					},
	{ 0x07, "Memory Capacity Exceeded"				},
	{ 0x08, "Connection Timeout"					},
	{ 0x09, "Connection Limit Exceeded"				},
	{ 0x0a, "Synchronous Connection Limit to a Device Exceeded"	},
	{ 0x0b, "ACL Connection Already Exists"				},
	{ 0x0c, "Command Disallowed"					},
	{ 0x0d, "Connection Rejected due to Limited Resources"		},
	{ 0x0e, "Connection Rejected due to Security Reasons"		},
	{ 0x0f, "Connection Rejected due to Unacceptable BD_ADDR"	},
	{ 0x10, "Connection Accept Timeout Exceeded"			},
	{ 0x11, "Unsupported Feature or Parameter Value"		},
	{ 0x12, "Invalid HCI Command Parameters"			},
	{ 0x13, "Remote User Terminated Connection"			},
	{ 0x14, "Remote Device Terminated due to Low Resources"		},
	{ 0x15, "Remote Device Terminated due to Power Off"		},
	{ 0x16, "Connection Terminated By Local Host"			},
	{ 0x17, "Repeated Attempts"					},
	{ 0x18, "Pairing Not Allowed"					},
	{ 0x19, "Unknown LMP PDU"					},
	{ 0x1a, "Unsupported Remote Feature / Unsupported LMP Feature"	},
	{ 0x1b, "SCO Offset Rejected"					},
	{ 0x1c, "SCO Interval Rejected"					},
	{ 0x1d, "SCO Air Mode Rejected"					},
	{ 0x1e, "Invalid LMP Parameters"				},
	{ 0x1f, "Unspecified Error"					},
	{ 0x20, "Unsupported LMP Parameter Value"			},
	{ 0x21, "Role Change Not Allowed"				},
	{ 0x22, "LMP Response Timeout / LL Response Timeout"		},
	{ 0x23, "LMP Error Transaction Collision"			},
	{ 0x24, "LMP PDU Not Allowed"					},
	{ 0x25, "Encryption Mode Not Acceptable"			},
	{ 0x26, "Link Key cannot be Changed"				},
	{ 0x27, "Requested QoS Not Supported"				},
	{ 0x28, "Instant Passed"					},
	{ 0x29, "Pairing With Unit Key Not Supported"			},
	{ 0x2a, "Different Transaction Collision"			},
	{ 0x2b, "Reserved"						},
	{ 0x2c, "QoS Unacceptable Parameter"				},
	{ 0x2d, "QoS Rejected"						},
	{ 0x2e, "Channel Classification Not Supported"			},
	{ 0x2f, "Insufficient Security"					},
	{ 0x30, "Parameter Out Of Manadatory Range"			},
	{ 0x31, "Reserved"						},
	{ 0x32, "Role Switch Pending"					},
	{ 0x33, "Reserved"						},
	{ 0x34, "Reserved Slot Violation"				},
	{ 0x35, "Role Switch Failed"					},
	{ 0x36, "Extended Inquiry Response Too Large"			},
	{ 0x37, "Secure Simple Pairing Not Supported By Host"		},
	{ 0x38, "Host Busy - Pairing"					},
	{ 0x39, "Connection Rejected due to No Suitable Channel Found"	},
	{ 0x3a, "Controller Busy"					},
	{ 0x3b, "Unacceptable Connection Interval"			},
	{ 0x3c, "Directed Advertising Timeout"				},
	{ 0x3d, "Connection Terminated due to MIC Failure"		},
	{ 0x3e, "Connection Failed to be Established"			},
	{ 0x3f, "MAC Connection Failed"					},
	{ }
};

static void print_error(const char *label, uint8_t error)
{
	const char *str = "Unknown";
	const char *color_on, *color_off;
	int i;

	for (i = 0; error2str_table[i].str; i++) {
		if (error2str_table[i].error == error) {
			str = error2str_table[i].str;
			break;
		}
	}

	if (use_color()) {
		if (error)
			color_on = COLOR_RED;
		else
			color_on = COLOR_GREEN;
		color_off = COLOR_OFF;
	} else {
		color_on = "";
		color_off = "";
	}

	print_field("%s: %s%s%s (0x%2.2x)", label,
				color_on, str, color_off, error);
}

static void print_status(uint8_t status)
{
	print_error("Status", status);
}

static void print_reason(uint8_t reason)
{
	print_error("Reason", reason);
}

static void print_bdaddr(const uint8_t *bdaddr)
{
	print_field("Address: %2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X"
					" (OUI %2.2X-%2.2X-%2.2X)",
					bdaddr[5], bdaddr[4], bdaddr[3],
					bdaddr[2], bdaddr[1], bdaddr[0],
					bdaddr[5], bdaddr[4], bdaddr[3]);
}

static void print_addr(const char *label, const uint8_t *addr,
						uint8_t addr_type)
{
	const char *str;

	switch (addr_type) {
	case 0x00:
		print_field("%s: %2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X"
				" (OUI %2.2X-%2.2X-%2.2X)", label,
					addr[5], addr[4], addr[3],
					addr[2], addr[1], addr[0],
					addr[5], addr[4], addr[3]);
		break;
	case 0x01:
		switch ((addr[5] & 0xc0) >> 6) {
		case 0x00:
			str = "Non-Resolvable";
			break;
		case 0x01:
			str = "Resolvable";
			break;
		case 0x03:
			str = "Static";
			break;
		default:
			str = "Reserved";
			break;
		}

		print_field("%s: %2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X (%s)",
					label, addr[5], addr[4], addr[3],
					addr[2], addr[1], addr[0], str);
		break;
	default:
		print_field("%s: %2.2X-%2.2X-%2.2X-%2.2X-%2.2X-%2.2X",
					label, addr[5], addr[4], addr[3],
					addr[2], addr[1], addr[0]);
		break;
	}
}

static void print_addr_type(const char *label, uint8_t addr_type)
{
	const char *str;

	switch (addr_type) {
	case 0x00:
		str = "Public";
		break;
	case 0x01:
		str = "Random";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("%s: %s (0x%2.2x)", label, str, addr_type);
}

static void print_handle(uint16_t handle)
{
	print_field("Handle: %d", btohs(handle));
}

static void print_phy_handle(uint8_t phy_handle)
{
	print_field("Physical handle: %d", phy_handle);
}

static void print_pkt_type(uint16_t pkt_type)
{
	print_field("Packet type: 0x%4.4x", btohs(pkt_type));
}

static void print_iac(const uint8_t *lap)
{
	const char *str = "";

	if (lap[2] == 0x9e && lap[1] == 0x8b) {
		switch (lap[0]) {
		case 0x33:
			str = " (General Inquiry)";
			break;
		case 0x00:
			str = " (Limited Inquiry)";
			break;
		}
	}

	print_field("Access code: 0x%2.2x%2.2x%2.2x%s",
						lap[2], lap[1], lap[0], str);
}

static void print_auth_enable(uint8_t enable)
{
	const char *str;

	switch (enable) {
	case 0x00:
		str = "Authentication not required";
		break;
	case 0x01:
		str = "Authentication required for all connections";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Enable: %s (0x%2.2x)", str, enable);
}

static void print_encrypt_mode(uint8_t mode)
{
	const char *str;

	switch (mode) {
	case 0x00:
		str = "Encryption not required";
		break;
	case 0x01:
		str = "Encryption required for all connections";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Mode: %s (0x%2.2x)", str, mode);
}

static const struct {
	uint8_t bit;
	const char *str;
} svc_class_table[] = {
	{ 0, "Positioning (Location identification)"		},
	{ 1, "Networking (LAN, Ad hoc)"				},
	{ 2, "Rendering (Printing, Speaker)"			},
	{ 3, "Capturing (Scanner, Microphone)"			},
	{ 4, "Object Transfer (v-Inbox, v-Folder)"		},
	{ 5, "Audio (Speaker, Microphone, Headset)"		},
	{ 6, "Telephony (Cordless telephony, Modem, Headset)"	},
	{ 7, "Information (WEB-server, WAP-server)"		},
	{ }
};

static const struct {
	uint8_t val;
	const char *str;
} major_class_computer_table[] = {
	{ 0x00, "Uncategorized, code for device not assigned"	},
	{ 0x01, "Desktop workstation"				},
	{ 0x02, "Server-class computer"				},
	{ 0x03, "Laptop"					},
	{ 0x04, "Handheld PC/PDA (clam shell)"			},
	{ 0x05, "Palm sized PC/PDA"				},
	{ 0x06, "Wearable computer (Watch sized)"		},
	{ 0x07, "Tablet"					},
	{ }
};

static const char *major_class_computer(uint8_t minor)
{
	int i;

	for (i = 0; major_class_computer_table[i].str; i++) {
		if (major_class_computer_table[i].val == minor)
			return major_class_computer_table[i].str;
	}

	return NULL;
}

static const struct {
	uint8_t val;
	const char *str;
} major_class_phone_table[] = {
	{ 0x00, "Uncategorized, code for device not assigned"	},
	{ 0x01, "Cellular"					},
	{ 0x02, "Cordless"					},
	{ 0x03, "Smart phone"					},
	{ 0x04, "Wired modem or voice gateway"			},
	{ 0x05, "Common ISDN Access"				},
	{ }
};

static const char *major_class_phone(uint8_t minor)
{
	int i;

	for (i = 0; major_class_phone_table[i].str; i++) {
		if (major_class_phone_table[i].val == minor)
			return major_class_phone_table[i].str;
	}

	return NULL;
}

static const struct {
	uint8_t val;
	const char *str;
} major_class_av_table[] = {
	{ 0x00, "Uncategorized, code for device not assigned"	},
	{ 0x01, "earable Headset Device"			},
	{ 0x02, "Hands-free Device"				},
	{ 0x04, "Microphone"					},
	{ 0x05, "Loudspeaker"					},
	{ 0x06, "Headphones"					},
	{ 0x07, "Portable Audio"				},
	{ 0x08, "Car audio"					},
	{ 0x09, "Set-top box"					},
	{ 0x0a, "HiFi Audio Device"				},
	{ 0x0b, "VCR"						},
	{ 0x0c, "Video Camera"					},
	{ 0x0d, "Camcorder"					},
	{ 0x0e, "Video Monitor"					},
	{ 0x0f, "Video Display and Loudspeaker"			},
	{ 0x10, "Video Conferencing"				},
	{ 0x12, "Gaming/Toy"					},
	{ }
};

static const char *major_class_av(uint8_t minor)
{
	int i;

	for (i = 0; major_class_av_table[i].str; i++) {
		if (major_class_av_table[i].val == minor)
			return major_class_av_table[i].str;
	}

	return NULL;
}

static const struct {
	uint8_t val;
	const char *str;
} major_class_wearable_table[] = {
	{ 0x01, "Wrist Watch"	},
	{ 0x02, "Pager"		},
	{ 0x03, "Jacket"	},
	{ 0x04, "Helmet"	},
	{ 0x05, "Glasses"	},
	{ }
};

static const char *major_class_wearable(uint8_t minor)
{
	int i;

	for (i = 0; major_class_wearable_table[i].str; i++) {
		if (major_class_wearable_table[i].val == minor)
			return major_class_wearable_table[i].str;
	}

	return NULL;
}

static const struct {
	uint8_t val;
	const char *str;
	const char *(*func)(uint8_t minor);
} major_class_table[] = {
	{ 0x00, "Miscellaneous"						},
	{ 0x01, "Computer (desktop, notebook, PDA, organizers)",
						major_class_computer	},
	{ 0x02, "Phone (cellular, cordless, payphone, modem)",
						major_class_phone	},
	{ 0x03, "LAN /Network Access point"				},
	{ 0x04, "Audio/Video (headset, speaker, stereo, video, vcr)",
						major_class_av		},
	{ 0x05, "Peripheral (mouse, joystick, keyboards)"		},
	{ 0x06, "Imaging (printing, scanner, camera, display)"		},
	{ 0x07, "Wearable",			major_class_wearable	},
	{ 0x08, "Toy"							},
	{ 0x09, "Health"						},
	{ 0x1f, "Uncategorized, specific device code not specified"	},
	{ }
};

static void print_dev_class(const uint8_t *dev_class)
{
	uint8_t mask, major_cls, minor_cls;
	const char *major_str = NULL;
	const char *minor_str = NULL;
	int i;

	print_field("Class: 0x%2.2x%2.2x%2.2x",
			dev_class[2], dev_class[1], dev_class[0]);

	if ((dev_class[0] & 0x03) != 0x00) {
		print_field("  Format type: 0x%2.2x", dev_class[0] & 0x03);
		print_text(COLOR_ERROR, "  invalid format type");
		return;
	}

	major_cls = dev_class[1] & 0x1f;
	minor_cls = (dev_class[0] & 0xfc) >> 2;

	for (i = 0; major_class_table[i].str; i++) {
		if (major_class_table[i].val == major_cls) {
			major_str = major_class_table[i].str;

			if (!major_class_table[i].func)
				break;

			minor_str = major_class_table[i].func(minor_cls);
			break;
		}
	}

	if (major_str) {
		print_field("  Major class: %s", major_str);
		if (minor_str)
			print_field("  Minor class: %s", minor_str);
		else
			print_field("  Minor class: 0x%2.2x", minor_cls);
	} else {
		print_field("  Major class: 0x%2.2x", major_cls);
		print_field("  Minor class: 0x%2.2x", minor_cls);
	}

	if (dev_class[1] & 0x20)
		print_field("  Limited Discoverable Mode");

	if ((dev_class[1] & 0xc0) != 0x00) {
		print_text(COLOR_ERROR, "  invalid service class");
		return;
	}

	mask = dev_class[2];

	for (i = 0; svc_class_table[i].str; i++) {
		if (dev_class[2] & (1 << svc_class_table[i].bit)) {
			print_field("  %s", svc_class_table[i].str);
			mask &= ~(1 << svc_class_table[i].bit);
		}
	}

	if (mask)
		print_text(COLOR_UNKNOWN_SERVICE_CLASS,
				"  Unknown service class (0x%2.2x)", mask);
}

static void print_voice_setting(uint16_t setting)
{
	print_field("Setting: 0x%4.4x", btohs(setting));
}

static void print_retransmission_effort(uint8_t effort)
{
	const char *str;

	switch (effort) {
	case 0x00:
		str = "No retransmissions";
		break;
	case 0x01:
		str = "Optimize for power consumption";
		break;
	case 0x02:
		str = "Optimize for link quality";
		break;
	case 0xff:
		str = "Don't care";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Retransmission effort: %s (0x%2.2x)", str, effort);
}

static void print_scan_enable(uint8_t scan_enable)
{
	const char *str;

	switch (scan_enable) {
	case 0x00:
		str = "No Scans";
		break;
	case 0x01:
		str = "Inquiry Scan";
		break;
	case 0x02:
		str = "Page Scan";
		break;
	case 0x03:
		str = "Inquiry Scan + Page Scan";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Scan enable: %s (0x%2.2x)", str, scan_enable);
}

static void print_link_policy(uint16_t link_policy)
{
	print_field("Link policy: 0x%4.4x", btohs(link_policy));
}

static void print_air_mode(uint8_t mode)
{
	const char *str;

	switch (mode) {
	case 0x00:
		str = "u-law log";
		break;
	case 0x01:
		str = "A-law log";
		break;
	case 0x02:
		str = "CVSD";
		break;
	case 0x03:
		str = "Transparent";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Air mode: %s (0x%2.2x)", str, mode);
}

static void print_inquiry_mode(uint8_t mode)
{
	const char *str;

	switch (mode) {
	case 0x00:
		str = "Standard Inquiry Result";
		break;
	case 0x01:
		str = "Inquiry Result with RSSI";
		break;
	case 0x02:
		str = "Inquiry Result with RSSI or Extended Inquiry Result";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Mode: %s (0x%2.2x)", str, mode);
}

static void print_inquiry_scan_type(uint8_t type)
{
	const char *str;

	switch (type) {
	case 0x00:
		str = "Standard Scan";
		break;
	case 0x01:
		str = "Interlaced Scan";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Type: %s (0x%2.2x)", str, type);
}

static void print_pscan_type(uint8_t type)
{
	const char *str;

	switch (type) {
	case 0x00:
		str = "Standard Scan";
		break;
	case 0x01:
		str = "Interlaced Scan";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Type: %s (0x%2.2x)", str, type);
}

static void print_afh_mode(uint8_t mode)
{
	const char *str;

	switch (mode) {
	case 0x00:
		str = "Disabled";
		break;
	case 0x01:
		str = "Enabled";
		break;
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Mode: %s (0x%2.2x)", str, mode);
}

static void print_simple_pairing_mode(uint8_t mode)
{
	const char *str;

	switch (mode) {
	case 0x00:
		str = "Disabled";
		break;
	case 0x01:
		str = "Enabled";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Mode: %s (0x%2.2x)", str, mode);
}

static void print_pscan_rep_mode(uint8_t pscan_rep_mode)
{
	const char *str;

	switch (pscan_rep_mode) {
	case 0x00:
		str = "R0";
		break;
	case 0x01:
		str = "R1";
		break;
	case 0x02:
		str = "R2";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Page scan repetition mode: %s (0x%2.2x)",
						str, pscan_rep_mode);
}

static void print_pscan_period_mode(uint8_t pscan_period_mode)
{
	const char *str;

	switch (pscan_period_mode) {
	case 0x00:
		str = "P0";
		break;
	case 0x01:
		str = "P1";
		break;
	case 0x02:
		str = "P2";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Page period mode: %s (0x%2.2x)", str, pscan_period_mode);
}

static void print_pscan_mode(uint8_t pscan_mode)
{
	const char *str;

	switch (pscan_mode) {
	case 0x00:
		str = "Mandatory";
		break;
	case 0x01:
		str = "Optional I";
		break;
	case 0x02:
		str = "Optional II";
		break;
	case 0x03:
		str = "Optional III";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Page scan mode: %s (0x%2.2x)", str, pscan_mode);
}

static void print_clock_offset(uint16_t clock_offset)
{
	print_field("Clock offset: 0x%4.4x", btohs(clock_offset));
}

static void print_clock(uint32_t clock)
{
	print_field("Clock: 0x%8.8x", btohl(clock));
}

static void print_clock_accuracy(uint16_t accuracy)
{
	if (btohs(accuracy) == 0xffff)
		print_field("Accuracy: Unknown (0x%4.4x)", btohs(accuracy));
	else
		print_field("Accuracy: %.4f msec (0x%4.4x)",
				btohs(accuracy) * 0.3125, btohs(accuracy));
}

static void print_link_type(uint8_t link_type)
{
	const char *str;

	switch (link_type) {
	case 0x00:
		str = "SCO";
		break;
	case 0x01:
		str = "ACL";
		break;
	case 0x02:
		str = "eSCO";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Link type: %s (0x%2.2x)", str, link_type);
}

static void print_encr_mode(uint8_t encr_mode)
{
	const char *str;

	switch (encr_mode) {
	case 0x00:
		str = "Disabled";
		break;
	case 0x01:
		str = "Enabled";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Encryption: %s (0x%2.2x)", str, encr_mode);
}

static void print_encr_mode_change(uint8_t encr_mode, uint16_t handle)
{
	const char *str;
	uint8_t conn_type;

	conn_type = get_type(btohs(handle));

	switch (encr_mode) {
	case 0x00:
		str = "Disabled";
		break;
	case 0x01:
		switch (conn_type) {
		case 0x00:
			str = "Enabled with E0";
			break;
		case 0x01:
			str = "Enabled with AES-CCM";
			break;
		default:
			str = "Enabled";
			break;
		}
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Encryption: %s (0x%2.2x)", str, encr_mode);
}

static void print_pin_type(uint8_t pin_type)
{
	const char *str;

	switch (pin_type) {
	case 0x00:
		str = "Variable";
		break;
	case 0x01:
		str = "Fixed";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("PIN type: %s (0x%2.2x)", str, pin_type);
}

static void print_key_flag(uint8_t key_flag)
{
	const char *str;

	switch (key_flag) {
	case 0x00:
		str = "Semi-permanent";
		break;
	case 0x01:
		str = "Temporary";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Key flag: %s (0x%2.2x)", str, key_flag);
}

static void print_key_len(uint8_t key_len)
{
	const char *str;

	switch (key_len) {
	case 32:
		str = "802.11 PAL";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Key length: %s (%d)", str, key_len);
}

static void print_key_type(uint8_t key_type)
{
	const char *str;

	switch (key_type) {
	case 0x00:
		str = "Combination key";
		break;
	case 0x01:
		str = "Local Unit key";
		break;
	case 0x02:
		str = "Remote Unit key";
		break;
	case 0x03:
		str = "Debug Combination key";
		break;
	case 0x04:
		str = "Unauthenticated Combination key";
		break;
	case 0x05:
		str = "Authenticated Combination key";
		break;
	case 0x06:
		str = "Changed Combination key";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Key type: %s (0x%2.2x)", str, key_type);
}

static void print_key_size(uint8_t key_size)
{
	print_field("Key size: %d", key_size);
}

static void print_hex_field(const char *label, const uint8_t *data,
								uint8_t len)
{
	char str[len * 2 + 1];
	uint8_t i;

	str[0] = '\0';

	for (i = 0; i < len; i++)
		sprintf(str + (i * 2), "%2.2x", data[i]);

	print_field("%s: %s", label, str);
}

static void print_key(const char *label, const uint8_t *link_key)
{
	print_hex_field(label, link_key, 16);
}

static void print_link_key(const uint8_t *link_key)
{
	print_key("Link key", link_key);
}

static void print_pin_code(const uint8_t *pin_code, uint8_t pin_len)
{
	char str[pin_len + 1];
	uint8_t i;

	for (i = 0; i < pin_len; i++)
		sprintf(str + i, "%c", (const char) pin_code[i]);

	print_field("PIN code: %s", str);
}

static void print_hash(const char *label, const uint8_t *hash)
{
	print_key("Hash C from %s", hash);
}

static void print_randomizer(const char *label, const uint8_t *randomizer)
{
	print_key("Randomizer R with %s", randomizer);
}

static void print_passkey(uint32_t passkey)
{
	print_field("Passkey: %06d", btohl(passkey));
}

static void print_io_capability(uint8_t capability)
{
	const char *str;

	switch (capability) {
	case 0x00:
		str = "DisplayOnly";
		break;
	case 0x01:
		str = "DisplayYesNo";
		break;
	case 0x02:
		str = "KeyboardOnly";
		break;
	case 0x03:
		str = "NoInputNoOutput";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("IO capability: %s (0x%2.2x)", str, capability);
}

static void print_oob_data(uint8_t oob_data)
{
	const char *str;

	switch (oob_data) {
	case 0x00:
		str = "Authentication data not present";
		break;
	case 0x01:
		str = "Authentication data present";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("OOB data: %s (0x%2.2x)", str, oob_data);
}

static void print_authentication(uint8_t authentication)
{
	const char *str;

	switch (authentication) {
	case 0x00:
		str = "No Bonding - MITM not required";
		break;
	case 0x01:
		str = "No Bonding - MITM required";
		break;
	case 0x02:
		str = "Dedicated Bonding - MITM not required";
		break;
	case 0x03:
		str = "Dedicated Bonding - MITM required";
		break;
	case 0x04:
		str = "General Bonding - MITM not required";
		break;
	case 0x05:
		str = "General Bonding - MITM required";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Authentication: %s (0x%2.2x)", str, authentication);
}

static void print_location_domain_aware(uint8_t aware)
{
	const char *str;

	switch (aware) {
	case 0x00:
		str = "Regulatory domain unknown";
		break;
	case 0x01:
		str = "Regulatory domain known";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Domain aware: %s (0x%2.2x)", str, aware);
}

static void print_location_domain(const uint8_t *domain)
{
	print_field("Domain: %c%c (0x%2.2x%2.2x)",
		(char) domain[0], (char) domain[1], domain[0], domain[1]);
}

static void print_location_domain_options(uint8_t options)
{
	print_field("Domain options: %c (0x%2.2x)", (char) options, options);
}

static void print_location_options(uint8_t options)
{
	print_field("Options: 0x%2.2x", options);
}

static void print_flow_control_mode(uint8_t mode)
{
	const char *str;

	switch (mode) {
	case 0x00:
		str = "Packet based";
		break;
	case 0x01:
		str = "Data block based";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Flow control mode: %s (0x%2.2x)", str, mode);
}

static void print_flow_direction(uint8_t direction)
{
	const char *str;

	switch (direction) {
	case 0x00:
		str = "Outgoing";
		break;
	case 0x01:
		str = "Incoming";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Flow direction: %s (0x%2.2x)", str, direction);
}

static void print_service_type(uint8_t service_type)
{
	const char *str;

	switch (service_type) {
	case 0x00:
		str = "No Traffic";
		break;
	case 0x01:
		str = "Best Effort";
		break;
	case 0x02:
		str = "Guaranteed";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Service type: %s (0x%2.2x)", str, service_type);
}

static void print_flow_spec(const char *label, const uint8_t *data)
{
	const char *str;

	switch (data[1]) {
	case 0x00:
		str = "No traffic";
		break;
	case 0x01:
		str = "Best effort";
		break;
	case 0x02:
		str = "Guaranteed";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("%s flow spec: 0x%2.2x", label, data[0]);
	print_field("  Service type: %s (0x%2.2x)", str, data[1]);
	print_field("  Maximum SDU size: 0x%4.4x", bt_get_le16(data + 2));
	print_field("  SDU inter-arrival time: 0x%8.8x", bt_get_le32(data + 4));
	print_field("  Access latency: 0x%8.8x", bt_get_le32(data + 8));
	print_field("  Flush timeout: 0x%8.8x", bt_get_le32(data + 12));
}

static void print_short_range_mode(uint8_t mode)
{
	const char *str;

	switch (mode) {
	case 0x00:
		str = "Disabled";
		break;
	case 0x01:
		str = "Enabled";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Short range mode: %s (0x%2.2x)", str, mode);
}

static void print_amp_status(uint8_t amp_status)
{
	const char *str;

	switch (amp_status) {
	case 0x00:
		str = "Present";
		break;
	case 0x01:
		str = "Bluetooth only";
		break;
	case 0x02:
		str = "No capacity";
		break;
	case 0x03:
		str = "Low capacity";
		break;
	case 0x04:
		str = "Medium capacity";
		break;
	case 0x05:
		str = "High capacity";
		break;
	case 0x06:
		str = "Full capacity";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("AMP status: %s (0x%2.2x)", str, amp_status);
}

static void print_num_resp(uint8_t num_resp)
{
	print_field("Num responses: %d", num_resp);
}

static void print_num_reports(uint8_t num_reports)
{
	print_field("Num reports: %d", num_reports);
}

static void print_rssi(int8_t rssi)
{
	if ((uint8_t) rssi == 0x99 || rssi == 127)
		print_field("RSSI: invalid (0x%2.2x)", (uint8_t) rssi);
	else
		print_field("RSSI: %d dBm (0x%2.2x)", rssi, (uint8_t) rssi);
}

static void print_slot_625(const char *label, uint16_t value)
{
	 print_field("%s: %.3f msec (0x%4.4x)", label,
					btohs(value) * 0.625, btohs(value));
}

static void print_slot_125(const char *label, uint16_t value)
{
	print_field("%s: %.2f msec (0x%4.4x)", label,
					btohs(value) * 1.25, btohs(value));
}

static void print_timeout(uint16_t timeout)
{
	print_slot_625("Timeout", timeout);
}

static void print_interval(uint16_t interval)
{
	print_slot_625("Interval", interval);
}

static void print_window(uint16_t window)
{
	print_slot_625("Window", window);
}

static void print_role(uint8_t role)
{
	const char *str;

	switch (role) {
	case 0x00:
		str = "Master";
		break;
	case 0x01:
		str = "Slave";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Role: %s (0x%2.2x)", str, role);
}

static void print_mode(uint8_t mode)
{
	const char *str;

	switch (mode) {
	case 0x00:
		str = "Active";
		break;
	case 0x01:
		str = "Hold";
		break;
	case 0x02:
		str = "Sniff";
		break;
	case 0x03:
		str = "Park";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Mode: %s (0x%2.2x)", str, mode);
}

static void print_name(const uint8_t *name)
{
	char str[249];

	memcpy(str, name, 248);
	str[248] = '\0';

	print_field("Name: %s", str);
}

static void print_channel_map(const uint8_t *map)
{
	char str[21];
	int i;

	for (i = 0; i < 10; i++)
		sprintf(str + (i * 2), "%2.2x", map[i]);

	print_field("Channel map: 0x%s", str);
}

void packet_print_version(const char *label, uint8_t version,
				const char *sublabel, uint16_t subversion)
{
	const char *str;

	switch (version) {
	case 0x00:
		str = "Bluetooth 1.0b";
		break;
	case 0x01:
		str = "Bluetooth 1.1";
		break;
	case 0x02:
		str = "Bluetooth 1.2";
		break;
	case 0x03:
		str = "Bluetooth 2.0";
		break;
	case 0x04:
		str = "Bluetooth 2.1";
		break;
	case 0x05:
		str = "Bluetooth 3.0";
		break;
	case 0x06:
		str = "Bluetooth 4.0";
		break;
	case 0x07:
		str = "Bluetooth 4.1";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("%s: %s (0x%2.2x) - %s %d (0x%4.4x)", label, str, version,
					sublabel, subversion, subversion);
}

static void print_hci_version(uint8_t version, uint16_t revision)
{
	packet_print_version("HCI version", version,
				"Revision", btohs(revision));
}

static void print_lmp_version(uint8_t version, uint16_t subversion)
{
	packet_print_version("LMP version", version,
				"Subversion", btohs(subversion));
}

static void print_pal_version(uint8_t version, uint16_t subversion)
{
	const char *str;

	switch (version) {
	case 0x01:
		str = "Bluetooth 3.0";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("PAL version: %s (0x%2.2x) - Subversion %d (0x%4.4x)",
			str, version, btohs(subversion), btohs(subversion));
}

void packet_print_company(const char *label, uint16_t company)
{
	print_field("%s: %s (%d)", label, bt_compidtostr(company), company);
}

static void print_manufacturer(uint16_t manufacturer)
{
	packet_print_company("Manufacturer", btohs(manufacturer));
}

static const char *get_supported_command(int bit);

static void print_commands(const uint8_t *commands)
{
	unsigned int count = 0;
	int i, n;

	for (i = 0; i < 64; i++) {
		for (n = 0; n < 8; n++) {
			if (commands[i] & (1 << n))
				count++;
		}
	}

	print_field("Commands: %u entr%s", count, count == 1 ? "y" : "ies");

	for (i = 0; i < 64; i++) {
		for (n = 0; n < 8; n++) {
			const char *cmd;

			if (!(commands[i] & (1 << n)))
				continue;

			cmd = get_supported_command((i * 8) + n);
			print_field("  %s (Octet %d - Bit %d)", cmd, i, n);
		}
	}
}

struct features_data {
	uint8_t bit;
	const char *str;
};

static const struct features_data features_page0[] = {
	{  0, "3 slot packets"				},
	{  1, "5 slot packets"				},
	{  2, "Encryption"				},
	{  3, "Slot offset"				},
	{  4, "Timing accuracy"				},
	{  5, "Role switch"				},
	{  6, "Hold mode"				},
	{  7, "Sniff mode"				},
	{  8, "Park state"				},
	{  9, "Power control requests"			},
	{ 10, "Channel quality driven data rate (CQDDR)"},
	{ 11, "SCO link"				},
	{ 12, "HV2 packets"				},
	{ 13, "HV3 packets"				},
	{ 14, "u-law log synchronous data"		},
	{ 15, "A-law log synchronous data"		},
	{ 16, "CVSD synchronous data"			},
	{ 17, "Paging parameter negotiation"		},
	{ 18, "Power control"				},
	{ 19, "Transparent synchronous data"		},
	{ 20, "Flow control lag (least significant bit)"},
	{ 21, "Flow control lag (middle bit)"		},
	{ 22, "Flow control lag (most significant bit)"	},
	{ 23, "Broadcast Encryption"			},
	{ 25, "Enhanced Data Rate ACL 2 Mbps mode"	},
	{ 26, "Enhanced Data Rate ACL 3 Mbps mode"	},
	{ 27, "Enhanced inquiry scan"			},
	{ 28, "Interlaced inquiry scan"			},
	{ 29, "Interlaced page scan"			},
	{ 30, "RSSI with inquiry results"		},
	{ 31, "Extended SCO link (EV3 packets)"		},
	{ 32, "EV4 packets"				},
	{ 33, "EV5 packets"				},
	{ 35, "AFH capable slave"			},
	{ 36, "AFH classification slave"		},
	{ 37, "BR/EDR Not Supported"			},
	{ 38, "LE Supported (Controller)"		},
	{ 39, "3-slot Enhanced Data Rate ACL packets"	},
	{ 40, "5-slot Enhanced Data Rate ACL packets"	},
	{ 41, "Sniff subrating"				},
	{ 42, "Pause encryption"			},
	{ 43, "AFH capable master"			},
	{ 44, "AFH classification master"		},
	{ 45, "Enhanced Data Rate eSCO 2 Mbps mode"	},
	{ 46, "Enhanced Data Rate eSCO 3 Mbps mode"	},
	{ 47, "3-slot Enhanced Data Rate eSCO packets"	},
	{ 48, "Extended Inquiry Response"		},
	{ 49, "Simultaneous LE and BR/EDR (Controller)"	},
	{ 51, "Secure Simple Pairing"			},
	{ 52, "Encapsulated PDU"			},
	{ 53, "Erroneous Data Reporting"		},
	{ 54, "Non-flushable Packet Boundary Flag"	},
	{ 56, "Link Supervision Timeout Changed Event"	},
	{ 57, "Inquiry TX Power Level"			},
	{ 58, "Enhanced Power Control"			},
	{ 63, "Extended features"			},
	{ }
};

static const struct features_data features_page1[] = {
	{  0, "Secure Simple Pairing (Host Support)"	},
	{  1, "LE Supported (Host)"			},
	{  2, "Simultaneous LE and BR/EDR (Host)"	},
	{ }
};

static const struct features_data features_page2[] = {
	{  0, "Connectionless Slave Broadcast - Master"	},
	{  1, "Connectionless Slave Broadcast - Slave"	},
	{  2, "Synchronization Train"			},
	{  3, "Synchronization Scan"			},
	{  4, "Inquiry Response Notification Event"	},
	{ }
};

static const struct features_data features_le[] = {
	{  0, "LE Encryption"				},
	{ }
};

static void print_features(uint8_t page, const uint8_t *features_array,
								uint8_t type)
{
	const struct features_data *features_table = NULL;
	uint64_t mask, features = 0;
	char str[41];
	int i;

	for (i = 0; i < 8; i++) {
		sprintf(str + (i * 5), " 0x%2.2x", features_array[i]);
		features |= ((uint64_t) features_array[i]) << (i * 8);
	}

	print_field("Features:%s", str);

	switch (type) {
	case 0x00:
		switch (page) {
		case 0:
			features_table = features_page0;
			break;
		case 1:
			features_table = features_page1;
			break;
		case 2:
			features_table = features_page2;
			break;
		}
		break;
	case 0x01:
		switch (page) {
		case 0:
			features_table = features_le;
			break;
		}
		break;
	}

	if (!features_table)
		return;

	mask = features;

	for (i = 0; features_table[i].str; i++) {
		if (features & (((uint64_t) 1) << features_table[i].bit)) {
			print_field("  %s", features_table[i].str);
			mask &= ~(((uint64_t) 1) << features_table[i].bit);
		}
	}

	if (mask)
		print_text(COLOR_UNKNOWN_FEATURE_BIT, "  Unknown features "
						"(0x%16.16" PRIx64 ")", mask);
}

void packet_print_features_ll(const uint8_t *features)
{
	print_features(0, features, 0x01);
}

static const struct {
	uint8_t bit;
	const char *str;
} le_states_table[] = {
	{  0, "Non-connectable Advertising State"			},
	{  1, "Scannable Advertising State"				},
	{  2, "Connectable Advertising State"				},
	{  3, "Directed Advertising State"				},
	{  4, "Passive Scanning State"					},
	{  5, "Active Scanning State"					},
	{  6, "Initiating State and Connection State in Master Role"	},
	{  7, "Connection State in Slave Role"				},
	{  8, "Non-connectable Advertising State and "
				"Passive Scanning State combination"	},
	{  9, "Scannable Advertising State and "
				"Passive Scanning State combination"	},
	{ 10, "Connectable Advertising State and "
				"Passive Scanning State combination"	},
	{ 11, "Directed Advertising State and "
				"Passive Scanning State combination"	},
	{ 12, "Non-connectable Advertising State and "
				"Active Scanning State combination"	},
	{ 13, "Scannable Advertising State and "
				"Active Scanning State combination"	},
	{ 14, "Connectable Advertising State and "
				"Active Scanning State combination"	},
	{ 15, "Directed Advertising State and "
				"Active Scanning State combination"	},
	{ 16, "Non-connectable Advertising State and "
				"Initiating State combination"		},
	{ 17, "Scannable Advertising State and "
				"Initiating State combination"		},
	{ 18, "Non-connectable Advertising State and "
				"Mater Role combination"		},
	{ 19, "Scannable Advertising State and "
				"Master Role combination"		},
	{ 20, "Non-connectable Advertising State and "
				"Slave Role combination"		},
	{ 21, "Scannable Advertising State and "
				"Slave Role combination"		},
	{ 22, "Passive Scanning State and Initiating State combination"	},
	{ 23, "Active Scanning State and Initiating State combination"	},
	{ 24, "Passive Scanning State and Master Role combination"	},
	{ 25, "Active Scanning State and Master Role combination"	},
	{ 26, "Passive Scanning State and Slave Role combination"	},
	{ 27, "Active Scanning State and Slave Role combination"	},
	{ 28, "Initiating State and Master Role combination"		},
	{ }
};

static void print_le_states(const uint8_t *states_array)
{
	uint64_t mask, states = 0;
	int i;

	for (i = 0; i < 8; i++)
		states |= ((uint64_t) states_array[i]) << (i * 8);

	print_field("States: 0x%16.16" PRIx64, states);

	mask = states;

	for (i = 0; le_states_table[i].str; i++) {
		if (states & (((uint64_t) 1) << le_states_table[i].bit)) {
			print_field("  %s", le_states_table[i].str);
			mask &= ~(((uint64_t) 1) << le_states_table[i].bit);
		}
	}

	if (mask)
		print_text(COLOR_UNKNOWN_LE_STATES, "  Unknown states "
						"(0x%16.16" PRIx64 ")", mask);
}

static void print_le_channel_map(const uint8_t *map)
{
	char str[11];
	int i;

	for (i = 0; i < 5; i++)
		sprintf(str + (i * 2), "%2.2x", map[i]);

	print_field("Channel map: 0x%s", str);
}

void packet_print_channel_map_ll(const uint8_t *map)
{
	print_le_channel_map(map);
}

static void print_random_number(const uint8_t *number)
{
	print_hex_field("Random number", number, 8);
}

static const struct {
	uint8_t bit;
	const char *str;
} events_table[] = {
	{  0, "Inquiry Complete"					},
	{  1, "Inquiry Result"						},
	{  2, "Connection Complete"					},
	{  3, "Connection Request"					},
	{  4, "Disconnection Complete"					},
	{  5, "Authentication Complete"					},
	{  6, "Remote Name Request Complete"				},
	{  7, "Encryption Change"					},
	{  8, "Change Connection Link Key Complete"			},
	{  9, "Master Link Key Complete"				},
	{ 10, "Read Remote Supported Features Complete"			},
	{ 11, "Read Remote Version Information Complete"		},
	{ 12, "QoS Setup Complete"					},
	{ 13, "Command Complete"					},
	{ 14, "Command Status"						},
	{ 15, "Hardware Error"						},
	{ 16, "Flush Occurred"						},
	{ 17, "Role Change"						},
	{ 18, "Number of Completed Packets"				},
	{ 19, "Mode Change"						},
	{ 20, "Return Link Keys"					},
	{ 21, "PIN Code Request"					},
	{ 22, "Link Key Request"					},
	{ 23, "Link Key Notification"					},
	{ 24, "Loopback Command"					},
	{ 25, "Data Buffer Overflow"					},
	{ 26, "Max Slots Change"					},
	{ 27, "Read Clock Offset Complete"				},
	{ 28, "Connection Packet Type Changed"				},
	{ 29, "QoS Violation"						},
	{ 30, "Page Scan Mode Change"					},
	{ 31, "Page Scan Repetition Mode Change"			},
	{ 32, "Flow Specification Complete"				},
	{ 33, "Inquiry Result with RSSI"				},
	{ 34, "Read Remote Extended Features Complete"			},
	{ 43, "Synchronous Connection Complete"				},
	{ 44, "Synchronous Connection Changed"				},
	{ 45, "Sniff Subrating"						},
	{ 46, "Extended Inquiry Result"					},
	{ 47, "Encryption Key Refresh Complete"				},
	{ 48, "IO Capability Request"					},
	{ 49, "IO Capability Request Reply"				},
	{ 50, "User Confirmation Request"				},
	{ 51, "User Passkey Request"					},
	{ 52, "Remote OOB Data Request"					},
	{ 53, "Simple Pairing Complete"					},
	{ 55, "Link Supervision Timeout Changed"			},
	{ 56, "Enhanced Flush Complete"					},
	{ 58, "User Passkey Notification"				},
	{ 59, "Keypress Notification"					},
	{ 60, "Remote Host Supported Features Notification"		},
	{ 61, "LE Meta"							},
	{ }
};

static void print_event_mask(const uint8_t *events_array)
{
	uint64_t mask, events = 0;
	int i;

	for (i = 0; i < 8; i++)
		events |= ((uint64_t) events_array[i]) << (i * 8);

	print_field("Mask: 0x%16.16" PRIx64, events);

	mask = events;

	for (i = 0; events_table[i].str; i++) {
		if (events & (((uint64_t) 1) << events_table[i].bit)) {
			print_field("  %s", events_table[i].str);
			mask &= ~(((uint64_t) 1) << events_table[i].bit);
		}
	}

	if (mask)
		print_text(COLOR_UNKNOWN_EVENT_MASK, "  Unknown mask "
						"(0x%16.16" PRIx64 ")", mask);
}

static const struct {
	uint8_t bit;
	const char *str;
} events_page2_table[] = {
	{  0, "Physical Link Complete"					},
	{  1, "Channel Selected"					},
	{  2, "Disconnection Physical Link Complete"			},
	{  3, "Physical Link Loss Early Warning"			},
	{  4, "Physical Link Recovery"					},
	{  5, "Logical Link Complete"					},
	{  6, "Disconnection Logical Link Complete"			},
	{  7, "Flow Specification Modify Complete"			},
	{  8, "Number of Completed Data Blocks"				},
	{  9, "AMP Start Test"						},
	{ 10, "AMP Test End"						},
	{ 11, "AMP Receiver Report"					},
	{ 12, "Short Range Mode Change Complete"			},
	{ 13, "AMP Status Change"					},
	{ 14, "Triggered Clock Capture"					},
	{ 15, "Synchronization Train Complete"				},
	{ 16, "Synchronization Train Received"				},
	{ 17, "Connectionless Slave Broadcast Receive"			},
	{ 18, "Connectionless Slave Broadcast Timeout"			},
	{ 19, "Truncated Page Complete"					},
	{ 20, "Slave Page Response Timeout"				},
	{ 21, "Connectionless Slave Broadcast Channel Map Change"	},
	{ 22, "Inquiry Response Notification"				},
	{ }
};

static void print_event_mask_page2(const uint8_t *events_array)
{
	uint64_t mask, events = 0;
	int i;

	for (i = 0; i < 8; i++)
		events |= ((uint64_t) events_array[i]) << (i * 8);

	print_field("Mask: 0x%16.16" PRIx64, events);

	mask = events;

	for (i = 0; events_page2_table[i].str; i++) {
		if (events & (((uint64_t) 1) << events_page2_table[i].bit)) {
			print_field("  %s", events_page2_table[i].str);
			mask &= ~(((uint64_t) 1) << events_page2_table[i].bit);
		}
	}

	if (mask)
		print_text(COLOR_UNKNOWN_EVENT_MASK, "  Unknown mask "
						"(0x%16.16" PRIx64 ")", mask);
}

static const struct {
	uint8_t bit;
	const char *str;
} events_le_table[] = {
	{  0, "LE Connection Complete"		},
	{  1, "LE Advertising Report"		},
	{  2, "LE Connection Update Complete"	},
	{  3, "LE Read Remote Used Features"	},
	{  4, "LE Long Term Key Request"	},
	{ }
};

static void print_event_mask_le(const uint8_t *events_array)
{
	uint64_t mask, events = 0;
	int i;

	for (i = 0; i < 8; i++)
		events |= ((uint64_t) events_array[i]) << (i * 8);

	print_field("Mask: 0x%16.16" PRIx64, events);

	mask = events;

	for (i = 0; events_le_table[i].str; i++) {
		if (events & (((uint64_t) 1) << events_le_table[i].bit)) {
			print_field("  %s", events_le_table[i].str);
			mask &= ~(((uint64_t) 1) << events_le_table[i].bit);
		}
	}

	if (mask)
		print_text(COLOR_UNKNOWN_EVENT_MASK, "  Unknown mask "
						"(0x%16.16" PRIx64 ")", mask);
}

static void print_fec(uint8_t fec)
{
	const char *str;

	switch (fec) {
	case 0x00:
		str = "Not required";
		break;
	case 0x01:
		str = "Required";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("FEC: %s (0x%02x)", str, fec);
}

#define BT_EIR_FLAGS			0x01
#define BT_EIR_UUID16_SOME		0x02
#define BT_EIR_UUID16_ALL		0x03
#define BT_EIR_UUID32_SOME		0x04
#define BT_EIR_UUID32_ALL		0x05
#define BT_EIR_UUID128_SOME		0x06
#define BT_EIR_UUID128_ALL		0x07
#define BT_EIR_NAME_SHORT		0x08
#define BT_EIR_NAME_COMPLETE		0x09
#define BT_EIR_TX_POWER			0x0a
#define BT_EIR_CLASS_OF_DEV		0x0d
#define BT_EIR_SSP_HASH_P192		0x0e
#define BT_EIR_SSP_RANDOMIZER_P192	0x0f
#define BT_EIR_DEVICE_ID		0x10
#define BT_EIR_SMP_TK			0x10
#define BT_EIR_SMP_OOB_FLAGS		0x11
#define BT_EIR_SLAVE_CONN_INTERVAL	0x12
#define BT_EIR_SERVICE_UUID16		0x14
#define BT_EIR_SERVICE_UUID128		0x15
#define BT_EIR_SERVICE_DATA		0x16
#define BT_EIR_PUBLIC_ADDRESS		0x17
#define BT_EIR_RANDOM_ADDRESS		0x18
#define BT_EIR_GAP_APPEARANCE		0x19
#define BT_EIR_ADVERTISING_INTERVAL	0x1a
#define BT_EIR_LE_DEVICE_ADDRESS	0x1b
#define BT_EIR_LE_ROLE			0x1c
#define BT_EIR_SSP_HASH_P256		0x1d
#define BT_EIR_SSP_RANDOMIZER_P256	0x1e
#define BT_EIR_3D_INFO_DATA		0x3d
#define BT_EIR_MANUFACTURER_DATA	0xff

static void print_manufacturer_apple(const void *data, uint8_t data_len)
{
	uint8_t type = *((uint8_t *) data);
	uint8_t len;
	const uint8_t *uuid;
	uint16_t minor, major;
	int8_t tx_power;
	char identifier[100];

	if (data_len < 1)
		return;

	switch (type) {
	case 0x01:
		snprintf(identifier, sizeof(identifier) - 1, "%s",
						(const char *) (data + 1));
		print_field("  Identifier: %s", identifier);
		break;
	case 0x02:
		len = *((uint8_t *) (data + 1));
		if (len != 0x15) {
			print_hex_field("  Data", data, data_len);
			break;
		}

		uuid = data + 2;
		print_field("  iBeacon: %8.8x-%4.4x-%4.4x-%4.4x-%8.8x%4.4x",
				bt_get_le32(&uuid[12]), bt_get_le16(&uuid[10]),
				bt_get_le16(&uuid[8]), bt_get_le16(&uuid[6]),
				bt_get_le32(&uuid[2]), bt_get_le16(&uuid[0]));

		major = bt_get_le16(data + 18);
		minor = bt_get_le16(data + 20);
		print_field("  Version: %u.%u", major, minor);

		tx_power = *(int8_t *) (data + 22);
		print_field("  TX power: %d dB", tx_power);
		break;
	default:
		print_hex_field("  Data", data, data_len);
		break;
	}
}

static void print_manufacturer_data(const void *data, uint8_t data_len)
{
	uint16_t company = bt_get_le16(data);

	packet_print_company("Company", company);

	switch (company) {
	case 76:
	case 19456:
		print_manufacturer_apple(data + 2, data_len - 2);
		break;
	default:
		print_hex_field("  Data", data + 2, data_len - 2);
		break;
	}
}

static void print_uuid16_list(const char *label, const void *data,
							uint8_t data_len)
{
	uint8_t count = data_len / sizeof(uint16_t);
	unsigned int i;

	print_field("%s: %u entr%s", label, count, count == 1 ? "y" : "ies");

	for (i = 0; i < count; i++) {
		uint16_t uuid = bt_get_le16(data + (i * 2));
		print_field("  %s (0x%4.4x)", uuid16_to_str(uuid), uuid);
	}
}

static void print_uuid32_list(const char *label, const void *data,
							uint8_t data_len)
{
	uint8_t count = data_len / sizeof(uint32_t);
	unsigned int i;

	print_field("%s: %u entr%s", label, count, count == 1 ? "y" : "ies");

	for (i = 0; i < count; i++) {
		uint32_t uuid = bt_get_le32(data + (i * 4));
		print_field("  %s (0x%8.8x)", uuid32_to_str(uuid), uuid);
	}
}

static void print_uuid128_list(const char *label, const void *data,
							uint8_t data_len)
{
	uint8_t count = data_len / 16;
	unsigned int i;

	print_field("%s: %u entr%s", label, count, count == 1 ? "y" : "ies");

	for (i = 0; i < count; i++) {
		const uint8_t *uuid = data + (i * 16);

		print_field("  %8.8x-%4.4x-%4.4x-%4.4x-%8.8x%4.4x",
				bt_get_le32(&uuid[12]), bt_get_le16(&uuid[10]),
				bt_get_le16(&uuid[8]), bt_get_le16(&uuid[6]),
				bt_get_le32(&uuid[2]), bt_get_le16(&uuid[0]));
	}
}

static const struct {
	uint8_t bit;
	const char *str;
} eir_flags_table[] = {
	{ 0, "LE Limited Discoverable Mode"		},
	{ 1, "LE General Discoverable Mode"		},
	{ 2, "BR/EDR Not Supported"			},
	{ 3, "Simultaneous LE and BR/EDR (Controller)"	},
	{ 4, "Simultaneous LE and BR/EDR (Host)"	},
	{ }
};

static const struct {
	uint8_t bit;
	const char *str;
} eir_3d_table[] = {
	{ 0, "Association Notification"					},
	{ 1, "Battery Level Reporting"					},
	{ 2, "Send Battery Level Report on Start-up Synchronization"	},
	{ 7, "Factory Test Mode"					},
	{ }
};

static void print_eir(const uint8_t *eir, uint8_t eir_len, bool le)
{
	uint16_t len = 0;

	if (eir_len == 0)
		return;

	while (len < eir_len - 1) {
		uint8_t field_len = eir[0];
		const uint8_t *data = &eir[2];
		uint8_t data_len;
		char name[239], label[100];
		uint8_t flags, mask;
		int i;

		/* Check for the end of EIR */
		if (field_len == 0)
			break;

		len += field_len + 1;

		/* Do not continue EIR Data parsing if got incorrect length */
		if (len > eir_len) {
			len -= field_len + 1;
			break;
		}

		data_len = field_len - 1;

		switch (eir[1]) {
		case BT_EIR_FLAGS:
			flags = *data;
			mask = flags;

			print_field("Flags: 0x%2.2x", flags);

			for (i = 0; eir_flags_table[i].str; i++) {
				if (flags & (1 << eir_flags_table[i].bit)) {
					print_field("  %s",
							eir_flags_table[i].str);
					mask &= ~(1 << eir_flags_table[i].bit);
				}
			}

			if (mask)
				print_text(COLOR_UNKNOWN_SERVICE_CLASS,
					"  Unknown flags (0x%2.2x)", mask);
			break;

		case BT_EIR_UUID16_SOME:
			if (data_len < sizeof(uint16_t))
				break;
			print_uuid16_list("16-bit Service UUIDs (partial)",
							data, data_len);
			break;

		case BT_EIR_UUID16_ALL:
			if (data_len < sizeof(uint16_t))
				break;
			print_uuid16_list("16-bit Service UUIDs (complete)",
							data, data_len);
			break;

		case BT_EIR_UUID32_SOME:
			if (data_len < sizeof(uint32_t))
				break;
			print_uuid32_list("32-bit Service UUIDs (partial)",
							data, data_len);
			break;

		case BT_EIR_UUID32_ALL:
			if (data_len < sizeof(uint32_t))
				break;
			print_uuid32_list("32-bit Service UUIDs (complete)",
							data, data_len);
			break;

		case BT_EIR_UUID128_SOME:
			if (data_len < 16)
				break;
			print_uuid128_list("128-bit Service UUIDs (partial)",
								data, data_len);
			break;

		case BT_EIR_UUID128_ALL:
			if (data_len < 16)
				break;
			print_uuid128_list("128-bit Service UUIDs (complete)",
								data, data_len);
			break;

		case BT_EIR_NAME_SHORT:
			memset(name, 0, sizeof(name));
			memcpy(name, data, data_len);
			print_field("Name (short): %s", name);
			break;

		case BT_EIR_NAME_COMPLETE:
			memset(name, 0, sizeof(name));
			memcpy(name, data, data_len);
			print_field("Name (complete): %s", name);
			break;

		case BT_EIR_TX_POWER:
			if (data_len < 1)
				break;
			print_field("TX power: %d dBm", (int8_t) *data);
			break;

		case BT_EIR_CLASS_OF_DEV:
			if (data_len < 3)
				break;
			print_dev_class(data);
			break;

		case BT_EIR_SSP_HASH_P192:
			if (data_len < 16)
				break;
			print_hash("P-192", data);
			break;

		case BT_EIR_SSP_RANDOMIZER_P192:
			if (data_len < 16)
				break;
			print_randomizer("P-192", data);
			break;

		case BT_EIR_DEVICE_ID:
			/* SMP TK has the same value as Device ID */
			if (le)
				print_hex_field("SMP TK", data, data_len);
			else if (data_len >= 8)
				print_field("Device ID: "
						"Source 0x%4.4x "
						"Vendor 0x%4.4x "
						"Product 0x%4.4x "
						"Version 0x%4.4x",
						bt_get_le16(&data[0]),
						bt_get_le16(&data[2]),
						bt_get_le16(&data[4]),
						bt_get_le16(&data[6]));
			break;

		case BT_EIR_SMP_OOB_FLAGS:
			print_field("SMP OOB Flags: 0x%2.2x", *data);
			break;

		case BT_EIR_SLAVE_CONN_INTERVAL:
			if (data_len < 4)
				break;
			print_field("Slave Conn. Interval: 0x%4.4x - 0x%4.4x",
							bt_get_le16(&data[0]),
							bt_get_le16(&data[2]));
			break;

		case BT_EIR_SERVICE_UUID16:
			if (data_len < sizeof(uint16_t))
				break;
			print_uuid16_list("16-bit Service UUIDs",
							data, data_len);
			break;

		case BT_EIR_SERVICE_UUID128:
			if (data_len < 16)
				break;
			print_uuid128_list("128-bit Service UUIDs",
							data, data_len);
			break;

		case BT_EIR_SERVICE_DATA:
			if (data_len < 2)
				break;
			sprintf(label, "Service Data (UUID 0x%4.4x)",
							bt_get_le16(&data[0]));
			print_hex_field(label, &data[2], data_len - 2);
			break;

		case BT_EIR_RANDOM_ADDRESS:
			if (data_len < 6)
				break;
			print_addr("Random Address", data, 0x01);
			break;

		case BT_EIR_PUBLIC_ADDRESS:
			if (data_len < 6)
				break;
			print_addr("Public Address", data, 0x00);
			break;

		case BT_EIR_GAP_APPEARANCE:
			if (data_len < 2)
				break;
			print_field("Appearance: 0x%4.4x", bt_get_le16(data));
			break;

		case BT_EIR_SSP_HASH_P256:
			if (data_len < 16)
				break;
			print_hash("P-256", data);
			break;

		case BT_EIR_SSP_RANDOMIZER_P256:
			if (data_len < 16)
				break;
			print_randomizer("P-256", data);
			break;

		case BT_EIR_3D_INFO_DATA:
			print_hex_field("3D Information Data", data, data_len);
			if (data_len < 2)
				break;

			flags = *data;
			mask = flags;

			print_field("  Features: 0x%2.2x", flags);

			for (i = 0; eir_3d_table[i].str; i++) {
				if (flags & (1 << eir_3d_table[i].bit)) {
					print_field("    %s",
							eir_3d_table[i].str);
					mask &= ~(1 << eir_3d_table[i].bit);
				}
			}

			if (mask)
				print_text(COLOR_UNKNOWN_FEATURE_BIT,
					"      Unknown features (0x%2.2x)", mask);

			print_field("  Path Loss Threshold: %d", data[1]);
			break;

		case BT_EIR_MANUFACTURER_DATA:
			if (data_len < 2)
				break;
			print_manufacturer_data(data, data_len);
			break;

		default:
			sprintf(label, "Unknown EIR field 0x%2.2x", eir[1]);
			print_hex_field(label, data, data_len);
			break;
		}

		eir += field_len + 1;
	}

	if (len < eir_len && eir[0] != 0)
		packet_hexdump(eir, eir_len - len);
}

void packet_print_addr(const char *label, const void *data, bool random)
{
	print_addr(label ? : "Address", data, random ? 0x01 : 0x00);
}

void packet_print_ad(const void *data, uint8_t size)
{
	print_eir(data, size, true);
}

void packet_hexdump(const unsigned char *buf, uint16_t len)
{
	static const char hexdigits[] = "0123456789abcdef";
	char str[68];
	uint16_t i;

	if (!len)
		return;

	for (i = 0; i < len; i++) {
		str[((i % 16) * 3) + 0] = hexdigits[buf[i] >> 4];
		str[((i % 16) * 3) + 1] = hexdigits[buf[i] & 0xf];
		str[((i % 16) * 3) + 2] = ' ';
		str[(i % 16) + 49] = isprint(buf[i]) ? buf[i] : '.';

		if ((i + 1) % 16 == 0) {
			str[47] = ' ';
			str[48] = ' ';
			str[65] = '\0';
			print_text(COLOR_WHITE, "%s", str);
			str[0] = ' ';
		}
	}

	if (i % 16 > 0) {
		uint16_t j;
		for (j = (i % 16); j < 16; j++) {
			str[(j * 3) + 0] = ' ';
			str[(j * 3) + 1] = ' ';
			str[(j * 3) + 2] = ' ';
			str[j + 49] = ' ';
		}
		str[47] = ' ';
		str[48] = ' ';
		str[65] = '\0';
		print_text(COLOR_WHITE, "%s", str);
	}
}

void packet_control(struct timeval *tv, uint16_t index, uint16_t opcode,
					const void *data, uint16_t size)
{
	if (index_filter && index_number != index)
		return;

	control_message(opcode, data, size);
}

struct monitor_new_index {
	uint8_t  type;
	uint8_t  bus;
	bdaddr_t bdaddr;
	char     name[8];
} __attribute__((packed));

#define MONITOR_NEW_INDEX_SIZE 16

#define MONITOR_DEL_INDEX_SIZE 0

#define MAX_INDEX 16

struct index_data {
	uint8_t  type;
	bdaddr_t bdaddr;
};

static struct index_data index_list[MAX_INDEX];

void packet_monitor(struct timeval *tv, uint16_t index, uint16_t opcode,
					const void *data, uint16_t size)
{
	const struct monitor_new_index *ni;
	char str[18], extra_str[24];

	if (index_filter && index_number != index)
		return;

	index_current = index;

	if (tv && time_offset == ((time_t) -1))
		time_offset = tv->tv_sec;

	switch (opcode) {
	case BTSNOOP_OPCODE_NEW_INDEX:
		ni = data;

		if (index < MAX_INDEX) {
			index_list[index].type = ni->type;
			bacpy(&index_list[index].bdaddr, &ni->bdaddr);
		}

		ba2str(&ni->bdaddr, str);
		packet_new_index(tv, index, str, ni->type, ni->bus, ni->name);
		break;
	case BTSNOOP_OPCODE_DEL_INDEX:
		if (index < MAX_INDEX)
			ba2str(&index_list[index].bdaddr, str);
		else
			ba2str(BDADDR_ANY, str);

		packet_del_index(tv, index, str);
		break;
	case BTSNOOP_OPCODE_COMMAND_PKT:
		packet_hci_command(tv, index, data, size);
		break;
	case BTSNOOP_OPCODE_EVENT_PKT:
		packet_hci_event(tv, index, data, size);
		break;
	case BTSNOOP_OPCODE_ACL_TX_PKT:
		packet_hci_acldata(tv, index, false, data, size);
		break;
	case BTSNOOP_OPCODE_ACL_RX_PKT:
		packet_hci_acldata(tv, index, true, data, size);
		break;
	case BTSNOOP_OPCODE_SCO_TX_PKT:
		packet_hci_scodata(tv, index, false, data, size);
		break;
	case BTSNOOP_OPCODE_SCO_RX_PKT:
		packet_hci_scodata(tv, index, true, data, size);
		break;
	default:
		sprintf(extra_str, "(code %d len %d)", opcode, size);
		print_packet(tv, index, '*', COLOR_ERROR,
					"Unknown packet", NULL, extra_str);
		packet_hexdump(data, size);
		break;
	}
}

void packet_simulator(struct timeval *tv, uint16_t frequency,
					const void *data, uint16_t size)
{
	char str[10];

	if (tv && time_offset == ((time_t) -1))
		time_offset = tv->tv_sec;

	sprintf(str, "%u MHz", frequency);

	print_packet(tv, 0, '*', COLOR_PHY_PACKET,
					"Physical packet:", NULL, str);

	ll_packet(frequency, data, size);
}

static void null_cmd(const void *data, uint8_t size)
{
}

static void status_rsp(const void *data, uint8_t size)
{
	uint8_t status = *((const uint8_t *) data);

	print_status(status);
}

static void status_bdaddr_rsp(const void *data, uint8_t size)
{
	uint8_t status = *((const uint8_t *) data);

	print_status(status);
	print_bdaddr(data + 1);
}

static void inquiry_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_inquiry *cmd = data;

	print_iac(cmd->lap);
	print_field("Length: %.2fs (0x%2.2x)",
				cmd->length * 1.28, cmd->length);
	print_num_resp(cmd->num_resp);
}

static void periodic_inquiry_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_periodic_inquiry *cmd = data;

	print_field("Max period: %.2fs (0x%2.2x)",
				cmd->max_period * 1.28, cmd->max_period);
	print_field("Min period: %.2fs (0x%2.2x)",
				cmd->min_period * 1.28, cmd->min_period);
	print_iac(cmd->lap);
	print_field("Length: %.2fs (0x%2.2x)",
				cmd->length * 1.28, cmd->length);
	print_num_resp(cmd->num_resp);
}

static void create_conn_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_create_conn *cmd = data;
	const char *str;

	print_bdaddr(cmd->bdaddr);
	print_pkt_type(cmd->pkt_type);
	print_pscan_rep_mode(cmd->pscan_rep_mode);
	print_pscan_mode(cmd->pscan_mode);
	print_clock_offset(cmd->clock_offset);

	switch (cmd->role_switch) {
	case 0x00:
		str = "Stay master";
		break;
	case 0x01:
		str = "Allow slave";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Role switch: %s (0x%2.2x)", str, cmd->role_switch);
}

static void disconnect_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_disconnect *cmd = data;

	print_handle(cmd->handle);
	print_reason(cmd->reason);
}

static void add_sco_conn_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_add_sco_conn *cmd = data;

	print_handle(cmd->handle);
	print_pkt_type(cmd->pkt_type);
}

static void create_conn_cancel_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_create_conn_cancel *cmd = data;

	print_bdaddr(cmd->bdaddr);
}

static void accept_conn_request_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_accept_conn_request *cmd = data;

	print_bdaddr(cmd->bdaddr);
	print_role(cmd->role);
}

static void reject_conn_request_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_reject_conn_request *cmd = data;

	print_bdaddr(cmd->bdaddr);
	print_reason(cmd->reason);
}

static void link_key_request_reply_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_link_key_request_reply *cmd = data;

	print_bdaddr(cmd->bdaddr);
	print_link_key(cmd->link_key);
}

static void link_key_request_neg_reply_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_link_key_request_neg_reply *cmd = data;

	print_bdaddr(cmd->bdaddr);
}

static void pin_code_request_reply_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_pin_code_request_reply *cmd = data;

	print_bdaddr(cmd->bdaddr);
	print_field("PIN length: %d", cmd->pin_len);
	print_pin_code(cmd->pin_code, cmd->pin_len);
}

static void pin_code_request_neg_reply_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_pin_code_request_neg_reply *cmd = data;

	print_bdaddr(cmd->bdaddr);
}

static void change_conn_pkt_type_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_change_conn_pkt_type *cmd = data;

	print_handle(cmd->handle);
	print_pkt_type(cmd->pkt_type);
}

static void auth_requested_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_auth_requested *cmd = data;

	print_handle(cmd->handle);
}

static void set_conn_encrypt_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_set_conn_encrypt *cmd = data;

	print_handle(cmd->handle);
	print_encr_mode(cmd->encr_mode);
}

static void change_conn_link_key_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_change_conn_link_key *cmd = data;

	print_handle(cmd->handle);
}

static void master_link_key_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_master_link_key *cmd = data;

	print_key_flag(cmd->key_flag);
}

static void remote_name_request_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_remote_name_request *cmd = data;

	print_bdaddr(cmd->bdaddr);
	print_pscan_rep_mode(cmd->pscan_rep_mode);
	print_pscan_mode(cmd->pscan_mode);
	print_clock_offset(cmd->clock_offset);
}

static void remote_name_request_cancel_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_remote_name_request_cancel *cmd = data;

	print_bdaddr(cmd->bdaddr);
}

static void read_remote_features_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_read_remote_features *cmd = data;

	print_handle(cmd->handle);
}

static void read_remote_ext_features_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_read_remote_ext_features *cmd = data;

	print_handle(cmd->handle);
	print_field("Page: %d", cmd->page);
}

static void read_remote_version_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_read_remote_version *cmd = data;

	print_handle(cmd->handle);
}

static void read_clock_offset_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_read_clock_offset *cmd = data;

	print_handle(cmd->handle);
}

static void read_lmp_handle_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_read_lmp_handle *cmd = data;

	print_handle(cmd->handle);
}

static void read_lmp_handle_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_lmp_handle *rsp = data;

	print_status(rsp->status);
	print_handle(rsp->handle);
	print_field("LMP handle: %d", rsp->lmp_handle);
	print_field("Reserved: %d", btohl(rsp->reserved));
}

static void setup_sync_conn_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_setup_sync_conn *cmd = data;

	print_handle(cmd->handle);
	print_field("Transmit bandwidth: %d", btohl(cmd->tx_bandwidth));
	print_field("Receive bandwidth: %d", btohl(cmd->rx_bandwidth));
	print_field("Max latency: %d", btohs(cmd->max_latency));
	print_voice_setting(cmd->voice_setting);
	print_retransmission_effort(cmd->retrans_effort);
	print_pkt_type(cmd->pkt_type);
}

static void accept_sync_conn_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_accept_sync_conn *cmd = data;

	print_bdaddr(cmd->bdaddr);
	print_field("Transmit bandwidth: %d", btohl(cmd->tx_bandwidth));
	print_field("Receive bandwidth: %d", btohl(cmd->rx_bandwidth));
	print_field("Max latency: %d", btohs(cmd->max_latency));
	print_voice_setting(cmd->voice_setting);
	print_retransmission_effort(cmd->retrans_effort);
	print_pkt_type(cmd->pkt_type);
}

static void reject_sync_conn_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_reject_sync_conn *cmd = data;

	print_bdaddr(cmd->bdaddr);
	print_reason(cmd->reason);
}

static void io_capability_request_reply_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_io_capability_request_reply *cmd = data;

	print_bdaddr(cmd->bdaddr);
	print_io_capability(cmd->capability);
	print_oob_data(cmd->oob_data);
	print_authentication(cmd->authentication);
}

static void user_confirm_request_reply_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_user_confirm_request_reply *cmd = data;

	print_bdaddr(cmd->bdaddr);
}

static void user_confirm_request_neg_reply_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_user_confirm_request_neg_reply *cmd = data;

	print_bdaddr(cmd->bdaddr);
}

static void user_passkey_request_reply_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_user_passkey_request_reply *cmd = data;

	print_bdaddr(cmd->bdaddr);
	print_passkey(cmd->passkey);
}

static void user_passkey_request_neg_reply_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_user_passkey_request_neg_reply *cmd = data;

	print_bdaddr(cmd->bdaddr);
}

static void remote_oob_data_request_reply_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_remote_oob_data_request_reply *cmd = data;

	print_bdaddr(cmd->bdaddr);
	print_hash("P-192", cmd->hash);
	print_randomizer("P-192", cmd->randomizer);
}

static void remote_oob_data_request_neg_reply_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_remote_oob_data_request_neg_reply *cmd = data;

	print_bdaddr(cmd->bdaddr);
}

static void io_capability_request_neg_reply_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_io_capability_request_neg_reply *cmd = data;

	print_bdaddr(cmd->bdaddr);
	print_reason(cmd->reason);
}

static void create_phy_link_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_create_phy_link *cmd = data;

	print_phy_handle(cmd->phy_handle);
	print_key_len(cmd->key_len);
	print_key_type(cmd->key_type);

	packet_hexdump(data + 3, size - 3);
}

static void accept_phy_link_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_accept_phy_link *cmd = data;

	print_phy_handle(cmd->phy_handle);
	print_key_len(cmd->key_len);
	print_key_type(cmd->key_type);

	packet_hexdump(data + 3, size - 3);
}

static void disconn_phy_link_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_disconn_phy_link *cmd = data;

	print_phy_handle(cmd->phy_handle);
	print_reason(cmd->reason);
}

static void create_logic_link_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_create_logic_link *cmd = data;

	print_phy_handle(cmd->phy_handle);
	print_flow_spec("TX", cmd->tx_flow_spec);
	print_flow_spec("RX", cmd->rx_flow_spec);
}

static void accept_logic_link_cmd(const void *data, uint8_t size)
{
        const struct bt_hci_cmd_accept_logic_link *cmd = data;

	print_phy_handle(cmd->phy_handle);
	print_flow_spec("TX", cmd->tx_flow_spec);
	print_flow_spec("RX", cmd->rx_flow_spec);
}

static void disconn_logic_link_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_disconn_logic_link *cmd = data;

	print_handle(cmd->handle);
}

static void logic_link_cancel_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_logic_link_cancel *cmd = data;

	print_phy_handle(cmd->phy_handle);
	print_field("TX flow spec: 0x%2.2x", cmd->flow_spec);
}

static void logic_link_cancel_rsp(const void *data, uint8_t size)
{
        const struct bt_hci_rsp_logic_link_cancel *rsp = data;

	print_status(rsp->status);
	print_phy_handle(rsp->phy_handle);
	print_field("TX flow spec: 0x%2.2x", rsp->flow_spec);
}

static void flow_spec_modify_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_flow_spec_modify *cmd = data;

	print_handle(cmd->handle);
	print_flow_spec("TX", cmd->tx_flow_spec);
	print_flow_spec("RX", cmd->rx_flow_spec);
}

static void hold_mode_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_hold_mode *cmd = data;

	print_handle(cmd->handle);
	print_slot_625("Hold max interval", cmd->max_interval);
	print_slot_625("Hold min interval", cmd->min_interval);
}

static void sniff_mode_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_sniff_mode *cmd = data;

	print_handle(cmd->handle);
	print_slot_625("Sniff max interval", cmd->max_interval);
	print_slot_625("Sniff min interval", cmd->min_interval);
	print_slot_125("Sniff attempt", cmd->attempt);
	print_slot_125("Sniff timeout", cmd->timeout);
}

static void exit_sniff_mode_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_exit_sniff_mode *cmd = data;

	print_handle(cmd->handle);
}

static void park_state_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_park_state *cmd = data;

	print_handle(cmd->handle);
	print_slot_625("Beacon max interval", cmd->max_interval);
	print_slot_625("Beacon min interval", cmd->min_interval);
}

static void exit_park_state_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_exit_park_state *cmd = data;

	print_handle(cmd->handle);
}

static void qos_setup_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_qos_setup *cmd = data;

	print_handle(cmd->handle);
	print_field("Flags: 0x%2.2x", cmd->flags);

	print_service_type(cmd->service_type);

	print_field("Token rate: %d", btohl(cmd->token_rate));
	print_field("Peak bandwidth: %d", btohl(cmd->peak_bandwidth));
	print_field("Latency: %d", btohl(cmd->latency));
	print_field("Delay variation: %d", btohl(cmd->delay_variation));
}

static void role_discovery_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_role_discovery *cmd = data;

	print_handle(cmd->handle);
}

static void role_discovery_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_role_discovery *rsp = data;

	print_status(rsp->status);
	print_handle(rsp->handle);
	print_role(rsp->role);
}

static void switch_role_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_switch_role *cmd = data;

	print_bdaddr(cmd->bdaddr);
	print_role(cmd->role);
}

static void read_link_policy_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_read_link_policy *cmd = data;

	print_handle(cmd->handle);
}

static void read_link_policy_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_link_policy *rsp = data;

	print_status(rsp->status);
	print_handle(rsp->handle);
	print_link_policy(rsp->policy);
}

static void write_link_policy_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_link_policy *cmd = data;

	print_handle(cmd->handle);
	print_link_policy(cmd->policy);
}

static void write_link_policy_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_write_link_policy *rsp = data;

	print_status(rsp->status);
	print_handle(rsp->handle);
}

static void read_default_link_policy_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_default_link_policy *rsp = data;

	print_status(rsp->status);
	print_link_policy(rsp->policy);
}

static void write_default_link_policy_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_default_link_policy *cmd = data;

	print_link_policy(cmd->policy);
}

static void flow_spec_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_flow_spec *cmd = data;

	print_handle(cmd->handle);
	print_field("Flags: 0x%2.2x", cmd->flags);

	print_flow_direction(cmd->direction);
	print_service_type(cmd->service_type);

	print_field("Token rate: %d", btohl(cmd->token_rate));
	print_field("Token bucket size: %d", btohl(cmd->token_bucket_size));
	print_field("Peak bandwidth: %d", btohl(cmd->peak_bandwidth));
	print_field("Access latency: %d", btohl(cmd->access_latency));
}

static void sniff_subrating_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_sniff_subrating *cmd = data;

	print_handle(cmd->handle);
	print_slot_625("Max latency", cmd->max_latency);
	print_slot_625("Min remote timeout", cmd->min_remote_timeout);
	print_slot_625("Min local timeout", cmd->min_local_timeout);
}

static void sniff_subrating_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_sniff_subrating *rsp = data;

	print_status(rsp->status);
	print_handle(rsp->handle);
}

static void set_event_mask_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_set_event_mask *cmd = data;

	print_event_mask(cmd->mask);
}

static void set_event_filter_cmd(const void *data, uint8_t size)
{
	uint8_t type = *((const uint8_t *) data);
	uint8_t filter;
	const char *str;

	switch (type) {
	case 0x00:
		str = "Clear All Filters";
		break;
	case 0x01:
		str = "Inquiry Result";
		break;
	case 0x02:
		str = "Connection Setup";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Type: %s (0x%2.2x)", str, type);

	switch (type) {
	case 0x00:
		if (size > 1) {
			print_text(COLOR_ERROR, "  invalid parameter size");
			packet_hexdump(data + 1, size - 1);
		}
		break;

	case 0x01:
		filter = *((const uint8_t *) (data + 1));

		switch (filter) {
		case 0x00:
			str = "Return responses from all devices";
			break;
		case 0x01:
			str = "Device with specific Class of Device";
			break;
		case 0x02:
			str = "Device with specific BD_ADDR";
			break;
		default:
			str = "Reserved";
			break;
		}

		print_field("Filter: %s (0x%2.2x)", str, filter);
		packet_hexdump(data + 2, size - 2);
		break;

	case 0x02:
		filter = *((const uint8_t *) (data + 1));

		switch (filter) {
		case 0x00:
			str = "Allow connections all devices";
			break;
		case 0x01:
			str = "Allow connections with specific Class of Device";
			break;
		case 0x02:
			str = "Allow connections with specific BD_ADDR";
			break;
		default:
			str = "Reserved";
			break;
		}

		print_field("Filter: %s (0x%2.2x)", str, filter);
		packet_hexdump(data + 2, size - 2);
		break;

	default:
		filter = *((const uint8_t *) (data + 1));

		print_field("Filter: Reserved (0x%2.2x)", filter);
		packet_hexdump(data + 2, size - 2);
		break;
	}
}

static void flush_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_flush *cmd = data;

	print_handle(cmd->handle);
}

static void flush_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_flush *rsp = data;

	print_status(rsp->status);
	print_handle(rsp->handle);
}

static void read_pin_type_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_pin_type *rsp = data;

	print_status(rsp->status);
	print_pin_type(rsp->pin_type);
}

static void write_pin_type_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_pin_type *cmd = data;

	print_pin_type(cmd->pin_type);
}

static void read_stored_link_key_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_read_stored_link_key *cmd = data;

	print_bdaddr(cmd->bdaddr);
	print_field("Read all: 0x%2.2x", cmd->read_all);
}

static void read_stored_link_key_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_stored_link_key *rsp = data;

	print_status(rsp->status);
	print_field("Max num keys: %d", btohs(rsp->max_num_keys));
	print_field("Num keys: %d", btohs(rsp->num_keys));
}

static void write_stored_link_key_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_stored_link_key *cmd = data;

	print_field("Num keys: %d", cmd->num_keys);

	packet_hexdump(data + 1, size - 1);
}

static void write_stored_link_key_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_write_stored_link_key *rsp = data;

	print_status(rsp->status);
	print_field("Num keys: %d", rsp->num_keys);
}

static void delete_stored_link_key_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_delete_stored_link_key *cmd = data;

	print_bdaddr(cmd->bdaddr);
	print_field("Delete all: 0x%2.2x", cmd->delete_all);
}

static void delete_stored_link_key_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_delete_stored_link_key *rsp = data;

	print_status(rsp->status);
	print_field("Num keys: %d", btohs(rsp->num_keys));
}

static void write_local_name_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_local_name *cmd = data;

	print_name(cmd->name);
}

static void read_local_name_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_local_name *rsp = data;

	print_status(rsp->status);
	print_name(rsp->name);
}

static void read_conn_accept_timeout_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_conn_accept_timeout *rsp = data;

	print_status(rsp->status);
	print_timeout(rsp->timeout);
}

static void write_conn_accept_timeout_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_conn_accept_timeout *cmd = data;

	print_timeout(cmd->timeout);
}

static void read_page_timeout_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_page_timeout *rsp = data;

	print_status(rsp->status);
	print_timeout(rsp->timeout);
}

static void write_page_timeout_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_page_timeout *cmd = data;

	print_timeout(cmd->timeout);
}

static void read_scan_enable_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_scan_enable *rsp = data;

	print_status(rsp->status);
	print_scan_enable(rsp->enable);
}

static void write_scan_enable_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_scan_enable *cmd = data;

	print_scan_enable(cmd->enable);
}

static void read_page_scan_activity_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_page_scan_activity *rsp = data;

	print_status(rsp->status);
	print_interval(rsp->interval);
	print_window(rsp->window);
}

static void write_page_scan_activity_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_page_scan_activity *cmd = data;

	print_interval(cmd->interval);
	print_window(cmd->window);
}

static void read_inquiry_scan_activity_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_inquiry_scan_activity *rsp = data;

	print_status(rsp->status);
	print_interval(rsp->interval);
	print_window(rsp->window);
}

static void write_inquiry_scan_activity_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_inquiry_scan_activity *cmd = data;

	print_interval(cmd->interval);
	print_window(cmd->window);
}

static void read_auth_enable_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_auth_enable *rsp = data;

	print_status(rsp->status);
	print_auth_enable(rsp->enable);
}

static void write_auth_enable_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_auth_enable *cmd = data;

	print_auth_enable(cmd->enable);
}

static void read_encrypt_mode_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_encrypt_mode *rsp = data;

	print_status(rsp->status);
	print_encrypt_mode(rsp->mode);
}

static void write_encrypt_mode_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_encrypt_mode *cmd = data;

	print_encrypt_mode(cmd->mode);
}

static void read_class_of_dev_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_class_of_dev *rsp = data;

	print_status(rsp->status);
	print_dev_class(rsp->dev_class);
}

static void write_class_of_dev_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_class_of_dev *cmd = data;

	print_dev_class(cmd->dev_class);
}

static void read_voice_setting_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_voice_setting *rsp = data;

	print_status(rsp->status);
	print_voice_setting(rsp->setting);
}

static void write_voice_setting_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_voice_setting *cmd = data;

	print_voice_setting(cmd->setting);
}

static void host_buffer_size_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_host_buffer_size *cmd = data;

	print_field("ACL MTU: %-4d ACL max packet: %d",
				btohs(cmd->acl_mtu), btohs(cmd->acl_max_pkt));
	print_field("SCO MTU: %-4d SCO max packet: %d",
				cmd->sco_mtu, btohs(cmd->sco_max_pkt));
}

static void read_link_supv_timeout_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_read_link_supv_timeout *cmd = data;

	print_handle(cmd->handle);
}

static void read_link_supv_timeout_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_link_supv_timeout *rsp = data;

	print_status(rsp->status);
	print_handle(rsp->handle);
	print_timeout(rsp->timeout);
}

static void write_link_supv_timeout_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_link_supv_timeout *cmd = data;

	print_handle(cmd->handle);
	print_timeout(cmd->timeout);
}

static void write_link_supv_timeout_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_write_link_supv_timeout *rsp = data;

	print_status(rsp->status);
	print_handle(rsp->handle);
}

static void read_num_supported_iac_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_num_supported_iac *rsp = data;

	print_status(rsp->status);
	print_field("Number of IAC: %d", rsp->num_iac);
}

static void read_current_iac_lap_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_current_iac_lap *rsp = data;
	uint8_t i;

	print_status(rsp->status);
	print_field("Number of IAC: %d", rsp->num_iac);

	for (i = 0; i < rsp->num_iac; i++)
		print_iac(rsp->iac_lap + (i * 3));
}

static void write_current_iac_lap_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_current_iac_lap *cmd = data;
	uint8_t i;

	print_field("Number of IAC: %d", cmd->num_iac);

	for (i = 0; i < cmd->num_iac; i++)
		print_iac(cmd->iac_lap + (i * 3));
}

static void read_page_scan_period_mode_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_page_scan_period_mode *rsp = data;

	print_status(rsp->status);
	print_pscan_period_mode(rsp->mode);
}

static void write_page_scan_period_mode_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_page_scan_period_mode *cmd = data;

	print_pscan_period_mode(cmd->mode);
}

static void read_page_scan_mode_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_page_scan_mode *rsp = data;

	print_status(rsp->status);
	print_pscan_mode(rsp->mode);
}

static void write_page_scan_mode_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_page_scan_mode *cmd = data;

	print_pscan_mode(cmd->mode);
}

static void set_afh_host_classification_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_set_afh_host_classification *cmd = data;

	print_channel_map(cmd->map);
}

static void read_inquiry_scan_type_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_inquiry_scan_type *rsp = data;

	print_status(rsp->status);
	print_inquiry_scan_type(rsp->type);
}

static void write_inquiry_scan_type_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_inquiry_scan_type *cmd = data;

	print_inquiry_scan_type(cmd->type);
}

static void read_inquiry_mode_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_inquiry_mode *rsp = data;

	print_status(rsp->status);
	print_inquiry_mode(rsp->mode);
}

static void write_inquiry_mode_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_inquiry_mode *cmd = data;

	print_inquiry_mode(cmd->mode);
}

static void read_page_scan_type_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_page_scan_type *rsp = data;

	print_status(rsp->status);
	print_pscan_type(rsp->type);
}

static void write_page_scan_type_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_page_scan_type *cmd = data;

	print_pscan_type(cmd->type);
}

static void read_afh_assessment_mode_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_afh_assessment_mode *rsp = data;

	print_status(rsp->status);
	print_afh_mode(rsp->mode);
}

static void write_afh_assessment_mode_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_afh_assessment_mode *cmd = data;

	print_afh_mode(cmd->mode);
}

static void read_ext_inquiry_response_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_ext_inquiry_response *rsp = data;

	print_status(rsp->status);
	print_fec(rsp->fec);
	print_eir(rsp->data, sizeof(rsp->data), false);
}

static void write_ext_inquiry_response_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_ext_inquiry_response *cmd = data;

	print_fec(cmd->fec);
	print_eir(cmd->data, sizeof(cmd->data), false);
}

static void refresh_encrypt_key_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_refresh_encrypt_key *cmd = data;

	print_handle(cmd->handle);
}

static void read_simple_pairing_mode_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_simple_pairing_mode *rsp = data;

	print_status(rsp->status);
	print_simple_pairing_mode(rsp->mode);
}

static void write_simple_pairing_mode_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_simple_pairing_mode *cmd = data;

	print_simple_pairing_mode(cmd->mode);
}

static void read_local_oob_data_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_local_oob_data *rsp = data;

	print_status(rsp->status);
	print_hash("P-192", rsp->hash);
	print_randomizer("P-192", rsp->randomizer);
}

static void read_inquiry_resp_tx_power_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_inquiry_resp_tx_power *rsp = data;

	print_status(rsp->status);
	print_field("TX power: %d dBm", rsp->level);
}

static void write_inquiry_tx_power_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_inquiry_tx_power *cmd = data;

	print_field("TX power: %d dBm", cmd->level);
}

static void enhanced_flush_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_enhanced_flush *cmd = data;
	const char *str;

	print_handle(cmd->handle);

	switch (cmd->type) {
	case 0x00:
		str = "Automatic flushable only";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Type: %s (0x%2.2x)", str, cmd->type);
}

static void set_event_mask_page2_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_set_event_mask_page2 *cmd = data;

	print_event_mask_page2(cmd->mask);
}

static void read_location_data_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_location_data *rsp = data;

	print_status(rsp->status);
	print_location_domain_aware(rsp->domain_aware);
	print_location_domain(rsp->domain);
	print_location_domain_options(rsp->domain_options);
	print_location_options(rsp->options);
}

static void write_location_data_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_location_data *cmd = data;

	print_location_domain_aware(cmd->domain_aware);
	print_location_domain(cmd->domain);
	print_location_domain_options(cmd->domain_options);
	print_location_options(cmd->options);
}

static void read_flow_control_mode_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_flow_control_mode *rsp = data;

	print_status(rsp->status);
	print_flow_control_mode(rsp->mode);
}

static void write_flow_control_mode_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_flow_control_mode *cmd = data;

	print_flow_control_mode(cmd->mode);
}

static void read_le_host_supported_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_le_host_supported *rsp = data;

	print_status(rsp->status);
	print_field("Supported: 0x%2.2x", rsp->supported);
	print_field("Simultaneous: 0x%2.2x", rsp->simultaneous);
}

static void write_le_host_supported_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_le_host_supported *cmd = data;

	print_field("Supported: 0x%2.2x", cmd->supported);
	print_field("Simultaneous: 0x%2.2x", cmd->simultaneous);
}

static void read_sync_train_params_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_sync_train_params *rsp = data;

	print_status(rsp->status);
	print_interval(rsp->interval);
	print_field("Timeout: %.3f msec (0x%8.8x)",
			btohl(rsp->timeout) * 0.625, btohl(rsp->timeout));
	print_field("Service Data: 0x%2.2x", rsp->service_data);
}

static void read_local_version_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_local_version *rsp = data;

	print_status(rsp->status);
	print_hci_version(rsp->hci_ver, rsp->hci_rev);

	switch (index_list[index_current].type) {
	case HCI_BREDR:
		print_lmp_version(rsp->lmp_ver, rsp->lmp_subver);
		break;
	case HCI_AMP:
		print_pal_version(rsp->lmp_ver, rsp->lmp_subver);
		break;
	}

	print_manufacturer(rsp->manufacturer);
}

static void read_local_commands_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_local_commands *rsp = data;

	print_status(rsp->status);
	print_commands(rsp->commands);
}

static void read_local_features_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_local_features *rsp = data;

	print_status(rsp->status);
	print_features(0, rsp->features, 0x00);
}

static void read_local_ext_features_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_read_local_ext_features *cmd = data;

	print_field("Page: %d", cmd->page);
}

static void read_local_ext_features_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_local_ext_features *rsp = data;

	print_status(rsp->status);
	print_field("Page: %d/%d", rsp->page, rsp->max_page);
	print_features(rsp->page, rsp->features, 0x00);
}

static void read_buffer_size_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_buffer_size *rsp = data;

	print_status(rsp->status);
	print_field("ACL MTU: %-4d ACL max packet: %d",
				btohs(rsp->acl_mtu), btohs(rsp->acl_max_pkt));
	print_field("SCO MTU: %-4d SCO max packet: %d",
				rsp->sco_mtu, btohs(rsp->sco_max_pkt));
}

static void read_country_code_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_country_code *rsp = data;
	const char *str;

	print_status(rsp->status);

	switch (rsp->code) {
	case 0x00:
		str = "North America, Europe*, Japan";
		break;
	case 0x01:
		str = "France";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Country code: %s (0x%2.2x)", str, rsp->code);
}

static void read_bd_addr_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_bd_addr *rsp = data;

	print_status(rsp->status);
	print_bdaddr(rsp->bdaddr);
}

static void read_data_block_size_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_data_block_size *rsp = data;

	print_status(rsp->status);
	print_field("Max ACL length: %d", btohs(rsp->max_acl_len));
	print_field("Block length: %d", btohs(rsp->block_len));
	print_field("Num blocks: %d", btohs(rsp->num_blocks));
}

static void read_failed_contact_counter_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_read_failed_contact_counter *cmd = data;

	print_handle(cmd->handle);
}

static void read_failed_contact_counter_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_failed_contact_counter *rsp = data;

	print_status(rsp->status);
	print_handle(rsp->handle);
	print_field("Counter: %u", htobs(rsp->counter));
}

static void reset_failed_contact_counter_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_reset_failed_contact_counter *cmd = data;

	print_handle(cmd->handle);
}

static void reset_failed_contact_counter_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_reset_failed_contact_counter *rsp = data;

	print_status(rsp->status);
	print_handle(rsp->handle);
}

static void read_link_quality_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_read_link_quality *cmd = data;

	print_handle(cmd->handle);
}

static void read_link_quality_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_link_quality *rsp = data;

	print_status(rsp->status);
	print_handle(rsp->handle);
	print_field("Link quality: 0x%2.2x", rsp->link_quality);
}

static void read_rssi_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_read_rssi *cmd = data;

	print_handle(cmd->handle);
}

static void read_rssi_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_rssi *rsp = data;

	print_status(rsp->status);
	print_handle(rsp->handle);
	print_rssi(rsp->rssi);
}

static void read_afh_channel_map_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_read_afh_channel_map *cmd = data;

	print_handle(cmd->handle);
}

static void read_afh_channel_map_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_afh_channel_map *rsp = data;

	print_status(rsp->status);
	print_handle(rsp->handle);
	print_afh_mode(rsp->mode);
	print_channel_map(rsp->map);
}

static void read_clock_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_read_clock *cmd = data;
	const char *str;

	print_handle(cmd->handle);

	switch (cmd->type) {
	case 0x00:
		str = "Local clock";
		break;
	case 0x01:
		str = "Piconet clock";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Type: %s (0x%2.2x)", str, cmd->type);
}

static void read_clock_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_clock *rsp = data;

	print_status(rsp->status);
	print_handle(rsp->handle);
	print_clock(rsp->clock);
	print_clock_accuracy(rsp->accuracy);
}

static void read_encrypt_key_size_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_read_encrypt_key_size *cmd = data;

	print_handle(cmd->handle);
}

static void read_encrypt_key_size_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_encrypt_key_size *rsp = data;

	print_status(rsp->status);
	print_handle(rsp->handle);
	print_key_size(rsp->key_size);
}

static void read_local_amp_info_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_local_amp_info *rsp = data;
	const char *str;

	print_status(rsp->status);
	print_amp_status(rsp->amp_status);

	print_field("Total bandwidth: %d kbps", btohl(rsp->total_bw));
	print_field("Max guaranteed bandwidth: %d kbps", btohl(rsp->max_bw));
	print_field("Min latency: %d", btohl(rsp->min_latency));
	print_field("Max PDU size: %d", btohl(rsp->max_pdu));

	switch (rsp->amp_type) {
	case 0x00:
		str = "Primary BR/EDR Controller";
		break;
	case 0x01:
		str = "802.11 AMP Controller";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Controller type: %s (0x%2.2x)", str, rsp->amp_type);

	print_field("PAL capabilities: 0x%4.4x", btohs(rsp->pal_cap));
	print_field("Max ASSOC length: %d", btohs(rsp->max_assoc_len));
	print_field("Max flush timeout: %d", btohl(rsp->max_flush_to));
	print_field("Best effort flush timeout: %d", btohl(rsp->be_flush_to));
}

static void read_local_amp_assoc_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_read_local_amp_assoc *cmd = data;

	print_phy_handle(cmd->phy_handle);
	print_field("Length so far: %d", btohs(cmd->len_so_far));
	print_field("Max ASSOC length: %d", btohs(cmd->max_assoc_len));
}

static void read_local_amp_assoc_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_read_local_amp_assoc *rsp = data;

	print_status(rsp->status);
	print_phy_handle(rsp->phy_handle);
	print_field("Remaining ASSOC length: %d", btohs(rsp->remain_assoc_len));

	packet_hexdump(data + 4, size - 4);
}

static void write_remote_amp_assoc_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_write_remote_amp_assoc *cmd = data;

	print_phy_handle(cmd->phy_handle);
	print_field("Length so far: %d", btohs(cmd->len_so_far));
	print_field("Remaining ASSOC length: %d", btohs(cmd->remain_assoc_len));

	packet_hexdump(data + 5, size - 5);
}

static void write_remote_amp_assoc_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_write_remote_amp_assoc *rsp = data;

	print_status(rsp->status);
	print_phy_handle(rsp->phy_handle);
}

static void le_set_event_mask_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_le_set_event_mask *cmd = data;

	print_event_mask_le(cmd->mask);
}

static void le_read_buffer_size_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_le_read_buffer_size *rsp = data;

	print_status(rsp->status);
	print_field("Data packet length: %d", btohs(rsp->le_mtu));
	print_field("Num data packets: %d", rsp->le_max_pkt);
}

static void le_read_local_features_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_le_read_local_features *rsp = data;

	print_status(rsp->status);
	print_features(0, rsp->features, 0x01);
}

static void le_set_random_address_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_le_set_random_address *cmd = data;

	print_addr("Address", cmd->addr, 0x01);
}

static void le_set_adv_parameters_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_le_set_adv_parameters *cmd = data;
	const char *str;

	print_slot_625("Min advertising interval", cmd->min_interval);
	print_slot_625("Max advertising interval", cmd->max_interval);

	switch (cmd->type) {
	case 0x00:
		str = "Connectable undirected - ADV_IND";
		break;
	case 0x01:
		str = "Connectable directed - ADV_DIRECT_IND";
		break;
	case 0x02:
		str = "Scannable undirected - ADV_SCAN_IND";
		break;
	case 0x03:
		str = "Non connectable undirect - ADV_NONCONN_IND";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Type: %s (0x%2.2x)", str, cmd->type);

	print_addr_type("Own address type", cmd->own_addr_type);
	print_addr_type("Direct address type", cmd->direct_addr_type);
	print_addr("Direct address", cmd->direct_addr, cmd->direct_addr_type);

	switch (cmd->channel_map) {
	case 0x01:
		str = "37";
		break;
	case 0x02:
		str = "38";
		break;
	case 0x03:
		str = "37, 38";
		break;
	case 0x04:
		str = "39";
		break;
	case 0x05:
		str = "37, 39";
		break;
	case 0x06:
		str = "38, 39";
		break;
	case 0x07:
		str = "37, 38, 39";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Channel map: %s (0x%2.2x)", str, cmd->channel_map);

	switch (cmd->filter_policy) {
	case 0x00:
		str = "Allow Scan Request from Any, "
			"Allow Connect Request from Any";
		break;
	case 0x01:
		str = "Allow Scan Request from White List Only, "
			"Allow Connect Request from Any";
		break;
	case 0x02:
		str = "Allow Scan Request from Any, "
			"Allow Connect Request from White List Only";
		break;
	case 0x03:
		str = "Allow Scan Request from White List Only, "
			"Allow Connect Request from White List Only";
	default:
		str = "Reserved";
		break;
	}

	print_field("Filter policy: %s (0x%2.2x)", str, cmd->filter_policy);
}

static void le_read_adv_tx_power_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_le_read_adv_tx_power *rsp = data;

	print_status(rsp->status);
	print_field("TX power: %d dBm", rsp->level);
}

static void le_set_adv_data_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_le_set_adv_data *cmd = data;

	print_field("Length: %d", cmd->len);
	print_eir(cmd->data, cmd->len, true);
}

static void le_set_scan_rsp_data_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_le_set_scan_rsp_data *cmd = data;

	print_field("Length: %d", cmd->len);
	print_eir(cmd->data, cmd->len, true);
}

static void le_set_adv_enable_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_le_set_adv_enable *cmd = data;
	const char *str;

	switch (cmd->enable) {
	case 0x00:
		str = "Disabled";
		break;
	case 0x01:
		str = "Enabled";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Advertising: %s (0x%2.2x)", str, cmd->enable);
}

static void le_set_scan_parameters_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_le_set_scan_parameters *cmd = data;
	const char *str;

	switch (cmd->type) {
	case 0x00:
		str = "Passive";
		break;
	case 0x01:
		str = "Active";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Type: %s (0x%2.2x)", str, cmd->type);

	print_interval(cmd->interval);
	print_window(cmd->window);
	print_addr_type("Own address type", cmd->own_addr_type);

	switch (cmd->filter_policy) {
	case 0x00:
		str = "Accept all advertisement";
		break;
	case 0x01:
		str = "Ignore not in white list";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Filter policy: %s (0x%2.2x)", str, cmd->filter_policy);
}

static void le_set_scan_enable_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_le_set_scan_enable *cmd = data;
	const char *str;

	switch (cmd->enable) {
	case 0x00:
		str = "Disabled";
		break;
	case 0x01:
		str = "Enabled";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Scanning: %s (0x%2.2x)", str, cmd->enable);

	switch (cmd->filter_dup) {
	case 0x00:
		str = "Disabled";
		break;
	case 0x01:
		str = "Enabled";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Filter duplicates: %s (0x%2.2x)", str, cmd->filter_dup);
}

static void le_create_conn_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_le_create_conn *cmd = data;
	const char *str;

	print_slot_625("Scan interval", cmd->scan_interval);
	print_slot_625("Scan window", cmd->scan_window);

	switch (cmd->filter_policy) {
	case 0x00:
		str = "White list is not used";
		break;
	case 0x01:
		str = "White list is used";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Filter policy: %s (0x%2.2x)", str, cmd->filter_policy);

	print_addr_type("Peer address type", cmd->peer_addr_type);
	print_addr("Peer address", cmd->peer_addr, cmd->peer_addr_type);
	print_addr_type("Own address type", cmd->own_addr_type);

	print_slot_125("Min connection interval", cmd->min_interval);
	print_slot_125("Max connection interval", cmd->max_interval);
	print_field("Connection latency: 0x%4.4x", btohs(cmd->latency));
	print_field("Supervision timeout: %d msec (0x%4.4x)",
		btohs(cmd->supv_timeout) * 10, btohs(cmd->supv_timeout));
	print_slot_625("Min connection length", cmd->min_length);
	print_slot_625("Max connection length", cmd->max_length);
}

static void le_read_white_list_size_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_le_read_white_list_size *rsp = data;

	print_status(rsp->status);
	print_field("Size: %u", rsp->size);
}

static void le_add_to_white_list_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_le_add_to_white_list *cmd = data;

	print_addr_type("Address type", cmd->addr_type);
	print_addr("Address", cmd->addr, cmd->addr_type);
}

static void le_remove_from_white_list_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_le_remove_from_white_list *cmd = data;

	print_addr_type("Address type", cmd->addr_type);
	print_addr("Address", cmd->addr, cmd->addr_type);
}

static void le_conn_update_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_le_conn_update *cmd = data;

	print_handle(cmd->handle);
	print_slot_125("Min connection interval", cmd->min_interval);
	print_slot_125("Max connection interval", cmd->max_interval);
	print_field("Connection latency: 0x%4.4x", btohs(cmd->latency));
	print_field("Supervision timeout: %d msec (0x%4.4x)",
		btohs(cmd->supv_timeout) * 10, btohs(cmd->supv_timeout));
	print_slot_625("Min connection length", cmd->min_length);
	print_slot_625("Max connection length", cmd->max_length);
}

static void le_set_host_classification_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_le_set_host_classification *cmd = data;

	print_le_channel_map(cmd->map);
}

static void le_read_channel_map_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_le_read_channel_map *cmd = data;

	print_handle(cmd->handle);
}

static void le_read_channel_map_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_le_read_channel_map *rsp = data;

	print_status(rsp->status);
	print_handle(rsp->handle);
	print_le_channel_map(rsp->map);
}

static void le_read_remote_features_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_le_read_remote_features *cmd = data;

	print_handle(cmd->handle);
}

static void le_encrypt_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_le_encrypt *cmd = data;

	print_key("Key", cmd->key);
	print_key("Plaintext data", cmd->plaintext);
}

static void le_encrypt_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_le_encrypt *rsp = data;

	print_status(rsp->status);
	print_key("Encrypted data", rsp->data);
}

static void le_rand_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_le_rand *rsp = data;

	print_status(rsp->status);
	print_random_number(rsp->number);
}

static void le_start_encrypt_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_le_start_encrypt *cmd = data;

	print_handle(cmd->handle);
	print_random_number(cmd->number);
	print_field("Encryption diversifier: 0x%4.4x",
					btohs(cmd->diversifier));
	print_key("Long term key", cmd->ltk);
}

static void le_ltk_req_reply_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_le_ltk_req_reply *cmd = data;

	print_handle(cmd->handle);
	print_key("Long term key", cmd->ltk);
}

static void le_ltk_req_reply_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_le_ltk_req_reply *rsp = data;

	print_status(rsp->status);
	print_handle(rsp->handle);
}

static void le_ltk_req_neg_reply_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_le_ltk_req_neg_reply *cmd = data;

	print_handle(cmd->handle);
}

static void le_ltk_req_neg_reply_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_le_ltk_req_neg_reply *rsp = data;

	print_status(rsp->status);
	print_handle(rsp->handle);
}

static void le_read_supported_states_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_le_read_supported_states *rsp = data;

	print_status(rsp->status);
	print_le_states(rsp->states);
}

static void le_receiver_test_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_le_receiver_test *cmd = data;

	print_field("RX frequency: %d MHz (0x%2.2x)",
				(cmd->frequency * 2) + 2402, cmd->frequency);
}

static void le_transmitter_test_cmd(const void *data, uint8_t size)
{
	const struct bt_hci_cmd_le_transmitter_test *cmd = data;

	print_field("TX frequency: %d MHz (0x%2.2x)",
				(cmd->frequency * 2) + 2402, cmd->frequency);
	print_field("Test data length: %d bytes", cmd->data_len);
	print_field("Packet payload: 0x%2.2x", cmd->payload);
}

static void le_test_end_rsp(const void *data, uint8_t size)
{
	const struct bt_hci_rsp_le_test_end *rsp = data;

	print_status(rsp->status);
	print_field("Number of packets: %d", btohs(rsp->num_packets));
}

struct opcode_data {
	uint16_t opcode;
	int bit;
	const char *str;
	void (*cmd_func) (const void *data, uint8_t size);
	uint8_t cmd_size;
	bool cmd_fixed;
	void (*rsp_func) (const void *data, uint8_t size);
	uint8_t rsp_size;
	bool rsp_fixed;
};

static const struct opcode_data opcode_table[] = {
	{ 0x0000,  -1, "NOP" },

	/* OGF 1 - Link Control */
	{ 0x0401,   0, "Inquiry",
				inquiry_cmd, 5, true },
	{ 0x0402,   1, "Inquiry Cancel",
				null_cmd, 0, true,
				status_rsp, 1, true },
	{ 0x0403,   2, "Periodic Inquiry Mode",
				periodic_inquiry_cmd, 9, true,
				status_rsp, 1, true },
	{ 0x0404,   3, "Exit Periodic Inquiry Mode",
				null_cmd, 0, true,
				status_rsp, 1, true },
	{ 0x0405,   4, "Create Connection",
				create_conn_cmd, 13, true },
	{ 0x0406,   5, "Disconnect",
				disconnect_cmd, 3, true },
	{ 0x0407,   6, "Add SCO Connection",
				add_sco_conn_cmd, 4, true },
	{ 0x0408,   7, "Create Connection Cancel",
				create_conn_cancel_cmd, 6, true,
				status_bdaddr_rsp, 7, true },
	{ 0x0409,   8, "Accept Connection Request",
				accept_conn_request_cmd, 7, true },
	{ 0x040a,   9, "Reject Connection Request",
				reject_conn_request_cmd, 7, true },
	{ 0x040b,  10, "Link Key Request Reply",
				link_key_request_reply_cmd, 22, true,
				status_bdaddr_rsp, 7, true },
	{ 0x040c,  11, "Link Key Request Negative Reply",
				link_key_request_neg_reply_cmd, 6, true,
				status_bdaddr_rsp, 7, true },
	{ 0x040d,  12, "PIN Code Request Reply",
				pin_code_request_reply_cmd, 23, true,
				status_bdaddr_rsp, 7, true },
	{ 0x040e,  13, "PIN Code Request Negative Reply",
				pin_code_request_neg_reply_cmd, 6, true,
				status_bdaddr_rsp, 7, true },
	{ 0x040f,  14, "Change Connection Packet Type",
				change_conn_pkt_type_cmd, 4, true },
	{ 0x0411,  15, "Authentication Requested",
				auth_requested_cmd, 2, true },
	{ 0x0413,  16, "Set Connection Encryption",
				set_conn_encrypt_cmd, 3, true },
	{ 0x0415,  17, "Change Connection Link Key",
				change_conn_link_key_cmd, 2, true },
	{ 0x0417,  18, "Master Link Key",
				master_link_key_cmd, 1, true },
	{ 0x0419,  19, "Remote Name Request",
				remote_name_request_cmd, 10, true },
	{ 0x041a,  20, "Remote Name Request Cancel",
				remote_name_request_cancel_cmd, 6, true,
				status_bdaddr_rsp, 7, true },
	{ 0x041b,  21, "Read Remote Supported Features",
				read_remote_features_cmd, 2, true },
	{ 0x041c,  22, "Read Remote Extended Features",
				read_remote_ext_features_cmd, 3, true },
	{ 0x041d,  23, "Read Remote Version Information",
				read_remote_version_cmd, 2, true },
	{ 0x041f,  24, "Read Clock Offset",
				read_clock_offset_cmd, 2, true },
	{ 0x0420,  25, "Read LMP Handle",
				read_lmp_handle_cmd, 2, true,
				read_lmp_handle_rsp, 8, true },
	{ 0x0428, 131, "Setup Synchronous Connection",
				setup_sync_conn_cmd, 17, true },
	{ 0x0429, 132, "Accept Synchronous Connection",
				accept_sync_conn_cmd, 21, true },
	{ 0x042a, 133, "Reject Synchronous Connection",
				reject_sync_conn_cmd, 7, true },
	{ 0x042b, 151, "IO Capability Request Reply",
				io_capability_request_reply_cmd, 9, true,
				status_bdaddr_rsp, 7, true },
	{ 0x042c, 152, "User Confirmation Request Reply",
				user_confirm_request_reply_cmd, 6, true,
				status_bdaddr_rsp, 7, true },
	{ 0x042d, 153, "User Confirmation Request Neg Reply",
				user_confirm_request_neg_reply_cmd, 6, true,
				status_bdaddr_rsp, 7, true },
	{ 0x042e, 154, "User Passkey Request Reply",
				user_passkey_request_reply_cmd, 10, true,
				status_bdaddr_rsp, 7, true },
	{ 0x042f, 155, "User Passkey Request Negative Reply",
				user_passkey_request_neg_reply_cmd, 6, true,
				status_bdaddr_rsp, 7, true },
	{ 0x0430, 156, "Remote OOB Data Request Reply",
				remote_oob_data_request_reply_cmd, 38, true,
				status_bdaddr_rsp, 7, true },
	{ 0x0433, 159, "Remote OOB Data Request Neg Reply",
				remote_oob_data_request_neg_reply_cmd, 6, true,
				status_bdaddr_rsp, 7, true },
	{ 0x0434, 163, "IO Capability Request Negative Reply",
				io_capability_request_neg_reply_cmd, 7, true,
				status_bdaddr_rsp, 7, true },
	{ 0x0435, 168, "Create Physical Link",
				create_phy_link_cmd, 3, false },
	{ 0x0436, 169, "Accept Physical Link",
				accept_phy_link_cmd, 3, false },
	{ 0x0437, 170, "Disconnect Physical Link",
				disconn_phy_link_cmd, 2, true },
	{ 0x0438, 171, "Create Logical Link",
				create_logic_link_cmd, 33, true },
	{ 0x0439, 172, "Accept Logical Link",
				accept_logic_link_cmd, 33, true },
	{ 0x043a, 173, "Disconnect Logical Link",
				disconn_logic_link_cmd, 2, true },
	{ 0x043b, 174, "Logical Link Cancel",
				logic_link_cancel_cmd, 2, true,
				logic_link_cancel_rsp, 3, true },
	{ 0x043c, 175, "Flow Specifcation Modify",
				flow_spec_modify_cmd, 34, true },
	{ 0x043d, 235, "Enhanced Setup Synchronous Connection" },
	{ 0x043e, 236, "Enhanced Accept Synchronous Connection" },
	{ 0x043f, 246, "Truncated Page" },
	{ 0x0440, 247, "Truncated Page Cancel" },
	{ 0x0441, 248, "Set Connectionless Slave Broadcast" },
	{ 0x0442, 249, "Set Connectionless Slave Broadcast Receive" },
	{ 0x0443, 250, "Start Synchronization Train" },
	{ 0x0444, 251, "Receive Synchronization Train" },

	/* OGF 2 - Link Policy */
	{ 0x0801,  33, "Holde Mode",
				hold_mode_cmd, 6, true },
	{ 0x0803,  34, "Sniff Mode",
				sniff_mode_cmd, 10, true },
	{ 0x0804,  35, "Exit Sniff Mode",
				exit_sniff_mode_cmd, 2, true },
	{ 0x0805,  36, "Park State",
				park_state_cmd, 6, true },
	{ 0x0806,  37, "Exit Park State",
				exit_park_state_cmd, 2, true },
	{ 0x0807,  38, "QoS Setup",
				qos_setup_cmd, 20, true },
	{ 0x0809,  39, "Role Discovery",
				role_discovery_cmd, 2, true,
				role_discovery_rsp, 4, true },
	{ 0x080b,  40, "Switch Role",
				switch_role_cmd, 7, true },
	{ 0x080c,  41, "Read Link Policy Settings",
				read_link_policy_cmd, 2, true,
				read_link_policy_rsp, 5, true },
	{ 0x080d,  42, "Write Link Policy Settings",
				write_link_policy_cmd, 4, true,
				write_link_policy_rsp, 3, true },
	{ 0x080e,  43, "Read Default Link Policy Settings",
				null_cmd, 0, true,
				read_default_link_policy_rsp, 3, true },
	{ 0x080f,  44, "Write Default Link Policy Settings",
				write_default_link_policy_cmd, 2, true,
				status_rsp, 1, true },
	{ 0x0810,  45, "Flow Specification",
				flow_spec_cmd, 21, true },
	{ 0x0811, 140, "Sniff Subrating",
				sniff_subrating_cmd, 8, true,
				sniff_subrating_rsp, 3, true },

	/* OGF 3 - Host Control */
	{ 0x0c01,  46, "Set Event Mask",
				set_event_mask_cmd, 8, true,
				status_rsp, 1, true },
	{ 0x0c03,  47, "Reset",
				null_cmd, 0, true,
				status_rsp, 1, true },
	{ 0x0c05,  48, "Set Event Filter",
				set_event_filter_cmd, 1, false,
				status_rsp, 1, true },
	{ 0x0c08,  49, "Flush",
				flush_cmd, 2, true,
				flush_rsp, 3, true },
	{ 0x0c09,  50, "Read PIN Type",
				null_cmd, 0, true,
				read_pin_type_rsp, 2, true },
	{ 0x0c0a,  51, "Write PIN Type",
				write_pin_type_cmd, 1, true,
				status_rsp, 1, true },
	{ 0x0c0b,  52, "Create New Unit Key",
				null_cmd, 0, true,
				status_rsp, 1, true },
	{ 0x0c0d,  53, "Read Stored Link Key",
				read_stored_link_key_cmd, 7, true,
				read_stored_link_key_rsp, 5, true },
	{ 0x0c11,  54, "Write Stored Link Key",
				write_stored_link_key_cmd, 1, false,
				write_stored_link_key_rsp, 2, true },
	{ 0x0c12,  55, "Delete Stored Link Key",
				delete_stored_link_key_cmd, 7, true,
				delete_stored_link_key_rsp, 3, true },
	{ 0x0c13,  56, "Write Local Name",
				write_local_name_cmd, 248, true,
				status_rsp, 1, true },
	{ 0x0c14,  57, "Read Local Name",
				null_cmd, 0, true,
				read_local_name_rsp, 249, true },
	{ 0x0c15,  58, "Read Connection Accept Timeout",
				null_cmd, 0, true,
				read_conn_accept_timeout_rsp, 3, true },
	{ 0x0c16,  59, "Write Connection Accept Timeout",
				write_conn_accept_timeout_cmd, 2, true,
				status_rsp, 1, true },
	{ 0x0c17,  60, "Read Page Timeout",
				null_cmd, 0, true,
				read_page_timeout_rsp, 3, true },
	{ 0x0c18,  61, "Write Page Timeout",
				write_page_timeout_cmd, 2, true,
				status_rsp, 1, true },
	{ 0x0c19,  62, "Read Scan Enable",
				null_cmd, 0, true,
				read_scan_enable_rsp, 2, true },
	{ 0x0c1a,  63, "Write Scan Enable",
				write_scan_enable_cmd, 1, true,
				status_rsp, 1, true },
	{ 0x0c1b,  64, "Read Page Scan Activity",
				null_cmd, 0, true,
				read_page_scan_activity_rsp, 5, true },
	{ 0x0c1c,  65, "Write Page Scan Activity",
				write_page_scan_activity_cmd, 4, true,
				status_rsp, 1, true },
	{ 0x0c1d,  66, "Read Inquiry Scan Activity",
				null_cmd, 0, true,
				read_inquiry_scan_activity_rsp, 5, true },
	{ 0x0c1e,  67, "Write Inquiry Scan Activity",
				write_inquiry_scan_activity_cmd, 4, true,
				status_rsp, 1, true },
	{ 0x0c1f,  68, "Read Authentication Enable",
				null_cmd, 0, true,
				read_auth_enable_rsp, 2, true },
	{ 0x0c20,  69, "Write Authentication Enable",
				write_auth_enable_cmd, 1, true,
				status_rsp, 1, true },
	{ 0x0c21,  70, "Read Encryption Mode",
				null_cmd, 0, true,
				read_encrypt_mode_rsp, 2, true },
	{ 0x0c22,  71, "Write Encryption Mode",
				write_encrypt_mode_cmd, 1, true,
				status_rsp, 1, true },
	{ 0x0c23,  72, "Read Class of Device",
				null_cmd, 0, true,
				read_class_of_dev_rsp, 4, true },
	{ 0x0c24,  73, "Write Class of Device",
				write_class_of_dev_cmd, 3, true,
				status_rsp, 1, true },
	{ 0x0c25,  74, "Read Voice Setting",
				null_cmd, 0, true,
				read_voice_setting_rsp, 3, true },
	{ 0x0c26,  75, "Write Voice Setting",
				write_voice_setting_cmd, 2, true,
				status_rsp, 1, true },
	{ 0x0c27,  76, "Read Automatic Flush Timeout" },
	{ 0x0c28,  77, "Write Automatic Flush Timeout" },
	{ 0x0c29,  78, "Read Num Broadcast Retransmissions" },
	{ 0x0c2a,  79, "Write Num Broadcast Retransmissions" },
	{ 0x0c2b,  80, "Read Hold Mode Activity" },
	{ 0x0c2c,  81, "Write Hold Mode Activity" },
	{ 0x0c2d,  82, "Read Transmit Power Level" },
	{ 0x0c2e,  83, "Read Sync Flow Control Enable" },
	{ 0x0c2f,  84, "Write Sync Flow Control Enable" },
	{ 0x0c31,  85, "Set Host Controller To Host Flow" },
	{ 0x0c33,  86, "Host Buffer Size",
				host_buffer_size_cmd, 7, true,
				status_rsp, 1, true },
	{ 0x0c35,  87, "Host Number of Completed Packets" },
	{ 0x0c36,  88, "Read Link Supervision Timeout",
				read_link_supv_timeout_cmd, 2, true,
				read_link_supv_timeout_rsp, 5, true },
	{ 0x0c37,  89, "Write Link Supervision Timeout",
				write_link_supv_timeout_cmd, 4, true,
				write_link_supv_timeout_rsp, 3, true },
	{ 0x0c38,  90, "Read Number of Supported IAC",
				null_cmd, 0, true,
				read_num_supported_iac_rsp, 2, true },
	{ 0x0c39,  91, "Read Current IAC LAP",
				null_cmd, 0, true,
				read_current_iac_lap_rsp, 2, false },
	{ 0x0c3a,  92, "Write Current IAC LAP",
				write_current_iac_lap_cmd, 1, false,
				status_rsp, 1, true },
	{ 0x0c3b,  93, "Read Page Scan Period Mode",
				null_cmd, 0, true,
				read_page_scan_period_mode_rsp, 2, true },
	{ 0x0c3c,  94, "Write Page Scan Period Mode",
				write_page_scan_period_mode_cmd, 1, true,
				status_rsp, 1, true },
	{ 0x0c3d,  95, "Read Page Scan Mode",
				null_cmd, 0, true,
				read_page_scan_mode_rsp, 2, true },
	{ 0x0c3e,  96, "Write Page Scan Mode",
				write_page_scan_mode_cmd, 1, true,
				status_rsp, 1, true },
	{ 0x0c3f,  97, "Set AFH Host Channel Classification",
				set_afh_host_classification_cmd, 10, true,
				status_rsp, 1, true },
	{ 0x0c42, 100, "Read Inquiry Scan Type",
				null_cmd, 0, true,
				read_inquiry_scan_type_rsp, 2, true },
	{ 0x0c43, 101, "Write Inquiry Scan Type",
				write_inquiry_scan_type_cmd, 1, true,
				status_rsp, 1, true },
	{ 0x0c44, 102, "Read Inquiry Mode",
				null_cmd, 0, true,
				read_inquiry_mode_rsp, 2, true },
	{ 0x0c45, 103, "Write Inquiry Mode",
				write_inquiry_mode_cmd, 1, true,
				status_rsp, 1, true },
	{ 0x0c46, 104, "Read Page Scan Type",
				null_cmd, 0, true,
				read_page_scan_type_rsp, 2, true },
	{ 0x0c47, 105, "Write Page Scan Type",
				write_page_scan_type_cmd, 1, true,
				status_rsp, 1, true },
	{ 0x0c48, 106, "Read AFH Channel Assessment Mode",
				null_cmd, 0, true,
				read_afh_assessment_mode_rsp, 2, true },
	{ 0x0c49, 107, "Write AFH Channel Assessment Mode",
				write_afh_assessment_mode_cmd, 1, true,
				status_rsp, 1, true },
	{ 0x0c51, 136, "Read Extended Inquiry Response",
				null_cmd, 0, true,
				read_ext_inquiry_response_rsp, 242, true },
	{ 0x0c52, 137, "Write Extended Inquiry Response",
				write_ext_inquiry_response_cmd, 241, true,
				status_rsp, 1, true },
	{ 0x0c53, 138, "Refresh Encryption Key",
				refresh_encrypt_key_cmd, 2, true },
	{ 0x0c55, 141, "Read Simple Pairing Mode",
				null_cmd, 0, true,
				read_simple_pairing_mode_rsp, 2, true },
	{ 0x0c56, 142, "Write Simple Pairing Mode",
				write_simple_pairing_mode_cmd, 1, true,
				status_rsp, 1, true },
	{ 0x0c57, 143, "Read Local OOB Data",
				null_cmd, 0, true,
				read_local_oob_data_rsp, 33, true },
	{ 0x0c58, 144, "Read Inquiry Response TX Power Level",
				null_cmd, 0, true,
				read_inquiry_resp_tx_power_rsp, 2, true },
	{ 0x0c59, 145, "Write Inquiry Transmit Power Level",
				write_inquiry_tx_power_cmd, 1, true,
				status_rsp, 1, true },
	{ 0x0c5a, 146, "Read Default Erroneous Reporting" },
	{ 0x0c5b, 147, "Write Default Erroneous Reporting" },
	{ 0x0c5f, 158, "Enhanced Flush",
				enhanced_flush_cmd, 3, true },
	{ 0x0c60, 162, "Send Keypress Notification" },
	{ 0x0c61, 176, "Read Logical Link Accept Timeout" },
	{ 0x0c62, 177, "Write Logical Link Accept Timeout" },
	{ 0x0c63, 178, "Set Event Mask Page 2",
				set_event_mask_page2_cmd, 8, true,
				status_rsp, 1, true },
	{ 0x0c64, 179, "Read Location Data",
				null_cmd, 0, true,
				read_location_data_rsp, 6, true },
	{ 0x0c65, 180, "Write Location Data",
				write_location_data_cmd, 5, true,
				status_rsp, 1, true },
	{ 0x0c66, 184, "Read Flow Control Mode",
				null_cmd, 0, true,
				read_flow_control_mode_rsp, 2, true },
	{ 0x0c67, 185, "Write Flow Control Mode",
				write_flow_control_mode_cmd, 1, true,
				status_rsp, 1, true },
	{ 0x0c68, 192, "Read Enhanced Transmit Power Level" },
	{ 0x0c69, 194, "Read Best Effort Flush Timeout" },
	{ 0x0c6a, 195, "Write Best Effort Flush Timeout" },
	{ 0x0c6b, 196, "Short Range Mode" },
	{ 0x0c6c, 197, "Read LE Host Supported",
				null_cmd, 0, true,
				read_le_host_supported_rsp, 3, true },
	{ 0x0c6d, 198, "Write LE Host Supported",
				write_le_host_supported_cmd, 2, true,
				status_rsp, 1, true },
	{ 0x0c6e, 238, "Set MWS Channel Parameters" },
	{ 0x0c6f, 239, "Set External Frame Configuration" },
	{ 0x0c70, 240, "Set MWS Signaling" },
	{ 0x0c71, 241, "Set MWS Transport Layer" },
	{ 0x0c72, 242, "Set MWS Scan Frequency Table" },
	{ 0x0c73, 244, "Set MWS Pattern Configuration" },
	{ 0x0c74, 252, "Set Reserved LT_ADDR" },
	{ 0x0c75, 253, "Delete Reserved LT_ADDR" },
	{ 0x0c76, 254, "Set Connectionless Slave Broadcast Data" },
	{ 0x0c77, 255, "Read Synchronization Train Parameters",
				null_cmd, 0, true,
				read_sync_train_params_rsp, 8, true },
	{ 0x0c78, 256, "Write Synchronization Train Parameters" },

	/* OGF 4 - Information Parameter */
	{ 0x1001, 115, "Read Local Version Information",
				null_cmd, 0, true,
				read_local_version_rsp, 9, true },
	{ 0x1002, 116, "Read Local Supported Commands",
				null_cmd, 0, true,
				read_local_commands_rsp, 65, true },
	{ 0x1003, 117, "Read Local Supported Features",
				null_cmd, 0, true,
				read_local_features_rsp, 9, true },
	{ 0x1004, 118, "Read Local Extended Features",
				read_local_ext_features_cmd, 1, true,
				read_local_ext_features_rsp, 11, true },
	{ 0x1005, 119, "Read Buffer Size",
				null_cmd, 0, true,
				read_buffer_size_rsp, 8, true },
	{ 0x1007, 120, "Read Country Code",
				null_cmd, 0, true,
				read_country_code_rsp, 2, true },
	{ 0x1009, 121, "Read BD ADDR",
				null_cmd, 0, true,
				read_bd_addr_rsp, 7, true },
	{ 0x100a, 186, "Read Data Block Size",
				null_cmd, 0, true,
				read_data_block_size_rsp, 7, true },
	{ 0x100b, 237, "Read Local Supported Codecs" },

	/* OGF 5 - Status Parameter */
	{ 0x1401, 122, "Read Failed Contact Counter",
				read_failed_contact_counter_cmd, 2, true,
				read_failed_contact_counter_rsp, 5, true },
	{ 0x1402, 123, "Reset Failed Contact Counter",
				reset_failed_contact_counter_cmd, 2, true,
				reset_failed_contact_counter_rsp, 3, true },
	{ 0x1403, 124, "Read Link Quality",
				read_link_quality_cmd, 2, true,
				read_link_quality_rsp, 4, true },
	{ 0x1405, 125, "Read RSSI",
				read_rssi_cmd, 2, true,
				read_rssi_rsp, 4, true },
	{ 0x1406, 126, "Read AFH Channel Map",
				read_afh_channel_map_cmd, 2, true,
				read_afh_channel_map_rsp, 14, true },
	{ 0x1407, 127, "Read Clock",
				read_clock_cmd, 3, true,
				read_clock_rsp, 9, true },
	{ 0x1408, 164, "Read Encryption Key Size",
				read_encrypt_key_size_cmd, 2, true,
				read_encrypt_key_size_rsp, 4, true },
	{ 0x1409, 181, "Read Local AMP Info",
				null_cmd, 0, true,
				read_local_amp_info_rsp, 31, true },
	{ 0x140a, 182, "Read Local AMP ASSOC",
				read_local_amp_assoc_cmd, 5, true,
				read_local_amp_assoc_rsp, 5, false },
	{ 0x140b, 183, "Write Remote AMP ASSOC",
				write_remote_amp_assoc_cmd, 6, false,
				write_remote_amp_assoc_rsp, 2, true },
	{ 0x140c, 243, "Get MWS Transport Layer Configuration" },
	{ 0x140d, 245, "Set Triggered Clock Capture" },

	/* OGF 6 - Testing */
	{ 0x1801, 128, "Read Loopback Mode" },
	{ 0x1802, 129, "Write Loopback Mode" },
	{ 0x1803, 130, "Enable Device Under Test Mode",
				null_cmd, 0, true,
				status_rsp, 1, true },
	{ 0x1804, 157, "Write Simple Pairing Debug Mode" },
	{ 0x1807, 189, "Enable AMP Receiver Reports" },
	{ 0x1808, 190, "AMP Test End" },
	{ 0x1809, 191, "AMP Test" },

	/* OGF 8 - LE Control */
	{ 0x2001, 200, "LE Set Event Mask",
				le_set_event_mask_cmd, 8, true,
				status_rsp, 1, true },
	{ 0x2002, 201, "LE Read Buffer Size",
				null_cmd, 0, true,
				le_read_buffer_size_rsp, 4, true },
	{ 0x2003, 202, "LE Read Local Supported Features",
				null_cmd, 0, true,
				le_read_local_features_rsp, 9, true },
	{ 0x2005, 204, "LE Set Random Address",
				le_set_random_address_cmd, 6, true,
				status_rsp, 1, true },
	{ 0x2006, 205, "LE Set Advertising Parameters",
				le_set_adv_parameters_cmd, 15, true,
				status_rsp, 1, true },
	{ 0x2007, 206, "LE Read Advertising Channel TX Power",
				null_cmd, 0, true,
				le_read_adv_tx_power_rsp, 2, true },
	{ 0x2008, 207, "LE Set Advertising Data",
				le_set_adv_data_cmd, 32, true,
				status_rsp, 1, true },
	{ 0x2009, 208, "LE Set Scan Response Data",
				le_set_scan_rsp_data_cmd, 32, true,
				status_rsp, 1, true },
	{ 0x200a, 209, "LE Set Advertise Enable",
				le_set_adv_enable_cmd, 1, true,
				status_rsp, 1, true },
	{ 0x200b, 210, "LE Set Scan Parameters",
				le_set_scan_parameters_cmd, 7, true,
				status_rsp, 1, true },
	{ 0x200c, 211, "LE Set Scan Enable",
				le_set_scan_enable_cmd, 2, true,
				status_rsp, 1, true },
	{ 0x200d, 212, "LE Create Connection",
				le_create_conn_cmd, 25, true },
	{ 0x200e, 213, "LE Create Connection Cancel",
				null_cmd, 0, true,
				status_rsp, 1, true },
	{ 0x200f, 214, "LE Read White List Size",
				null_cmd, 0, true,
				le_read_white_list_size_rsp, 2, true },
	{ 0x2010, 215, "LE Clear White List",
				null_cmd, 0, true,
				status_rsp, 1, true },
	{ 0x2011, 216, "LE Add Device To White List",
				le_add_to_white_list_cmd, 7, true,
				status_rsp, 1, true },
	{ 0x2012, 217, "LE Remove Device From White List",
				le_remove_from_white_list_cmd, 7, true,
				status_rsp, 1, true },
	{ 0x2013, 218, "LE Connection Update",
				le_conn_update_cmd, 14, true },
	{ 0x2014, 219, "LE Set Host Channel Classification",
				le_set_host_classification_cmd, 5, true,
				status_rsp, 1, true },
	{ 0x2015, 220, "LE Read Channel Map",
				le_read_channel_map_cmd, 2, true,
				le_read_channel_map_rsp, 8, true },
	{ 0x2016, 221, "LE Read Remote Used Features",
				le_read_remote_features_cmd, 2, true },
	{ 0x2017, 222, "LE Encrypt",
				le_encrypt_cmd, 32, true,
				le_encrypt_rsp, 17, true },
	{ 0x2018, 223, "LE Rand",
				null_cmd, 0, true,
				le_rand_rsp, 9, true },
	{ 0x2019, 224, "LE Start Encryption",
				le_start_encrypt_cmd, 28, true },
	{ 0x201a, 225, "LE Long Term Key Request Reply",
				le_ltk_req_reply_cmd, 18, true,
				le_ltk_req_reply_rsp, 3, true },
	{ 0x201b, 226, "LE Long Term Key Request Neg Reply",
				le_ltk_req_neg_reply_cmd, 2, true,
				le_ltk_req_neg_reply_rsp, 3, true },
	{ 0x201c, 227, "LE Read Supported States",
				null_cmd, 0, true,
				le_read_supported_states_rsp, 9, true },
	{ 0x201d, 228, "LE Receiver Test",
				le_receiver_test_cmd, 1, true,
				status_rsp, 1, true },
	{ 0x201e, 229, "LE Transmitter Test",
				le_transmitter_test_cmd, 3, true,
				status_rsp, 1, true },
	{ 0x201f, 230, "LE Test End",
				null_cmd, 0, true,
				le_test_end_rsp, 3, true },
	{ }
};

static const char *get_supported_command(int bit)
{
	int i;

	for (i = 0; opcode_table[i].str; i++) {
		if (opcode_table[i].bit == bit)
			return opcode_table[i].str;
	}

	return NULL;
}

static void status_evt(const void *data, uint8_t size)
{
	uint8_t status = *((uint8_t *) data);

	print_status(status);
}

static void inquiry_result_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_inquiry_result *evt = data;

	print_num_resp(evt->num_resp);
	print_bdaddr(evt->bdaddr);
	print_pscan_rep_mode(evt->pscan_rep_mode);
	print_pscan_period_mode(evt->pscan_period_mode);
	print_pscan_mode(evt->pscan_mode);
	print_dev_class(evt->dev_class);
	print_clock_offset(evt->clock_offset);

	if (size > sizeof(*evt))
		packet_hexdump(data + sizeof(*evt), size - sizeof(*evt));
}

static void conn_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_conn_complete *evt = data;

	print_status(evt->status);
	print_handle(evt->handle);
	print_bdaddr(evt->bdaddr);
	print_link_type(evt->link_type);
	print_encr_mode(evt->encr_mode);

	if (evt->status == 0x00)
		assign_handle(btohs(evt->handle), 0x00);
}

static void conn_request_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_conn_request *evt = data;

	print_bdaddr(evt->bdaddr);
	print_dev_class(evt->dev_class);
	print_link_type(evt->link_type);
}

static void disconnect_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_disconnect_complete *evt = data;

	print_status(evt->status);
	print_handle(evt->handle);
	print_reason(evt->reason);

	if (evt->status == 0x00)
		release_handle(btohs(evt->handle));
}

static void auth_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_auth_complete *evt = data;

	print_status(evt->status);
	print_handle(evt->handle);
}

static void remote_name_request_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_remote_name_request_complete *evt = data;

	print_status(evt->status);
	print_bdaddr(evt->bdaddr);
	print_name(evt->name);
}

static void encrypt_change_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_encrypt_change *evt = data;

	print_status(evt->status);
	print_handle(evt->handle);
	print_encr_mode_change(evt->encr_mode, evt->handle);
}

static void change_conn_link_key_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_change_conn_link_key_complete *evt = data;

	print_status(evt->status);
	print_handle(evt->handle);
}

static void master_link_key_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_master_link_key_complete *evt = data;

	print_status(evt->status);
	print_handle(evt->handle);
	print_key_flag(evt->key_flag);
}

static void remote_features_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_remote_features_complete *evt = data;

	print_status(evt->status);
	print_handle(evt->handle);
	print_features(0, evt->features, 0x00);
}

static void remote_version_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_remote_version_complete *evt = data;

	print_status(evt->status);
	print_handle(evt->handle);
	print_lmp_version(evt->lmp_ver, evt->lmp_subver);
	print_manufacturer(evt->manufacturer);
}

static void qos_setup_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_qos_setup_complete *evt = data;

	print_status(evt->status);
	print_handle(evt->handle);
	print_field("Flags: 0x%2.2x", evt->flags);

	print_service_type(evt->service_type);

	print_field("Token rate: %d", btohl(evt->token_rate));
	print_field("Peak bandwidth: %d", btohl(evt->peak_bandwidth));
	print_field("Latency: %d", btohl(evt->latency));
	print_field("Delay variation: %d", btohl(evt->delay_variation));
}

static void cmd_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_cmd_complete *evt = data;
	uint16_t opcode = btohs(evt->opcode);
	uint16_t ogf = cmd_opcode_ogf(opcode);
	uint16_t ocf = cmd_opcode_ocf(opcode);
	const struct opcode_data *opcode_data = NULL;
	const char *opcode_color, *opcode_str;
	int i;

	for (i = 0; opcode_table[i].str; i++) {
		if (opcode_table[i].opcode == opcode) {
			opcode_data = &opcode_table[i];
			break;
		}
	}

	if (opcode_data) {
		if (opcode_data->rsp_func)
			opcode_color = COLOR_HCI_COMMAND;
		else
			opcode_color = COLOR_HCI_COMMAND_UNKNOWN;
		opcode_str = opcode_data->str;
	} else {
		opcode_color = COLOR_HCI_COMMAND_UNKNOWN;
		opcode_str = "Unknown";
	}

	print_indent(6, opcode_color, "", opcode_str, COLOR_OFF,
			" (0x%2.2x|0x%4.4x) ncmd %d", ogf, ocf, evt->ncmd);

	if (!opcode_data || !opcode_data->rsp_func) {
		packet_hexdump(data + 3, size - 3);
		return;
	}

	if (opcode_data->rsp_fixed) {
		if (size - 3 != opcode_data->rsp_size) {
			print_text(COLOR_ERROR, "invalid packet size");
			packet_hexdump(data + 3, size - 3);
			return;
		}
	} else {
		if (size - 3 < opcode_data->rsp_size) {
			print_text(COLOR_ERROR, "too short packet");
			packet_hexdump(data + 3, size - 3);
			return;
		}
	}

	opcode_data->rsp_func(data + 3, size - 3);
}

static void cmd_status_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_cmd_status *evt = data;
	uint16_t opcode = btohs(evt->opcode);
	uint16_t ogf = cmd_opcode_ogf(opcode);
	uint16_t ocf = cmd_opcode_ocf(opcode);
	const struct opcode_data *opcode_data = NULL;
	const char *opcode_color, *opcode_str;
	int i;

	for (i = 0; opcode_table[i].str; i++) {
		if (opcode_table[i].opcode == opcode) {
			opcode_data = &opcode_table[i];
			break;
		}
	}

	if (opcode_data) {
		opcode_color = COLOR_HCI_COMMAND;
		opcode_str = opcode_data->str;
	} else {
		opcode_color = COLOR_HCI_COMMAND_UNKNOWN;
		opcode_str = "Unknown";
	}

	print_indent(6, opcode_color, "", opcode_str, COLOR_OFF,
			" (0x%2.2x|0x%4.4x) ncmd %d", ogf, ocf, evt->ncmd);

	print_status(evt->status);
}

static void hardware_error_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_hardware_error *evt = data;

	print_field("Code: 0x%2.2x", evt->code);
}

static void flush_occurred_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_flush_occurred *evt = data;

	print_handle(evt->handle);
}

static void role_change_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_role_change *evt = data;

	print_status(evt->status);
	print_bdaddr(evt->bdaddr);
	print_role(evt->role);
}

static void num_completed_packets_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_num_completed_packets *evt = data;

	print_field("Num handles: %d", evt->num_handles);
	print_handle(evt->handle);
	print_field("Count: %d", btohs(evt->count));

	if (size > sizeof(*evt))
		packet_hexdump(data + sizeof(*evt), size - sizeof(*evt));
}

static void mode_change_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_mode_change *evt = data;

	print_status(evt->status);
	print_handle(evt->handle);
	print_mode(evt->mode);
	print_interval(evt->interval);
}

static void return_link_keys_evt(const void *data, uint8_t size)
{
	uint8_t num_keys = *((uint8_t *) data);

	print_field("Num keys: %d", num_keys);

	packet_hexdump(data + 1, size - 1);
}

static void pin_code_request_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_pin_code_request *evt = data;

	print_bdaddr(evt->bdaddr);
}

static void link_key_request_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_link_key_request *evt = data;

	print_bdaddr(evt->bdaddr);
}

static void link_key_notify_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_link_key_notify *evt = data;

	print_bdaddr(evt->bdaddr);
	print_link_key(evt->link_key);
	print_key_type(evt->key_type);
}

static void loopback_command_evt(const void *data, uint8_t size)
{
	packet_hexdump(data, size);
}

static void data_buffer_overflow_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_data_buffer_overflow *evt = data;

	print_link_type(evt->link_type);
}

static void max_slots_change_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_max_slots_change *evt = data;

	print_handle(evt->handle);
	print_field("Max slots: %d", evt->max_slots);
}

static void clock_offset_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_clock_offset_complete *evt = data;

	print_status(evt->status);
	print_handle(evt->handle);
	print_clock_offset(evt->clock_offset);
}

static void conn_pkt_type_changed_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_conn_pkt_type_changed *evt = data;

	print_status(evt->status);
	print_handle(evt->handle);
	print_pkt_type(evt->pkt_type);
}

static void qos_violation_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_qos_violation *evt = data;

	print_handle(evt->handle);
}

static void pscan_mode_change_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_pscan_mode_change *evt = data;

	print_bdaddr(evt->bdaddr);
	print_pscan_mode(evt->pscan_mode);
}

static void pscan_rep_mode_change_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_pscan_rep_mode_change *evt = data;

	print_bdaddr(evt->bdaddr);
	print_pscan_rep_mode(evt->pscan_rep_mode);
}

static void flow_spec_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_flow_spec_complete *evt = data;

	print_status(evt->status);
	print_handle(evt->handle);
	print_field("Flags: 0x%2.2x", evt->flags);

	print_flow_direction(evt->direction);
	print_service_type(evt->service_type);

	print_field("Token rate: %d", btohl(evt->token_rate));
	print_field("Token bucket size: %d", btohl(evt->token_bucket_size));
	print_field("Peak bandwidth: %d", btohl(evt->peak_bandwidth));
	print_field("Access latency: %d", btohl(evt->access_latency));
}

static void inquiry_result_with_rssi_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_inquiry_result_with_rssi *evt = data;

	print_num_resp(evt->num_resp);
	print_bdaddr(evt->bdaddr);
	print_pscan_rep_mode(evt->pscan_rep_mode);
	print_pscan_period_mode(evt->pscan_period_mode);
	print_dev_class(evt->dev_class);
	print_clock_offset(evt->clock_offset);
	print_rssi(evt->rssi);

	if (size > sizeof(*evt))
		packet_hexdump(data + sizeof(*evt), size - sizeof(*evt));
}

static void remote_ext_features_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_remote_ext_features_complete *evt = data;

	print_status(evt->status);
	print_handle(evt->handle);
	print_field("Page: %d/%d", evt->page, evt->max_page);
	print_features(evt->page, evt->features, 0x00);
}

static void sync_conn_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_sync_conn_complete *evt = data;

	print_status(evt->status);
	print_handle(evt->handle);
	print_bdaddr(evt->bdaddr);
	print_link_type(evt->link_type);
	print_field("Transmission interval: 0x%2.2x", evt->tx_interval);
	print_field("Retransmission window: 0x%2.2x", evt->retrans_window);
	print_field("RX packet length: %d", btohs(evt->rx_pkt_len));
	print_field("TX packet length: %d", btohs(evt->tx_pkt_len));
	print_air_mode(evt->air_mode);
}

static void sync_conn_changed_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_sync_conn_changed *evt = data;

	print_status(evt->status);
	print_handle(evt->handle);
	print_field("Transmission interval: 0x%2.2x", evt->tx_interval);
	print_field("Retransmission window: 0x%2.2x", evt->retrans_window);
	print_field("RX packet length: %d", btohs(evt->rx_pkt_len));
	print_field("TX packet length: %d", btohs(evt->tx_pkt_len));
}

static void sniff_subrating_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_sniff_subrating *evt = data;

	print_status(evt->status);
	print_handle(evt->handle);
	print_slot_625("Max transmit latency", evt->max_tx_latency);
	print_slot_625("Max receive latency", evt->max_rx_latency);
	print_slot_625("Min remote timeout", evt->min_remote_timeout);
	print_slot_625("Min local timeout", evt->min_local_timeout);
}

static void ext_inquiry_result_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_ext_inquiry_result *evt = data;

	print_num_resp(evt->num_resp);
	print_bdaddr(evt->bdaddr);
	print_pscan_rep_mode(evt->pscan_rep_mode);
	print_pscan_period_mode(evt->pscan_period_mode);
	print_dev_class(evt->dev_class);
	print_clock_offset(evt->clock_offset);
	print_rssi(evt->rssi);
	print_eir(evt->data, sizeof(evt->data), false);
}

static void encrypt_key_refresh_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_encrypt_key_refresh_complete *evt = data;

	print_status(evt->status);
	print_handle(evt->handle);
}

static void io_capability_request_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_io_capability_request *evt = data;

	print_bdaddr(evt->bdaddr);
}

static void io_capability_response_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_io_capability_response *evt = data;

	print_bdaddr(evt->bdaddr);
	print_io_capability(evt->capability);
	print_oob_data(evt->oob_data);
	print_authentication(evt->authentication);
}

static void user_confirm_request_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_user_confirm_request *evt = data;

	print_bdaddr(evt->bdaddr);
	print_passkey(evt->passkey);
}

static void user_passkey_request_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_user_passkey_request *evt = data;

	print_bdaddr(evt->bdaddr);
}

static void remote_oob_data_request_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_remote_oob_data_request *evt = data;

	print_bdaddr(evt->bdaddr);
}

static void simple_pairing_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_simple_pairing_complete *evt = data;

	print_status(evt->status);
	print_bdaddr(evt->bdaddr);
}

static void link_supv_timeout_changed_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_link_supv_timeout_changed *evt = data;

	print_handle(evt->handle);
	print_timeout(evt->timeout);
}

static void enhanced_flush_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_enhanced_flush_complete *evt = data;

	print_handle(evt->handle);
}

static void user_passkey_notify_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_user_passkey_notify *evt = data;

	print_bdaddr(evt->bdaddr);
	print_passkey(evt->passkey);
}

static void keypress_notify_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_keypress_notify *evt = data;
	const char *str;

	print_bdaddr(evt->bdaddr);

	switch (evt->type) {
	case 0x00:
		str = "Passkey entry started";
		break;
	case 0x01:
		str = "Passkey digit entered";
		break;
	case 0x02:
		str = "Passkey digit erased";
		break;
	case 0x03:
		str = "Passkey clared";
		break;
	case 0x04:
		str = "Passkey entry completed";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Notification type: %s (0x%2.2x)", str, evt->type);
}

static void remote_host_features_notify_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_remote_host_features_notify *evt = data;

	print_bdaddr(evt->bdaddr);
	print_features(1, evt->features, 0x00);
}

static void phy_link_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_phy_link_complete *evt = data;

	print_status(evt->status);
	print_phy_handle(evt->phy_handle);
}

static void channel_selected_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_channel_selected *evt = data;

	print_phy_handle(evt->phy_handle);
}

static void disconn_phy_link_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_disconn_phy_link_complete *evt = data;

	print_status(evt->status);
	print_phy_handle(evt->phy_handle);
	print_reason(evt->reason);
}

static void phy_link_loss_early_warning_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_phy_link_loss_early_warning *evt = data;
	const char *str;

	print_phy_handle(evt->phy_handle);

	switch (evt->reason) {
	case 0x00:
		str = "Unknown";
		break;
	case 0x01:
		str = "Range related";
		break;
	case 0x02:
		str = "Bandwidth related";
		break;
	case 0x03:
		str = "Resolving conflict";
		break;
	case 0x04:
		str = "Interference";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Reason: %s (0x%2.2x)", str, evt->reason);
}

static void phy_link_recovery_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_phy_link_recovery *evt = data;

	print_phy_handle(evt->phy_handle);
}

static void logic_link_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_logic_link_complete *evt = data;

	print_status(evt->status);
	print_handle(evt->handle);
	print_phy_handle(evt->phy_handle);
	print_field("TX flow spec: 0x%2.2x", evt->flow_spec);
}

static void disconn_logic_link_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_disconn_logic_link_complete *evt = data;

	print_status(evt->status);
	print_handle(evt->handle);
	print_reason(evt->reason);
}

static void flow_spec_modify_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_flow_spec_modify_complete *evt = data;

	print_status(evt->status);
	print_handle(evt->handle);
}

static void num_completed_data_blocks_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_num_completed_data_blocks *evt = data;

	print_field("Total num data blocks: %d", btohs(evt->total_num_blocks));
	print_field("Num handles: %d", evt->num_handles);
	print_handle(evt->handle);
	print_field("Num packets: %d", evt->num_packets);
	print_field("Num blocks: %d", evt->num_blocks);

	if (size > sizeof(*evt))
		packet_hexdump(data + sizeof(*evt), size - sizeof(*evt));
}

static void short_range_mode_change_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_short_range_mode_change *evt = data;

	print_status(evt->status);
	print_phy_handle(evt->phy_handle);
	print_short_range_mode(evt->mode);
}

static void amp_status_change_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_amp_status_change *evt = data;

	print_status(evt->status);
	print_amp_status(evt->amp_status);
}

static void le_conn_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_le_conn_complete *evt = data;

	print_status(evt->status);
	print_handle(evt->handle);
	print_role(evt->role);
	print_addr_type("Peer address type", evt->peer_addr_type);
	print_addr("Peer address", evt->peer_addr, evt->peer_addr_type);
	print_slot_125("Connection interval", evt->interval);
	print_slot_125("Connection latency", evt->latency);
	print_field("Supervision timeout: %d msec (0x%4.4x)",
		btohs(evt->supv_timeout) * 10, btohs(evt->supv_timeout));
	print_field("Master clock accuracy: 0x%2.2x", evt->clock_accuracy);

	if (evt->status == 0x00)
		assign_handle(btohs(evt->handle), 0x01);
}

static void le_adv_report_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_le_adv_report *evt = data;
	const char *str;
	uint8_t evt_len;
	int8_t *rssi;

	print_num_reports(evt->num_reports);

	switch (evt->event_type) {
	case 0x00:
		str = "Connectable undirected - ADV_IND";
		break;
	case 0x01:
		str = "Connectable directed - ADV_DIRECT_IND";
		break;
	case 0x02:
		str = "Scannable undirected - ADV_SCAN_IND";
		break;
	case 0x03:
		str = "Non connectable undirected - ADV_NONCONN_IND";
		break;
	case 0x04:
		str = "Scan response - SCAN_RSP";
		break;
	default:
		str = "Reserved";
		break;
	}

	print_field("Event type: %s (0x%2.2x)", str, evt->event_type);
	print_addr_type("Address type", evt->addr_type);
	print_addr("Address", evt->addr, evt->addr_type);
	print_field("Data length: %d", evt->data_len);
	print_eir(evt->data, evt->data_len, true);

	rssi = (int8_t *) (evt->data + evt->data_len);
	print_rssi(*rssi);

	evt_len = sizeof(*evt) + evt->data_len + 1;

	if (size > evt_len)
		packet_hexdump(data + evt_len, size - evt_len);
}

static void le_conn_update_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_le_conn_update_complete *evt = data;

	print_status(evt->status);
	print_handle(evt->handle);
	print_slot_125("Connection interval", evt->interval);
	print_slot_125("Connection latency", evt->latency);
	print_field("Supervision timeout: %d msec (0x%4.4x)",
		btohs(evt->supv_timeout) * 10, btohs(evt->supv_timeout));
}

static void le_remote_features_complete_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_le_remote_features_complete *evt = data;

	print_status(evt->status);
	print_handle(evt->handle);
	print_features(0, evt->features, 0x01);
}

static void le_long_term_key_request_evt(const void *data, uint8_t size)
{
	const struct bt_hci_evt_le_long_term_key_request *evt = data;

	print_handle(evt->handle);
	print_random_number(evt->number);
	print_field("Encryption diversifier: 0x%4.4x",
					btohs(evt->diversifier));
}

struct subevent_data {
	uint8_t subevent;
	const char *str;
	void (*func) (const void *data, uint8_t size);
	uint8_t size;
	bool fixed;
};

static const struct subevent_data subevent_table[] = {
	{ 0x01, "LE Connection Complete",
				le_conn_complete_evt, 18, true },
	{ 0x02, "LE Advertising Report",
				le_adv_report_evt, 1, false },
	{ 0x03, "LE Connection Update Complete",
				le_conn_update_complete_evt, 9, true },
	{ 0x04, "LE Read Remote Used Features",
				le_remote_features_complete_evt, 11, true },
	{ 0x05, "LE Long Term Key Request",
				le_long_term_key_request_evt, 12, true },
	{ }
};

static void le_meta_event_evt(const void *data, uint8_t size)
{
	uint8_t subevent = *((const uint8_t *) data);
	const struct subevent_data *subevent_data = NULL;
	const char *subevent_color, *subevent_str;
	int i;

	for (i = 0; subevent_table[i].str; i++) {
		if (subevent_table[i].subevent == subevent) {
			subevent_data = &subevent_table[i];
			break;
		}
	}

	if (subevent_data) {
		if (subevent_data->func)
			subevent_color = COLOR_HCI_EVENT;
		else
			subevent_color = COLOR_HCI_EVENT_UNKNOWN;
		subevent_str = subevent_data->str;
	} else {
		subevent_color = COLOR_HCI_EVENT_UNKNOWN;
		subevent_str = "Unknown";
	}

	print_indent(6, subevent_color, "", subevent_str, COLOR_OFF,
						" (0x%2.2x)", subevent);

	if (!subevent_data || !subevent_data->func) {
		packet_hexdump(data + 1, size - 1);
		return;
	}

	if (subevent_data->fixed) {
		if (size - 1 != subevent_data->size) {
			print_text(COLOR_ERROR, "invalid packet size");
			packet_hexdump(data + 1, size - 1);
			return;
		}
	} else {
		if (size - 1 < subevent_data->size) {
			print_text(COLOR_ERROR, "too short packet");
			packet_hexdump(data + 1, size - 1);
			return;
		}
	}

	subevent_data->func(data + 1, size - 1);
}

static void vendor_evt(const void *data, uint8_t size)
{
	vendor_event(0xffff, data, size);
}

struct event_data {
	uint8_t event;
	const char *str;
	void (*func) (const void *data, uint8_t size);
	uint8_t size;
	bool fixed;
};

static const struct event_data event_table[] = {
	{ 0x01, "Inquiry Complete",
				status_evt, 1, true },
	{ 0x02, "Inquiry Result",
				inquiry_result_evt, 1, false },
	{ 0x03, "Connect Complete",
				conn_complete_evt, 11, true },
	{ 0x04, "Connect Request",
				conn_request_evt, 10, true },
	{ 0x05, "Disconnect Complete",
				disconnect_complete_evt, 4, true },
	{ 0x06, "Auth Complete",
				auth_complete_evt, 3, true },
	{ 0x07, "Remote Name Req Complete",
				remote_name_request_complete_evt, 255, true },
	{ 0x08, "Encryption Change",
				encrypt_change_evt, 4, true },
	{ 0x09, "Change Connection Link Key Complete",
				change_conn_link_key_complete_evt, 3, true },
	{ 0x0a, "Master Link Key Complete",
				master_link_key_complete_evt, 4, true },
	{ 0x0b, "Read Remote Supported Features",
				remote_features_complete_evt, 11, true },
	{ 0x0c, "Read Remote Version Complete",
				remote_version_complete_evt, 8, true },
	{ 0x0d, "QoS Setup Complete",
				qos_setup_complete_evt, 21, true },
	{ 0x0e, "Command Complete",
				cmd_complete_evt, 3, false },
	{ 0x0f, "Command Status",
				cmd_status_evt, 4, true },
	{ 0x10, "Hardware Error",
				hardware_error_evt, 1, true },
	{ 0x11, "Flush Occurred",
				flush_occurred_evt, 2, true },
	{ 0x12, "Role Change",
				role_change_evt, 8, true },
	{ 0x13, "Number of Completed Packets",
				num_completed_packets_evt, 1, false },
	{ 0x14, "Mode Change",
				mode_change_evt, 6, true },
	{ 0x15, "Return Link Keys",
				return_link_keys_evt, 1, false },
	{ 0x16, "PIN Code Request",
				pin_code_request_evt, 6, true },
	{ 0x17, "Link Key Request",
				link_key_request_evt, 6, true },
	{ 0x18, "Link Key Notification",
				link_key_notify_evt, 23, true },
	{ 0x19, "Loopback Command",
				loopback_command_evt, 3, false },
	{ 0x1a, "Data Buffer Overflow",
				data_buffer_overflow_evt, 1, true },
	{ 0x1b, "Max Slots Change",
				max_slots_change_evt, 3, true },
	{ 0x1c, "Read Clock Offset Complete",
				clock_offset_complete_evt, 5, true },
	{ 0x1d, "Connection Packet Type Changed",
				conn_pkt_type_changed_evt, 5, true },
	{ 0x1e, "QoS Violation",
				qos_violation_evt, 2, true },
	{ 0x1f, "Page Scan Mode Change",
				pscan_mode_change_evt, 7, true },
	{ 0x20, "Page Scan Repetition Mode Change",
				pscan_rep_mode_change_evt, 7, true },
	{ 0x21, "Flow Specification Complete",
				flow_spec_complete_evt, 22, true },
	{ 0x22, "Inquiry Result with RSSI",
				inquiry_result_with_rssi_evt, 1, false },
	{ 0x23, "Read Remote Extended Features",
				remote_ext_features_complete_evt, 13, true },
	{ 0x2c, "Synchronous Connect Complete",
				sync_conn_complete_evt, 17, true },
	{ 0x2d, "Synchronous Connect Changed",
				sync_conn_changed_evt, 9, true },
	{ 0x2e, "Sniff Subrating",
				sniff_subrating_evt, 11, true },
	{ 0x2f, "Extended Inquiry Result",
				ext_inquiry_result_evt, 1, false },
	{ 0x30, "Encryption Key Refresh Complete",
				encrypt_key_refresh_complete_evt, 3, true },
	{ 0x31, "IO Capability Request",
				io_capability_request_evt, 6, true },
	{ 0x32, "IO Capability Response",
				io_capability_response_evt, 9, true },
	{ 0x33, "User Confirmation Request",
				user_confirm_request_evt, 10, true },
	{ 0x34, "User Passkey Request",
				user_passkey_request_evt, 6, true },
	{ 0x35, "Remote OOB Data Request",
				remote_oob_data_request_evt, 6, true },
	{ 0x36, "Simple Pairing Complete",
				simple_pairing_complete_evt, 7, true },
	{ 0x38, "Link Supervision Timeout Changed",
				link_supv_timeout_changed_evt, 4, true },
	{ 0x39, "Enhanced Flush Complete",
				enhanced_flush_complete_evt, 2, true },
	{ 0x3b, "User Passkey Notification",
				user_passkey_notify_evt, 10, true },
	{ 0x3c, "Keypress Notification",
				keypress_notify_evt, 7, true },
	{ 0x3d, "Remote Host Supported Features",
				remote_host_features_notify_evt, 14, true },
	{ 0x3e, "LE Meta Event",
				le_meta_event_evt, 1, false },
	{ 0x40, "Physical Link Complete",
				phy_link_complete_evt, 2, true },
	{ 0x41, "Channel Selected",
				channel_selected_evt, 1, true },
	{ 0x42, "Disconnect Physical Link Complete",
				disconn_phy_link_complete_evt, 3, true },
	{ 0x43, "Physical Link Loss Early Warning",
				phy_link_loss_early_warning_evt, 2, true },
	{ 0x44, "Physical Link Recovery",
				phy_link_recovery_evt, 1, true },
	{ 0x45, "Logical Link Complete",
				logic_link_complete_evt, 5, true },
	{ 0x46, "Disconnect Logical Link Complete",
				disconn_logic_link_complete_evt, 4, true },
	{ 0x47, "Flow Specification Modify Complete",
				flow_spec_modify_complete_evt, 3, true },
	{ 0x48, "Number of Completed Data Blocks",
				num_completed_data_blocks_evt, 3, false },
	{ 0x49, "AMP Start Test" },
	{ 0x4a, "AMP Test End" },
	{ 0x4b, "AMP Receiver Report" },
	{ 0x4c, "Short Range Mode Change Complete",
				short_range_mode_change_evt, 3, true },
	{ 0x4d, "AMP Status Change",
				amp_status_change_evt, 2, true },
	{ 0xfe, "Testing" },
	{ 0xff, "Vendor", vendor_evt, 0, false },
	{ }
};

void packet_new_index(struct timeval *tv, uint16_t index, const char *label,
				uint8_t type, uint8_t bus, const char *name)
{
	char details[48];

	sprintf(details, "(%s,%s,%s)", hci_typetostr(type),
					hci_bustostr(bus), name);

	print_packet(tv, index, '=', COLOR_NEW_INDEX, "New Index",
							label, details);
}

void packet_del_index(struct timeval *tv, uint16_t index, const char *label)
{
	print_packet(tv, index, '=', COLOR_DEL_INDEX, "Delete Index",
							label, NULL);
}

void packet_hci_command(struct timeval *tv, uint16_t index,
					const void *data, uint16_t size)
{
	const hci_command_hdr *hdr = data;
	uint16_t opcode = btohs(hdr->opcode);
	uint16_t ogf = cmd_opcode_ogf(opcode);
	uint16_t ocf = cmd_opcode_ocf(opcode);
	const struct opcode_data *opcode_data = NULL;
	const char *opcode_color, *opcode_str;
	char extra_str[25];
	int i;

	if (size < HCI_COMMAND_HDR_SIZE) {
		sprintf(extra_str, "(len %d)", size);
		print_packet(tv, index, '*', COLOR_ERROR,
			"Malformed HCI Command packet", NULL, extra_str);
		packet_hexdump(data, size);
		return;
	}

	data += HCI_COMMAND_HDR_SIZE;
	size -= HCI_COMMAND_HDR_SIZE;

	for (i = 0; opcode_table[i].str; i++) {
		if (opcode_table[i].opcode == opcode) {
			opcode_data = &opcode_table[i];
			break;
		}
	}

	if (opcode_data) {
		if (opcode_data->cmd_func)
			opcode_color = COLOR_HCI_COMMAND;
		else
			opcode_color = COLOR_HCI_COMMAND_UNKNOWN;
		opcode_str = opcode_data->str;
	} else {
		opcode_color = COLOR_HCI_COMMAND_UNKNOWN;
		opcode_str = "Unknown";
	}

	sprintf(extra_str, "(0x%2.2x|0x%4.4x) plen %d", ogf, ocf, hdr->plen);

	print_packet(tv, index, '<', opcode_color, "HCI Command",
							opcode_str, extra_str);

	if (!opcode_data || !opcode_data->cmd_func) {
		packet_hexdump(data, size);
		return;
	}

	if (opcode_data->cmd_fixed) {
		if (hdr->plen != opcode_data->cmd_size) {
			print_text(COLOR_ERROR, "invalid packet size");
			packet_hexdump(data, size);
			return;
		}
	} else {
		if (hdr->plen < opcode_data->cmd_size) {
			print_text(COLOR_ERROR, "too short packet");
			packet_hexdump(data, size);
			return;
		}
	}

	opcode_data->cmd_func(data, hdr->plen);
}

void packet_hci_event(struct timeval *tv, uint16_t index,
					const void *data, uint16_t size)
{
	const hci_event_hdr *hdr = data;
	const struct event_data *event_data = NULL;
	const char *event_color, *event_str;
	char extra_str[25];
	int i;

	if (size < HCI_EVENT_HDR_SIZE) {
		sprintf(extra_str, "(len %d)", size);
		print_packet(tv, index, '*', COLOR_ERROR,
			"Malformed HCI Event packet", NULL, extra_str);
		packet_hexdump(data, size);
		return;
	}

	data += HCI_EVENT_HDR_SIZE;
	size -= HCI_EVENT_HDR_SIZE;

	for (i = 0; event_table[i].str; i++) {
		if (event_table[i].event == hdr->evt) {
			event_data = &event_table[i];
			break;
		}
	}

	if (event_data) {
		if (event_data->func)
			event_color = COLOR_HCI_EVENT;
		else
			event_color = COLOR_HCI_EVENT_UNKNOWN;
		event_str = event_data->str;
	} else {
		event_color = COLOR_HCI_EVENT_UNKNOWN;
		event_str = "Unknown";
	}

	sprintf(extra_str, "(0x%2.2x) plen %d", hdr->evt, hdr->plen);

	print_packet(tv, index, '>', event_color, "HCI Event",
                                                        event_str, extra_str);

	if (!event_data || !event_data->func) {
		packet_hexdump(data, size);
		return;
	}

	if (event_data->fixed) {
		if (hdr->plen != event_data->size) {
			print_text(COLOR_ERROR, "invalid packet size");
			packet_hexdump(data, size);
			return;
		}
	} else {
		if (hdr->plen < event_data->size) {
			print_text(COLOR_ERROR, "too short packet");
			packet_hexdump(data, size);
			return;
		}
	}

	event_data->func(data, hdr->plen);
}

void packet_hci_acldata(struct timeval *tv, uint16_t index, bool in,
					const void *data, uint16_t size)
{
	const struct bt_hci_acl_hdr *hdr = data;
	uint16_t handle = btohs(hdr->handle);
	uint16_t dlen = btohs(hdr->dlen);
	uint8_t flags = acl_flags(handle);
	char handle_str[16], extra_str[32];

	if (size < sizeof(*hdr)) {
		if (in)
			print_packet(tv, index, '*', COLOR_ERROR,
				"Malformed ACL Data RX packet", NULL, NULL);
		else
			print_packet(tv, index, '*', COLOR_ERROR,
				"Malformed ACL Data TX packet", NULL, NULL);
		packet_hexdump(data, size);
		return;
	}

	data += sizeof(*hdr);
	size -= sizeof(*hdr);

	sprintf(handle_str, "Handle %d", acl_handle(handle));
	sprintf(extra_str, "flags 0x%2.2x dlen %d", flags, dlen);

	print_packet(tv, index, in ? '>' : '<', COLOR_HCI_ACLDATA,
				in ? "ACL Data RX" : "ACL Data TX",
						handle_str, extra_str);

	if (size != dlen) {
		print_text(COLOR_ERROR, "invalid packet size (%d != %d)",
								size, dlen);
		packet_hexdump(data, size);
		return;
	}

	if (filter_mask & PACKET_FILTER_SHOW_ACL_DATA)
		packet_hexdump(data, size);

	l2cap_packet(index, in, acl_handle(handle), flags, data, size);
}

void packet_hci_scodata(struct timeval *tv, uint16_t index, bool in,
					const void *data, uint16_t size)
{
	const hci_sco_hdr *hdr = data;
	uint16_t handle = btohs(hdr->handle);
	uint8_t flags = acl_flags(handle);
	char handle_str[16], extra_str[32];

	if (size < HCI_SCO_HDR_SIZE) {
		if (in)
			print_packet(tv, index, '*', COLOR_ERROR,
				"Malformed SCO Data RX packet", NULL, NULL);
		else
			print_packet(tv, index, '*', COLOR_ERROR,
				"Malformed SCO Data TX packet", NULL, NULL);
		packet_hexdump(data, size);
		return;
	}

	data += HCI_SCO_HDR_SIZE;
	size -= HCI_SCO_HDR_SIZE;

	sprintf(handle_str, "Handle %d", acl_handle(handle));
	sprintf(extra_str, "flags 0x%2.2x dlen %d", flags, hdr->dlen);

	print_packet(tv, index, in ? '>' : '<', COLOR_HCI_SCODATA,
				in ? "SCO Data RX" : "SCO Data TX",
						handle_str, extra_str);

	if (size != hdr->dlen) {
		print_text(COLOR_ERROR, "invalid packet size (%d != %d)",
							size, hdr->dlen);
		packet_hexdump(data, size);
		return;
	}

	if (filter_mask & PACKET_FILTER_SHOW_SCO_DATA)
		packet_hexdump(data, size);
}
