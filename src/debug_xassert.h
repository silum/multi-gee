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

#include <stdbool.h>   /* bool */

__BEGIN_DECLS

#define XASSERT(expr) xassert(expr){}

/**
 * @brief flag to enable or disable pausing for user input on assertion
 * failure report
 */
extern bool XASSERT_INTERACTIVE;
/**
 * @brief set to true when an assertion failure has occured
 */
extern bool XASSERT_FAILURE;

/**
 * @brief user defined assertion failure report
 *
 * @desc prints an error message refering to the file name and line
 * number where the error has occured
 *
 * @param file_name  file where error occured
 * @param line       line number where error occured
 */
void report_xassert(const char *file_name, int line);

/**
 * @brief test framework function
 *
 * @desc calls the user function and tests for assertion failures and
 * memory allocation on return
 *
 * @param user_func  user defined function
 */
int debug_test(void (*user_func)());

__END_DECLS

#endif /* ndef DSM_DEBUG_XASSERT */
