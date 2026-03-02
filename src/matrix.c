#include "../inc/matrix.h"

#include <stdio.h>
#include <math.h>

bool are_int32(const matrix* x, const matrix* y) {
	return (x->type == kint || x->type == kint32) && (y->type == kint || y->type == kint32);
}

bool are_float32(const matrix* x, const matrix* y) {
	return (x->type == kfloat ||  x->type == kfloat32) && (y->type == kfloat || y->type == kfloat32);
}
bool are_float64(const matrix* x, const matrix* y) {
	return (x->type == kfloat64 ||  x->type == kdouble) && (y->type == kfloat64 || y->type == kdouble);
}
bool are_int64(const matrix* x, const matrix* y) {
	return x->type == kint64 && y->type == kint64;
}

uint32_t matrix_compute_offset(const uint32_t strides[2], const uint32_t row, const uint32_t col) {
	return row * strides[0] + col * strides[1];
}

bool is_valid_matrix_pointer(const matrix* mat) {
	if (!mat) {
		return false;
	} else {
		return mat->data;
	}
}

matrix *matrix_make(data_type type, uint32_t shape[2]) {
	// Compute the size
	if (shape[0] == 0 || shape[1] == 0) {
		fprintf(stderr, "Error shape must be > 0.\n");
		exit(EXIT_FAILURE);
	}
	uint32_t size = shape[0] * shape[1];

	// Allocate the matrix
	matrix *mat = malloc(sizeof(matrix));
	if (!mat) {
		fprintf(stderr, "Error allocating matrix.\n");
		exit(EXIT_FAILURE);
	}

	// Allocate the data
	void *data = NULL;
	if (type == kint || type == kint32) {
		data = (int *)malloc(size * sizeof(int));
	} else if (type == kfloat || type == kfloat32) {
		data = (float *)malloc(size * sizeof(float));
	} else if (type == kdouble || type == kfloat64) {
		data = (double *)malloc(size * sizeof(double));
	} else if (type == kint64) {
		data = (int64_t*)malloc(size * sizeof(int64_t));
	} else {
		fprintf(stderr, "Error unknown data type.\n");
		exit(EXIT_FAILURE);
	}

	if (!data) {
		fprintf(stderr, "Error allocating matrix data.\n");
		exit(EXIT_FAILURE);
	}


	mat->own_data = true;
	mat->type = type;
	mat->size = size;
	mat->shape[0] = shape[0];
	mat->shape[1] = shape[1];
	mat->strides[0] = 1;
	mat->strides[1] = shape[0];
	mat->data = data;

	return mat;
}

void matrix_free(matrix *mat) {
	if (mat) {
		if (mat->data)
			free(mat->data);
		free(mat);
	}
}

void matrix_print(const matrix *mat) {
	if (!is_valid_matrix_pointer(mat)) {
		fprintf(stderr, "Error invalid matrix pointer.\n");
		exit(EXIT_FAILURE);
	}
	if(mat->type == knone) {
		fprintf(stderr, "Error unknown data type.\n");
		exit(EXIT_FAILURE);
	}
	uint32_t rows = mat->shape[0];
	uint32_t cols = mat->shape[1];
	printf("matrix of ");
	if (mat->type == kint || mat->type == kint32) {
		printf("int32");
	} else if (mat->type == kfloat || mat->type == kfloat32) {
		printf("float32");
	} else if (mat->type == kdouble || mat->type == kfloat64) {
		printf("float64");
	} else if (mat->type == kint64) {
		printf("int64");
	}
	printf("\n");
	for (uint32_t i = 0; i < rows; i++) {
		for (uint32_t j = 0; j < cols; j++) {
			uint32_t offset = matrix_compute_offset(mat->strides, i, j);
			if (mat->type == kint || mat->type == kint32) {
				int *casted_data = (int *)mat->data;
				printf("%i", casted_data[offset]);
			} else if (mat->type == kfloat || mat->type == kfloat32) {
				float *casted_data = (float *)mat->data;
				printf("%f", casted_data[offset]);
			} else if (mat->type == kdouble || mat->type == kfloat64) {
				double *casted_data = (double *)mat->data;
				printf("%f", casted_data[offset]);
			} else if (mat->type == kint64) {
				int64_t *casted_data = (int64_t *)mat->data;
				printf("%li", casted_data[offset]);
			}
			if (j + 1 < cols) {
				printf(" ");
			}
		}
		printf("\n");
	}
}

uint32_t matrix_rows(const matrix* mat) {
	if (!is_valid_matrix_pointer(mat)) {
		fprintf(stderr, "Error cannot get rows of an invalid matrix pointer.\n");
		exit(EXIT_FAILURE);
	}
	return mat->shape[0];
}

