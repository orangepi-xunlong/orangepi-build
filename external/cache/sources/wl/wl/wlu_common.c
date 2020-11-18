/*
 * Common code for wl routines
 *
 * $Copyright (C) 2002-2005 Broadcom Corporation$
 *
 * $Id: wlu_common.c 288685 2011-10-08 00:50:34Z pgarg $
 */

#ifdef WIN32
#include <windows.h>
#endif

#include "wlu_common.h"
#include "wlu.h"
#include <bcmendian.h>

extern int wl_get(void *wl, int cmd, void *buf, int len);
extern int wl_set(void *wl, int cmd, void *buf, int len);

wl_cmd_list_t cmd_list;
int cmd_pkt_list_num;
bool cmd_batching_mode;

const char *wlu_av0;

#ifdef SERDOWNLOAD
extern int debug;
#endif

#ifdef ATE_BUILD
int wlu_iovar_get(void *wl, const char *iovar, void *outbuf, int len);
int wlu_get(void *wl, int cmd, void *cmdbuf, int len);
int wlu_set(void *wl, int cmd, void *cmdbuf, int len);
int add_one_batched_cmd(int cmd, void *cmdbuf, int len);
int wlu_iovar_setint(void *wl, const char *iovar, int val);
int wlu_iovar_set(void *wl, const char *iovar, void *param, int paramlen);
#endif /* ATE_BUILD */

/*
 * format an iovar buffer
 * iovar name is converted to lower case
 */
static uint
wl_iovar_mkbuf(const char *name, char *data, uint datalen, char *iovar_buf, uint buflen, int *perr)
{
	uint iovar_len;
	char *p;

	iovar_len = strlen(name) + 1;

	/* check for overflow */
	if ((iovar_len + datalen) > buflen) {
		*perr = BCME_BUFTOOSHORT;
		return 0;
	}

	/* copy data to the buffer past the end of the iovar name string */
	if (datalen > 0)
		memmove(&iovar_buf[iovar_len], data, datalen);

	/* copy the name to the beginning of the buffer */
	strcpy(iovar_buf, name);

	/* wl command line automatically converts iovar names to lower case for
	 * ease of use
	 */
	p = iovar_buf;
	while (*p != '\0') {
		*p = tolower((int)*p);
		p++;
	}

	*perr = 0;
	return (iovar_len + datalen);
}

void
init_cmd_batchingmode(void)
{
	cmd_pkt_list_num = 0;
	cmd_batching_mode = FALSE;
}

void
clean_up_cmd_list(void)
{
	wl_seq_cmd_pkt_t *this_cmd, *next_cmd;

	this_cmd = cmd_list.head;
	while (this_cmd != NULL) {
		next_cmd = this_cmd->next;
		if (this_cmd->data != NULL) {
			free(this_cmd->data);
		}
		free(this_cmd);
		this_cmd = next_cmd;
	}
	cmd_list.head = NULL;
	cmd_list.tail = NULL;
	cmd_pkt_list_num = 0;
}

int
add_one_batched_cmd(int cmd, void *cmdbuf, int len)
{
	wl_seq_cmd_pkt_t *new_cmd;

	new_cmd = malloc(sizeof(wl_seq_cmd_pkt_t));

	if (new_cmd == NULL) {
		printf("malloc(%d) failed, free %d batched commands and exit batching mode\n",
			(int)sizeof(wl_seq_cmd_pkt_t), cmd_pkt_list_num);
		goto free_and_exit;
	} else {
#ifdef SERDOWNLOAD
		if (debug)
#endif /* SERDOWNLOAD */
			printf("batching %dth command %d\n", cmd_pkt_list_num+1, cmd);

	}

	new_cmd->cmd_header.cmd = cmd;
	new_cmd->cmd_header.len = len;
	new_cmd->next  = NULL;

	new_cmd->data = malloc(len);

	if (new_cmd->data == NULL) {
		printf("malloc(%d) failed, free %d batched commands and exit batching mode\n",
			len, cmd_pkt_list_num);
		free(new_cmd);
		goto free_and_exit;
	}

	memcpy(new_cmd->data, cmdbuf, len);

	if (cmd_list.tail != NULL)
		cmd_list.tail->next = new_cmd;
	else
		cmd_list.head = new_cmd;

	cmd_list.tail = new_cmd;

	cmd_pkt_list_num ++;
	return 0;

free_and_exit:

	clean_up_cmd_list();

	if (cmd_batching_mode) {
		cmd_batching_mode = FALSE;
	}
	else {
		printf("calling add_one_batched_cmd() at non-command-batching mode, weird\n");
	}

	return -1;
}

