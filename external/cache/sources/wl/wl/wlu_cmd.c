/*
 * $Id: wlu_cmd.c 399026 2013-04-27 00:44:07Z yongfang $
 * $Copyright: (c) 2006, Broadcom Corp.
 * All Rights Reserved.$
 *
 * File:        wlu_cmd.c
 * Purpose:     Command and IO variable information commands
 * Requires:
 */

#if	defined(_CFE_)
#include <lib_types.h>
#include <lib_string.h>
#include <lib_printf.h>
#include <lib_malloc.h>
#include <cfe_error.h>
#elif	defined(__IOPOS__)
#include <typedefs.h>
#include <bcmstdlib.h>
#include <bcmutils.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#endif /* defined(_CFE_) */

#include <typedefs.h>
#include <bcmutils.h>
#include <wlioctl.h>

#include "wlu.h"

#if	defined(__IOPOS__)
#define fprintf(file, fmt, arg...) jtag_printf(fmt , ## arg)
#endif /* __IOPOS__ */

#ifdef BCMINTERNAL

#ifndef NUM_ELS
#define NUM_ELS(_a) (sizeof(_a)/sizeof(_a[0]))
#endif /* num els */

typedef struct cmd2cat {
	const char *name;
	uint32 cat;
} cmd2cat_t;

/* Command categorization:
 *
 * Please keep these alphabetized (ascii ordering).
 */
