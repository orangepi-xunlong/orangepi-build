/*
 * This module does provides various mappings to or from the CLM rate indexes.
 *
 * $Copyright (C) 2011 Broadcom Corporation$
 */
/*FILE-CSTYLED*/

#include <stdio.h>
#include <typedefs.h>

#include "wlu_rates_matrix.h"

#define CLM_NO_RATE_STRING "NO_RATE"
#define EXP_MODE_UNKNOWN   "Unknown expansion mode"
#define AUTO_RATESPEC 0x0
#define MHZ_TO_HALF_MHZ 2

typedef enum exp_mode {
	EXP_MODE_ID_DEF = 0,
	EXP_MODE_ID_STBC,
	EXP_MODE_ID_TXBF,
	EXP_MODE_ID_MAX
} exp_mode_t;
#ifdef D11AC_IOTYPES 
static reg_rate_index_t get_legacy_reg_rate_index(int rate_hmhz, int tx_expansion, exp_mode_t mode);
static reg_rate_index_t get_ht_reg_rate_index(int mcs, int tx_expansion, exp_mode_t mode);
static reg_rate_index_t get_vht_reg_rate_index(int mcs, int nss, int tx_expansion, exp_mode_t mode);

static int get_legacy_rate_identifier(int rate_hmhz);
static int get_legacy_mode_identifier(int tx_expansion, exp_mode_t mode);

static int get_vht_rate_identifier(int vht_mcs_index);
static int get_vht_ss1_mode_identifier(int tx_expansion, exp_mode_t mode);
static int get_vht_ss2_mode_identifier(int tx_expansion, exp_mode_t mode);
static int get_vht_ss3_mode_identifier(int tx_expansion, exp_mode_t mode);

static const char *get_mode_name(exp_mode_t mode);
#endif

enum {
	LEGACY_RATE_ID_NO_RATE = -1,
	LEGACY_RATE_ID_1MHZ = 0,
	LEGACY_RATE_ID_2MHZ,
	LEGACY_RATE_ID_5_5MHZ,
	LEGACY_RATE_ID_11MHZ,
	LEGACY_RATE_ID_6MHZ,
	LEGACY_RATE_ID_9MHZ,
	LEGACY_RATE_ID_12MHZ,
	LEGACY_RATE_ID_18MHZ,
	LEGACY_RATE_ID_24MHZ,
	LEGACY_RATE_ID_36MHZ,
	LEGACY_RATE_ID_48MHZ,
	LEGACY_RATE_ID_54MHZ,
	LEGACY_RATE_ID_MAX
};

enum {
	LEGACY_MODE_ID_NONE = 0,
	LEGACY_MODE_ID_TXEXP1,
	LEGACY_MODE_ID_TXEXP2,
	LEGACY_MODE_ID_TXBF1,
	LEGACY_MODE_ID_TXBF2,
	LEGACY_MODE_ID_MAX
};

enum {
	VHT_RATE_INDEX_0 = 0,
	VHT_RATE_INDEX_1,
	VHT_RATE_INDEX_2,
	VHT_RATE_INDEX_3,
	VHT_RATE_INDEX_4,
	VHT_RATE_INDEX_5,
	VHT_RATE_INDEX_6,
	VHT_RATE_INDEX_7,
	VHT_RATE_INDEX_8,
	VHT_RATE_INDEX_9,
	VHT_RATE_INDEX_MAX
};

enum {
	VHT_SS1_MODE_ID_NONE = 0,
	VHT_SS1_MODE_ID_CDD1,
	VHT_SS1_MODE_ID_STBC,
	VHT_SS1_MODE_ID_CDD2,
	VHT_SS1_MODE_ID_STBC_SPEXP1,
	VHT_SS1_MODE_ID_TXBF1,
	VHT_SS1_MODE_ID_TXBF2,
	VHT_SS1_MODE_ID_MAX
};

enum {
	VHT_SS2_MODE_ID_NONE = 0,
	VHT_SS2_MODE_ID_SPEXP1,
	VHT_SS2_MODE_ID_TXBF0,
	VHT_SS2_MODE_ID_TXBF1,
	VHT_SS2_MODE_ID_MAX
};

enum {
	VHT_SS3_MODE_ID_NONE = 0,
	VHT_SS3_MODE_ID_TXBF0,
	VHT_SS3_MODE_ID_MAX
};

