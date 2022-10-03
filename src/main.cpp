#include "cgmath.h"		// slee's simple math library
#define STB_IMAGE_IMPLEMENTATION
#include "cgut.h"		// slee's OpenGL utility
#include "trackball.h"
#include "light_material.h"
#include "irrKlang\irrKlang.h"
#pragma comment(lib, "irrKlang.lib")
#include "sound.h"
#include "meshes.h"
#include "object.h"
#include <windows.h> 

#include "imagefuncs.h" 
//#include "particle.h"//준석 추가
#include "fire_particle.h"
#include <String>

static const char* window_name = "Final_Project";
static const char* vert_shader_path = "../bin/shaders/main.vert";
static const char* frag_shader_path = "../bin/shaders/main.frag";

static const char* first_step_path = "../bin/sounds/first_step.mp3";
static const char* pirates_path = "../bin/sounds/pirates.mp3";
static const char* fire_path = "../bin/sounds/fire_breath1.mp3";

float gravity = 190.8f;
float diff_mv = 300.0f;
int   frame_val = 0;
int		count_ult = 0;
struct camera
{
	vec3	eye = vec3(0, 30, 100);
	vec3	at = vec3(0, 0, 0);
	vec3	up = vec3(0, 10, -3);
	mat4	view_matrix = mat4::look_at(eye, at, up);

	float	fovy = PI / 4.0f; // must be in radian
	float	aspect = 1.0f;
	float	dnear = 1.0f;
	float	dfar = 10000.0f;
	mat4	projection_matrix;
};

// camera, trackball, lightcam
camera		cam;
trackball	tb;
light_t		light = { vec4(0, 0, 1, 0) };
bool	view3_flag = false;

// GL variables
GLFWwindow* window;
ivec2		window_size = cg_default_window_size();
GLuint	program;
GLuint	vertex_array;
mesh* sphereMesh;
mesh* cylnderMesh;
mesh* coneMesh;
mesh* ringMesh;
mesh* cuboidMesh;

static const char* mp3_path = "../bin/sounds/11months.mp3";
static const char* avengers_path = "../bin/sounds/avengers.mp3";
static const char* himusk_path = "../bin/sounds/himusk.mp3";
static const char* dogbark_path = "../bin/sounds/dogbark.mp3";
static const char* doge_path = "../bin/sounds/doge.mp3";
static const char* doge_jump_path = "../bin/sounds/dog_jump.mp3";
static const char* someday_path = "../bin/sounds/someday.mp3";
static const char* bark_sh1_1_path = "../bin/sounds/bark_sh1_1.mp3";
static const char* bark_sh2_1_path = "../bin/sounds/bark_sh2_1.mp3";
static const char* doge_ending_path = "../bin/sounds/doge_ending.mp3";

irrklang::vec3df listener_pos;
irrklang::vec3df listener_up;
irrklang::vec3df look_direction;
// textures
GLuint rainbowTex, buildingTex, elonTex, starMapTex, furTex, dogeFaceTex;
GLuint earthTex, marsTex;

bool pause = false; //for pauie
// time
int		frame;
float	t;
float	before_time;

// game
int difficulty;
int game_flag; // 0 die, 1 end, 2 playing
bool w_flag, a_flag, d_flag, s_flag;

#ifndef GL_ES_VERSION_2_0
bool	b_wireframe = false;
#endif

//objects
character player;
cuboid board;
sphere earth;
sphere mars;
sphere star_map;

std::vector<cuboid> buildings;
std::vector<sphere> obstacles;

// functions for program
void show_intro();
void show_video(float _before_time);
int show_ingame_interface();
void mouse_intro(GLFWwindow* window, int button, int action, int mods); //변경
void motion_intro(GLFWwindow* window, double x, double y);
void keyboard_intro(GLFWwindow* window, int key, int scancode, int action, int mods);
void keyboard_help(GLFWwindow* window, int key, int scancode, int action, int mods);
void keyboard_credit(GLFWwindow* window, int key, int scancode, int action, int mods);
int show_difficulty_select();
int start_game();
void show_help_menu();
void pause_game();
void restart_game();
void show_ending_credit();

// functions for game
void update();
void render();
void reshape(GLFWwindow* window, int width, int height);
void mouse(GLFWwindow* window, int button, int action, int mods);
void motion(GLFWwindow* window, double x, double y);
void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
bool user_init();
void user_finalize();
bool game_init();
void game_finalize();
void create_obstacles();
float get_randomVal() { return (float)rand() * 2 / (float)RAND_MAX - 1.0f; }

// functions for text rendering.
bool init_text();
void render_text(std::string text, GLint x, GLint y, GLfloat scale, vec4 color, GLfloat dpi_scale = 1.0f);


//functions for particle 준석 추가

bool particle_init();
void update_particle(float scale, vec3 posi, float t, bool reset);//준석 수정
void render_particle(float aspect, mat4 view, mat4 proj, bool doge, bool fire);
bool flag_for_doge = false;
bool flag_for_fire = false;

std::vector<particle_fire_t> getParticles();
void reset_particles();

int main(int argc, char* argv[])
{
	int program_flag = 0;
	// create window and initialize OpenGL extensions
	if (!(window = cg_create_window(window_name, window_size.x, window_size.y))) { glfwTerminate(); return 1; }
	if (!cg_init_extensions(window)) { glfwTerminate(); return 1; }	// init OpenGL extensions

	// initializations and validations of GLSL program
	if (!(program = cg_create_program(vert_shader_path, frag_shader_path))) { glfwTerminate(); return 1; }	// create and compile shaders/program
	if (!user_init()) { printf("Failed to user_init()\n"); glfwTerminate(); return 1; }						// user initialization

	// register event callbacks - reshape.
	glfwSetWindowSizeCallback(window, reshape);
	float now = float(glfwGetTime());
	
	show_video(now);
	while (true)
	{
		show_intro();
		// when game is end.
		if (difficulty == 123) break;
		program_flag = start_game();
		if (program_flag == 0) {  play3dsound(dogbark_path, &player, false); }
		// when one game is ended.
		if (program_flag == 1) show_ending_credit();
	}

	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}