cmd2cat_t cmd2cat[] = {
	/* Please keep alphabetized */
	{"PM", CMD_PHY + CMD_POWER},
	{"a_mrate", CMD_PHY + CMD_RATE},
	{"a_rate", CMD_PHY + CMD_RATE},
	{"abminrate", CMD_MGMT + CMD_AP + CMD_STA},
	{"aciargs", CMD_PHY},
	{"add_ie", CMD_MGMT},
	{"addkey", CMD_MGMT + CMD_SEC},
	{"addwep", CMD_MGMT + CMD_SEC},
	{"ampdu_clear_dump", CMD_MON + CMD_ADMIN},
	{"ampdu_dump", CMD_DEP + CMD_UNCAT},
	{"ampdu_retry_limit_tid", CMD_UNCAT},
	{"ampdu_rr_retry_limit_tid", CMD_UNCAT},
	{"ampdu_send_addba", CMD_MGMT},
	{"ampdu_send_delba", CMD_MGMT},
	{"ampdu_tid", CMD_MGMT},
	{"amsdu_clear_counters", CMD_MON + CMD_ADMIN},
	{"amsdu_dump", CMD_DEP + CMD_UNCAT},
	{"antdiv", CMD_PHY},
	{"ap", CMD_AP + CMD_STA},
	{"ap_isolate", CMD_MGMT + CMD_AP},
	{"apname", CMD_MGMT + CMD_MON + CMD_AP},
	{"assoc", CMD_MON + CMD_STA + CMD_AP},
	{"assoc_info", CMD_MGMT + CMD_STA},
	{"assoc_pref", CMD_MGMT + CMD_AP + CMD_STA},
	{"assoclist", CMD_MGMT + CMD_AP},
	{"atten", CMD_PHY + CMD_CHAN + CMD_POWER},
	{"auth", CMD_MGMT + CMD_SEC},
	{"authe_sta_list", CMD_MGMT + CMD_SEC},
	{"autho_sta_list", CMD_MGMT + CMD_SEC},
	{"authorize", CMD_MGMT + CMD_SEC},
	{"autochannel", CMD_PHY + CMD_CHAN},
	{"ba_clear_counters", CMD_MON + CMD_ADMIN},
	{"ba_dump", CMD_DEP + CMD_UNCAT},
	{"band", CMD_PHY + CMD_CHAN},
	{"bands", CMD_PHY + CMD_CHAN},
	{"band_range", CMD_PHY + CMD_CHAN},
	{"bcmerrorstr", CMD_ADMIN},
	{"bg_mrate", CMD_PHY + CMD_RATE},
	{"bg_rate", CMD_PHY + CMD_RATE},
	{"bi", CMD_MGMT + CMD_AP},
	{"bss", CMD_MGMT + CMD_AP + CMD_STA},
	{"bssid", CMD_AP},
	{"bssmax", CMD_AP + CMD_ADMIN},
	{"btc_flags", CMD_DEV},
	{"btc_params", CMD_DEV},
	{"cac_addts", CMD_MGMT + CMD_WME + CMD_AP + CMD_STA},
	{"cac_delts", CMD_MGMT + CMD_WME + CMD_AP + CMD_STA},
	{"cac_delts_ea", CMD_MGMT + CMD_WME + CMD_AP + CMD_STA},
	{"cac_tslist", CMD_MGMT + CMD_WME + CMD_AP + CMD_STA},
	{"cac_tspec", CMD_MGMT + CMD_WME + CMD_AP + CMD_STA},
	{"cap", CMD_ADMIN},
	{"cck_txbw", CMD_PHY + CMD_CHAN + CMD_UNCAT},
	{"chan_info", CMD_PHY + CMD_CHAN + CMD_MON},
	{"chanlist", CMD_DEP + CMD_UNCAT},
	{"channel", CMD_PHY + CMD_CHAN},
	{"channel_qa", CMD_PHY + CMD_CHAN},
	{"channel_qa_start", CMD_PHY + CMD_CHAN},
	{"channels", CMD_PHY + CMD_CHAN},
	{"channels_in_country", CMD_PHY + CMD_CHAN},
	{"chanspec", CMD_PHY + CMD_CHAN},
	{"chanspecs", CMD_PHY + CMD_CHAN},
	{"clk", CMD_BOARD},
	{"closed", CMD_SEC},
	{"closednet", CMD_SEC},
	{"cmds", CMD_ADMIN},
	{"config", CMD_MGMT},
	{"constraint", CMD_PHY + CMD_POWER},
	{"counters", CMD_MON + CMD_ADMIN},
	{"country", CMD_MGMT},
	{"crsuprs", CMD_PHY + CMD_CHAN},
	{"csa", CMD_PHY + CMD_CHAN + CMD_MGMT},
	{"csscantimer", CMD_PHY + CMD_CHAN},
	{"cur_etheraddr", CMD_MGMT},
	{"curpower", CMD_PHY + CMD_POWER},
	{"cwmax", CMD_MGMT},
	{"cwmin", CMD_MGMT},
	{"deauthenticate", CMD_MGMT + CMD_SEC},
	{"deauthorize", CMD_MGMT + CMD_SEC},
	{"decryptstatus", CMD_MGMT + CMD_SEC},
	{"del_ie", CMD_MGMT},
	{"delta_stats", CMD_MON + CMD_ADMIN},
	{"delta_stats_interval", CMD_MON + CMD_ADMIN},
	{"dfs_status", CMD_PHY + CMD_CHAN},
	{"diag", CMD_ADMIN},
	{"disassoc", CMD_MGMT + CMD_STA},
	{"down", CMD_BOARD + CMD_ADMIN},
	{"dtim", CMD_MGMT + CMD_AP},
	{"dump", CMD_ADMIN},
	{"eap", CMD_MGMT + CMD_SEC},
	{"eap_restrict", CMD_SEC},
	{"encryptstrength", CMD_MGMT + CMD_SEC},
	{"event_msgs", CMD_MAC},
	{"eventing", CMD_MAC},
	{"evm", CMD_PHY + CMD_CHAN + CMD_RATE},
	{"fasttimer", CMD_DEP + CMD_UNCAT},
	{"fips", CMD_MGMT + CMD_STA},
	{"force", CMD_PHY | CMD_BOARD},
	{"fqacurcy", CMD_PHY + CMD_CHAN},
	{"frag", CMD_DEP + CMD_UNCAT},
	{"frameburst", CMD_MAC + CMD_MGMT},
	{"freqtrack", CMD_PHY + CMD_CHAN},
	{"glacialtimer", CMD_DEP + CMD_UNCAT},
	{"gmode", CMD_PHY},
	{"gmode_protection", CMD_PHY},
	{"gmode_protection_control", CMD_PHY},
	{"gmode_protection_override", CMD_PHY},
	{"gpiodump", CMD_DEP + CMD_UNCAT},
	{"gpioout", CMD_DEV},
	{"help", CMD_ADMIN},
	{"hwkeys", CMD_SEC + CMD_ADMIN + CMD_DEV},
	{"ignore_bcns", CMD_MGMT},
	{"infra", CMD_MGMT + CMD_AP + CMD_STA},
	{"interference", CMD_PHY},
	{"iov", CMD_ADMIN},
	{"isup", CMD_MGMT + CMD_ADMIN},
	{"join", CMD_MGMT + CMD_STA},
	{"join_pref", CMD_MGMT + CMD_AP + CMD_STA},
	{"keys", CMD_MGMT + CMD_SEC},
	{"lazywds", CMD_MGMT + CMD_SEC},
	{"leap", CMD_MGMT + CMD_SEC},
	{"legacy_erp", CMD_UNCAT},
	{"legacylink", CMD_PHY + CMD_BOARD},
	{"lifetime", CMD_MGMT + CMD_AP + CMD_STA},
	{"list", CMD_ADMIN},
	{"list_ie", CMD_MGMT},
	{"listen", CMD_MGMT},
	{"longtrain", CMD_PHY + CMD_CHAN},
	{"loop", CMD_MAC + CMD_ADMIN + CMD_DEV},
	{"lrl", CMD_MGMT},
	{"mac", CMD_MAC + CMD_MGMT},
	{"macmode", CMD_MAC + CMD_MGMT},
	{"macreg", CMD_MAC + CMD_DEV},
	{"malloc_dump", CMD_DEP + CMD_UNCAT},
	{"measure_req", CMD_PHY + CMD_MON},
	{"mimo_ft", CMD_PHY + CMD_CHAN},
	{"mimo_ofdm", CMD_PHY + CMD_CHAN},
	{"mimo_ps", CMD_PHY + CMD_CHAN + CMD_POWER},
	{"mimo_txbw", CMD_PHY + CMD_CHAN},
	{"mode_reqd", CMD_MGMT},
	{"monitor", CMD_PHY + CMD_MON},
	{"mrate", CMD_DEP + CMD_UNCAT},
	{"msglevel", CMD_ADMIN},
	{"n_bw", CMD_PHY + CMD_CHAN},
	{"ndis_frag", CMD_MGMT},
	{"ndis_rts", CMD_MGMT},
	{"ndisscan", CMD_MGMT + CMD_STA},
	{"nettype", CMD_MGMT + CMD_PHY},
	{"nettypes_supported", CMD_ADMIN + CMD_MON},
	{"noise", CMD_PHY + CMD_MON},
	{"nrate", CMD_PHY + CMD_RATE},
	{"nvget", CMD_ADMIN},
	{"nvotpw", CMD_ADMIN + CMD_DEV},
	{"nvram_dump", CMD_ADMIN},
	{"nvram_get", CMD_ADMIN},
	{"nvset", CMD_ADMIN},
	{"ofdm_txbw", CMD_PHY + CMD_CHAN + CMD_UNCAT},
	{"otpdump", CMD_ADMIN + CMD_DEV},
	{"otpw", CMD_ADMIN + CMD_DEV},
	{"out", CMD_BOARD + CMD_ADMIN},
	{"passive", CMD_PHY + CMD_MGMT + CMD_AP + CMD_STA},
	{"pcicfgreg", CMD_BOARD + CMD_DEV},
	{"pciereg", CMD_BOARD + CMD_DEV},
	{"pcieregdump", CMD_DEP + CMD_UNCAT},
	{"phylist", CMD_PHY},
	{"phyreg", CMD_PHY + CMD_DEV},
	{"phytype", CMD_PHY},
	{"piomode", CMD_ADMIN + CMD_DEV},
	{"pktcnt", CMD_MGMT + CMD_MON},
	{"plcphdr", CMD_UNCAT},
	{"pm2_rcv_dur", CMD_POWER},
	{"pm2_sleep_ret", CMD_POWER},
	{"pmkid_info", CMD_SEC},
	{"powerindex", CMD_PHY + CMD_POWER},
	{"prb_resp_timeout", CMD_PHY + CMD_CHAN + CMD_MGMT + CMD_AP + CMD_STA},
	{"primary_key", CMD_MGMT + CMD_SEC},
	{"promisc", CMD_PHY + CMD_MON},
	{"protection_control", CMD_MGMT + CMD_AP + CMD_STA},
	{"pwr_percent", CMD_PHY + CMD_POWER},
	{"quiet", CMD_PHY},
	{"radar", CMD_PHY},
	{"radarargs", CMD_PHY},
	{"radar_status", CMD_PHY},
	{"clear_radar_status", CMD_PHY},
	{"radio", CMD_PHY + CMD_DEV},
	{"radioreg", CMD_PHY + CMD_DEV},
	{"rand", CMD_ADMIN + CMD_DEV},
	{"rate", CMD_PHY},
	{"ratedump", CMD_PHY + CMD_RATE},
	{"rateparam", CMD_PHY + CMD_RATE},
	{"rateset", CMD_PHY + CMD_RATE},
	{"rcvdropped", CMD_MON},
	{"rcvok", CMD_MON},
	{"reassoc", CMD_MGMT + CMD_STA},
	{"reboot", CMD_BOARD + CMD_ADMIN},
	{"regulatory", CMD_MGMT},
	{"reinit", CMD_ADMIN + CMD_DEV},
	{"reset_d11cnts", CMD_MON + CMD_ADMIN},
	{"restart", CMD_BOARD + CMD_ADMIN},
	{"revinfo", CMD_ADMIN + CMD_DEV},
	{"rm_rep", CMD_PHY + CMD_MON},
	{"rm_req", CMD_PHY + CMD_MON},
	{"rmwep", CMD_MGMT + CMD_SEC},
	{"rndismac", CMD_MGMT},
	{"roam_delta", CMD_MGMT + CMD_STA},
	{"roam_scan_period", CMD_MGMT + CMD_STA},
	{"roam_trigger", CMD_MGMT + CMD_STA},
	{"rssi", CMD_UNCAT},
	{"rssi_event", CMD_UNCAT},
	{"rts", CMD_DEP + CMD_UNCAT},
	{"sample_collect", CMD_PHY},
	{"scan", CMD_PHY + CMD_MGMT + CMD_AP + CMD_STA},
	{"scan_channel_time", CMD_PHY + CMD_CHAN},
	{"scan_home_time", CMD_PHY + CMD_CHAN},
	{"scan_nprobes", CMD_UNCAT},
	{"scan_passive_time", CMD_PHY + CMD_CHAN},
	{"scan_unassoc_time", CMD_PHY + CMD_CHAN},
	{"scanresults", CMD_MON + CMD_AP + CMD_STA},
	{"scansuppress", CMD_PHY},
	{"scb_timeout", CMD_MGMT + CMD_SEC + CMD_AP},
	{"sd_blockmode", CMD_DEV},
	{"sd_blocksize", CMD_DEV},
	{"sd_cis", CMD_DEV},
	{"sd_devreg", CMD_DEV},
	{"sd_divisor", CMD_DEV},
	{"sd_dma", CMD_DEV},
	{"sd_drivestrength", CMD_DEV},
	{"sd_hciregs", CMD_DEV},
	{"sd_highspeed", CMD_DEV},
	{"sd_hostreg", CMD_DEV},
	{"sd_ints", CMD_DEV},
	{"sd_mode", CMD_DEV},
	{"sd_msglevel", CMD_ADMIN},
	{"sd_numints", CMD_DEV},
	{"sd_numlocalints", CMD_DEV},
	{"send", CMD_ADMIN},
	{"serdespllreg", CMD_BOARD + CMD_DEV},
	{"serdesrxreg", CMD_BOARD + CMD_DEV},
	{"serdestxreg", CMD_BOARD + CMD_DEV},
	{"set_pmk", CMD_MGMT + CMD_SEC},
	{"shmem", CMD_DEV},
	{"shmem_dump", CMD_DEP + CMD_UNCAT},
	{"shortslot", CMD_PHY},
	{"shortslot_override", CMD_PHY},
	{"shortslot_restrict", CMD_PHY},
	{"shownetworks", CMD_MGMT + CMD_STA},
	{"slowtimer", CMD_DEP + CMD_UNCAT},
	{"spect", CMD_PHY + CMD_MGMT},
	{"srdump", CMD_ADMIN + CMD_DEV},
	{"srl", CMD_MGMT},
	{"ssid", CMD_MGMT + CMD_AP + CMD_STA},
	{"sta_info", CMD_MON + CMD_STA},
	{"staname", CMD_MGMT + CMD_MON + CMD_STA},
	{"stats", CMD_MON},
	{"status", CMD_MON + CMD_STA + CMD_AP},
	{"suprates", CMD_PHY + CMD_RATE},
	{"tkip_countermeasures", CMD_MGMT + CMD_SEC},
#ifdef TRAFFIC_MGMT
	{"trf_mgmt_config", CMD_ADMIN},
	{"trf_mgmt_filters_add", CMD_ADMIN},
	{"trf_mgmt_filters_remove", CMD_ADMIN},
	{"trf_mgmt_filters_list", CMD_ADMIN},
	{"trf_mgmt_filters_clear", CMD_ADMIN},
	{"trf_mgmt_bandwidth", CMD_ADMIN},
	{"trf_mgmt_flags", CMD_ADMIN},
	{"trf_mgmt_stats", CMD_ADMIN},
	{"trf_mgmt_stats_clear", CMD_ADMIN},
	{"trf_mgmt_shaping_info", CMD_ADMIN},
#endif  /* TRAFFIC_MGMT */
	{"tsc", CMD_MGMT + CMD_SEC + CMD_MON},
	{"tsf_dump", CMD_DEP + CMD_UNCAT},
	{"tssi", CMD_PHY},
	{"txant", CMD_PHY},
	{"txinstpwr", CMD_PHY + CMD_POWER},
	{"txpathpwr", CMD_PHY + CMD_POWER},
	{"txpwr", CMD_DEP + CMD_UNCAT},
	{"txpwr1", CMD_PHY + CMD_POWER},
	{"txpwrlimit", CMD_PHY + CMD_POWER},
	{"ucantdiv", CMD_PHY + CMD_DEV},
	{"ucflags", CMD_DEV},
	{"up", CMD_BOARD + CMD_ADMIN},
	{"upgrade", CMD_ADMIN},
	{"ver", CMD_ADMIN},
	{"wake", CMD_PHY + CMD_POWER},
	{"wds", CMD_MGMT + CMD_SEC},
	{"wds_remote_mac", CMD_MGMT + CMD_SEC},
	{"wds_wpa_role", CMD_MGMT + CMD_SEC},
	{"wds_wpa_role_old", CMD_MGMT + CMD_SEC},
	{"wepdefault", CMD_MGMT + CMD_SEC},
	{"wepstatus", CMD_DEP + CMD_UNCAT},
	{"wet", CMD_MGMT + CMD_SEC},
	{"wme", CMD_WME},
	{"wme_ac", CMD_WME},
	{"wme_apsd", CMD_PHY + CMD_POWER + CMD_WME + CMD_AP},
	{"wme_apsd_sta", CMD_WME},
	{"wme_clear_counters", CMD_WME + CMD_MON + CMD_ADMIN},
	{"wme_counters", CMD_WME + CMD_MON + CMD_ADMIN},
	{"wme_dp", CMD_WME},
	{"wme_tx_params", CMD_WME},
	{"wme_maxbw_params", CMD_WME},
	{"wpa_auth", CMD_MGMT + CMD_SEC},
	{"wpa_cap", CMD_MGMT + CMD_SEC},
	{"wsec", CMD_MGMT + CMD_SEC},
	{"wsec_restrict", CMD_MGMT + CMD_SEC},
	{"wsec_test", CMD_MGMT + CMD_SEC},
	{"xlist", CMD_ADMIN},
	{"xmitok", CMD_MON},
	{"zzzzz", 0} /* Last for binary search */
};