uint32_t matrix_cols(const matrix* mat) {
	if (!is_valid_matrix_pointer(mat)) {
		fprintf(stderr, "Error cannot get columns of an invalid matrix pointer.\n");
		exit(EXIT_FAILURE);
	}
	return mat->shape[1];
}

data_type matrix_type(const matrix* mat) {
	if (!is_valid_matrix_pointer(mat)) {
		fprintf(stderr, "Error cannot get type of an invalid matrix pointer.\n");
		exit(EXIT_FAILURE);
	}
	return mat->type;
}


void matrix_get(const matrix* mat, uint32_t row, uint32_t col, void* value) {
	if (!is_valid_matrix_pointer(mat)) {
		fprintf(stderr, "Error cannot get value from an ivalid matrix pointer.\n");
		exit(EXIT_FAILURE);
	}
	uint32_t offset = matrix_compute_offset(mat->strides, row, col);
	if (offset >= mat->size) {
		fprintf(stderr, "Index out of range.\n");
		exit(EXIT_FAILURE);
	}
	if (mat->type == kint || mat->type == kint32) {
		int* data = (int *)mat->data;
		int* value_int = (int*) value;
		*value_int = data[offset];
	} else if (mat->type == kfloat || mat->type == kfloat32) {
		float *data = (float *)mat->data;
		float* value_float = (float*) value;
		*value_float = data[offset];
	} else if (mat->type == kdouble || mat->type == kfloat64) {
		double* data = (double *)mat->data;
		double* value_double = (double*) value;
		*value_double = data[offset];
	} else if (mat->type == kint64) {
		int64_t* data = (int64_t *)mat->data;
		int64_t* value_int = (int64_t*) value;
		*value_int = data[offset];
	} else {
		fprintf(stderr, "Unknown data type.\n");
		exit(EXIT_FAILURE);
	}
}

void matrix_at(const matrix* mat, uint32_t index, void* value) {
	if (!is_valid_matrix_pointer(mat)) {
		fprintf(stderr, "Error cannot get value from an invalid matrix pointer.\n");
		exit(EXIT_FAILURE);
	}
	if (index >= mat->size) {
		fprintf(stderr, "Index out %i of range [0, %i].\n", index, mat->size);
		exit(EXIT_FAILURE);
	}
	if (mat->type == kint || mat->type == kint32) {
		int* data = (int *)mat->data;
		int* value_int = (int*) value;
		*value_int = data[index];
	} else if (mat->type == kfloat || mat->type == kfloat32) {
		float *data = (float *)mat->data;
		float* value_float = (float*) value;
		*value_float = data[index];
	} else if (mat->type == kdouble || mat->type == kfloat64) {
		double* data = (double *)mat->data;
		double* value_double = (double*) value;
		*value_double = data[index];
	} else if (mat->type == kint64) {
		int64_t* data = (int64_t *)mat->data;
		int64_t* value_int = (int64_t*) value;
		*value_int = data[index];
	} else {
		fprintf(stderr, "Unknown data type.\n");
		exit(EXIT_FAILURE);
	}
}

void matrix_set(matrix* mat, uint32_t row, uint32_t col, void* value) {
	if (!is_valid_matrix_pointer(mat)) {
		fprintf(stderr, "Error cannot get value from an invalid matrix pointer.\n");
		exit(EXIT_FAILURE);
	}
	uint32_t offset = matrix_compute_offset(mat->strides, row, col);
	if (offset >= mat->size) {
		fprintf(stderr, "Index [%i, %i] out of range.\n", row, col);
	}
	if (mat->type == kint || mat->type == kint32) {
		int *ptr = (int *)mat->data;
		ptr[offset] = *(int *)value;
	} else if (mat->type == kfloat || mat->type == kfloat32) {
		float *ptr = (float *)mat->data;
		ptr[offset] = *(float *)value;
	} else if (mat->type == kdouble || mat->type == kfloat64) {
		double *ptr = (double *)mat->data;
		ptr[offset] = *(double *)value;
	} else if (mat->type == kint64) {
		int64_t *ptr = (int64_t *)mat->data;
		ptr[offset] = *(int64_t *)value;
	}
}

void matrix_set_at(matrix* mat, uint32_t index, void* value) {
	if (!is_valid_matrix_pointer(mat)) {
		fprintf(stderr, "Error invalid matrix pointer.\n");
		exit(EXIT_FAILURE);
	}
	if (index >= mat->size) {
		fprintf(stderr, "Index %idx out of range 0-%i.\n", index, mat->size);
	}
	if (mat->type == kint || mat->type == kint32) {
		int *ptr = (int *)mat->data;
		ptr[index] = *(int *)value;
	} else if (mat->type == kfloat || mat->type == kfloat32) {
		float *ptr = (float *)mat->data;
		ptr[index] = *(float *)value;
	} else if (mat->type == kdouble || mat->type == kfloat64) {
		double *ptr = (double *)mat->data;
		ptr[index] = *(double *)value;
	} else if (mat->type == kint64) {
		int64_t *ptr = (int64_t *)mat->data;
		ptr[index] = *(int64_t *)value;
	} else {
		fprintf(stderr, "Error unknown data type.\n");
		exit(EXIT_FAILURE);
	}
}

