#include "AlexAlgebra.h"

namespace aa {
	// Operator overloading

	vec2 operator*(float scalar, vec2 v)
	{
		return vec2(scalar * v.x, scalar * v.y);
	}
	vec3 operator*(float scalar, vec3 v)
	{
		return vec3(scalar * v.x, scalar * v.y, scalar * v.z);
	}
	vec4 operator*(float scalar, vec4 v)
	{
		return vec4(scalar * v.x, scalar * v.y, scalar * v.z, scalar * v.w);
	}

	// Functions

	float dot(vec2 a, vec2 b)
	{
		return a.x * b.x + a.y * b.y;
	}
	float dot(vec3 a, vec3 b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}
	float dot(vec4 a, vec4 b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}

	vec3 cross(vec3 a, vec3 b)
	{
		vec3 result;
		result[0] = a[1] * b[2] - a[2] * b[1];
		result[1] = a[2] * b[0] - a[0] * b[2];
		result[2] = a[0] * b[1] - a[1] * b[0];
		return result;
	}

	float length(vec2 a)
	{
		return sqrt(dot(a, a));
	}
	float length(vec3 a)
	{
		return sqrt(dot(a, a));
	}
	float length(vec4 a)
	{
		return sqrt(dot(a, a));
	}

	float distance(vec2 a, vec2 b)
	{
		vec2 path = b - a;
		return sqrt(dot(path, path));
	}
	float distance(vec3 a, vec3 b)
	{
		vec3 path = b - a;
		return sqrt(dot(path, path));
	}
	float distance(vec4 a, vec4 b)
	{
		vec4 path = b - a;
		return sqrt(dot(path, path));
	}

	vec2 normalize(vec2 a)
	{
		float magnitude = length(a);
		return vec2(a.x / magnitude, a.y / magnitude);
	}
	vec3 normalize(vec3 a)
	{
		float magnitude = length(a);
		return vec3(a.x / magnitude, a.y / magnitude, a.z / magnitude);
	}
	vec4 normalize(vec4 a)
	{
		float magnitude = length(a);
		return vec4(a.x / magnitude, a.y / magnitude, a.z / magnitude, a.w / magnitude);
	}

	float* value_ptr(vec2& a)
	{
		return &(a.x);
	}
	float* value_ptr(vec3& a)
	{
		return &(a.x);
	}
	float* value_ptr(vec4& a)
	{
		return &(a.x);
	}
	float* value_ptr(mat4& a)
	{
		return &(a[0][0]);
	}

	mat4 lookAt(vec3 eye, vec3 center, vec3 up)
	{
		vec3 const f(normalize(center - eye));
		vec3 const s(normalize(cross(f, up)));
		vec3 const u(cross(s, f));

		mat4 result(1);
		result[0][0] = s.x;
		result[1][0] = s.y;
		result[2][0] = s.z;
		result[0][1] = u.x;
		result[1][1] = u.y;
		result[2][1] = u.z;
		result[0][2] = -f.x;
		result[1][2] = -f.y;
		result[2][2] = -f.z;
		result[3][0] = -dot(s, eye);
		result[3][1] = -dot(u, eye);
		result[3][2] =  dot(f, eye);
		result[3][3] = 1.0f;
		return result;
	}

