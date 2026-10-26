#ifndef CAMERA_H
#define CAMERA_H

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <stdbool.h>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <common_math.h>
#include <load_tex.h>
#ifdef EBUG
	#define H_MIN -1.0
	#define D_MIN 0.0
	#define D_MAX 99.9
#else
	#define H_MIN 0.05
	#define D_MIN 2.0
	#define D_MAX 9.0
#endif
#define H_MAX 1.0

typedef struct camera {
	struct {
		double x;
		double y;
		double z;
	} position;
	struct {
		double x;
		double y;
		double z;
	} rotation;
	struct {
		double t;
		double h;
		double d;
	} arcball_position;
	struct {
		double t;
		double h;
		double d;
	} arcball_speed;
	bool disable_clamp;
} camera;

camera camera_create();
void camera_process(camera* camera, double delta);
void camera_set_view(camera* camera);

void camera_rotate(camera* camera, double horizontal, double vertical);
void camera_set_t_speed(camera* camera, double speed);
void camera_set_h_speed(camera* camera, double speed);
void camera_set_d_speed(camera* camera, double speed);

void camera_update_position_mouse(camera* camera, double t_, double h_);
void camera_update_zoom_mouse(camera* camera, double zoom);
void camera_force_position(camera* camera, double d, double t, double h);

#endif // CAMERA_H
