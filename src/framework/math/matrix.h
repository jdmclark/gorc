#pragma once

#include "vector.h"
#include <array>
#include <algorithm>
#include <iostream>

namespace Gorc {
namespace Math {

template <typename F = float> class Matrix {
private:
	static constexpr F deg2rad = 0.0174532925;
	F data[4][4];

public:
	Matrix() {
		return;
	}

	Matrix(F const* gl_matrix) {
		std::copy(gl_matrix, gl_matrix + 16, data);
	}

	F* GetOpenGLMatrix() {
		return &data[0][0];
	}

	const F* GetOpenGLMatrix() const {
		return &data[0][0];
	}

	static Matrix MakeIdentityMatrix() {
		Matrix rv;
		for(int i = 0; i < 4; ++i) {
			for(int j = 0; j < 4; ++j) {
				rv.data[i][j] = (i == j) ? 1 : 0;
			}
		}

		return rv;
	}

	static Matrix MakeFrustumMatrix(float left, float right, float bottom, float top, float znear, float zfar) {
		Matrix rv;

		float temp, temp2, temp3, temp4;
		temp = 2.0f * znear;
		temp2 = right - left;
		temp3 = top - bottom;
		temp4 = zfar - znear;

		rv.data[0][0] = temp / temp2;
		rv.data[0][1] = 0;
		rv.data[0][2] = 0;
		rv.data[0][3] = 0;

		rv.data[1][0] = 0;
		rv.data[1][1] = temp / temp3;
		rv.data[1][2] = 0;
		rv.data[1][3] = 0;

		rv.data[2][0] = (right + left) / temp2;
		rv.data[2][1] = (top + bottom) / temp3;
		rv.data[2][2] = (-zfar - znear) / temp4;
		rv.data[2][3] = -1.0;

		rv.data[3][0] = 0;
		rv.data[3][1] = 0;
		rv.data[3][2] = (-temp * zfar) / temp4;
		rv.data[3][3] = 0;

		return rv;
	}

	static Matrix MakePerspectiveMatrix(float fovyInDegrees, float aspectRatio, float znear, float zfar) {
		float ymax = znear * std::tan(fovyInDegrees * deg2rad * 0.5);
		float xmax = ymax * aspectRatio;
		return MakeFrustumMatrix(-xmax, xmax, -ymax, ymax, znear, zfar);
	}

	static Matrix MakeTranslationMatrix(const Vector<3, F>& translation) {
		Matrix rv;

		for(int i = 0; i < 4; ++i) {
			for(int j = 0; j < 4; ++j) {
				rv.data[i][j] = (i == j) ? 1 : 0;
			}
		}

		rv.data[3][0] = Get<0>(translation);
		rv.data[3][1] = Get<1>(translation);
		rv.data[3][2] = Get<2>(translation);

		return rv;
	}

	static Matrix MakeLookMatrix(const Vector<3, F>& camera_position, const Vector<3, F>& camera_look, const Vector<3, F>& old_camera_up) {
		auto camera_side = Cross(camera_look, old_camera_up);
		auto camera_up = Cross(camera_side, camera_look);

		Matrix rv;

		rv.data[0][0] = Get<0>(camera_side);
		rv.data[0][1] = Get<0>(camera_up);
		rv.data[0][2] = -Get<0>(camera_look);
		rv.data[0][3] = 0;

		rv.data[1][0] = Get<1>(camera_side);
		rv.data[1][1] = Get<1>(camera_up);
		rv.data[1][2] = -Get<1>(camera_look);
		rv.data[1][3] = 0;

		rv.data[2][0] = Get<2>(camera_side);
		rv.data[2][1] = Get<2>(camera_up);
		rv.data[2][2] = -Get<2>(camera_look);
		rv.data[2][3] = 0;

		rv.data[3][0] = 0;
		rv.data[3][1] = 0;
		rv.data[3][2] = 0;
		rv.data[3][3] = 1;

		return rv * MakeTranslationMatrix(-camera_position);
	}

	static Matrix MakeRotationMatrix(F angle, const Math::Vector<3, F>& axis) {
		Matrix rv;

		F ax = Get<0>(axis);
		F ay = Get<1>(axis);
		F az = Get<2>(axis);

		F cost = std::cos(angle * deg2rad);
		F sint = std::sin(angle * deg2rad);
		F icost = 1 - cost;

		rv.data[0][0] = cost + ax * ax * icost;
		rv.data[0][1] = ax * ay * icost + az * sint;
		rv.data[0][2] = ax * az * icost - ay * sint;
		rv.data[0][3] = 0;

		rv.data[1][0] = ax * ay * icost - az * sint;
		rv.data[1][1] = cost + ay * ay * icost;
		rv.data[1][2] = ay * az * icost + ax * sint;
		rv.data[1][3] = 0;

		rv.data[2][0] = ax * az * icost + ay * sint;
		rv.data[2][1] = ay * az * icost - ax * sint;
		rv.data[2][2] = cost + az * az * icost;
		rv.data[2][3] = 0;

		rv.data[3][0] = 0;
		rv.data[3][1] = 0;
		rv.data[3][2] = 0;
		rv.data[3][3] = 1;

		return rv;
	}

	Matrix operator*(const Matrix& m) const {
		Matrix rv;

		for(size_t j = 0; j < 4; ++j) {
			for(size_t i = 0; i < 4; ++i) {
				rv.data[j][i] = 0;
				for(size_t k = 0; k < 4; ++k) {
					rv.data[j][i] += data[k][i] * m.data[j][k];
				}
			}
		}

		return rv;
	}

	Matrix operator+(const Matrix& m) const {
		Matrix rv;

		for(size_t j = 0; j < 4; ++j) {
			for(size_t i = 0; i < 4; ++i) {
				rv.data[j][i] = data[j][i] + m.data[j][i];
			}
		}

		return rv;
	}

	Matrix Transpose() const {
		Matrix rv;

		for(size_t j = 0; j < 4; ++j) {
			for(size_t i = 0; i < 4; ++i) {
				rv.data[j][i] = data[i][j];
			}
		}

		return rv;
	}
};

}
}
