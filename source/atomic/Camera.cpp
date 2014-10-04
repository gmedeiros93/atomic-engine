#include "Camera.h"
#include <stdio.h>

using namespace atomic;

Camera::Camera() :
fieldOfView(75.0f),
zNear(0.1f), zFar(100.0f)
{
	this->position = glm::vec3();
	this->rotation = glm::vec3();
}

const glm::vec3& Camera::getPosition() const
{
	return position;
}

void Camera::setPosition(glm::vec3 position)
{
	this->position = position;
}

float Camera::getFieldOfView() const
{
	return fieldOfView;
}

void Camera::setFieldOfView(float fieldOfView)
{
	this->fieldOfView = fieldOfView;
}

void Camera::getClippingPlanes(float *zNear, float *zFar) const
{
	*zNear = this->zNear;
	*zFar = this->zFar;
}

void Camera::setClippingPlanes(float zNear, float zFar)
{
	assert(zNear > 0.0f);
	assert(zFar > zNear);

	this->zNear = zNear;
	this->zFar = zFar;
}

void Camera::move(glm::vec3 offset)
{
	position += offset;
}

void Camera::look(glm::vec3 angles)
{
	rotation += angles;
	clampRotation();
}

void Camera::lookAt(glm::vec3 target)
{
	assert(position != target);
	glm::vec3 direction = glm::normalize(target - position);
	rotation[0] = asinf(-direction.y);
	rotation[1] = atan2f(-direction.x, -direction.z);
	rotation[2] = 0.0f;
	clampRotation();
}

glm::mat4 Camera::getRotationMatrix() const
{
	glm::mat4 mat;
	mat = glm::rotate(mat, rotation[1], glm::vec3(1,0,0));
	mat = glm::rotate(mat, rotation[0], glm::vec3(0,1,0));
	//mat = glm::rotate(mat, rotation[2], glm::vec3(0,0,1));
	return mat;
}

glm::mat4 Camera::getViewMatrix() const
{
	return getRotationMatrix() * glm::translate(glm::mat4(), -position);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const
{
	return glm::perspective(glm::radians(fieldOfView), aspectRatio, zNear, zFar);
	//return glm::infinitePerspective(fieldOfView, aspectRatio, zNear);
}

glm::vec3 Camera::getForwardVector() const
{
	glm::vec4 forward = glm::inverse(getRotationMatrix()) * glm::vec4(0,0,-1,1);
	return glm::vec3(forward);
}

glm::vec3 Camera::getRightVector() const
{
	glm::vec4 right = glm::inverse(getRotationMatrix()) * glm::vec4(1,0,0,1);
	return glm::vec3(right);
}

glm::vec3 Camera::getUpVector() const
{
	glm::vec4 up = glm::inverse(getRotationMatrix()) * glm::vec4(0,1,0,1);
	return glm::vec3(up);
}

float clamp(float value, float min, float max)
{
	if (value < min)
		return min;
	else if (value > max)
		return max;
	else
		return value;
}

void Camera::clampRotation()
{
	float full = glm::radians(360.0f);
	float gimbal = glm::radians(85.0f);

	rotation[0] = fmodf(rotation[0], full);
	rotation[1] = clamp(rotation[1], -gimbal, gimbal);
	rotation[2] = fmodf(rotation[2], full);

	if (rotation[0] < 0.0f)
		rotation[0] += full;

	if (rotation[2] < 0.0f)
		rotation[2] += full;
}