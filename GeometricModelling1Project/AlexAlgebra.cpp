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

	float distance_2(vec2 a, vec2 b)
	{
		vec2 path = b - a;
		return dot(path, path);
	}
	float distance_2(vec3 a, vec3 b)
	{
		vec3 path = b - a;
		return dot(path, path);
	}
	float distance_2(vec4 a, vec4 b)
	{
		vec4 path = b - a;
		return dot(path, path);
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
	vec3 bezier(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float t)
	{
		// De Casteljau
		p0 = (p0 * (1.0f - t) + p1 * t);
		p1 = (p1 * (1.0f - t) + p2 * t);
		p2 = (p2 * (1.0f - t) + p3 * t);

		p0 = (p0 * (1.0f - t) + p1 * t);
		p1 = (p1 * (1.0f - t) + p2 * t);

		return (p0 * (1.0f - t) + p1 * t);
	}
	vec3 bezier_derivative(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float t)
	{
		p0 = p1 - p0;
		p1 = p2 - p1;
		p2 = p3 - p2;

		// De Casteljau
		p0 = (p0 * (1.0f - t) + p1 * t);
		p1 = (p1 * (1.0f - t) + p2 * t);

		return 3.0f * (p0 * (1.0f - t) + p1 * t);
	}
	aa::vec3 aa::bspline(
		const aa::vec3 p0,
		const aa::vec3 p1,
		const aa::vec3 p2,
		const aa::vec3 p3,
		float t)
	{
		float t2 = t * t;
		float t3 = t2 * t;

		float b0 = (-t3 + 3.0f * t2 - 3.0f * t + 1.0f) / 6.0f;
		float b1 = (3.0f * t3 - 6.0f * t2 + 4.0f) / 6.0f;
		float b2 = (-3.0f * t3 + 3.0f * t2 + 3.0f * t + 1.0f) / 6.0f;
		float b3 = (t3) / 6.0f;

		return
			b0 * p0 +
			b1 * p1 +
			b2 * p2 +
			b3 * p3;
	}
	aa::vec3 aa::bspline_derivative(
		const aa::vec3 p0,
		const aa::vec3 p1,
		const aa::vec3 p2,
		const aa::vec3 p3,
		float t)
	{
		float t2 = t * t;

		float db0 = (-3.0f * t2 + 6.0f * t - 3.0f) / 6.0f;
		float db1 = (9.0f * t2 - 12.0f * t) / 6.0f;
		float db2 = (-9.0f * t2 + 6.0f * t + 3.0f) / 6.0f;
		float db3 = (3.0f * t2) / 6.0f;

		return
			db0 * p0 +
			db1 * p1 +
			db2 * p2 +
			db3 * p3;
	}

	aa::vec4 aa::Mat3ToQuat(const aa::mat4& m)
	{
		aa::vec4 q;

		// 1. Extract and normalize columns to strip away the Scale factor
		aa::vec3 col0(m[0][0], m[0][1], m[0][2]);
		aa::vec3 col1(m[1][0], m[1][1], m[1][2]);
		aa::vec3 col2(m[2][0], m[2][1], m[2][2]);

		// Perform vector length normalization (assuming you have a length/normalize helper)
		float len0 = sqrt(col0.x * col0.x + col0.y * col0.y + col0.z * col0.z);
		float len1 = sqrt(col1.x * col1.x + col1.y * col1.y + col1.z * col1.z);
		float len2 = sqrt(col2.x * col2.x + col2.y * col2.y + col2.z * col2.z);

		if (len0 > 1e-6f) { col0.x /= len0; col0.y /= len0; col0.z /= len0; }
		if (len1 > 1e-6f) { col1.x /= len1; col1.y /= len1; col1.z /= len1; }
		if (len2 > 1e-6f) { col2.x /= len2; col2.y /= len2; col2.z /= len2; }

		// 2. Compute trace using the pure, unscaled rotation components
		float trace = col0.x + col1.y + col2.z;

		if (trace > 0.0f)
		{
			float s = sqrt(trace + 1.0f) * 2.0f;
			q.w = 0.25f * s;
			// Corrected signs for Column-Major matrix (m[col][row])
			q.x = (col1.z - col2.y) / s; // (m[1][2] - m[2][1])
			q.y = (col2.x - col0.z) / s; // (m[2][0] - m[0][2])
			q.z = (col0.y - col1.x) / s; // (m[0][1] - m[1][0])
		}
		else
		{
			if (col0.x > col1.y && col0.x > col2.z)
			{
				float s = sqrt(1.0f + col0.x - col1.y - col2.z) * 2.0f;
				q.w = (col1.z - col2.y) / s;
				q.x = 0.25f * s;
				q.y = (col0.y + col1.x) / s;
				q.z = (col2.x + col0.z) / s;
			}
			else if (col1.y > col2.z)
			{
				float s = sqrt(1.0f + col1.y - col0.x - col2.z) * 2.0f;
				q.w = (col2.x - col0.z) / s;
				q.x = (col0.y + col1.x) / s;
				q.y = 0.25f * s;
				q.z = (col1.z + col2.y) / s;
			}
			else
			{
				float s = sqrt(1.0f + col2.z - col0.x - col1.y) * 2.0f;
				q.w = (col0.y - col1.x) / s;
				q.x = (col2.x + col0.z) / s;
				q.y = (col1.z + col2.y) / s;
				q.z = 0.25f * s;
			}
		}

		return q;
	}

	aa::mat4 QuatToMat4(const aa::vec4& q)
	{
		aa::mat4 m(1.0f);
		m[0][3] = 0.0f; m[1][3] = 0.0f; m[2][3] = 0.0f; m[3][3] = 1.0f;
		m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f;

		// 1. Calculate the magnitude to normalize the quaternion safely
		float n = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;

		// If it's already zero or completely invalid, return an identity matrix
		if (n < 1e-6f) {
			return m;
		}

		// Normalize components
		float invLen = 1.0f / sqrt(n);
		float w = q.w * invLen;
		float x = q.x * invLen;
		float y = q.y * invLen;
		float z = q.z * invLen;

		float xx = q.x * q.x;
		float yy = q.y * q.y;
		float zz = q.z * q.z;
		float xy = q.x * q.y;
		float xz = q.x * q.z;
		float yz = q.y * q.z;
		float wx = q.w * q.x;
		float wy = q.w * q.y;
		float wz = q.w * q.z;

		m[0][0] = 1.0f - 2.0f * (yy + zz);
		m[0][1] = 2.0f * (xy - wz);
		m[0][2] = 2.0f * (xz + wy);

		m[1][0] = 2.0f * (xy + wz);
		m[1][1] = 1.0f - 2.0f * (xx + zz);
		m[1][2] = 2.0f * (yz - wx);

		m[2][0] = 2.0f * (xz - wy);
		m[2][1] = 2.0f * (yz + wx);
		m[2][2] = 1.0f - 2.0f * (xx + yy);

		return m;
	}

	aa::vec4 Multiply(const aa::vec4& a, const aa::vec4& b)
	{
		aa::vec4 r;

		r.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
		r.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
		r.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
		r.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;

		return r;
	}
}