/* Get the command category flags for command name; returns 0 if not found */

static uint32 cmd_cat_get(const char *name)
{
	int low = 0, high = NUM_ELS(cmd2cat) - 1;
	int mid;
	int prev_mid;
	int cmp_val;

	mid = low + high / 2;
	do {
		prev_mid = mid;
		cmp_val = strcmp(name, cmd2cat[mid].name);

		if (cmp_val == 0) return cmd2cat[mid].cat;
		else if (cmp_val > 0) low = mid + 1;
		else high = mid - 1;

		mid = (low + high) / 2;
	} while (prev_mid != mid);

	return 0;
}

/*
 * IO variable information command
 */

/* Consolidation of parsing and state info for iov command */
struct iov_control_info {
	char *substr;   /* sub string to match if non-NULL */
	int match_type; /* Display only the matched type if >= 0 */
	bool show_type; /* Display type */
	bool show_desc; /* Display var desc if avail */
	bool show_mod;  /* Display var modules if known */
	bool show_dflt; /* Display var modules if known */
	bool exact_match; /* Match at most one variable exactly to substring */

	bool driver_not_ui;   /* Show variables in driver but not in local info */
	bool ui_not_driver;   /* Show variables in local info but not driver */
	int tot_count;  /* How many variables from driver */
	int out_count;  /* How many displayed */

