#include "dice.h"

dice dice_create() {
	dice new_dice;
	load_model(&new_dice.dice.model, "./assets/dice/dice.obj");
	new_dice.dice.texture_id = load_texture("./assets/dice/dice.png");
	new_dice.dice.fake_shadow_texture_id = load_texture("./assets/piece/fake_ao.png");
	
	new_dice.position.x = 0.0;
	new_dice.position.y = 0.0;
	new_dice.position.z = 0.0;
	new_dice.rotation_mul = 0.0;
	new_dice.rotation = 0.0;
	new_dice.rotation_alt = 0.0;
	
	new_dice.roll = false;
	new_dice.roll_time = 0.0;
	new_dice.roll_begin = 0.0;
	
	return new_dice;
}

int _get_roll_result(int rot, int rot_alt) {
	// #ifdef EBUG
	// return 6;
	// #endif
	int res[] = {
		3, 5, 4, 2,
		6, 5, 1, 2,
		4, 5, 3, 2,
		1, 5, 6, 2,
	};
	return res[(((rot_alt + 4) % 4) * 4) + (rot % 4)];
}

int dice_process(dice* dice, double delta) {
	if (dice->roll) {
		dice->roll_time += delta;
		dice->position.y = lerp(dice->position.y, 0.0, 1.0 - pow(INVERSE_LERP_WEIGHT, delta));
		dice->rotation_mul = lerp(1.0, 0.0, 1.0 - dice->position.y / 1.5);
		dice->rotation += dice->rotation_mul * 2.1 * delta;
		double rotation_limit = fabs(dice->rotation - floor(dice->rotation / 90.0) * 90.0);
		if ((dice->roll_time >= 650.0 && rotation_limit < 2.0) || dice->roll_time >= 1200.0) {
			dice->roll = false;
			dice->rotation_mul = 0.0;
			dice->rotation = floor((dice->rotation + 2.0) / 90.0) * 90.0;
			return _get_roll_result((int)(dice->rotation / 90.0), (int)(dice->rotation_alt / 90.0));
		}
	}
	return 0;
}

void _dice_draw_fake_shadow(dice* dice) {
	const double shadow_size = 0.26 + 0.1 * dice->position.z;
	const double shadow_height = 0.0001;
	glBindTexture(GL_TEXTURE_2D, dice->dice.fake_shadow_texture_id);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3d(dice->position.x + shadow_size, dice->position.y + shadow_size, shadow_height);
	glTexCoord2f(1.0, 0.0);
	glVertex3d(dice->position.x - shadow_size, dice->position.y + shadow_size, shadow_height);
	glTexCoord2f(1.0, 1.0);
	glVertex3d(dice->position.x - shadow_size, dice->position.y - shadow_size, shadow_height);
	glTexCoord2f(0.0, 1.0);
	glVertex3d(dice->position.x + shadow_size, dice->position.y - shadow_size, shadow_height);
	glEnd();
}

void dice_render_gl(dice* dice) {
	double rotation_range = fabs(45.0 - (dice->rotation - floor(dice->rotation / 90.0) * 90.0));
	double h = DICE_DIAGONAL_SIZE * cos(rotation_range * DEG_TO_RAD);
	
	_dice_draw_fake_shadow(dice);
	// glRotatef(180.0, 0.0, 0.0, 1.0);
	glTranslatef(dice->position.x, dice->position.y, h + dice->position.z);
	glRotatef(dice->rotation, 1.0, 0.0, 0.0);
	glRotatef(dice->rotation_alt, 0.0, 1.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, dice->dice.texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	draw_model(&dice->dice.model);
}


void dice_begin_roll(dice* dice, double uptime) {
	dice->position.x = -2.2;
	dice->position.y = 1.5;
	dice->position.z = 0.0;
	dice->rotation_mul = 1.0;
	dice->rotation = 90.0 * (rand() % 4);
	dice->rotation_alt = 90.0 * (rand() % 8 - 3);
	dice->rotation_alt = 90.0 * -3;
	
	dice->roll = true;
	dice->roll_time = 0.0;
	dice->roll_begin = uptime;
}
