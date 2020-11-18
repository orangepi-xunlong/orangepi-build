/*
 * Trace log blocks sent over HBUS
 *
 * $Copyright Open Broadcom Corporation$
 *
 * $Id: logtrace.h 333856 2012-05-17 23:43:07Z $
 */

#ifndef	_LOGTRACE_H
#define	_LOGTRACE_H

#include <msgtrace.h>

extern void logtrace_start(void);
extern void logtrace_stop(void);
extern int logtrace_sent(void);
extern void logtrace_trigger(void);
extern void logtrace_init(void *hdl1, void *hdl2, msgtrace_func_send_t func_send);
extern bool logtrace_event_enabled(void);

#endif	/* _LOGTRACE_H */