void matrix_add(const matrix* x, const matrix* y, matrix* z) {
	if (!is_valid_matrix_pointer(x)) {
		fprintf(stderr, "Error x is an invalid matrix pointer.\n");
		exit(EXIT_FAILURE);
	}
	if (!is_valid_matrix_pointer(y)) {
		fprintf(stderr, "Error y is an invalid matrix pointer.\n");
		exit(EXIT_FAILURE);
	}
	if (!is_valid_matrix_pointer(z)) {
		fprintf(stderr, "Error z is an invalid matrix pointer.\n");
		exit(EXIT_FAILURE);
	}

	if (!(are_float32(x, y) || are_float64(x, y) || are_int32(x, y) || are_int64(x, y))) {
		fprintf(stderr, "Error add different types.");
		exit(EXIT_FAILURE);
	}
	if (!(are_float32(x, z) || are_float64(x, z) || are_int32(x, z) || are_int64(x, z))) {
		fprintf(stderr, "Error add different output type.");
		exit(EXIT_FAILURE);
	}

	if ((x->shape[0] != y->shape[0]) || (x->shape[1] != y->shape[1])) {
		fprintf(stderr, "Error different input shapes.");
		exit(EXIT_FAILURE);
	}
	if ((x->shape[0] != z->shape[0]) || (x->shape[1] != z->shape[1])) {
		fprintf(stderr, "Error different output shape.");
		exit(EXIT_FAILURE);
	}

	uint32_t size = x->size;
	data_type type = x->type;
	if (type == kint || type == kint32) {
		int* casted_x = (int*) x->data;
		int* casted_y = (int*) y->data;
		int* casted_z = (int*) z->data;
		#pragma clang loop vectorize(enable)
		for (uint32_t i = 0; i < size; i++) {
			casted_z[i] = casted_x[i] + casted_y[i];
		}
	} else if (type == kfloat || type == kfloat32) {
		float* casted_x = (float*) x->data;
		float* casted_y = (float*) y->data;
		float* casted_z = (float*) z->data;
		#pragma clang loop vectorize(enable)
		for (uint32_t i = 0; i < size; i++) {
			casted_z[i] = casted_x[i] + casted_y[i];
		}
	} else if (type == kdouble || type == kfloat64) {
		double* casted_x = (double*) x->data;
		double* casted_y = (double*) y->data;
		double* casted_z = (double*) z->data;
		#pragma clang loop vectorize(enable)
		for (uint32_t i = 0; i < size; i++) {
			casted_z[i] = casted_x[i] + casted_y[i];
		}
	} else if (type == kint64) {
		int64_t* casted_x = (int64_t*) x->data;
		int64_t* casted_y = (int64_t*) y->data;
		int64_t* casted_z = (int64_t*) z->data;
		#pragma clang loop vectorize(enable)
		for (uint32_t i = 0; i < size; i++) {
			casted_z[i] = casted_x[i] + casted_y[i];
		}
	}
}

