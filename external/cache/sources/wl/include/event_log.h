/*
 * EVENT_LOG system definitions
 *
 * $Copyright Open Broadcom Corporation$
 *
 * $Id: event_log.h 241182 2011-02-17 21:50:03Z $
 */

#ifndef _EVENT_LOG_H_
#define _EVENT_LOG_H_


/* Set a maximum number of sets here.  It is not dynamic for
 *  efficiency of the EVENT_LOG calls.
 */
#define NUM_EVENT_LOG_SETS 4

/* Define new event log tags here */
#define EVENT_LOG_TAG_NULL	0	/* Special null tag */
#define EVENT_LOG_TAG_TS	1	/* Special timestamp tag */
#define EVENT_LOG_TAG_BUS_OOB	2
#define EVENT_LOG_TAG_BUS_STATE	3
#define EVENT_LOG_TAG_BUS_PROTO	4
#define EVENT_LOG_TAG_BUS_CTL	5
#define EVENT_LOG_TAG_BUS_EVENT	6
#define EVENT_LOG_TAG_BUS_PKT	7
#define EVENT_LOG_TAG_BUS_FRAME	8
#define EVENT_LOG_TAG_BUS_DESC	9
#define EVENT_LOG_TAG_BUS_SETUP	10
#define EVENT_LOG_TAG_BUS_MISC	11
#define EVENT_LOG_TAG_MAX 	11

/* Flags for tag control */
#define EVENT_LOG_TAG_FLAG_NONE		0
#define EVENT_LOG_TAG_FLAG_LOG		0x80
#define EVENT_LOG_TAG_FLAG_PRINT	0x40
#define EVENT_LOG_TAG_FLAG_MASK		0x3f

/* We make sure that the block size will fit in a single packet
 *  (allowing for a bit of overhead on each packet
 */
#define EVENT_LOG_MAX_BLOCK_SIZE 1400

/*
 * There are multiple levels of objects define here:
 *   event_log_set - a set of buffers
 *   event log groups - every event log call is part of just one.  All
 *                      event log calls in a group are handled the
 *                      same way.  Each event log group is associated
 *                      with an event log set or is off.
 */

#ifndef __ASSEMBLER__

/* On the external system where the dumper is we need to make sure
 * that these types are the same size as they are on the ARM the
 * produced them
 */
#ifdef EVENT_LOG_DUMPER
#define _EL_BLOCK_PTR uint32
#define _EL_TYPE_PTR uint32
#define _EL_SET_PTR uint32
#define _EL_TOP_PTR uint32
#else
#define _EL_BLOCK_PTR struct event_log_block *
#define _EL_TYPE_PTR uint32 *
#define _EL_SET_PTR struct event_log_set **
#define _EL_TOP_PTR struct event_log_top *
#endif /* EVENT_LOG_DUMPER */

/* Each event log entry has a type.  The type is the LAST word of the
 * event log.  The printing code walks the event entries in reverse
 * order to find the first entry.
 */
typedef union event_log_hdr {
	struct {
		uint8 tag;		/* Event_log entry tag */
		uint8 count;		/* Count of 4-byte entries */
		uint16 fmt_num;		/* Format number */
	};
	uint32 t;			/* Type cheat */
} event_log_hdr_t;

/* Event log sets (a logical circurlar buffer) consist of one or more
 * event_log_blocks.  The blocks themselves form a logical circular
 * list.  The log entries are placed in each event_log_block until it
 * is full.  Logging continues with the next event_log_block in the
 * event_set until the last event_log_block is reached and then
 * logging starts over with the first event_log_block in the
 * event_set.
 */
typedef struct event_log_block {
	_EL_BLOCK_PTR next_block;
	_EL_BLOCK_PTR prev_block;
	_EL_TYPE_PTR end_ptr;

	/* Start of packet sent for log tracing */
	uint16 pktlen;			/* Size of rest of block */
	uint16 count;			/* Logtrace counter */
	uint32 timestamp;		/* Timestamp at start of use */
	uint32 event_logs;
} event_log_block_t;

/* There can be multiple event_sets with each logging a set of
 * associated events (i.e, "fast" and "slow" events).
 */
typedef struct event_log_set {
	_EL_BLOCK_PTR first_block; 	/* Pointer to first event_log block */
	_EL_BLOCK_PTR last_block; 	/* Pointer to last event_log block */
	_EL_BLOCK_PTR logtrace_block;	/* next block traced */
	_EL_BLOCK_PTR cur_block;   	/* Pointer to current event_log block */
	_EL_TYPE_PTR cur_ptr;      	/* Current event_log pointer */
	uint32 blockcount;		/* Number of blocks */
	uint16 logtrace_count;		/* Last count for logtrace */
	uint16 blockfill_count;		/* Fill count for logtrace */
	uint32 timestamp;		/* Last timestamp event */
	uint32 cyclecount;		/* Cycles at last timestamp event */
} event_log_set_t;

/* Top data structure for access to everything else */
typedef struct event_log_top {
	uint32 magic;
#define EVENT_LOG_TOP_MAGIC 0x474C8669 /* 'EVLG' */
	uint32 version;
#define EVENT_LOG_VERSION 1
	uint32 num_sets;
	uint32 logstrs_size;		/* Size of lognums + logstrs area */
	uint32 timestamp;		/* Last timestamp event */
	uint32 cyclecount;		/* Cycles at last timestamp event */
	_EL_SET_PTR sets; 		/* Ptr to array of <num_sets> set ptrs */
} event_log_top_t;

