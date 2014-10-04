#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace atomic
{
	class Camera
	{
	public:
		Camera();
		
		const glm::vec3& getPosition() const;
		void setPosition(glm::vec3 position);

		float getFieldOfView() const;
		void setFieldOfView(float fieldOfView);

		void getClippingPlanes(float *zNear, float *zFar) const;
		void setClippingPlanes(float zNear, float zFar);

		void move(glm::vec3 offset);
		void look(glm::vec3 angles);
		void lookAt(glm::vec3 target);

		glm::mat4 getRotationMatrix() const;
		glm::mat4 getViewMatrix() const;
		glm::mat4 getProjectionMatrix(float aspectRatio) const;

		glm::vec3 getForwardVector() const;
		glm::vec3 getRightVector() const;
		glm::vec3 getUpVector() const;
		
	private:
		glm::vec3 position;
		glm::vec3 rotation;

		float fieldOfView;
		float zNear, zFar;

		void clampRotation();
	};
}