void matrix_sub(const matrix* x, const matrix* y, matrix* z) {
	if (!is_valid_matrix_pointer(x)) {
		fprintf(stderr, "Error x is an invalid matrix pointer.\n");
		exit(EXIT_FAILURE);
	}
	if (!is_valid_matrix_pointer(y)) {
		fprintf(stderr, "Error y is an invalid matrix pointer.\n");
		exit(EXIT_FAILURE);
	}
	if (!is_valid_matrix_pointer(z)) {
		fprintf(stderr, "Error z is an invalid matrix pointer.\n");
		exit(EXIT_FAILURE);
	}

	if (!(are_float32(x, y) || are_float64(x, y) || are_int32(x, y) || are_int64(x, y))) {
		fprintf(stderr, "Error sub different types.");
		exit(EXIT_FAILURE);
	}
	if (!(are_float32(x, z) || are_float64(x, z) || are_int32(x, z) || are_int64(x, z))) {
		fprintf(stderr, "Error sub different output type.");
		exit(EXIT_FAILURE);
	}

	if ((x->shape[0] != y->shape[0]) || (x->shape[1] != y->shape[1])) {
		fprintf(stderr, "Error different input shapes.");
		exit(EXIT_FAILURE);
	}
	if ((x->shape[0] != z->shape[0]) || (x->shape[1] != z->shape[1])) {
		fprintf(stderr, "Error different output shape.");
		exit(EXIT_FAILURE);
	}

	uint32_t size = x->size;
	data_type type = x->type;
	if (type == kint || type == kint32) {
		int* casted_x = (int*) x->data;
		int* casted_y = (int*) y->data;
		int* casted_z = (int*) z->data;
		#pragma clang loop vectorize(enable)
		for (uint32_t i = 0; i < size; i++) {
			casted_z[i] = casted_x[i] - casted_y[i];
		}
	} else if (type == kfloat || type == kfloat32) {
		float* casted_x = (float*) x->data;
		float* casted_y = (float*) y->data;
		float* casted_z = (float*) z->data;
		#pragma clang loop vectorize(enable)
		for (uint32_t i = 0; i < size; i++) {
			casted_z[i] = casted_x[i] - casted_y[i];
		}
	} else if (type == kdouble || type == kfloat64) {
		double* casted_x = (double*) x->data;
		double* casted_y = (double*) y->data;
		double* casted_z = (double*) z->data;
		#pragma clang loop vectorize(enable)
		for (uint32_t i = 0; i < size; i++) {
			casted_z[i] = casted_x[i] - casted_y[i];
		}
	} else if (type == kint64) {
		int64_t* casted_x = (int64_t*) x->data;
		int64_t* casted_y = (int64_t*) y->data;
		int64_t* casted_z = (int64_t*) z->data;
		#pragma clang loop vectorize(enable)
		for (uint32_t i = 0; i < size; i++) {
			casted_z[i] = casted_x[i] - casted_y[i];
		}
	}
}

void matrix_mul(const matrix* x, const matrix* y, matrix* z) {
	if (!is_valid_matrix_pointer(x)) {
		fprintf(stderr, "Error x is an invalid matrix pointer.\n");
		exit(EXIT_FAILURE);
	}
	if (!is_valid_matrix_pointer(y)) {
		fprintf(stderr, "Error y is an invalid matrix pointer.\n");
		exit(EXIT_FAILURE);
	}
	if (!is_valid_matrix_pointer(z)) {
		fprintf(stderr, "Error z is an invalid matrix pointer.\n");
		exit(EXIT_FAILURE);
	}

	if (!(are_float32(x, y) || are_float64(x, y) || are_int32(x, y) || are_int64(x, y))) {
		fprintf(stderr, "Error mul different types.");
		exit(EXIT_FAILURE);
	}
	if (!(are_float32(x, z) || are_float64(x, z) || are_int32(x, z) || are_int64(x, z))) {
		fprintf(stderr, "Error mul different output type.");
		exit(EXIT_FAILURE);
	}

	if ((x->shape[0] != y->shape[0]) || (x->shape[1] != y->shape[1])) {
		fprintf(stderr, "Error different input shapes.");
		exit(EXIT_FAILURE);
	}
	if ((x->shape[0] != z->shape[0]) || (x->shape[1] != z->shape[1])) {
		fprintf(stderr, "Error different output shape.");
		exit(EXIT_FAILURE);
	}

	uint32_t size = x->size;
	data_type type = x->type;
	if (type == kint || type == kint32) {
		int* casted_x = (int*) x->data;
		int* casted_y = (int*) y->data;
		int* casted_z = (int*) z->data;
		#pragma clang loop vectorize(enable)
		for (uint32_t i = 0; i < size; i++) {
			casted_z[i] = casted_x[i] * casted_y[i];
		}
	} else if (type == kfloat || type == kfloat32) {
		float* casted_x = (float*) x->data;
		float* casted_y = (float*) y->data;
		float* casted_z = (float*) z->data;
		#pragma clang loop vectorize(enable)
		for (uint32_t i = 0; i < size; i++) {
			casted_z[i] = casted_x[i] * casted_y[i];
		}
	} else if (type == kdouble || type == kfloat64) {
		double* casted_x = (double*) x->data;
		double* casted_y = (double*) y->data;
		double* casted_z = (double*) z->data;
		#pragma clang loop vectorize(enable)
		for (uint32_t i = 0; i < size; i++) {
			casted_z[i] = casted_x[i] * casted_y[i];
		}
	} else if (type == kint64) {
		int64_t* casted_x = (int64_t*) x->data;
		int64_t* casted_y = (int64_t*) y->data;
		int64_t* casted_z = (int64_t*) z->data;
		#pragma clang loop vectorize(enable)
		for (uint32_t i = 0; i < size; i++) {
			casted_z[i] = casted_x[i] * casted_y[i];
		}
	}
}

