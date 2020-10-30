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

struct l2cap_frame {
	uint16_t index;
	bool in;
	uint16_t handle;
	uint16_t cid;
	const void *data;
	uint16_t size;
};

static inline void l2cap_frame_init(struct l2cap_frame *frame,
				uint16_t index, bool in, uint16_t handle,
				uint16_t cid, const void *data, uint16_t size)
{
	frame->index  = index;
	frame->in     = in;
	frame->handle = handle;
	frame->cid    = cid;
	frame->data   = data;
	frame->size   = size;
}

static inline void l2cap_frame_pull(struct l2cap_frame *frame,
				const struct l2cap_frame *source, uint16_t len)
{
	frame->index   = source->index;
	frame->in      = source->in;
	frame->handle  = source->handle;
	frame->cid     = source->cid;
	frame->data    = source->data + len;
	frame->size    = source->size - len;
}

void l2cap_packet(uint16_t index, bool in, uint16_t handle, uint8_t flags,
					const void *data, uint16_t size);
