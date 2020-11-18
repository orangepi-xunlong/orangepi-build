/*
 * Automatically generated IOVariable merge information
 * $Copyright: (c) 2006, Broadcom Corp.
 * All Rights Reserved.$
 * $Id: wlu_iov.c 399026 2013-04-27 00:44:07Z yongfang $
 *
 * Contains user IO variable information
 * Note that variable type information comes from the driver
 */


#if	defined(_CFE_)
#include <lib_types.h>
#include <lib_string.h>
#include <lib_printf.h>
#include <lib_malloc.h>
#include <cfe_error.h>
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

#ifdef BCMINTERNAL

wlu_iov_info_t wlu_iov_info[] = {
	{
		"2g_mrate", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"2g_rate", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"5g_mrate", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"5g_rate", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"a_mrate", CMD_PHY + CMD_RATE,
		0, 0,
		"Multicast rate override setting for .11a"
	},
	{
		"a_rate", CMD_PHY + CMD_RATE,
		0, 0,
		"Rate override setting for .11a"
	},
	{
		"activezone", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"ampdu", CMD_UNCAT,
		0, 0,
		"A-MPDU policy value; to modify, driver must be down"
	},
	{
		"ampdu_ba_wsize", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID | WLU_IOVI_BCM_INTERNAL, 64,
		"Advertized ba window size (in MPDU)"
	},
	{
		"ampdu_clear_dump", CMD_MON + CMD_ADMIN,
		0, 0,
		"Clears AMPDU specific statistics (WLCNT)"
	},
	{
		"ampdu_density", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID | WLU_IOVI_BCM_INTERNAL, 6,
		"Advertised receive density"
	},
	{
		"ampdu_dur", CMD_UNCAT,
		WLU_IOVI_BCM_INTERNAL, 0,
		"Max duration of transmitted ampdu (in msec)"
	},
	{
		"ampdu_ffpld", CMD_UNCAT,
		0, 0,
		"Display the ampdu for each mcs and the estimated minimum dma transfer rate"
	},
	{
		"ampdu_ffpld_rsvd", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"ampdu_hiagg_mode", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"ampdu_manual_mode", CMD_UNCAT,
		WLU_IOVI_BCM_INTERNAL, 0,
		"Enables manual mode; addba is sent by user through command line (in msec)"
	},
	{
		"ampdu_max_txunfl", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 200,
		"Maximum acceptable tx underflow per ampdu: tx_unfl < ampdu_max_txunfl * ampdus"
	},
	{
		"ampdu_mpdu", CMD_UNCAT,
		WLU_IOVI_BCM_INTERNAL, 0,
		"Max number of mpdu in ampdu"
	},
	{
		"ampdu_probe_mpdu", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"ampdu_resp_flush", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"ampdu_resp_timeout", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"ampdu_retry_limit", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID | WLU_IOVI_BCM_INTERNAL, 5,
		"Number retries before sending BAR to move window"
	},
	{
		"ampdu_rr_retry_limit", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID | WLU_IOVI_BCM_INTERNAL, 2,
		"Number retries of AMPDU at regular rate before change to fallback rate"
	},
	{
		"ampdu_rts", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"ampdu_rx_factor", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID | WLU_IOVI_BCM_INTERNAL, 2,
		"Advertises ampdu max size"
	},
	{
		"ampdu_send_addba", CMD_UNCAT,
		WLU_IOVI_BCM_INTERNAL, 0,
		"Sends addba request and initializes tid initiator"
	},
	{
		"ampdu_send_delba", CMD_UNCAT,
		WLU_IOVI_BCM_INTERNAL, 0,
		"Sends delba request and cleans up tid initiator"
	},
	{
		"ampdu_tid", CMD_MGMT,
		0, 0,
		"Enables/Disables AMPDU on a per-tid basis"
	},
	{
		"ampdu_tx_lowat", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID | WLU_IOVI_BCM_INTERNAL, 1,
		"Low watermark to initiate aggregate"
	},
	{
		"ampdu_txpkt_weight", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"amsdu", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 0,
		"A-MSDU policy value; driver must be down to modify"
	},
	{
		"amsdu_aggblock", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID | WLU_IOVI_BCM_INTERNAL, 0,
		"Manually block all scb A-MSDU aggregation"
	},
	{
		"amsdu_aggbytes", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 1600,
		"A-MSDU aggregation bytes limit"
	},
	{
		"amsdu_aggflush", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID | WLU_IOVI_BCM_INTERNAL | WLU_IOVI_WRITE_ONLY, 0,
		"Flush all scb A-MSDU buffers"
	},
	{
		"amsdu_aggsf", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 10,
		"A-MSDU aggregation MSDU number limit"
	},
	{
		"amsdu_clear_counters", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"amsdu_counters", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"amsdu_deaggdump", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"amsdu_hiwm", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"amsdu_lowm", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"amsdu_noack", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 0,
		"A-MSDU ack policy; send A-MSDU without mac ack"
	},
	{
		"amsdu_rxmax", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"amsdu_sim", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"antennas", CMD_UNCAT,
		WLU_IOVI_READ_ONLY, 0,
		"Read only return total number of antennas to be used(1-4 for now)"
	},
	{
		"ap", CMD_AP + CMD_STA,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Running as an access point"
	},
	{
		"ap_isolate", CMD_MGMT + CMD_AP,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Data frames received by the AP are passed up only, not forwarded"
	},
	{
		"apcschspec", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"apname", CMD_MGMT + CMD_MON + CMD_AP,
		WLU_IOVI_READ_ONLY | WLU_IOVI_DEFAULT_VALID, 0,
		"Get current associated AP; max len 16; if not associated may return stale data"
	},
	{
		"apsta", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Enable AP-STA mode"
	},
	{
		"apsta_dbg", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Debug mode for AP/STA"
	},
	{
		"assoc_info", CMD_MGMT + CMD_STA,
		0, 0,
		"Returns a wl_assoc_info_t struct with assoc pkts lengths and 802.11 headers"
	},
	{
		"assoc_req_ies", CMD_UNCAT,
		0, 0,
		"Returns the IE portion of the most recent association request pkt"
	},
	{
		"assoc_resp_ies", CMD_UNCAT,
		0, 0,
		"Returns the IE portion of the most recent association response pkt"
	},
	{
		"assocroam", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Enable roaming to assoc preferred band when there is no need to roam"
	},
	{
		"auth", CMD_MGMT + CMD_SEC,
		0, 0,
		"no desc"
	},
	{
		"authe_sta_list", CMD_MGMT + CMD_SEC,
		0, 0,
		"no desc"
	},
	{
		"autho_sta_list", CMD_MGMT + CMD_SEC,
		0, 0,
		"no desc"
	},
	{
		"autocountry_default", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"ba", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Block Ack policy value; can only be modified when driver is down"
	},
	{
		"band_range", CMD_PHY + CMD_CHAN,
		0, 0,
		"2.4G band - 0; 5G band: low - 1, mid - 2, high - 3"
	},
	{
		"ba_bar_timeout", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 100,
		"timeout in msec to send BAR"
	},
	{
		"ba_barfreq", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 4,
		"freq of packets at which BAR is sent."
	},
	{
		"ba_bsize", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 16,
		"advertized ba window size (in MPDU)"
	},
	{
		"ba_cf_policy", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 0,
		"BA/BAR ctrl frame ack policy; 0:normal; 1:noack"
	},
	{
		"ba_clear_counters", CMD_MON + CMD_ADMIN,
		WLU_IOVI_BCM_INTERNAL, 0,
		"clear/reset BA counters (BCMDBG)"
	},
	{
		"ba_counters", CMD_UNCAT,
		WLU_IOVI_BCM_INTERNAL, 0,
		"Return BA counters (BCMDBG)"
	},
	{
		"ba_delba_timeout", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"ba_ini_pkt_thresh", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 50,
		"moving average in pkts/sec for prior 8-sec window to initiate addba"
	},
	{
		"ba_send", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID | WLU_IOVI_BCM_INTERNAL, 1,
		"Send unsolicited ADDBA; for testing;"
	},
	{
		"ba_sim", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID | WLU_IOVI_BCM_INTERNAL, 1,
		"Setup for linuxsim testing;"
	},
	{
		"bcmerror", CMD_UNCAT,
		0, 0,
		"Get the error code corresonding to the last configuration error."
	},
	{
		"bcmerrorstr", CMD_ADMIN,
		0, 0,
		"Get the error string corresonding to the last configuration error."
	},
	{
		"bcn_timeout", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"bg_mrate", CMD_PHY + CMD_RATE,
		0, 0,
		"Multicast rate override setting for .11b/g"
	},
	{
		"bg_rate", CMD_PHY + CMD_RATE,
		0, 0,
		"Rate override setting for .11b/g"
	},
	{
		"bss", CMD_MGMT + CMD_AP + CMD_STA,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Enable BSS configuration"
	},
	{
		"bss_maxassoc", CMD_UNCAT,
		0, 0,
		"Limit the number of 802.11 Associations the driver should accept for a BSS"
	},
	{
		"btc_mode", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Bluetooth coexistance modes; 0: disable; 1: enable; 2: prempt"
	},
	{
		"cac", CMD_UNCAT,
		0, 0,
		"Call Admission Control; CAC can only be enabled if WME is enabled"
	},
	{
		"cac_addts", CMD_MGMT + CMD_WME + CMD_AP + CMD_STA,
		0, 0,
		"Add/update TSPEC; requires tspec_arg_t structure as input"
	},
	{
		"cac_addts_timeout", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"cac_delts", CMD_MGMT + CMD_WME + CMD_AP + CMD_STA,
		0, 0,
		"Delete matching TSPEC; requires tspec_arg_t structure as input"
	},
	{
		"cac_tslist", CMD_MGMT + CMD_WME + CMD_AP + CMD_STA,
		WLU_IOVI_READ_ONLY, 0,
		"Get current list of driver TSINFO"
	},
	{
		"cac_tspec", CMD_MGMT + CMD_WME + CMD_AP + CMD_STA,
		WLU_IOVI_READ_ONLY, 0,
		"Get specified TSPEC with given TSINFO"
	},
	{
		"cap", CMD_ADMIN,
		WLU_IOVI_READ_ONLY, 0,
		"Returns whitespace-separated driver/device capabilities"
	},
	{
		"ccgpioctrl", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"ccgpioin", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"ccgpioout", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"ccgpioouten", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"cck_txbw", CMD_PHY + CMD_CHAN + CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID | WLU_IOVI_BCM_INTERNAL, 2,
		"CCK frame tx bandwidth; 2: 20Mhz lower, 3: 20Mhz upper"
	},
	{
		"ccx_auth_mode", CMD_UNCAT,
		0, 0,
		"CCX authentication mode"
	},
	{
		"ccx_rm", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"ccx_rm_limit", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"ccx_version", CMD_UNCAT,
		0, 0,
		"Network ccx version."
	},
	{
		"ccx_ihv", CMD_UNCAT,
		WLU_IOVI_READ_ONLY, 0,
		"Under IHV control"
	},
	{
		"ccx_enable", CMD_UNCAT,
		0, 0,
		"CCX enabled"
	},
	{
		"ccx_v4_only", CMD_UNCAT,
		0, 0,
		"CCXv4 only"
	},
	{
		"chanspec", CMD_PHY + CMD_CHAN,
		0, 0,
		"Current chanspec in use; set for next BSS created"
	},
	{
		"chanspecs", CMD_PHY + CMD_CHAN,
		0, 0,
		"Returns a list of all of the available chanspecs"
	},
	{
		"clkreqtimer", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"closednet", CMD_SEC,
		WLU_IOVI_DEFAULT_VALID, 0,
		"BSS Configuration Closed Network attribute; BSS must be disabled to change"
	},
	{
		"counters", CMD_MON + CMD_ADMIN,
		WLU_IOVI_READ_ONLY, 0,
		"Get a buffer containing driver/ucode counters"
	},
	{
		"country_ie_override", CMD_UNCAT,
		WLU_IOVI_BCM_INTERNAL, 0,
		"Stores the country IE for beacons and probe responses"
	},
	{
		"country_list_extended", CMD_UNCAT,
		0, 0,
		"If true, all countries reported by get_country_list"
	},
	{
		"cram", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"csa", CMD_PHY + CMD_CHAN + CMD_MGMT,
		0, 0,
		"AP mode only; send out Channel Switch Announcement management action frame"
	},
	{
		"cur_etheraddr", CMD_UNCAT,
		0, 0,
		"Current MAC address"
	},
	{
		"cur_mcsset", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"dbgsel", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"dfs_channel_forced", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 0,
		"The next dfs channel is forced to this channel for testing"
	},
	{
		"dfs_ism_monitor", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Force ISM to be in monitor-only mode; disables channel swtiching, etc"
	},
	{
		"dfs_postism", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 60,
		"Number of seconds to listen for radar on new channel for 802.11H"
	},
	{
		"dfs_preism", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 60,
		"Number of seconds to listen for radar before initiating a network"
	},
	{
		"dfs_status", CMD_PHY + CMD_CHAN,
		WLU_IOVI_READ_ONLY, 0,
		"Report the status of dfs processing"
	},
	{
		"diag", CMD_ADMIN,
		0, 0,
		"Returns test information"
	},
	{
		"dump", CMD_ADMIN,
		0, 0,
		"Options all, list, <functions>; call registered dump functions"
	},
	{
		"eap_restrict", CMD_SEC,
		0, 0,
		"no desc"
	},
	{
		"eirp", CMD_UNCAT,
		0, 0,
		"Is current locale use of txpower EIRP or not"
	},
	{
		"event_msgs", CMD_MAC,
		0, 0,
		"128-bit vector to enable MAC event reporting"
	},
	{
		"fast_timer", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"fips", CMD_UNCAT,
		0, 0,
		"Windows NDIS STA; 0: disable; 1: enable; -1: prohibit FIPS"
	},
	{
		"fips_add_key", CMD_UNCAT,
		0, 0,
		"Windows NDIS STA driver; set key for FIPS."
	},
	{
		"fips_oid", CMD_UNCAT,
		0, 0,
		"Windows NDIS STA; change when not associated; OID_FSW_FIPS_MODE request"
	},
	{
		"fips_remove_key", CMD_UNCAT,
		0, 0,
		"Windows NDIS STA driver; remove key for FIPS."
	},
	{
		"fragthresh", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"freqtrack", CMD_PHY + CMD_CHAN,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Current frequency tracking mode; 0: auto; 1: on; 2: off"
	},
#if defined(WLNINTENDO)
	{
		"gameinfo", CMD_UNCAT,
		0, 0,
		"no desc"
	},
#endif /* WLNINTENDO */
	{
		"gf_cap", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"gf_protection", CMD_UNCAT,
		0, 0,
		"gf_protection; 0: off, 1: on"
	},
	{
		"gf_protection_override", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, -1,
		"State of gf_protection_override; -1: auto, 0: off, 1: on"
	},
	{
		"glacial_timer", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"gpioout", CMD_DEV,
		0, 0,
		"Set GPIO pins to given; use with EXTREME caution; see wl gpiodump"
	},
	{
		"gpiotimermask", CMD_UNCAT,
		WLU_IOVI_BCM_INTERNAL, 0,
		"Timer mask for gpiotimer"
	},
	{
		"gpiotimerval", CMD_UNCAT,
		0, 0,
		"Deprecated: use leddc"
	},
	{
		"htphy_membership", CMD_UNCAT,
		0, 0,
		"enable/disable HT PHY Membership; 0: off, 1: on"
	},
	{
		"hwkeys", CMD_SEC + CMD_ADMIN + CMD_DEV,
		0, 0,
		"no desc"
	},
	{
		"ibss_allowed", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"ibss_gmode", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"join_pref", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"keydatareq", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"led_blinkfast", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"led_blinkmed", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"led_blinkslow", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"leddc", CMD_UNCAT,
		0, 0,
		"gpiotimer value; to enable powersavings for LEDs"
	},
	{
		"lifetime", CMD_MGMT + CMD_AP + CMD_STA,
		0, 0,
		"This iovar controls the packet lifetime in ms for an access class"
	},
	{
		"lpphy_rxiqcal", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"lpphy_tx_tone", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"lpphy_txiqlocal", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"lpphy_txpwrctrl", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"lpphy_txpwrindex", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"malloced", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"maxassoc", CMD_UNCAT,
		0, 0,
		"Limit the number of 802.11 Associations the driver should accept"
	},
	{
		"mcast_list", CMD_UNCAT,
		0, 0,
		"List of multicast addresses to be passed to upper-layer protocols (4320 CDC)"
	},
	{
		"mimo_bw_cap", CMD_UNCAT,
		0, 0,
		"Supported channel width bit in HT Cap Info field, TRUE = 40MHz; FALSE = 20MHz"
	},
	{
		"mimo_preamble", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, -1,
		"n-mode preamb override; -1: auto; 0: mixed-mode; 1: GF-only; device down to set"
	},
	{
		"mimo_ps", CMD_PHY + CMD_CHAN + CMD_POWER,
		WLU_IOVI_DEFAULT_VALID, 3,
		"MIMO EWC capability IE; 0: No MIMO, 1: RTS before MIMO, 2: N/A, 3: No Restriction"
	},
	{
		"mimo_ss_stf", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"mimo_txbw", CMD_PHY + CMD_CHAN,
		WLU_IOVI_DEFAULT_VALID, 2,
		"MIMO frame tx bandwidth; 2: 20Mhz lower, 3: 20Mhz upper, 4: 40Mhz, 5: 40Mhz dup"
	},
	{
		"mode_reqd", CMD_MGMT,
		WLU_IOVI_DEFAULT_VALID, 0,
		"set/get required mode for STA association acceptance with AP"
	},
	{
		"mpc", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Enable Minimum Power Consumption in driver"
	},
	{
		"mpreq", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"mssid", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Multi-SSID enabled; driver must be down to set"
	},
	{
		"n_bw", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"ndis_unblock_8021x", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Override 802.1x authentication"
	},
#if defined(WLNINTENDO)
	{
		"nitro", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nitro_clear_counters", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nitro_counters", CMD_UNCAT,
		0, 0,
		"no desc"
	},
#endif /* WLNINTENDO */
	{
		"nmode", CMD_UNCAT,
		0, 0,
		"Enable pre-n features; driver must be down to set"
	},
	{
		"nmode_protection", CMD_UNCAT,
		0, 0,
		"n_protection; 0: off, 1: mixed-mode, 2: CTS-to-self"
	},
	{
		"nmode_protection_override", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, -1,
		"State of n_protection; -1: auto, 0: off, 1: on, 2: MM Hdr 3: CTS"
	},
	{
		"nobcnssid", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nobcprbresp", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nonerp_present", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_5g_pwrgain", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_aci_scan", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_aci_w2based_detection", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"phy_antsel", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"phy_antsel_override", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_est_power", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_fixed_noise", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_forcecal", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_forcemphasecal", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_gain_boost", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_gpiosel", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_mphasecal", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_mphasecalstate", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_nextcal", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_percal", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_rfseq", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_rssisel", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_rxant_config", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_rxcalparams", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_rxiqcal", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_scraminit", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_test_tssi", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_test_tssi_offs", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_tx_tone", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_txant_config", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_txiqlocal", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_txpwrctrl", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_txpwrindex", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nphy_txrx_chain", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nrate", CMD_PHY + CMD_RATE,
		0, 0,
		"band-specific rate override; 0: SISO, 1: CDD, 2: STBC, 3: SDM"
	},
	{
		"nreqd", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"nvram_dump", CMD_ADMIN,
		0, 0,
		"Retrieve nvram contents in multi-string, double-null-byte-terminated format"
	},
	{
		"nvram_get", CMD_ADMIN,
		0, 0,
		"Get an nvram variable by name"
	},
	{
		"ofdm_present", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"ofdm_txbw", CMD_PHY + CMD_CHAN + CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID | WLU_IOVI_BCM_INTERNAL, 2,
		"OFDM frame tx bandwidth; 2: 20Mhz(lower), 3: 20Mhz upper, 4: N/A, 5: 40Mhz dup"
	},
	{
		"otpdump", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"passive", CMD_PHY + CMD_MGMT + CMD_AP + CMD_STA,
		0, 0,
		"no desc"
	},
	{
		"pcicfgreg", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"pcieclkreq", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"pciegpioout", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"pciegpioouten", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"pciel1plldown", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"pcielcreg", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"pciereg", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"per_chan_info", CMD_UNCAT,
		WLU_IOVI_READ_ONLY, 0,
		"Various channel attributes returned as bitfields"
	},
	{
		"perm_etheraddr", CMD_UNCAT,
		WLU_IOVI_READ_ONLY, 0,
		"Permanent MAC address"
	},
	{
		"phy_rssi_ant", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"phy_watchdog", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"phynoise_polling", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"pm2_rcv_dur", CMD_POWER,
		WLU_IOVI_DEFAULT_VALID, 0,
		"power mgmt mode 2 receive duty cycle as a percentage "
		"(15-85%) of the beacon interval"
	},
	{
		"pm2_sleep_ret", CMD_POWER,
		WLU_IOVI_DEFAULT_VALID, -1,
		"power mgmt mode 2 inactive ms before re-entering power save mode"
	},
	{
		"pmkid_info", CMD_SEC,
		0, 0,
		"pmkid cache table"
	},
	{
		"qtxpower", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"radarargs", CMD_PHY,
		0, 0,
		"Radar detection parameters"
	},
	{
		"radarargs40", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"radar_status", CMD_PHY,
		WLU_IOVI_READ_ONLY, 0,
		"Report the status of radar detection"
	},
	{
		"clear_radar_status", CMD_PHY,
		0, 0,
		"Clear the status of radar detection"
	},
	{
		"rand", CMD_ADMIN + CMD_DEV,
		WLU_IOVI_READ_ONLY, 0,
		"Generate a random number from MAC"
	},
	{
		"ratesel_maxshortampdus", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"ratesel_sim", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"ratesel_table_index", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"ratesel_table_value", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"reset_d11cnts", CMD_MON + CMD_ADMIN,
		0, 0,
		"Resets the IEEE 802.11 MIB counters"
	},
	{
		"rfdisabledly", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"rifs", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 0,
		"EWC spec to reduce tx interframe gap; it implies no-ack"
	},
	{
		"rm_rep", CMD_PHY + CMD_MON,
		0, 0,
		"no desc"
	},
	{
		"rm_req", CMD_PHY + CMD_MON,
		0, 0,
		"no desc"
	},
	{
		"rtsthresh", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"scan_assoc_time", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 20,
		"Dwell time in ms when channel scanning"
	},
	{
		"scan_home_time", CMD_PHY + CMD_CHAN,
		WLU_IOVI_DEFAULT_VALID, 20,
		"Dwell time on home BSS channel"
	},
	{
		"scan_nprobes", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 2,
		"Number of probe requests sent during the dwell time on an actively scanned channel"
	},
	{
		"scan_passive_time", CMD_PHY + CMD_CHAN,
		WLU_IOVI_DEFAULT_VALID, 250,
		"Dwell time in ms for each channel passively scanned"
	},
	{
		"scan_unassoc_time", CMD_PHY + CMD_CHAN,
		WLU_IOVI_DEFAULT_VALID, 20,
		"Dwell time in ms for each channel actively scanned when not participating in a BSS"
	},
	{
		"scanresults_minrssi", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"scb_activity_time", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"sd_cis", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"sd_drivestrength", CMD_DEV,
		0, 0,
		"SDIO drive strength in mA(2mA-12mA), in 2mA increment"
	},
	{
		"serdespllreg", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"serdesrxreg", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"serdestxreg", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"slow_timer", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"ssid", CMD_DEV,
		0, 0,
		"BSS configuration's SSID; can only be changed when the indexed BSS is disabled"
	},
	{
		"ssidmask", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"sta_info", CMD_MON + CMD_STA,
		WLU_IOVI_READ_ONLY, 0,
		"Gives station info based on MAC"
	},
	{
		"sta_retry_time", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Time in seconds before retry after failed assoc attempt"
	},
	{
		"staname", CMD_MGMT + CMD_MON + CMD_STA,
		0, 0,
		"The station name; max 16 bytes"
	},
	{
		"sup_auth_status", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"sup_auth_status_ext", CMD_UNCAT,
		0, 0,
		"Extended in-driver supplicant authentication status"
	},
	{
		"sup_wpa", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"test_s60", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"txburst_limit", CMD_UNCAT,
		0, 0,
		"get current Tx Burst Limit setting; 0: off, 1: on"
	},
	{
		"txburst_limit_override", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, -1,
		"State of txburst_limit_override; -1: auto, 0: off, 1: on"
	},
	{
		"crash", CMD_UNCAT,
		WLU_IOVI_WRITE_ONLY, 0,
		"Force crash; 0: oops, 1: ASSERT"
	},
	{
		"txc", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"txc_policy", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 1,
		"no desc"
	},
	{
		"txc_sticky", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 0,
		"no desc"
	},
	{
		"txfifo_sz", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"txinstpwr", CMD_PHY + CMD_POWER,
		WLU_IOVI_READ_ONLY, 0,
		"Power output based on current TSSI"
	},
	{
		"txmaxpkts", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"vblanktsf", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"ver", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"vlan_mode", CMD_UNCAT,
		0, 0,
		"Should driver strip priority tags (VLAN ID zero)"
	},
	{
		"vndr_ie", CMD_UNCAT,
		0, 0,
		"Access vendor proprietary IEs to 802.11 Management Packets transmitted from the AP"
	},
	{
		"wds_wpa_role", CMD_MGMT + CMD_SEC,
		0, 0,
		"no desc"
	},
	{
		"wdstimeout", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"wet", CMD_MGMT + CMD_SEC,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Wireless ethernet support; wl must be in STA mode"
	},
	{
		"wet_host_ipv4", CMD_UNCAT,
		0, 0,
		"Enable WET client DHCP relay agent"
	},
	{
		"wet_host_mac", CMD_UNCAT,
		0, 0,
		"Enable WET host DHCP relay agent"
	},
	{
		"wlan_assoc_reason", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 0,
		"CCXv4 assocation reason codes"
	},
	{
		"wlfeatureflag", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Flags defined in wlioctl.h"
	},
	{
		"wme", CMD_WME,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Media extension modes; -1: auto, 1: enabled, 0: disabled"
	},
	{
		"wme_ac_ap", CMD_UNCAT,
		0, 0,
		"Access class parameters for AP"
	},
	{
		"wme_ac_sta", CMD_UNCAT,
		0, 0,
		"Access class parameters for STA"
	},
	{
		"wme_apsd", CMD_PHY + CMD_POWER + CMD_WME + CMD_AP,
		WLU_IOVI_DEFAULT_VALID, 1,
		"no desc"
	},
	{
		"wme_bss_disable", CMD_WME,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Disable WMM/WME advertising for a specific BSS (if wme is enabled in the system)"
	},
	{
		"wme_clear_counters", CMD_WME + CMD_MON + CMD_ADMIN,
		WLU_IOVI_BCM_INTERNAL, 0,
		"Clear WMM counters"
	},
	{
		"wme_counters", CMD_WME + CMD_MON + CMD_ADMIN,
		WLU_IOVI_READ_ONLY | WLU_IOVI_BCM_INTERNAL, 0,
		"Get WME counters maintained by the driver"
	},
	{
		"wme_dp", CMD_WME,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Bitmap discard policy per AC; 0: newest 1: oldest; [3:0]: VO VI BK BE"
	},
	{
		"wme_noack", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Enable no-ack WME mode"
	},
	{
		"wme_prec_queuing", CMD_UNCAT,
		0, 0,
		"no desc"
	},
	{
		"wme_qosinfo", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Power save delivery info, per access class"
	},
	{
		"wme_sta_list", CMD_UNCAT,
		0, 0,
		"Get the WME STA list; AP only"
	},
	{
		"wme_tx_params", CMD_WME,
		0, 0,
		"Set/get per-AC TX retry and max rate parameters"
	},
	{
		"wpa_auth", CMD_MGMT + CMD_SEC,
		WLU_IOVI_DEFAULT_VALID, 0,
		"WPA authentication mode"
	},
	{
		"wme_maxbw_params", CMD_WME,
		WLU_IOVI_BCM_INTERNAL, 0,
		"Set/get per-AC max bandwidth"
	},
	{
		"wpa_cap", CMD_MGMT + CMD_SEC,
		WLU_IOVI_DEFAULT_VALID, 0,
		"RSN capability advertisement; see 802.11 RSN info fields"
	},
	{
		"wpa_msgs", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Enable WPA event notification to external supp/auth"
	},
	{
		"wpaie", CMD_UNCAT,
		0, 0,
		"Info element for WPA associated with E_ASSOC_IND event; Set is STA only"
	},
	{
		"wsec", CMD_MGMT + CMD_SEC,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Encryption types for BSS config"
	},
	{
		"wsec_authorization", CMD_UNCAT,
		0, 0,
		"Convey auth status between upper layers"
	},
	{
		"wsec_key", CMD_UNCAT,
		WLU_IOVI_WRITE_ONLY, 0,
		"Add or delete a wsec key"
	},
	{
		"wsec_restrict", CMD_MGMT + CMD_SEC,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Drop unencrypted packets when WSEC is enabled in bsscfg"
	},
	{
		"40_intolerant", CMD_UNCAT,
		0, 0,
		"Device is 40Mhz Intolerant in the 2.4G Band"
	},
	{
		"40_intolerant_detected", CMD_UNCAT,
		WLU_IOVI_READ_ONLY, 0,
		"Indicates if another 40Mhz Intolerant device has been detected"
	},
	{
		"btc_params", CMD_UNCAT,
		0, 0,
		"Bluetooth coexistance parameters"
	},
	{
		"btc_flags", CMD_UNCAT,
		0, 0,
		"Bluetooth coexistance flags"
	},
	{
		"brcm_ie", CMD_UNCAT,
		WLU_IOVI_DEFAULT_VALID, 0,
		"Enables brcm ie in the auth requests sent by the sta"
	}
};

int wlu_iov_info_count = ARRAYSIZE(wlu_iov_info);

#endif /* BCMINTERNAL */
