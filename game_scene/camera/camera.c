#include "camera.h"

camera camera_create() {
	camera new_camera;
	new_camera.position.x = 0.0;
	new_camera.position.y = 0.0;
	new_camera.position.z = 0.0;
	new_camera.rotation.x = 0.0;
	new_camera.rotation.y = 0.0;
	new_camera.rotation.z = 0.0;
	new_camera.arcball_position.t = 0.25;
	new_camera.arcball_position.h = 0.5;
	new_camera.arcball_position.d = 4.5;
	new_camera.arcball_speed.t = 0.0;
	new_camera.arcball_speed.h = 0.0;
	new_camera.arcball_speed.d = 0.0;
	new_camera.disable_clamp = false;
	return new_camera;
}

double _degrees_to_radians(double degree) {
	return degree * M_PI / 180.0;
}

void _process_motion(camera* camera, double d_delta, double t_delta, double h_delta) {
	double d = camera->arcball_position.d + d_delta;
	double t = camera->arcball_position.t + t_delta;
	double h = camera->arcball_position.h + h_delta;
	if (!camera->disable_clamp) {
		d = clamp(d, D_MIN, D_MAX);
		h = clamp(h, H_MIN, H_MAX);
	}
	
	camera->position.x = d * cos(M_PI * h / 2.0) * sin(M_PI * t);
	camera->position.y = d * cos(M_PI * h / 2.0) * cos(M_PI * t);
	camera->position.z = d * sin(M_PI * h / 2.0);
	
	camera->arcball_position.d = d;
	camera->arcball_position.t = t;
	camera->arcball_position.h = h;

	camera->rotation.z = (t + 0.5) * -180.0;
	camera->rotation.x = h * -90.0;
}

void camera_process(camera* camera, double delta) {
	_process_motion(camera, camera->arcball_speed.d * delta, camera->arcball_speed.t * delta, camera->arcball_speed.h * delta);
}

void camera_set_view(camera* camera) {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glRotatef(-(camera->rotation.x + 90), 1.0, 0.0, 0.0);
	glRotatef(-(camera->rotation.z - 90), 0.0, 0.0, 1.0);
	glTranslatef(-camera->position.x, -camera->position.y, -camera->position.z);
}


void camera_rotate(camera* camera, double horizontal, double vertical) {
	camera->rotation.z += horizontal;
	camera->rotation.x += vertical;

	if (camera->rotation.z < 0) {
		camera->rotation.z += 360.0;
	}

	if (camera->rotation.z > 360.0) {
		camera->rotation.z -= 360.0;
	}

	if (camera->rotation.x < 0) {
		camera->rotation.x += 360.0;
	}

	if (camera->rotation.x > 360.0) {
		camera->rotation.x -= 360.0;
	}
}

void camera_set_t_speed(camera* camera, double speed) {
	camera->arcball_speed.t = speed;
}

void camera_set_h_speed(camera* camera, double speed) {
	camera->arcball_speed.h = speed;
}

void camera_set_d_speed(camera* camera, double speed) {
	camera->arcball_speed.d = speed;
}


void camera_update_position_mouse(camera* camera, double t_, double h_) {
	_process_motion(camera, 0.0, t_ * 0.001, h_ * 0.002);
}

void camera_update_zoom_mouse(camera* camera, double zoom) {
	_process_motion(camera, zoom * 0.5, 0.0, 0.0);
}

void camera_force_position(camera* camera, double d, double t, double h) {
	camera->arcball_position.t = t;
	camera->arcball_position.h = h;
	camera->arcball_position.d = d;
}
