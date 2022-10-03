#include "cgmath.h"	// slee's simple math library
#include "cgut.h"	// slee's OpenGL utility
#include "fire_particle.h"
#include "particle.h"

GLuint program_particle = 0;
GLuint vertex_array_particle = 0;
GLuint TEX_STAR = 0;
GLuint TEX_FIRE = 0;

static const char* vert_particle_path = "../bin/shaders/particle.vert";
static const char* frag_particle_path = "../bin/shaders/particle.frag";
static const char* image_path = "../bin/images/Doge1.jpg";
static const char* fire_path = "../bin/images/fire1.png";

std::vector<particle_t> particles;
std::vector<particle_fire_t> particles_fire;

bool particle_init() {
	if (!(program_particle = cg_create_program(vert_particle_path, frag_particle_path))) { glfwTerminate(); return 1; }	// create and compile shaders/program

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE2);		// notify GL the current texture slot is 0
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	static vertex vertices[] = { {vec3(-1,-1,0),vec3(0,0,1),vec2(0,0)}, {vec3(1,-1,0),vec3(0,0,1),vec2(1,0)}, {vec3(-1,1,0),vec3(0,0,1),vec2(0,1)}, {vec3(1,1,0),vec3(0,0,1),vec2(1,1)} }; // strip ordering [0, 1, 3, 2]

	// generation of vertex buffer: use vertices as it is
	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * 4, &vertices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array_particle) glDeleteVertexArrays(1, &vertex_array_particle);
	vertex_array_particle = cg_create_vertex_array(vertex_buffer);
	if (!vertex_array_particle) { printf("%s(): failed to create vertex aray\n", __func__); return false; }

	TEX_STAR = cg_create_texture(image_path, false); if (!TEX_STAR) return false; // disable mipmapping
	TEX_FIRE = cg_create_texture(fire_path, false); if (!TEX_FIRE) return false;
	// initialize particles
	particles.resize(particle_t::MAX_PARTICLES);
	particles_fire.resize(particle_fire_t::MAX_PARTICLES);
	for (int i = 0; i < particle_fire_t::MAX_PARTICLES; i++) {
		particle_fire_t t;
		t.pos_p = vec3(0);
		//t.reset();
		particles_fire.emplace_back(t);
	}

	return true;
}

void update_particle(float scale, vec3 posi, float t, bool reset) {

	for (auto& p : particles) p.update(scale, t);
	for (auto& p : particles_fire) {
		if (!reset) p.reset();
		p.update(scale, posi, t, reset);
	}

}

void render_particle(float aspect, mat4 view, mat4 proj, bool doge, bool fire) {

	glUseProgram(program_particle);



	//
	mat4 aspect_matrix =
	{
		std::min(1 / aspect,1.0f), 0, 0, 0,
		0, std::min(aspect,1.0f), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	// setup texture


	// enable blending
	if (doge) {
		glActiveTexture(GL_TEXTURE2);								// select the texture slot to bind
		glBindTexture(GL_TEXTURE_2D, TEX_STAR);
		glUniform1i(glGetUniformLocation(program_particle, "TEX_STAR"), 2);
		GLint uloc = glGetUniformLocation(program_particle, "aspect_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, aspect_matrix);



		// bind vertex array object
		glBindVertexArray(vertex_array_particle);
		for (auto& p : particles) 			{

			mat4 translate_matrix = mat4::translate(vec3(p.pos.x, p.pos.y, 0));
			mat4 scale_matrix = mat4::scale(p.scale);
			mat4 model_matrix = translate_matrix * scale_matrix;

			GLint uloc;
			uloc = glGetUniformLocation(program_particle, "color");			if (uloc > -1) glUniform4fv(uloc, 1, p.color);
			uloc = glGetUniformLocation(program_particle, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
			uloc = glGetUniformLocation(program_particle, "mode");		if (uloc > -1) glUniform1i(uloc, 0);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	}



	if (fire) {
		glActiveTexture(GL_TEXTURE2);								// select the texture slot to bind
		glBindTexture(GL_TEXTURE_2D, TEX_FIRE);
		glUniform1i(glGetUniformLocation(program_particle, "TEX_FIRE"), 2);
		glBindVertexArray(vertex_array_particle);
		for (auto& p : particles_fire) 			{

			mat4 translate_matrix = mat4::translate(vec3(p.pos.x, p.pos.y, p.pos.z));
			mat4 scale_matrix = mat4::scale(p.scale);
			mat4 model_matrix = translate_matrix * mat4::rotate(vec3(1, 0, 0).normalize(), 3 * PI / 4) * scale_matrix;

			GLint uloc;
			uloc = glGetUniformLocation(program_particle, "color");			if (uloc > -1) glUniform4fv(uloc, 1, p.color);
			uloc = glGetUniformLocation(program_particle, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
			uloc = glGetUniformLocation(program_particle, "view_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, view);
			uloc = glGetUniformLocation(program_particle, "mode");		if (uloc > -1) glUniform1i(uloc, 1);
			uloc = glGetUniformLocation(program_particle, "projection_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, proj);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	}


}


std::vector<particle_fire_t> getParticles() {
	return particles_fire;
}

void reset_particles() {
	for (int i = 0; i < (int)particles_fire.size(); i++)
		particles_fire[i].reset();
}