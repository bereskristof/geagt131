#include "game_scene.h"

void _init_team_pieces(char title[], const int index, const double x, const double y, game_scene* scene) {
	for (size_t i = 0; i < 4; i++) {
		const double a = i / 2 ? 1.0 : -1.0;
		const double b = i % 2 ? 1.0 : -1.0;
		double pos[3] = {x * BOARD_STEP * (4.5 + a * PLAYER_QUAD_CENTRE_DISTANCE), y * BOARD_STEP * (4.5 + b * PLAYER_QUAD_CENTRE_DISTANCE), 0.0};
		scene->pieces[index][i] = piece_create(pos, title, i + index, index);
	}
}

game_scene scene_create() {
	tile_create();
	
	game_scene new_scene;
	
	new_scene.camera = camera_create();
	new_scene.dice = dice_create();
	new_scene.brightness = 1.0;
	new_scene.current_player_id = 0;
	new_scene.current_game_state = GAME_STATE_BEGIN;
	new_scene.moveable_pieces_count = 0;
	new_scene.value_rolled = 0;
	new_scene.focused_piece = -1;
	for (int i = 0; i < 4; i++) {
		new_scene.player_victory_progress[i] = 0;
		new_scene.is_player_ai[i] = false;
	}
	
	load_model(&new_scene.game_board.board_model, "./assets/board/board.obj");
	new_scene.game_board.texture_id = load_texture("./assets/board/board.png");
	
	load_model(&new_scene.background.model, "./assets/map/map.obj");
	new_scene.background.texture_id = load_texture("./assets/map/map.png");
	
	load_model(&new_scene.player_signal.model, "./assets/signal/signal.obj");
	new_scene.player_signal.texture_id = load_texture("./assets/signal/signal.png");
	
	new_scene.world_below = load_texture("./assets/ui/f2menu.png");
	
	_init_team_pieces("red", 0, 1.0, 1.0, &new_scene);
	_init_team_pieces("green", 1, 1.0, -1.0, &new_scene);
	_init_team_pieces("yellow", 2, -1.0, -1.0, &new_scene);
	_init_team_pieces("blue", 3, -1.0, 1.0, &new_scene);
	
	for (int i = 0; i < 4; i++) {
		new_scene.moveable_pieces[i] = NULL;
	}
	
	return new_scene;
}

void _scene_go_to_next_player(game_scene* scene) {
	scene->current_game_state = GAME_STATE_BEGIN;
	scene->current_player_id = (scene->current_player_id + 1) % 4;
	scene->value_rolled = 0;
	scene->moveable_pieces_count = 0;
	scene->focused_piece = -1;
	for (int i = 0; i < 4; i++) {
		if (scene->moveable_pieces[i] != NULL) {
			scene->moveable_pieces[i]->selectable = false;
			scene->moveable_pieces[i] = NULL;
		}
	}
}

void scene_process(game_scene* scene, double delta) {
	camera_process(&scene->camera, delta);
	int roll = dice_process(&scene->dice, delta);
	if (roll != 0) {
		scene->value_rolled = roll;
		
		for (int i = 0; i < 4; i++) {
			int status = scene->pieces[scene->current_player_id][i].status;
			if (status == PIECE_STATUS_PATH || (roll == 6 && status == PIECE_STATUS_YARD)) {
				scene->moveable_pieces[scene->moveable_pieces_count] = &scene->pieces[scene->current_player_id][i];
				scene->moveable_pieces_count += 1;
				scene->pieces[scene->current_player_id][i].selectable = true;
			}
		}
		
		if (scene->moveable_pieces_count == 0) {
			_scene_go_to_next_player(scene);
		} else {
			scene->current_game_state = GAME_STATE_PIECE_AWAIT;
		}
	}
	
	for (size_t i = 0; i < 16; i++) {
		piece_test_lod_preference(&scene->pieces[i / 4][i % 4], &scene->camera);
		piece_process(&scene->pieces[i / 4][i % 4]);
	}
}

