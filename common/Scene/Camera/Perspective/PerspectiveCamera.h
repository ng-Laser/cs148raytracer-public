#pragma once

#include "common/Scene/Camera/Camera.h"

class PerspectiveCamera : public Camera
{
public:
    // inputFov is in degrees. 
    PerspectiveCamera(float aspectRatio, float inputFov, float zNear=0.f, float zFar=std::numeric_limits<float>::max());
    virtual std::shared_ptr<class Ray> GenerateRayForNormalizedCoordinates(glm::vec2 coordinate) const override;

    void SetZNear(float input);
    void SetZFar(float input);

private:
    float aspectRatio;
    float fov; // fov is stored as radians

    float zNear;
    float zFar;
};