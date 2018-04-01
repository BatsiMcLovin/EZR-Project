//
// Created by dino on 02.12.15.
//

#include "Quad.h"

ezr::Quad::Quad()
{

    //Erstmal vao Handle bauen

    glGenVertexArrays(1, &_vao);

    // binden

    bind();


    float min_pos = -1.0f;
    float max_pos = 1.0f;

    float positions[] =
            {
                    min_pos, min_pos,
                    min_pos, max_pos,
                    max_pos, min_pos,
                    max_pos, max_pos
            };

    float min_uv = 0.0;
    float max_uv = 1.0f;

    float uv[] =
            {
                    min_uv, min_uv,
                    min_uv, max_uv,
                    max_uv, min_uv,
                    max_uv, max_uv
            };

    GLuint positionBuffer;
    glGenBuffers(1, &positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*8, positions, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint uvBuffer;
    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*8, uv, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    unbind();

    _indexCount = 4;
    _mode = GL_TRIANGLE_STRIP;
}

void ezr::Quad::draw()
{
    glBindVertexArray(_vao);
    glDrawArrays(_mode, 0, 4);
}
