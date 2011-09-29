/***************************************************************************
* Rand.C  (Random Number Generators)                                       *
*                                                                          *
* Source file for pseudo-random number utilities.  Rand is the             *
* base class for several different algorithms for generating pseudo-random *
* numbers.  Any method can generate individual samples or arrays of        *
* samples using "Eval".  The random seed can be reset at any time by       *
* calling "Seed" with any integer.  Random permutations of the integers    *
* 0,1,...(n-1) are generated by "Perm(n,P)".                               *
*                                                                          *
*   HISTORY                                                                *
*      Name    Date        Description                                     *
*                                                                          *
*      arvo    08/04/97    Changed to virtual functions.                   *
*      arvo    06/06/93    Optimization, especially for array evaluators.  *
*      arvo    10/06/91    Converted to C++                                *
*      arvo    11/20/89    Added "gen_seed" function to handle.            *
*      arvo    10/30/89    "state" allocation now done in rand_alloc.      *
*      arvo    07/08/89    Initial coding.                                 *
*                                                                          *
*--------------------------------------------------------------------------*
* Copyright (C) 1989, James Arvo                                           *
*                                                                          *
* This program is free software; you can redistribute it and/or modify it  *
* under the terms of the GNU General Public License as published by the    *
* Free Software Foundation.  See http://www.fsf.org/copyleft/gpl.html      *
*                                                                          *
* This program is distributed in the hope that it will be useful, but      *
* WITHOUT EXPRESS OR IMPLIED WARRANTY of merchantability or fitness for    *
* any particular purpose.  See the GNU General Public License for more     *
* details.                                                                 *
*                                                                          *
***************************************************************************/
#include <stdio.h>
#include <math.h>
#include "Rand.h"

namespace ArvoMath {
#ifndef ABS
#define ABS( x ) ((x) > 0 ? (x) : -(x))
#endif

	/*-------------------------------------------------------------------------*
	* M E T H O D 1                                                           *
	*                                                                         *
	* From "Numerical Recipes," by William H. Press, Brian P. Flannery,       *
	* Saul A. Teukolsky, and William T. Vetterling, p. 197.                   *
	*                                                                         *
	*-------------------------------------------------------------------------*/
	static const long   M1 = 714025;
	static const long   IA =   1366;
	static const long   IC = 150889;
	static const double RM = 1.400512E-6;

	float RandGen_1::Eval()
	{
		register long  *elem;
		register long  offset;
		register float rand;
		offset = 1 + ( 97 * index ) / M1;
		if( offset > 97 ) offset = 97;
		if( offset <  1 ) offset =  1;
		elem   = shuffle + offset;
		rand   = ( index = *elem ) * RM;
		*elem  = ( seed  = ( IA * seed + IC ) % M1 );
		return rand;
	}

	void RandGen_1::Eval( int n, float *array )
	{
		register long *shfl = shuffle;
		register long *elem;
		register long offset;
		for( int i = 0; i < n; i++ ) 
		{
			offset   = 1 + ( 97 * index ) / M1;
			if( offset > 97 ) offset = 97;
			if( offset <  1 ) offset =  1;
			elem     = shfl + offset;
			*array++ = ( index = *elem ) * RM;
			*elem    = ( seed  = ( IA * seed + IC ) % M1 );
		}
	}

	void RandGen_1::Seed( long seed )
	{
		long t = ( IC + ABS( seed ) + 1 ) % M1;
		for( register int k = 1; k <= 97; k++ )
		{
			t = ( IA * t + IC ) % M1;
			shuffle[k] = ABS( t );
		}
		t = ( IA * t + IC ) % M1;
		seed  = ABS( t );
		index = ABS( t );
	}

	/*-------------------------------------------------------------------------*
	* M E T H O D 2                                                           *
	*                                                                         *
	* From "The Multiple Prime Random Number Generator," by Alexander Haas,   *
	* ACM Transactions on Mathematical Software, Vol. 13, No. 4, December     *
	* 1987, pp. 368-381.                                                      *
	*                                                                         *
	*-------------------------------------------------------------------------*/
	float RandGen_2::Eval()
	{
		if( (m += 7    ) >=   9973 ) m -=  9871;
		if( (i += 1907 ) >=  99991 ) i -= 89989;
		if( (j += 73939) >= 224729 ) j -= 96233;
		r = ((r * m + i + j) % 100000) / 10;
		return r * 1.00010001E-4;
	}

