#pragma once


#include "camera.h"


class Renderer
{
    void begin_scene(const Camera& camera);
    void end_scene();
    void flush();
};