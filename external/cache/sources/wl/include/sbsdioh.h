/*
 * BRCM SDIOH (host controller) core hardware definitions.
 *
 * SDIOH support 1bit, 4 bit SDIO mode as well as SPI mode.
 *
 * $Id: sbsdioh.h 241182 2011-02-17 21:50:03Z gmo $
 * Copyright(c) 2003 Broadcom Corporation
 */

#ifndef	_SBSDIOH_H
#define	_SBSDIOH_H

/* cpp contortions to concatenate w/arg prescan */
#ifndef PAD
#define	_PADLINE(line)	pad ## line
#define	_XSTR(line)	_PADLINE(line)
#define	PAD		_XSTR(__LINE__)
#endif	/* PAD */

typedef volatile struct {
	uint32 devcontrol;	/* device control */
	uint32 PAD[15];		/* PADDING old codec registers */

	/* sdioh registers */
	uint32 mode;		/* 0x40: sd mode: SDIO1/SDIO4/SPI */
	uint32 delay;		/* 0x44: various clock config */
	uint32 rdto;		/* 0x48: read timeout value before SDERROR.ReadTimeOut
				 * is generated
				 */
	uint32 rbto;		/* 0x4c: response busy timeout value before SDERROR.BusyTimeOut
				 * is generated
				 */
	uint32 test;		/* 0x50: test register to force bad CRC */
	uint32 arvm;		/* 0x54: auto response value and mask */

	uint32 error;		/* 0x58: Error */
	uint32 errormask;	/* 0x5c: Error mask */
	uint32 cmddat;		/* 0x60: SDIO CMD data */
	uint32 cmdl;		/* 0x64: SDIO CMD argument */
	uint32 fifodata;	/* 0x68: SDIO fifo data, little endian, changable
				 * in mode register
				 */
	uint32 respq;		/* 0x6c: response queue */

	uint32 ct_cmddat;	/* 0x70: SDIO CMD data for cutthrough commands */
	uint32 ct_cmdl;		/* 0x74: SDIO CMD argument for cutthrough commands */
	uint32 ct_fifodata;	/* 0x78: SDIO fifo data for cutthrough commands,
				 * little endian only
				 */
	uint32 PAD;

	uint32 ap_cmddat;	/* 0x80: SDIO CMD data for append commands */
	uint32 ap_cmdl;		/* 0x84: SDIO CMD argument for append commands */
	uint32 ap_fifodata;	/* 0x88: SDIO fifo data for append commands, little endian only */
	uint32 PAD;

	uint32 intstatus;	/* 0x90: interrupt status */
	uint32 intmask;		/* 0x94: interrupt mask */
	uint32 PAD;

	uint32 debuginfo;	/* 0x9c: debug register */
	uint32 fifoctl;		/* 0xa0: fifo control */
	uint32 blocksize;	/* 0xa4: cmd53 block mode block size */

	uint32 PAD[86];

	dma32regp_t dmaregs;	/* 0x200 - 0x21C */

} sdioh_regs_t;

/* devcontrol */
#define CODEC_DEVCTRL_SDIOH	0x4000		/* 1: config codec to SDIOH mode,
						 * 0: normal codec mode
						 */

/* mode */
#define MODE_DONT_WAIT_DMA	0x2000		/* diag only: in receive DMA mode, Cmdatadone
						 * will be generated when data transfer done
						 */
#define MODE_BIG_ENDIAN		0x1000		/* 1: big endian, 0: small endian */
#define MODE_STOP_ALL_CLK	0x0800		/* diag only: 1: stop all SD clock */
#define MODE_PRECMD_CNT_EN	0x0400		/* 1: enable precmd count,
						 * 0: disable precmd count
						 */
#define	MODE_CLK_OUT_EN		0x0200		/* 0/1: en/disable the clock output to sdio bus */
#define	MODE_USE_EXT_CLK	0x0100		/* use external clock or not */
#define	MODE_CLK_DIV_MASK	0x00f0		/* divide host clock by 2*this field */
#define	MODE_OP_MASK		0x000f		/* mode is [3:0] bits */
#define	MODE_OP_SDIO4BIT	2		/* SDIO 4 bit mode */
#define	MODE_OP_SDIO1BIT	1		/* SDIO 1 bit mode */
#define	MODE_OP_SPI		0		/* SPI mode */
#define	MODE_HIGHSPEED_EN	0x10000		/* Enable High-Speed clocking Mode. */

