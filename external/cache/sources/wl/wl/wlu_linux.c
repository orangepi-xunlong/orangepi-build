/*
 * Linux port of wl command line utility
 *
 * $Copyright (C) 2002-2003 Broadcom Corporation$
 *
 * $Id: wlu_linux.c 394871 2013-04-04 01:14:42Z chihap $
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#ifndef TARGETENV_android
#include <error.h>
#endif /* TARGETENV_android */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <proto/ethernet.h>
#include <proto/bcmip.h>

#ifndef TARGETENV_android
typedef u_int64_t u64;
typedef u_int32_t u32;
typedef u_int16_t u16;
typedef u_int8_t u8;
typedef u_int64_t __u64;
typedef u_int32_t __u32;
typedef u_int16_t __u16;
typedef u_int8_t __u8;
#endif /* TARGETENV_android */

#include <linux/sockios.h>
#include <linux/ethtool.h>
#include <signal.h>
#include <typedefs.h>
#include <wlioctl.h>
#include <bcmutils.h>
#include <sys/wait.h>
#include <netdb.h>
#include <netinet/in.h>
#include "wlu.h"
#include <bcmcdc.h>
#include "wlu_remote.h"
#include "wlu_client_shared.h"
#include "wlu_pipe.h"
#include <miniopt.h>

#define DEV_TYPE_LEN					3 /* length for devtype 'wl'/'et' */
#define INTERACTIVE_NUM_ARGS			15
#define INTERACTIVE_MAX_INPUT_LENGTH	512
#define NO_ERROR						0
#define RWL_WIFI_JOIN_DELAY				5

/* Function prototypes */
static cmd_t *wl_find_cmd(char* name);
static int do_interactive(struct ifreq *ifr);
static int wl_do_cmd(struct ifreq *ifr, char **argv);
int process_args(struct ifreq* ifr, char **argv);
extern int g_child_pid;
/* RemoteWL declarations */
int remote_type = NO_REMOTE;
int rwl_os_type = LINUX_OS;
static bool rwl_dut_autodetect = TRUE;
static bool debug = FALSE;
extern char *g_rwl_buf_mac;
extern char* g_rwl_device_name_serial;
unsigned short g_rwl_servport;
char *g_rwl_servIP = NULL;
unsigned short defined_debug = DEBUG_ERR | DEBUG_INFO;
static uint interactive_flag = 0;
extern char *remote_vista_cmds[];
extern char g_rem_ifname[IFNAMSIZ];
static void
syserr(char *s)
{
	fprintf(stderr, "%s: ", wlu_av0);
	perror(s);
	exit(errno);
}

int
wl_ioctl(void *wl, int cmd, void *buf, int len, bool set)
{
	struct ifreq *ifr = (struct ifreq *) wl;
	wl_ioctl_t ioc;
	int ret = 0;
	int s;

	/* open socket to kernel */
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		syserr("socket");

	/* do it */
	ioc.cmd = cmd;
	ioc.buf = buf;
	ioc.len = len;
	ioc.set = set;
	ifr->ifr_data = (caddr_t) &ioc;
	if ((ret = ioctl(s, SIOCDEVPRIVATE, ifr)) < 0) {
		if (cmd != WLC_GET_MAGIC) {
			ret = IOCTL_ERROR;
		}
	}

	/* cleanup */
	close(s);
	return ret;
}

static int
wl_get_dev_type(char *name, void *buf, int len)
{
	int s;
	int ret;
	struct ifreq ifr;
	struct ethtool_drvinfo info;

	/* open socket to kernel */
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		syserr("socket");

	/* get device type */
	memset(&info, 0, sizeof(info));
	info.cmd = ETHTOOL_GDRVINFO;
	ifr.ifr_data = (caddr_t)&info;
	strncpy(ifr.ifr_name, name, IFNAMSIZ);
	if ((ret = ioctl(s, SIOCETHTOOL, &ifr)) < 0) {

		/* print a good diagnostic if not superuser */
		if (errno == EPERM)
			syserr("wl_get_dev_type");

		*(char *)buf = '\0';
	} else {
		strncpy(buf, info.driver, len);
	}

	close(s);
	return ret;
}

