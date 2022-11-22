#pragma once

#include <iostream>

#include <glew.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <vec3.hpp>
#include <mat4x4.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

enum direction
{
	UP = 0, DOWN, LEFT, RIGHT
};

class Camera
{
private:
	glm::mat4 ViewMatrix;

	GLfloat movementSpeed;
	GLfloat sensitivity;

	glm::vec3 worldUp;
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;

	GLfloat pitch;
	GLfloat yaw;
	GLfloat roll;

	void updateCameraVectors()
	{
		this->front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		this->front.y = sin(glm::radians(this->pitch));
		this->front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

		this->front = glm::normalize(this->front);
		this->right = glm::normalize(glm::cross(this->front, this->worldUp));
		this->up = glm::normalize(glm::cross(this->right, this->front));
	}

public:
	Camera()
	{

	}
	Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 worldUp)
	{
		this->ViewMatrix = glm::mat4(1.f);

		this->movementSpeed = 0.1f;
		this->sensitivity = 10.f;

		this->worldUp = worldUp;
		this->position = position;
		this->right = glm::vec3(0.f);
		this->up = worldUp;

		this->pitch = 0.f;
		this->yaw = 45.f;
		this->roll = 0.f;

		this->updateCameraVectors();
	}
	~Camera()
	{

	}

	const glm::mat4 getViewMatrix()
	{
		this->updateCameraVectors();

		this->ViewMatrix = glm::lookAt(this->position, this->position + this->front, this->up);

		return this->ViewMatrix;
	}

	const glm::vec3 getPosition() const
	{
		return this->position;
	}

	void setPosition(glm::vec3 oldPos)
	{
		this->position = oldPos;
	}

	void updateKeyboardInput(const float& dt, const int direction)
	{
		//Update position vect
		switch (direction)
		{
		case UP:
			this->position += this->front * this->movementSpeed * dt;
			break;
		case DOWN:
			this->position -= this->front * this->movementSpeed * dt;
			break;
		default:
			break;
		}
		if (this->position.x < 0.01f) this->position.x = 0.01f;
		if (this->position.y < 0.01f) this->position.y = 0.01f;
		if (this->position.z < 0.01f) this->position.z = 0.01f;
		if (this->position.x > 1.f - 0.01f) this->position.x = 1.f - 0.01f;
		if (this->position.y > 1.f - 0.01f) this->position.y = 1.f - 0.01f;
		if (this->position.z > 1.f - 0.01f) this->position.z = 1.f - 0.01f;
	}

	void updateMouseInput(const float& dt, const double& offsetX, const double& offsetY)
	{
		//Update pitch yaw roll
		this->pitch -= static_cast<GLfloat>(offsetY) * this->sensitivity * dt;
		this->yaw += static_cast<GLfloat>(offsetX) * this->sensitivity * dt;

		if (this->pitch > 89.f) this->pitch = 89.f;
		else if (this->pitch < -89.f) this->pitch = -89.f;

		if (this->yaw > 360.f) this->yaw -= 360.f;
		else if (this->yaw < -360.f) this->yaw += 360.f;
	}

	void updateInput(const float& dt, const int direction, const double& offsetX, const double& offsetY)
	{
		this->updateMouseInput(dt, offsetX, offsetY);
		this->updateKeyboardInput(dt, direction);
	}
};