void matrix_div(const matrix* x, const matrix* y, matrix* z) {
	if (!is_valid_matrix_pointer(x)) {
		fprintf(stderr, "Error x is an invalid matrix pointer.\n");
		exit(EXIT_FAILURE);
	}
	if (!is_valid_matrix_pointer(y)) {
		fprintf(stderr, "Error y is an invalid matrix pointer.\n");
		exit(EXIT_FAILURE);
	}
	if (!is_valid_matrix_pointer(z)) {
		fprintf(stderr, "Error z is an invalid matrix pointer.\n");
		exit(EXIT_FAILURE);
	}

	if (!(are_float32(x, y) || are_float64(x, y) || are_int32(x, y) || are_int64(x, y))) {
		fprintf(stderr, "Error div different types.");
		exit(EXIT_FAILURE);
	}
	if (!(are_float32(x, z) || are_float64(x, z) || are_int32(x, z) || are_int64(x, z))) {
		fprintf(stderr, "Error div different output type.");
		exit(EXIT_FAILURE);
	}

	if ((x->shape[0] != y->shape[0]) || (x->shape[1] != y->shape[1])) {
		fprintf(stderr, "Error different input shapes.");
		exit(EXIT_FAILURE);
	}
	if ((x->shape[0] != z->shape[0]) || (x->shape[1] != z->shape[1])) {
		fprintf(stderr, "Error different output shape.");
		exit(EXIT_FAILURE);
	}

	uint32_t size = x->size;
	data_type type = x->type;
	if (type == kint || type == kint32) {
		int* casted_x = (int*) x->data;
		int* casted_y = (int*) y->data;
		int* casted_z = (int*) z->data;
		#pragma clang loop vectorize(enable)
		for (uint32_t i = 0; i < size; i++) {
			casted_z[i] = casted_x[i] / casted_y[i];
		}
	} else if (type == kfloat || type == kfloat32) {
		float* casted_x = (float*) x->data;
		float* casted_y = (float*) y->data;
		float* casted_z = (float*) z->data;
		#pragma clang loop vectorize(enable)
		for (uint32_t i = 0; i < size; i++) {
			casted_z[i] = casted_x[i] / casted_y[i];
		}
	} else if (type == kdouble || type == kfloat64) {
		double* casted_x = (double*) x->data;
		double* casted_y = (double*) y->data;
		double* casted_z = (double*) z->data;
		#pragma clang loop vectorize(enable)
		for (uint32_t i = 0; i < size; i++) {
			casted_z[i] = casted_x[i] / casted_y[i];
		}
	} else if (type == kint64) {
		int64_t* casted_x = (int64_t*) x->data;
		int64_t* casted_y = (int64_t*) y->data;
		int64_t* casted_z = (int64_t*) z->data;
		#pragma clang loop vectorize(enable)
		for (uint32_t i = 0; i < size; i++) {
			casted_z[i] = casted_x[i] / casted_y[i];
		}
	}
}

