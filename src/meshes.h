#ifndef __MESHES_H_
#define __MESHES_H_

class cuboid_t
{
public:
	vec3	center = vec3(0, 0, 0);												// 2D position for translation
	vec3	edge_ratio = vec3(1, 1, 1);											// edge ratio

	bool	rotationFlag = false;
	bool	legRotateFlag = true;
	vec3	rotationAxis = vec3(1, 0, 0);
	float	rotationSpeed = 3.0f;
	float	rotationAngle = 0.0f;


	vec4	color = vec4(1.0f, 0.0f, 0.0f, 1.0f);								// RGBA color in [0,1]
	mat4	model_matrix = mat4::identity();									// modeling transformation
	void	mesh_update(float delta_t);
};

class sphere_t
{
public:
	vec3	center = vec3(0, 0, 0);												// 3D position for translation
	float	radius = 1.0f;														// radius


	// rotation control
	bool	rotationFlag = false;
	vec3	rotationAxis = vec3(0, 0, 1);
	float	rotationSpeed = 3.0f;
	float	rotationAngle = -PI / 2;



	vec4	color = vec4(0.0f, 0.0f, 1.0f, 1.0f);								// RGBA color in [0,1]
	mat4	model_matrix = mat4::identity();									// modeling transformation
	void	mesh_update(float delta_t);
};

void cuboid_t::mesh_update(float delta_t) {
	mat4 transferMatrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, center.z,
		0, 0, 0, 1
	};

	mat4 scaleMatrix =
	{
		edge_ratio.x, 0, 0, 0,
		0, edge_ratio.y, 0, 0,
		0, 0, edge_ratio.z, 0,
		0, 0, 0, 1
	};

	model_matrix = mat4::identity();
	if (rotationFlag) 	{
		float delta_angle = delta_t * rotationSpeed;
		//if (rotationAngle < PI / 6 && rotationAngle + delta_angle > PI / 6) legRotateFlag = !legRotateFlag;
		//if (rotationAngle > -PI / 6 && rotationAngle - delta_angle < -PI / 6) legRotateFlag = !legRotateFlag;

		if (legRotateFlag && rotationAngle > PI / 6) legRotateFlag = !legRotateFlag;
		if (!legRotateFlag && rotationAngle < -PI / 6) legRotateFlag = !legRotateFlag;
		rotationAngle = legRotateFlag ? rotationAngle + delta_angle : rotationAngle - delta_angle;


		mat4 placeJointWell =
		{
			1, 0, 0, 0,
			0, 1, 0, edge_ratio.z * sin(rotationAngle),
			0, 0, 1, edge_ratio.z * (1 - cos(rotationAngle)),
			0, 0, 0, 1
		};

		model_matrix = placeJointWell * transferMatrix * mat4::rotate(rotationAxis, rotationAngle) * scaleMatrix * model_matrix;
	}
	else model_matrix = transferMatrix * scaleMatrix * model_matrix;
}

void sphere_t::mesh_update(float delta_t) {
	model_matrix =
	{
		radius, 0, 0, center.x,
		0, radius, 0, center.y,
		0, 0, radius, center.z,
		0, 0, 0, 1
	};

	if (rotationFlag) rotationAngle += delta_t * rotationSpeed;
	model_matrix = model_matrix * mat4::rotate(rotationAxis, rotationAngle);
}


#endif