	void *wl;       /* Need for wl_get calls */

	/* The following are dynmically allocated */
	wlc_iov_trx_t *trx_block;  /* Holding buffer for a block of variables */
	int trx_block_bytes;
	bool *var_found;  /* Mark local vars to detect those not in driver */
	char *mod_names;  /* To be implemented */
};

/* Type string names for matching */
static const char *wl_iov_type_strings[] = BCM_IOV_TYPE_INIT;
static int wl_iov_type_count = ARRAYSIZE(wl_iov_type_strings);

static char wl_iov_mod_names[WLU_MOD_NAME_MAX][WLU_MOD_NAME_BYTES];
/* static bool wl_iov_mod_names_cached = FALSE; */ /* If needed, cache module names */
#define MOD_VALID(mod) (((unsigned int)(mod)) < WLU_MOD_NAME_MAX)

/* Returned when info not found if still printing */
static wlu_iov_info_t iov_unknown_info = {
	NULL,
	0,
	0,
	0,
	"Variable not found in local info"
};

static void
wl_iov_usage(FILE *fid, char *cmd)
{
	int i;

	fprintf(fid, "Usage:  %s [<options>] [-T <type>] [<substr>]\n", cmd);
	fprintf(fid, "    -h Display help message\t\t-i Show description (info)\n");
	fprintf(fid, "    -m Show module name\t\t\t-t Show variable types\n");
	fprintf(fid, "    -a Show all names\t\t\t-d Show default values\n");
	fprintf(fid, "    -e Exact match of substring\n");
	fprintf(fid, "    -v (verbose) Show module, type, default values\n");
	fprintf(fid, "    -V (verbose) Like -v plus descriptions\n");
	fprintf(fid, "    -u (undoc) Show only driver variables without UI info\n");
	fprintf(fid, "    -x Show only variable with UI info that are not in current driver\n");
	fprintf(fid, "    -T <type> Restrict to variables of the given type (see below)\n");
	fprintf(fid, "    <substr> Display all names containing the given substring\n");

	fprintf(fid, "-u and -x are mutually exclusive\n");
	fprintf(fid, "\nKnown variable types include:\n    ");
	for (i = 0; i < wl_iov_type_count; i++) {
		fprintf(fid, "%s ", wl_iov_type_strings[i]);
	}
	fprintf(fid, "\n");
}

