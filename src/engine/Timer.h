/* OpenHoW
 * Copyright (C) 2017-2020 TalonBrave.info and Others (see CONTRIBUTORS)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <time.h>

class Timer {
public:
	Timer() {
		clock_gettime( CLOCK_MONOTONIC, &clock );
	}

	void End() {
		struct timespec end;
		clock_gettime( CLOCK_MONOTONIC, &end );
		timeTaken = TimespecToDouble( TimespecSub( end, clock ) );
	}

	double GetTimeTaken() const {
		return timeTaken;
	}

private:
#define NSEC_PER_SEC 1000000000

	/** \fn double timespec_to_double(struct timespec ts)
 	 *  \brief Converts a timespec to a fractional number of seconds.
 	 *  Originally written by Daniel Collins (2017), used with permission
	 */
	static double TimespecToDouble( struct timespec ts ) {
		return ( ( double ) ( ts.tv_sec ) + ( ( double ) ( ts.tv_nsec ) / NSEC_PER_SEC ) );
	}

	/** \fn struct timespec timespec_normalise(struct timespec ts)
	 *  \brief Normalises a timespec structure.
	 *
	 * Returns a normalised version of a timespec structure, according to the
	 * following rules:
	 *
	 * 1) If tv_nsec is >1,000,000,00 or <-1,000,000,000, flatten the surplus
	 *    nanoseconds into the tv_sec field.
	 *
	 * 2) If tv_sec is >0 and tv_nsec is <0, decrement tv_sec and roll tv_nsec up
	 *    to represent the same value on the positive side of the new tv_sec.
	 *
	 * 3) If tv_sec is <0 and tv_nsec is >0, increment tv_sec and roll tv_nsec down
	 *    to represent the same value on the negative side of the new tv_sec.
	 *
	 * Originally written by Daniel Collins (2017), used with permission
	 */
	static struct timespec TimespecNormalise( struct timespec ts ) {
		while ( ts.tv_nsec >= NSEC_PER_SEC ) {
			++( ts.tv_sec );
			ts.tv_nsec -= NSEC_PER_SEC;
		}

		while ( ts.tv_nsec <= -NSEC_PER_SEC ) {
			--( ts.tv_sec );
			ts.tv_nsec += NSEC_PER_SEC;
		}

		if ( ts.tv_nsec < 0 && ts.tv_sec > 0 ) {
			/* Negative nanoseconds while seconds is positive.
			 * Decrement tv_sec and roll tv_nsec over.
			*/

			--( ts.tv_sec );
			ts.tv_nsec = NSEC_PER_SEC - ( -1 * ts.tv_nsec );
		} else if ( ts.tv_nsec > 0 && ts.tv_sec < 0 ) {
			/* Positive nanoseconds while seconds is negative.
			 * Increment tv_sec and roll tv_nsec over.
			*/

			++( ts.tv_sec );
			ts.tv_nsec = -NSEC_PER_SEC - ( -1 * ts.tv_nsec );
		}

		return ts;
	}

	/** \fn struct timespec timespec_sub(struct timespec ts1, struct timespec ts2)
 	 *  \brief Returns the result of subtracting ts2 from ts1.
 	 *  Originally written by Daniel Collins (2017), used with permission
	 */
	static struct timespec TimespecSub( struct timespec ts1, struct timespec ts2 ) {
		/* Normalise inputs to prevent tv_nsec rollover if whole-second values
		 * are packed in it.
		*/
		ts1 = TimespecNormalise( ts1 );
		ts2 = TimespecNormalise( ts2 );

		ts1.tv_sec -= ts2.tv_sec;
		ts1.tv_nsec -= ts2.tv_nsec;

		return TimespecNormalise( ts1 );
	}

	struct timespec clock;
	double timeTaken{ 0.0 };
};
