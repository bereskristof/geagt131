#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/gl.h>
#include <stdbool.h>
#include <stdio.h>

#include <load.h>
#include <draw.h>
#include <model.h>
#include <load_tex.h>
#include <common_math.h>

#include "camera/camera.h"
#include "play_piece/play_piece.h"
#include "dice/dice.h"
#include "tile/tile.h"

#define PLAYER_QUAD_CENTRE_DISTANCE 0.8
#define DAY_R 78.0
#define DAY_G 172.0
#define DAY_B 183.0
#define NIGHT_R 4.0
#define NIGHT_G 19.0
#define NIGHT_B 42.0
#define GAME_STATE_BEGIN 0
#define GAME_STATE_DICE_AWAIT 1
#define GAME_STATE_PIECE_AWAIT 2

typedef struct game_scene {
	camera camera;
	dice dice;
	struct {
		Model board_model;
		GLuint texture_id;
	} game_board;
	struct {
		Model model;
		GLuint texture_id;
	} background;
	struct {
		Model model;
		GLuint texture_id;
	} player_signal;
	GLuint world_below;
	play_piece pieces[4][4];
	int moveable_pieces_count;
	play_piece* moveable_pieces[4];
	bool is_player_ai[4];
	int player_victory_progress[4];
	int focused_piece;
	float brightness;
	int current_player_id;
	// 0: Begin
	// 1: Rolling dice
	// 2: Dice roll finished
	int current_game_state;
	int value_rolled;
} game_scene;

game_scene scene_create();
void scene_process(game_scene* scene, double delta);
void scene_render_gl(game_scene* scene, double uptime);

void scene_set_brightness(game_scene* scene, int direction);
void scene_highlight_selected_piece(game_scene* scene, int piece_id);
void scene_perform_selected_piece(game_scene* scene);

#endif // GAME_SCENE_H
