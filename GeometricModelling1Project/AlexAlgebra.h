#ifndef ALEX_ALGEBRA_H
#define ALEX_ALGEBRA_H

#include <cmath>

namespace aa {
	
	// Basic structures
	struct vec2;
	struct vec3;
	struct vec4;
	struct mat4;

	struct vec2 {
		float x, y;
		
		// Constructors
		vec2() : x(0), y(0) {};
		vec2(float scalar) : x(scalar), y(scalar) {};
		vec2(float _x, float _y) : x(_x), y(_y) {};

		// Operators
		vec2 operator+(const vec2& v) const
		{
			return vec2(x + v.x, y + v.y);
		}
		vec2 operator-(const vec2& v) const
		{
			return vec2(x - v.x, y - v.y);
		}
		vec2 operator*(const float s) const
		{
			return vec2(x * s, y * s);
		}
		vec2 operator/(const float s) const
		{
			return vec2(x / s, y / s);
		}
		vec2& operator+=(const vec2& v)
		{
			x += v.x;
			y += v.y;
			return *this;
		}
		vec2& operator-=(const vec2& v)
		{
			x -= v.x;
			y -= v.y;
			return *this;
		}
		vec2 operator-()
		{
			return vec2(-x, -y);
		}
	};

	struct vec3 {
		union {
			struct { float x, y, z; };
			struct { float r, g, b; };
			struct { vec2 xy; float z_; };
			struct { float x_; vec2 yz; };
		};

		// Constructors
		vec3() : x(0), y(0), z(0) {};
		vec3(float scalar) : x(scalar), y(scalar), z(scalar) {};
		vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {};
		vec3(vec2 v, float _z) : x(v.x), y(v.y), z(_z) {};
		vec3(float _x, vec2 v) : x(_x), y(v.x), z(v.y) {};

		// Operators
		vec3 operator+(const vec3& v) const
		{
			return vec3(x + v.x, y + v.y, z + v.z);
		}
		vec3 operator-(const vec3& v) const
		{
			return vec3(x - v.x, y - v.y, z - v.z);
		}
		vec3 operator*(const float s) const
		{
			return vec3(x * s, y * s, z * s);
		}
		vec3 operator/(const float s) const
		{
			return vec3(x / s, y / s, z / s);
		}
		vec3& operator+=(const vec3& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}
		vec3& operator-=(const vec3& v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
			return *this;
		}
		float& operator[](const int i)
		{
			return (&x)[i];
		}
		const float& operator[](const int i) const
		{
			return (&x)[i];
		}
		vec3 operator-()
		{
			return vec3(-x, -y, -z);
		}
	};

	struct vec4 {
		union {
			struct { float x, y, z, w; };
			struct { float r, g, b, a; };
			struct { vec2 xy, zw; };
			struct { vec3 xyz; float w_; };
			struct { float x_; vec3 yzw; };
			struct { float x__; vec2 yz; float w__; };
		};

		// Constructors
		vec4() : x(0), y(0), z(0), w(0) {};
		vec4(float scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {};
		vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {};
		vec4(vec2 v, float _z, float _w) : x(v.x), y(v.y), z(_z), w(_w) {};
		vec4(float _x, float _y, vec2 v) : x(_x), y(_y), z(v.x), w(v.y) {};
		vec4(vec3 v, float _w) : x(v.x), y(v.y), z(v.z), w(_w) {};

		// Operators
		vec4 operator+(const vec4& v) const
		{
			return vec4(x + v.x, y + v.y, z + v.z, w + v.w);
		}
		vec4 operator-(const vec4& v) const
		{
			return vec4(x - v.x, y - v.y, z - v.z, w - v.w);
		}
		vec4 operator*(const float s) const
		{
			return vec4(x * s, y * s, z * s, w * s);
		}
		vec4 operator/(const float s) const
		{
			return vec4(x / s, y / s, z / s, w / s);
		}
		vec4& operator+=(const vec4& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
			w += v.w;
			return *this;
		}
		vec4& operator-=(const vec4& v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
			w -= v.w;
			return *this;
		}
		float& operator[](const int i)
		{
			return (&x)[i];
		}
		const float& operator[](const int i) const
		{
			return (&x)[i];
		}
		vec4 operator-()
		{
			return vec4(-x, -y, -z, -w);
		}
		/*vec4 operator*(const mat4& b) const
		{
			vec4 a = *this;
			vec4 result;

			for (int i = 0; i < 4; i++)
				result[i] =
				a[0] * b[i][0] +
				a[1] * b[i][1] +
				a[2] * b[i][2] +
				a[3] * b[i][3];
			return result;
		}*/
	};

