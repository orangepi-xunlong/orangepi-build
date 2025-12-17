/*
 * Broadcom USB/HSIC framing
 * Software-specific framing definitions shared between host and device
 *
 * $Copyright Open 2012 Broadcom Corporation$
 *
 * $Id: bcmusb.h 281591 2012-09-02 18:43:02Z sgsiener $
 */

#ifndef	_bcmusb_h_
#define	_bcmusb_h_

/*
 * Software-defined USB frame header
 */

#ifdef USB_FRAME_HEADER
#define USBF_HEADER_LEN		16
#else
#define USBF_HEADER_LEN		0
#endif

#ifdef USBF_CKSUM_OVER_FRAME
#define USBF_DEF_CKSUM_LEN 0
#else
#define USBF_DEF_CKSUM_LEN USBF_HEADER_LEN
#endif

/* Current protocol version */
#define USBF_PROT_VERSION	1

/* ----- first 32-bit word ----- */

/* 4-bit version field */
#define USBF_VER_MASK			0x0000000f

/* 8-bit flags field */
#define USBF_FLAGS_MASK			0x00000ff0
#define USBF_FLAGS_SHIFT		4

/* flags field definitions */
#define USBF_FLAGS_TIMESTAMPED	0x01	/* frame header includes timestamp */
#define USBF_FLAGS_FRAME_EVENT	0x02	/* set when frame contains an Event indication */
#define USBF_FLAGS_FRAME_CKSUM	0x04	/* set when cksum is over entire frame */

/* 3-bit channel identifier field */
#define USBF_CHAN_MASK			0x00007000
#define USBF_CHAN_SHIFT			12

#define USBF_CHAN_CONTROL		0
#define USBF_CHAN_INTERRUPT		1
#define USBF_CHAN_DATAEVENT		2

/* used by host & device to disambiguate frames with this header from ones without */
#define USBF_TAG			0x00008000 /* bit won't appear in bdc/ioctl header */

/* 16-bit checksum */
#define USBF_CHECKSUM_MASK		0xffff0000	/* checksum mask */
#define USBF_CHECKSUM_SHIFT		16

/* ----- second 32-bit word ----- */

/* 16-bit length */
#define USBF_LENGTH_MASK		0x0000ffff	/* length mask */

/* 16-bit sequence number */
#define USBF_SEQNUM_MASK		0xffff0000	/* sequence number mask */
#define USBF_SEQSUM_SHIFT		16

/* ----- third 32-bit word ----- */

/* 32-bit timestamp */
#define USBF_TIMESTAMP_MASK		0xffffffff	/* timestamp value mask */

/* ----- fourth 32-bit word ----- */

/* reserve 32 bits for future use */
#define USBF_RESERVED_MASK		0xffffffff	/* reserved for future use mask */

#endif	/* _bcmusb_h_ */