void update()
{
	// update global simulation parameter
	t = float(glfwGetTime());
	if (!pause) {
		player.update(t - before_time);

		// camera update
		if (view3_flag) 	{
			cam.eye = player.eye2;
			cam.at = player.head.center;
			cam.up = vec3(0, 1, 3); //dont change
		}
		else 	{
			cam.eye = player.eye1;
			cam.at = (player.eye1 + vec3(0, 1, 0));
			cam.up = vec3(0, 0, 1); //dont change
		}

		listener_pos = reinterpret_cast<irrklang::vec3df&> (cam.eye);
		listener_up = reinterpret_cast<irrklang::vec3df&> (cam.up);
		look_direction = reinterpret_cast<irrklang::vec3df&> (cam.at);
		irrklang::vec3df vel = reinterpret_cast<irrklang::vec3df&> (player.velocity);

		listener_pos.Z = -listener_pos.Z;
		look_direction.Z = -look_direction.Z;
		listener_up.Z = -listener_up.Z;
		vel.Z = -vel.Z;

		engine->setListenerPosition(listener_pos, look_direction, vel * 0.5f, listener_up);
		if (view3_flag)
			update_particle(1.0f, vec3(player.center.x, player.center.y+ player.range_of_object.y + 1, player.center.z), t - before_time, flag_for_fire);
		else
			update_particle(2.0f, vec3(player.center.x, player.center.y+ player.range_of_object.y + 1, player.center.z), t - before_time, flag_for_fire);

		// mars arrival check
		// board check
		// earth update and check
		// building update and check
		// obstacles update and check

		// mars arrival check
		earth.update(t - before_time);
		mars.update(t - before_time);

		if (player.is_collision(mars)) game_flag = 1;
		if (player.is_collision(earth)) game_flag = 0;

		if (player.is_collision(board)) {
			player.center.z = player.head_radius;
			player.velocity.z = 0;
		}
		if (player.center.z < -50.0f) game_flag = 0;

		for (size_t i = 0; i < buildings.size(); i++) 	{
			// check building collision.
			if (player.is_collision(buildings.at(i))) 		{
				//game_flag = 0;
				int collision_axis = player.get_collision_axis(buildings.at(i));
				switch (collision_axis) 			{
				case 1:
					player.center.x = buildings.at(i).center.x - buildings.at(i).range_of_object.x - player.range_of_object.x;
					break;
				case 2:
					player.center.x = buildings.at(i).center.x + buildings.at(i).range_of_object.x + player.range_of_object.x;
					break;
				case 3:
					player.center.y = buildings.at(i).center.y - buildings.at(i).range_of_object.y - player.range_of_object.y;
					break;
				case 4:
					player.center.y = buildings.at(i).center.y + buildings.at(i).range_of_object.y + player.range_of_object.y;
					break;
				case 5:
					player.center.z = buildings.at(i).center.z - buildings.at(i).range_of_object.z - player.range_of_object.z;
					player.velocity.z = 0;
					break;
				case 6:
					player.center.z = buildings.at(i).center.z + buildings.at(i).range_of_object.z + player.range_of_object.z;
					player.velocity.z = 0;
					break;
				default:
					printf("ERROR\n");
					game_flag = 0;
				}
				flag_for_doge = true;
			}
			// check if building is passed.
		}

		// obstacles creation, update and check.
		create_obstacles();
		bool continueFlag;
		for (size_t i = 0; i < obstacles.size(); i++) {
			continueFlag = false;
			obstacles.at(i).update(t - before_time);
			if (player.is_collision(obstacles.at(i))) {  game_flag = 0; }
			if (obstacles.at(i).center.y < 0) { obstacles.erase(obstacles.begin() + i); i--; continue; }
			for (size_t j = 0; j < 30; j++) {
				if (obstacles.at(i).is_collision_fire(getParticles()[j].pos)) {
					obstacles.erase(obstacles.begin() + i);
					continueFlag = true;
					break;
				}
			}
			if (continueFlag) continue;
			if (player.is_nearby(obstacles.at(i))) {
				if(i%2)
					play3dsound(himusk_path, &obstacles.at(i), true);
				else
					play3dsound(someday_path, &obstacles.at(i), true);
			}
			else {
				drop3d(&obstacles.at(i));
			}
		}


		// camera maker
		cam.eye = mat3(tb.save_rotation) * (cam.eye - cam.at) + cam.at;
		cam.up = mat3(tb.save_rotation) * cam.up;
		cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);

		// tricky aspect correction matrix for non-square window
		cam.aspect = window_size.x / float(window_size.y);
		cam.projection_matrix = mat4::perspective(cam.fovy, cam.aspect, cam.dnear, cam.dfar);
	}
	before_time = t;
	
}


