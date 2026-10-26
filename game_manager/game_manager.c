#include "game_manager.h"

void _shutdown_program(char* error_message) {
	fprintf(stderr, "\x1b[31mERROR\x1b[0m: %s\n", error_message);
	exit(-1);
}

game_manager game_manager_create(int window_width, int window_height, char* window_title) {
	game_manager new_game_manager;
	
	new_game_manager.is_active = false;
	
	int error = SDL_Init(SDL_INIT_EVERYTHING);
	if (error != 0) _shutdown_program("SDL failed to initialise!");
	
	new_game_manager.window = SDL_CreateWindow(
		window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		window_width, window_height, SDL_WINDOW_OPENGL
	);
	if (new_game_manager.window == NULL) _shutdown_program("SDL create window failure!");
	
	error = IMG_Init(IMG_INIT_PNG);
	if (error == 0) _shutdown_program("SDL image failed to initialise!");
	
	new_game_manager.gl_context = SDL_GL_CreateContext(new_game_manager.window);
	if (new_game_manager.gl_context == NULL) _shutdown_program("OpenGL create context failure!");
	
	_set_opengl_parameters();
	_resize_viewport(window_width, window_height);
	new_game_manager.is_active = true;
	new_game_manager.handle_input = true;
	
	new_game_manager.scene = scene_create();
	new_game_manager.internal_uptime = SDL_GetPerformanceCounter();
	new_game_manager.uptime = 0.0;
	new_game_manager.ai_waiter = 0;
	new_game_manager.increment_ai_waiter = false;
	
	srand(time(NULL));
	
	printf("Initialised game manager\n");
	return new_game_manager;
}

