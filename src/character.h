#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "meshes.h"

class character_t
{
public:
	vec3 center = vec3(0, 0, 5);
	vec3 velocity = vec3(0, 5, 0);
	float head_radius = 5.0f;
	float move_speed = 5.0f;

	sphere_t head = { center + vec3(0,0,head_radius), head_radius, vec4(1, 0, 0, 1) };
	cuboid_t body = { center, vec3(head_radius/2.0f , head_radius/2.0f, head_radius), vec4(0, 0, 1, 1) };

	// 1D player see toward y axis.
	vec3 eye1 = head.center + head.radius * vec3(0,1,0);
	// 3D player eye
	vec3 eye2 = head.center + sqrt(head.radius) * vec3(0,-18,3);

	// public functions
	void	update(float t);
};

void character_t::update(float t)
{
	center = center += t * velocity*10.0f;
	head.center = center + vec3(0, 0, head_radius);
	body.center = center;
	eye1 = head.center + head.radius * vec3(0, 1, 0);
	eye2 = head.center + head.radius * vec3(0, -18*sqrt(head_radius), 3*sqrt(head_radius));
	head.update(t);
	body.update(t);
}

#endif
