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


#include <stdio.h>
#include <math.h>
#include "MoonPhase.h"



MoonPhase::MoonPhase() {
	// TODO Auto-generated constructor stub

}

MoonPhase::~MoonPhase() {
	// TODO Auto-generated destructor stub
}

/*
long MoonPhase::Java_JD(long time_in_ms){
  return time_in_ms / 86400000 + 2440587.5;
}


double MoonPhase::BeleuchtungZeile(long zeit){
  return round(Lightning(zeit) * 10000) / 10000;
}


double MoonPhase::Lightning(double JDE){
  double t, d, m, m1, i;
  t = (JDE - 2451545) / 36525;
  d = 297.8502042 + 445267.11151686 * t - .00163 * t * t + t * t * t / 545868 - t * t * t * t / 113065000;
  m = 357.5291092 + 35999.0502909 * t - .0001536 * t * t + t * t * t / 24490000;
  m1 = 134.9634114 + 477198.8676313 * t + .008997 * t * t + t * t * t / 69699 - t * t * t * t / 14712000;
  i = 180 - d - 6.289 * SN(m1) + 2.1 * SN(m) - 1.274 * SN(2 * d - m1) - .658 * SN(2 * d) - .241 * SN(2 * m1) - .110 * SN(d);
  return (1 + CS(i)) / 2 * 100;
}

double MoonPhase::CS(double x){
  return cos(x * .0174532925199433);
}

double MoonPhase::SN(double x){
    return sin(x * .0174532925199433);
}
*/

void MoonPhase::JulianToDate(TimePlace* now, double jd)
{
    long jdi, b;
    long c,d,e,g,g1;

    jd += 0.5;
    jdi = jd;
    if (jdi > 2299160) {
        long a = (jdi - 1867216.25)/36524.25;
        b = jdi + 1 + a - a/4;
    }
    else b = jdi;

    c = b + 1524;
    d = (c - 122.1)/365.25;
    e = 365.25 * d;
    g = (c - e)/30.6001;
    g1 = 30.6001 * g;
    now->day = c - e - g1;
    now->hour = (jd - jdi) * 24.0;
    if (g <= 13) now->month = g - 1;
    else now->month = g - 13;
    if (now->month > 2) now->year = d - 4716;
    else now->year = d - 4715;
}

double MoonPhase::Julian(int year,int month,double day)
{
    /*
      Returns the number of julian days for the specified day.
      */

    int a,b,c,e;
    if (month < 3) {
	year--;
	month += 12;
    }
    if (year > 1582 || (year == 1582 && month>10) ||
	(year == 1582 && month==10 && day > 15)) {
	a=year/100;
	b=2-a+a/4;
    }
    c = 365.25*year;
    e = 30.6001*(month+1);
    return b+c+e+day+1720994.5;
}

double MoonPhase::sun_position(double j)
{
    double n,x,e,l,dl,v;
    int i;

    n=360/365.2422*j;
    i=n/360;
    n=n-i*360.0;
    x=n-3.762863;
    if (x<0) x += 360;
    x *= RAD;
    e=x;
    do {
	dl=e-.016718*sin(e)-x;
	e=e-dl/(1-.016718*cos(e));
    } while (fabs(dl)>=SMALL_FLOAT);
    v=360/PI*atan(1.01686011182*tan(e/2));
    l=v+282.596403;
    i=l/360;
    l=l-i*360.0;
    return l;
}

double MoonPhase::moon_position(double j, double ls)
{

    double ms,l,mm,n,ev,sms,ae,ec;
    int i;

    /* ls = sun_position(j) */
    ms = 0.985647332099*j - 3.762863;
    if (ms < 0) ms += 360.0;
    l = 13.176396*j + 64.975464;
    i = l/360;
    l = l - i*360.0;
    if (l < 0) l += 360.0;
    mm = l-0.1114041*j-349.383063;
    i = mm/360;
    mm -= i*360.0;
    n = 151.950429 - 0.0529539*j;
    i = n/360;
    n -= i*360.0;
    ev = 1.2739*sin((2*(l-ls)-mm)*RAD);
    sms = sin(ms*RAD);
    ae = 0.1858*sms;
    mm += ev-ae- 0.37*sms;
    ec = 6.2886*sin(mm*RAD);
    l += ev+ec-ae+ 0.214*sin(2*mm*RAD);
    l= 0.6583*sin(2*(l-ls)*RAD)+l;
    return l;
}

double MoonPhase::moon_phase(int year,int month,int day, double hour, int* ip)
{
    /*
      Calculates more accurately than Moon_phase , the phase of the moon at
      the given epoch.
      returns the moon phase as a real number (0-1)
      */

    double j= Julian(year,month,(double)day+hour/24.0)-2444238.5;
    double ls = sun_position(j);
    double lm = moon_position(j, ls);

    double t = lm - ls;
    if (t < 0) t += 360;
    *ip = (int)((t + 22.5)/45) & 0x7;
    return (1.0 - cos((lm - ls)*RAD))/2;
}

void MoonPhase::nextDay(int* y, int* m, int* d, double dd)
{
    TimePlace tp;
    double jd = Julian(*y, *m, (double)*d);

    jd += dd;
    JulianToDate(&tp, jd);

    *y = tp.year;
    *m = tp.month;
    *d = tp.day;
}