void render()
{
	GLint uloc;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);


	uloc = glGetUniformLocation(program, "show_flag");				if (uloc > -1) glUniform1i(uloc, 0);
	uloc = glGetUniformLocation(program, "view_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.view_matrix);
	uloc = glGetUniformLocation(program, "projection_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.projection_matrix);
	uloc = glGetUniformLocation(program, "shiness");				if (uloc > -1) glUniform1f(uloc, 1000.0f);
	uloc = glGetUniformLocation(program, "light_position");			if (uloc > -1) glUniform4fv(uloc, 1, light.position);
	uloc = glGetUniformLocation(program, "Ia");						if (uloc > -1) glUniform4fv(uloc, 1, light.ambient);
	uloc = glGetUniformLocation(program, "Id");						if (uloc > -1) glUniform4fv(uloc, 1, light.diffuse);
	uloc = glGetUniformLocation(program, "Is");						if (uloc > -1) glUniform4fv(uloc, 1, light.specular);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(program, "TEX"), 0);


	// cuboids -> normal vector modified needed.
	glBindVertexArray(cuboidMesh->vertex_array);

	// board
	glBindTexture(GL_TEXTURE_2D, rainbowTex);
	uloc = glGetUniformLocation(program, "color_flag");				if (uloc > -1) glUniform1i(uloc, 1);
	uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, board.body.model_matrix);
	uloc = glGetUniformLocation(program, "solid_color");			if (uloc > -1) glUniform4fv(uloc, 1, board.body.color);
	glDrawElements(GL_TRIANGLES, cuboidMesh->index_list.size(), GL_UNSIGNED_INT, nullptr);

	// buildings
	glBindTexture(GL_TEXTURE_2D, buildingTex);
	for (size_t i = 0; i < buildings.size(); i++) 	{
		uloc = glGetUniformLocation(program, "color_flag");			if (uloc > -1) glUniform1i(uloc, 2);
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, buildings.at(i).body.model_matrix);
		uloc = glGetUniformLocation(program, "solid_color");		if (uloc > -1) glUniform4fv(uloc, 1, buildings.at(i).body.color);
		glDrawElements(GL_TRIANGLES, cuboidMesh->index_list.size(), GL_UNSIGNED_INT, nullptr);
	}

	// player - body
	glBindTexture(GL_TEXTURE_2D, furTex);
	uloc = glGetUniformLocation(program, "color_flag");				if (uloc > -1) glUniform1i(uloc, 2);
	uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, player.body.model_matrix);
	uloc = glGetUniformLocation(program, "solid_color");			if (uloc > -1) glUniform4fv(uloc, 1, player.body.color);
	glDrawElements(GL_TRIANGLES, cuboidMesh->index_list.size(), GL_UNSIGNED_INT, nullptr);

	// player - leg
	for (int i = 0; i < 4; i++) 	{
		uloc = glGetUniformLocation(program, "color_flag");				if (uloc > -1) glUniform1i(uloc, 2);
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, player.leg[i].model_matrix);
		uloc = glGetUniformLocation(program, "solid_color");			if (uloc > -1) glUniform4fv(uloc, 1, player.leg[i].color);
		glDrawElements(GL_TRIANGLES, cuboidMesh->index_list.size(), GL_UNSIGNED_INT, nullptr);
	}

	// sphere -> normal vector modified unneed.
	glBindVertexArray(sphereMesh->vertex_array);

	// player - head
	glBindTexture(GL_TEXTURE_2D, dogeFaceTex);
	uloc = glGetUniformLocation(program, "color_flag");				if (uloc > -1) glUniform1i(uloc, 2);
	uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, player.head.model_matrix);
	uloc = glGetUniformLocation(program, "solid_color");			if (uloc > -1) glUniform4fv(uloc, 1, player.head.color);
	glDrawElements(GL_TRIANGLES, sphereMesh->index_list.size(), GL_UNSIGNED_INT, nullptr);

	glBindTexture(GL_TEXTURE_2D, elonTex);
	for (size_t i = 0; i < obstacles.size(); i++) 	{
		uloc = glGetUniformLocation(program, "color_flag");				if (uloc > -1) glUniform1i(uloc, 2);
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, obstacles.at(i).body.model_matrix);
		uloc = glGetUniformLocation(program, "solid_color");			if (uloc > -1) glUniform4fv(uloc, 1, obstacles.at(i).body.color);
		glDrawElements(GL_TRIANGLES, sphereMesh->index_list.size(), GL_UNSIGNED_INT, nullptr);
	}

	// earth
	glBindTexture(GL_TEXTURE_2D, earthTex);
	uloc = glGetUniformLocation(program, "color_flag");				if (uloc > -1) glUniform1i(uloc, 2);
	uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, earth.body.model_matrix);
	uloc = glGetUniformLocation(program, "solid_color");			if (uloc > -1) glUniform4fv(uloc, 1, earth.body.color);
	glDrawElements(GL_TRIANGLES, sphereMesh->index_list.size(), GL_UNSIGNED_INT, nullptr);

	// mars
	glBindTexture(GL_TEXTURE_2D, marsTex);
	uloc = glGetUniformLocation(program, "color_flag");				if (uloc > -1) glUniform1i(uloc, 2);
	uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, mars.body.model_matrix);
	uloc = glGetUniformLocation(program, "solid_color");			if (uloc > -1) glUniform4fv(uloc, 1, mars.body.color);
	glDrawElements(GL_TRIANGLES, sphereMesh->index_list.size(), GL_UNSIGNED_INT, nullptr);


	// star map
	glBindTexture(GL_TEXTURE_2D, starMapTex);
	uloc = glGetUniformLocation(program, "color_flag");				if (uloc > -1) glUniform1i(uloc, 2);
	uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, star_map.body.model_matrix);
	uloc = glGetUniformLocation(program, "solid_color");			if (uloc > -1) glUniform4fv(uloc, 1, player.head.color);
	glDrawElements(GL_TRIANGLES, sphereMesh->index_list.size(), GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);


	render_particle(window_size.x / float(window_size.y), cam.view_matrix, cam.projection_matrix, flag_for_doge, flag_for_fire);
	if (flag_for_doge && view3_flag) {
		
		play3dsound(doge_path, &player, 1.0f);
	}
	else {
		drop3d(&player);
	}
	flag_for_doge = false;
	show_ingame_interface();
	
	glfwSwapBuffers(window);
}

void reshape(GLFWwindow* window, int width, int height)
{
	window_size = ivec2(width, height);
	glViewport(0, 0, width, height);
}

void mouse(GLFWwindow* window, int button, int action, int mods)
{
	dvec2 pos;
	vec2 npos;
	pos; glfwGetCursorPos(window, &pos.x, &pos.y);
	npos = cursor_to_ndc(pos, window_size);
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS && view3_flag)			tb.begin(npos);
		else if (action == GLFW_RELEASE)				tb.end();
	}
}

void motion(GLFWwindow* window, double x, double y)
{
	if (!tb.is_tracking()) return;
	vec2 npos = cursor_to_ndc(dvec2(x, y), window_size);
	tb.update(npos);
}


