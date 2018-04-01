//
// Created by dino on 05.12.15.
//

#ifndef EZR_PROJECT_CAMERA_H
#define EZR_PROJECT_CAMERA_H

#include "Utils/debug.h"

namespace ezr
{
    class Camera
    {
    public:
        Camera();

        glm::mat4 getViewMatrix();
        glm::mat4 getRotation();
        glm::mat4 getTranslation();

        glm::vec3 getPosition();

        void setPosition(glm::vec3 vec);
        void setOrientation(GLfloat x, GLfloat y, GLfloat z);
        void setRelativePosition(glm::vec3 vec);
        void setRelativeOrientation(GLfloat x, GLfloat y, GLfloat z);

        void setCamera(glm::mat4 cam_mat);

    private:
        glm::vec3 _position;
        glm::vec3 _orientation;
    };

}

#endif //EZR_PROJECT_CAMERA_H