	mat4 inverse(const mat4& m)
	{
		float inv[16];
		float det;

		// Flatten column-major mat4 into 1D array
		float a[16];
		for (int col = 0; col < 4; col++)
			for (int row = 0; row < 4; row++)
				a[col * 4 + row] = m[col][row];

		inv[0] = a[5] * a[10] * a[15] -
			a[5] * a[11] * a[14] -
			a[9] * a[6] * a[15] +
			a[9] * a[7] * a[14] +
			a[13] * a[6] * a[11] -
			a[13] * a[7] * a[10];

		inv[4] = -a[4] * a[10] * a[15] +
			a[4] * a[11] * a[14] +
			a[8] * a[6] * a[15] -
			a[8] * a[7] * a[14] -
			a[12] * a[6] * a[11] +
			a[12] * a[7] * a[10];

		inv[8] = a[4] * a[9] * a[15] -
			a[4] * a[11] * a[13] -
			a[8] * a[5] * a[15] +
			a[8] * a[7] * a[13] +
			a[12] * a[5] * a[11] -
			a[12] * a[7] * a[9];

		inv[12] = -a[4] * a[9] * a[14] +
			a[4] * a[10] * a[13] +
			a[8] * a[5] * a[14] -
			a[8] * a[6] * a[13] -
			a[12] * a[5] * a[10] +
			a[12] * a[6] * a[9];

		inv[1] = -a[1] * a[10] * a[15] +
			a[1] * a[11] * a[14] +
			a[9] * a[2] * a[15] -
			a[9] * a[3] * a[14] -
			a[13] * a[2] * a[11] +
			a[13] * a[3] * a[10];

		inv[5] = a[0] * a[10] * a[15] -
			a[0] * a[11] * a[14] -
			a[8] * a[2] * a[15] +
			a[8] * a[3] * a[14] +
			a[12] * a[2] * a[11] -
			a[12] * a[3] * a[10];

		inv[9] = -a[0] * a[9] * a[15] +
			a[0] * a[11] * a[13] +
			a[8] * a[1] * a[15] -
			a[8] * a[3] * a[13] -
			a[12] * a[1] * a[11] +
			a[12] * a[3] * a[9];

		inv[13] = a[0] * a[9] * a[14] -
			a[0] * a[10] * a[13] -
			a[8] * a[1] * a[14] +
			a[8] * a[2] * a[13] +
			a[12] * a[1] * a[10] -
			a[12] * a[2] * a[9];

		inv[2] = a[1] * a[6] * a[15] -
			a[1] * a[7] * a[14] -
			a[5] * a[2] * a[15] +
			a[5] * a[3] * a[14] +
			a[13] * a[2] * a[7] -
			a[13] * a[3] * a[6];

		inv[6] = -a[0] * a[6] * a[15] +
			a[0] * a[7] * a[14] +
			a[4] * a[2] * a[15] -
			a[4] * a[3] * a[14] -
			a[12] * a[2] * a[7] +
			a[12] * a[3] * a[6];

		inv[10] = a[0] * a[5] * a[15] -
			a[0] * a[7] * a[13] -
			a[4] * a[1] * a[15] +
			a[4] * a[3] * a[13] +
			a[12] * a[1] * a[7] -
			a[12] * a[3] * a[5];

		inv[14] = -a[0] * a[5] * a[14] +
			a[0] * a[6] * a[13] +
			a[4] * a[1] * a[14] -
			a[4] * a[2] * a[13] -
			a[12] * a[1] * a[6] +
			a[12] * a[2] * a[5];

		inv[3] = -a[1] * a[6] * a[11] +
			a[1] * a[7] * a[10] +
			a[5] * a[2] * a[11] -
			a[5] * a[3] * a[10] -
			a[9] * a[2] * a[7] +
			a[9] * a[3] * a[6];

		inv[7] = a[0] * a[6] * a[11] -
			a[0] * a[7] * a[10] -
			a[4] * a[2] * a[11] +
			a[4] * a[3] * a[10] +
			a[8] * a[2] * a[7] -
			a[8] * a[3] * a[6];

		inv[11] = -a[0] * a[5] * a[11] +
			a[0] * a[7] * a[9] +
			a[4] * a[1] * a[11] -
			a[4] * a[3] * a[9] -
			a[8] * a[1] * a[7] +
			a[8] * a[3] * a[5];

		inv[15] = a[0] * a[5] * a[10] -
			a[0] * a[6] * a[9] -
			a[4] * a[1] * a[10] +
			a[4] * a[2] * a[9] +
			a[8] * a[1] * a[6] -
			a[8] * a[2] * a[5];

		det = a[0] * inv[0] + a[1] * inv[4] + a[2] * inv[8] + a[3] * inv[12];

		if (fabs(det) < 1e-6f)
			return mat4(1.0f); // fallback (singular matrix)

		det = 1.0f / det;

		mat4 result;
		for (int col = 0; col < 4; col++)
			for (int row = 0; row < 4; row++)
				result[col][row] = inv[col * 4 + row] * det;

		return result;
	}

	mat4 transpose(const mat4& m)
	{
		mat4 res;
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				res[i][j] = m[j][i];
			}
		}
		return res;
	}

	vec3 reflect(const vec3& incident, const vec3& normal)
	{
		return incident - 2.0f * dot(incident, normal) * normal;
	}

	mat4 scale(vec3 factor)
	{
		return mat4(factor.x, factor.y, factor.z, 1.0f);
	}
	mat4 rotate(Axis axis, float angle)
	{
		mat4 matrix(1.0f);
		float s = sin(angle);
		float c = cos(angle);
		switch (axis)
		{
		case X:
			matrix[1][1] = c;
			matrix[1][2] = s;
			matrix[2][1] = -s;
			matrix[2][2] = c;
			return matrix;
		case Y:
			matrix[0][0] = c;
			matrix[2][0] = s;
			matrix[0][2] = -s;
			matrix[2][2] = c;
			return matrix;
		case Z:
			matrix[0][0] = c;
			matrix[0][1] = s;
			matrix[1][0] = -s;
			matrix[1][1] = c;
		}
		return matrix;
	}
	mat4 translate(vec3 vector)
	{
		mat4 matrix(1.0f);
		matrix[3][0] = vector.x;
		matrix[3][1] = vector.y;
		matrix[3][2] = vector.z;
		return matrix;
	}

	//glm equivalent
	mat4 scale(mat4 model, vec3 factor)
	{
		return scale(factor) * model;
	}
	mat4 rotate(mat4 model, Axis axis, float angle)
	{
		return rotate(axis, angle) * model;
	}
	mat4 translate(mat4 model, vec3 vector)
	{
		return translate(vector) * model;
	}

	float radians(float degrees)
	{
		return degrees * 0.01745329251;
	}
	float degrees(float radians)
	{
		return radians * 57.2957795131;
	}

	float clip(float scalar, float lower, float upper)
	{
		if (scalar < lower)
			return lower;
		if (scalar > upper)
			return upper;
		return scalar;
	}
	vec2 clip(vec2 vector, float lower, float upper)
	{
		return vec2(clip(vector.x, lower, upper), clip(vector.y, lower, upper));
	}
	vec3 clip(vec3 vector, float lower, float upper)
	{
		return vec3(clip(vector.x, lower, upper), clip(vector.y, lower, upper), clip(vector.z, lower, upper));
	}
	vec4 clip(vec4 vector, float lower, float upper)
	{
		return vec4(clip(vector.x, lower, upper), clip(vector.y, lower, upper), clip(vector.z, lower, upper), clip(vector.w, lower, upper));
	}
}