void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) 	{
		if (key == GLFW_KEY_R)	game_flag = 0;
		else if (key == GLFW_KEY_F1)						show_help_menu();
		else if (key == GLFW_KEY_V)							view3_flag = !view3_flag;

#ifndef GL_ES_VERSION_2_0
		else if (key == GLFW_KEY_K) 		{
			b_wireframe = !b_wireframe;
			glPolygonMode(GL_FRONT_AND_BACK, b_wireframe ? GL_LINE : GL_FILL);
			printf("> using %s mode\n", b_wireframe ? "wireframe" : "solid");
		}

		// wasd 이동 키.. 눌리면 속도 증가
		else if (key == GLFW_KEY_D) player.velocity += player.move_speed * vec3(1, 0, 0);
		else if (key == GLFW_KEY_A)	player.velocity += player.move_speed * vec3(-1, 0, 0);
		else if (key == GLFW_KEY_W)	player.velocity += player.move_speed * vec3(0, 1, 0);
		else if (key == GLFW_KEY_S)	player.velocity += player.move_speed * vec3(0, -1, 0);
		else if (key == GLFW_KEY_SPACE) {
			if (player.center.z < player.head_radius * 1.01f) {
				player.velocity += player.move_speed * vec3(0, 0, 2);
				play3dsound(doge_jump_path, &player, false);
			}
		}
		else if (key == GLFW_KEY_1) { play3dsound(dogbark_path, &player, false); }
		else if (key == GLFW_KEY_E) {
			if (count_ult < (3 - difficulty) * 5) {
				flag_for_fire = true; play3dsound(fire_path, &player, false);
			}
			
		}
#endif
		}
		else if (action == GLFW_RELEASE) 	{
			// wasd 이동 키.. 떼면 속도 원 위치
			if (key == GLFW_KEY_D)		player.velocity -= player.move_speed * vec3(1, 0, 0);
			else if (key == GLFW_KEY_A)	player.velocity -= player.move_speed * vec3(-1, 0, 0);
			else if (key == GLFW_KEY_W)	player.velocity -= player.move_speed * vec3(0, 1, 0);
			else if (key == GLFW_KEY_S)	player.velocity -= player.move_speed * vec3(0, -1, 0);
			else if (key == GLFW_KEY_E) { count_ult++; flag_for_fire = false; }
		}
	}


bool user_init()
{
	char* vertexBinName;
	char* indexBinName;
	glLineWidth(1.0f);
	glClearColor(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.0f);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	drop2d();
	play2dsound(mp3_path, 0.7f, true);
	
	engine->setDopplerEffectParameters(0.3f, 0.1f);
	listener_pos = reinterpret_cast<irrklang::vec3df&> (cam.eye);
	listener_up = reinterpret_cast<irrklang::vec3df&> (cam.up);
	look_direction = reinterpret_cast<irrklang::vec3df&> (cam.at);

	listener_pos.Z = -listener_pos.Z;
	look_direction.Z = -look_direction.Z;
	listener_up.Z = -listener_up.Z;

	// sphere mesh
	vertexBinName = "../bin/mesh/sphere.index.bin";
	indexBinName = "../bin/mesh/sphere.vertex.bin";
	sphereMesh = cg_load_mesh(vertexBinName, indexBinName);
	if (sphereMesh == nullptr) { printf("SPHERE MESH NOT LOADED.\n"); return false; }

	// cuboid mesh
	vertexBinName = "../bin/mesh/cuboid.index.bin";
	indexBinName = "../bin/mesh/cuboid.vertex.bin";
	cuboidMesh = cg_load_mesh(vertexBinName, indexBinName);
	if (cuboidMesh == nullptr) { printf("CUBOID MESH NOT LOADED.\n"); return false; }
	
	if (!particle_init()) return false;
	if (!init_text()) return false;


	// load textures for game
	rainbowTex = cg_create_texture("../bin/textures/rainbow.jpg", true);
	buildingTex = cg_create_texture("../bin/textures/building_face.jpg", true);
	elonTex = cg_create_texture("../bin/textures/elon.png", true);
	starMapTex = cg_create_texture("../bin/textures/star_map.png", true);
	earthTex = cg_create_texture("../bin/textures/earth.jpg", true);
	marsTex = cg_create_texture("../bin/textures/mars.jpg", true);
	furTex = cg_create_texture("../bin/textures/fur.jpg", true);
	dogeFaceTex = cg_create_texture("../bin/textures/doge_face_4.png", true);

	return true;
}

// 게임 초기화 함수, 난이도에 따라 장애물, 유저 속도 등등 조정
bool game_init()
{
	t = before_time = 0;
	//create objects
	// buildings
	float b_x1 = 40.0f;		// building center.x of right side
	float b_x2 = -40.0f;	// building center.x of left side
	float b_y = 10.0f;		// building center.y
	float b_z = 30.0f;		// building center.z
	while (b_y < 1990.0f)
	{
		buildings.push_back(cuboid(vec3(b_x1, b_y, b_z), vec3(10, 10, b_z)));
		buildings.push_back(cuboid(vec3(b_x2, b_y, b_z), vec3(10, 10, b_z)));
		b_y += 50.0f;
	}
	// player
	float player_size = 3.0f;
	player = character(vec3(0, player_size * 0.5f, 100.0f), vec3(0, 0, 0), player_size, 50.0f);

	// board
	board = cuboid(vec3(0, 1000, -1), vec3(50, 1000, 1));

	// star map
	star_map = sphere(vec3(0, 0, 0), 5000.0f);

	// mars, earth;
	earth = sphere(vec3(0, -200, 10), 80, vec3(0, 35, 0), vec3(1, 0, 0), -1.0f);
	mars = sphere(vec3(0, 2100, 0), 100, vec3(0, 0, 0), vec3(0, 0, 1), 1.0f);

	//initialize objects
	player.update(0);
	board.update(0);
	star_map.update(0);
	earth.update(0);
	mars.update(0);
	for (size_t i = 0; i < buildings.size(); i++) buildings.at(i).update(0);

	// register event callbacks
	glfwSetKeyCallback(window, keyboard);
	glfwSetMouseButtonCallback(window, mouse);
	glfwSetCursorPosCallback(window, motion);

	before_time = float(glfwGetTime());
	return true;
}

