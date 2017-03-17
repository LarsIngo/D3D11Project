#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <Windows.h>
#include "FrameBuffer.hpp"

Camera::Camera(float fov, FrameBuffer* frameBuffer)
{
    mProjectionMatrix = glm::perspectiveFovLH(glm::radians(fov), (float)frameBuffer->mWidth, (float)frameBuffer->mHeight, 0.01f, 200.f);
    mpFrameBuffer = frameBuffer;
}

Camera::~Camera() 
{

}

void Camera::Update(float moveSpeed, float rotationSpeed, float dt)
{
    // Get movement & rotation.
    glm::vec3 movement = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 rotation = glm::vec3(0.f, 0.f, 0.f);

    if (GetAsyncKeyState('W'))
    {
        movement += mFrontDirection;
    }
    if (GetAsyncKeyState('A'))
    {
        movement -= mRightDirection;
    }
    if (GetAsyncKeyState('S'))
    {
        movement -= mFrontDirection;
    }
    if (GetAsyncKeyState('D'))
    {
        movement += mRightDirection;
    }

    if (GetAsyncKeyState('Q'))
    {
        movement += mUpDirection;
    }

    if (GetAsyncKeyState('E'))
    {
        movement -= mUpDirection;
    }

    if (GetAsyncKeyState('Q'))
    {
        movement += mUpDirection;
    }

    if (GetAsyncKeyState('E'))
    {
        movement -= mUpDirection;
    }

    if (GetAsyncKeyState('X')) 
    {
        rotation -= glm::vec3(0.f, 0.f, 1.f);
    }

    if (GetAsyncKeyState('Z')) 
    {
        rotation += glm::vec3(0.f, 0.f, 1.f);
    }

    // Update postion & rotation.
    mPosition += movement * moveSpeed * dt;
    rotation *= rotationSpeed * dt;

    // Update direction vectors and matrices.
    Roll(rotation.z);
    Pitch(rotation.y);
    Yaw(rotation.x);

    mOrientationMatrix = CalculateOrientationMatrix();
    mViewMatrix = CalculateViewMatrix();
}

void Camera::Yaw(float rotation)
{
    glm::quat q = glm::angleAxis(glm::radians(rotation), mUpDirection);
    mRightDirection = glm::normalize(q * mRightDirection);
    mFrontDirection = glm::normalize(q * mFrontDirection);
}

void Camera::Pitch(float rotation)
{
    glm::quat q = glm::angleAxis(glm::radians(rotation), mRightDirection);
    mFrontDirection = glm::normalize(q * mFrontDirection);
    mUpDirection = glm::normalize(q * mUpDirection);
}

void Camera::Roll(float rotation)
{
    glm::quat q = glm::angleAxis(glm::radians(rotation), mFrontDirection);
    mUpDirection = glm::normalize(q * mUpDirection);
    mRightDirection = glm::normalize(q * mRightDirection);
}

glm::mat4 Camera::CalculateOrientationMatrix() const
{
    return glm::transpose(glm::mat4(
        glm::vec4(mRightDirection, 0.f),
        glm::vec4(mUpDirection, 0.f),
        glm::vec4(mFrontDirection, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)
    ));
}

glm::mat4 Camera::CalculateViewMatrix() const 
{
    return mOrientationMatrix * glm::translate(glm::mat4(), -mPosition);
}