const char *clm_rate_group_labels[] = {
	"DSSS",
	"OFDM",
	"MCS0_7",
	"VHT8_9SS1",
	"DSSS_MULTI1",
	"OFDM_CDD1",
	"MCS0_7_CDD1",
	"VHT8_9SS1_CDD1",
	"MCS0_7_STBC",
	"VHT8_9SS1_STBC",
	"MCS8_15",
	"VHT8_9SS2",
	"DSSS_MULTI2",
	"OFDM_CDD2",
	"MCS0_7_CDD2",
	"VHT8_9SS1_CDD2",
	"MCS0_7_STBC_SPEXP1",
	"VHT8_9SS1_STBC_SPEXP1",
	"MCS8_15_SPEXP1",
	"VHT8_9SS2_SPEXP1",
	"MCS16_23",
	"VHT8_9SS3",
	"OFDM_TXBF1",
	"MCS0_7_TXBF1",
	"VHT8_9SS1_TXBF1",
	"MCS8_15_TXBF0",
	"OFDM_TXBF2",
	"MCS0_7_TXBF2",
	"VHT8_9SS1_TXBF2",
	"MCS8_15_TXBF1",
	"VHT8_9SS2_TXBF1",
	"MCS16_23_TXBF0",
};


const char *clm_rate_labels[] = {
	"DSSS1",
	"DSSS2",
	"DSSS5",
	"DSSS11",
	"OFDM6",
	"OFDM9",
	"OFDM12",
	"OFDM18",
	"OFDM24",
	"OFDM36",
	"OFDM48",
	"OFDM54",
	"MCS0",
	"MCS1",
	"MCS2",
	"MCS3",
	"MCS4",
	"MCS5",
	"MCS6",
	"MCS7",
	"VHT8SS1",
	"VHT9SS1",
	"DSSS1_MULTI1",
	"DSSS2_MULTI1",
	"DSSS5_MULTI1",
	"DSSS11_MULTI1",
	"OFDM6_CDD1",
	"OFDM9_CDD1",
	"OFDM12_CDD1",
	"OFDM18_CDD1",
	"OFDM24_CDD1",
	"OFDM36_CDD1",
	"OFDM48_CDD1",
	"OFDM54_CDD1",
	"MCS0_CDD1",
	"MCS1_CDD1",
	"MCS2_CDD1",
	"MCS3_CDD1",
	"MCS4_CDD1",
	"MCS5_CDD1",
	"MCS6_CDD1",
	"MCS7_CDD1",
	"VHT8SS1_CDD1",
	"VHT9SS1_CDD1",
	"MCS0_STBC",
	"MCS1_STBC",
	"MCS2_STBC",
	"MCS3_STBC",
	"MCS4_STBC",
	"MCS5_STBC",
	"MCS6_STBC",
	"MCS7_STBC",
	"VHT8SS1_STBC",
	"VHT9SS1_STBC",
	"MCS8",
	"MCS9",
	"MCS10",
	"MCS11",
	"MCS12",
	"MCS13",
	"MCS14",
	"MCS15",
	"VHT8SS2",
	"VHT9SS2",
	"DSSS1_MULTI2",
	"DSSS2_MULTI2",
	"DSSS5_MULTI2",
	"DSSS11_MULTI2",
	"OFDM6_CDD2",
	"OFDM9_CDD2",
	"OFDM12_CDD2",
	"OFDM18_CDD2",
	"OFDM24_CDD2",
	"OFDM36_CDD2",
	"OFDM48_CDD2",
	"OFDM54_CDD2",
	"MCS0_CDD2",
	"MCS1_CDD2",
	"MCS2_CDD2",
	"MCS3_CDD2",
	"MCS4_CDD2",
	"MCS5_CDD2",
	"MCS6_CDD2",
	"MCS7_CDD2",
	"VHT8SS1_CDD2",
	"VHT9SS1_CDD2",
	"MCS0_STBC_SPEXP1",
	"MCS1_STBC_SPEXP1",
	"MCS2_STBC_SPEXP1",
	"MCS3_STBC_SPEXP1",
	"MCS4_STBC_SPEXP1",
	"MCS5_STBC_SPEXP1",
	"MCS6_STBC_SPEXP1",
	"MCS7_STBC_SPEXP1",
	"VHT8SS1_STBC_SPEXP1",
	"VHT9SS1_STBC_SPEXP1",
	"MCS8_SPEXP1",
	"MCS9_SPEXP1",
	"MCS10_SPEXP1",
	"MCS11_SPEXP1",
	"MCS12_SPEXP1",
	"MCS13_SPEXP1",
	"MCS14_SPEXP1",
	"MCS15_SPEXP1",
	"VHT8SS2_SPEXP1",
	"VHT9SS2_SPEXP1",
	"MCS16",
	"MCS17",
	"MCS18",
	"MCS19",
	"MCS20",
	"MCS21",
	"MCS22",
	"MCS23",
	"VHT8SS3",
	"VHT9SS3",
	"OFDM6_TXBF1",
	"OFDM9_TXBF1",
	"OFDM12_TXBF1",
	"OFDM18_TXBF1",
	"OFDM24_TXBF1",
	"OFDM36_TXBF1",
	"OFDM48_TXBF1",
	"OFDM54_TXBF1",
	"MCS0_TXBF1",
	"MCS1_TXBF1",
	"MCS2_TXBF1",
	"MCS3_TXBF1",
	"MCS4_TXBF1",
	"MCS5_TXBF1",
	"MCS6_TXBF1",
	"MCS7_TXBF1",
	"VHT8SS1_TXBF1",
	"VHT9SS1_TXBF1",
	"MCS8_TXBF0",
	"MCS9_TXBF0",
	"MCS10_TXBF0",
	"MCS11_TXBF0",
	"MCS12_TXBF0",
	"MCS13_TXBF0",
	"MCS14_TXBF0",
	"MCS15_TXBF0",
	"OFDM6_TXBF2",
	"OFDM9_TXBF2",
	"OFDM12_TXBF2",
	"OFDM18_TXBF2",
	"OFDM24_TXBF2",
	"OFDM36_TXBF2",
	"OFDM48_TXBF2",
	"OFDM54_TXBF2",
	"MCS0_TXBF2",
	"MCS1_TXBF2",
	"MCS2_TXBF2",
	"MCS3_TXBF2",
	"MCS4_TXBF2",
	"MCS5_TXBF2",
	"MCS6_TXBF2",
	"MCS7_TXBF2",
	"VHT8SS1_TXBF2",
	"VHT9SS1_TXBF2",
	"MCS8_TXBF1",
	"MCS9_TXBF1",
	"MCS10_TXBF1",
	"MCS11_TXBF1",
	"MCS12_TXBF1",
	"MCS13_TXBF1",
	"MCS14_TXBF1",
	"MCS15_TXBF1",
	"VHT8SS2_TXBF1",
	"VHT9SS2_TXBF1",
	"MCS16_TXBF0",
	"MCS17_TXBF0",
	"MCS18_TXBF0",
	"MCS19_TXBF0",
	"MCS20_TXBF0",
	"MCS21_TXBF0",
	"MCS22_TXBF0",
	"MCS23_TXBF0",
};