/* Parse args and initialize control structure */
static int
iov_parse_args_alloc(FILE *fid, void *wl, char **argv, struct iov_control_info *cinfo)
{
	int idx;
	int i;
	int bytes;

	memset(cinfo, 0, sizeof(*cinfo));
	cinfo->match_type = -1;
	cinfo->wl = wl;

	for (idx = 1; argv[idx] != NULL; idx++) {
		if (strcmp(argv[idx], "-T") == 0) { /* Search for type in list */
			idx++;
			if (argv[idx] == NULL) {
				fprintf(fid, "Must specify type with -t.\n");
				return -1;
			}
			for (i = 0; i < wl_iov_type_count; i++) {
				if (strcmp(argv[idx], wl_iov_type_strings[i]) == 0) {
					cinfo->match_type = i;
					break;
				}
			}
			if (cinfo->match_type < 0) {
				fprintf(fid, "Unknown variable type: %s\n", argv[idx]);
				return -1;
			}
		} else if (strcmp(argv[idx], "-v") == 0) {
			cinfo->show_type = TRUE;
			cinfo->show_mod = TRUE;
			cinfo->show_dflt = TRUE;
		} else if (strcmp(argv[idx], "-V") == 0) {
			cinfo->show_type = TRUE;
			cinfo->show_mod = TRUE;
			cinfo->show_dflt = TRUE;
			cinfo->show_desc = TRUE;
		} else if (strcmp(argv[idx], "-m") == 0) {
			cinfo->show_mod = TRUE;
		} else if (strcmp(argv[idx], "-t") == 0) {
			cinfo->show_type = TRUE;
		} else if (strcmp(argv[idx], "-i") == 0) {
			cinfo->show_desc = TRUE;
		} else if (strcmp(argv[idx], "-d") == 0) {
			cinfo->show_dflt = TRUE;
		} else if (strcmp(argv[idx], "-e") == 0) {
			cinfo->exact_match = TRUE;
		} else if (strcmp(argv[idx], "-u") == 0) {
			if (cinfo->ui_not_driver) {
				return -1;
			}
			cinfo->driver_not_ui = TRUE;
		} else if (strcmp(argv[idx], "-x") == 0) {
			if (cinfo->driver_not_ui) {
				return -1;
			}
			cinfo->ui_not_driver = TRUE;
		} else if (strcmp(argv[idx], "-a") != 0) { /* matches all vars, so ignore */
			/* Anything else is substring */
			cinfo->substr = argv[idx];
		}
	}

	/* Allocate transfer buffer */
	bytes = sizeof(wlc_iov_trx_t) * WLU_IOV_BLOCK_LEN;
	if ((cinfo->trx_block = malloc(bytes)) == NULL) {
		fprintf(fid, "ERROR: Could not allocate transfer buffer of %d bytes\n", bytes);
		return -1;
	} else {
		memset(cinfo->trx_block, 0, bytes);
		cinfo->trx_block_bytes = bytes;
	}

	/* If "local vars not in driver" is set, allocate found buffer */
	if (cinfo->ui_not_driver) { /* Allocate space to match vars */
		bytes = sizeof(bool) * wlu_iov_info_count;
		if ((cinfo->var_found = malloc(bytes)) != NULL) {
			memset(cinfo->var_found, 0, bytes);
		} else { /* do not show these vars */
			fprintf(fid, "Unable to show variables missing from local info\n");
			cinfo->ui_not_driver = FALSE;
		}
	}

	if (cinfo->show_mod) {
		/* Put parameters into ioctl buffer */
		int *param_p;

		/* XXX May implement "caching" of module names; add "force refresh" cmd if so */
		param_p = (int *)(wl_iov_mod_names);
		/* memcpy might be safer for alignment */
		param_p[0] = WLU_MOD_NAME_BYTES;
		param_p[1] = WLU_MOD_NAME_MAX;
		if (wl_get(cinfo->wl, WLC_IOV_MODULES_GET, (void *)(wl_iov_mod_names),
			sizeof(wl_iov_mod_names)) < 0) {
			fprintf(fid, "Unable to show module names\n");
			cinfo->show_mod = FALSE;
		}
	}

	return 0;
}

