#ifndef TILE_H
#define TILE_H

#include "../play_piece/play_piece.h"

typedef struct tile {
	struct {
		int x;
		int y;
	} position;
	struct {
		play_piece* pieces[16];
		int count;
	} on_tile;
} tile;

#define TILES_ON_BOARD (18 * 4 + 1)
#define TILES_TO_MOVE (13 * 4 + 5)
// NOTE: Defined in main.c
extern tile global_tiles[TILES_ON_BOARD];
// NOTE: Defined in main.c
extern tile* global_paths[4][TILES_TO_MOVE];

void tile_create();
void tile_remove_piece(tile* tile, const int piece_uid);
void tile_add_piece(tile* tile, play_piece* piece);
void tile_set_pieces_on_tile(tile* tile);
void tile_remove_enemy_pieces(tile* tile, play_piece* attacking_piece);

#endif // TILE_H
