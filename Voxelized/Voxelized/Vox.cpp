
#include "Vox.h"

Vox3D::Vox3D() {
	this->mass = 50;
	this->position = Vector2f(50, 50);
	this->speed = Vector2f(0, 0);
	this->acceleration = Vector2f(0, 0);
	this->movable = true;
}
Vox3D::Vox3D(float mass, Vector2f position, Vector2f speed) {
	this->mass = mass;
	this->position = position;
	this->speed = speed;
}

float Vox3D::getMass() const {
	return mass;
}
void Vox3D::setMass(float const& mass) {
	this->mass = mass;
}

vector<Vector2f> Vox3D::getForce() const {
	return this->force;
}
Vector2f Vox3D::getTotalForce() const {
	Vector2f totalForce(0, 0);
	for (int i(0); i < int(this->force.size()); i++) {
		totalForce += this->force[i];
	}
	return totalForce;
}
void Vox3D::addForce(Vector2f const& force) {
	this->force.push_back(force);
}
void Vox3D::clearForce() {
	this->force.clear();
}

Vector2f Vox3D::getPosition() const {
	return this->position;
}
void Vox3D::setPosition(Vector2f const& position) {
	this->position = position;
}

Vector2f Vox3D::getSpeed() const {
	return this->speed;
}
void Vox3D::setSpeed(Vector2f const& speed) {
	this->speed = speed;
}

Vector2f Vox3D::getAcceleration() const {
	return this->acceleration;
}
void Vox3D::setAcceleration(Vector2f const& acceleration) {
	this->acceleration = acceleration;
}
void Vox3D::setStatic() {
	this->movable = false;
}
void Vox3D::setMovable() {
	this->movable = true;
}
bool Vox3D::isMovable() const {
	return this->movable;
}

void Vox3D::updatePosition(float const& dt) {

	Vector2f previousPosition = this->position;
	/*
	Newton's law states : F=m*a, that is to say a=F/m
	we integrate this equation with the following approximation : between each frame, the acceleration is constant (it's nearly true)
	Notation : x(t) is the position during the current frame, x(t+dt) is the position we want to find for the next frame. Same for v(t)...
	so we have v(t+dt)=(F/m)*dt + v(t) -> if we differentiate this equation, we indeed find back a=F/m.
	We integrate again the equation above considering that v is constant aswell, thus we get x(t+dt) = (1/2)*(F/m)*dt^2 + v(t)*dt + x(t) -> knowing F, m, dt, 
	the speed of the particle and its position, we can guess its next position !
	This is known as the leapfrog numerical integration. It is equivalent to the Runge Kutta order 2 (RK2)
	*/
	this->position = this->position + this->speed*dt + 0.5f*this->getTotalForce() / this->mass*(float)pow(dt, 2); /* The equation from above */
	this->speed = (this->position - previousPosition) / dt; /* Update the speed by differentiation the position - i.e. make the particle move */
}