/* Deallocation routine for dynamic mem */
static void
wl_iov_cleanup(struct iov_control_info *cinfo)
{
	if (cinfo != NULL) {
		if (cinfo->trx_block != NULL) {
			free(cinfo->trx_block);
			cinfo->trx_block = NULL;
		}
		if (cinfo->var_found != NULL) {
			free(cinfo->var_found);
			cinfo->var_found = NULL;
		}
	}
}

/* Look for a variable from driver in the local info; returns index in iov_info array */
static int
wl_iov_find(char *name)
{
	int idx;

	/* Could implement as bin search if iov_info kept sorted */
	for (idx = 0; idx < wlu_iov_info_count; idx++) {
		if (strcmp(wlu_iov_info[idx].name, name) == 0) {
			return idx;
		}
	}

	return -1;
}

/* First level of variable qualification check:  type and substring */
static bool
wl_iov_var_match(wlc_iov_trx_t *var, struct iov_control_info *cinfo)
{
	bool type_ok;
	bool substr_ok;

	type_ok = (cinfo->match_type < 0 || var->type == cinfo->match_type);
	substr_ok = !cinfo->substr || !*cinfo->substr ||
		(strstr(var->name, cinfo->substr) != NULL);

	/* Look for exact match of string? */
	if (cinfo->exact_match) {
		substr_ok = (cinfo && cinfo->substr &&
			strcmp(cinfo->substr, var->name) == 0);
	}

	return type_ok && substr_ok;
}

/* Does the var name match the given qualifications?  Is it found in local info? */
static wlu_iov_info_t *
wl_iov_qualify(wlc_iov_trx_t *var, struct iov_control_info *cinfo)
{
	int local_var_idx = -1;
	wlu_iov_info_t *rv = NULL;
	int var_qualifies;

	cinfo->tot_count++; /* Count this driver variable */
	var_qualifies = wl_iov_var_match(var, cinfo);

	if (var_qualifies || cinfo->ui_not_driver) {  /*  Look up in local info */
		local_var_idx = wl_iov_find(var->name);
	}

	/* Mark local variable if found */
	if (local_var_idx >= 0 && cinfo->ui_not_driver && cinfo->var_found != NULL) {
		cinfo->var_found[local_var_idx] = TRUE;
	}

	if (var_qualifies) { /* matched variable */
		if (cinfo->driver_not_ui && local_var_idx < 0) {  /* Not found case */
			rv = &iov_unknown_info;
		}
		if (!cinfo->driver_not_ui && local_var_idx >= 0) { /* Found case */
			rv = &wlu_iov_info[local_var_idx];
		}
	} /* else, not qualified, so return NULL */

	return rv;
}

#define NAMES_ONLY(cinfo)  (!cinfo->show_type && !cinfo->show_mod && \
			    !cinfo->show_desc && !cinfo->show_dflt)

