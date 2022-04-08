#ifndef GAMEOBJ3D_HPP
#define GAMEOBJ3D_HPP

#include "Model3D.hpp"
#include "glm/gtx/quaternion.hpp"
#include <algorithm>
#include <initializer_list>
#include <limits>

class GameObj3D;
bool CollidesWithSth(GameObj3D&);

struct TransformInfo
{
public:
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation; // degrees

    glm::mat4 getTransformMatrix()
    {
        return glm::translate(glm::mat4(1.0f), this->position) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), this->scale);
    }
};

struct Collider
{
public:
    float getMaxX() { return maxX; }
    float getMinX() { return minX; }
    float getMaxY() { return maxY; }
    float getMinY() { return minY; }
    float getMaxZ() { return maxZ; }
    float getMinZ() { return minZ; }

private:
    bool isBox = true; // true = box collider, false = sphere collider
    glm::vec3 center;

    // for box

    glm::vec3 toX, toY, toZ;
    float maxX, minX, maxY, minY, maxZ, minZ;

    void updateMinMax(){
        float maxXD = (toX.x>=0?toX.x:-toX.x) + (toY.x>=0?toY.x:-toY.x) + (toZ.x>=0?toZ.x:-toZ.x);
        float maxYD = (toX.y>=0?toX.y:-toX.y) + (toY.y>=0?toY.y:-toY.y) + (toZ.y>=0?toZ.y:-toZ.y);
        float maxZD = (toX.z>=0?toX.z:-toX.z) + (toY.z>=0?toY.z:-toY.z) + (toZ.z>=0?toZ.z:-toZ.z);

        maxX = center.x + maxXD;
        minX = center.x - maxXD;
        maxY = center.y + maxYD;
        minY = center.y - maxYD;
        maxZ = center.z + maxZD;
        minZ = center.z - maxZD;

    };
    // for sphere
    float radius;

friend class GameObj3D;
};

class GameObj3D
{
public:
    GameObj3D(Model3D &model, bool isBoxCollider = true);
    ~GameObj3D();

    void draw();
    void load(Model3D *model);

    void translate(float x, float y, float z);
    void scale(float x, float y, float z);
    void rotate(float x, float y, float z, bool called); // degrees
    void moveTowards(float x, float y, float z, float speed);

    void moveFront(float num, bool called);
    void moveRight(float num, bool called);
    void moveUp(float num, bool called);
    void moveBack(float num, bool called);

    glm::vec3 front();
    glm::vec3 back();
    glm::vec3 up();
    glm::vec3 right();


    glm::vec3 position();
    glm::vec3 scale();
    glm::vec3 rotation();
    glm::mat4 getTransform();

    Collider collider;
    unsigned textureId = 0;
    unsigned id;

    bool hasGravity = true;
    float velocityY = 0;
    bool isCollid = true;
    string type = "block";

private:
    glm::vec3 _front = glm::vec3(0, 0, -1);
    glm::vec3 _back = glm::vec3(0, 0, 1);
    glm::vec3 _up = glm::vec3(0, 1, 0);
    glm::vec3 _right = glm::vec3(-1, 0, 0);

private:
    void setCollider(bool isBoxCollider);
    static unsigned uuid;

private:
    Model3D *model3d = nullptr;
    TransformInfo transformInfo;
};

GameObj3D::GameObj3D(Model3D &model, bool isBoxCollider)
{
    this -> id = GameObj3D::uuid;
    GameObj3D::uuid++;

    this->transformInfo.position = glm::vec3(0);
    this->transformInfo.scale = glm::vec3(1);
    this->transformInfo.rotation = glm::vec3(0);

    this->model3d = &model;

    // set collider
    setCollider(isBoxCollider);
}

GameObj3D::~GameObj3D()
{
    cout << "Object " << id << " deleted." << endl;
}

void GameObj3D::draw()
{
    this->model3d->bind();
    glDrawElements(GL_TRIANGLES, model3d->getIndicesCount(), GL_UNSIGNED_INT, NULL);
}

void GameObj3D::load(Model3D *model)
{
    delete (this->model3d);
    this->model3d = model3d;
    collider.updateMinMax();
};

void GameObj3D::translate(float x, float y, float z)
{   
    // update collider position
    const glm::vec3 posPrev = this->transformInfo.position;
    collider.center += glm::vec3(x-posPrev.x,y-posPrev.y,z-posPrev.z);

    this->transformInfo.position = glm::vec3(x, y, z);
    
    collider.updateMinMax();
}

void GameObj3D::moveTowards(float x, float y, float z, float speed)
{
    // update collider position
    const glm::vec3 posPrev = this->transformInfo.position;
    collider.center = posPrev + (glm::vec3(x, y, z) - posPrev) * speed;

    this->transformInfo.position = posPrev + (glm::vec3(x, y, z) - posPrev) * speed;

    collider.updateMinMax();
}

