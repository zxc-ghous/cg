#pragma once
#include <cmath>
#include <utility>

struct vec
{
	float x;
	float y;
	float z;

	vec(float _x = 0, float _y = 0, float _z = 0);
	vec(const vec& other);
	void operator=(const vec& other);
	vec(vec&& other) noexcept;
	void operator=(vec&& other) noexcept;

	void normalize();

	float length() const;

	vec operator+(const vec& other);
	vec operator-(const vec& other);
	vec operator*(float coeff);

	void operator+=(const vec& other);
	void operator-=(const vec& other);
	void operator*=(float coeff);
};

struct matrix
{
	float** mat = nullptr;

	matrix();
	~matrix();
	matrix(const matrix& other);
	void operator=(const matrix& other);
	matrix(matrix&& other) noexcept;
	void operator=(matrix&& other) noexcept;

	void transpose();

	matrix operator+(const matrix& other);
	matrix operator-(const matrix& other);
	matrix operator*(const matrix& other);
	vec operator*(const vec& other);
	matrix operator*(float coeff);

	void operator+=(const matrix& other);
	void operator-=(const matrix& other);
	void operator*=(const matrix& other);
	void operator*=(float coeff);
};

vec::vec(float _x, float _y, float _z) :
	x(_x), y(_y), z(_z)
{
}

vec::vec(const vec& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
}

void vec::operator=(const vec& other)
{
	if (this == &other)
		return;

	x = other.x;
	y = other.y;
	z = other.z;
}

vec::vec(vec&& other) noexcept
{
	x = other.x;
	y = other.y;
	z = other.z;
}

void vec::operator=(vec&& other) noexcept
{
	if (this == &other)
		return;

	x = other.x;
	y = other.y;
	z = other.z;
}

void vec::normalize()
{
	float inv_len = 1.0f / this->length();
	(*this) *= inv_len;
}

float vec::length() const
{
	return sqrtf(x * x + y * y + z * z);
}

vec vec::operator+(const vec& other)
{
	vec vec(*this);
	vec += other;

	return vec;
}

vec vec::operator-(const vec& other)
{
	vec vec(*this);
	vec -= other;

	return vec;
}

vec vec::operator*(float coeff)
{
	vec vec(*this);
	vec *= coeff;

	return vec;
}

void vec::operator+=(const vec& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
}

void vec::operator-=(const vec& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
}

void vec::operator*=(float coeff)
{
	x *= coeff;
	y *= coeff;
	z *= coeff;
}

matrix::matrix()
{
	mat = new float* [3];
	mat[0] = new float[3];
	mat[1] = new float[3];
	mat[2] = new float[3];

	mat[0][0] = 1; mat[0][1] = 0; mat[0][2] = 0;
	mat[1][0] = 0; mat[1][1] = 1; mat[1][2] = 0;
	mat[2][0] = 0; mat[2][1] = 0; mat[2][2] = 1;
}

matrix::~matrix()
{
	delete[] mat[0];
	delete[] mat[1];
	delete[] mat[2];
	delete[] mat;
}

matrix::matrix(const matrix& other)
{
	mat = new float* [3];
	mat[0] = new float[3];
	mat[1] = new float[3];
	mat[2] = new float[3];

	mat[0][0] = other.mat[0][0]; mat[0][1] = other.mat[0][1]; mat[0][2] = other.mat[0][2];
	mat[1][0] = other.mat[1][0]; mat[1][1] = other.mat[1][1]; mat[1][2] = other.mat[1][2];
	mat[2][0] = other.mat[2][0]; mat[2][1] = other.mat[2][1]; mat[2][2] = other.mat[2][2];
}

void matrix::operator=(const matrix& other)
{
	if (this == &other)
		return;

	mat[0][0] = other.mat[0][0]; mat[0][1] = other.mat[0][1]; mat[0][2] = other.mat[0][2];
	mat[1][0] = other.mat[1][0]; mat[1][1] = other.mat[1][1]; mat[1][2] = other.mat[1][2];
	mat[2][0] = other.mat[2][0]; mat[2][1] = other.mat[2][1]; mat[2][2] = other.mat[2][2];
}

matrix::matrix(matrix&& other) noexcept
{
	mat = other.mat;
	other.mat = nullptr;
}

void matrix::operator=(matrix&& other) noexcept
{
	float** temp = mat;
	mat = other.mat;
	other.mat = temp;
}

void matrix::transpose()
{
	matrix temp(*this);
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			mat[i][j] = temp.mat[j][i];
}

matrix matrix::operator+(const matrix& other)
{
	matrix new_mat(*this);
	new_mat += other;

	return new_mat;
}

matrix matrix::operator-(const matrix& other)
{
	matrix new_mat(*this);
	new_mat -= other;

	return new_mat;
}

matrix matrix::operator*(const matrix& other)
{
	matrix new_mat(*this);
	new_mat += other;

	return new_mat;
}

vec matrix::operator*(const vec& other)
{
	vec ans;

	ans.x = mat[0][0] * other.x + mat[0][1] * other.y + mat[0][2] * other.z;
	ans.y = mat[1][0] * other.x + mat[1][1] * other.y + mat[1][2] * other.z;
	ans.z = mat[2][0] * other.x + mat[2][1] * other.y + mat[2][2] * other.z;

	return ans;
}

matrix matrix::operator*(float coeff)
{
	matrix new_mat(*this);
	new_mat *= coeff;

	return new_mat;
}

void matrix::operator+=(const matrix& other)
{
	mat[0][0] += other.mat[0][0]; mat[0][1] += other.mat[0][1]; mat[0][2] += other.mat[0][2];
	mat[1][0] += other.mat[1][0]; mat[1][1] += other.mat[1][1]; mat[1][2] += other.mat[1][2];
	mat[2][0] += other.mat[2][0]; mat[2][1] += other.mat[2][1]; mat[2][2] += other.mat[2][2];
}

void matrix::operator-=(const matrix& other)
{
	mat[0][0] -= other.mat[0][0]; mat[0][1] -= other.mat[0][1]; mat[0][2] -= other.mat[0][2];
	mat[1][0] -= other.mat[1][0]; mat[1][1] -= other.mat[1][1]; mat[1][2] -= other.mat[1][2];
	mat[2][0] -= other.mat[2][0]; mat[2][1] -= other.mat[2][1]; mat[2][2] -= other.mat[2][2];
}

void matrix::operator*=(const matrix& other)
{
	matrix new_mat(*this);

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			new_mat.mat[i][j] = 0;
			for (int k = 0; k < 3; k++)
				new_mat.mat[i][j] += mat[i][k] * other.mat[k][j];
		}

	(*this) = std::move(new_mat);
}

void matrix::operator*=(float coeff)
{
	mat[0][0] *= coeff; mat[0][1] *= coeff; mat[0][2] *= coeff;
	mat[1][0] *= coeff; mat[1][1] *= coeff; mat[1][2] *= coeff;
	mat[2][0] *= coeff; mat[2][1] *= coeff; mat[2][2] *= coeff;
}