void matrix_matmul(const matrix* x, const matrix* y, matrix* z) {
	    if (!is_valid_matrix_pointer(x)) {
        fprintf(stderr, "Error: x is an invalid matrix pointer.\n");
        exit(EXIT_FAILURE);
    }
    if (!is_valid_matrix_pointer(y)) {
        fprintf(stderr, "Error: y is an invalid matrix pointer.\n");
        exit(EXIT_FAILURE);
    }
    if (!is_valid_matrix_pointer(z)) {
        fprintf(stderr, "Error: z is an invalid matrix pointer.\n");
        exit(EXIT_FAILURE);
    }
    if (!(are_float32(x, y) || are_float64(x, y) ||
          are_int32(x, y)   || are_int64(x, y))) {
        fprintf(stderr, "Error: mul different types.\n");
        exit(EXIT_FAILURE);
    }
    if (!(are_float32(x, z) || are_float64(x, z) ||
          are_int32(x, z)   || are_int64(x, z))) {
        fprintf(stderr, "Error: mul different output type.\n");
        exit(EXIT_FAILURE);
    }
    uint32_t M = x->shape[0];
    uint32_t N = x->shape[1];
    uint32_t P = y->shape[1];
    if (y->shape[0] != N) {
        fprintf(stderr, "Error: input shape mismatch. x.cols != y.rows\n");
        exit(EXIT_FAILURE);
    }
    if (z->shape[0] != M || z->shape[1] != P) {
        fprintf(stderr, "Error: output matrix size mismatch.\n");
        exit(EXIT_FAILURE);
    }
    if (x->type == kfloat || x->type == kfloat32) {
        float * restrict A = (float*) x->data;
        float * restrict B = (float*) y->data;
        float * restrict C = (float*) z->data;
        for (uint32_t i = 0; i < M * P; i++)
            C[i] = 0.0f;
        const uint32_t BS = 64;
        for (uint32_t ii = 0; ii < M; ii += BS)
            for (uint32_t kk = 0; kk < N; kk += BS)
                for (uint32_t jj = 0; jj < P; jj += BS)
                {
                    uint32_t i_max = (ii + BS < M) ? ii + BS : M;
                    uint32_t k_max = (kk + BS < N) ? kk + BS : N;
                    uint32_t j_max = (jj + BS < P) ? jj + BS : P;
                    for (uint32_t i = ii; i < i_max; i++) {
                        float* restrict c_row = C + i * P;
                        float* restrict a_row = A + i * N;
                        for (uint32_t k = kk; k < k_max; k++) {
                            float a_val = a_row[k];
                            float* restrict b_row = B + k * P;
                            #pragma clang loop vectorize(enable) interleave(enable)
                            for (uint32_t j = jj; j < j_max; j++) {
                                c_row[j] += a_val * b_row[j];
                            }
                        }
                    }
                }
    }
    else if (x->type == kdouble || x->type == kfloat64) {
        double * restrict A = (double*) x->data;
        double * restrict B = (double*) y->data;
        double * restrict C = (double*) z->data;
        for (uint32_t i = 0; i < M * P; i++)
            C[i] = 0.0;
        const uint32_t BS = 48;
        for (uint32_t ii = 0; ii < M; ii += BS)
            for (uint32_t kk = 0; kk < N; kk += BS)
                for (uint32_t jj = 0; jj < P; jj += BS)
                {
                    uint32_t i_max = (ii + BS < M) ? ii + BS : M;
                    uint32_t k_max = (kk + BS < N) ? kk + BS : N;
                    uint32_t j_max = (jj + BS < P) ? jj + BS : P;
                    for (uint32_t i = ii; i < i_max; i++) {
                        double* restrict c_row = C + i * P;
                        double* restrict a_row = A + i * N;
                        for (uint32_t k = kk; k < k_max; k++) {
                            double a_val = a_row[k];
                            double* restrict b_row = B + k * P;
                            #pragma clang loop vectorize(enable) interleave(enable)
                            for (uint32_t j = jj; j < j_max; j++) {
                                c_row[j] += a_val * b_row[j];
                            }
                        }
                    }
                }
    }
    else if (x->type == kint || x->type == kint32) {
        int32_t * restrict A = (int32_t*) x->data;
        int32_t * restrict B = (int32_t*) y->data;
        int32_t * restrict C = (int32_t*) z->data;
        for (uint32_t i = 0; i < M * P; i++)
            C[i] = 0;
        const uint32_t BS = 64;
        for (uint32_t ii = 0; ii < M; ii += BS)
            for (uint32_t kk = 0; kk < N; kk += BS)
                for (uint32_t jj = 0; jj < P; jj += BS)
                {
                    uint32_t i_max = (ii + BS < M) ? ii + BS : M;
                    uint32_t k_max = (kk + BS < N) ? kk + BS : N;
                    uint32_t j_max = (jj + BS < P) ? jj + BS : P;
                    for (uint32_t i = ii; i < i_max; i++) {
                        int32_t* restrict c_row = C + i * P;
                        int32_t* restrict a_row = A + i * N;
                        for (uint32_t k = kk; k < k_max; k++) {
                            int32_t a_val = a_row[k];
                            int32_t* restrict b_row = B + k * P;
                            #pragma clang loop vectorize(enable) interleave(enable)
                            for (uint32_t j = jj; j < j_max; j++) {
                                c_row[j] += a_val * b_row[j];
                            }
                        }
                    }
                }
    }
    else if (x->type == kint64) {
        int64_t * restrict A = (int64_t*) x->data;
        int64_t * restrict B = (int64_t*) y->data;
        int64_t * restrict C = (int64_t*) z->data;
        for (uint32_t i = 0; i < M * P; i++)
            C[i] = 0;
        const uint32_t BS = 48;
        for (uint32_t ii = 0; ii < M; ii += BS)
            for (uint32_t kk = 0; kk < N; kk += BS)
                for (uint32_t jj = 0; jj < P; jj += BS)
                {
                    uint32_t i_max = (ii + BS < M) ? ii + BS : M;
                    uint32_t k_max = (kk + BS < N) ? kk + BS : N;
                    uint32_t j_max = (jj + BS < P) ? jj + BS : P;
                    for (uint32_t i = ii; i < i_max; i++) {
                        int64_t* restrict c_row = C + i * P;
                        int64_t* restrict a_row = A + i * N;
                        for (uint32_t k = kk; k < k_max; k++) {
                            int64_t a_val = a_row[k];
                            int64_t* restrict b_row = B + k * P;
                            #pragma clang loop vectorize(enable) interleave(enable)
                            for (uint32_t j = jj; j < j_max; j++) {
                                c_row[j] += a_val * b_row[j];
                            }
                        }
                    }
                }
    }
    else {
        fprintf(stderr, "Error: unsupported type in matrix_mul.\n");
        exit(EXIT_FAILURE);
    }
}