static int
wl_find(struct ifreq *ifr)
{
	char proc_net_dev[] = "/proc/net/dev";
	FILE *fp;
	char buf[1000], *c, *name;
	char dev_type[DEV_TYPE_LEN];
	int status;

	ifr->ifr_name[0] = '\0';

	if (!(fp = fopen(proc_net_dev, "r")))
		return BCME_ERROR;

	/* eat first two lines */
	if (!fgets(buf, sizeof(buf), fp) ||
	    !fgets(buf, sizeof(buf), fp)) {
		fclose(fp);
		return BCME_ERROR;
	}

	while (fgets(buf, sizeof(buf), fp)) {
		c = buf;
		while (isspace(*c))
			c++;
		if (!(name = strsep(&c, ":")))
			continue;
		strncpy(ifr->ifr_name, name, IFNAMSIZ);
		if (wl_get_dev_type(name, dev_type, DEV_TYPE_LEN) >= 0 &&
			!strncmp(dev_type, "wl", 2))
			if (wl_check((void *) ifr) == 0)
				break;
		ifr->ifr_name[0] = '\0';
	}
	if (ifr->ifr_name[0] == '\0')
		status = BCME_ERROR;
	else
		status = BCME_OK;

	fclose(fp);
	return status;
}


static int
ioctl_queryinformation_fe(void *wl, int cmd, void* input_buf, unsigned long *input_len)
{
	int error = NO_ERROR;

	if (remote_type == NO_REMOTE) {
		error = wl_ioctl(wl, cmd, input_buf, *input_len, FALSE);
	} else {
		error = rwl_queryinformation_fe(wl, cmd, input_buf,
		              input_len, 0, REMOTE_GET_IOCTL);

	}
	return error;
}

static int
ioctl_setinformation_fe(void *wl, int cmd, void* buf, unsigned long *input_len)
{
	int error = 0;

	if (remote_type == NO_REMOTE) {
		error = wl_ioctl(wl,  cmd, buf, *input_len, TRUE);
	} else {
		error = rwl_setinformation_fe(wl, cmd, buf,
			input_len, 0, REMOTE_SET_IOCTL);
	}

	return error;
}

int
wl_get(void *wl, int cmd, void *buf, int len)
{
	int error = 0;
	unsigned long input_len = len;

	if ((rwl_os_type == WIN32_OS || rwl_os_type == WINVISTA_OS) && remote_type != NO_REMOTE)
		cmd += WL_OID_BASE;
	error = (int)ioctl_queryinformation_fe(wl, cmd, buf, &input_len);

	if (error == SERIAL_PORT_ERR)
		return SERIAL_PORT_ERR;
	else if (error == BCME_NODEVICE)
		return BCME_NODEVICE;
	else if (error != 0)
		return IOCTL_ERROR;

	return 0;
}

int
wl_set(void *wl, int cmd, void *buf, int len)
{
	int error = 0;
	unsigned long input_len = len;

	if ((rwl_os_type == WIN32_OS || rwl_os_type == WINVISTA_OS) && remote_type != NO_REMOTE)
		cmd += WL_OID_BASE;
	error = (int)ioctl_setinformation_fe(wl, cmd, buf, &input_len);

	if (error == SERIAL_PORT_ERR)
		return SERIAL_PORT_ERR;
	else if (error == BCME_NODEVICE)
		return BCME_NODEVICE;
	else if (error != 0)
		return IOCTL_ERROR;

	return 0;
}

#if defined(WLMSO)
int wl_os_type_get_rwl()
{
	return rwl_os_type;
}

void wl_os_type_set_rwl(int os_type)
{
	rwl_os_type = os_type;
}

int wl_ir_init_rwl(void **irh)
{
	switch (rwl_get_remote_type()) {
	case NO_REMOTE:
	case REMOTE_WIFI: {
		struct ifreq *ifr;
		ifr = malloc(sizeof(struct ifreq));
		if (ifr) {
			memset(ifr, 0, sizeof(ifr));
			wl_find(ifr);
		}
		*irh = ifr;
	}
		break;
	default:
		break;
	}

	return 0;
}

void wl_close_rwl(int remote_type, void *irh)
{
	switch (remote_type) {
	case NO_REMOTE:
	case REMOTE_WIFI:
		free(irh);
		break;
	default:
		break;
	}
}

#define LINUX_NUM_ARGS  16

