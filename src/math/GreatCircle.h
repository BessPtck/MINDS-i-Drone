#ifndef GREATCIRCLE_H
#define GREATCIRCLE_H
#include "math.h"
#include "Waypoint.h"
#ifdef STAND_ALONE_MATH
	#include <stdint.h>
#endif

float  simplifyRadian(float ref, float val);
float  simplifyDegree(float ref, float val);
float  distanceRadian(float   a, float   b);
float  distanceDegree(float   a, float   b);
float  truncateRadian(float val);
float  truncateDegree(float val);
float  trunkAngle(float	angle);
//double trunkAngle(double	angle);
//int    trunkAngle(int 		angle);
float  calcHeading (Waypoint a, Waypoint b);
float  calcDistance(Waypoint a, Waypoint b); //all distances are in Miles
Waypoint extrapPosition(Waypoint position, float bearing, float distance);

#endif