void matrix_transpose(const matrix* x, matrix* y) {
	if (!is_valid_matrix_pointer(x)) {
		fprintf(stderr, "Matrix transpose x is an invalid matrix pinter.\n");
		exit(EXIT_FAILURE);
	}
	if (!is_valid_matrix_pointer(y)) {
		fprintf(stderr, "Matrix transpose y is an invalid matrix pointer.\n");
		exit(EXIT_FAILURE);
	}
	if (!(are_int32(x, y) || are_float32(x,y) || are_float64(x, y))) {
		fprintf(stderr, "Matrix transpose, different data types.\n");
		exit(EXIT_FAILURE);
	}
	if ((x->shape[0] != y->shape[1]) || (x->shape[1] != y->shape[0])) {
		fprintf(stderr, "Matrix transpose incompatible shapes.\n");
		exit(EXIT_FAILURE);
	}
	data_type type = x->type;
	uint32_t m = x->shape[0];
	uint32_t n = x->shape[1];
	uint32_t x_strides_rows = x->strides[0];
	uint32_t x_strides_cols = x->strides[1];
	uint32_t y_strides_rows = y->strides[0];
	uint32_t y_strides_cols = y->strides[1];
	if (type == kint || type == kint32) {
		int* casted_x = (int*)x->data;
		int* casted_y = (int*)y->data;
		for (uint32_t i = 0; i < m; i++) {
			for (uint32_t j = 0; j < n; j++) {
				casted_y[j*y_strides_rows + i*y_strides_cols] = casted_x[i*x_strides_rows + j*x_strides_cols];
			}
		}
	} else if (type == kfloat || type == kfloat32) {
		float* casted_x = (float*)x->data;
		float* casted_y = (float*)y->data;
		for (uint32_t i = 0; i < m; i++) {
			for (uint32_t j = 0; j < n; j++) {
				casted_y[j*y_strides_rows + i*y_strides_cols] = casted_x[i*x_strides_rows + j*x_strides_cols];
			}
		}
	} else if (type == kdouble || type == kfloat64) {
		double* casted_x = (double*)x->data;
		double* casted_y = (double*)y->data;
		for (uint32_t i = 0; i < m; i++) {
			for (uint32_t j = 0; j < n; j++) {
				casted_y[j*y_strides_rows + i*y_strides_cols] = casted_x[i*x_strides_rows + j*x_strides_cols];
			}
		}
	}
	else if (type == kint64) {
		int64_t* casted_x = (int64_t*)x->data;
		int64_t* casted_y = (int64_t*)y->data;
		for (uint32_t i = 0; i < m; i++) {
			for (uint32_t j = 0; j < n; j++) {
				casted_y[j*y_strides_rows + i*y_strides_cols] = casted_x[i*x_strides_rows + j*x_strides_cols];
			}
		}
	}
}

matrix *matrix_ones(data_type type, uint32_t shape[2]) {
	matrix *mat = matrix_make(type, shape);
	if (type == kint || type == kint32) {
		int value = 1;
		for (uint32_t i = 0; i < mat->size; i++) {
			matrix_set_at(mat, i, &value);
		}
	} else if (type == kfloat || type == kfloat32) {
		float value = 1.f;
		for (uint32_t i = 0; i < mat->size; i++) {
			matrix_set_at(mat, i, &value);
		}
	} else if (type == kdouble || type == kfloat64) {
		double value = 1.;
		for (uint32_t i = 0; i < mat->size; i++) {
			matrix_set_at(mat, i, &value);
		}
	} else if (type == kint64) {
		int64_t value = 1.;
		for (uint32_t i = 0; i < mat->size; i++) {
			matrix_set_at(mat, i, &value);
		}
	}
	return mat;
}

matrix *matrix_zeros(data_type type, uint32_t shape[2]) {
	matrix *mat = matrix_make(type, shape);
	if (type == kint || type == kint32) {
		int value = 0;
		for (uint32_t i = 0; i < mat->size; i++) {
			matrix_set_at(mat, i, &value);
		}
	} else if (type == kfloat || type == kfloat32) {
		float value = 0.f;
		for (uint32_t i = 0; i < mat->size; i++) {
			matrix_set_at(mat, i, &value);
		}
	} else if (type == kdouble || type == kfloat64) {
		double value = 0.;
		for (uint32_t i = 0; i < mat->size; i++) {
			matrix_set_at(mat, i, &value);
		}
	} else if (type == kint64) {
		int64_t value = 0.;
		for (uint32_t i = 0; i < mat->size; i++) {
			matrix_set_at(mat, i, &value);
		}
	}
	return mat;
}