// 게임 시작 함수
int start_game()
{
	if (!game_init()) { printf("Game initialize failed...\n"); return 0; }
	// 플레이어가 죽으면 선택 화면을 띄울 수 있도록 함
	// 플레이어의 선택지를 반환
	flag_for_doge = false;
	flag_for_fire = false;
	count_ult = 0;
	drop2d();
	if(difficulty==3)
		play2dsound(avengers_path, 0.4f,true);
	else if(difficulty==2)
		play2dsound(pirates_path, 0.2f,true);
	else if(difficulty==1)
		play2dsound(first_step_path, 1.0f, true);//이 함수 쓰려면 앞에 drop2d();호출 필수

	printf("Game start with difficulty %d\n", difficulty);


	// enters rendering/event loop
	game_flag = 15947;
	for (frame = 0; game_flag == 15947; frame++)
	{
		glfwPollEvents();
		update();
		render();
	}
	game_finalize();
	return game_flag;
}


void game_finalize()
{
	t = before_time = 0;
	// remove objects
	obstacles.clear();
	buildings.clear();
	return;
}
void create_obstacles()
{
	// Test fetch and pull
	// Every 5 second.
	if ((int)(t*difficulty) % 2 == 0 && (int)(before_time* difficulty) % 2 == 1)
	{
		float maxVal;
		vec3 newCenter, newVelocity;
		float newRadius;

		// random radius from 1 to 20
		newRadius = (get_randomVal() + 1) * 5.0f + 5.0f; // 5 ~ 15 radius
		maxVal = 30 - newRadius;
		newCenter = vec3(get_randomVal() * maxVal, 3000, get_randomVal() * maxVal + 10);
		newVelocity = vec3(0, -abs(get_randomVal() * diff_mv * 2), 0) * (float)difficulty; // where difficulty should go


		sphere newObstacle = sphere(newCenter, newRadius, newVelocity, true);
		obstacles.push_back(newObstacle);
	}
}






bool easy_on = false;
bool normal_on = false;
bool hard_on = false;
bool end_intro_on = false;

void mouse_intro(GLFWwindow* window, int button, int action, int mods) {//변경
	dvec2 pos;
	vec2 npos;
	pos; glfwGetCursorPos(window, &pos.x, &pos.y);
	npos = cursor_to_ndc(pos, window_size);
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			if (npos.x > -0.5f * 0.8f && npos.x < 0.5f * 0.8f && npos.y > -0.1f * 0.8f - 0.3f && npos.y < 0.1f * 0.8f - 0.3f) {
				end_intro_on = true;
			}
			if (npos.x > -0.5f * 0.8f && npos.x < 0.5f * 0.8f && npos.y > -0.1f * 0.8f - 0.5f && npos.y < 0.1f * 0.8f - 0.5f) {
				end_intro_on = true;
			}
			if (npos.x > -0.5f * 0.8f && npos.x < 0.5f * 0.8f && npos.y > -0.1f * 0.8f - 0.7f && npos.y < 0.1f * 0.8f - 0.7f) {
				end_intro_on = true;
			}
		}
		else if (action == GLFW_RELEASE) {

		}
	}
}
bool flag_for_bark = false;
bool flag_for_bark2 = false;
bool flag_for_bark3 = false;
void motion_intro(GLFWwindow* window, double x, double y) {
	vec2 npos = cursor_to_ndc(dvec2(x, y), window_size);
	if (npos.x > -0.5f * 0.8f && npos.x < 0.5f * 0.8f && npos.y > -0.1f * 0.8f - 0.3f && npos.y < 0.1f * 0.8f - 0.3f) {
		if(!flag_for_bark){
			if (player.sound) drop3d(&player);
			play3dsound(bark_sh2_1_path,  &player,true);
			flag_for_bark = true;
		}

		
		easy_on = true;
		difficulty = 1;
	}
	else {
		flag_for_bark = false;
		easy_on = false;
	}
	if (npos.x > -0.5f * 0.8f && npos.x < 0.5f * 0.8f && npos.y > -0.1f * 0.8f - 0.5f && npos.y < 0.1f * 0.8f - 0.5f) {
		if (!flag_for_bark2) {
			if (player.sound) drop3d(&player);
			play3dsound(bark_sh2_1_path, &player, true);
			flag_for_bark2 = true;
		}
		normal_on = true;
		difficulty = 2;
	}
	else {
		flag_for_bark2 = false;
		normal_on = false;
	}
	if (npos.x > -0.5f * 0.8f && npos.x < 0.5f * 0.8f && npos.y > -0.1f * 0.8f - 0.7f && npos.y < 0.1f * 0.8f - 0.7f) {
		if (!flag_for_bark3) {
			if (player.sound) drop3d(&player);
			play3dsound(bark_sh1_1_path, &player, true);
			flag_for_bark3 = true;
		}
		hard_on = true;
		difficulty = 3;
	}
	else {
		hard_on = false;
		flag_for_bark3 = false;
	}
}

int show_ingame_interface() {
	float dpi_scale = cg_get_dpi_scale();
	frame_val = (int)((float)frame / t);
	float av = (float)sqrt(window_size.x * window_size.y) / 2500;
	render_text("press R : restart", (int)(window_size.x * (0.03f)), (int)(window_size.y * (0.03f)), av, vec4(0.5f, 0.8f, 0.2f, 1.0f), dpi_scale);
	render_text("     F1 : help", (int)(window_size.x * (0.03f)), (int)(window_size.y * (0.06f)), av, vec4(0.5f, 0.8f, 0.2f, 1.0f), dpi_scale);
	render_text(std::string("MAX FIRE count : ") + std::to_string(15 - 5*difficulty), (int)(window_size.x * (0.70f / dpi_scale)), (int)(window_size.y * (0.90f) / dpi_scale), av, vec4(0.5f, 0.8f, 0.2f, 1.0f), dpi_scale);
	if (count_ult < 15 - 5 * difficulty) {
		render_text(std::string("used FIRE count : ") + std::to_string(count_ult), (int)(window_size.x * (0.70f / dpi_scale)), (int)(window_size.y * (0.94f) / dpi_scale), av, vec4(0.5f, 0.8f, 0.2f, 1.0f), dpi_scale);
	}
	else {
		render_text("YOU HAVE USED ALL FIRE", (int)(window_size.x * (0.66f / dpi_scale)), (int)(window_size.y * (0.94f) / dpi_scale), av, vec4(0.5f, 0.8f, 0.2f, 1.0f), dpi_scale);
	}
	//render_text("press Q to quit", (int)(window_size.x * (0.03f)), (int)(window_size.y * (0.94f)/ dpi_scale), av, vec4(0.5f, 0.8f, 0.2f, 1.0f), dpi_scale);
	char s1[300];
	if (difficulty == 1) {
		sprintf(s1, "difficulty :  Easy");
	}
	else if (difficulty == 2) {
		sprintf(s1, "difficulty :  Normal");
	}
	else {
		sprintf(s1, "difficulty :  Hard");
	}

	render_text(s1, (int)(window_size.x * 0.03f), (int)(window_size.y * (0.90f / dpi_scale)), av, vec4(0.5f, 0.8f, 0.2f, 1.0f), dpi_scale);

	return 1;
}


