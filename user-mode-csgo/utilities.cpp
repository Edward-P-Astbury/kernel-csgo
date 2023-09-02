#include "utilities.h"

vector3 operator+(vector3 const& lhs, vector3 const& rhs)
{
	return vector3{ lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
}

vector3 operator-(vector3 const& lhs, vector3 const& rhs)
{
	return vector3{ lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
}

std::ostream& operator<<(std::ostream& os, vector3 const& pos)
{
	os << "xyz: " << pos.x << ", " << pos.y << ", " << pos.z;
	return os;
}

vector3& vector3::operator*=(float number)
{
	this->x *= number;
	this->y *= number;
	this->z *= number;
	return *this;
}

vector3& vector3::operator-=(vector3 const& rhs)
{
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	return *this;
}

float calculate_distance(float x1, float y1, float x2, float y2)
{
	return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}