

#ifndef EZR_PROJECT_CUBE_H
#define EZR_PROJECT_CUBE_H

#include "Renderable.h"

namespace ezr
{
    class Cube : public Renderable
    {
    public:
        Cube(float size = 1.0f);

        virtual void draw() override;
    };

}

#endif //EZR_PROJECT_CUBE_H
