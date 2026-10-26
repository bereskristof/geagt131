#include "game_manager/game_manager.h"
#include <stdio.h>
#include <string.h>

#define SCREEN_SIZE 800
#define ASPECT_RATIO 16 / 10

tile global_tiles[TILES_ON_BOARD];
tile* global_paths[4][TILES_TO_MOVE];

int main(int argc, char* argv[])
{
	game_manager game = game_manager_create(SCREEN_SIZE * ASPECT_RATIO, SCREEN_SIZE, "Ludo");
	
	if (argc >= 2) {
		printf("Arguments passed: '%s'\n", argv[1]);
		if (strlen(argv[1]) >= 4) {
			for (int i = 0; i < 4; i++) {
				if (argv[1][i] == '1') {
					game.scene.is_player_ai[i] = true;
				} else if (argv[1][i] != '0') {
					printf("Unknown character: '%c', defaulting to player.\n", argv[1][i]);
				}
			}
		} else {
			printf("No enough arguments passed, defaulting to: '0000'\n");
		}
	} else {
		printf("No arguments passed, defaulting to: '0000'\n");
	}
	
	while (game.is_active) {
		game_manager_handle_input(&game);
		game_manager_update_scene(&game);
		game_manager_render_scene(&game);
	}
	game_manager_cleanup(&game);
	
	return 0;
}
