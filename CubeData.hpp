#pragma once

#include <vector>
#include "glm/glm.hpp"
using namespace std;

namespace CubeData
{
    vector<glm::vec3> positions = {
        // front
        glm::vec3(1.0, 1.0, 1.0),
        glm::vec3(1.0, -1.0, 1.0),
        glm::vec3(-1.0, -1.0, 1.0),
        glm::vec3(-1.0, 1.0, 1.0),
        // back
        glm::vec3(1.0, 1.0, -1.0),
        glm::vec3(1.0, -1.0, -1.0),
        glm::vec3(-1.0, -1.0, -1.0),
        glm::vec3(-1.0, 1.0, -1.0),
        // right
        glm::vec3(1.0, 1.0, -1.0),
        glm::vec3(1.0, -1.0, -1.0),
        glm::vec3(1.0, -1.0, 1.0),
        glm::vec3(1.0, 1.0, 1.0),
        // left
        glm::vec3(-1.0, 1.0, 1.0),
        glm::vec3(-1.0, -1.0, 1.0),
        glm::vec3(-1.0, -1.0, -1.0),
        glm::vec3(-1.0, 1.0, -1.0),
        // top
        glm::vec3(1.0, 1.0, -1.0),
        glm::vec3(1.0, 1.0, 1.0),
        glm::vec3(-1.0, 1.0, 1.0),
        glm::vec3(-1.0, 1.0, -1.0),
        // bottom
        glm::vec3(1.0, -1.0, -1.0),
        glm::vec3(1.0, -1.0, 1.0),
        glm::vec3(-1.0, -1.0, 1.0),
        glm::vec3(-1.0, -1.0, -1.0)};

    vector<glm::vec3> normals = {
        // front
        glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0),
        // back
        glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 0.0, -1.0),
        // right
        glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0),
        // left
        glm::vec3(-1.0, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0),
        // top
        glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0),
        // bottom
        glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, -1.0, 0.0)};

    vector<glm::vec2> uvs(unsigned x = 1, unsigned y = 1)
    {
        vector<glm::vec2> uv = {
            // front
            glm::vec2(0.666 * x, 0.666 * y),
            glm::vec2(0.666 * x, 0.333 * y),
            glm::vec2(0.333 * x, 0.333 * y),
            glm::vec2(0.333 * x, 0.666 * y),
            // back
            glm::vec2(0.666 * x, 0.666 * y),
            glm::vec2(0.666 * x, 0.333 * y),
            glm::vec2(0.333 * x, 0.333 * y),
            glm::vec2(0.333 * x, 0.666 * y),
            // right
            glm::vec2(0.666 * x, 0.666 * y),
            glm::vec2(0.666 * x, 0.333 * y),
            glm::vec2(0.333 * x, 0.333 * y),
            glm::vec2(0.333 * x, 0.666 * y),
            // left
            glm::vec2(0.666 * x, 0.666 * y),
            glm::vec2(0.666 * x, 0.333 * y),
            glm::vec2(0.333 * x, 0.333 * y),
            glm::vec2(0.333 * x, 0.666 * y),
            // top
            glm::vec2(0.666 * x, 1.0 * y),
            glm::vec2(0.666 * x, 0.666 * y),
            glm::vec2(0.333 * x, 0.666 * y),
            glm::vec2(0.333 * x, 1.0 * y),
            // bottom
            glm::vec2(0.666 * x, 0.333 * y),
            glm::vec2(0.666 * x, 0.0 * y),
            glm::vec2(0.333 * x, 0.0 * y),
            glm::vec2(0.333 * x, 0.333 * y)};
        return uv;
    }

    vector<glm::vec2> uvs_floor(unsigned x = 1, unsigned y = 1)
    {
        vector<glm::vec2> uv = {
            // front
            glm::vec2(1.0 * x, 1.0 * y),
            glm::vec2(1.0 * x, 0.0 * y),
            glm::vec2(0.0 * x, 0.0 * y),
            glm::vec2(0.0 * x, 1.0 * y),
            // back
            glm::vec2(1.0 * x, 1.0 * y),
            glm::vec2(1.0 * x, 0.0 * y),
            glm::vec2(0.0 * x, 0.0 * y),
            glm::vec2(0.0 * x, 1.0 * y),
            // right
            glm::vec2(1.0 * x, 1.0 * y),
            glm::vec2(1.0 * x, 0.0 * y),
            glm::vec2(0.0 * x, 0.0 * y),
            glm::vec2(0.0 * x, 1.0 * y),
            // left
            glm::vec2(1.0 * x, 1.0 * y),
            glm::vec2(1.0 * x, 0.0 * y),
            glm::vec2(0.0 * x, 0.0 * y),
            glm::vec2(0.0 * x, 1.0 * y),
            // top
            glm::vec2(1.0 * x, 1.0 * y),
            glm::vec2(1.0 * x, 0.0 * y),
            glm::vec2(0.0 * x, 0.0 * y),
            glm::vec2(0.0 * x, 1.0 * y),
            // bottom
            glm::vec2(1.0 * x, 1.0 * y),
            glm::vec2(1.0 * x, 0.0 * y),
            glm::vec2(0.0 * x, 0.0 * y),
            glm::vec2(0.0 * x, 1.0 * y)
            };
        return uv;
    }

    vector<unsigned int> indices = {
        // front
        0,
        1,
        2,
        2,
        3,
        0,
        // back
        4,
        5,
        6,
        6,
        7,
        4,
        // right
        8,
        9,
        10,
        10,
        11,
        8,
        // left
        12,
        13,
        14,
        14,
        15,
        12,
        // top
        16,
        17,
        18,
        18,
        19,
        16,
        // bottom
        20,
        21,
        22,
        22,
        23,
        20,
    };
}