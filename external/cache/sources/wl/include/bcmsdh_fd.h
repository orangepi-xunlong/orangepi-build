/*
 *  BCMSDH Function Driver for SDIO-Linux
 *
 * Copyright (C) 1999-2012, Broadcom Corporation
 * 
 *      Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed to you
 * under the terms of the GNU General Public License version 2 (the "GPL"),
 * available at http://www.broadcom.com/licenses/GPLv2.php, with the
 * following added to such license:
 * 
 *      As a special exception, the copyright holders of this software give you
 * permission to link this software with independent modules, and to copy and
 * distribute the resulting executable under terms of your choice, provided that
 * you also meet, for each linked independent module, the terms and conditions of
 * the license of that module.  An independent module is a module which is not
 * derived from this software.  The special exception does not apply to any
 * modifications of the software.
 * 
 *      Notwithstanding the above, under no circumstances may you combine this
 * software in any way with any other Broadcom software provided under a license
 * other than the GPL, without Broadcom's express prior written consent.
 *
 * $Id: bcmsdh_fd.h 241182 2011-02-17 21:50:03Z gmo $
 */

#ifndef __BCMSDH_FD_H__
#define __BCMSDH_FD_H__

#ifdef BCMDBG
#define sd_err(x)	do { if (sd_msglevel & SDH_ERROR_VAL) printf x; } while (0)
#define sd_trace(x)	do { if (sd_msglevel & SDH_TRACE_VAL) printf x; } while (0)
#define sd_info(x)	do { if (sd_msglevel & SDH_INFO_VAL)  printf x; } while (0)
#define sd_debug(x)	do { if (sd_msglevel & SDH_DEBUG_VAL) printf x; } while (0)
#define sd_data(x)	do { if (sd_msglevel & SDH_DATA_VAL)  printf x; } while (0)
#define sd_ctrl(x)	do { if (sd_msglevel & SDH_CTRL_VAL)  printf x; } while (0)
#else
#define sd_err(x)
#define sd_trace(x)
#define sd_info(x)
#define sd_debug(x)
#define sd_data(x)
#define sd_ctrl(x)
#endif

#define sd_sync_dma(sd, read, nbytes)
#define sd_init_dma(sd)
#define sd_ack_intr(sd)
#define sd_wakeup(sd);
/* Allocate/init/free per-OS private data */
extern int sdiohfd_osinit(sdioh_info_t *sd);
extern void sdiohfd_osfree(sdioh_info_t *sd);

#ifdef BCMPERFSTATS
#define sd_log(x)	do { if (sd_msglevel & SDH_LOG_VAL)	 bcmlog x; } while (0)
#else
#define sd_log(x)
#endif

#define SDIOH_ASSERT(exp) \
	do { if (!(exp)) \
		printf("!!!ASSERT fail: file %s lines %d", __FILE__, __LINE__); \
	} while (0)

#define BLOCK_SIZE_4318 64
#define BLOCK_SIZE_4328 512

/* internal return code */
#define SUCCESS	0
#define ERROR	1

/* private bus modes */
#define SDIOH_MODE_SD4		2
#define CLIENT_INTR 		0x100	/* Get rid of this! */
#define DMA_MAX_LEN			(7*1024)

struct sdioh_info {
	osl_t 		*osh;			/* osh handler */
	bool		client_intr_enabled;	/* interrupt connnected flag */
	bool		intr_handler_valid;	/* client driver interrupt handler valid */
	sdioh_cb_fn_t	intr_handler;		/* registered interrupt handler */
	void		*intr_handler_arg;	/* argument to call interrupt handler */
	uint16		intmask;		/* Current active interrupts */
	void		*sdos_info;		/* Pointer to per-OS private data */

	uint 		irq;			/* Client irq */
	int 		intrcount;		/* Client interrupts */

	bool		sd_use_dma;		/* DMA on CMD53 */
	bool 		sd_blockmode;		/* sd_blockmode == FALSE => 64 Byte Cmd 53s. */
						/*  Must be on for sd_multiblock to be effective */
	bool 		use_client_ints;	/* If this is false, make sure to restore */
	int 		sd_mode;		/* SD1/SD4/SPI */
	int 		client_block_size[SDIOD_MAX_IOFUNCS];		/* Blocksize */
	uint8 		num_funcs;		/* Supported funcs on client */
	uint32 		com_cis_ptr;
	uint32 		func_cis_ptr[SDIOD_MAX_IOFUNCS];
	uint		max_dma_len;
	uint		max_dma_descriptors;	/* DMA Descriptors supported by this controller. */
	SDDMA_DESCRIPTOR	SGList[32];	/* Scatter/Gather DMA List */
};

/************************************************************
 * Internal interfaces: per-port references into bcmsdiohfd.c
 */

/* Global message bits */
extern uint sd_msglevel;

/* OS-independent interrupt handler */
extern bool check_client_intr(sdioh_info_t *sd);

/* Core interrupt enable/disable of device interrupts */
extern void sdiohfd_devintr_on(sdioh_info_t *sd);
extern void sdiohfd_devintr_off(sdioh_info_t *sd);


/**************************************************************
 * Internal interfaces: bcmsdiohfd.c references to per-port code
 */

/* Register mapping routines */
extern uint32 *sdiohfd_reg_map(osl_t *osh, int32 addr, int size);
extern void sdiohfd_reg_unmap(osl_t *osh, int32 addr, int size);

/* Interrupt (de)registration routines */
extern int sdiohfd_register_irq(sdioh_info_t *sd, uint irq);
extern void sdiohfd_free_irq(uint irq, sdioh_info_t *sd);

typedef struct _BCMSDH_FD_INSTANCE {
    PSDDEVICE	pDevice[3];  /* bus driver's device we are supporting */
	sdioh_info_t	*sd;
    SDIO_STATUS		LastRequestStatus;  /* last request status */
} BCMSDH_FD_INSTANCE, *PBCMSDH_FD_INSTANCE;

typedef struct _BCMSDH_FD_CONTEXT {
    SDFUNCTION      Function;       /* function description for bus driver */
	PBCMSDH_FD_INSTANCE pInstance;
    SD_BUSCLOCK_RATE ClockOverride;  /* clock rate override */
} BCMSDH_FD_CONTEXT, *PBCMSDH_FD_CONTEXT;

/* prototypes */
SDIO_STATUS client_init(PBCMSDH_FD_CONTEXT  pFuncContext,
                               PBCMSDH_FD_INSTANCE pInstance,
                               PSDDEVICE                 pDevice);
void client_detach(PBCMSDH_FD_CONTEXT   pFuncContext,
                          PBCMSDH_FD_INSTANCE  pInstance);

#endif /* __BCMSDH_FD_H__ */
