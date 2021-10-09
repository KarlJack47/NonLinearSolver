#ifndef __LINEAR_ALGEBRA_H__
#define __LINEAR_ALGEBRA_H__

#include "general.h"

struct Matrix {
	number **data;
	int m;
	int n;
};
typedef struct Matrix matrix;

matrix create_matrix( int m, int n ) {
	number **data = calloc(m, sizeof(**data));

	for (int i = 0; i < m; i++) data[i] = calloc(n, sizeof(**data));

	matrix A = { data, m, n };

	return A;
}

// if A.m * A.n < n_vals then vals = [ i0, j0, val0, i1, j1, val1, ... ]
// else vals = [ val0, val1, ... ]
void edit_matrix( matrix *A, number *vals, int n_vals ) {
	if (A->m*A->n == n_vals) {
		for (int i = 0; i < A->m; i++)
			for (int j = 0; j < A->n; j++)
				A->data[i][j] = vals[i * A->n + j];
	} else {
		for (int i = 0; i < n_vals; i++)
			A->data[(int) vals[i*3]][(int) vals[i*3+1]] = vals[i*3+2];
	}
}

int matrix_equals_matrix( matrix *A, matrix *B ) {
	if (A->data == B->data) return 0;
	else return 1;
}

void free_matrix( matrix *A ) {
	for (int i = 0; i < A->m; i++) free(A->data[i]);
	free(A->data);
}

void print_matrix( matrix *A ) {
	for (int i = 0; i < A->m; i++) {
		for (int j = 0; j < A->n; j++)
			printf("%Lf ", A->data[i][j]);
		printf("\n");
	}
}

number* gradient( list *f, int func_idx, list *df, list *var, number *vals ) {
	number *out = malloc(var->len*sizeof(*out));

	for (int i = 0; i < var->len; i++) {
		if (df == NULL || (df != NULL && df[func_idx*var->len+i].len == 0))
			out[i] = differentiate(f, var, get_by_idx(var, i)->data.v.name, vals, 1);
		else if (df != NULL && df[func_idx*var->len+i].len != 0)
			out[i] = evaluate_function(&df[func_idx*var->len+i], var, vals);
	}

	return out;
}

matrix compute_jacobian( list *f, list *df, int n_func, list *var, number *vals ) {
	number **data = calloc(n_func, sizeof(**data));

	for (int i = 0; i < n_func; i++)
		data[i] = gradient(&f[i], i, df, var, vals);

	matrix J = { data, n_func, var->len };

	return J;
}

number* evaluate_vector_functions( list *f, int n_func, list *var, number *vals ) {
	number *f_vals = calloc(n_func, sizeof(*f_vals));

	for (int i = 0; i < n_func; i++)
		f_vals[i] = evaluate_function(&f[i], var, vals);

	return f_vals;
}

number vec_norm( number *vals, int n ) {
	number result = 0.0L;

	for (int i = 0; i < n;  i++) result += powl(vals[i], 2.0L);

	return sqrtl(result);
}

// out = matrix_multiply_matrix( A, B )
// if matrix_equals_matrix(A, out) then multiplication was successful
matrix matrix_multiply_matrix( matrix *A, matrix *B ) {
	if (A->n != B->m) return *A;

	matrix out = create_matrix(A->m, B->n);

	for (int i = 0; i < A->m; i++) {
		for (int j = 0; j < B->n; j++)
			for (int k = 0; k < A->n; k++)
				out.data[i][j] += A->data[i][k]*B->data[k][j];

	}

	return out;
}

number* matrix_multiply_vector( matrix *A, number *x, int m ) {
	if (A->n != m) return x;

	number *out = calloc(A->m, sizeof(*out));

	for (int i = 0; i < A->m; i++)
		for (int j = 0; j < A->n; j++)
			out[i] += A->data[i][j]*x[j];

	return out;
}

number* vector_multiply_matrix( number *x, matrix *A, int n ) {
	if (A->m != n) return x;

	number *out = calloc(A->n, sizeof(*out));

	for (int i = 0; i < A->n; i++)
		for (int j = 0; j < A->m; j++)
			out[i] += x[j]*A->data[j][i];

	return out;
}

number vector_multiply_vector( number *x, int n_x, number *y, int n_y ) {
	if (n_x != n_y) return NAN;

	number out = 0.0L;

	for (int i = 0; i < n_x; i++)
		out += x[i]*y[i];

	return out;
}

matrix outer_product( number *x, int n_x, number *y, int n_y ) {
	matrix out = create_matrix(n_x, n_y);

	for (int i = 0; i < n_x; i++) {
		for (int j = 0; j < n_y; j++)
			for (int k = 0; k < n_y; k++)
				out.data[i][k] = x[i]*y[k];
	}

	return out;
}

int LUP_decompose( matrix *A, number tol, int *P ) {
	int imax;
	number maxA, *ptr, absA;

	for (int i = 0; i <= A->m; i++) P[i] = i;

	for (int i = 0; i < A->m; i++) {
		maxA = 0.0L; imax = i;

		for (int k = i; k < A->n; k++)
			if ((absA = fabs(A->data[k][i])) > maxA) {
				maxA = absA;
				imax = k;
			}

		if (maxA < tol) return 1; // matrix is degenerate

		if (imax != i) {
			// pivoting P
			int j = P[i];
			P[i] = P[imax];
			P[imax] = j;

			// pivoting rows of A
			ptr = A->data[i];
			A->data[i] = A->data[imax];
			A->data[imax] = ptr;

			// counting pivots starting from n (for determinant)
			P[A->m]++;
		}

		for (int j = i + 1; j < A->n; j++) {
			A->data[j][i] /= A->data[i][i];

			for (int k = i + 1; k < A->n; k++)
				A->data[j][k] -= A->data[j][i] * A->data[i][k];
		}
	}

	return 0;
}