/* delay */
#define DLY_CLK_COUNT_PRE_M	0x0000ffff	/* dynamic clock: pre clock on cycles before tx */
#define DLY_CLK_COUNT_PRE_O	0
#define DLY_TX_START_COUNT_M	0xffff0000	/* DMA mode only: wait cycle before transferring
						 * non-empty fifo
						 */
#define DLY_TX_START_COUNT_O	23

/* test */
#define TEST_BAD_CMD_CRC	0x1		/* force bad CMD crc */
#define TEST_BAD_DAT_CRC	0x2		/* force bad DAT crc */

/* arvm */
#define	AR_MASK_OFT		8		/* CMD53 auto response mask */
#define	AR_VAL			0x00ff		/* CMD53 expected value of auto response,
						 * after mask
						 */

/* cmd dat */
#define CMDAT_INDEX_M		0x0000003f	/* command index */
#define CMDAT_EXP_RSPTYPE_M	0x000001c0	/* expected response type */
#define CMDAT_EXP_RSPTYPE_O	6
#define CMDAT_DAT_EN_M		0x00000200	/* data command flag */
#define CMDAT_DAT_EN_O		9
#define CMDAT_DAT_WR_M		0x00000400	/* 0: read from SD device,
						 * 1: write to SD device
						 */
#define CMDAT_DAT_WR_O		10
#define CMDAT_DMA_MODE_M	0x00000800	/* 0: pio, 1: dma */
#define CMDAT_DMA_MODE_O	11
#define CMDAT_ARC_EN_M		0x00001000	/* auto response check enable/disable */
#define CMDAT_ARC_EN_O		12
#define CMDAT_EXP_BUSY_M	0x00002000	/* R1b only: expect busy after response */
#define CMDAT_EXP_BUSY_O	13
#define CMDAT_NO_RSP_CRC_CHK_M	0x00004000	/* disable response crc checking */
#define CMDAT_NO_RSP_CRC_CHK_O	14
#define CMDAT_NO_RSP_CDX_CHK_M	0x00008000	/* disable response command index checking */
#define CMDAT_NO_RSP_CDX_CHK_O	15
#define CMDAT_DAT_TX_CNT_M	0x1fff0000	/* total number of bytes to transfer */
#define CMDAT_DAT_TX_CNT_O	16
#define CMDAT_DATLEN_PIO	64		/* data length limit for pio mode */
#define CMDAT_DATLEN_DMA_NON53	512		/* data length limit for DMA mode non cmd53 */
#define CMDAT_DATLEN_DMA_53	8096		/* data length limit for DMA mode cmd53 */
#define CMDAT_APPEND_EN_M	0x20000000	/* enable sdioh to append a command */
#define CMDAT_APPEND_EN_O	29
#define CMDAT_ABORT_M		0x40000000	/* abort data */
#define CMDAT_ABORT_O		30
#define CMDAT_BLK_EN_M		0x80000000	/* use block mode */
#define CMDAT_BLK_EN_O		31

/* error and error_mask */
#define ERROR_RSP_CRC		0x0001		/* response crc error */
#define ERROR_RSP_TIME		0x0002		/* response time error */
#define ERROR_RSP_DBIT		0x0004		/* response D bit error */
#define ERROR_RSP_EBIT		0x0008		/* response E bit error */
#define ERROR_DAT_CRC		0x0010		/* data r/w crc error */
#define ERROR_DAT_SBIT		0x0020		/* receive data START bir error */
#define ERROR_DAT_EBIT		0x0040		/* receive data END bit error */
#define ERROR_DAT_RSP_S		0x0080		/* data crc response START bit error */
#define ERROR_DAT_RSP_E		0x0100		/* data crc response END bit error */
#define ERROR_DAT_RSP_UNKNOWN	0x0200		/* data response unknown, not 101 or 010 */
#define ERROR_DAT_RSP_TURNARD	0x0400		/* no 2 turnaround cycle between WRITE and
						 * CRC reponse
						 */
#define ERROR_DAT_READ_TO	0x0800		/* data read timeout */
#define ERROR_SPI_TOKEN_UNK	0x1000		/* SPI token unknown */
#define ERROR_SPI_TOKEN_BAD	0x2000		/* SPI error token received */
#define ERROR_SPI_ET_OUTRANGE	0x4000		/* SPI error token: out of range */
#define ERROR_SPI_ET_ECC	0x8000		/* SPI error token: ECC failed */
#define ERROR_SPI_ET_CC		0x010000	/* SPI error token: cc error */
#define ERROR_SPI_ET_ERR	0x020000	/* SPI error token: error */
#define ERROR_AUTO_RSP_CHK	0x040000	/* auto response check error */
#define ERROR_RSP_BUSY_TO	0x080000	/* busy timeout for RBTO */
#define ERROR_RSP_CMDIDX_BAD	0x100000	/* response command index error */