#ifndef EVENT_LOG_DUMPER

#ifndef EVENT_LOG_COMPILE

/* Null define if no tracing */
#define EVENT_LOG(format, ...)

#else  /* EVENT_LOG_COMPILE */

/* The first few are special because they can be done more efficiently
 * this way and they are the common case.  Once there are too many
 * parameters the code size starts to be an issue and a loop is better
 */
#define _EVENT_LOG0(tag, fmt_num) 			\
	event_log0(tag, fmt_num)
#define _EVENT_LOG1(tag, fmt_num, t1) 			\
	event_log1(tag, fmt_num, t1)
#define _EVENT_LOG2(tag, fmt_num, t1, t2) 		\
	event_log2(tag, fmt_num, t1, t2)
#define _EVENT_LOG3(tag, fmt_num, t1, t2, t3) 		\
	event_log3(tag, fmt_num, t1, t2, t3)
#define _EVENT_LOG4(tag, fmt_num, t1, t2, t3, t4) 	\
	event_log4(tag, fmt_num, t1, t2, t3, t4)

/* The rest call the generic routine that takes a count */
#define _EVENT_LOG5(tag, fmt_num, ...) event_logn(5, tag, fmt_num, __VA_ARGS__)
#define _EVENT_LOG6(tag, fmt_num, ...) event_logn(6, tag, fmt_num, __VA_ARGS__)
#define _EVENT_LOG7(tag, fmt_num, ...) event_logn(7, tag, fmt_num, __VA_ARGS__)
#define _EVENT_LOG8(tag, fmt_num, ...) event_logn(8, tag, fmt_num, __VA_ARGS__)
#define _EVENT_LOG9(tag, fmt_num, ...) event_logn(9, tag, fmt_num, __VA_ARGS__)
#define _EVENT_LOGa(tag, fmt_num, ...) event_logn(10, tag, fmt_num, __VA_ARGS__)
#define _EVENT_LOGb(tag, fmt_num, ...) event_logn(11, tag, fmt_num, __VA_ARGS__)
#define _EVENT_LOGc(tag, fmt_num, ...) event_logn(12, tag, fmt_num, __VA_ARGS__)
#define _EVENT_LOGd(tag, fmt_num, ...) event_logn(13, tag, fmt_num, __VA_ARGS__)
#define _EVENT_LOGe(tag, fmt_num, ...) event_logn(14, tag, fmt_num, __VA_ARGS__)
#define _EVENT_LOGf(tag, fmt_num, ...) event_logn(15, tag, fmt_num, __VA_ARGS__)

/* Hack to make the proper routine call when variadic macros get
 * passed.  Note the max of 15 arguments.  More than that can't be
 * handled by the event_log entries anyways so best to catch it at compile
 * time
 */

#define _EVENT_LOG_VA_NUM_ARGS(F, _1, _2, _3, _4, _5, _6, _7, _8, _9,	\
			       _A, _B, _C, _D, _E, _F, N, ...) F ## N

#define _EVENT_LOG(tag, fmt, ...)					\
	static char logstr[] __attribute__ ((section(".logstrs"))) = fmt; \
	static uint32 fmtnum __attribute__ ((section(".lognums"))) = (uint32) &logstr; \
	_EVENT_LOG_VA_NUM_ARGS(_EVENT_LOG, ##__VA_ARGS__,		\
			       F, E, D, C, B, A, 9, 8,			\
			       7, 6, 5, 4, 3, 2, 1, 0)			\
	(tag, (int) &fmtnum , ## __VA_ARGS__);				\


#define EVENT_LOG_FAST(tag, fmt, ...)					\
	if (event_log_tag_sets != NULL) {				\
		uint8 tag_flag = *(event_log_tag_sets + tag);		\
		if (tag_flag != 0) {					\
			_EVENT_LOG(tag, fmt , ## __VA_ARGS__);		\
		}							\
	}

#define EVENT_LOG_COMPACT(tag, fmt, ...)				\
	if (1) {							\
		_EVENT_LOG(tag, fmt , ## __VA_ARGS__);			\
	}

#define EVENT_LOG(tag, fmt, ...) EVENT_LOG_COMPACT(tag, fmt , ## __VA_ARGS__)

extern uint8 *event_log_tag_sets;

extern int event_log_init(si_t *sih);
extern int event_log_set_init(si_t *sih, int set_num, int size);
extern int event_log_set_expand(si_t *sih, int set_num, int size);
extern int event_log_set_shrink(si_t *sih, int set_num, int size);
extern int event_log_tag_start(int tag, int set_num, int flags);
extern int event_log_tag_stop(int tag);
extern int event_log_get(int set_num, int buflen, void *buf);
extern uint8 * event_log_next_logtrace(int set_num);

extern void event_log0(int tag, int fmtNum);
extern void event_log1(int tag, int fmtNum, uint32 t1);
extern void event_log2(int tag, int fmtNum, uint32 t1, uint32 t2);
extern void event_log3(int tag, int fmtNum, uint32 t1, uint32 t2, uint32 t3);
extern void event_log4(int tag, int fmtNum, uint32 t1, uint32 t2, uint32 t3, uint32 t4);
extern void event_logn(int num_args, int tag, int fmtNum, ...);

extern void event_log_time_sync(void);

#endif /* EVENT_LOG_DUMPER */

#endif /* EVENT_LOG_COMPILE */

#endif /* __ASSEMBLER__ */

#endif /* _EVENT_LOG_H */
