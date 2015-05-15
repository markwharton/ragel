/*
 *  Copyright 2010-2012 Adrian Thurston <thurston@complang.org>
 */

/*  This file is part of Aapl.
 *
 *  Aapl is free software; you can redistribute it and/or modify it under the
 *  terms of the GNU Lesser General Public License as published by the Free
 *  Software Foundation; either version 2.1 of the License, or (at your option)
 *  any later version.
 *
 *  Aapl is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Aapl; if not, write to the Free Software Foundation, Inc., 59
 *  Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <colm/rtvector.h>
#include <colm/pdarun.h>

#include <string.h>
#include <stdlib.h>
#include <assert.h>


void initRtCodeVect( struct rt_code_vect *vect )
{
	vect->data = 0;
	vect->tabLen = 0;
	vect->allocLen = 0;
}

static long newSizeUp( long existing, long needed )
{ 
	return needed > existing ? (needed<<1) : existing;
}

static long newSizeDown( long existing, long needed )
{
	return needed < (existing>>2) ? (needed<<1) : existing;
}

/* Up resize the data for len elements using Resize::upResize to tell us the
 * new tabLen. Reads and writes allocLen. Does not read or write tabLen. */
static void upResize( struct rt_code_vect *vect, long len )
{
	/* Ask the resizer what the new tabLen will be. */
	long newLen = newSizeUp(vect->allocLen, len);

	/* Did the data grow? */
	if ( newLen > vect->allocLen ) {
		vect->allocLen = newLen;
		if ( vect->data != 0 ) {
			/* Table exists already, resize it up. */
			vect->data = (code_t*) realloc( vect->data, sizeof(code_t) * newLen );
			//if ( vect->data == 0 )
			//	throw std::bad_alloc();
		}
		else {
			/* Create the data. */
			vect->data = (code_t*) malloc( sizeof(code_t) * newLen );
			//if ( vect->data == 0 )
			//	throw std::bad_alloc();
		}
	}
}

/* Down resize the data for len elements using Resize::downResize to determine
 * the new tabLen. Reads and writes allocLen. Does not read or write tabLen. */
static void downResize( struct rt_code_vect *vect, long len)
{
	/* Ask the resizer what the new tabLen will be. */
	long newLen = newSizeDown( vect->allocLen, len );

	/* Did the data shrink? */
	if ( newLen < vect->allocLen ) {
		vect->allocLen = newLen;
		if ( newLen == 0 ) {
			/* Simply free the data. */
			free( vect->data );
			vect->data = 0;
		}
		else {
			/* Not shrinking to size zero, realloc it to the smaller size. */
			vect->data = (code_t*) realloc( vect->data, sizeof(code_t) * newLen );
			//if ( vect->data == 0 )
			//	throw std::bad_alloc();
		}
	}
}


void colm_rt_code_vect_empty( struct rt_code_vect *vect )
{
	if ( vect->data != 0 ) {
		/* Free the data space. */
		free( vect->data );
		vect->data = 0;
		vect->tabLen = vect->allocLen = 0;
	}
}

void colm_rt_code_vect_replace( struct rt_code_vect *vect, long pos,
		const code_t *val, long len )
{
	long endPos, i;
	//code_t *item;

	/* If we are given a negative position to replace at then
	 * treat it as a position relative to the length. */
	if ( pos < 0 )
		pos = vect->tabLen + pos;

	/* The end is the one past the last item that we want
	 * to write to. */
	endPos = pos + len;

	/* Make sure we have enough space. */
	if ( endPos > vect->tabLen ) {
		upResize( vect, endPos );

		/* Delete any objects we need to delete. */
		//item = vect->data + pos;
		//for ( i = pos; i < vect->tabLen; i++, item++ )
		//	item->~code_t();
		
		/* We are extending the vector, set the new data length. */
		vect->tabLen = endPos;
	}
	else {
		/* Delete any objects we need to delete. */
		//item = vect->data + pos;
		//for ( i = pos; i < endPos; i++, item++ )
		//	item->~code_t();
	}

	/* Copy data in using copy constructor. */
	code_t *dst = vect->data + pos;
	const code_t *src = val;
	for ( i = 0; i < len; i++, dst++, src++ )
		*dst = *src;
}

void colm_rt_code_vect_remove( struct rt_code_vect *vect, long pos, long len )
{
	long newLen, lenToSlideOver, endPos;
	code_t *dst;//, *item;

	/* If we are given a negative position to remove at then
	 * treat it as a position relative to the length. */
	if ( pos < 0 )
		pos = vect->tabLen + pos;

	/* The first position after the last item deleted. */
	endPos = pos + len;

	/* The new data length. */
	newLen = vect->tabLen - len;

	/* The place in the data we are deleting at. */
	dst = vect->data + pos;

	/* Call Destructors. */
	//item = dst;
	//for ( long i = 0; i < len; i += 1, item += 1 )
	//	item->~code_t();
	
	/* Shift data over if necessary. */
	lenToSlideOver = vect->tabLen - endPos;	
	if ( len > 0 && lenToSlideOver > 0 )
		memmove(dst, dst + len, sizeof(code_t)*lenToSlideOver);

	/* Shrink the data if necessary. */
	downResize( vect, newLen );

	/* Set the new data length. */
	vect->tabLen = newLen;
}