void show_intro() {

	easy_on = false;
	normal_on = false;
	hard_on = false;
	drop2d();
	play2dsound(mp3_path, 0.7f, true);
	glfwSetCursorPosCallback(window, motion_intro);
	glfwSetMouseButtonCallback(window, mouse_intro);
	glfwSetKeyCallback(window, keyboard_intro); //다시 바인딩
	int diff = 1;
	float scale = 1.0f;
	vertex corners[4];
	corners[0].pos = vec3(-1 * scale, -1 * scale, 0.5f);	corners[0].tex = vec2(0.0f, 0.0f);
	corners[1].pos = vec3(scale, -1 * scale, 0.5f);	corners[1].tex = vec2(1.0f, 0.0f);
	corners[2].pos = vec3(scale, scale, 0.5f);	corners[2].tex = vec2(1.0f, 1.0f);
	corners[3].pos = vec3(-1 * scale, scale, 0.5f);	corners[3].tex = vec2(0.0f, 1.0f);

	vertex vertices[6] = { corners[0], corners[1], corners[2], corners[0], corners[2], corners[3] };

	//
	// generation of vertex buffer is the same, but use vertices instead of corners
	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array) glDeleteVertexArrays(1, &vertex_array);
	vertex_array = cg_create_vertex_array(vertex_buffer);
	if (!vertex_array) { printf("%s(): failed to create vertex array\n", __func__); }

	//고침 
	GLuint intro_tc = cg_create_texture("../bin/textures/intro_image1.jpg", true);
	GLuint intro_diff = cg_create_texture("../bin/textures/diff.png", true);
	GLuint intro_easy = cg_create_texture("../bin/textures/easy.jpg", true);
	GLuint intro_easy_after = cg_create_texture("../bin/textures/easy_after.jpg", true);
	GLuint intro_normal = cg_create_texture("../bin/textures/normal.jpg", true);
	GLuint intro_normal_after = cg_create_texture("../bin/textures/normal_after.jpg", true);
	GLuint intro_normal_after2 = cg_create_texture("../bin/textures/normal2.png", true);
	GLuint intro_hard = cg_create_texture("../bin/textures/hard.jpg", true);
	GLuint intro_hard_after = cg_create_texture("../bin/textures/hard_after.jpg", true);
	GLuint help = cg_create_texture("../bin/textures/hard_after.jpg", true);
	GLuint quit = cg_create_texture("../bin/textures/hard_after.jpg", true);
	GLuint credit = cg_create_texture("../bin/textures/hard_after.jpg", true);
	GLuint title = cg_create_texture("../bin/textures/dr.png", true);
	GLint uloc;
	mat4 model_matrix;


	for (frame = 0; !end_intro_on; frame++) {
		glfwPollEvents();
		// update
		// render
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(program);
		uloc = glGetUniformLocation(program, "show_flag");				if (uloc > -1) glUniform1i(uloc, 2);
		uloc = glGetUniformLocation(program, "color_flag");				if (uloc > -1) glUniform1i(uloc, 1);
		glActiveTexture(GL_TEXTURE0);

		glBindVertexArray(vertex_array);
		glUniform1i(glGetUniformLocation(program, "TEX"), 0);


		glBindTexture(GL_TEXTURE_2D, title);
		model_matrix = update(0.7f, 0.49f, 0.0f, 0.6f);
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		glDrawArrays(vertex_array, 0, 6);

		glBindTexture(GL_TEXTURE_2D, intro_tc);
		model_matrix = update(0.25f, 0.2f, +0.75f, -0.7f);
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		glDrawArrays(vertex_array, 0, 6);

		glBindTexture(GL_TEXTURE_2D, intro_diff);
		model_matrix = update(0.7f, 0.1f, 0.0f, -0.0f);
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		glDrawArrays(vertex_array, 0, 6);

		if (!easy_on) {
			glBindTexture(GL_TEXTURE_2D, intro_easy);
			model_matrix = update(0.5f * 0.8f, 0.1f * 0.8f, 0.0f, -0.3f);
			uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
			glDrawArrays(vertex_array, 0, 6);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, intro_easy_after);
			model_matrix = update(0.5f * 0.8f, 0.1f * 0.8f, 0.0f, -0.3f);
			uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
			glDrawArrays(vertex_array, 0, 6);
			difficulty = 1;
		}
		if (!normal_on) {
			glBindTexture(GL_TEXTURE_2D, intro_normal);
			model_matrix = update(0.5f * 0.8f, 0.1f * 0.8f, 0.0f, -0.5f);
			uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
			glDrawArrays(vertex_array, 0, 6);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, intro_normal_after2);
			model_matrix = update(0.5f * 0.8f, 0.1f * 0.8f, 0.0f, -0.5f);
			uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
			glDrawArrays(vertex_array, 0, 6);
			difficulty = 2;
		}
		if (!hard_on) {
			glBindTexture(GL_TEXTURE_2D, intro_hard);
			model_matrix = update(0.5f * 0.8f, 0.1f * 0.8f, 0.0f, -0.7f);
			uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
			glDrawArrays(vertex_array, 0, 6);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, intro_hard_after);
			model_matrix = update(0.5f * 0.8f, 0.1f * 0.8f, 0.0f, -0.7f);
			uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
			glDrawArrays(vertex_array, 0, 6);
			difficulty = 3;
		}
		float dpi_scale = cg_get_dpi_scale();
		float av = (float)sqrt(window_size.x * window_size.y) / 3000;
		render_text("Press F1 to show Ending Credit", (int)(window_size.x * (0.01f)), (int)(window_size.y * (0.94f) / dpi_scale), av, vec4(0.0f, 0.0f, 0.0f, 1.0f), dpi_scale);
		render_text("Press q to quit game", (int)(window_size.x * (0.01f)), (int)(window_size.y * (0.91f) / dpi_scale), av, vec4(0.0f, 0.0f, 0.0f, 1.0f), dpi_scale);
		glfwSwapBuffers(window);
	}
	easy_on = false;
	normal_on = false;
	hard_on = false;
	end_intro_on = false; // 다시 초기화
	glfwSetKeyCallback(window, keyboard); //다시 바인딩
	return;
}