int legacy_reg_rate_map[LEGACY_RATE_ID_MAX][LEGACY_MODE_ID_MAX] = {
	{DSSS1,  DSSS1_MULTI1,  DSSS1_MULTI2, NO_RATE,       NO_RATE},
	{DSSS2,  DSSS2_MULTI1,  DSSS2_MULTI2, NO_RATE,       NO_RATE},
	{DSSS5,  DSSS5_MULTI1,  DSSS5_MULTI2, NO_RATE,       NO_RATE},
	{DSSS11, DSSS11_MULTI1, DSSS11_MULTI2,NO_RATE,       NO_RATE},
	{OFDM6,  OFDM6_CDD1,    OFDM6_CDD2,   OFDM6_TXBF1,   OFDM6_TXBF2},
	{OFDM9,  OFDM9_CDD1,    OFDM9_CDD2,   OFDM9_TXBF1,   OFDM9_TXBF2},
	{OFDM12, OFDM12_CDD1,   OFDM12_CDD2,  OFDM12_TXBF1,  OFDM12_TXBF2},
	{OFDM18, OFDM18_CDD1,   OFDM18_CDD2,  OFDM18_TXBF1,  OFDM18_TXBF2},
	{OFDM24, OFDM24_CDD1,   OFDM24_CDD2,  OFDM24_TXBF1,  OFDM24_TXBF2},
	{OFDM36, OFDM36_CDD1,   OFDM36_CDD2,  OFDM36_TXBF1,  OFDM36_TXBF2},
	{OFDM48, OFDM48_CDD1,   OFDM48_CDD2,  OFDM48_TXBF1,  OFDM48_TXBF2},
	{OFDM54, OFDM54_CDD1,   OFDM54_CDD2,  OFDM54_TXBF1,  OFDM54_TXBF2}
};

