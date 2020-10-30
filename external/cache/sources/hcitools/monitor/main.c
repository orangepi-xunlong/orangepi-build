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
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "mainloop.h"
#include "packet.h"
#include "control.h"

static void signal_callback(int signum, void *user_data)
{
	switch (signum) {
	case SIGINT:
	case SIGTERM:
		mainloop_quit();
		break;
	}
}

static void usage(void)
{
	printf("btmon - Bluetooth monitor\n"
		"Usage:\n");
	printf("\tbtmon [options]\n");
	printf("options:\n"
		"\t-r, --read <file>      Read traces in btsnoop format\n"
		"\t-w, --write <file>     Save traces in btsnoop format\n"
		"\t-s, --server <socket>  Start monitor server socket\n"
		"\t-i, --index <num>      Show only specified controller\n"
		"\t-t, --time             Show time instead of time offset\n"
		"\t-T, --date             Show time and date information\n"
		"\t-S, --sco              Dump SCO traffic\n"
		"\t-h, --help             Show help options\n");
}

static const struct option main_options[] = {
	{ "read",    required_argument, NULL, 'r' },
	{ "write",   required_argument, NULL, 'w' },
	{ "server",  required_argument, NULL, 's' },
	{ "index",   required_argument, NULL, 'i' },
	{ "time",    no_argument,       NULL, 't' },
	{ "date",    no_argument,       NULL, 'T' },
	{ "sco",     no_argument,	NULL, 'S' },
	{ "version", no_argument,       NULL, 'v' },
	{ "help",    no_argument,       NULL, 'h' },
	{ }
};

int main(int argc, char *argv[])
{
	unsigned long filter_mask = 0;
	const char *str, *reader_path = NULL, *writer_path = NULL;
	sigset_t mask;

	mainloop_init();

	filter_mask |= PACKET_FILTER_SHOW_TIME_OFFSET;

	for (;;) {
		int opt;

		opt = getopt_long(argc, argv, "r:w:s:i:tTSvh",
						main_options, NULL);
		if (opt < 0)
			break;

		switch (opt) {
		case 'r':
			reader_path = optarg;
			break;
		case 'w':
			writer_path = optarg;
			break;
		case 's':
			control_server(optarg);
			break;
		case 'i':
			if (strlen(optarg) > 3 && !strncmp(optarg, "hci", 3))
				str = optarg + 3;
			else
				str = optarg;
			if (!isdigit(*str)) {
				usage();
				return EXIT_FAILURE;
			}
			packet_select_index(atoi(str));
			break;
		case 't':
			filter_mask &= ~PACKET_FILTER_SHOW_TIME_OFFSET;
			filter_mask |= PACKET_FILTER_SHOW_TIME;
			break;
		case 'T':
			filter_mask &= ~PACKET_FILTER_SHOW_TIME_OFFSET;
			filter_mask |= PACKET_FILTER_SHOW_TIME;
			filter_mask |= PACKET_FILTER_SHOW_DATE;
			break;
		case 'S':
			filter_mask |= PACKET_FILTER_SHOW_SCO_DATA;
			break;
		case 'v':
			printf("%s\n", VERSION);
			return EXIT_SUCCESS;
		case 'h':
			usage();
			return EXIT_SUCCESS;
		default:
			return EXIT_FAILURE;
		}
	}

	if (argc - optind > 0) {
		fprintf(stderr, "Invalid command line parameters\n");
		return EXIT_FAILURE;
	}

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGTERM);

	mainloop_set_signal(&mask, signal_callback, NULL, NULL);

	printf("Bluetooth monitor ver %s\n", VERSION);

	packet_set_filter(filter_mask);

	if (reader_path) {
		control_reader(reader_path);
		return EXIT_SUCCESS;
	}

	if (writer_path)
		control_writer(writer_path);

	if (control_tracing() < 0)
		return EXIT_FAILURE;

	return mainloop_run();
}
