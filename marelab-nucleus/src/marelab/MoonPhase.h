/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Author      : Marc Philipp Hammermann
 * Version     :
 * Copyright © 2013 marc philipp hammermann  <marchammermann@googlemail.com>
 *
 *
 *
 *
 */

#ifndef MOONPHASE_H_
#define MOONPHASE_H_

typedef struct {
    int year,month,day;
    double hour;
} TimePlace;

#define		PI	3.1415926535897932384626433832795
#define		RAD	(PI/180.0)
#define     SMALL_FLOAT	(1e-12)

class MoonPhase {
public:
	MoonPhase();
	virtual ~MoonPhase();
	void JulianToDate(TimePlace* now, double jd);
	double Julian(int year,int month,double day);
	double sun_position(double j);
	double moon_position(double j, double ls);
	double moon_phase(int year,int month,int day, double hour, int* ip);
	void nextDay(int* y, int* m, int* d, double dd);
	//double MoonPhase::SN(double x);
	//double MoonPhase::CS(double x);
	//double MoonPhase::Lightning(double JDE);
	//double MoonPhase::BeleuchtungZeile(int zeit);
	//long MoonPhase::Java_JD(long time_in_ms);
};

#endif /* MOONPHASE_H_ */
