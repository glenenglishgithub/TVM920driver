#ifndef CTVM920INFOH
#define CTVM920INFOH

#include "Cmachloc.h"
#include "tvm920def.h"
#include <stdint.h>
#include <math.h>
class CTVM920Info {

public :
	double  xticks_per_mm;
	double  yticks_per_mm;
    double  z1ticks_per_mm;
    double  a1ticks_per_degree;
    double  x_pos_home;
    double  y_pos_home;
    double camWheelRadiusmm;
    double zStepperTicksPerDegree;
    double z12_HomeOffsetDegrees;
    double z34_HomeOffsetDegrees;
    double xtravel;
    double ytravel;
    Cmachloc<double> poslimits;
    Cmachloc<double> neglimits;
public :
    CTVM920Info() : poslimits(TVM920_NNOZZ), neglimits(TVM920_NNOZZ) {
       xticks_per_mm = 85190.19355;
       yticks_per_mm  = 60206.6;                /* simialr to register $1e
       /* essentially, a tick is a stepper tick,
       and the minimum tick is 256/512,
       so 256/1024  == 1 stepper step ??

       /* reg 2d, 2e look like decimal rep of  GUI tweaak XY */

       a1ticks_per_degree  =  800000/180;

       xtravel = x_pos_home = 465.0;
       ytravel= y_pos_home = 445.0;
       camWheelRadiusmm = 9.62; /* approx */
       zStepperTicksPerDegree = 4000;     /* approx 360000 ticks / 90 deg measured*/
       z12_HomeOffsetDegrees = 0;
       z34_HomeOffsetDegrees = 0;
       poslimits.x = 465.0;
       neglimits.x = 0.0;
       poslimits.y = 445.0;
       neglimits.y = 0.0;
       for (int i=0;i<TVM920_NNOZZ;i++) {
           poslimits.z[i] = 0;
           poslimits.a[i] = 180;
           neglimits.z[i] = (-2) * camWheelRadiusmm;
           neglimits.a[i] = -180;
       }

    }
public :
	/* takes class with millimeter dustances, turns into ticks */
    int CTVM920Info::xlateToTicks(Cmachloc<double> & s, Cmachloc<int32_t> * d);
    int CTVM920Info::xlateTomm(Cmachloc<int32_t> & s, Cmachloc<double> & d) ;
    double  CTVM920Info::xlateZTicksTomm(int32_t ticks, double degsHomeoffset);
    int32_t CTVM920Info::xlatemmToZticks(double mm, double degsHomeoffset );
    int32_t CTVM920Info::xlateADegToAticks(double deg);
    void ImposeBoundaryLimit(Cmachloc<double> & t);
};

#endif
