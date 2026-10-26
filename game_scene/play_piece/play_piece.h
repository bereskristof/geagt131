#ifndef PLAY_PIECE_H
#define PLAY_PIECE_H

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

#define LERP_WEIGHT (0.23 * 0.5)
#define BOARD_STEP 0.2575
#define LOD_DIST 100.0
#define PIECE_STATUS_YARD 0
#define PIECE_STATUS_PATH 1
#define PIECE_STATUS_HOME 2
#define PIECE_RADIUS 0.076

typedef struct play_piece {
	struct {
		double x;
		double y;
		double z;
	} position;
	struct {
		double x;
		double y;
		double z;
	} target_position;
	struct {
		double x;
		double y;
		double z;
	} yard_position;
	struct {
		Model model;
		Model model_low_lod;
		GLuint texture_id;
		GLuint focus_texture_id;
		GLuint fake_shadow_texture_id;
	} piece;
	int fake_shadow_height;
	bool use_low_lod;
	// 0: Yard
	// 1: Path
	// 2: Home
	int status;
	int tile_id;
	int uid;
	int player_id;
	bool selectable;
} play_piece;

play_piece piece_create(double targets[3], char texture_title[], int count, int player_id);
void piece_process(play_piece* piece);
void piece_render_gl(play_piece* piece, double uptime);

void piece_move_to_coords(play_piece* piece, const int x, const int y, const double r, const double angle_rad);
void piece_test_lod_preference(play_piece* piece, camera* camera);
void piece_return_to_yard(play_piece* piece);

#endif // PLAY_PIECE_H
