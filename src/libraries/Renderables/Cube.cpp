//
// Created by dino on 15.12.15.
//

#include "Cube.h"

ezr::Cube::Cube(float size)
{
    _mode = GL_TRIANGLES;

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    GLuint vertexBufferHandles[3];
    glGenBuffers(3, vertexBufferHandles);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandles[0]);
    float positions[] = {
            -size,-size,size, size,-size,size, size,size,size,
            size,size,size, -size,size,size, -size,-size,size,
            // Right face
            size,-size,size, size,-size,-size, size,size,-size,
            size,size,-size, size,size,size, size,-size,size,
            // Back face
            -size,-size,-size, -size,size,-size, size,size,-size,
            size,size,-size, size,-size,-size, -size,-size,-size,
            // Left face
            -size,-size,size, -size,size,size, -size,size,-size,
            -size,size,-size, -size,-size,-size, -size,-size,size,
            // Bottom face
            -size,-size,size, -size,-size,-size, size,-size,-size,
            size,-size,-size, size,-size,size, -size,-size,size,
            // Top Face
            -size,size,size, size,size,size, size,size,-size,
            size,size,-size, -size,size,-size, -size,size,size,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    GLfloat uvCoordinates[] = {
            // Front face
            0,0, 1,0, 1,1,
            1,1, 0,1, 0,0,
            // Right face
            0,0, 1,0, 1,1,
            1,1, 0,1, 0,0,
            // Back face
            0,0, 1,0, 1,1,
            1,1, 0,1, 0,0,
            // Left face
            0,0, 1,0, 1,1,
            1,1, 0,1, 0,0,
            // Bottom face
            0,0, 1,0, 1,1,
            1,1, 0,1, 0,0,
            // Top face
            0,0, 1,0, 1,1,
            1,1, 0,1, 0,0,
    };
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandles[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvCoordinates), uvCoordinates, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    GLfloat normals[] = {
            // Front face
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,

            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            // Right face
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,

            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,

            // Back face
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,

            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            // Left face
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,

            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            // Bottom face
            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,

            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            // Top face
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,

            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,

    };
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandles[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    _vboHandles.push_back(vertexBufferHandles[0]);
    _vboHandles.push_back(vertexBufferHandles[1]);
    _vboHandles.push_back(vertexBufferHandles[2]);
}

void ezr::Cube::draw()
{
    glBindVertexArray(_vao);
    glDrawArrays(_mode, 0, 36);
}