void _set_global_lighting(game_scene* scene) {
	const float br = scene->brightness;
	const float amb[] = {0.4 * br, 0.35 * br + 0.05, 0.2 * br + 0.2, 1.0};
	const float dif[] = {1.0 * br, 0.8 * br + 0.2, 0.6 * br + 0.4, 1.0};
	const float pos[] = {0.0, 0.0, 2.0};
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
}

void _draw_world_below(game_scene* scene) {
	const double w = 2.5;
	const double h = w * 0.25;
	glBindTexture(GL_TEXTURE_2D, scene->world_below);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3d(w, h, -0.2);
	glTexCoord2f(0.0, 1.0);
	glVertex3d(w, -h, -0.2);
	glTexCoord2f(1.0, 1.0);
	glVertex3d(-w, -h, -0.2);
	glTexCoord2f(1.0, 0.0);
	glVertex3d(-w, h, -0.2);
	glEnd();
}

void scene_render_gl(game_scene* scene, double uptime) {
	_set_global_lighting(scene);
	
	glTranslatef(0.0, 0.0, 0.0);
	
	_draw_world_below(scene);
	
	glBindTexture(GL_TEXTURE_2D, scene->game_board.texture_id);
	draw_model(&scene->game_board.board_model);
	
	glBindTexture(GL_TEXTURE_2D, scene->background.texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	draw_model(&scene->background.model);
	
	glRotatef(scene->current_player_id * -90.0, 0.0, 0.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, scene->player_signal.texture_id);
	draw_model(&scene->player_signal.model);
	glRotatef(scene->current_player_id * 90.0, 0.0, 0.0, 1.0);
	
	for (size_t i = 0; i < 16; i++) {
		piece_render_gl(&scene->pieces[i / 4][i % 4], uptime);
	}
	
	dice_render_gl(&scene->dice);
}


void scene_set_brightness(game_scene* scene, int direction) {
	scene->brightness = clamp(scene->brightness + (direction * 0.1), 0.0, 1.0);
	
	double cr = lerp(NIGHT_R, DAY_R, scene->brightness);
	double cg = lerp(NIGHT_G, DAY_G, scene->brightness);
	double cb = lerp(NIGHT_B, DAY_B, scene->brightness);
	glClearColor(cr / 256.0, cg / 256.0, cb / 256.0, 1.0);
}

void scene_highlight_selected_piece(game_scene* scene, int piece_id) {
	if (piece_id >= scene->moveable_pieces_count) return;
	
	for (int i = 0; i < scene->moveable_pieces_count; i++) {
		if (i == piece_id) {
			scene->moveable_pieces[i]->target_position.z = 0.2;
			scene->focused_piece = i;
		} else {
			scene->moveable_pieces[i]->target_position.z = 0.0;
		}
	}
}

void scene_perform_selected_piece(game_scene* scene) {
	tile* tile,* current_tile;
	play_piece* active_piece = scene->moveable_pieces[scene->focused_piece];
	if (active_piece->status == PIECE_STATUS_YARD) {
		current_tile = NULL;
		active_piece->status = PIECE_STATUS_PATH;
		active_piece->tile_id = 0;
		tile = global_paths[scene->current_player_id][0];
	} else if (active_piece->status == PIECE_STATUS_PATH) {
		current_tile = global_paths[scene->current_player_id][active_piece->tile_id];
		active_piece->tile_id += scene->value_rolled;
		if (active_piece->tile_id > 56) {
			active_piece->tile_id = 56 - (active_piece->tile_id - 56);
		}
		tile = global_paths[scene->current_player_id][active_piece->tile_id];
	}
	
	tile_remove_enemy_pieces(tile, active_piece);
	if (current_tile != NULL) {
		tile_remove_piece(current_tile, active_piece->uid);
		tile_set_pieces_on_tile(current_tile);
	}
	if (active_piece->status == PIECE_STATUS_HOME) {
		scene->player_victory_progress[active_piece->player_id]++;
	}
	_scene_go_to_next_player(scene);
}