	void RandGen_2::Eval( int n, float *array )
	{
		for( register int k = 0; k < n; k++ ) 
		{
			if( (m += 7    ) >=   9973 ) m -=  9871;
			if( (i += 1907 ) >=  99991 ) i -= 89989;
			if( (j += 73939) >= 224729 ) j -= 96233;
			r = ((r * m + i + j) % 100000) / 10;
			*array++ = r * 1.00010001E-4;
		}
	}

	void RandGen_2::Seed( long seed )
	{
		r = ABS( seed      );
		m = ABS( seed *  7 );
		i = ABS( seed * 11 );
		j = ABS( seed * 13 );
		if( m < 100    ) m += 100;
		if( i < 10000  ) i += 10000;
		if( j < 128000 ) j += 128000;
	}

	/*-------------------------------------------------------------------------*
	* M E T H O D 3                                                           *
	*                                                                         *
	* From "A More Portable Fortran Random Number Generator," by Linus        *
	* Schrage, ACM Transactions on Mathematical Software, Vol. 5, No, 2,      *
	* June 1979, pp. 132-138.                                                 *
	*                                                                         *
	*-------------------------------------------------------------------------*/
	static const long A3 = 16807;
	static const long P3 = 2147483647;

	float RandGen_3::Eval()
	{
		long xhi    = ix >> 16;
		long xalo   = ( ix & 0xFFFF ) * A3;
		long leftlo = xalo >> 16;
		long fhi    = xhi * A3 + leftlo;
		long k      = fhi >> 15;
		ix          = ( ((xalo - (leftlo << 16)) - P3) +
			((fhi - (k << 15)) << 16) ) + k;
		if( ix < 0 ) ix += P3;
		return ix * 4.656612875E-10;
	}

	void RandGen_3::Eval( int n, float *array )
	{
		register long xhi, xalo, leftlo;
		register long fhi, k;
		for( register int i = 0; i < n; i++ ) 
		{
			xhi    = ix >> 16;
			xalo   = ( ix & 0xFFFF ) * A3;
			leftlo = xalo >> 16;
			fhi    = xhi * A3 + leftlo;
			k      = fhi >> 15;
			ix     = ( ((xalo - (leftlo << 16)) - P3) +
				((fhi - (k << 15)) << 16) ) + k;
			if( ix < 0 ) ix += P3;
			*array++ = ix * 4.656612875E-10;
		}
	}

	void RandGen_3::Seed( long seed )
	{
		ix = ABS( seed );
	}

	/*-------------------------------------------------------------------------*
	* R A N D : : P E R M        (Permutation)                                *
	*                                                                         *
	* This routine fills an integer array of length "len" with a random       *
	* permutation of the integers 0, 1, 2, ... (len-1).                       *
	*                                                                         *
	* For efficiency, the random numbers are generated in batches of up to    *
	* "Nmax" at a time.  The constant Nmax can be set to any value >= 1.      *
	*                                                                         *
	*-------------------------------------------------------------------------*/
	static const int Nmax = 20;

	void RandGen::Perm( int len, int perm[] )
	{
		float R[ Nmax ];    // A buffer for getting random numbers.
		int   L = len - 1;  // Total number of random numbers needed.
		int   N = 0;        // How many to generate when we call Eval.
		int   n = 0;        // The array index into R.

		// First initialize the array "perm" to the identity permutation.

		for( int j = 0; j < len; j++ ) perm[j] = j;

		// Now swap a random element in the front with the i'th element.
		// When i gets down to 0, we're done.

		for( int i = len - 1; i > 0; i-- )   // Element i is a swap candidate.
		{
			if( n == N )                     // Generate more random numbers.
			{
				N = ( L < Nmax ) ? L : Nmax; // Can't get more than "Nmax".
				Eval( N, R );                // Generate N random numbers.
				L -= N;                      // Decrement total counter.
				n  = 0;                      // Start index at beginning of R.
			}
			float r = ( i + 1 ) * R[ n++ ];  // Pick a float in [0,i+1].
			int   k = (int)r;                // Truncate r to an integer.
			if( k < i )                      // Disregard k == i and k == i+1.
			{
				int tmp = perm[i];           // Swap elements i and k.
				perm[i] = perm[k];
				perm[k] = tmp;
			}
		}
	}
};