#ifndef GREATCIRCLE_H
#define GREATCIRCLE_H
#include "math.h"
#include "Arduino.h"

float		trunkAngle(float	angle);
double		trunkAngle(double	angle);
int			trunkAngle(int 		angle);
float		toRad(float degrees);
float		toDeg(float radians);
#include "util/Waypoint.h" //uuuuuuuuggh. Waypoint needs toRad and toDeg
float		calcHeading (Waypoint a, Waypoint b);
float		calcDistance(Waypoint a, Waypoint b); //all distances are in Miles
Waypoint	extrapPosition(Waypoint position, float bearing, float distance);

#endif