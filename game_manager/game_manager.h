#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include "../game_scene/game_scene.h"
#include "../game_scene/camera/camera.h"
#define FRUTSUM_SCALE 0.00006
#define AI_WAIT_TIME 200

typedef struct game_manager {
	SDL_Window* window;
	SDL_GLContext gl_context;
	bool is_active;
	bool handle_input;
	long long internal_uptime;
	long long uptime;
	game_scene scene;
	bool increment_ai_waiter;
	long long ai_waiter;
} game_manager;

game_manager game_manager_create(int window_width, int window_height, char* window_title);
void _set_opengl_parameters();
void _resize_viewport(int window_width, int window_height);

void game_manager_handle_input(game_manager* manager);
void game_manager_update_scene(game_manager* manager);
void game_manager_render_scene(game_manager* manager);

void game_manager_cleanup(game_manager* manager);

#endif // GAME_MANAGER_H
