#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
typedef enum {
    kint    = 0,
    kint32  = 1,
    kint64  = 2,
    kfloat  = 3,
    kfloat32 = 4,
    kdouble = 5,
    kfloat64 = 6
} data_type;
typedef struct matrix {
    bool own_data;
    data_type type;
    uint32_t size;
    uint32_t shape[2];
    uint32_t strides[2];
    void *data;
} matrix;
bool is_valid_matrix_pointer(const matrix* m) {
    return (m != NULL && m->data != NULL);
}

bool are_float32(const matrix* x, const matrix* y) {
    return ( (x->type == kfloat || x->type == kfloat32) &&
             (y->type == kfloat || y->type == kfloat32) );
}
bool are_float64(const matrix* x, const matrix* y) {
    return ( (x->type == kdouble || x->type == kfloat64) &&
             (y->type == kdouble || y->type == kfloat64) );
}
bool are_int32(const matrix* x, const matrix* y) {
    return ( (x->type == kint || x->type == kint32) &&
             (y->type == kint || y->type == kint32) );
}
bool are_int64(const matrix* x, const matrix* y) {
    return (x->type == kint64 && y->type == kint64);
}
void matrix_mul(const matrix* x, const matrix* y, matrix* z)
{
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
int main(void)
{
    uint32_t M = 256, N = 256, P = 256;
    matrix A = { true, kfloat32, M*N, {M, N}, {N, 1}, NULL };
    matrix B = { true, kfloat32, N*P, {N, P}, {P, 1}, NULL };
    matrix C = { true, kfloat32, M*P, {M, P}, {P, 1}, NULL };
    A.data = malloc(sizeof(float) * M * N);
    B.data = malloc(sizeof(float) * N * P);
    C.data = malloc(sizeof(float) * M * P);
    for (uint32_t i = 0; i < M*N; i++) ((float*)A.data)[i] = 1.0f;
    for (uint32_t i = 0; i < N*P; i++) ((float*)B.data)[i] = 1.0f;
    clock_t start = clock();
    matrix_mul(&A, &B, &C);
    clock_t end = clock();
    printf("Matrix multiply done in %.4f seconds\n",(double)(end - start) / CLOCKS_PER_SEC);
    printf("C[0]=%f\n", ((float*)C.data)[0]);
    free(A.data);
    free(B.data);
    free(C.data);
    return 0;
}