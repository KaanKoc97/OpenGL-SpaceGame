#pragma once

#include <vector>
#include "glm/glm.hpp"
using namespace std;

glm::vec3 computeFaceNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
	// Uses p2 as a new origin for p1,p3
	auto a = p3 - p2;
	auto b = p1 - p2;
	// Compute the cross product a X b to get the face normal
	return glm::normalize(glm::cross(a, b));
}

namespace PyramidData
{
	vector<glm::vec3> positions = {
		// base
		glm::vec3(0.5, 0, 0),
		glm::vec3(-0.5, 0, 0),
		glm::vec3(0.5, 0, -1),
		glm::vec3(-0.5, 0, -1),
		// left
		glm::vec3(-0.5, 0, 0),
		glm::vec3(-0.5, 0, -1),
		glm::vec3(0, 1, -0.5),
		// right
		glm::vec3(0.5, 0, 0),
		glm::vec3(0.5, 0, -1.0),
		glm::vec3(0, 1, -0.5),
		// front
		glm::vec3(0.5, 0, 0),
		glm::vec3(-0.5, 0, 0),
		glm::vec3(0, 1, -0.5),
		// rear
		glm::vec3(0.5, 0, -1),
		glm::vec3(-0.5, 0, -1),
		glm::vec3(0, 1, -0.5),
	};
	vector<glm::vec3> normals = {
		// base
		computeFaceNormal(glm::vec3(0.5, 0, 0), glm::vec3(-0.5, 0, 0), glm::vec3(0.5, 0, -1)),
		computeFaceNormal(glm::vec3(0.5, 0, 0), glm::vec3(-0.5, 0, 0), glm::vec3(0.5, 0, -1)),
		computeFaceNormal(glm::vec3(0.5, 0, 0), glm::vec3(-0.5, 0, 0), glm::vec3(0.5, 0, -1)),
		computeFaceNormal(glm::vec3(0.5, 0, 0), glm::vec3(-0.5, 0, 0), glm::vec3(0.5, 0, -1)),
		// left
		computeFaceNormal(glm::vec3(-0.5, 0, 0), glm::vec3(-0.5, 0, -1.0), glm::vec3(0, 1, -0.5)),
		computeFaceNormal(glm::vec3(-0.5, 0, 0), glm::vec3(-0.5, 0, -1.0), glm::vec3(0, 1, -0.5)),
		computeFaceNormal(glm::vec3(-0.5, 0, 0), glm::vec3(-0.5, 0, -1.0), glm::vec3(0, 1, -0.5)),
		// right
		computeFaceNormal(glm::vec3(0.5, 0, 0), glm::vec3(0.5, 0, -1.0), glm::vec3(0, 1, -0.5)),
		computeFaceNormal(glm::vec3(0.5, 0, 0), glm::vec3(0.5, 0, -1.0), glm::vec3(0, 1, -0.5)),
		computeFaceNormal(glm::vec3(0.5, 0, 0), glm::vec3(0.5, 0, -1.0), glm::vec3(0, 1, -0.5)),
		// front
		computeFaceNormal(glm::vec3(0.5, 0, 0), glm::vec3(-0.5, 0, 0), glm::vec3(0, 1, -0.5)),
		computeFaceNormal(glm::vec3(0.5, 0, 0), glm::vec3(-0.5, 0, 0), glm::vec3(0, 1, -0.5)),
		computeFaceNormal(glm::vec3(0.5, 0, 0), glm::vec3(-0.5, 0, 0), glm::vec3(0, 1, -0.5)),
		// rear
		computeFaceNormal(glm::vec3(0.5, 0, -1), glm::vec3(-0.5, 0, -1), glm::vec3(0, 1, -0.5)),
		computeFaceNormal(glm::vec3(0.5, 0, -1), glm::vec3(-0.5, 0, -1), glm::vec3(0, 1, -0.5)),
		computeFaceNormal(glm::vec3(0.5, 0, -1), glm::vec3(-0.5, 0, -1), glm::vec3(0, 1, -0.5)),
	};

	vector<glm::vec2> uvs = {
		// base
		glm::vec2(1, 1),
		glm::vec2(1, 0),
		glm::vec2(0, 0),
		glm::vec2(0, 1),
		// left
		glm::vec2(1, 1),
		glm::vec2(1, 0),
		glm::vec2(0, 0),
		// right
		glm::vec2(1, 1),
		glm::vec2(1, 0),
		glm::vec2(0, 0),
		// front
		glm::vec2(1, 1),
		glm::vec2(1, 0),
		glm::vec2(0, 0),
		// rear
		glm::vec2(1, 1),
		glm::vec2(1, 0),
		glm::vec2(0, 0),
	};

	vector<unsigned int> indices = {
		// base
		0, 1, 3, 2,
		// left
		1, 3, 6,
		// right
		0, 2, 6,
		// front
		0, 1, 6,
		// rear
		2, 3, 6
	};
}