int vht_ss1_reg_rate_map[VHT_RATE_INDEX_MAX][VHT_SS1_MODE_ID_MAX] = {
	{MCS0,    MCS0_CDD1,    MCS0_STBC,    MCS0_CDD2,    MCS0_STBC_SPEXP1,  MCS0_TXBF1,  MCS0_TXBF2},
	{MCS1,    MCS1_CDD1,    MCS1_STBC,    MCS1_CDD2,    MCS1_STBC_SPEXP1,  MCS1_TXBF1,  MCS1_TXBF2},
	{MCS2,    MCS2_CDD1,    MCS2_STBC,    MCS2_CDD2,    MCS2_STBC_SPEXP1,  MCS2_TXBF1,  MCS2_TXBF2},
	{MCS3,    MCS3_CDD1,    MCS3_STBC,    MCS3_CDD2,    MCS3_STBC_SPEXP1,  MCS3_TXBF1,  MCS3_TXBF2},
	{MCS4,    MCS4_CDD1,    MCS4_STBC,    MCS4_CDD2,    MCS4_STBC_SPEXP1,  MCS4_TXBF1,  MCS4_TXBF2},
	{MCS5,    MCS5_CDD1,    MCS5_STBC,    MCS5_CDD2,    MCS5_STBC_SPEXP1,  MCS5_TXBF1,  MCS5_TXBF2},
	{MCS6,    MCS6_CDD1,    MCS6_STBC,    MCS6_CDD2,    MCS6_STBC_SPEXP1,  MCS6_TXBF1,  MCS6_TXBF2},
	{MCS7,    MCS7_CDD1,    MCS7_STBC,    MCS7_CDD2,    MCS7_STBC_SPEXP1,  MCS7_TXBF1,  MCS7_TXBF2},
	{VHT8SS1, VHT8SS1_CDD1, VHT8SS1_STBC, VHT8SS1_CDD2, VHT8SS1_STBC_SPEXP1,VHT8SS1_TXBF1, VHT8SS1_TXBF2},
	{VHT9SS1, VHT9SS1_CDD1, VHT9SS1_STBC, VHT9SS1_CDD2, VHT9SS1_STBC_SPEXP1,VHT9SS1_TXBF1, VHT9SS1_TXBF2}
};

int vht_ss2_reg_rate_map[VHT_RATE_INDEX_MAX][VHT_SS2_MODE_ID_MAX] = {
	{MCS8,    MCS8_SPEXP1,   MCS8_TXBF0,     MCS8_TXBF1},
	{MCS9,    MCS9_SPEXP1,   MCS9_TXBF0,     MCS9_TXBF1},
	{MCS10,   MCS10_SPEXP1,  MCS10_TXBF0,    MCS10_TXBF1},
	{MCS11,   MCS11_SPEXP1,  MCS11_TXBF0,    MCS11_TXBF1},
	{MCS12,   MCS12_SPEXP1,  MCS12_TXBF0,    MCS12_TXBF1},
	{MCS13,   MCS13_SPEXP1,  MCS13_TXBF0,    MCS13_TXBF1},
	{MCS14,   MCS14_SPEXP1,  MCS14_TXBF0,    MCS14_TXBF1},
	{MCS15,   MCS15_SPEXP1,  MCS15_TXBF0,    MCS15_TXBF1},
	{VHT8SS2, VHT8SS2_SPEXP1,NO_RATE,        VHT8SS2_TXBF1},
	{VHT9SS2, VHT9SS2_SPEXP1,NO_RATE,        VHT8SS2_TXBF1}
};

int vht_ss3_reg_rate_map[VHT_RATE_INDEX_MAX][VHT_SS3_MODE_ID_MAX] = {
	{MCS16,      MCS16_TXBF0},
	{MCS17,      MCS17_TXBF0},
	{MCS18,      MCS18_TXBF0},
	{MCS19,      MCS19_TXBF0},
	{MCS20,      MCS20_TXBF0},
	{MCS21,      MCS21_TXBF0},
	{MCS22,      MCS22_TXBF0},
	{MCS23,      MCS23_TXBF0},
	{VHT8SS3,    NO_RATE},
	{VHT9SS3,    NO_RATE}
};

