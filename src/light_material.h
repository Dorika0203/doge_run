#ifndef __LIGHT_MATERIAL_H__
#define __LIGHT_MATERIAL_H__

class light_t
{
public:
    vec4	position = vec4(1.0f, 0.0f, 10.0f, 0.0f);
    vec4	ambient = vec4(0.3f, 0.3f, 0.3f, 1.0f);
    vec4	diffuse = vec4(0.8f, 0.8f, 0.8f, 1.0f);
    vec4	specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
};

class material_t
{
public:
    vec4	ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
    vec4	diffuse = vec4(0.8f, 0.8f, 0.8f, 1.0f);
    vec4	specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    float	shininess = 1000.0f;
};


#endif