#include "common_math.h"

double lerp(double from, double to, double w) {
	return (1.0 - w) * from + w * to;
}

double clamp(double value, double min, double max) {
	if (value > max) return max;
	if (value < min) return min;
	return value;
}