const char *exp_mode_name[EXP_MODE_ID_MAX] = {
	"CDD",
	"STBC",
	"TXBF"
};

const char *
get_clm_rate_group_label(int rategroup)
{
	return clm_rate_group_labels[rategroup];
}
#ifdef D11AC_IOTYPES
const char *
get_reg_rate_string_from_ratespec(int ratespec)
{
	reg_rate_index_t index = get_reg_rate_index_from_ratespec(ratespec);

	if (index >= 0)
	{
		return clm_rate_labels[index];
	}
	return CLM_NO_RATE_STRING;
}

reg_rate_index_t
get_reg_rate_index_from_ratespec(int ratespec)
{
	uint encode, rate, txexp;
	bool stbc,txbf;
	int rate_index = NO_RATE;
	exp_mode_t expmode = EXP_MODE_ID_DEF;

	/* If auto is set, we don't get a ratespec that can be decoded */
	if (ratespec == AUTO_RATESPEC)
		return rate_index;

	encode = (ratespec & WL_RSPEC_ENCODING_MASK);
	rate   = (ratespec & WL_RSPEC_RATE_MASK);
	txexp  = (ratespec & WL_RSPEC_TXEXP_MASK) >> WL_RSPEC_TXEXP_SHIFT;
	stbc   = (ratespec & WL_RSPEC_STBC) != 0;
	txbf   = (ratespec & WL_RSPEC_TXBF) != 0;

	if (stbc && txbf) {
		return rate_index;
	} else if (txbf) {
		expmode = EXP_MODE_ID_TXBF;
	} else if (stbc) {
		expmode = EXP_MODE_ID_STBC;
	}

	if (encode == WL_RSPEC_ENCODE_RATE) {
		rate_index = get_legacy_reg_rate_index(rate, txexp, expmode);
	} else if (encode == WL_RSPEC_ENCODE_HT) {
		rate_index = get_ht_reg_rate_index(rate, txexp, expmode);
	} else if (encode == WL_RSPEC_ENCODE_VHT) {
		uint mcs = (ratespec & WL_RSPEC_VHT_MCS_MASK);
		uint nss = (ratespec & WL_RSPEC_VHT_NSS_MASK) >> WL_RSPEC_VHT_NSS_SHIFT;
		rate_index = get_vht_reg_rate_index(mcs, nss, txexp, expmode);
	}

	return rate_index;
}

reg_rate_index_t
get_legacy_reg_rate_index(int rate_hmhz, int tx_expansion, exp_mode_t expmode)
{
	reg_rate_index_t index = NO_RATE;
	int rate_id;
	rate_id = get_legacy_rate_identifier(rate_hmhz);
	if (rate_id == LEGACY_RATE_ID_NO_RATE || (expmode == EXP_MODE_ID_TXBF && rate_id < LEGACY_RATE_ID_6MHZ))
	{
		fprintf(stderr, "ERROR: Bad legacy rate spec: %d\n", rate_hmhz);
	}
	else
	{
		index = legacy_reg_rate_map[rate_id][get_legacy_mode_identifier(tx_expansion, expmode)];
	}
	return index;
}


static int get_legacy_rate_identifier(int rate_hmhz)
{
	int rate_lut[LEGACY_RATE_ID_MAX];
	int rate_index = LEGACY_RATE_ID_NO_RATE;
	int i;

	rate_lut[LEGACY_RATE_ID_1MHZ]   = 1 * MHZ_TO_HALF_MHZ;
	rate_lut[LEGACY_RATE_ID_2MHZ]   = 2 * MHZ_TO_HALF_MHZ;
	rate_lut[LEGACY_RATE_ID_5_5MHZ] = 11; /* 5.5 * MHZ_TO_HALF_MHZ */
	rate_lut[LEGACY_RATE_ID_11MHZ]  = 11 * MHZ_TO_HALF_MHZ;
	rate_lut[LEGACY_RATE_ID_6MHZ]   = 6 * MHZ_TO_HALF_MHZ;
	rate_lut[LEGACY_RATE_ID_9MHZ]   = 9 * MHZ_TO_HALF_MHZ;
	rate_lut[LEGACY_RATE_ID_12MHZ]  = 12 * MHZ_TO_HALF_MHZ;
	rate_lut[LEGACY_RATE_ID_18MHZ]  = 18 * MHZ_TO_HALF_MHZ;
	rate_lut[LEGACY_RATE_ID_24MHZ]  = 24 * MHZ_TO_HALF_MHZ;
	rate_lut[LEGACY_RATE_ID_36MHZ]  = 36 * MHZ_TO_HALF_MHZ;
	rate_lut[LEGACY_RATE_ID_48MHZ]  = 48 * MHZ_TO_HALF_MHZ;
	rate_lut[LEGACY_RATE_ID_54MHZ]  = 54 * MHZ_TO_HALF_MHZ;

	for (i = 0; i < LEGACY_RATE_ID_MAX; i++)
	{
		if (rate_lut[i] == rate_hmhz)
		{
			rate_index = i;
			break;
		}
	}

	return rate_index;
}

