/********************************************************************\
 * This program is free software; you can redistribute it and/or    *
 * modify it under the terms of the GNU General Public License as   *
 * published by the Free Software Foundation; either version 2 of   *
 * the License, or (at your option) any later version.              *
 *                                                                  *
 * This program is distributed in the hope that it will be useful,  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
 * GNU General Public License for more details.                     *
 *                                                                  *
 * You should have received a copy of the GNU General Public License*
 * along with this program; if not, write to the Free Software      *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.        *
\********************************************************************/

/*
 * FILE:
 * quickfillcell.c
 *
 * FUNCTION:
 * Implements a text cell with automatic typed-phrase
 * completion.
 *
 * HISTORY:
 * Copyright (c) 1998-2000 Linas Vepstas
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "basiccell.h"
#include "quickfillcell.h"

#define SET(cell,str) { 			\
   if ((cell)->value) free ((cell)->value);	\
   (cell)->value = strdup (str);		\
}

/* ================================================ */

static void 
quick_set (BasicCell *_cell,
           const char *val) 
{
   QuickFillCell *cell = (QuickFillCell *) _cell;
   xaccSetQuickFillCellValue (cell, val);
}

/* ================================================ */
/* when entering new cell, put cursor at end and select everything */

static const char * 
quick_enter (BasicCell *_cell, const char *val,
             int *cursor_position,
             int *start_selection,
             int *end_selection)
{
   QuickFillCell *cell = (QuickFillCell *) _cell;

   cell->qf = cell->qfRoot;

   *cursor_position = -1;
   *start_selection = 0;
   *end_selection = -1;

   return val;
}

/* ================================================ */
/* by definition, all text is valid text.  So accept
 * all modifications */

static const char * 
quick_modify (BasicCell *_cell,
              const char *oldval, 
              const char *change, 
              const char *newval,
              int *cursor_position,
              int *start_selection,
              int *end_selection)
{
   QuickFillCell *cell = (QuickFillCell *) _cell;
   const char *retval;
   QuickFill *match;

   /* If deleting, just accept */
   if (change == NULL)
   {
     SET (&(cell->cell), newval);
     return newval;
   }

   /* If we are inserting in the middle, just accept */
   if (*cursor_position < strlen(oldval))
   {
     SET (&(cell->cell), newval);
     return newval;
   }

   match = xaccGetQuickFillStr(cell->qfRoot, newval);

   if ((match == NULL) || (match->text == NULL))
   {
     SET (&(cell->cell), newval);
     return newval;
   }

   retval = strdup(match->text);

   *start_selection = strlen(newval);
   *end_selection = -1;
   *cursor_position += strlen(change);

   SET (&(cell->cell), retval);
   return retval;
}

/* ================================================ */
/* when leaving cell, make sure that text was put into the qf    */

static const char * 
quick_leave (BasicCell *_cell, const char *val) 
{
   QuickFillCell *cell = (QuickFillCell *) _cell;

   cell->qf = cell->qfRoot;
   xaccQFInsertText (cell->qfRoot, val, cell->sort);
   return val;
}

/* ================================================ */

QuickFillCell *
xaccMallocQuickFillCell (void)
{
   QuickFillCell *cell;
   cell = ( QuickFillCell *) malloc (sizeof (QuickFillCell));

   xaccInitQuickFillCell (cell);
   return cell;
}

/* ================================================ */

void
xaccInitQuickFillCell (QuickFillCell *cell)
{
   xaccInitBasicCell (&(cell->cell));

   cell->qfRoot = xaccMallocQuickFill();
   cell->qf = cell->qfRoot;
   cell->sort = QUICKFILL_LIFO;

   cell->cell.enter_cell    = quick_enter;
   cell->cell.modify_verify = quick_modify;
   cell->cell.leave_cell    = quick_leave;
   cell->cell.set_value     = quick_set;

   xaccQuickFillGUIInit (cell);
}

/* ================================================ */

void
xaccDestroyQuickFillCell (QuickFillCell *cell)
{
   xaccFreeQuickFill (cell->qfRoot);
   cell->qfRoot = NULL;
   cell->qf = NULL;

   cell->cell.enter_cell    = NULL;
   cell->cell.modify_verify = NULL;
   cell->cell.leave_cell    = NULL;
   cell->cell.set_value     = NULL;

   xaccDestroyBasicCell (&(cell->cell));
}

/* ================================================ */

void
xaccSetQuickFillCellValue (QuickFillCell *cell, const char * value)
{
   xaccQFInsertText (cell->qfRoot, value, cell->sort);
   SET (&(cell->cell), value);
}

/* ================================================ */

void
xaccSetQuickFillSort (QuickFillCell *cell, QuickFillSort sort)
{
  if (cell == NULL)
    return;

  cell->sort = sort;
}

/* =============== END OF FILE ==================== */