	struct mat4 {
		vec4 columns[4];
		vec4& operator[](const int i) { return columns[i]; };
		const vec4& operator[](const int i) const { return columns[i]; }

		// Constructors
		mat4() {
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					columns[i][j] = 0.0f;
		};
		mat4(float scalar) {
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					if(i==j)
						columns[i][j] = scalar;
					else
						columns[i][j] = 0.0f;
		};
		mat4(vec4 x, vec4 y, vec4 z, vec4 w)
		{
			columns[0] = x;
			columns[1] = y;
			columns[2] = z;
			columns[3] = w;
		}
		mat4(float a, float b, float c, float d)
		{
			columns[0][0] = a;
			columns[1][1] = b;
			columns[2][2] = c;
			columns[3][3] = d;
		}

		// Identity constructor
		static mat4 identity()
		{
			mat4 identity_matrix;
			identity_matrix[0][0] = 1;
			identity_matrix[1][1] = 1;
			identity_matrix[2][2] = 1;
			identity_matrix[3][3] = 1;
			return identity_matrix;
		}

		// Operators
		mat4 operator+(const mat4& b) const {
			mat4 a = *this;
			mat4 result;

			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					result[i][j] = a[i][j] + b[i][j];
			return result;
		}
		mat4 operator*(const mat4& b) const {
			mat4 a = *this;
			mat4 result;

			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					result[i][j] =
					a[0][j] * b[i][0] +
					a[1][j] * b[i][1] +
					a[2][j] * b[i][2] +
					a[3][j] * b[i][3];

			return result;
		}
		vec4 operator*(const vec4& b) const {
			mat4 a = *this;
			vec4 result;

			for (int i = 0; i < 4; i++)
				result[i] =
				a[0][i] * b[0] +
				a[1][i] * b[1] +
				a[2][i] * b[2] +
				a[3][i] * b[3];
			return result;
		}
	};

	// Operator overloading

	vec2 operator*(float scalar, vec2 v);
	vec3 operator*(float scalar, vec3 v);
	vec4 operator*(float scalar, vec4 v);

	// Functions

	float dot(vec2 a, vec2 b);
	float dot(vec3 a, vec3 b);
	float dot(vec4 a, vec4 b);

	vec3 cross(vec3 a, vec3 b);

	float length(vec2 a);
	float length(vec3 a);
	float length(vec4 a);

	float distance(vec2 a, vec2 b);
	float distance(vec3 a, vec3 b);
	float distance(vec4 a, vec4 b);

	vec2 normalize(vec2 a);
	vec3 normalize(vec3 a);
	vec4 normalize(vec4 a);

	float* value_ptr(vec2& a);
	float* value_ptr(vec3& a);
	float* value_ptr(vec4& a);
	float* value_ptr(mat4& a);

	mat4 lookAt(vec3 eye, vec3 center, vec3 up);

	mat4 inverse(const mat4& m);
	mat4 transpose(const mat4& m);
	vec3 reflect(const vec3& incident, const vec3& normal);

	enum Axis {
		X, Y, Z
	};
	mat4 scale(vec3 factor);
	mat4 rotate(Axis axis, float angle);
	mat4 translate(vec3 vector);

	// glm equivalent
	mat4 scale(mat4 model, vec3 factor);
	mat4 rotate(mat4 model, Axis axis, float angle);
	mat4 translate(mat4 model, vec3 vector);

	float radians(float degrees);
	float degrees(float radians);

	float clip(float scalar, float lower = 0.0f, float upper = 1.0f);
	vec2 clip(vec2 vector, float lower = 0.0f, float upper = 1.0f);
	vec3 clip(vec3 vector, float lower = 0.0f, float upper = 1.0f);
	vec4 clip(vec4 vector, float lower = 0.0f, float upper = 1.0f);
};


#endif