/* Display headers if needed */
static void
show_header(FILE *fid, struct iov_control_info *cinfo)
{
	char line_buf[80];
	char *buf_p;

	buf_p = line_buf;
	buf_p += sprintf(buf_p, "Variables ");
	if (BCM_IOVT_VALID(cinfo->match_type)) {
		buf_p += sprintf(buf_p, "of type %s", wl_iov_type_strings[cinfo->match_type]);
	}
	if (cinfo->substr && *cinfo->substr) {
		buf_p += sprintf(buf_p, "matching =%s=", cinfo->substr);
	}
	fprintf(fid, "%s\n", buf_p);
	if (cinfo->driver_not_ui) {
		fprintf(fid, "The following variables do NOT have UI information\n");
	}

	if (NAMES_ONLY(cinfo)) {
		return;
	}

	buf_p = line_buf;
	buf_p += sprintf(buf_p, "%-30s", "Variable name");
	if (cinfo->show_type) {
		buf_p += sprintf(buf_p, " %-8s", "Type");
	}
	if (cinfo->show_mod) {
		buf_p += sprintf(buf_p, " %-20s", "Module");
	}
	if (cinfo->show_dflt) {
		buf_p += sprintf(buf_p, " %-10s", "Default");
	}

	fprintf(fid, "%s\n", line_buf);

}

/* See if var should be displayed; if so do that and count */
static void
wl_iov_check_show(FILE *fid, wlc_iov_trx_t *var, struct iov_control_info *cinfo)
{
	wlu_iov_info_t *local_var;
	char line_buf[80];
	char *buf_p;

	if ((local_var = wl_iov_qualify(var, cinfo)) != NULL) {
		if (cinfo->ui_not_driver) {
			return;  /* Don't print out driver matched vars */
		}
		if (NAMES_ONLY(cinfo)) {
			fprintf(fid, "%-25s%s", var->name,
				(cinfo->out_count + 1) % 3 ? " " : "\n");
		} else {
			buf_p = line_buf;
			buf_p += sprintf(buf_p, "%-30s", var->name);
			if (cinfo->show_type) {
				buf_p += sprintf(buf_p, " %-8s", BCM_IOVT_VALID(var->type) ?
					wl_iov_type_strings[var->type] : "no-type");
			}
			if (cinfo->show_mod) {
				buf_p += sprintf(buf_p, " %-20s", MOD_VALID(var->module) ?
					wl_iov_mod_names[var->module] : "no-mod");
			}
			if (cinfo->show_dflt) {
				if (local_var->flags & WLU_IOVI_DEFAULT_VALID) {
					buf_p += sprintf(buf_p, " %-10d", local_var->dflt);
				} else {
					buf_p += sprintf(buf_p, " %-10s", "?");
				}
			}

			fprintf(fid, "%s\n", line_buf);
			if (cinfo->show_desc) {
				fprintf(fid, "    %s\n", local_var->desc);
			}
		}
		cinfo->out_count++;
	}
}

/* After all vars gotten from driver, check if any missed from local info */
static void
wl_iov_check_ui_not_driver(FILE *fid, struct iov_control_info *cinfo)
{
	int idx, out_idx;
	bool first = TRUE;

	if (cinfo->ui_not_driver && cinfo->var_found != NULL) {
		for (out_idx = idx = 0; idx < wlu_iov_info_count; idx++) {
			if (!cinfo->var_found[idx]) {
				if (first) {
					fprintf(fid, "UI variables not in driver:\n");
					first = FALSE;
				}
				fprintf(fid, "%-25s%s", wlu_iov_info[idx].name,
				       (++out_idx % 3) ? " " : "\n");
				cinfo->out_count++;
			}
		}
		if (out_idx % 3) {
			fprintf(fid, "\n");
		}
	}
}

/*
 * IO variable information commands
 */
int
wl_iov_names(void *wl, cmd_t *cmd, char **argv)
{
	wlc_iov_trx_t *iov_p;
	int idx;
	int rv = 0;
	struct iov_control_info cinfo;  /* XXX:  Careful of internal dynamic allocation */

	UNUSED_PARAMETER(cmd);

	if (argv[1] == NULL || strcmp(argv[1], "-h") == 0) { /* Print usage */
		wl_iov_usage(stderr, argv[0]);
		return 0;
	}

	if (iov_parse_args_alloc(stderr, wl, argv, &cinfo) < 0) {
		fprintf(stderr, "Error parsing arguments for command %s\n", argv[0]);
		wl_iov_usage(stderr, argv[0]);
		return -1;
	}

	/*
	 * NOTE:  cinfo may contain dynamically allocate memory pointers that
	 * may require deallocation after this point
	 */

	show_header(stdout, &cinfo);

	do {
		/* Get a block of variables into trx_block starting at tot_vars */
		/* Put input parameters into ioctl buffer */
		int *param_p;

		param_p = (int *)(cinfo.trx_block);
		/* memcpy might be safer for alignment */
		param_p[0] = WLC_IOV_NAME_LEN;
		param_p[1] = cinfo.tot_count;
		param_p[2] = WLU_IOV_BLOCK_LEN;
		if (wl_get(wl, WLC_IOV_BLOCK_GET, (void *)(cinfo.trx_block),
			cinfo.trx_block_bytes) < 0) {
			fprintf(stderr, "Error on wl_get getting IOV block\n");
			rv = -1;
			break;
		}

		/* Search and display variables for matching criteria */
		for (idx = 0, iov_p = cinfo.trx_block;
		     idx < WLU_IOV_BLOCK_LEN && iov_p->name[0] != '\0';
		     idx++, iov_p++) {
			wl_iov_check_show(stdout, iov_p, &cinfo);
		}
	} while (idx == WLU_IOV_BLOCK_LEN);

	wl_iov_check_ui_not_driver(stdout, &cinfo);

	fprintf(stdout, "\n");
	if (cinfo.out_count == 0) {
		fprintf(stdout, "No matching variables found\n");
	}

	wl_iov_cleanup(&cinfo);

	return rv;
}