void show_video(float _before_time) {
	float scale = 1.0f;
	vertex corners[4];
	corners[0].pos = vec3(-1 * scale, -1 * scale, 0.5f);	corners[0].tex = vec2(0.0f, 0.0f);
	corners[1].pos = vec3(scale, -1 * scale, 0.5f);	corners[1].tex = vec2(1.0f, 0.0f);
	corners[2].pos = vec3(scale, scale, 0.5f);	corners[2].tex = vec2(1.0f, 1.0f);
	corners[3].pos = vec3(-1 * scale, scale, 0.5f);	corners[3].tex = vec2(0.0f, 1.0f);

	vertex vertices[6] = { corners[0], corners[1], corners[2], corners[0], corners[2], corners[3] };

	//
	// generation of vertex buffer is the same, but use vertices instead of corners
	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array) glDeleteVertexArrays(1, &vertex_array);
	vertex_array = cg_create_vertex_array(vertex_buffer);
	if (!vertex_array) { printf("%s(): failed to create vertex array\n", __func__); }

	//고침 
	GLuint videos[1000];
	char buffer[1000];
	int index = 0;
	for (int i = 0; i < 100; i += 2) {
		videos[index] = cg_create_texture("../bin/textures/dr.png", true);
		index++;
	}
	for (int i = 357; i < 376; i +=2) 		{
		sprintf(buffer, "../bin/intro/scene00%03d.png", i);
		videos[index]  = cg_create_texture( buffer, true);
		index++;
	}
	for (int i = 425; i < 558; i += 2) {
		sprintf(buffer, "../bin/intro/scene00%03d.png", i);
		videos[index] = cg_create_texture(buffer, true);
		index++;
	}
	float now_t;
	GLint uloc;
	mat4 model_matrix;
	int realframeindex = 0;
	float _now_delta = 0;
	while(realframeindex <= index){
		now_t = float(glfwGetTime());
		_now_delta += (now_t - _before_time);
		//printf("%f %d\n", _now_delta, realframeindex);
		if (_now_delta > 25000) {
			_now_delta = 0;
			glfwPollEvents();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(program);

			uloc = glGetUniformLocation(program, "show_flag");				if (uloc > -1) glUniform1i(uloc, 1);
			uloc = glGetUniformLocation(program, "color_flag");				if (uloc > -1) glUniform1i(uloc, 1);
			glActiveTexture(GL_TEXTURE0);

			glBindVertexArray(vertex_array);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);
			glBindTexture(GL_TEXTURE_2D, videos[realframeindex]);
			realframeindex++;
			model_matrix = update(0.7f, 0.1f, 0.0f, -0.0f);
			uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
			glDrawArrays(vertex_array, 0, 6);
			glfwSwapBuffers(window);
			_before_time = now_t;
		}
		
	}
	return;
}

int show_difficulty_select()
{
	// 난이도 선택 창 보이기
	// show_intro와 마찬가지로, 새로운 콜백 함수 설정, 난이도 버튼을 선택하면 맞는 난이도를 반환
	return 1;
}

