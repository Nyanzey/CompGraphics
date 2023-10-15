#pragma once

#include <math.h>

#define PI 3.14159265

class mat4;
bool inverse(mat4 M, mat4& inv);

class vec4
{
public:
	float x, y, z, w;

	vec4()
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
		this->w = 0;

	}

	vec4(float _x, float _y, float _z, float _w)
	{
		this->x = _x;
		this->y = _y;
		this->z = _z;
		this->w = _w;

	}

	void update(float _x, float _y, float _z, float _w)
	{
		this->x = _x;
		this->y = _y;
		this->z = _z;
		this->w = _w;

	}

	vec4 operator+(const vec4& a)
	{
		vec4 res(this->x + a.x, this->y + a.y, this->z + a.z, this->w);
		return res;
	}

	vec4 operator-(const vec4& a)
	{
		vec4 res(this->x - a.x, this->y - a.y, this->z - a.z, this->w);
		return res;
	}

	vec4 operator/(float val)
	{
		vec4 res(this-> x / val, this->y / val, this->z / val, this->w);
		return res;
	}

	vec4 operator*(float val)
	{
		vec4 res(this->x * val, this->y * val, this->z * val, this->w);
		return res;
	}

	float operator*(const vec4& a)
	{
		return x * a.x + y * a.y + z * a.z + w * a.w;
	}

	void operator=(const vec4& a)
	{
		this->x = a.x;
		this->y = a.y;
		this->z = a.z;
		this->w = a.w;
	}

	vec4 cross_product(vec4 vec)
	{
		return vec4(
			(y * vec.z) - (z * vec.y),
			(z * vec.x) - (x * vec.z),
			(x * vec.y) - (y * vec.x), 1.0f);
	}

	float get_norm()
	{
		return sqrt(x * x + y * y + z * z);
	}

	void print()
	{
		std::cout << "(" << x << " " << y << " " << z << " " << w << ")" << std::endl;
	}

};

class mat4
{
public:

	vec4 mat[4];

	mat4()
	{
		mat[0].x = 1;
		mat[1].y = 1;
		mat[2].z = 1;
		mat[3].w = 1;
	}

	void fill(float val)
	{
		mat[0].update(val, val, val, val);
		mat[1].update(val, val, val, val);
		mat[2].update(val, val, val, val);
		mat[3].update(val, val, val, val);
	}

	std::vector<std::vector<float>> mat_to_raw()
	{
		std::vector<std::vector<float>> res;
		
		res.push_back(std::vector<float>());
		res[0].push_back(mat[0].x);
		res[0].push_back(mat[0].y);
		res[0].push_back(mat[0].z);
		res[0].push_back(mat[0].w);

		res.push_back(std::vector<float>());
		res[1].push_back(mat[1].x);
		res[1].push_back(mat[1].y);
		res[1].push_back(mat[1].z);
		res[1].push_back(mat[1].w);

		res.push_back(std::vector<float>());
		res[2].push_back(mat[2].x);
		res[2].push_back(mat[2].y);
		res[2].push_back(mat[2].z);
		res[2].push_back(mat[2].w);

		res.push_back(std::vector<float>());
		res[3].push_back(mat[3].x);
		res[3].push_back(mat[3].y);
		res[3].push_back(mat[3].z);
		res[3].push_back(mat[3].w);

		return res;
	}

	mat4 raw_to_mat(std::vector<std::vector<float>> raw)
	{
		mat4 res;

		res.mat[0].x = raw[0][0];
		res.mat[0].y = raw[0][1];
		res.mat[0].z = raw[0][2];
		res.mat[0].w = raw[0][3];

		res.mat[1].x = raw[1][0];
		res.mat[1].y = raw[1][1];
		res.mat[1].z = raw[1][2];
		res.mat[1].w = raw[1][3];

		res.mat[2].x = raw[2][0];
		res.mat[2].y = raw[2][1];
		res.mat[2].z = raw[2][2];
		res.mat[2].w = raw[2][3];

		res.mat[3].x = raw[3][0];
		res.mat[3].y = raw[3][1];
		res.mat[3].z = raw[3][2];
		res.mat[3].w = raw[3][3];

		return res;
	}

	//Note: each vector represents a COLUMN of the matrix NOT ROWS
	void set_RZ(float angle)
	{
		mat[0].x = cos(angle * (PI / 180.0));
		mat[0].y = -sin(angle * (PI / 180.0));

		mat[1].x = sin(angle * (PI / 180.0));
		mat[1].y = cos(angle * (PI / 180.0));
	}

	void set_RX(float angle)
	{
		mat[1].y = cos(angle * (PI / 180.0));
		mat[1].z = -sin(angle * (PI / 180.0));

		mat[2].y = sin(angle * (PI / 180.0));
		mat[2].z = cos(angle * (PI / 180.0));
	}

