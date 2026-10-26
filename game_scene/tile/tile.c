#include "tile.h"

void _generate_path(int offset) {
	const int path_outer_begin = 8 + offset * 13;
	const int path_outer_end = path_outer_begin - 2;
	const int path_inner_begin = 52 + offset * 5;
	const int path_inner_end = path_inner_begin + 4;
	int tile_num = 0;
	
	for (int i = path_outer_begin; i <= 51; i++) {
		global_paths[offset][tile_num++] = &global_tiles[i];
	}
	for (int i = 0; i <= path_outer_end; i++) {
		global_paths[offset][tile_num++] = &global_tiles[i];
	}
	
	for (int i = path_inner_begin; i <= path_inner_end; i++) {
		global_paths[offset][tile_num++] = &global_tiles[i];
	}
	global_paths[offset][tile_num] = &global_tiles[72];
}

void _tile_wing(int offset, int cos_rot, int sin_rot) {
	const int offset_outer = offset * 13;
	const int offset_inner = offset * 5;
	
	for (int i = 0 + offset_outer; i <= 5 + offset_outer; i++) {
		global_tiles[i].position.x = -1;
		global_tiles[i].position.y = 2 + (i - offset_outer);
	}
	global_tiles[6 + offset_outer].position.x = 0;
	global_tiles[6 + offset_outer].position.y = 7;
	for (int i = 7 + offset_outer; i <= 12 + offset_outer; i++) {
		global_tiles[i].position.x = 1;
		global_tiles[i].position.y = 14 - (i - offset_outer);
	}
	for (int i = 52 + offset_inner; i <= 56 + offset_inner; i++) {
		global_tiles[i].position.x = 0;
		global_tiles[i].position.y = 58 - (i - offset_inner);
	}
	
	for (int i = 0 + offset_outer; i <= 12 + offset_outer; i++) {
		const int x = global_tiles[i].position.x;
		const int y = global_tiles[i].position.y;
		global_tiles[i].position.x = cos_rot * x - sin_rot * y;
		global_tiles[i].position.y = sin_rot * x + cos_rot * y;
	}
	for (int i = 52 + offset_inner; i <= 56 + offset_inner; i++) {
		const int x = global_tiles[i].position.x;
		const int y = global_tiles[i].position.y;
		global_tiles[i].position.x = cos_rot * x - sin_rot * y;
		global_tiles[i].position.y = sin_rot * x + cos_rot * y;
	}
}

void tile_create() {
	_tile_wing(0, +1, +0);
	_tile_wing(1, +0, -1);
	_tile_wing(2, -1, +0);
	_tile_wing(3, +0, +1);
	
	global_tiles[72].position.x = 0;
	global_tiles[72].position.y = 0;
	
	_generate_path(0);
	_generate_path(1);
	_generate_path(2);
	_generate_path(3);
}

void tile_remove_piece(tile* tile, const int piece_uid) {
	int offset = 0;
	for (int i = 0; i < tile->on_tile.count; i++) {
		tile->on_tile.pieces[i + offset] = tile->on_tile.pieces[i];
		if (tile->on_tile.pieces[i]->uid == piece_uid) {
			tile->on_tile.pieces[i] = NULL;
			offset--;
		}
	}
	tile->on_tile.count--;
}

void tile_add_piece(tile* tile, play_piece* piece) {
	tile->on_tile.pieces[tile->on_tile.count++] = piece;
}

void tile_set_pieces_on_tile(tile* tile) {
	const double n = (double)tile->on_tile.count;
	double angle_step = (2.0 * M_PI) / tile->on_tile.count;
	double begin = 0.0;
	double alpha, x;
	switch (tile->on_tile.count) {
		case 0:
			return;
			break;
		case 1:
			alpha = 0.0;
			x = 0.0;
			break;
		case 2:
			begin = M_PI / 4.0;
			x = 1.0;
			break;
		default:
			begin = angle_step / 2.0;
			alpha = (M_PI * (n - 2)) / (2 * n);
			x = 1.0 / cos(alpha);
			break;
	}
	
	for (int i = 0; i < tile->on_tile.count; i++) {
		piece_move_to_coords(tile->on_tile.pieces[i], tile->position.x, tile->position.y, x, begin + angle_step * i);
	}
}

void tile_remove_enemy_pieces(tile* tile, play_piece* attacking_piece) {
	const int pieces = tile->on_tile.count;
	if (!(tile->position.x == 0 && tile->position.y == 0)) {
		if (pieces == 1 && tile->on_tile.pieces[0]->player_id != attacking_piece->player_id) {
			piece_return_to_yard(tile->on_tile.pieces[0]);
			tile_remove_piece(tile, tile->on_tile.pieces[0]->uid);
		} else if (pieces > 1 && tile->on_tile.pieces[0]->player_id != attacking_piece->player_id) {
			piece_return_to_yard(attacking_piece);
			return;
		}
	} else {
		attacking_piece->status = PIECE_STATUS_HOME;
	}
	tile_add_piece(tile, attacking_piece);
	tile_set_pieces_on_tile(tile);
}