void _set_opengl_parameters() {
	glShadeModel(GL_SMOOTH);

	glEnable(GL_NORMALIZE);
	glEnable(GL_AUTO_NORMAL);

	glClearColor(DAY_R / 256.0, DAY_G / 256.0, DAY_B / 256.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearDepth(1.0);

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glCullFace(GL_BACK);
}

void _resize_viewport(int window_width, int window_height) {
	glViewport(0, 0, window_width, window_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(
		-(window_width * FRUTSUM_SCALE), (window_width * FRUTSUM_SCALE),
		-(window_height * FRUTSUM_SCALE), (window_height * FRUTSUM_SCALE),
		.1, 50
	);
}


void _display_black_texture(game_manager* manager) {
	camera_force_position(&manager->scene.camera, 4.5, 0.0, -1.0);
	glDisable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	manager->handle_input = false;
	manager->scene.camera.disable_clamp = true;
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void _handle_sdl_key_event_down(const SDL_Scancode scancode, game_manager* manager) {
	static bool render_wireframes = false;
	static bool is_f1_open = false;
	static double d;
	static double t;
	static double h;
	const double mov_speed = 0.5 * 0.0015;
	const double rot_speed = 0.25 * 0.0015;
	const double zom_speed = 2.5 * 0.0015;
	camera* camera = &manager->scene.camera;
	switch (scancode) {
		case SDL_SCANCODE_ESCAPE:
			manager->is_active = false;
			break;
		case SDL_SCANCODE_W:
		case SDL_SCANCODE_UP:
			camera_set_h_speed(camera, mov_speed);
			break;
		case SDL_SCANCODE_S:
		case SDL_SCANCODE_DOWN:
			camera_set_h_speed(camera, -mov_speed);
			break;
		case SDL_SCANCODE_A:
		case SDL_SCANCODE_LEFT:
			camera_set_t_speed(camera, rot_speed);
			break;
		case SDL_SCANCODE_D:
		case SDL_SCANCODE_RIGHT:
			camera_set_t_speed(camera, -rot_speed);
			break;
		case SDL_SCANCODE_LSHIFT:
			camera_set_d_speed(camera, -zom_speed);
			break;
		case SDL_SCANCODE_LCTRL:
			camera_set_d_speed(camera, zom_speed);
			break;
		case SDL_SCANCODE_MINUS:
			scene_set_brightness(&manager->scene, -1);
			break;
		case SDL_SCANCODE_EQUALS:
			scene_set_brightness(&manager->scene, 1);
			break;
		case SDL_SCANCODE_SPACE:
			if (manager->scene.is_player_ai[manager->scene.current_player_id]) return;
			if (manager->scene.current_game_state == GAME_STATE_BEGIN) {
				manager->scene.current_game_state = GAME_STATE_DICE_AWAIT;
				dice_begin_roll(&manager->scene.dice, manager->uptime);
			} else if (manager->scene.current_game_state == GAME_STATE_PIECE_AWAIT && manager->scene.focused_piece != -1) {
				scene_perform_selected_piece(&manager->scene);
			}
			break;
		case SDL_SCANCODE_1:
			if (manager->scene.is_player_ai[manager->scene.current_player_id]) return;
			scene_highlight_selected_piece(&manager->scene, 0);
			break;
		case SDL_SCANCODE_2:
			if (manager->scene.is_player_ai[manager->scene.current_player_id]) return;
			scene_highlight_selected_piece(&manager->scene, 1);
			break;
		case SDL_SCANCODE_3:
			if (manager->scene.is_player_ai[manager->scene.current_player_id]) return;
			scene_highlight_selected_piece(&manager->scene, 2);
			break;
		case SDL_SCANCODE_4:
			if (manager->scene.is_player_ai[manager->scene.current_player_id]) return;
			scene_highlight_selected_piece(&manager->scene, 3);
			break;
		case SDL_SCANCODE_0:
			render_wireframes = !render_wireframes;
			if (render_wireframes) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			} else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			break;
		case SDL_SCANCODE_B:
			camera_force_position(camera, 4.5, 0.25, 0.5);
			break;
		case SDL_SCANCODE_N:
			camera_force_position(camera, 4.5, 0.0, 0.5);
			break;
		case SDL_SCANCODE_M:
			camera_force_position(camera, 4.5, 0.0, 1.0);
			break;
		#ifdef EBUG
		case SDL_SCANCODE_X:
			manager->scene.player_victory_progress[1] = 4;
			break;
		#endif
		case SDL_SCANCODE_F1:
			if (!manager->handle_input && !is_f1_open) return;
			if (!is_f1_open) {
				d = camera->arcball_position.d;
				t = camera->arcball_position.t;
				h = camera->arcball_position.h;
				_display_black_texture(manager);
			} else {
				camera_force_position(camera, d, t, h);
				glEnable(GL_LIGHTING);
				manager->handle_input = true;
				scene_set_brightness(&manager->scene, 0);
				if (render_wireframes) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				manager->scene.camera.disable_clamp = false;
			}
			is_f1_open = !is_f1_open;
		default:
			break;
	}
}

void _handle_sdl_key_event_up(const SDL_Scancode scancode, game_manager* manager) {
	camera* camera = &manager->scene.camera;
	switch (scancode) {
		case SDL_SCANCODE_W:
		case SDL_SCANCODE_S:
		case SDL_SCANCODE_UP:
		case SDL_SCANCODE_DOWN:
			camera_set_h_speed(camera, 0.0);
			break;
		case SDL_SCANCODE_A:
		case SDL_SCANCODE_D:
		case SDL_SCANCODE_LEFT:
		case SDL_SCANCODE_RIGHT:
			camera_set_t_speed(camera, 0.0);
			break;
		case SDL_SCANCODE_LSHIFT:
		case SDL_SCANCODE_LCTRL:
			camera_set_d_speed(camera, 0.0);
			break;
		default:
			break;
	}
}

int _predict_move(game_scene* scene, play_piece* active_piece) {
	#define AI_HAS_ENEMY_LONE 9
	#define AI_IS_HOME 8
	#define AI_HAS_ALLY_LONE 7
	#define AI_ENTER_COLOURED_ZONE 6
	#define AI_PIECE_CAN_LEAVE_YARD 5
	#define AI_HAS_ALLY_FORT 4
	#define AI_STANDARD_MOVE 3
	#define AI_MOVE_WITHIN_COLOURED_ZONE 2
	#define AI_HAS_ENEMY_FORT 1
	
	if (active_piece->status == PIECE_STATUS_YARD) {
		return AI_PIECE_CAN_LEAVE_YARD;
	}
	
	int target_tile = active_piece->tile_id + scene->value_rolled;
	if (target_tile > 56) target_tile = 56 - (target_tile - 56);
	tile* tile = global_paths[scene->current_player_id][target_tile];
	
	if (tile->position.x == 0 && tile->position.y == 0) return AI_IS_HOME;
	if (tile->on_tile.count == 1 && tile->on_tile.pieces[0]->player_id != active_piece->player_id) return AI_HAS_ENEMY_LONE;
	if (tile->on_tile.count > 1 && tile->on_tile.pieces[0]->player_id != active_piece->player_id) return AI_HAS_ENEMY_FORT;
	if (tile->on_tile.count == 1 && tile->on_tile.pieces[0]->player_id == active_piece->player_id) return AI_HAS_ALLY_LONE;
	if (tile->on_tile.count > 1 && tile->on_tile.pieces[0]->player_id == active_piece->player_id) return AI_HAS_ALLY_FORT;
	if (target_tile > 50 && active_piece->tile_id <= 50) return AI_ENTER_COLOURED_ZONE;
	if (target_tile > 50 && active_piece->tile_id > 50) return AI_MOVE_WITHIN_COLOURED_ZONE;
	return AI_STANDARD_MOVE;
	
	#undef AI_MOVE_WITHIN_COLOURED_ZONE
	#undef AI_PIECE_CAN_LEAVE_YARD
	#undef AI_ENTER_COLOURED_ZONE
	#undef AI_HAS_ENEMY_FORT
	#undef AI_HAS_ENEMY_LONE
	#undef AI_STANDARD_MOVE
	#undef AI_HAS_ALLY_FORT
	#undef AI_HAS_ALLY_LONE
	#undef AI_IS_HOME
}

void _manage_ai_player(game_manager* manager) {
	if (!manager->increment_ai_waiter) {
		if (manager->scene.current_game_state == GAME_STATE_BEGIN || manager->scene.current_game_state == GAME_STATE_PIECE_AWAIT) {
			manager->increment_ai_waiter = true;
		}
	}
	if (manager->ai_waiter >= AI_WAIT_TIME) {
		if (manager->scene.current_game_state == GAME_STATE_BEGIN) {
			manager->scene.current_game_state = GAME_STATE_DICE_AWAIT;
			dice_begin_roll(&manager->scene.dice, manager->uptime);
		} else if (manager->scene.current_game_state == GAME_STATE_PIECE_AWAIT) {
			int max_performance_score = 0;
			int selected = -1;
			for (int i = 0; i < manager->scene.moveable_pieces_count; i++) {
				int score = _predict_move(&manager->scene, manager->scene.moveable_pieces[i]);
				if (score > max_performance_score) {
					selected = i;
					max_performance_score = score;
				}
			}
			if (manager->scene.current_game_state == GAME_STATE_PIECE_AWAIT && selected != -1) {
				scene_highlight_selected_piece(&manager->scene, selected);
				scene_perform_selected_piece(&manager->scene);
			}
		}
		manager->ai_waiter = 0;
		manager->increment_ai_waiter = false;
	}
}

void game_manager_handle_input(game_manager* manager) {
	camera* camera = &manager->scene.camera;
	SDL_Event event;
	static bool is_mouse_down = false;
	static int last_x = 0;
	static int last_y = 0;
	int x, y;
	
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				manager->is_active = false;
				return;
				break;
			case SDL_KEYDOWN:
				if (manager->handle_input || event.key.keysym.scancode == SDL_SCANCODE_F1) _handle_sdl_key_event_down(event.key.keysym.scancode, manager);
				break;
			case SDL_KEYUP:
				if (manager->handle_input || event.key.keysym.scancode == SDL_SCANCODE_F1) _handle_sdl_key_event_up(event.key.keysym.scancode, manager);
				break;
			case SDL_MOUSEBUTTONDOWN:
				is_mouse_down = true;
				break;
			case SDL_MOUSEBUTTONUP:
				is_mouse_down = false;
				break;
			case SDL_MOUSEWHEEL:
				if (manager->handle_input) camera_update_zoom_mouse(camera, -event.wheel.y);
				break;
			default:
				break;
		}
	}
	
	SDL_GetMouseState(&x, &y);
	if (is_mouse_down) {
		double dx = (double)last_x - x;
		double dy = (double)last_y - y;
		if (manager->handle_input) camera_update_position_mouse(camera, -dx, -dy);
	}
	last_x = x;
	last_y = y;
	
	if (manager->scene.is_player_ai[manager->scene.current_player_id] && manager->handle_input) {
		_manage_ai_player(manager);
	}
}

