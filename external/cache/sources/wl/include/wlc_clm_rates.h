/*
 * Indices for 802.11 a/b/g/n 1-3 chain symmetric transmit rates
 * Copyright (C) 2012, Broadcom Corporation
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 *
 * $Id: wlc_clm_rates.h 252708 2011-04-12 06:45:56Z fnejati $
 */

#ifndef _WLC_CLM_RATES_H_
#define _WLC_CLM_RATES_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define CHSPEC_IS80(x) FALSE


#define WL_RATESET_SZ_DSSS		4
#define WL_RATESET_SZ_OFDM		8
#define WL_RATESET_SZ_HT_MCS	8
#define WL_RATESET_SZ_VHT_MCS	10

#define WL_TX_CHAINS_MAX	3

#define WL_RATE_DISABLED		(-128) /* Power value corresponding to unsupported rate */

/* Transmit channel bandwidths */
typedef enum wl_tx_bw {
	WL_TX_BW_20,
	WL_TX_BW_40,
	WL_TX_BW_80,
	WL_TX_BW_20IN40,
	WL_TX_BW_20IN80,
	WL_TX_BW_40IN80,
	WL_TX_BW_ALL
} wl_tx_bw_t;


/*
 * Transmit modes.
 * Not all modes are listed here, only those required for disambiguation. e.g. SPEXP is not listed
 */
typedef enum wl_tx_mode {
	WL_TX_MODE_NONE,
	WL_TX_MODE_STBC,
	WL_TX_MODE_CDD,
	WL_TX_MODE_TXBF,
	WL_NUM_TX_MODES
} wl_tx_mode_t;


/* Number of transmit chains */
typedef enum wl_tx_chains {
	WL_TX_CHAINS_1 = 1,
	WL_TX_CHAINS_2,
	WL_TX_CHAINS_3
} wl_tx_chains_t;


/* Number of transmit streams */
typedef enum wl_tx_nss {
	WL_TX_NSS_1 = 1,
	WL_TX_NSS_2,
	WL_TX_NSS_3
} wl_tx_nss_t;