int show_game_over_select()
{
	// 플레이어 사망 시 선택 화면.
	// 새로운 콜백 함수 사용, 선택지 선택 가능토록 설정
	// 선택 화면 : 게임종료(0), 재도전(1), 난이도선택(2)
	return 0;
}
bool end_credit = false;
void show_ending_credit()
{
	glfwSetKeyCallback(window, keyboard_credit); //다시 바인딩
	pause_game();
	drop2d();
	play2dsound(doge_ending_path, 0.6f, true);
	float scale1 = 1.0f;
	vertex corners[4];
	corners[0].pos = vec3(-1 * scale1, -1 * scale1, 0.5f);	corners[0].tex = vec2(0.0f, 0.0f);
	corners[1].pos = vec3(scale1, -1 * scale1, 0.5f);	corners[1].tex = vec2(1.0f, 0.0f);
	corners[2].pos = vec3(scale1, scale1, 0.5f);	corners[2].tex = vec2(1.0f, 1.0f);
	corners[3].pos = vec3(-1 * scale1, scale1, 0.5f);	corners[3].tex = vec2(0.0f, 1.0f);

	vertex vertices[6] = { corners[0], corners[1], corners[2], corners[0], corners[2], corners[3] };

	//
	// generation of vertex buffer is the same, but use vertices instead of corners
	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	float dpi_scale = cg_get_dpi_scale();
	frame_val = (int)((float)frame / t);
	float scale = (float)sqrt(window_size.x * window_size.y) / 3000;
	float x_coor = 0.12f;
	float y_coor = 0.3f;
	GLint uloc;
	mat4 model_matrix;
	for (frame = 0; !end_credit; frame++) {
		glfwPollEvents();
		// update
		// render
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(program);

		uloc = glGetUniformLocation(program, "show_flag");				if (uloc > -1) glUniform1i(uloc, 2);
		uloc = glGetUniformLocation(program, "color_flag");				if (uloc > -1) glUniform1i(uloc, 1);
		glActiveTexture(GL_TEXTURE0);

		glBindVertexArray(vertex_array);
		glUniform1i(glGetUniformLocation(program, "TEX"), 0);

		GLuint intro_tc = cg_create_texture("../bin/textures/intro_image1.jpg", true);
		glBindTexture(GL_TEXTURE_2D, intro_tc);
		model_matrix = update(0.35f, 0.25f, +0.65f, -0.6f);
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		glDrawArrays(vertex_array, 0, 6);

		float dpi_scale = cg_get_dpi_scale();
		float av = (float)sqrt(window_size.x * window_size.y) / 2400;
		render_text("Project Leader: Kim Doyeol", (int)(window_size.x * x_coor/dpi_scale), (int)(window_size.y * (0.03f + y_coor) / dpi_scale), av, vec4(0.0f, 0.0f, 0.0f, 1.0f), dpi_scale);
		render_text("members: Kim Sanghun, Lee Junseok", (int)(window_size.x * x_coor/dpi_scale), (int)(window_size.y * (0.07f + y_coor) / dpi_scale), av, vec4(0.0f, 0.0f, 0.0f, 1.0f), dpi_scale);
		render_text("Class :  Computer Graphics 2021-1", (int)(window_size.x * x_coor / dpi_scale), (int)(window_size.y * (0.11f + y_coor) / dpi_scale), av, vec4(0.0f, 0.0f, 0.0f, 1.0f), dpi_scale);
		render_text("Professor : Lee Sungkil", (int)(window_size.x * x_coor / dpi_scale), (int)(window_size.y * (0.15f + y_coor) / dpi_scale), av, vec4(0.0f, 0.0f, 0.0f, 1.0f), dpi_scale);
		render_text("Special thanks to Kabosu, who is known for Doge", (int)(window_size.x * x_coor / dpi_scale), (int)(window_size.y * (0.19f + y_coor) / dpi_scale), av, vec4(0.0f, 0.0f, 0.0f, 1.0f), dpi_scale);
		render_text("Also thanks to The Emperor of Mars, Elon Musk", (int)(window_size.x * x_coor / dpi_scale), (int)(window_size.y * (0.23f + y_coor) / dpi_scale), av, vec4(0.0f, 0.0f, 0.0f, 1.0f), dpi_scale);
		render_text("Thanks for playing our game", (int)(window_size.x * x_coor / dpi_scale), (int)(window_size.y * (0.27f + y_coor) / dpi_scale), av, vec4(0.0f, 0.0f, 0.0f, 1.0f), dpi_scale);
		render_text("Intro video from channel ZimoNitrome", (int)(window_size.x * x_coor / dpi_scale), (int)(window_size.y * (0.31f + y_coor) / dpi_scale), av, vec4(0.0f, 0.0f, 0.0f, 1.0f), dpi_scale);
		render_text("url : https://youtu.be/000al7ru3ms", (int)(window_size.x * x_coor / dpi_scale), (int)(window_size.y * (0.35f + y_coor) / dpi_scale), av, vec4(0.0f, 0.0f, 0.0f, 1.0f), dpi_scale);
		render_text("Back to Start Page : Press F1", (int)(window_size.x * (0.01f) / dpi_scale), (int)(window_size.y * (0.93f) / dpi_scale), av, vec4(0.0f, 0.0f, 0.0f, 1.0f), dpi_scale);
		//-- Graphics in A+ --

		glfwSwapBuffers(window);
	}
	end_credit = false;
	drop2d();
	restart_game();
	glfwSetKeyCallback(window, keyboard_intro); //다시 바인딩
	
	return;
}

// 도움말 창
bool end_help_on = false;

void keyboard_help(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_F1)						end_help_on = true;
	}
}

void keyboard_credit(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_F1)						end_credit = true;
	}
}

void show_help_menu() {
	// pause game
	
	pause_game();
	glfwSetKeyCallback(window, keyboard_help); //다시 바인딩
	
	// show help menu
	vertex corners[4];
	float scale = 1.0f;
	corners[0].pos = vec3(-1 * scale, -1 * scale, 0.5f);	corners[0].tex = vec2(0.0f, 0.0f);
	corners[1].pos = vec3(scale, -1 * scale, 0.5f);	corners[1].tex = vec2(1.0f, 0.0f);
	corners[2].pos = vec3(scale, scale, 0.5f);	corners[2].tex = vec2(1.0f, 1.0f);
	corners[3].pos = vec3(-1 * scale, scale, 0.5f);	corners[3].tex = vec2(0.0f, 1.0f);
	vertex vertices[6] = { corners[0], corners[1], corners[2], corners[0], corners[2], corners[3] };
	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	if (vertex_array) glDeleteVertexArrays(1, &vertex_array);
	vertex_array = cg_create_vertex_array(vertex_buffer);
	if (!vertex_array) { printf("%s(): failed to create vertex array\n", __func__); }
	GLuint help = cg_create_texture("../bin/textures/help_tmp.png", true);
	GLint uloc;
	mat4 model_matrix;
	for (frame = 0; !end_help_on; frame++) {
		glfwPollEvents();
		// update
		// render
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(program);

		uloc = glGetUniformLocation(program, "show_flag");				if (uloc > -1) glUniform1i(uloc, 1);
		uloc = glGetUniformLocation(program, "color_flag");				if (uloc > -1) glUniform1i(uloc, 1);
		glActiveTexture(GL_TEXTURE0);

		glBindVertexArray(vertex_array);
		glUniform1i(glGetUniformLocation(program, "TEX"), 0);

		glBindTexture(GL_TEXTURE_2D, help);
		model_matrix = update(0.5f, 0.5f, 0.0f, 0.0f);
		uloc = glGetUniformLocation(program, "model_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
		glDrawArrays(vertex_array, 0, 6);
		glfwSwapBuffers(window);


	}
	end_help_on = false; // 다시 초기화
	glfwSetKeyCallback(window, keyboard); //다시 바인딩
	restart_game();

	return;
}

void keyboard_intro(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_F1) {
			show_ending_credit();
			drop2d();
			play2dsound(mp3_path, 0.7f, true);
		}
		else if (key == GLFW_KEY_Q) {
			end_intro_on = true;
			difficulty = 123;
		}			
	}
}


// 게임 일시정지
void pause_game() {
	pause = true;
	return;
}
void restart_game() {
	pause = false;
	return;
}



// 프로그램 종료
void user_finalize()
{
}