/* intstatus and intmask */
#define INT_CMD_DAT_DONE	0x0001		/* sticky, sdio command/data xfer done */
#define INT_HOST_BUSY		0x0002		/* host busy */
#define INT_DEV_INT		0x0004		/* sdio card interrupt recieved */
#define INT_ERROR_SUM		0x0008		/* logic OR of Error register masked by ErrorMask */
#define INT_CARD_INS		0x0010		/* card inserted */
#define INT_CARD_GONE		0x0020		/* card removed */
#define INT_CMDBUSY_CUTTHRU	0x0040		/* sdioh is busy writing to cmdl_cutthru register */
#define INT_CMDBUSY_APPEND	0x0080		/* this bit is clear when writing cmdl,
						 * and set when APPEND starts
						 */
#define INT_CARD_PRESENT	0x0100		/* card is present */
#define INT_STD_PCI_DESC	0x0400		/* standard DMA engine definition */
#define INT_STD_PCI_DATA	0x0800		/* standard DMA engine definition */
#define INT_STD_DESC_ERR	0x1000		/* standard DMA engine definition */
#define INT_STD_RCV_DESC_UF	0x2000		/* standard DMA engine definition */
#define INT_STD_RCV_FIFO_OF	0x4000		/* standard DMA engine definition */
#define INT_STD_XMT_FIFO_UF	0x8000		/* standard DMA engine definition */
#define INT_RCV_INT		0x00010000		/* standard DMA engine definition */
#define INT_XMT_INT		0x01000000		/* standard DMA engine definition */

/* debuginfo */
#define DBGI_REMAIN_COUNT	0x00001fff	/* remaining count for data comand,
						 * change on the fly
						 */
#define DBGI_CUR_ADDR		0xCfffE000	/* current address of CDM53 */
#define DBGI_CARD_WASBUSY	0x40000000	/* receive card busy signal on data line */
#define DBGI_R1B_DETECTED	0x80000000	/* R1B detected, overwritten by next cmd's status */

/* fifoctl(rcv/xmt) */
#define FIFO_RCV_BUF_RDY	0x10		/* HW set 1 when data are ready and avaiable in
						 * FIFO, write 1 before read RCVFIFODATA
						 */
#define FIFO_XMT_BYTE_VALID	0x0f		/* which bit is valid in all subsequent writes to
						 * xmtfifodata
						 */
#define FIFO_VALID_BYTE1	0x01		/* byte 0 valid */
#define FIFO_VALID_BYTE2	0x02		/* byte 1 valid */
#define FIFO_VALID_BYTE3	0x04		/* byte 2 valid */
#define FIFO_VALID_BYTE4	0x08		/* byte 3 valid */
#define FIFO_VALID_ALL		0x0f		/* all four bytes are valid */

#define SDIOH_MODE_PIO		0		/* pio mode */
#define SDIOH_MODE_DMA		1		/* dma mode */

#define SDIOH_CMDTYPE_NORMAL	0		/* normal command */
#define SDIOH_CMDTYPE_APPEND	1		/* append command */
#define SDIOH_CMDTYPE_CUTTHRU	2		/* cut through command */

#define SDIOH_DMA_START_EARLY	0
#define SDIOH_DMA_START_LATE	1

#define SDIOH_DMA_TX		1
#define SDIOH_DMA_RX		2

#define SDIOH_BLOCK_SIZE_MIN	4
#define SDIOH_BLOCK_SIZE_MAX	0x200

#define SDIOH_SB_ENUM_OFFSET	0x1000		/* sdioh-codec core SB address inside pci-sdioh
						 * controller
						 */

#define SDIOH_HOST_SUPPORT_OCR	0xfff000	/* supported OCR by host controller */

#define RESP_TYPE_NONE 		0
#define RESP_TYPE_R1  		1
#define RESP_TYPE_R2  		2
#define RESP_TYPE_R3  		3
#define RESP_TYPE_R4  		4
#define RESP_TYPE_R5  		5
#define RESP_TYPE_R6  		6