void game_manager_update_scene(game_manager* manager) {
	long long current_time = SDL_GetPerformanceCounter();
	double delta = ((current_time - manager->internal_uptime) * 1000 / (double)SDL_GetPerformanceFrequency());
	manager->internal_uptime = current_time;
	manager->uptime += delta;
	if (manager->increment_ai_waiter) {
		manager->ai_waiter += delta;
	}
	
	scene_process(&manager->scene, delta);
	for (int i = 0; i < 4; i++) {
		if (manager->scene.player_victory_progress[i] >= 4) {
			manager->handle_input = false;
			printf("Player %d won!\n", i);
			manager->scene.player_victory_progress[i] = 0;
			char file_path[32];
			sprintf(file_path, "./assets/ui/%dwon.png", i);
			manager->scene.world_below = load_texture(file_path);
			_display_black_texture(manager);
		}
	}
}

void game_manager_render_scene(game_manager* manager) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	
	glPushMatrix();
	camera_set_view(&manager->scene.camera);
	scene_render_gl(&manager->scene, manager->uptime);
	glPopMatrix();
	
	SDL_GL_SwapWindow(manager->window);
}


void game_manager_cleanup(game_manager* manager) {
	if (manager->gl_context != NULL) SDL_GL_DeleteContext(manager->gl_context);
	if (manager->window != NULL) SDL_DestroyWindow(manager->window);
	SDL_Quit();
	printf("Cleaned up game manager\n");
}
