#ifndef __PARTICLE_H__
#define __PARTICLE_H__
#pragma once

#include "cgmath.h"
#include "cgut.h"

inline float random_range(float min, float max) { return mix(min, max, rand() / float(RAND_MAX)); }

struct particle_t
{
	static constexpr int MAX_PARTICLES = 7;

	vec2 pos;
	vec4 color;
	vec2 velocity;
	float scale;
	float life;

	//optional
	float elapsed_time;
	float time_interval;
	float scale_f;

	particle_t() { reset(); }
	void reset();
	void update(float scale1, float t);
};

inline void particle_t::reset() {
	pos = vec2(random_range(-1.0f, 1.0f), random_range(0.3f, 1.0f)) * 0.4f * scale_f;
	color = vec4(random_range(0.8f, 1.0f), random_range(0.7f, 0.9f), 0, 1);
	scale = random_range(0.005f, 0.08f) * 1.5f * scale_f;
	life = random_range(0.01f, 1.0f);
	velocity = vec2(random_range(-0.5f, 0.5f), random_range(0.3f, 0.8f)) * 2.6f * scale_f;
	elapsed_time = 0.0f;
	time_interval = random_range(400.0f, 800.0f) * 2.0f;
}

inline void particle_t::update(float scale1, float t) {
	elapsed_time += t * 100.0f;

	if (scale_f != scale1) {
		scale_f = scale1;
	}

	pos += velocity * t;
	velocity.y -= 9.8f * t;
	constexpr float life_factor = 5.0f;
	life -= life_factor * t / scale_f;

	// disappear
	if (life < 0.0f) 	{
		constexpr float alpha_factor = 5.0f;
		color.a -= alpha_factor * t;
	}

	// dead
	if (color.a < 0.0f) reset();
}

#endif