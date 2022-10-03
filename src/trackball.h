#ifndef __TRACKBALL_H__
#define __TRACKBALL_H__

struct trackball
{
	bool	b_tracking = false;
	float	scale;
	vec2	m0;
	mat4	save_rotation = mat4::identity();
	float theta_z = 0;
	float theta_x = 0;

	trackball(float rot_scale = 2.0f) : scale(rot_scale) {}
	bool is_tracking() const { return b_tracking; }
	void begin(vec2 m);
	void end() { b_tracking = false; save_rotation.set_identity(); }
	void update(vec2 m);
};

inline void trackball::begin(vec2 m)
{
	b_tracking = true;
	m0 = m;
}

inline void trackball::update(vec2 m)
{
	vec2 p2 = m - m0;
	theta_z = -PI * p2.x;
	theta_x = PI * p2.y * 0.5f;
	/*theta_x = abs(theta_z) < PI ? PI * p2.y * 0.5f : -PI * p2.y * 0.5f;*/
	save_rotation = mat4::rotate(vec3(0, 0, 1), theta_z) * mat4::rotate(vec3(1, 0, 0), theta_x);
}

// utility function
inline vec2 cursor_to_ndc(dvec2 cursor, ivec2 window_size)
{
	// normalize window pos to [0,1]^2
	vec2 npos = vec2(float(cursor.x) / float(window_size.x - 1),
		float(cursor.y) / float(window_size.y - 1));
	return vec2(npos.x * 2.0f - 1.0f, 1.0f - npos.y * 2.0f);
}

#endif // __TRACKBALL_H__