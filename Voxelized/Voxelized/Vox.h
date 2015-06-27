#pragma once

#include <SFML/Graphics.hpp>
#include <math.h>

using namespace std;
using namespace sf;

class Vox3D // class that manages particles following the Newton's law
{
public:

	Vox3D();
	Vox3D(float mass, Vector2f position, Vector2f speed);

	float getMass() const;
	void setMass(float const& mass);
	vector<Vector2f> getForce() const;
	Vector2f getTotalForce() const;
	void addForce(Vector2f const& force);
	void clearForce();
	Vector2f getPosition() const;
	void setPosition(Vector2f const& position);
	Vector2f getSpeed() const;
	void setSpeed(Vector2f const& speed);
	Vector2f getAcceleration() const;
	void setAcceleration(Vector2f const& acceleration);
	void setStatic();
	void setMovable();
	bool isMovable() const;

	void updatePosition(float const& dt);

private:
	float mass;
	vector<Vector2f> force; // Forces applied to this 3D Pixel
	Vector2f position; // Position of the 3D Pixel
	Vector2f speed; // Speed of the 3D Pixel : speed v= ( x(t+dt) - x(t) ) / dt | =======> v=dx/dt
	Vector2f acceleration; // Acceleration of the 3D Pixel : a = ( v(t+dt) - v(t) ) / dt | ========> a=dv/dt
	bool movable; // State of the 3D Pixel
};
