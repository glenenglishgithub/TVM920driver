#include "CTVM920info.h"

	int32_t CTVM920Info::xlateADegToAticks(double deg) {
        return deg *   a1ticks_per_degree;
    }

    void CTVM920Info::ImposeBoundaryLimit(Cmachloc<double> & t) {
    	int i;
        if ( t.x > poslimits.x) t.x = poslimits.x;
        if (t.x < neglimits.x) t.x = neglimits.x;

        if ( t.y > poslimits.y) t.y = poslimits.y;
        if (t.y < neglimits.y) t.y = neglimits.y;

        for (i=0;i<TVM920_NNOZZ;i++) {
        	if ( t.z[i] > poslimits.z[i]) t.z[i] = poslimits.z[i];
        	if ( t.z[i] < neglimits.z[i]) t.z[i] = neglimits.z[i];
        }
        for (i=0;i<TVM920_NNOZZ;i++) {
        	if ( t.a[i] > poslimits.a[i]) t.a[i] = poslimits.a[i];
        	if ( t.a[i] < neglimits.a[i]) t.a[i] = neglimits.a[i];
        }
    }

	int CTVM920Info::xlateToTicks(Cmachloc<double> & s, Cmachloc<int32_t> * d) {

    	d->x = s.x * xticks_per_mm;
    	d->y = s.y * yticks_per_mm;
        int n = TVM920_NNOZZ;
        while (n--) {
            d->a[n] = xlateADegToAticks(s.a[n]);
            if (n & 0x2) d->z[n] = xlatemmToZticks(s.z[n],z34_HomeOffsetDegrees);
            else   d->z[n] =  xlatemmToZticks(s.z[n],z12_HomeOffsetDegrees);
        }
        return 0;
    }

	int CTVM920Info::xlateTomm(Cmachloc<int32_t> & s, Cmachloc<double> & d) {

    	d.x = s.x / xticks_per_mm;
    	d.y = s.y / yticks_per_mm;
        int n = TVM920_NNOZZ;
        while (n--) {
            d.a[n] = s.a[n] / a1ticks_per_degree   ;
            /* do different nozzle banks */
            if (n&0x2) d.z[n] = xlateZTicksTomm(s.z[n],z34_HomeOffsetDegrees);
            else d.z[n] = xlateZTicksTomm(s.z[n],z12_HomeOffsetDegrees);
        }
        return 0;
    }


    double  CTVM920Info::xlateZTicksTomm(int32_t ticks, double degsHomeoffset) {
    	double mm;

        double degs = ticks   /  zStepperTicksPerDegree;
        degs = ticks / zStepperTicksPerDegree;
        degs -= degsHomeoffset;

        double rads = degs/180.0 * M_PI;

		/* values for negative ticks and negative angles come out negative */
        if ( (rads >= 0  ) && (rads <= M_PI/2 )) {
        	mm = sin(rads) * camWheelRadiusmm; /* sector 0 */
        }
        else if ((rads >= (-M_PI/2)  ) && (rads < 0 )) {
        	mm = sin(rads) * camWheelRadiusmm; /* sector 1 */
        }

        else if (rads >(M_PI/2)) {    /* sector 3*/
            mm = camWheelRadiusmm + sin(rads - M_PI/2) * camWheelRadiusmm;
        }
        else {
        /* sector 2 */
            mm =  sin(rads - -M_PI/2) * camWheelRadiusmm - camWheelRadiusmm ;
        }

        return mm;

    }
    /* translate mm to Z ticks */
    int32_t CTVM920Info::xlatemmToZticks(double mm, double degsHomeoffset ) {
    	double rads;

        if (fabs(mm) >= (2*camWheelRadiusmm)) {
        	/* impose limits */
            const double backoffvalue  = 0.3;
            mm = mm > 0 ?   ((2*camWheelRadiusmm)-backoffvalue)   :  ((-2*camWheelRadiusmm)+backoffvalue);
        }
        if (mm >=0) {	/*second nozzle, viewled right hand side fron front */
            /* determine if in the >90deg sector */
            if (mm >  camWheelRadiusmm)  { /* sector 3  */
                mm -= camWheelRadiusmm;/* subtract it */
                rads = M_PI/2 + asin(mm/camWheelRadiusmm);
            }
            else {
                /* sector 0 */
                rads = asin(mm/camWheelRadiusmm);
            }
        }
        else { /* negaive mmm */
            if (mm <  -camWheelRadiusmm) {
                mm += camWheelRadiusmm;/* add  it */
                rads = asin(mm/camWheelRadiusmm) - M_PI/2;
            }
            else {
               rads = asin(mm/camWheelRadiusmm);
            }

        }
        double degs = rads / M_PI * 180.0;
        degs +=   degsHomeoffset;

        int32_t ticks =    degs  * zStepperTicksPerDegree;
        return ticks;
    }