number* LUP_solve( matrix *A, int *P, number *b ) {
	number *x = calloc(A->m, sizeof(*x));
	for (int i = 0; i < A->m; i++) {
		x[i] = b[P[i]];

		for (int k = 0; k < i; k++)
			x[i] -= A->data[i][k] * x[k];
	}

	for (int i = A->m - 1; i >= 0; i--) {
		for (int k = i + 1; k < A->n; k++)
			x[i] -= A->data[i][k] * x[k];

		x[i] = x[i] / A->data[i][i];
	}

	return x;
}

matrix LUP_invert( matrix *A, int *P ) {
	matrix IA = create_matrix(A->m, A->n);
	for (int j = 0; j < A->n; j++) {
		for (int i = 0; i < A->m; i++) {
			if (P[i] == j)
				IA.data[i][j] = 1.0L;
			else
				IA.data[i][j] = 0.0L;

			for (int k = 0; k < i; k++)
				IA.data[i][j] -= A->data[i][k] * IA.data[k][j];
		}

		for (int i = A->m - 1; i >= 0; i--) {
			for (int k = i + 1; k < A->n; k++)
				IA.data[i][j] -= A->data[i][k] * IA.data[k][j];

			IA.data[i][j] = IA.data[i][j] / A->data[i][i];
		}
	}

	return IA;
}

number LUP_determinant( matrix *A, int *P ) {
	number det = A->data[0][0];

	for (int i = 1; i < A->m; i++)
		det *= A->data[i][i];

	if ((P[A->m] - A->n) % 2 == 0)
		return det;
	else
		return -det;
}

number* steepest_descent_linear_system_solve( matrix *A, number *b, number *x_init, int maxIt, number tol ) {
	number *x = malloc(A->n*sizeof(*x));
	number *r = malloc(A->n*sizeof(*r));

	memcpy(x, x_init, A->n*sizeof(*x));
	number *result = matrix_multiply_vector(A, x, A->n);
	for (int i = 0; i < A->n; i++) r[i] = b[i] - result[i];
	int iter = 0;
	while (iter != maxIt && !(vec_norm(r, A->n) < tol)) {
		number numerator = powl(vec_norm(r, A->n), (number) 2);
		number denominator = vector_multiply_vector(r, A->n, matrix_multiply_vector(A, r, A->n), A->n);
		number t = numerator / denominator;
		for (int i = 0;  i < A->n; i++) x[i] = x[i] + t * r[i];
		result = matrix_multiply_vector(A, x, A->n);
		for (int i = 0; i < A->n; i++) r[i] = b[i] - result[i];
		iter++;
	}

	free(result); free(r);

	return x;
}

number* steepest_descent( list *g, data *d, number *x_init, int maxIt, number tol ) {
	if (d->iter == 0) d->iter = 1;
	else d->iter++;

	number *x1 = malloc(d->var.len*sizeof(*x1));
	if (x_init != NULL) memcpy(x1, x_init, d->var.len*sizeof(*x1));
	else for (int i = 0; i < d->var.len; i++) x1[i] = get_by_idx(&d->init[0], i)->data.v.value;
	while (d->iter <= maxIt) {
		number *z = gradient(g, -1, NULL, &d->var, x1);
		number z0 = vec_norm(z, d->var.len);
		if (fabsl(z0) < tol) return x1;
		for (int i = 0; i < d->var.len; i++) z[i] = z[i] / z0;
		number alpha1 = 0; number alpha3 = 1;
		number g1 = evaluate_function(g, &d->var, x1);
		number *x3 = malloc(d->var.len*sizeof(*x3));
		for (int i = 0; i < d->var.len; i++) x3[i] = x1[i] - alpha3*z[i];
		number g3 = evaluate_function(g, &d->var, x3);
		while (g3 >= g1) {
			alpha3 = alpha3 / 2.0L;
			for (int i = 0; i < d->var.len; i++) x3[i] = x1[i] - alpha3*z[i];
			g3 = evaluate_function(g, &d->var, x3);
			if (fabsl(alpha3) < tol / 2.0L) return x1;
		}
		number alpha2 = alpha3 / 2.0L;
		number *x2 = x3;
		for (int i = 0; i < d->var.len; i++) x2[i] = x1[i] - alpha2*z[i];
		number g2 = evaluate_function(g, &d->var, x2);
		number h1 = (g2 - g1) / alpha2;
		number h2 = (g3 - g2) / (alpha3 - alpha2);
		number h3 = (h2 - h1) / alpha3;
		number alpha0 = 0.5L*(alpha2 - h1 / h3);
		number *x0 = x2;
		for (int i = 0; i < d->var.len; i++) x0[i] = x1[i] - alpha0*z[i];
		number g0 = evaluate_function(g, &d->var, x0);
		free(x0);
		number alpha = alpha0;
		if (g3 < g0) alpha = alpha3;
		for (int i = 0; i < d->var.len; i++) x1[i] = x1[i] - alpha*z[i];
		free(z);
		if (fabsl(evaluate_function(g, &d->var, x1) - g1) < tol) return x1;
		d->iter++;
	}

	return x1;
}

#endif // __LINEAR_ALGEBRA_H__
