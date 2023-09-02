#pragma once

#include <vector>
#include <ostream>

struct position
{
	float x;
	float y;
};

float calculate_distance(float x1, float y1, float x2, float y2);

typedef union vector3 
{
	struct 
	{
		float x, y, z;
	};
	friend vector3 operator + (vector3 const& lhs, vector3 const& rhs);
	friend vector3 operator - (vector3 const& lhs, vector3 const& rhs);
	vector3& operator *= (float number);
	vector3& operator -= (vector3 const& rhs);
	friend std::ostream& operator<<(std::ostream& os, vector3 const& pos);
};