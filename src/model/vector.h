#ifndef _MODEL_VECTOR_
#define _MODEL_VECTOR_

#include <cstdint>
#include <functional>

/*
 * An immutable representation of a point in 3-space.
 * Vector operations assume a right-hand-rule coordinate system.
 */
class Vector {
public:
	Vector(int32_t x, int32_t y, int32_t z);
	~Vector();

	int32_t getX() const { return x; }
	int32_t getY() const { return y; }
	int32_t getZ() const { return z; }

	Vector operator+(const Vector & v) const;
	Vector operator-(const Vector & v) const;
	Vector operator-(void) const;

	bool operator==(const Vector & v) const;

protected:
	int32_t x;
	int32_t y;
	int32_t z;
};

namespace std {
	template <> struct hash<Vector> {
		size_t operator() (const Vector & v) const {
			size_t result = 1;
			result = 31 * result + v.getX();
			result = 31 * result + v.getY();
			result = 31 * result + v.getZ();
			return result;
		}
	};
}

#endif // _MODEL_VECTOR_