static int
buf_to_args(char *tmp, char *new_args[])
{
	char line[INTERACTIVE_MAX_INPUT_LENGTH];
	char *token;
	int argc = 0;

	if (strlen(tmp) >= INTERACTIVE_MAX_INPUT_LENGTH) {
		printf("wl:error: Input string too long; must be < %d bytes\n",
		       INTERACTIVE_MAX_INPUT_LENGTH);
		return 0;
	}

	strcpy(line, tmp);
	while  (argc < (LINUX_NUM_ARGS - 1) &&
		(token = strtok(argc ? NULL : line, " \t")) != NULL) {
		new_args[argc] = malloc(strlen(token)+1);
		strncpy(new_args[argc], token, strlen(token)+1);
		argc++;
	}
	new_args[argc] = NULL;
	if (argc == (LINUX_NUM_ARGS - 1) && (token = strtok(NULL, " \t")) != NULL) {
		printf("wl:error: too many args; argc must be < %d\n",
		       (LINUX_NUM_ARGS - 1));
		argc = LINUX_NUM_ARGS;
	}
	return argc;
}

int
wl_lib(char *input_str)
{
	struct ifreq ifr;
	char *ifname = NULL;
	int err = 0;
	int help = 0;
	int status = CMD_WL;
	void* serialHandle = NULL;
	char *tmp_argv[LINUX_NUM_ARGS];
	char **argv = tmp_argv;
	int argc;

	/* buf_to_args return 0 if no args or string too long
	 * or return NDIS_NUM_ARGS if too many args
	 */
	if (((argc = buf_to_args(input_str, argv)) == 0) || (argc == LINUX_NUM_ARGS)) {
		printf("wl:error: can't convert input string\n");
		return (-1);
	}
#else
/* Main client function */
int
main(int argc, char **argv)
{
	struct ifreq ifr;
	char *ifname = NULL;
	int err = 0;
	int help = 0;
	int status = CMD_WL;
#if defined (RWL_DONGLE) || (RWL_SERIAL)
	void* serialHandle = NULL;
#endif

#endif /* WLMSO */
	wlu_av0 = argv[0];

	wlu_init();
	memset(&ifr, 0, sizeof(ifr));
	(void)*argv++;

	if ((status = wl_option(&argv, &ifname, &help)) == CMD_OPT) {
		if (ifname)
			strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
		/* Bug fix: If -h is used as an option, the above function call
		 * will notice it and raise the flag but it won't be processed
		 * in this function so we undo the argv increment so that the -h
		 * can be spotted by the next call of wl_option. This will ensure
		 * that wl -h [cmd] will function as desired.
		 */
		else if (help)
			(void)*argv--;
	}

	/* Linux client looking for a indongle reflector */
	if (*argv && strncmp (*argv, "--indongle", strlen(*argv)) == 0) {
		rwl_dut_autodetect = FALSE;
		(void)*argv++;
	}
	/* Linux client looking for a WinVista server */
	if (*argv && strncmp (*argv, "--vista", strlen(*argv)) == 0) {
		rwl_os_type = WINVISTA_OS;
		rwl_dut_autodetect = FALSE;
		(void)*argv++;
	}

	/* Provide option for disabling remote DUT autodetect */
	if (*argv && strncmp(*argv, "--nodetect", strlen(*argv)) == 0) {
		rwl_dut_autodetect = FALSE;
		argv++;
	}

	if (*argv && strncmp (*argv, "--debug", strlen(*argv)) == 0) {
		debug = TRUE;
		argv++;
	}

	/* RWL socket transport Usage: --socket ipaddr/hostname [port num] */
	if (*argv && strncmp (*argv, "--socket", strlen(*argv)) == 0) {
		(void)*argv++;

		remote_type = REMOTE_SOCKET;

		if (!(*argv)) {
			rwl_usage(remote_type);
			return err;
		}
		/* IP address validation is done in client_shared file */
		g_rwl_servIP = *argv;
		(void)*argv++;

		g_rwl_servport = DEFAULT_SERVER_PORT;
		if ((*argv) && isdigit(**argv)) {
			g_rwl_servport = atoi(*argv);
			(void)*argv++;
		}
	}

	/* RWL from system serial port on client to uart serial port on server */
	/* Usage: --serial /dev/ttyS0 */
	if (*argv && strncmp (*argv, "--serial", strlen(*argv)) == 0) {
		(void)*argv++;
		remote_type = REMOTE_SERIAL;
	}

	/* RWL from system serial port on client to uart dongle port on server */
	/* Usage: --dongle /dev/ttyS0 */
	if (*argv && strncmp (*argv, "--dongle", strlen(*argv)) == 0) {
		(void)*argv++;
		remote_type = REMOTE_DONGLE;
	}

#if defined (RWL_SERIAL) || defined (RWL_DONGLE)
	if (remote_type == REMOTE_SERIAL || remote_type == REMOTE_DONGLE) {
		if (!(*argv)) {
			rwl_usage(remote_type);
			return err;
		}
		g_rwl_device_name_serial = *argv;
		(void)*argv++;
		if ((serialHandle = rwl_open_pipe(remote_type, g_rwl_device_name_serial, 0, 0))
			 == NULL) {
			DPRINT_ERR(ERR, "serial device open error\r\n");
			return -1;
		}
		ifr = (*(struct ifreq *)serialHandle);
	}
#endif /*  RWL_SERIAL */

	/* RWL over wifi.  Usage: --wifi mac_address */
	if (*argv && strncmp (*argv, "--wifi", strlen(*argv)) == 0) {
		(void)*argv++;
		/* use default interface */
		if (!*ifr.ifr_name)
			wl_find(&ifr);

		/* validate the interface */
		if (!*ifr.ifr_name || (err = wl_check((void *)&ifr)) < 0) {
			fprintf(stderr, "%s: wl driver adapter not found\n", wlu_av0);
			exit(1);
		}

		remote_type = REMOTE_WIFI;

		if (argc < 4) {
			rwl_usage(remote_type);
			return err;
		}
		/* copy server mac address to local buffer for later use by findserver cmd */
		if (!wl_ether_atoe(*argv, (struct ether_addr *)g_rwl_buf_mac)) {
			fprintf(stderr,
			"could not parse as an ethternet MAC address\n");
			return FAIL;
		}
		(void)*argv++;
	}

	if ((*argv) && (strlen(*argv) > 2) &&
		(strncmp(*argv, "--interactive", strlen(*argv)) == 0)) {
		interactive_flag = 1;
	}

	/* Process for local wl */
	if (remote_type == NO_REMOTE) {
		if (interactive_flag == 1)
			(void)*argv--;
		err = process_args(&ifr, argv);
		return err;
	} else {
#ifndef OLYMPIC_RWL
		/* Autodetect remote DUT */
		if (rwl_dut_autodetect == TRUE)
			rwl_detect((void*)&ifr, debug, &rwl_os_type);
#endif /* OLYMPIC_RWL */
	}

	/* RWL client needs to initialize ioctl_version */
	if (wl_check((void *)&ifr) != 0) {
		fprintf(stderr, "%s: wl driver adapter not found\n", wlu_av0);
			exit(1);
	}

	if (interactive_flag == 1) {
		err = do_interactive(&ifr);
		return err;
	}

	if ((*argv) && (interactive_flag == 0)) {
		err = process_args(&ifr, argv);
		if ((err == SERIAL_PORT_ERR) && (remote_type == REMOTE_DONGLE)) {
			DPRINT_ERR(ERR, "\n Retry again\n");
			err = process_args((struct ifreq*)&ifr, argv);
		}
		return err;
	}
	rwl_usage(remote_type);
#if defined (RWL_DONGLE) || (RWL_SERIAL)
	if (remote_type == REMOTE_DONGLE || remote_type == REMOTE_SERIAL)
		rwl_close_pipe(remote_type, (void*)&ifr);
#endif /* RWL_DONGLE || RWL_SERIAL */
	return err;
}

/*
 * Function called for  'local' execution and for 'remote' non-interactive session
 * (shell cmd, wl cmd)
 */
int
process_args(struct ifreq* ifr, char **argv)
{
	char *ifname = NULL;
	int help = 0;
	int status = 0;
	int vista_cmd_index;
	int err = 0;
	cmd_t *cmd = NULL;
#ifdef RWL_WIFI
	int retry;
#endif

	while (*argv) {
		if ((strcmp (*argv, "sh") == 0) && (remote_type != NO_REMOTE)) {
			(void)*argv++; /* Get the shell command */
			if (*argv) {
				/* Register handler in case of shell command only */
				err = rwl_shell_cmd_proc((void*)ifr, argv, SHELL_CMD);
			} else {
				DPRINT_ERR(ERR, "Enter the shell "
				           "command, e.g. ls(Linux) or dir(Win CE)\n");
				err = -1;
			}
			return err;
		}

#ifdef RWLASD
		if ((strcmp (*argv, "asd") == 0) && (remote_type != NO_REMOTE)) {
			(void)*argv++; /* Get the asd command */
			if (*argv) {
				err = rwl_shell_cmd_proc((void*)ifr, argv, ASD_CMD);
			} else {
				DPRINT_ERR(ERR, "Enter the ASD command, e.g. ca_get_version\n");
				err = -1;
			}
			return err;
		}
#endif
		if (rwl_os_type == WINVISTA_OS) {
			for (vista_cmd_index = 0; remote_vista_cmds[vista_cmd_index] &&
				strcmp(remote_vista_cmds[vista_cmd_index], *argv);
				vista_cmd_index++);
			if (remote_vista_cmds[vista_cmd_index] != NULL) {
				err = rwl_shell_cmd_proc((void *)ifr, argv, VISTA_CMD);
				if ((remote_type == REMOTE_WIFI) && ((!strcmp(*argv, "join")))) {
#ifdef RWL_WIFI
					DPRINT_INFO(OUTPUT,
						"\nChannel will be synchronized by Findserver\n\n");
					sleep(RWL_WIFI_JOIN_DELAY);
					for (retry = 0; retry < RWL_WIFI_RETRY; retry++) {
						if ((rwl_find_remote_wifi_server(ifr,
							&g_rwl_buf_mac[0]) == 0)) {
						break;
					}
				}
#endif /* RWL_WIFI */
			}
			return err;
			}
		}

		if ((status = wl_option(&argv, &ifname, &help)) == CMD_OPT) {
			if (help)
				break;
			if (ifname) {
				if (remote_type == NO_REMOTE) {
					strncpy((*ifr).ifr_name, ifname, IFNAMSIZ);
				}
				else {
					strncpy(g_rem_ifname, ifname, IFNAMSIZ);
				}
			}
			continue;
		}
		/* parse error */
		else if (status == CMD_ERR)
			break;

		if (remote_type == NO_REMOTE) {
			/* use default interface */
			if (!*(*ifr).ifr_name)
				wl_find(ifr);

			/* validate the interface */
			if (!*(*ifr).ifr_name || (err = wl_check((void *)ifr)) < 0) {
				fprintf(stderr, "%s: wl driver adapter not found\n", wlu_av0);
				exit(1);
			}

			if ((strcmp (*argv, "--interactive") == 0) || (interactive_flag == 1)) {
				err = do_interactive(ifr);
				return err;
			}
		 }
		/* search for command */
		cmd = wl_find_cmd(*argv);
		/* if not found, use default set_var and get_var commands */
		if (!cmd) {
			cmd = &wl_varcmd;
		}
#ifdef RWL_WIFI
		if (!strcmp(cmd->name, "findserver")) {
			remote_wifi_ser_init_cmds((void *) ifr);
		}
#endif /* RWL_WIFI */

		/* RWL over Wifi supports 'lchannel' command which lets client
		 * (ie *this* machine) change channels since normal 'channel' command
		 * applies to the server (ie target machine)
		 */
		if (remote_type == REMOTE_WIFI)	{
#ifdef RWL_WIFI
			if (!strcmp(argv[0], "lchannel")) {
				strcpy(argv[0], "channel");
				rwl_wifi_swap_remote_type(remote_type);
				err = (*cmd->func)((void *) ifr, cmd, argv);
				rwl_wifi_swap_remote_type(remote_type);
			} else {
				err = (*cmd->func)((void *) ifr, cmd, argv);
			}
			/* After join cmd's gets exeuted on the server side , client needs to know
			* the channel on which the server is associated with AP , after delay of
			* few seconds client will intiate the scan on diffrent channels by calling
			* rwl_find_remote_wifi_server fucntion
			*/
			if ((!strcmp(cmd->name, "join") || ((!strcmp(cmd->name, "ssid") &&
				(*(++argv) != NULL))))) {
				DPRINT_INFO(OUTPUT, "\n Findserver is called to synchronize the"
				"channel\n\n");
				sleep(RWL_WIFI_JOIN_DELAY);
				for (retry = 0; retry < RWL_WIFI_RETRY; retry++) {
					if ((rwl_find_remote_wifi_server(ifr,
					&g_rwl_buf_mac[0]) == 0)) {
						break;
					}
				}
			}
#endif /* RWL_WIFI */
		} else {
			/* do command */
			err = (*cmd->func)((void *) ifr, cmd, argv);
		}
		break;
	} /* while loop end */

/* provide for help on a particular command */
	if (help && *argv) {
		cmd = wl_find_cmd(*argv);
		if (cmd) {
			wl_cmd_usage(stdout, cmd);
		} else {
			DPRINT_ERR(ERR, "%s: Unrecognized command \"%s\", type -h for help\n",
			                                                          wlu_av0, *argv);
		}
	} else if (!cmd)
		wl_usage(stdout, NULL);
	else if (err == USAGE_ERROR)
		wl_cmd_usage(stderr, cmd);
	else if (err == IOCTL_ERROR)
		wl_printlasterror((void *) ifr);
	else if (err == BCME_NODEVICE)
		DPRINT_ERR(ERR, "%s : wl driver adapter not found\n", g_rem_ifname);

	return err;
}

/* Function called for 'local' interactive session and for 'remote' interactive session */
static int
do_interactive(struct ifreq *ifr)
{
	int err = 0;

#ifdef RWL_WIFI
	int retry;
#endif

	while (1) {
		char *fgsret;
		char line[INTERACTIVE_MAX_INPUT_LENGTH];
		fprintf(stdout, "> ");
		fgsret = fgets(line, sizeof(line), stdin);

		/* end of file */
		if (fgsret == NULL)
			break;
		if (line[0] == '\n')
			continue;

		if (strlen (line) > 0) {
			/* skip past first arg if it's "wl" and parse up arguments */
			char *argv[INTERACTIVE_NUM_ARGS];
			int argc;
			char *token;
			argc = 0;

			while (argc < (INTERACTIVE_NUM_ARGS - 1) &&
			       (token = strtok(argc ? NULL : line, " \t\n")) != NULL) {

				/* Specifically make sure empty arguments (like SSID) are empty */
				if (token[0] == '"' && token[1] == '"') {
				    token[0] = '\0';
				}

				argv[argc++] = token;
			}
			argv[argc] = NULL;
			if (argc == (INTERACTIVE_NUM_ARGS - 1) &&
			    (token = strtok(NULL, " \t")) != NULL) {
				printf("wl:error: too many args; argc must be < %d\n",
				       (INTERACTIVE_NUM_ARGS - 1));
				continue;
			}
#ifdef RWL_WIFI
		if (!strcmp(argv[0], "findserver")) {
			remote_wifi_ser_init_cmds((void *) ifr);
		}
#endif /* RWL_WIFI */

			if (strcmp(argv[0], "q") == 0 || strcmp(argv[0], "exit") == 0) {
				break;
			}

			if ((strcmp(argv[0], "sh") == 0) && (remote_type != NO_REMOTE))  {
				if (argv[1]) {
					process_args(ifr, argv);
				} else {
					DPRINT_ERR(ERR, "Give shell command");
					continue;
				}
			} else { /* end shell */
				/* check for lchannel support,applicable only for wifi transport.
				* when lchannel is called remote type is swapped by calling swap_
				* remote_type.This is done to change, the remote type to local,
				* so that local machine's channel can be executed and
				* returned to the user.
				* To get back the original remote type, swap is recalled.
				*/
				if (remote_type == REMOTE_WIFI) {
#ifdef RWL_WIFI
					if (!strcmp(argv[0], "lchannel")) {
						strcpy(argv[0], "channel");
						rwl_wifi_swap_remote_type(remote_type);
						err = wl_do_cmd(ifr, argv);
						rwl_wifi_swap_remote_type(remote_type);
					} else {
						err = wl_do_cmd(ifr, argv);
					}
				/* After join cmd's gets exeuted on the server side, client
				 * needs to know the channel on which the server is associated
				 * with AP , after delay of few seconds client will intiate the
				 * scan on diffrent channels by calling
				 * rwl_find_remote_wifi_server function
				 */
					if ((!strcmp(argv[0], "join")) ||
						(!strcmp(argv[0], "ssid"))) {
						DPRINT_INFO(OUTPUT, "\n Findserver is called"
						"after the join issued to remote \n \n");
						sleep(RWL_WIFI_JOIN_DELAY);
						for (retry = 0; retry < RWL_WIFI_RETRY; retry++) {
							if ((rwl_find_remote_wifi_server(ifr,
							&g_rwl_buf_mac[0]) == 0)) {
								break;
							}
						}
					}
#endif /* RWL_WIFI */
				} else {
					err = wl_do_cmd(ifr, argv);
				}
			} /* end of wl */
		} /* end of strlen (line) > 0 */
	} /* while (1) */

	return err;
}

/*
 * find command in argv and execute it
 * Won't handle changing ifname yet, expects that to happen with the --interactive
 * Return an error if unable to find/execute command
 */
static int
wl_do_cmd(struct ifreq *ifr, char **argv)
{
	cmd_t *cmd = NULL;
	int err = 0;
	int help = 0;
	char *ifname = NULL;
	int status = CMD_WL;

	/* skip over 'wl' if it's there */
	if (*argv && strcmp (*argv, "wl") == 0) {
		argv++;
	}

	/* handle help or interface name changes */
	if (*argv && (status = wl_option (&argv, &ifname, &help)) == CMD_OPT) {
		if (ifname) {
			fprintf(stderr,
			        "Interface name change not allowed within --interactive\n");
		}
	}

	/* in case wl_option eats all the args */
	if (!*argv) {
		return err;
	}

	if (status != CMD_ERR) {
		/* search for command */
		cmd = wl_find_cmd(*argv);

		/* defaults to using the set_var and get_var commands */
		if (!cmd) {
			cmd = &wl_varcmd;
		}
		/* do command */
		err = (*cmd->func)((void *)ifr, cmd, argv);
	}
	/* provide for help on a particular command */
	if (help && *argv) {
	  cmd = wl_find_cmd(*argv);
	 if (cmd) {
		wl_cmd_usage(stdout, cmd);
	} else {
			DPRINT_ERR(ERR, "%s: Unrecognized command \"%s\", type -h for help\n",
			       wlu_av0, *argv);
	       }
	} else if (!cmd)
		wl_usage(stdout, NULL);
	else if (err == USAGE_ERROR)
		wl_cmd_usage(stderr, cmd);
	else if (err == IOCTL_ERROR)
		wl_printlasterror((void *)ifr);
	else if (err == BCME_NODEVICE)
		DPRINT_ERR(ERR, "%s : wl driver adapter not found\n", g_rem_ifname);

	return err;
}

/* Search the wl_cmds table for a matching command name.
 * Return the matching command or NULL if no match found.
 */
static cmd_t *
wl_find_cmd(char* name)
{
	cmd_t *cmd = NULL;

	/* search the wl_cmds for a matching name */
	for (cmd = wl_cmds; cmd->name && strcmp(cmd->name, name); cmd++);

	if (cmd->name == NULL)
		cmd = NULL;

	return cmd;
}

void def_handler(int signum)
{
	UNUSED_PARAMETER(signum);
	kill(g_child_pid, SIGINT);
	kill(getpid(), SIGINT);
	exit(0);
}
/* Create a child that waits for Ctrl-C at the client side
 */
int rwl_shell_createproc(void *wl)
{
	UNUSED_PARAMETER(wl);
	signal(SIGINT, ctrlc_handler);
	signal(SIGTERM, def_handler);
	signal(SIGABRT, def_handler);
	return fork();
}

/* In case if the server shell command exits normally
 * then kill the thread that was waiting for Ctr-C to happen
 * at the client side
 */
void rwl_shell_killproc(int pid)
{
	kill(pid, SIGKILL);
	signal(SIGINT, SIG_DFL);
	wait(NULL);
}
#ifdef RWL_SOCKET
/* to validate hostname/ip given by the client */
int validate_server_address()
{
	struct hostent *he;
	struct ipv4_addr temp;
	if (!wl_atoip(g_rwl_servIP, &temp)) {
	/* Wrong IP address format check for hostname */
		if ((he = gethostbyname(g_rwl_servIP)) != NULL) {
			if (!wl_atoip(*he->h_addr_list, &temp)) {
				g_rwl_servIP =
				inet_ntoa(*(struct in_addr *)*he->h_addr_list);
				if (g_rwl_servIP == NULL) {
					DPRINT_ERR(ERR, "Error at inet_ntoa \n");
					return FAIL;
				}
			} else {
				DPRINT_ERR(ERR, "Error in IP address \n");
				return FAIL;
			}
		} else {
			DPRINT_ERR(ERR, "Enter correct IP address/hostname format\n");
			return FAIL;
		}
	}
	return SUCCESS;
}
#endif /* RWL_SOCKET */