matrix *matrix_fill(data_type type, uint32_t shape[2], void* value) {
	matrix *mat = matrix_make(type, shape);
	if (type == kint || type == kint32) {
		for (uint32_t i = 0; i < mat->size; i++) {
			matrix_set_at(mat, i, value);
		}
	} else if (type == kfloat || type == kfloat32) {
		for (uint32_t i = 0; i < mat->size; i++) {
			matrix_set_at(mat, i, value);
		}
	} else if (type == kdouble || type == kfloat64) {
		for (uint32_t i = 0; i < mat->size; i++) {
			matrix_set_at(mat, i, value);
		}
	} else if (type == kint64) {
		for (uint32_t i = 0; i < mat->size; i++) {
			matrix_set_at(mat, i, value);
		}
	}
	return mat;
}

matrix *matrix_arange(data_type type, uint32_t shape[2], void* value) {
	matrix *mat = matrix_make(type, shape);
	if (type == kint || type == kint32) {
		matrix_set_at(mat, 0, value);
		for (uint32_t i = 1; i < mat->size; i++) {
			int next_value = ((int*)mat->data)[i-1] + 1;
			matrix_set_at(mat, i, &next_value);
		}
	} else if (type == kfloat || type == kfloat32) {
		matrix_set_at(mat, 0, value);
		for (uint32_t i = 1; i < mat->size; i++) {
			float next_value = ((float*)mat->data)[i-1] + 1.f;
			matrix_set_at(mat, i, &next_value);
		}
	} else if (type == kdouble || type == kfloat64) {
		matrix_set_at(mat, 0, value);
		for (uint32_t i = 1; i < mat->size; i++) {
			double next_value = ((double*)mat->data)[i-1] + 1.;
			matrix_set_at(mat, i, &next_value);
		}
	} else if (type == kint64) {
		matrix_set_at(mat, 0, value);
		for (uint32_t i = 1; i < mat->size; i++) {
			int64_t next_value = ((int64_t*)mat->data)[i-1] + 1.;
			matrix_set_at(mat, i, &next_value);
		}
	}

	return mat;
}

bool matrix_are_close(const matrix* x, const matrix *y, double eps) {
	if (!is_valid_matrix_pointer(x)) {
		fprintf(stderr, "Error x is an invalid matrix pointer.\n");
		exit(EXIT_FAILURE);
	}
	if (!is_valid_matrix_pointer(y)) {
		fprintf(stderr, "Error y is an invalid matrix pointer.\n");
		exit(EXIT_FAILURE);
	}
	if (x->type != y->type) {
		fprintf(stderr, "Error different types.");
		exit(EXIT_FAILURE);
	}
	if ((x->shape[0] != y->shape[0]) || (x->shape[1] != y->shape[1])) {
		fprintf(stderr, "Eror different shapes.\n");
		return false;
	}
	uint32_t size = x->size;
	data_type type = x->type;
	bool ret = true;
	if (type == kint || type == kint32) {
		int* casted_x = (int*) x->data;
		int* casted_y = (int*) y->data;
		for (uint32_t i = 0; i < size; i++) {
			if (casted_x[i] != casted_y[i]) {
				ret = false;
				break;
			}
		}
	} else if (type == kfloat || type == kfloat32) {
		float* casted_x = (float*) x->data;
		float* casted_y = (float*) y->data;
		for (uint32_t i = 0; i < size; i++) {
			if (fabs(casted_x[i] - casted_y[i]) >= eps) {
				ret = false;
				break;
			}
		}
	} else if (type == kdouble || type == kfloat64) {
		double* casted_x = (double*) x->data;
		double* casted_y = (double*) y->data;
		for (uint32_t i = 0; i < size; i++) {
			if (fabs(casted_x[i] - casted_y[i]) >= eps) {
				ret = false;
				break;
			}
		}
	} else if (type == kint64) {
		int64_t* casted_x = (int64_t*) x->data;
		int64_t* casted_y = (int64_t*) y->data;
		for (uint32_t i = 0; i < size; i++) {
			if (casted_x[i] != casted_y[i]) {
				ret = false;
				break;
			}
		}
	}
	return ret;
}

bool matrix_equal(const matrix* x, const matrix *y) {
	if (!is_valid_matrix_pointer(x)) {
		fprintf(stderr, "Error x is a NULL matrix.\n");
		exit(EXIT_FAILURE);
	}
	if (!is_valid_matrix_pointer(y)) {
		fprintf(stderr, "Error y is a NULL matrix.\n");
		exit(EXIT_FAILURE);
	}
	if (x->type != y->type) {
		fprintf(stderr, "Error different types.");
		exit(EXIT_FAILURE);
	}
	if ((x->shape[0] != y->shape[0]) || (x->shape[1] != y->shape[1])) {
		fprintf(stderr, "Eror different shapes.\n");
		return false;
	}
	return x->data == y->data;
}

