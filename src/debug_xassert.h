/* $Id$ */
/*
 * Debug report_xassert function.
 *
 * Author:    Deneys S. Maartens
 * Copyright: Deneys S. Maartens (C) 2004
 * Version:   $Rev$
 * Date:      $Date$
 */

#ifndef DSM_DEBUG_XASSERT_H
#define DSM_DEBUG_XASSERT_H 1

#include <stdbool.h>

__BEGIN_DECLS

extern bool XASSERT_INTERACTIVE;
extern bool XASSERT_FAILURE;

/*
 * user defined assertion failure report
 */
void report_xassert(char *file_name, int line);

/*
 * test framework function
 */
int debug_test(void (*)());

__END_DECLS

#endif /* ndef DSM_DEBUG_XASSERT */
