#pragma once

#include <glm/glm.hpp>

using glm::bvec3;
using glm::vec2;
using glm::vec3;
using glm::mat4;

extern const float MAX_FOV;
extern const float MIN_FOV;

extern const vec3 WORLD_AXIS_UP;
extern const vec3 WORLD_AXIS_FORWARD;
extern const vec3 WORLD_AXIS_RIGHT;


class Camera
{
public:
    vec3 position;

    vec3 forward;
    vec3 up;
    vec3 right;
    double pitch;
    double yaw;

    float  fov;
    float  speed;

    int width;
    int height;

    float near;
    float far;

    bool  is_orthographic;
    float aspect_ratio;

    explicit Camera();
    explicit Camera(vec3 position, vec3 target);

private:

};

void Move(Camera& camera, vec3 local_direction);
void Rotate(Camera& camera, vec2 rotation);
void RefreshCoordinateSystem(Camera& camera);

mat4 ViewMatrix(const Camera& camera);
mat4 ProjectionMatrix(const Camera& camera);

mat4 OrthographicProjectionMatrix(const Camera& camera);
mat4 PerspectiveProjectionMatrix(const Camera& camera);
