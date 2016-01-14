#include "vector.h"

Vector::Vector(int32_t x, int32_t y, int32_t z)
: x(x), y(y), z(z) {
}

Vector::~Vector() { }

Vector Vector::operator+(const Vector & v) const {
	return Vector(x + v.x, y + v.y, z + v.z);
}

Vector Vector::operator-(const Vector & v) const {
	return Vector(x - v.x, y - v.y, z - v.z);
}

Vector Vector::operator-(void) const {
	return Vector(-x, -y, -z);
}

bool Vector::operator==(const Vector &v) const {
	return (x == v.x && y == v.y && z == v.z);
}
