/*
 * Header file for save-restore functionality in driver
 *
 * Copyright (C) 2012, Broadcom Corporation
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 *
 * $Id: Exp $
 */

#ifndef _SAVERESTORE_H
#define _SAVERESTORE_H

/* WL_ENAB_RUNTIME_CHECK may be set based upon the #define below (for ROM builds). It may also
 * be defined via makefiles (e.g. ROM auto abandon unoptimized compiles).
 */
#if defined(BCMROMBUILD)
	#ifndef WL_ENAB_RUNTIME_CHECK
		#define WL_ENAB_RUNTIME_CHECK
	#endif
#endif /* BCMROMBUILD */

/* SAVERESTORE Support */
#ifdef SAVERESTORE
	extern bool _sr;
	#if defined(WL_ENAB_RUNTIME_CHECK) || !defined(DONGLEBUILD)
		#define SR_ENAB() (_sr)
	#elif defined(SAVERESTORE_DISABLED)
		#define SR_ENAB()	(0)
	#else
		#define SR_ENAB()	(1)
	#endif
#else
	#define SR_ENAB() 		(0)
#endif /* SAVERESTORE */

/* BANK size is calculated in the units of 32bit WORDS */
#define SRCTL_BANK_SIZE(sr_cntrl) ((((sr_cntrl & 0x7F0) >> 4) + 1) << 8)
#define SRCTL_BANK_NUM(sr_cntrl) (sr_cntrl & 0xF)
#define SRCTL_EXP_MEM_SIZE(chipid) (chipid == BCM43239_CHIP_ID ? (24 << 8) : (48 << 8))

#define SR_HOST 0
#define SR_ENGINE 1

#ifdef SAVERESTORE
typedef enum {
	SR_HSIC_OOB_SHALLOW_WAKE_MODE = 0,
	SR_HSIC_OOB_SHALLOW_SLEEP_MODE
} hsic_sr_module_t;

bool hsic_sr_save(void *arg);
#endif /* defined(SAVERESTORE) */

extern CONST uint32 sr_source_code[];
extern CONST uint sr_source_codesz;

typedef bool (*sr_save_callback_t)(void* arg);
typedef void (*sr_restore_callback_t)(void* arg);

/* Function prototypes */
void sr_download_firmware(si_t *si_h);
int sr_engine_enable(si_t *si_h, bool oper, bool enable);
int sr_pll_toggle(si_t *si_h, bool enable);
int sr_update_srfast_dependency(si_t *sih, bool enable);
uint32 sr_chipcontrol(si_t *si_h, uint32 mask, uint32 val);
void sr_save_restore_init(si_t *si_h);
uint32 sr_mem_access(si_t *sih, int op, uint32 addr, uint32 data);

uint32 sr_register_save(si_t *sih, sr_save_callback_t cb, void *arg);
uint32 sr_register_restore(si_t *sih, sr_restore_callback_t cb, void *arg);
void sr_process_save(si_t *sih);
void sr_process_restore(si_t *sih);
int sr_gpio_oobwake(si_t *si_h, bool enable);
int sr_uart_oobwake(si_t *si_h, bool enable);
int sr_uart_oobwake_irq_ack(si_t *sih);
void sr_wakeup_workaround(si_t *sih);
uint32 sr_get_cur_minresmask(si_t *sih);

#ifdef SAVERESTORE
bool sr_cap(si_t *sih);
bool sr_is_wakeup_from_deep_sleep(void);
bool sr_is_wakeup_from_deep_sleep_bit28_check(void);
void sr_wokeup_from_deep_sleep(bool state);
void sr_force_deep_sleep_bit28_check(bool state);
#else
#define sr_cap(a)	(FALSE)
#define sr_is_wakeup_from_deep_sleep() (FALSE)
#define sr_is_wakeup_from_deep_sleep_bit28_check() (FALSE)
#define sr_wokeup_from_deep_sleep(state) (FALSE)
#define sr_force_deep_sleep_bit28_check(state) (FALSE)
#endif

#endif /* _SAVERESTORE_H */