typedef enum clm_rates {
	/************
	 * 1 chain  *
	 ************
	 */

	CLM_RATE_1X1_DSSS_1			= 0,
	CLM_RATE_1X1_DSSS_2			= 1,
	CLM_RATE_1X1_DSSS_5_5		= 2,
	CLM_RATE_1X1_DSSS_11		= 3,

	CLM_RATE_1X1_OFDM_6			= 4,
	CLM_RATE_1X1_OFDM_9			= 5,
	CLM_RATE_1X1_OFDM_12		= 6,
	CLM_RATE_1X1_OFDM_18		= 7,
	CLM_RATE_1X1_OFDM_24		= 8,
	CLM_RATE_1X1_OFDM_36		= 9,
	CLM_RATE_1X1_OFDM_48		= 10,
	CLM_RATE_1X1_OFDM_54		= 11,

	CLM_RATE_1X1_MCS0			= 12,
	CLM_RATE_1X1_MCS1			= 13,
	CLM_RATE_1X1_MCS2			= 14,
	CLM_RATE_1X1_MCS3			= 15,
	CLM_RATE_1X1_MCS4			= 16,
	CLM_RATE_1X1_MCS5			= 17,
	CLM_RATE_1X1_MCS6			= 18,
	CLM_RATE_1X1_MCS7			= 19,

	/************
	 * 2 chains *
	 ************
	 */

	/* 1 Stream expanded + 1 */
	CLM_RATE_1X2_DSSS_1			= 20,
	CLM_RATE_1X2_DSSS_2			= 21,
	CLM_RATE_1X2_DSSS_5_5		= 22,
	CLM_RATE_1X2_DSSS_11		= 23,

	CLM_RATE_1X2_CDD_OFDM_6		= 24,
	CLM_RATE_1X2_CDD_OFDM_9		= 25,
	CLM_RATE_1X2_CDD_OFDM_12	= 26,
	CLM_RATE_1X2_CDD_OFDM_18	= 27,
	CLM_RATE_1X2_CDD_OFDM_24	= 28,
	CLM_RATE_1X2_CDD_OFDM_36	= 29,
	CLM_RATE_1X2_CDD_OFDM_48	= 30,
	CLM_RATE_1X2_CDD_OFDM_54	= 31,

	CLM_RATE_1X2_CDD_MCS0		= 32,
	CLM_RATE_1X2_CDD_MCS1		= 33,
	CLM_RATE_1X2_CDD_MCS2		= 34,
	CLM_RATE_1X2_CDD_MCS3		= 35,
	CLM_RATE_1X2_CDD_MCS4		= 36,
	CLM_RATE_1X2_CDD_MCS5		= 37,
	CLM_RATE_1X2_CDD_MCS6		= 38,
	CLM_RATE_1X2_CDD_MCS7		= 39,

	/* 2 Streams */
	CLM_RATE_2X2_STBC_MCS0		= 40,
	CLM_RATE_2X2_STBC_MCS1		= 41,
	CLM_RATE_2X2_STBC_MCS2		= 42,
	CLM_RATE_2X2_STBC_MCS3		= 43,
	CLM_RATE_2X2_STBC_MCS4		= 44,
	CLM_RATE_2X2_STBC_MCS5		= 45,
	CLM_RATE_2X2_STBC_MCS6		= 46,
	CLM_RATE_2X2_STBC_MCS7		= 47,

	CLM_RATE_2X2_SDM_MCS8		= 48,
	CLM_RATE_2X2_SDM_MCS9		= 49,
	CLM_RATE_2X2_SDM_MCS10		= 50,
	CLM_RATE_2X2_SDM_MCS11		= 51,
	CLM_RATE_2X2_SDM_MCS12		= 52,
	CLM_RATE_2X2_SDM_MCS13		= 53,
	CLM_RATE_2X2_SDM_MCS14		= 54,
	CLM_RATE_2X2_SDM_MCS15		= 55,


	/************
	 * 3 chains *
	 ************
	 */

	/* 1 Stream expanded + 2 */
	CLM_RATE_1X3_DSSS_1			= 56,
	CLM_RATE_1X3_DSSS_2			= 57,
	CLM_RATE_1X3_DSSS_5_5		= 58,
	CLM_RATE_1X3_DSSS_11		= 59,

	CLM_RATE_1X3_CDD_OFDM_6		= 60,
	CLM_RATE_1X3_CDD_OFDM_9		= 61,
	CLM_RATE_1X3_CDD_OFDM_12	= 62,
	CLM_RATE_1X3_CDD_OFDM_18	= 63,
	CLM_RATE_1X3_CDD_OFDM_24	= 64,
	CLM_RATE_1X3_CDD_OFDM_36	= 65,
	CLM_RATE_1X3_CDD_OFDM_48	= 66,
	CLM_RATE_1X3_CDD_OFDM_54	= 67,

	CLM_RATE_1X3_CDD_MCS0		= 68,
	CLM_RATE_1X3_CDD_MCS1		= 69,
	CLM_RATE_1X3_CDD_MCS2		= 70,
	CLM_RATE_1X3_CDD_MCS3		= 71,
	CLM_RATE_1X3_CDD_MCS4		= 72,
	CLM_RATE_1X3_CDD_MCS5		= 73,
	CLM_RATE_1X3_CDD_MCS6		= 74,
	CLM_RATE_1X3_CDD_MCS7		= 75,

	/* 2 Streams expanded + 1 */
	CLM_RATE_2X3_STBC_MCS0		= 76,
	CLM_RATE_2X3_STBC_MCS1		= 77,
	CLM_RATE_2X3_STBC_MCS2		= 78,
	CLM_RATE_2X3_STBC_MCS3		= 79,
	CLM_RATE_2X3_STBC_MCS4		= 80,
	CLM_RATE_2X3_STBC_MCS5		= 81,
	CLM_RATE_2X3_STBC_MCS6		= 82,
	CLM_RATE_2X3_STBC_MCS7		= 83,

	CLM_RATE_2X3_SDM_MCS8		= 84,
	CLM_RATE_2X3_SDM_MCS9		= 85,
	CLM_RATE_2X3_SDM_MCS10		= 86,
	CLM_RATE_2X3_SDM_MCS11		= 87,
	CLM_RATE_2X3_SDM_MCS12		= 88,
	CLM_RATE_2X3_SDM_MCS13		= 89,
	CLM_RATE_2X3_SDM_MCS14		= 90,
	CLM_RATE_2X3_SDM_MCS15		= 91,

	/* 3 Streams */
	CLM_RATE_3X3_SDM_MCS16		= 92,
	CLM_RATE_3X3_SDM_MCS17		= 93,
	CLM_RATE_3X3_SDM_MCS18		= 94,
	CLM_RATE_3X3_SDM_MCS19		= 95,
	CLM_RATE_3X3_SDM_MCS20		= 96,
	CLM_RATE_3X3_SDM_MCS21		= 97,
	CLM_RATE_3X3_SDM_MCS22		= 98,
	CLM_RATE_3X3_SDM_MCS23		= 99

	/* Number of rate codes */
	} clm_rates_t;

#define CLM_NUMRATES			100

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _WLC_CLM_RATES_H_ */