static int get_legacy_mode_identifier(int tx_expansion, exp_mode_t expmode)
{
	int mode_identifier = 0;

	if (tx_expansion == 0)
	{
		mode_identifier = LEGACY_MODE_ID_NONE;
	}
	else if (tx_expansion == 1)
	{
		switch (expmode)
		{
		case EXP_MODE_ID_DEF:
			mode_identifier = LEGACY_MODE_ID_TXEXP1;
			break;
		case EXP_MODE_ID_TXBF:
			mode_identifier = LEGACY_MODE_ID_TXBF1;
			break;
		default:
			fprintf(stderr, "ERROR: Bad legacy tx_expansion spec: %d expansion mode %s\n",
				 tx_expansion, get_mode_name(expmode));
			break;
		}
	}
	else if (tx_expansion == 2)
	{
		switch (expmode)
		{
		case EXP_MODE_ID_DEF:
			mode_identifier = LEGACY_MODE_ID_TXEXP2;
			break;
		case EXP_MODE_ID_TXBF:
			mode_identifier = LEGACY_MODE_ID_TXBF2;
			break;
		default:
			fprintf(stderr, "ERROR: Bad legacy tx_expansion spec: %d expansion mode %s\n",
				 tx_expansion, get_mode_name(expmode));
			break;
		}
	}
	else
	{
		fprintf(stderr, "ERROR: Bad legacy tx_expansion spec: %d expansion mode %s\n", tx_expansion,
				get_mode_name(expmode));
	}

	return mode_identifier;
}

reg_rate_index_t
get_ht_reg_rate_index(int mcs, int tx_expansion, exp_mode_t expmode)
{
	reg_rate_index_t rate_index = NO_RATE;
	int vht_mcs, nss;

	if (mcs > 23)
	{
		fprintf(stderr, "ERROR: Bad ht mcs spec: %d\n", mcs);
	}
	else
	{
		vht_mcs = mcs % 8;
		nss = (mcs / 8) + 1;

		rate_index = get_vht_reg_rate_index(vht_mcs, nss, tx_expansion, expmode);
	}

	return rate_index;
}

reg_rate_index_t
get_vht_reg_rate_index(int mcs, int nss, int tx_expansion, exp_mode_t expmode)
{
	reg_rate_index_t rate_index = NO_RATE;
	int rate_id = get_vht_rate_identifier(mcs);

	if (nss == 1)
	{
		rate_index = vht_ss1_reg_rate_map[rate_id][get_vht_ss1_mode_identifier(tx_expansion, expmode)];
	}
	else if (nss == 2)
	{
		rate_index = vht_ss2_reg_rate_map[rate_id][get_vht_ss2_mode_identifier(tx_expansion, expmode)];
	}
	else if (nss == 3)
	{
		rate_index = vht_ss3_reg_rate_map[rate_id][get_vht_ss3_mode_identifier(tx_expansion, expmode)];
	}

	return rate_index;
}

static int get_vht_rate_identifier(int vht_mcs_index)
{
	int rate_index = 0;

	if (vht_mcs_index >= VHT_RATE_INDEX_0 && vht_mcs_index < VHT_RATE_INDEX_MAX)
	{
		rate_index = vht_mcs_index;
	}
	else
	{
		fprintf(stderr, "ERROR: Bad vht mcs spec: %d\n", vht_mcs_index);
	}
	return rate_index;
}

