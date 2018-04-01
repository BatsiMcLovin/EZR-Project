//
// Created by dino on 05.12.15.
//

#include "Camera.h"

ezr::Camera::Camera()
{
    _position = glm::vec3(0.0f, 1.0f, 1.0f);

}

glm::mat4 ezr::Camera::getViewMatrix()
{
    return getRotation() * getTranslation();
}

void ezr::Camera::setPosition(glm::vec3 vec)
{
    _position = vec;
}

void ezr::Camera::setRelativePosition(glm::vec3 vec)
{
    glm::vec4 temp(vec.x, vec.y, vec.z, 1.0f);

    temp = glm::transpose(getRotation()) * temp;

    _position += glm::vec3(temp.x, temp.y, temp.z);
}

void ezr::Camera::setOrientation(GLfloat x, GLfloat y, GLfloat z)
{
    _orientation = glm::vec3(x, y, z);
}

void ezr::Camera::setRelativeOrientation(GLfloat x, GLfloat y, GLfloat z)
{
    _orientation += glm::vec3(x, y, z);
}

glm::mat4 ezr::Camera::getRotation()
{
    return glm::rotate(_orientation.x, glm::vec3(1.0f, 0.0f, 0.0f))
            * glm::rotate(_orientation.y, glm::vec3(0.0f, 1.0f, 0.0f))
            * glm::rotate(_orientation.z, glm::vec3(0.0f, 0.0f, 1.0f));
}

glm::mat4 ezr::Camera::getTranslation()
{
    return glm::translate(-_position);
}

glm::vec3 ezr::Camera::getPosition() {
    return _position;
}

void ezr::Camera::setCamera(glm::mat4 cam_mat)
{

    glm::mat3 rot;
    rot[0][0] = cam_mat[0][0];
    rot[0][1] = cam_mat[0][1];
    rot[0][2] = cam_mat[0][2];
    rot[1][0] = cam_mat[1][0];
    rot[1][1] = cam_mat[1][1];
    rot[1][2] = cam_mat[1][2];
    rot[2][0] = cam_mat[2][0];
    rot[2][1] = cam_mat[2][1];
    rot[2][2] = cam_mat[2][2];

    _position.x = cam_mat[3][0];
    _position.y = cam_mat[3][1];
    _position.z = cam_mat[3][2];

    _position = glm::transpose(rot) * -_position;

    float y = asin(rot[2][0]);
    float x = acos(rot[2][2] / glm::cos(y));
    float z = acos(rot[0][0] / glm::cos(y));

    _orientation.x = x;
    _orientation.y = y;
    _orientation.z = z;
}
