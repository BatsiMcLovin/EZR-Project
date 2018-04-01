//
// Created by dino on 02.12.15.
//

#ifndef EZR_PROJECT_TRIANGLE_H
#define EZR_PROJECT_TRIANGLE_H


#include "Renderable.h"


namespace ezr
{

    class Quad : public Renderable
    {
    public:
        Quad();

        virtual void draw() override;


    private:
    };

}


#endif //EZR_PROJECT_TRIANGLE_H
