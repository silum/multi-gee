/* $Id$
 * Copyright (C) 2004, 2005 Deneys S. Maartens <dsm@tlabs.ac.za>
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
#ifndef ITL_PROCESS_UTIL_H
#define ITL_PROCESS_UTIL_H 1

#include <multi-gee.h>

__BEGIN_DECLS

#define PGM_FILE "file.pgm"

void
process(multi_gee_t multi_gee,
	mg_frame_t frame);

__END_DECLS

#endif /* ITL_PROCESS_UTIL_H */