void GameObj3D::scale(float x, float y, float z)
{
    // update collider scale
    const glm::vec3 posPrev = this->transformInfo.scale;
    const float xR = (x - posPrev.x);
    const float yR = (y - posPrev.y);
    const float zR = (z - posPrev.z);

    collider.toX += collider.toX * xR;
    collider.toY += collider.toY * yR;
    collider.toZ += collider.toZ * zR;

    this->transformInfo.scale = glm::vec3(x, y, z);
    collider.updateMinMax();
}

void GameObj3D::rotate(float x, float y, float z, bool called = false)
{
    const glm::vec3 rotPrev = glm::degrees(this->transformInfo.rotation);
    if(!called && rotPrev == glm::vec3(x,y,z)) {
        return;
    }

    // update collider rotation
    const float xR = (x - rotPrev.x);
    const float yR = (y - rotPrev.y);
    const float zR = (z - rotPrev.z);

    collider.toX = glm::vec3(glm::toMat4(glm::quat(glm::radians(glm::vec3(xR,yR,zR)))) * glm::vec4(collider.toX, 1));
    collider.toY = glm::vec3(glm::toMat4(glm::quat(glm::radians(glm::vec3(xR,yR,zR)))) * glm::vec4(collider.toY, 1));
    collider.toZ = glm::vec3(glm::toMat4(glm::quat(glm::radians(glm::vec3(xR,yR,zR)))) * glm::vec4(collider.toZ, 1));

    // rotate object
    const glm::vec3 rt = glm::radians(glm::vec3(x,y,z));
    this->transformInfo.rotation = rt;

    glm::mat4 trans = glm::toMat4(glm::quat(rt));
    //_up = glm::vec3(trans * glm::vec4(glm::vec3(0, 1, 0), 1));
    //_front = glm::vec3(trans * glm::vec4(glm::vec3(0, 0, -1), 1));
    //_right = glm::cross(_front, _up);

    collider.updateMinMax();

    if(isCollid && !called && CollidesWithSth(*this)){
        rotate(rotPrev.x, rotPrev.y, rotPrev.z, true);
    }
}

void GameObj3D::moveFront(float num, bool called = false) {
    const glm::vec3 newPos = front() * num + position();
    translate(newPos.x, newPos.y, newPos.z);

    if(isCollid && !called && CollidesWithSth(*this)){
        moveFront(-num, true);
    }
}

void GameObj3D::moveBack(float num, bool called = false) {
    const glm::vec3 newPos = back() * num + position();
    translate(newPos.x, newPos.y, newPos.z);

    if (isCollid && !called && CollidesWithSth(*this)) {
        moveBack(-num, true);
    }
}

void GameObj3D::moveRight(float num, bool called = false) {
    const glm::vec3 newPos = right() * num + position();
    translate(newPos.x, newPos.y, newPos.z);
    
    if(isCollid && !called && CollidesWithSth(*this)){
        moveRight(-num, true);
    }
}

void GameObj3D::moveUp(float num, bool called = false) {
    const glm::vec3 newPos = up() * num + position();
    translate(newPos.x, newPos.y, newPos.z);

    if(isCollid && !called && CollidesWithSth(*this)){
        moveUp(-num, true);
    }
}

glm::vec3 GameObj3D::front() {
    return glm::normalize(_front);
}

glm::vec3 GameObj3D::back() {
    return glm::normalize(_back);
}

glm::vec3 GameObj3D::up() {
    return glm::normalize(_up);
}

glm::vec3 GameObj3D::right() {
    return glm::normalize(glm::cross(_up, _front));
}

glm::vec3 GameObj3D::position()
{
    return this->transformInfo.position;
}

glm::vec3 GameObj3D::scale()
{
    return this->transformInfo.scale;
}

glm::vec3 GameObj3D::rotation()
{
    return glm::degrees(this->transformInfo.rotation);
}

glm::mat4 GameObj3D::getTransform()
{
    return this->transformInfo.getTransformMatrix();
}

void GameObj3D::setCollider(bool isBoxCollider)
{
    float 
        maxX = std::numeric_limits<float>::min(), minX = std::numeric_limits<float>::max(),
        maxY = std::numeric_limits<float>::min(), minY = std::numeric_limits<float>::max(),
        maxZ = std::numeric_limits<float>::min(), minZ = std::numeric_limits<float>::max();
    
    for (std::vector<glm::vec3>::iterator t = this->model3d->positions.begin(); t != this->model3d->positions.end(); ++t) {
        float x = t->x, y = t->y, z = t->z;
        if(x > maxX) maxX = x;
        else if(x < minX) minX = x;
        if(y > maxY) maxY = y;
        else if(y < minY) minY = y;
        if(z > maxZ) maxZ = z;
        else if(z < minZ) minZ = z;
    }

    float xD = (maxX - minX)*0.5, yD = (maxY - minY)*0.5, zD = (maxY - minY)*0.5;

    collider.center = glm::vec3(minX + xD, minY + yD, minZ + zD);
    collider.toX = glm::vec3(xD,0,0);
    collider.toY = glm::vec3(0,yD,0);
    collider.toZ = glm::vec3(0,0,zD);

    // for box
    if(isBoxCollider) {
        collider.updateMinMax();
    } 
}

unsigned GameObj3D::uuid = 0;

#endif