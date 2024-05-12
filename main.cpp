#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ft2build.h"
#include FT_FREETYPE_H

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/common.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Window.hpp"
#include "GameObj3D.hpp"
#include "ShaderProgram.hpp"
#include "Camera.hpp"
#include "Parametric3DShape.hpp"
#include "CubeData.hpp"
#include "collusion-helpers.hpp"
#include "Scene.hpp"
#include "Skybox.hpp"
#include "OBJ_Loader.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
using namespace std;

// Globals
int u_transform, u_pv, u_frame, u_light_pos, u_light_color;
int moveFront = 0, moveRight = 0;
int score = 0;
float mouseX = 0, mouseY = 0;
bool fired = false;
bool reset = false;
bool written = false;
bool scoreboard = false;
int life = 0;
float gameSpeed = 0.0005;
glm::mat4 projection = glm::ortho(0.0f, 1200.0f, 0.0f, 1200.0f);
unsigned int VAO, VBO;

struct Character {
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
};

map<char, Character> Characters;

void RenderText(ShaderProgram& s, string text, float x, float y, float scale, glm::vec3 color)
{
	// activate corresponding render state
	s.use();
	glUniform3f(glGetUniformLocation(s.id, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void keyCallback(GLFWwindow* _, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		moveFront = 1;
	}
	else if (key == GLFW_KEY_W && action == GLFW_RELEASE)
	{
		moveFront = 0;
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		moveFront = -1;
	}
	else if (key == GLFW_KEY_S && action == GLFW_RELEASE)
	{
		moveFront = 0;
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		moveRight = 1;
	}
	else if (key == GLFW_KEY_D && action == GLFW_RELEASE)
	{
		moveRight = 0;
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		moveRight = -1;
	}
	else if (key == GLFW_KEY_A && action == GLFW_RELEASE)
	{
		moveRight = 0;
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		fired = true;
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		life = 3;
		reset = true;
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (scoreboard)
			scoreboard = false;
		else
			scoreboard = true;
	}
}

static void cursorPositionCallback(GLFWwindow* _, double x, double y)
{
	mouseX = 2.0 * ((float)x / Window::width) - 1;
	mouseY = 2.0 * (1 - ((float)y / Window::height)) - 1;
}

int main()
{
	// init window
	Window::init(1200, 1200, "SpaceCraft");

	glfwSetKeyCallback(Window::window, keyCallback);
	glfwSetCursorPosCallback(Window::window, cursorPositionCallback);

	//scoreboard file reading
	ifstream inputScore;
	ofstream outputScore;
	inputScore.open("scoreboard.txt");
	vector<int> scoreArray(5, 0);

	string line;
	while (getline(inputScore, line))
	{
		cout << stoi(line) << endl;
		scoreArray.push_back(stoi(line));
	}
	sort(scoreArray.begin(), scoreArray.end());
	inputScore.close();
	outputScore.open("scoreboard.txt", std::ios_base::app); // append instead of overwrite

	//init text render
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return -1;
	}

	FT_Face face;
	if (FT_New_Face(ft, "fonts/space.ttf", 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return -1;
	}
	else
	{
		FT_Set_Pixel_Sizes(face, 0, 48);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
		for (unsigned char c = 0; c < 128; c++)
		{
			// load character glyph
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			// generate texture
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			// set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// now store character for later use
			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			Characters.insert(pair<char, Character>(c, character));
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// load obj
	objl::Loader loader;
	loader.LoadFile("spacecraft.obj");
	vector<glm::vec3> positions;
	vector<glm::vec3> normals;
	vector<glm::vec2> uvs;
	for (int i = 0; i < loader.LoadedMeshes[0].Vertices.size(); i++)
	{
		glm::vec3 temp;
		glm::vec2 temp2;
		temp.x = loader.LoadedMeshes[0].Vertices[i].Position.X;
		temp.y = loader.LoadedMeshes[0].Vertices[i].Position.Y;
		temp.z = loader.LoadedMeshes[0].Vertices[i].Position.Z;
		positions.push_back(temp);
		temp.x = loader.LoadedMeshes[0].Vertices[i].Normal.X;
		temp.y = loader.LoadedMeshes[0].Vertices[i].Normal.Y;
		temp.z = loader.LoadedMeshes[0].Vertices[i].Normal.Z;
		normals.push_back(temp);
		temp2.x = loader.LoadedMeshes[0].Vertices[i].TextureCoordinate.X;
		temp2.y = loader.LoadedMeshes[0].Vertices[i].TextureCoordinate.Y;
		uvs.push_back(temp2);
	}


	// init objects
	Model3D sphereModel = Parametric3DShape::generate(ParametricLine::halfCircle, 50, 50);
	Model3D cubeModel1(CubeData::positions, CubeData::normals, CubeData::uvs_floor(75, 75), CubeData::indices);
	Model3D cubeModel2(CubeData::positions, CubeData::normals, CubeData::uvs(1, 1), CubeData::indices);
	Model3D spacecraftModel(positions, normals, uvs, loader.LoadedIndices);

	GameObj3D spaceCraft(spacecraftModel);
	spaceCraft.translate(0, 0.5, 3.5);
	spaceCraft.scale(0.002, 0.002, 0.002);
	spaceCraft.textureId = 2;
	spaceCraft.type = "model";

	scene.push_back(&spaceCraft);

	GameObj3D spaceCraftHitBox(sphereModel);
	spaceCraftHitBox.translate(0, 0.5, 3.5);
	spaceCraftHitBox.scale(0.3, 0.3, 0.3);
	spaceCraftHitBox.type = "spacecraft";

	GameObj3D garbage(cubeModel1);
	garbage.translate(0, 0.5, 8.5);
	garbage.scale(0.5, 0.5, 0.5);

	GameObj3D garbage2(cubeModel1);
	garbage2.translate(0, 0.5, -28.0);
	garbage2.scale(5.0, 5.0, 5.0);


	// generation locations
	float genLocation[7] = { -3.0f, -2.0f, -1.0f, 0.0f , 1.0f, 2.0f, 3.0f };

	// light
	glm::vec3 lightPos = glm::vec3(1.0, 1.0, 1.0);
	glm::vec3 lightColor = glm::vec3(1.0, 1.0, 1.0);

	const vector<string> texture_files{ "./textures/gold.jpg", "./textures/asteroid.jpg","./spacecraft.jpg",
									   "./textures/yellowpyramid.jpg", "./textures/fire.jpg", "./textures/redtexture.jpg" };

	// load textures
	vector<unsigned int> textures = Textures::loadTextures(texture_files);

	// load skybox
	unsigned int skyboxVAO, skyboxVBO;
	initSkybox(skyboxVAO, skyboxVBO);
	vector<std::string> faces{
		"./textures/right.jpg",
		"./textures/left.jpg",
		"./textures/top.jpg",
		"./textures/bottom.jpg",
		"./textures/front.jpg",
		"./textures/back.jpg" };
	unsigned cubemapTexture = loadCubemap(faces);
	ShaderProgram skyboxShader("./shader/skybox.vert", "./shader/skybox.frag");
	skyboxShader.use();
	auto skybox_texture = glGetUniformLocation(skyboxShader.id, "skybox");
	auto u_pv_sky = glGetUniformLocation(skyboxShader.id, "u_pv");
	glUniform1i(skybox_texture, 0); // 0th unit

	// create shader
	ShaderProgram sp("./shader/vertex.vert", "./shader/frag.frag");
	sp.use();
	u_transform = glGetUniformLocation(sp.id, "u_transform");
	u_pv = glGetUniformLocation(sp.id, "u_pv");
	u_frame = glGetUniformLocation(sp.id, "u_frame");
	u_light_pos = glGetUniformLocation(sp.id, "u_light_pos");
	u_light_color = glGetUniformLocation(sp.id, "u_light_color");
	auto u_texture = glGetUniformLocation(sp.id, "u_texture");
	glUniform1i(u_texture, 0);
	glActiveTexture(GL_TEXTURE0); // active 0th unit

	// shader to text-rendering of scores
	ShaderProgram sp2("./shader/score.vert", "./shader/score.frag");
	sp2.use();
	glUniformMatrix4fv(glGetUniformLocation(sp2.id, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	// timer for score
	auto start = std::chrono::system_clock::time_point();
	auto start2 = std::chrono::system_clock::now();

	// variables for game loop
	int goldCollected = 0;
	int crash = 0;
	int ammo = 4;

	// game loop
	while (!Window::isClosed())
	{
		// camera positions
		Camera::position = spaceCraft.position() - spaceCraft.front() * 6.0f + spaceCraft.up() * 2.0f;
		Camera::front = spaceCraft.front() + glm::vec3(0, mouseY, 0);
		Camera::up = glm::cross(Camera::front, spaceCraft.right());
		Camera::front = glm::rotateY(Camera::front, -mouseX);

		if (life > 0)
		{

			// update spacecraft
			spaceCraft.moveRight(moveRight * -0.1, true);
			spaceCraft.rotate(0, moveRight * -10, 0, true);

			// texture change based on life
			if (life == 1)
			{
				spaceCraft.textureId = 5;
			}
			else if (life == 3)
			{
				spaceCraft.textureId = 2;
			}


			// update hitbox
			spaceCraftHitBox.translate(spaceCraft.position().x, spaceCraft.position().y, spaceCraft.position().z);

			// gamespeed control
			gameSpeed = 0.01 + score * 0.000001;

			// fire control
			if (fired && ammo > 0)
			{
				GameObj3D* fire = new GameObj3D(sphereModel);
				fire->translate(spaceCraft.position().x, spaceCraft.position().y, spaceCraft.position().z);
				fire->scale(0.2, 0.2, 0.2);
				fire->textureId = 4;
				fire->type = "fire";

				scene.push_back(fire);

				fired = false;
				ammo--;
			}
			ammo = ammo % 30;

			// reload
			auto end2 = std::chrono::system_clock::now();
			chrono::duration<double> elapsed_seconds2 = end2 - start2;
			if (elapsed_seconds2.count() >= 0.75)
			{
				GameObj3D* block = new GameObj3D(sphereModel);
				block->translate(genLocation[rand() % 7], 0.7f, -15.0f);
				block->scale(0.35, 0.35, 0.35);
				block->textureId = 1;
				block->type = "block";
				scene.push_back(block);

				GameObj3D* point = new GameObj3D(sphereModel, false);
				point->translate(genLocation[rand() % 7], 0.7f, -15.0f);
				point->scale(0.2, 0.2, 0.2);
				point->textureId = 0;
				point->type = "point";
				scene.push_back(point);
				ammo++;
				start2 = end2;
			}

			// life check
			if (life == 0)
			{
				scoreboard = true;
			}

			// move the objects
			for (int i = 0; i < scene.size(); i++)
			{
				if (scene[i]->type == "fire")
				{
					scene[i]->moveTowards(garbage2.position().x, garbage2.position().y, garbage2.position().z, gameSpeed);
				}
				else if (scene[i]->type == "block")
				{
					scene[i]->moveTowards(garbage.position().x, garbage.position().y, garbage.position().z, gameSpeed);
				}
				else if (scene[i]->type == "point")
				{
					scene[i]->moveTowards(garbage.position().x, garbage.position().y, garbage.position().z, gameSpeed);
				}
			}

			// collision detection
			for (int i = 0; i < scene.size(); i++)
			{
				if (scene[i]->type == "fire")
				{
					if (intersect(garbage2, *scene[i]))
					{
						DeleteFromScene(scene[i]->id);
					}
					else if(CollidesWithBlock(*scene[i]))
					{
						DeleteFromScene(scene[i]->id);
						goldCollected++;
					}
				}
			}
			for (int i = 0; i < scene.size(); i++)
			{
				if (scene[i]->type == "block")
				{
					if (intersect(spaceCraftHitBox, *scene[i]))
					{
						DeleteFromScene(scene[i]->id);
						crash++;
						life--;
					}
					else if (CollidesWithFire(*scene[i]))
					{
						DeleteFromScene(scene[i]->id);
					}
					else if (intersect(garbage, *scene[i]))
					{
						DeleteFromScene(scene[i]->id);
					}
				}
			}
			for (int i = 0; i < scene.size(); i++)
			{
				if (scene[i]->type == "point")
				{
					if (intersect(spaceCraftHitBox, *scene[i]))
					{
						DeleteFromScene(scene[i]->id);
						goldCollected++;
					}
					else if (intersect(garbage, *scene[i]))
					{
						DeleteFromScene(scene[i]->id);
					}
				}
			}


			// update uniforms
			sp.use();
			glUniformMatrix4fv(u_pv, 1, GL_FALSE, glm::value_ptr(Camera::getProjMatrix() * Camera::getViewMatrix()));
			glUniform1i(u_frame, 1);
			glUniform3fv(u_light_pos, 1, glm::value_ptr(lightPos));
			glUniform3fv(u_light_color, 1, glm::value_ptr(lightColor));

			// scene draw
			for (std::vector<GameObj3D*>::iterator t = scene.begin(); t != scene.end(); ++t)
			{
				// get the object
				const int i = t - scene.begin();
				GameObj3D* object = scene[i];

				// draw the object
				glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(object->getTransform()));
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, textures[object->textureId]);
				object->draw();
			}

			// score timers and calculations
			auto end = std::chrono::system_clock::now();
			chrono::duration<double> elapsed_seconds = end - start;
			score = int(elapsed_seconds.count() * gameSpeed * 1000) + goldCollected * 100 + crash * -100;
			RenderText(sp2, "score: " + to_string(score), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
			RenderText(sp2, "ammo: " + to_string(ammo), 25.0f, 100.0f, 1.0f, glm::vec3(0.0, 1.9f, 2.5f));
			RenderText(sp2, "life: " + to_string(life), 25.0f, 175.0f, 1.0f, glm::vec3(2.5, 0.0f, 0.0f));
			RenderText(sp2, "level: " + to_string(score / 1000), 25.0f, 250.0f, 1.0f, glm::vec3(1.2, 0.0f, 1.2f));
		}
		else
		{
			if (scoreboard)
			{
				// scoreboard
				RenderText(sp2, "scoreboard", 500.0f, 500.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
				RenderText(sp2, "your score: " + to_string(score), 500.0f, 650.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
				if (!written)
				{
					outputScore << to_string(score) << '\n';
					scoreArray.push_back(score);
				}
				written = true;
				sort(scoreArray.begin(), scoreArray.end());
				int k = 0;
				for (unsigned i = scoreArray.size() - 1; i > scoreArray.size() - 6; i--)
				{
					RenderText(sp2, to_string(k + 1) + ". " + to_string(scoreArray[i]), 500.0f, 425.0f - (75.0 * k), 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
					k++;
				}
			}
			else
			{
				// start screen
				RenderText(sp2, "welcome to spacecraft", 400.0f, 650.0f, 1.0f, glm::vec3(0.0, 2.5f, 2.5f));
				RenderText(sp2, "press r to start ", 400.0f, 500.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
			}
		}


		// reset the timer and game variables
		if (reset)
		{
			start = std::chrono::system_clock::now();
			reset = false;
			written = false;
			scoreboard = false;
			goldCollected = 0;
			crash = 0;
			ammo = 0;

			scene.clear();
			scene.push_back(&spaceCraft);
		}

		// draw skybox
		glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		glUniformMatrix4fv(u_pv_sky, 1, GL_FALSE, glm::value_ptr(Camera::getProjMatrix() * glm::mat4(glm::mat3(Camera::getViewMatrix()))));

		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default

		// update the scene
		Window::refresh();
	}

	// filestreams closure
	outputScore.close();

	Window::terminate();
	return 0;
}
