#include "play_piece.h"

play_piece piece_create(double targets[3], char texture_title[], int count, int player_id) {
	static int uid = 10;
	play_piece new_piece;
	load_model(&new_piece.piece.model, "./assets/piece/piece.obj");
	load_model(&new_piece.piece.model_low_lod, "./assets/piece/piece_ld.obj");
	char file_path[52];
	sprintf(file_path, "./assets/piece/piece_%s.png", texture_title);
	new_piece.piece.texture_id = load_texture(file_path);
	sprintf(file_path, "./assets/piece/piece_%s_focus.png", texture_title);
	new_piece.piece.focus_texture_id = load_texture(file_path);
	new_piece.piece.fake_shadow_texture_id = load_texture("./assets/piece/fake_ao.png");
	
	new_piece.position.x = 0.0;
	new_piece.position.y = 0.0;
	new_piece.position.z = 1.0;
	new_piece.target_position.x = targets[0];
	new_piece.target_position.y = targets[1];
	new_piece.target_position.z = targets[2];
	new_piece.yard_position.x = targets[0];
	new_piece.yard_position.y = targets[1];
	new_piece.yard_position.z = targets[2];
	new_piece.fake_shadow_height = count;
	new_piece.use_low_lod = false;
	new_piece.status = PIECE_STATUS_YARD;
	new_piece.tile_id = -1;
	new_piece.uid = uid++;
	new_piece.player_id = player_id;
	new_piece.selectable = false;
	return new_piece;
}

void piece_process(play_piece* piece) {
	piece->position.x = lerp(piece->position.x, piece->target_position.x, LERP_WEIGHT);
	piece->position.y = lerp(piece->position.y, piece->target_position.y, LERP_WEIGHT);
	piece->position.z = lerp(piece->position.z, piece->target_position.z, LERP_WEIGHT);
}

void _draw_fake_shadow(play_piece* piece) {
	const double shadow_size = 0.12 + 0.1 * piece->position.z;
	const double shadow_height = 0.0001 * (1 + piece->fake_shadow_height);
	glBindTexture(GL_TEXTURE_2D, piece->piece.fake_shadow_texture_id);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3d(piece->position.x + shadow_size, piece->position.y + shadow_size, shadow_height);
	glTexCoord2f(1.0, 0.0);
	glVertex3d(piece->position.x - shadow_size, piece->position.y + shadow_size, shadow_height);
	glTexCoord2f(1.0, 1.0);
	glVertex3d(piece->position.x - shadow_size, piece->position.y - shadow_size, shadow_height);
	glTexCoord2f(0.0, 1.0);
	glVertex3d(piece->position.x + shadow_size, piece->position.y - shadow_size, shadow_height);
	glEnd();
}

void piece_render_gl(play_piece* piece, double uptime) {
	glBindTexture(GL_TEXTURE_2D, (piece->selectable && (int)(uptime * 0.004) % 2 == 0) ? piece->piece.focus_texture_id : piece->piece.texture_id);
	glTranslatef(piece->position.x, piece->position.y, piece->position.z);
	draw_model(piece->use_low_lod ? &piece->piece.model_low_lod : &piece->piece.model);
	glTranslatef(-piece->position.x, -piece->position.y, -piece->position.z);
	_draw_fake_shadow(piece);
}


void piece_move_to_coords(play_piece* piece, const int x, const int y, const double r, const double angle_rad) {
	const double offset_x = cos(angle_rad);
	const double offset_y = sin(angle_rad);
	piece->target_position.x = (double)x * BOARD_STEP + offset_x * r * PIECE_RADIUS;
	piece->target_position.y = (double)y * BOARD_STEP + offset_y * r * PIECE_RADIUS;
	piece->target_position.z = 0.0;
}

void piece_test_lod_preference(play_piece* piece, camera* camera) {
	const double dx = camera->position.x - piece->position.x;
	const double dy = camera->position.y - piece->position.y;
	const double dz = camera->position.z - piece->position.z;
	const double d = dx * dx + dy * dy + dz * dz;
	piece->use_low_lod = (d > LOD_DIST);
}

void piece_return_to_yard(play_piece* piece) {
	piece->status = PIECE_STATUS_YARD;
	piece->tile_id = -1;
	piece->target_position.x = piece->yard_position.x;
	piece->target_position.y = piece->yard_position.y;
	piece->target_position.z = piece->yard_position.z;
}
