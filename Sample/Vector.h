#pragma once
#include <vector>

template <class T>
std::vector<T> add(std::vector<T> a, std::vector<T> b) {
	std::vector<T> result(a.size());
	for (int i = 0; i < a.size(); i++) {
		result.at(i) = a.at(i) + b.at(i);
	}

	return result;
}

template <class T>
std::vector<std::vector<T>> multi(std::vector<std::vector<T>> matrix1, std::vector<std::vector<T>> matrix2) {
	std::vector<std::vector<T>> result;

	int sizeY1 = matrix1.size();
	int sizeX1 = matrix1[0].size();
	int sizeY2 = matrix2.size();
	int sizeX2 = matrix2[0].size();

	if (sizeX1 != sizeY2)
		return result;

	result.resize(sizeY1, std::vector<T>(sizeX2));
	for (int i = 0; i < sizeY1; i++) {
		for (int j = 0; j < sizeX2; j++) {
			float buf = 0;
			for (int k = 0; k < sizeX1; k++) {
				buf += matrix1[i][k] * matrix2[k][j];
			}
			result[i][j] = buf;
		}
	}

	return result;
}

template <class T>
std::vector<std::vector<T>> inverse(std::vector<std::vector<T>> matrix) {
	int n = matrix.size();
	float buf;
	int i, j, k;
	std::vector<std::vector<T>> a(n, std::vector<T>(n));
	std::vector<std::vector<T>> inv(n, std::vector<T>(n));

	//単位行列を作る
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			a[i][j] = matrix[i][j];
			inv[i][j] = (i == j) ? 1.0 : 0.0;
		}
	}
	//掃き出し法
	for (i = 0; i < n; i++) {
		if (a[i][i] != 0) {
			buf = 1 / a[i][i];
			for (j = 0; j < n; j++) {
				a[i][j] *= buf;
				inv[i][j] *= buf;
			}
		}
		for (j = 0; j < n; j++) {
			if (i != j) {
				buf = a[j][i];
				for (k = 0; k < n; k++) {
					a[j][k] -= a[i][k] * buf;
					inv[j][k] -= inv[i][k] * buf;
				}
			}
		}
	}

	std::vector<std::vector<T>> result = {
		{-0.003382, 0.003382, -0.000642, 0.000642, -0.008558, 0.008558, -0.006235, 0.006235},
		{-0.007352, 0.001470, -0.000546, 0.006428, -0.001678, 0.001678, -0.001222, 0.001222},
		{1.691176, -0.338235, 0.032786, -0.385728, 0.835986, -0.835986, 0.609097, -0.609097},
		{0,        0,         0,        0,         -0.003823, 0.003823, -0.000642, 0.000642},
		{-0.004264, 0.004264, -0.003107, 0.003107, -0.006073, 0.000191, 0.000385, 0.005496},
		{0.202941, -0.202941, 0.147862, -0.141786, 1.630294, -0.277352, -0.011571, -0.341369},
		{0,        0,         0,        0,        -0.000014, 0.000014, -0.000010, 0.000010},
		{-0.000014, 0.000014, -0.000010, 0.000010, 0.000004, -0.000004, 0.000003, -0.000003}
	};

	//return result;
	return inv;
}

