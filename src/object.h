#ifndef __OBJECT_H__
#define __OBJECT_H__
#include "meshes.h"
#include <math.h>
#include "irrKlang\irrKlang.h"

extern float gravity;

class object
{
public:
	vec3	center;			 // collision detection box center
	vec3	range_of_object; // x, y, z length from center
	vec3	velocity;
	irrklang::ISound* sound = nullptr;

	object() {
		center = vec3(0, 0, 0);
		range_of_object = vec3(0, 0, 0);
		velocity = vec3(0, 0, 0);
	}
	object(vec3 c, vec3 r, vec3 v) {
		center = c;
		range_of_object = r;
		velocity = v;
	}

	bool is_collision(object a) {
		vec3 center_vec = a.center - center; //center to a.center
		if (abs(center_vec.x) > a.range_of_object.x + range_of_object.x) return false;
		if (abs(center_vec.y) > a.range_of_object.y + range_of_object.y) return false;
		if (abs(center_vec.z) > a.range_of_object.z + range_of_object.z) return false;
		return true;
	}
	//준석 추가
	bool is_collision_fire(vec3 a) {
		vec3 center_vec = a - center; //center to a.center
		if (abs(center_vec.x) > range_of_object.x) return false;
		if (abs(center_vec.y) > range_of_object.y) return false;
		if (abs(center_vec.z) > range_of_object.z) return false;
		return true;
	}
	//
	int get_collision_axis(object a) {
		vec3 center_vec = a.center - center;
		vec3 ranSum = a.range_of_object + range_of_object;
		float x, y, z;
		x = ranSum.x - abs(center_vec.x);
		y = ranSum.y - abs(center_vec.y);
		z = ranSum.z - abs(center_vec.z);
		//center_vec = center_vec - (a.range_of_object + range_of_object);
		if (x <= y && x <= z && center_vec.x > 0) return 1;
		if (x <= y && x <= z && center_vec.x < 0) return 2;
		if (y <= x && y <= z && center_vec.y > 0) return 3;
		if (y <= x && y <= z && center_vec.y < 0) return 4;
		if (z <= x && z <= y && center_vec.z > 0) return 5;
		if (z <= x && z <= y && center_vec.z < 0) return 6;
		return 0;
	}
	bool is_nearby(object a) {
		vec3 center_vec = a.center - center;
		vec3 radi_vec = a.range_of_object - range_of_object;
		float enough2listen = 50000.0f;
		if (dot(center_vec, center_vec) < enough2listen + dot(radi_vec, radi_vec)) return true;
		return false;
	}
	bool is_front(object a) {
		vec3 center_vec = center - a.center;
		if (center_vec.y > 0 && center_vec.y > a.range_of_object.y + range_of_object.y) return true;
		return false;
	}
};

class character : public object
{
public:
	float head_radius;
	float move_speed;
	sphere_t head;
	cuboid_t body;
	cuboid_t leg[4];
	// 1D player see toward y axis.
	vec3 eye1;
	// 3D player eye
	vec3 eye2;

	character() : object() {
		head_radius = 0;
		move_speed = 0;
		head = sphere_t();
		body = cuboid_t();
		eye1 = head.center + head.radius * vec3(0, 1, 0);
		eye2 = head.center + head.radius * vec3(0, -3, 1) * 5.0f;
	}

	character(vec3 newCenter, vec3 v, float hr, float ms) : object(newCenter, vec3(hr, 2 * hr, hr), v) {
		head_radius = hr;
		move_speed = ms;
		head = { center + vec3(0, hr, 0), hr };
		body = { center + vec3(0, -hr * 0.5f, 0), vec3(0.5f, 1.5f, 0.5f) * hr };
		leg[0] = { center + vec3(-0.75f, -0.5f, -0.5f) * hr, vec3(0.25f, 0.125f, 0.5f) * hr };
		leg[1] = { center + vec3(-0.75f, -1.5f, -0.5f) * hr, vec3(0.25f, 0.125f, 0.5f) * hr };
		leg[2] = { center + vec3(0.75f, -0.5f, -0.5f) * hr, vec3(0.25f, 0.125f, 0.5f) * hr };
		leg[3] = { center + vec3(0.75f, -1.5f, -0.5f) * hr, vec3(0.25f, 0.125f, 0.5f) * hr };
		leg[0].rotationFlag = leg[1].rotationFlag = leg[2].rotationFlag = leg[3].rotationFlag = true;
		leg[0].legRotateFlag = leg[1].legRotateFlag = true;
		leg[2].legRotateFlag = leg[3].legRotateFlag = false;
		eye1 = head.center + head.radius * vec3(0, 1, 0);
		eye2 = head.center + head.radius * vec3(0, -3, 1) * 5.0f;
	}

	void update(float delta_t) {
		center = center += delta_t * velocity;
		velocity += gravity * vec3(0, 0, -1) * delta_t;
		head.center = center + vec3(0, head_radius, 0);
		body.center = center + vec3(0, -0.5f * head_radius, 0);
		leg[0].center = center + vec3(-0.75f, -0.5f, -0.5f) * head_radius;
		leg[1].center = center + vec3(-0.75f, -1.5f, -0.5f) * head_radius;
		leg[2].center = center + vec3(0.75f, -0.5f, -0.5f) * head_radius;
		leg[3].center = center + vec3(0.75f, -1.5f, -0.5f) * head_radius;
		eye1 = head.center + head.radius * vec3(0, 1, 0);
		eye2 = head.center + head.radius * vec3(0, -3, 1) * 5.0f;
		head.mesh_update(delta_t);
		body.mesh_update(delta_t);
		for (int i = 0; i < 4; i++) leg[i].mesh_update(delta_t);
	}
};

class cuboid : public object
{
public:
	cuboid_t body;

	cuboid() : object() { body = cuboid_t(); }
	cuboid(vec3 _center, vec3 _edge_ratio, vec3 _velocity) : object(_center, _edge_ratio, _velocity) { body = { _center, _edge_ratio }; }
	cuboid(vec3 _center, vec3 _edge_ratio) : object(_center, _edge_ratio, vec3(0, 0, 0)) { body = { _center, _edge_ratio }; }

	// public functions
	void update(float delta_t) {
		center = center += delta_t * velocity;
		body.center = center;
		body.mesh_update(delta_t);
	}
};

class sphere : public object
{
public:
	sphere_t body;

	sphere() : object() { body = sphere_t(); }
	sphere(vec3 _center, float radius, vec3 _velocity) : object(_center, vec3(1, 1, 1)* radius, _velocity) { body = { _center, radius }; }
	sphere(vec3 _center, float radius) : object(_center, vec3(1, 1, 1)* radius, vec3(0, 0, 0)) { body = { _center, radius }; }
	sphere(vec3 _center, float radius, vec3 _velocity, bool rFlag) : object(_center, vec3(1, 1, 1)* radius, _velocity) { body = { _center, radius, rFlag }; }
	sphere(vec3 _center, float radius, vec3 _velocity, vec3 rAxis, float rSpeed) : object(_center, vec3(1, 1, 1)* radius, _velocity) { body = { _center, radius, true, rAxis, rSpeed }; }

	// public functions
	void update(float delta_t) {
		center = center += delta_t * velocity;
		body.center = center;
		body.mesh_update(delta_t);
	}
};




#endif // __TRACKBALL_H__