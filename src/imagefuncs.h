#pragma once
#ifndef __IMAGE_H__
#define __IMAGE_H__
#define MAX_CIRCLE 255

extern float ellasped_mat[MAX_CIRCLE][MAX_CIRCLE];
extern float ellasped_mat_next[MAX_CIRCLE][MAX_CIRCLE];
struct image_t {
	vec2	center = vec2(0.0f, 0.0f);		// 2D position for translation
	float	radius = 1.0f;		// radius
	float	theta = 0.0f;			// rotation angle
	vec4	color;				// RGBA color in [0,1]
	mat4	model_matrix;		// modeling transformation
	vec2	v;		  //speed now
	int     circle_id;
	void	update(float ratio, float time_gap);
};

mat4 update(float scale_x, float scale_y, float x_coor, float y_coor) {

	mat4 translate_matrix;

	mat4 scale_matrix = //크기조절
	{
		scale_x, 0, 0, 0,
		0, scale_y, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 rotation_matrix = // 회전
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	translate_matrix = // 이동
	{
		1, 0, 0, x_coor,
		0, 1, 0, y_coor,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 model_matrix = translate_matrix * rotation_matrix * scale_matrix;
	return model_matrix;
}

#endif