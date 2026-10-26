#ifndef DICE_H
#define DICE_H

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include <load.h>
#include <draw.h>
#include <model.h>
#include <load_tex.h>
#include <common_math.h>
#include "../camera/camera.h"

#define BOARD_STEP 0.2575
#define LOD_DIST 100.0
#define DICE_DIAGONAL_SIZE 0.1838478 // 0.13 * sqrt(2)
#define DEG_TO_RAD 0.01745329
#define INVERSE_LERP_WEIGHT 0.997

typedef struct dice {
	struct {
		double x;
		double y;
		double z;
	} position;
	struct {
		Model model;
		GLuint texture_id;
		GLuint fake_shadow_texture_id;
	} dice;
	double rotation_mul;
	double rotation_alt;
	double rotation;
	bool roll;
	double roll_time;
	double roll_begin;
} dice;

dice dice_create();
int dice_process(dice* dice, double delta);
void dice_render_gl(dice* dice);

void dice_begin_roll(dice* dice, double uptime);

#endif // DICE_H