static int get_vht_ss1_mode_identifier(int tx_expansion, exp_mode_t expmode)
{
	int mode_identifier = 0;

	if (tx_expansion == 0)
	{
		switch (expmode)
		{
			case EXP_MODE_ID_DEF:
				mode_identifier = VHT_SS1_MODE_ID_NONE;
				break;
			case EXP_MODE_ID_STBC:
				mode_identifier = VHT_SS1_MODE_ID_STBC;
				break;
			default:
				fprintf(stderr, "ERROR: Bad vht ss1 mode: %d, expansion mode %s\n", tx_expansion,
						get_mode_name(expmode));
				break;
		}
	}
	else if (tx_expansion == 1)
	{
		switch (expmode)
		{
			case EXP_MODE_ID_DEF:
				mode_identifier = VHT_SS1_MODE_ID_CDD1;
				break;
			case EXP_MODE_ID_STBC:
				mode_identifier = VHT_SS1_MODE_ID_STBC_SPEXP1;
				break;
			case EXP_MODE_ID_TXBF:
				mode_identifier = VHT_SS1_MODE_ID_TXBF1;
				break;
			default:
				fprintf(stderr, "ERROR: Bad vht ss1 mode: %d, expansion mode %s\n", tx_expansion,
						get_mode_name(expmode));
				break;
		}
	}
	else if (tx_expansion == 2)
	{
		switch (expmode)
		{
			case EXP_MODE_ID_DEF:
				mode_identifier = VHT_SS1_MODE_ID_CDD2;
				break;
			case EXP_MODE_ID_TXBF:
				mode_identifier = VHT_SS1_MODE_ID_TXBF2;
				break;
			default:
				fprintf(stderr, "ERROR: Bad vht ss1 mode: %d, expansion mode %s\n", tx_expansion,
						get_mode_name(expmode));
				break;
		}
	}
	else
	{
		fprintf(stderr, "ERROR: Bad vht ss1 mode: %d, expansion mode: %s\n", tx_expansion,
				 get_mode_name(expmode));
	}

	return mode_identifier;
}

static int get_vht_ss2_mode_identifier(int tx_expansion, exp_mode_t expmode)
{
	int mode_identifier = 0;

	if (tx_expansion == 0)
	{
		switch (expmode)
		{
		case EXP_MODE_ID_DEF:
			mode_identifier = VHT_SS2_MODE_ID_NONE;
			break;
		case EXP_MODE_ID_TXBF:
			mode_identifier = VHT_SS2_MODE_ID_TXBF0;
			break;
		default:
			fprintf(stderr, "ERROR: Bad vht ss2 mode: %d, expansion mode %s\n", tx_expansion,
				get_mode_name(expmode));
			break;
		}
	}
	else if (tx_expansion == 1)
	{
		switch (expmode)
		{
		case EXP_MODE_ID_DEF:
			mode_identifier = VHT_SS2_MODE_ID_SPEXP1;
			break;
		case EXP_MODE_ID_TXBF:
			mode_identifier = VHT_SS2_MODE_ID_TXBF1;
			break;
		default:
			fprintf(stderr, "ERROR: Bad vht ss2 mode: %d, expansion mode %s\n", tx_expansion,
				  get_mode_name(expmode));
			break;
		}
	}
	else
	{
		fprintf(stderr, "ERROR: Bad vht ss2 mode: %d expansion mode: %s\n", tx_expansion,
			   get_mode_name(expmode));
	}

	return mode_identifier;
}

static int get_vht_ss3_mode_identifier(int tx_expansion, exp_mode_t expmode)
{
	int mode_identifier = 0;

	if (tx_expansion == 0)
	{
		switch (expmode)
		{
		case EXP_MODE_ID_DEF:
			mode_identifier = VHT_SS3_MODE_ID_NONE;
			break;
		case EXP_MODE_ID_TXBF:
			mode_identifier = VHT_SS3_MODE_ID_TXBF0;
			break;
		default:
			fprintf(stderr, "ERROR: Bad vht ss3 mode: %d, expansion mode: %s\n", tx_expansion
					, get_mode_name(expmode));
			break;
		}
	}
	else
	{
		fprintf(stderr, "ERROR: Bad vht ss3 mode: %d, expansion: %s\n", tx_expansion,
			   get_mode_name(expmode));
	}

	return mode_identifier;
}

static const char *get_mode_name(exp_mode_t mode)
{
	if (mode >= EXP_MODE_ID_MAX)
		return EXP_MODE_UNKNOWN;

	return exp_mode_name[mode];
}
#endif

