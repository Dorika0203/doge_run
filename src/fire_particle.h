
#ifndef __PARTICLE_FIRE_H__
#define __PARTICLE_FIRE_H__
#pragma once

#include "cgmath.h"
#include "cgut.h"

inline float random_range1(float min, float max) { return mix(min, max, rand() / float(RAND_MAX)); }

struct particle_fire_t
{
	static constexpr int MAX_PARTICLES = 30;

	vec3 pos;
	vec4 color;
	vec3 velocity;
	float scale;
	float life;

	//optional
	float elapsed_time;
	float time_interval;

	vec3 pos_p;
	bool flag = false;
	float scale_f;
	particle_fire_t() { reset(); }
	void reset();
	void update(float t, vec3 posi, float scale1, bool reset);
};

inline void particle_fire_t::reset() {
	pos = vec3(0, 0.5f, 0);
	if (flag)
		pos = vec3(0, 1.5f, 0.5f) + pos_p;
	color = vec4(random_range1(0.8f, 1.0f), random_range1(0.7f, 0.9f), 0, 1);
	scale = random_range1(0.005f, 0.08f) * 50.5f;
	life = random_range1(0.01f, 1.0f) * 0.02f;
	velocity = vec3(random_range1(-0.1f, 0.1f), 1.5f, random_range1(-0.1f, 0.1f)) * 70.3f;
	elapsed_time = 0.0f;
	time_interval = random_range1(400.0f, 800.0f) * 2.0f;
}

inline void particle_fire_t::update(float scale1, vec3 posi, float t, bool reset1) {
	pos_p = posi;
	flag = true;
	if (scale_f != scale1) {
		scale_f = scale1;
	}

	elapsed_time += t * 100.0f;
	if (reset1) {
		pos += velocity * t;
		constexpr float life_factor = 0.85f;
		life -= life_factor * t;

		// disappear
		if (life < 0.0f) 			{
			constexpr float alpha_factor = 0.85f;
			color.a -= alpha_factor * t;
		}

		// dead
		if (color.a < 0.0f) { reset(); }
	}

}

#endif