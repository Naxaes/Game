#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "debug.h"

const float MAX_FOV = 90.0f;
const float MIN_FOV = 30.0f;

extern const vec3 WORLD_AXIS_RIGHT   = vec3(1.0f,  0.0f,  0.0f);
extern const vec3 WORLD_AXIS_UP      = vec3(0.0f,  1.0f,  0.0f);
extern const vec3 WORLD_AXIS_FORWARD = vec3(0.0f,  0.0f,  1.0f);

Camera::Camera()
{
    this->position = vec3(0, 1, 3);
    this->forward  = -WORLD_AXIS_FORWARD;
    this->up       = WORLD_AXIS_UP;
    this->right    = glm::normalize(glm::cross(up, forward));

    this->fov   = 80.0f;
    this->speed = 4.0f;
    this->pitch = 0.0f;
    this->yaw   = 0.0f;

    // TODO(ted): FIX!
    this->width  = 0;
    this->height = 0;

    this->near = 0.1f;
    this->far  = 100.0f;

    this->aspect_ratio = 1.0f;
    this->is_orthographic = false;
}

Camera::Camera(vec3 position, vec3 target)
{

}

void Move(Camera& camera, vec3 local_direction)
{
    camera.position += camera.right   * local_direction.x;
    camera.position += camera.up      * local_direction.y;
    camera.position += camera.forward * local_direction.z;
}

// Does not support roll. Expects radians.
void Rotate(Camera& camera, vec2 rotation)
{
    camera.yaw   = std::fmod(camera.yaw + rotation.x, 2 * M_PI);
    camera.pitch = glm::clamp(camera.pitch + rotation.y, -M_PI_2 + 0.001f, M_PI_2 - 0.001f);

    camera.forward.x =  sin(camera.yaw) * cos(camera.pitch);
    camera.forward.z = -cos(camera.yaw) * cos(camera.pitch);  // Invert as we treat (0, 0, -1) as forward.
    camera.forward.y =  sin(camera.pitch);

    camera.forward = glm::normalize(camera.forward);
    camera.right   = glm::normalize(glm::cross(WORLD_AXIS_UP,  -camera.forward));
    camera.up      = glm::normalize(glm::cross(-camera.forward, camera.right));
}
void RefreshCoordinateSystem(Camera& camera)
{
    camera.forward.x = cos(camera.yaw) * cos(camera.pitch);
    camera.forward.y = sin(camera.pitch);
    camera.forward.z = sin(camera.yaw) * cos(camera.pitch);

    camera.forward = glm::normalize(camera.forward);
    camera.right   = glm::normalize(glm::cross(camera.forward, WORLD_AXIS_UP));
    camera.up      = glm::normalize(glm::cross(camera.right, camera.forward));
}


mat4 ViewMatrix(const Camera& camera)
{
    mat4 view(1.0f);
    view = glm::lookAt(camera.position, camera.position + camera.forward, WORLD_AXIS_UP);
    return view;
}

mat4 ProjectionMatrix(const Camera& camera)
{
    if (camera.is_orthographic)
        return OrthographicProjectionMatrix(camera);
    else
        return PerspectiveProjectionMatrix(camera);
}

mat4 OrthographicProjectionMatrix(const Camera& camera)
{
    mat4 projection(1.0f);
    projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, camera.near, camera.far);

    return projection;
}

mat4 PerspectiveProjectionMatrix(const Camera& camera)
{
    mat4 projection(1.0f);
    projection = glm::perspective(glm::radians(camera.fov), camera.aspect_ratio, camera.near, camera.far);
    return projection;
}