	void set_RY(float angle)
	{
		mat[0].x = cos(angle * (PI / 180.0));
		mat[0].z = sin(angle * (PI / 180.0));

		mat[2].x = -sin(angle * (PI / 180.0));
		mat[2].z = cos(angle * (PI / 180.0));
	}

	//Note: each vector represents a ROW of the matrix NOT COLUMNS (different for this because its matrix*v not v*matrix)
	void set_T(vec4 vec)
	{
		mat[0].w = vec.x;
		mat[1].w = vec.y;
		mat[2].w = vec.z;
	}

	void set_S(float sx, float sy, float sz)
	{
		mat[0].x = sx;
		mat[1].y = sy;
		mat[2].z = sz;
	}

	void print()
	{
		std::cout << "Matrix: " << std::endl;
		for (int i = 0; i < 4; i++)
		{
			mat[i].print();
		}
	}

	mat4 multiply(mat4 M)
	{
		int x, i, j;
		mat4 res;

		std::vector<std::vector<float>> rslt = res.mat_to_raw();
		std::vector<std::vector<float>> mat1 = this->mat_to_raw();
		std::vector<std::vector<float>> mat2 = M.mat_to_raw();

		for (int i = 0; i < 4; i++)//Rows of mat1
		{
			for (int j = 0; j < 4; j++)//Cols of mat1
			{
				rslt[i][j] = 0;
				for (int k = 0; k < 4; k++)//Rows of mat2
				{
					rslt[i][j] += mat1[i][k] * mat2[k][j];
				}
			}
		}

		return raw_to_mat(rslt);
	}

	mat4 transpose()
	{
		mat4 res;
		res.fill(0.0f);

		std::vector<std::vector<float>> res_raw = res.mat_to_raw();
		std::vector<std::vector<float>> orig_raw = this->mat_to_raw();

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				res_raw[i][j] = orig_raw[j][i];

		return raw_to_mat(res_raw);
	}

	mat4 operator*(const mat4& M)
	{
		return this->multiply(M);
	}

	void operator=(const mat4& M)
	{
		mat[0] = M.mat[0];
		mat[1] = M.mat[1];
		mat[2] = M.mat[2];
		mat[3] = M.mat[3];
	}

	mat4 inv()
	{
		mat4 res;
		inverse(*this, res);

		return res;
	}

};

void apply_mat(vec4& vec, mat4& matrix)
{
	vec4 res;

	res.x = vec * matrix.mat[0];
	res.y = vec * matrix.mat[1];
	res.z = vec * matrix.mat[2];
	res.w = vec * matrix.mat[3];

	vec = res;
}

//Matrix operations

void getCfactor(mat4 &M, mat4& t, int p, int q, int n)
{
	std::vector<std::vector<float>> bufM = M.mat_to_raw();
	std::vector<std::vector<float>> buft = t.mat_to_raw();

	int i = 0, j = 0;
	for (int r = 0; r < n; r++) 
	{
		for (int c = 0; c < n; c++)
		{
			if (r != p && c != q)
			{
				buft[i][j++] = bufM[r][c];
				if (j == n - 1) 
				{
					j = 0; i++;
				}
			}
		} 	
	}

	M = M.raw_to_mat(bufM);
	t = t.raw_to_mat(buft);
}

float det(mat4 M, int n)
{
	float D = 0;
	if (n == 1)
	{
		return M.mat[0].x;
	}

	std::vector<std::vector<float>> bufM = M.mat_to_raw();

	mat4 t;
	t.fill(0);

	int s = 1; 
	for (int f = 0; f < n; f++) {
		getCfactor(M, t, 0, f, n);
		D += s * bufM[0][f] * det(t, n - 1);
		s = -s;
	}
	return D;
}

void adj(mat4 M, mat4 &adj)
{
	int s = 1;

	mat4 t;
	t.fill(0);

	std::vector<std::vector<float>> bufAdj = adj.mat_to_raw();

	for (int i = 0; i < 4; i++) 
	{
		for (int j = 0; j < 4; j++) 
		{
			getCfactor(M, t, i, j, 4);
			s = ((i + j) % 2 == 0) ? 1 : -1;
			bufAdj[j][i] = (s) * (det(t, 4 - 1));
		}
	}

	adj = adj.raw_to_mat(bufAdj);
}

bool inverse(mat4 M, mat4 &inv)
{
	float determinant = det(M, 4);
	if (determinant == 0)
	{
		std::cout << "can't find its inverse";
		return false;
	}

	mat4 adjunt;
	adjunt.fill(0);
	adj(M, adjunt);

	std::vector<std::vector<float>> bufInv = inv.mat_to_raw();
	std::vector<std::vector<float>> bufAdj = adjunt.mat_to_raw();

	for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) bufInv[i][j] = bufAdj[i][j] / float(determinant);

	inv = inv.raw_to_mat(bufInv);

	return true;
}