/*
 * Help command with category support
 */

const char *wl_cmd_category_strings[] = CMD_CATEGORY_STRINGS_INIT;
const int wl_cmd_category_count = ARRAYSIZE(wl_cmd_category_strings);
const char *wl_cmd_category_desc[] = CMD_CATEGORY_DESC_INIT;

/* ASSERT(ARRAYSIZE(wl_cmd_category_desc) == wl_cmd_category_count) */

/* If fewer than this many cmds are to be displayed, show full usage */
#define HELP_DESC_MAX 6

static void
wl_help_usage(FILE *fid, char *cmd)
{
	int i;

	fprintf(fid, "Usage:  wl %s [-s <substr>] [<categories>]\n", cmd);
	fprintf(fid, "    Display commands matching substr in any of the given categories\n");
	/* Note that -g (grep) is a synonym for -s */
	fprintf(fid, "Command categories include (use lower case to specify):\n");
	for (i = 0; i < wl_cmd_category_count; i++) {
		fprintf(fid, "    %-8s %s\n", wl_cmd_category_strings[i], wl_cmd_category_desc[i]);
	}
}

static unsigned int
cat_str_to_flag(char *val)
{
	int idx;
	int len, match_len;

	if (val == NULL || *val == '\0') {
		return 0;
	}
	for (idx = 0; idx < wl_cmd_category_count; idx++) {
		len = (int)strlen(val);
		match_len = len < 3 ? 3 : len;
		if (len > (int)strlen(wl_cmd_category_strings[idx])) {
			continue;
		}
		if (strncmp(val, wl_cmd_category_strings[idx], match_len) == 0) {
			return (1 << idx);
		}
	}

	fprintf(stderr, "Unrecognized command category: %s\n", val);
	return 0;
}

static int
wl_cmd_count(unsigned int cats, const char *substr)
{
	int count = 0;
	int i;

	for (i = 0; i < (int)NUM_ELS(cmd2cat); i++) {
		if ((cmd2cat[i].cat & cats) != 0) {
			if (substr == NULL || strstr(cmd2cat[i].name, substr) != NULL) {
				count++;
			}
		}
	}

	return count;
}

static void
print_cmds(unsigned int cats, const char *substr, int show_desc)
{
	char buf[160], *bufp;
	int count;
	cmd_t *cmd_loop;
	uint32 cmd_cat;

	bufp = buf;
	count = 0;
	*bufp = '\0';
	for (cmd_loop = wl_cmds; cmd_loop->name; cmd_loop++) {
		cmd_cat = cmd_cat_get(cmd_loop->name);
		if ((cmd_cat & cats) != 0) {
			if (substr == NULL || strstr(cmd_loop->name, substr) != NULL) {
				if (show_desc) {
					wl_cmd_usage(stdout, cmd_loop);
				} else {
					bufp += sprintf(bufp, "%-18s ", cmd_loop->name);
					count++;
					if (count == 4) {
						printf("%s\n", buf);
						count = 0;
						bufp = buf;
					}
				}
			}
		}
	}
	if (!show_desc && count > 0) {
		printf("%s\n", buf);
	}

}


int
wl_cmd_help(void *wl, cmd_t *cmd_in, char **argv)
{
	char *substr = NULL;
	int argv_idx = 1;
	unsigned int cats = 0, flag;
	int count;

	UNUSED_PARAMETER(wl);
	UNUSED_PARAMETER(cmd_in);

	if (argv[1] == NULL || strcmp(argv[1], "-h") == 0) { /* Print usage */
		wl_help_usage(stderr, argv[0]);
		return 0;
	}

	if (strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "-g") == 0) {
		if (argv[2] == NULL) {
			argv_idx = 2;
		} else {
			substr = argv[2];
			argv_idx = 3;
		}
	}
	while (argv[argv_idx] != NULL) {
		if ((flag = cat_str_to_flag(argv[argv_idx++])) == 0) {
			return -1;
		}
		cats |= flag;
	}
	if (cats == 0) {
		cats = CMD_ALL;
	}

	count = wl_cmd_count(cats, substr);
	if (count == 0) {
		printf("No matching commands\n");
	} else {
		print_cmds(cats, substr, count <= HELP_DESC_MAX);
	}

	return 0;
}


#endif /* BCMINTERNAL */