/* SDCMDAT Register */
#define SDIOH_CMD_INDEX_M	BITFIELD_MASK(6)	/* Bits [5:0] 	- Command number */
#define SDIOH_CMD_INDEX_S	0
#define SDIOH_CMD_RESP_TYPE_M	BITFIELD_MASK(3)	/* Bits [8:6] 	- Response type */
#define SDIOH_CMD_RESP_TYPE_S	6
#define SDIOH_CMD_DATA_EN_M	BITFIELD_MASK(1)	/* Bit 9 	- Using DAT line */
#define SDIOH_CMD_DATA_EN_S	9

#define SDIOH_CMD_DATWR_M	BITFIELD_MASK(1)	/* Bit 10 	- Data Write */
#define SDIOH_CMD_DATWR_S	10
#define SDIOH_CMD_DMAMODE_M	BITFIELD_MASK(1)	/* Bit 11 	- DMA Mode */
#define SDIOH_CMD_DMAMODE_S	11
#define SDIOH_CMD_ARC_EN_M	BITFIELD_MASK(1)	/* Bit 12 	- Auto Response Checking */
#define SDIOH_CMD_ARC_EN_S	12
#define SDIOH_CMD_EXP_BSY_M	BITFIELD_MASK(1)	/* Bit 13 	- Expect Busy (R1b) */
#define SDIOH_CMD_EXP_BSY_S	13

#define SDIOH_CMD_CRC_DIS_M	BITFIELD_MASK(1)	/* Bit 14 	- CRC disable */
#define SDIOH_CMD_CRC_DIS_S	14
#define SDIOH_CMD_INDEX_DIS_M	BITFIELD_MASK(1)	/* Bit 15 	- Disable index checking */
#define SDIOH_CMD_INDEX_DIS_S	15

#define SDIOH_CMD_TR_COUNT_M 	BITFIELD_MASK(13)	/* Bits [28:16] - Transfer Count */
#define SDIOH_CMD_TR_COUNT_S	16

#define SDIOH_CMD_APPEND_EN_M	BITFIELD_MASK(1)	/* Bit 29 	- Append enable */
#define SDIOH_CMD_APPEND_EN_S	29
#define SDIOH_CMD_ABORT_EN_M	BITFIELD_MASK(1)	/* Bit 30 	- Abort enable */
#define SDIOH_CMD_ABORT_EN_S	30
#define SDIOH_CMD_BLKMODE_EN_M	BITFIELD_MASK(1)	/* Bit 31 	- Blockmode enable */
#define SDIOH_CMD_BLKMODE_EN_S	31


/* intstatus and intmask */
#define INT_CMD_DAT_DONE_M	BITFIELD_MASK(1)	/* Bit 0: sticky,
							 * sdio command/data xfer done
							 */
#define INT_CMD_DAT_DONE_S	0
#define INT_HOST_BUSY_M		BITFIELD_MASK(1)	/* Bit 1: host busy */
#define INT_HOST_BUSY_S		1
#define INT_DEV_INT_M		BITFIELD_MASK(1)	/* Bit 2: sdio dev interrupt recieved */
#define INT_DEV_INT_S		2
#define INT_ERROR_SUM_M		BITFIELD_MASK(1)	/* Bit 3: OR of Error reg
							 * masked by ErrorMask
							 */
#define INT_ERROR_SUM_S		3
#define INT_CARD_INS_M		BITFIELD_MASK(1)	/* Bit 4: dev inserted */
#define INT_CARD_INS_S		4
#define INT_CARD_GONE_M		BITFIELD_MASK(1)	/* Bit 5: dev removed */
#define INT_CARD_GONE_S		5
#define INT_CMDBUSY_CUTTHRU_M	BITFIELD_MASK(1)	/* sdioh is busy writing to cmdl_cutthru reg
							 */
#define INT_CMDBUSY_CUTTHRU_S	6
#define INT_CMDBUSY_APPEND_M	BITFIELD_MASK(1)	/* this bit is clear when writing cmdl, */
#define INT_CMDBUSY_APPEND_S	7	/* and set when APPEND starts */

#define INT_RCV_INT_M		BITFIELD_MASK(1)	/* Receive DMA Interrupt */
#define INT_RCV_INT_S		16
#define INT_XMT_INT_M		BITFIELD_MASK(1)	/* Transmit DMA Interrupt */
#define INT_XMT_INT_S		24


/* SDBLOCK Register */
#define SDBLOCK_M		BITFIELD_MASK(10)	/* Bits [9:0] Blocksize */
#define SDBLOCK_S		0

#define SD1_MODE 0x1				/* SD Host Cntrlr Spec */
#define SD4_MODE 0x2				/* SD Host Cntrlr Spec */

#endif	/* _SBSDIOH_H */