#ifndef ATE_BUILD
int
wlu_get_req_buflen(int cmd, void *cmdbuf, int len)
{
	int modified_len = len;
	char *cmdstr = (char *)cmdbuf;

	if (len == WLC_IOCTL_MAXLEN) {
		if ((strcmp(cmdstr, "dump") == 0) ||
			(cmd == WLC_SCAN_RESULTS))
			modified_len = WLC_IOCTL_MAXLEN;
		else
			modified_len = WLC_IOCTL_MEDLEN;
	}
	return modified_len;
}
#endif /* !ATE_BUILD */

/* now IOCTL GET commands shall call wlu_get() instead of wl_get() so that the commands
 * can be batched when needed
 */
int
wlu_get(void *wl, int cmd, void *cmdbuf, int len)
{
	if (cmd_batching_mode) {
		if (!WL_SEQ_CMDS_GET_IOCTL_FILTER(cmd)) {
			printf("IOCTL GET command %d is not supported in batching mode\n", cmd);
			return IOCTL_ERROR;
		}
	}

	return wl_get(wl, cmd, cmdbuf, len);
}

/* now IOCTL SET commands shall call wlu_set() instead of wl_set() so that the commands
 * can be batched when needed
 */
int
wlu_set(void *wl, int cmd, void *cmdbuf, int len)
{
	if (cmd_batching_mode) {
		return add_one_batched_cmd(cmd, cmdbuf, len);
	}
	else {
		return wl_set(wl, cmd, cmdbuf, len);
	}
}

/*
 * get named iovar providing both parameter and i/o buffers
 * iovar name is converted to lower case
 */
int
wlu_iovar_getbuf(void* wl, const char *iovar,
	void *param, int paramlen, void *bufptr, int buflen)
{
	int err;

	wl_iovar_mkbuf(iovar, param, paramlen, bufptr, buflen, &err);
	if (err)
		return err;

	return wlu_get(wl, WLC_GET_VAR, bufptr, buflen);
}

/*
 * set named iovar providing both parameter and i/o buffers
 * iovar name is converted to lower case
 */
int
wlu_iovar_setbuf(void* wl, const char *iovar,
	void *param, int paramlen, void *bufptr, int buflen)
{
	int err;
	int iolen;

	iolen = wl_iovar_mkbuf(iovar, param, paramlen, bufptr, buflen, &err);
	if (err)
		return err;

	return wlu_set(wl, WLC_SET_VAR, bufptr, iolen);
}

/*
 * get named iovar without parameters into a given buffer
 * iovar name is converted to lower case
 */
int
wlu_iovar_get(void *wl, const char *iovar, void *outbuf, int len)
{
	char smbuf[WLC_IOCTL_SMLEN];
	int err;

	/* use the return buffer if it is bigger than what we have on the stack */
	if (len > (int)sizeof(smbuf)) {
		err = wlu_iovar_getbuf(wl, iovar, NULL, 0, outbuf, len);
	} else {
		memset(smbuf, 0, sizeof(smbuf));
		err = wlu_iovar_getbuf(wl, iovar, NULL, 0, smbuf, sizeof(smbuf));
		if (err == 0)
			memcpy(outbuf, smbuf, len);
	}

	return err;
}

/*
 * set named iovar given the parameter buffer
 * iovar name is converted to lower case
 */
int
wlu_iovar_set(void *wl, const char *iovar, void *param, int paramlen)
{
	char smbuf[WLC_IOCTL_SMLEN*2];

	memset(smbuf, 0, sizeof(smbuf));

	return wlu_iovar_setbuf(wl, iovar, param, paramlen, smbuf, sizeof(smbuf));
}

/*
 * get named iovar as an integer value
 * iovar name is converted to lower case
 */
int
wlu_iovar_getint(void *wl, const char *iovar, int *pval)
{
	int ret;

	ret = wlu_iovar_get(wl, iovar, pval, sizeof(int));
	if (ret >= 0)
	{
		*pval = dtoh32(*pval);
	}
	return ret;
}

/*
 * set named iovar given an integer parameter
 * iovar name is converted to lower case
 */
int
wlu_iovar_setint(void *wl, const char *iovar, int val)
{
	val = htod32(val);
	return wlu_iovar_set(wl, iovar, &val, sizeof(int));
}
