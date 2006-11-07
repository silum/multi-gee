/* $Id$
 * Copyright (C) 2006 Deneys S. Maartens <dsm@tlabs.ac.za>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
%{
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <cclass/classdef.h>

#include "examples/device.h"

#define YY_NO_UNPUT
#include "y.tab.h"

USE_XASSERT

/**
 * @brief count input lines and colums, used for error reporting
 */
static void count();

/**
 * @brief error printing function
 *
 * @param s  error message
 */
void yyerror(char const *s);

/**
 * @brief flex/lex specific
 */
int yywrap();

/**
 * @brief convert argyment to long, or issue error
 *
 * @param s  string to convert
 *
 * @return converted string
 */
static long xstrtol(char *s);

%}

D                       [0-9]
L                       [a-zA-Z_]
S                       [/]

%%

captured                { count(); return CAPTURED; }
count                   { count(); return COUNT; }
dev                     { count(); return DEV; }
diff                    { count(); return DIFF; }
end                     { count(); return END; }
frames                  { count(); return FRAMES; }
id                      { count(); return ID; }
integer                 { count(); return INTEGER; }
multi-gee               { count(); return MULTI_GEE; }
now                     { count(); return NOW; }
overhead                { count(); return OVERHEAD; }
sequence                { count(); return SEQUENCE; }
start                   { count(); return START; }
startup                 { count(); return STARTUP; }
then                    { count(); return THEN; }
tv                      { count(); return TV; }

\*\*                    { count(); return STARS; }

{D}{1,4}-{D}{2}-{D}{2}  { count(); STRDUP(yylval.s, yytext); return DATE_STR; }
{D}{1,2}:{D}{2}:{D}{2}  { count(); STRDUP(yylval.s, yytext); return TIME_STR; }

({L}|{S})({L}|{D}|{S})* { count(); STRDUP(yylval.s, yytext); return IDENTIFIER; }

[+-]?{D}+               { count(); yylval.n = xstrtol(yytext); return(INTEGER); }

[-=:.]                  { count(); return(yytext[0]); }

[ \t\v\n\f]             { count(); }

<<EOF>>                 { XASSERT(1){ /* silence unused _do_xassert error */ }; return END_OF_FILE; }

%%

// .                       { count(); /* ignore bad characters */ }

/* flex/lex specific */
int
yywrap()
{
	return(1);
}

/* input line and column counters */
int line = 1;
int column = 0;

void
count()
{
	int i;

	/* until end of string */
	for (i = 0; yytext[i] != '\0'; i++)

		/* end of line found */
		if (yytext[i] == '\n') {
			column = 0;
			++line;
		} else
		/* tab found */
		if (yytext[i] == '\t')
			column += 8 - (column % 8);
		else
		/* increment by 1 char */
			++column;

	/* uncomment this to echo the input to scanner */
	/* ECHO */;
}

long
xstrtol(char *s)
{
	char *end;
	long l;

	l = strtol(s, &end, 10);

	char *ep = s;
	while (*ep++) { /* empty */ }
	if (end != --ep) {
		warnx("could not convert `%s' to int\n", s);
	}

	return l;
}
