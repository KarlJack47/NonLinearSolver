#ifndef __METHODS_H__
#define __METHODS_H__

#include "general.h"

number bisection( data *d, char v ) {
	number *a = malloc(d->var.len*sizeof(*a)), *b = malloc(d->var.len*sizeof(*b));
	int idx = 0;
	for (int i = 0; i < d->var.len; i++) {
		VAR v0 = get_by_idx(&d->init[0], i)->data.v;
		VAR v1 = get_by_idx(&d->init[1], i)->data.v;
		if (v0.name == v) idx = i;
		a[i] = v0.value;
		b[i] = v1.value;
	}

	number f_a = evaluate_function(&d->f[0], &d->var, a);
	number f_b = evaluate_function(&d->f[0], &d->var, b);
	if (fabsl(f_a) < d->tol) {
		append(&d->x, (data_type) { .v={ v, a[idx] } });
		append(&d->f_vals, (data_type) { .l={ f_a } });
		number result = a[idx];
		free(a); free(b);
		return result;
	}
	if (fabsl(f_b) < d->tol) {
		append(&d->x, (data_type) { .v={ v, b[idx] } });
		append(&d->f_vals, (data_type) { .l={ f_b } });
		number result = b[idx];
		free(a); free(b);
		return result;
	}

	if (f_a * f_b > 0) {
		free(a); free(b);
		return NAN;
	}

	number *mid = malloc(d->var.len*sizeof(*mid)); number f_mid;
	memcpy(mid, b, d->var.len*sizeof(*mid));
	while (d->iter < N) {
		mid[idx] = (a[idx] + b[idx]) / (number) 2;
		f_mid = evaluate_function(&d->f[0], &d->var, mid);
		append(&d->x, (data_type) { .v={ v, mid[idx] } });
		append(&d->f_vals, (data_type) { .l={ f_mid } });
		if (fabsl(f_mid) < d->tol)
			break;
		f_a = evaluate_function(&d->f[0], &d->var, a);
		if (f_mid * f_a < 0)
			b[idx] = mid[idx];
		else
			a[idx] = mid[idx];
		d->iter++;
	}

	free(a); free(b);
	number result = mid[idx];
	free(mid);

	if (d->iter == N && !(fabsl(f_mid) < d->tol))
		return NAN;

	return result;
}

number false_position( data *d, char v ) {
	number *a = malloc(d->var.len*sizeof(*a)), *b = malloc(d->var.len*sizeof(*b));
	int idx = 0;
	for (int i = 0; i < d->var.len; i++) {
		VAR v0 = get_by_idx(&d->init[0], i)->data.v;
		VAR v1 = get_by_idx(&d->init[1], i)->data.v;
		if (v0.name == v) idx = i;
		a[i] = v0.value;
		b[i] = v1.value;
	}

	number f_a = evaluate_function(&d->f[0], &d->var, a);
	number f_b = evaluate_function(&d->f[0], &d->var, b);
	if (fabsl(f_a) < d->tol) {
		append(&d->x, (data_type) { .v={ v, a[idx] } });
		append(&d->f_vals, (data_type) { .l={ f_a } });
		number result = a[idx];
		free(a); free(b);
		return result;
	} else if (fabsl(f_b) < d->tol) {
		append(&d->x, (data_type) { .v={ v, b[idx] } });
		append(&d->f_vals, (data_type) { .l={ f_b } });
		number result = b[idx];
		free(a); free(b);
		return result;
	}

	number *x = malloc(d->var.len*sizeof(*x)); number f_x;
	memcpy(x, b, d->var.len*sizeof(*x));
	while (d->iter < N) {
		f_a = evaluate_function(&d->f[0], &d->var, a);
		f_b = evaluate_function(&d->f[0], &d->var, b);
		x[idx] = (a[idx] * f_b - b[idx] * f_a) / (f_b - f_a);
		f_x = evaluate_function(&d->f[0], &d->var, x);
		if (f_a * f_x < 0)
			b[idx] = x[idx];
		else
			a[idx] = x[idx];

		append(&d->x, (data_type) { .v={ v, x[idx] } });
		append(&d->f_vals, (data_type) { .l={ f_x } });
		d->iter++;
		if (fabsl(f_x) < d->tol)
			break;
	}

	free(a); free(b);
	number result = x[idx];
	free(x);

	if (d->iter == N && !(fabsl(f_x) < d->tol))
		return NAN;

	return result;
}

number fixed_point( data *d, char v ) {
	number *x = malloc(d->var.len*sizeof(*x));
	int idx = 0;
	for (int i = 0; i < d->var.len; i++) {
		VAR v0 = get_by_idx(&d->init[0], i)->data.v;
		if (v0.name == v) idx = i;
		x[i] = v0.value;
	}

	number f = evaluate_function(&d->f[0], &d->var, x);
	if (d->iter == 0 || (isEmpty(&d->x) == 1 && !(fabsl(x[idx] - last(&d->x)->data.v.value) < TOL))) {
		append(&d->x, (data_type) { .v={ v, x[idx] } });
		append(&d->f_vals, (data_type) { .l={ f } });
	}

	if (fabsl(f) < d->tol) { number result = x[idx]; free(x); return result; }

	number x_prev = x[idx];
	while (d->iter < N) {
		d->iter++;
		x_prev = x[idx];
		x[idx] = evaluate_function(&d->f[1], &d->var, x);
		if (d->accel == 0) {
			number x1 = x[idx];
			x[idx] = evaluate_function(&d->f[1], &d->var, x);
			number x2 = x[idx];
			if ((x2 - 2.0L * x1 + x_prev) == 0) { x[idx] = x2; break; }
			x[idx] = x_prev - powl((x1 - x_prev), 2.0L) / (x2 - 2.0L * x1 + x_prev);
		}
		append(&d->x, (data_type) { .v={ v, x[idx] } });
		append(&d->f_vals, (data_type) { .l={ evaluate_function(&d->f[0], &d->var, x) } });
		if (fabsl(x[idx] - x_prev) < d->tol)
			break;
	}

	number result = x[idx];
	free(x);

	if (d->iter == N && !(fabsl(x[idx] - x_prev) < d->tol))
		return NAN;

	return result;
}

// Quadratic order of convergence scheme
number newton( data *d, char v ) {
	number *curr = malloc(d->var.len*sizeof(*curr));
	int idx = 0;
	for (int i = 0; i < d->var.len; i++) {
		VAR v0 = get_by_idx(&d->init[0], i)->data.v;
		if (v0.name == v) idx = i;
		curr[i] = v0.value;
	}

	number f_curr = evaluate_function(&d->f[0], &d->var, curr);
	if (d->iter == 0 || (isEmpty(&d->x) == 1 && !(fabsl(curr[idx] - last(&d->x)->data.v.value) < TOL))) {
		append(&d->x, (data_type) { .v={ v, curr[idx] } });
		append(&d->f_vals, (data_type) { .l={ f_curr } });
	}

	if (fabsl(f_curr) < d->tol) { number result = curr[idx]; free(curr); return result; }

	number *next = malloc(d->var.len*sizeof(*next));
	memcpy(next, curr, d->var.len*sizeof(*next));
	number f_next = NAN;
	while (d->iter < N) {
		d->iter++;
		f_curr = evaluate_function(&d->f[0], &d->var, curr);
		number df_curr;
		if (d->df[0].len == 0) df_curr = differentiate(&d->f[0], &d->var, v, curr, 1);
		else df_curr = evaluate_function(&d->df[0], &d->var, curr);
		next[idx] = curr[idx] - (f_curr / df_curr);
		f_next = evaluate_function(&d->f[0], &d->var, next);
		append(&d->x, (data_type) { .v={ v, next[idx] } });
		append(&d->f_vals, (data_type) { .l={ f_next } });
		if (stopping_condition(next[idx], f_next, curr[idx], d->tol) == 0)
			break;
		curr[idx] = next[idx];
	}

	free(curr);
	number result = next[idx];
	free(next);

	if (d->iter == N && stopping_condition(result, f_next, d->x.tail->prev->data.v.value, d->tol) == 1)
		return NAN;

	return result;
}

// Quadratic order of convergence scheme
number secant( data *d, char v ) {
	number *prev = malloc(d->var.len*sizeof(*prev));
	number *curr = malloc(d->var.len*sizeof(*curr));
	int idx = 0;
	for (int i = 0; i < d->var.len; i++) {
		VAR v0 = get_by_idx(&d->init[0], i)->data.v;
		VAR v1 = get_by_idx(&d->init[1], i)->data.v;
		if (v0.name == v) idx = i;
		prev[i] = v0.value;
		curr[i] = v1.value;
	}

	number f_prev = evaluate_function(&d->f[0], &d->var, prev);
	if (d->iter == 0 || (isEmpty(&d->x) == 1 && !(fabsl(prev[idx] - last(&d->x)->data.v.value) < TOL))) {
		append(&d->x, (data_type) { .v={ v, prev[idx] } });
		append(&d->f_vals, (data_type) { .l={ f_prev } });
	}

	number f_curr = evaluate_function(&d->f[0], &d->var, curr);
	append(&d->x, (data_type) { .v={ v, curr[idx] } });
	append(&d->f_vals, (data_type) { .l={ f_curr } });

	if (fabsl(f_prev) < d->tol) { free(curr); number result = prev[idx]; free(prev); return result; }
	if (fabsl(f_curr) < d->tol) { number result = curr[idx]; free(curr); free(prev); return result; }

	number *next = malloc(d->var.len*sizeof(*next));
	memcpy(next, curr, d->var.len*sizeof(*next));
	number f_next = NAN;
	while (d->iter < N) {
		d->iter++;
		f_prev = evaluate_function(&d->f[0], &d->var, prev);
		f_curr = evaluate_function(&d->f[0], &d->var, curr);
		next[idx] = curr[idx] - ((f_curr / (f_curr - f_prev)) * (curr[idx] - prev[idx]));
		f_next = evaluate_function(&d->f[0], &d->var, next);
		append(&d->x, (data_type) { .v={ v, next[idx] } });
		append(&d->f_vals, (data_type) { .l={ f_next } });
		if (stopping_condition(next[idx], f_next, curr[idx], d->tol) == 0)
			break;
		prev[idx] = curr[idx]; curr[idx] = next[idx];
	}

	free(curr); free(prev);
	number result = next[idx];
	free(next);

	if (d->iter == N && stopping_condition(result, f_next, d->x.tail->prev->data.v.value, d->tol) == 1)
		return NAN;

	return result;
}

// Cubic order of convergence scheme
number halley( data *d, char v ) {
	number *curr = malloc(d->var.len*sizeof(*curr));
	int idx = 0;
	for (int i = 0; i < d->var.len; i++) {
		VAR v0 = get_by_idx(&d->init[0], i)->data.v;
		if (v0.name == v) idx = i;
		curr[i] = v0.value;
	}

	number f_curr = evaluate_function(&d->f[0], &d->var, curr);
	if (d->iter == 0 || (isEmpty(&d->x) == 1 && !(fabsl(curr[idx] - last(&d->x)->data.v.value) < TOL))) {
		append(&d->x, (data_type) { .v={ v, curr[idx] } });
		append(&d->f_vals, (data_type) { .l= { f_curr } });
	}

	if (fabsl(f_curr) < d->tol) { number result = curr[idx]; free(curr); return result; }

	number *next = malloc(d->var.len*sizeof(*next));
	memcpy(next, curr, d->var.len*sizeof(*next));
	number f_next = NAN;
	while (d->iter < N) {
		d->iter++;
		f_curr = evaluate_function(&d->f[0], &d->var, curr);
		number df_curr, ddf_curr;
		if (d->df[0].len == 0) df_curr = differentiate(&d->f[0], &d->var, v, curr, 1);
		else df_curr = evaluate_function(&d->df[0], &d->var, curr);
		if (d->df[0].len != 0 && d->ddf[0].len == 0) ddf_curr = differentiate(&d->df[0], &d->var, v, curr, 1);
		else if (d->df[0].len == 0 && d->ddf[0].len == 0) ddf_curr = differentiate(&d->f[0], &d->var, v, curr, 2);
		else ddf_curr = evaluate_function(&d->ddf[0], &d->var, curr);
		next[idx] = curr[idx] - ((2 * f_curr * df_curr) / ((2 * df_curr * df_curr) - (f_curr * ddf_curr)));
		f_next = evaluate_function(&d->f[0], &d->var, next);
		append(&d->x, (data_type) { .v={ v, next[idx] } });
		append(&d->f_vals, (data_type) { .l={ f_next } });
		if (stopping_condition(next[idx], f_next, curr[idx], d->tol) == 0)
			break;
		curr[idx] = next[idx];
	}

	free(curr);
	number result = next[idx];
	free(next);

	if (d->iter == N && stopping_condition(result, f_next, d->x.tail->prev->data.v.value, d->tol) == 1)
		return NAN;

	return result;
}

number hybrid( data *d, char v, method *methods, int n, number *tols ) {
	number t_tol = d->tol;
	number t_a = get_var(&d->init[0], v);
	number t_b = get_var(&d->init[1], v);

	number root = NAN;
	for (int i = 0; i < n; i++) {
		d->tol = tols[i];
		root = methods[i](d, v);
		if (!(isnan(root))) update_var(&d->init[0], v, last(&d->x)->data.v.value);
		if (get_var(&d->init[1], v) < get_var(&d->init[0], v)) {
			number b_old = get_var(&d->init[1], v);
			update_var(&d->init[1], v, b_old + fabsl(b_old - get_var(&d->init[0], v)));
		}
	}

	d->tol = t_tol;

	update_var(&d->init[0], v, t_a); update_var(&d->init[1], v, t_b);

	return root;
}

int multiple_roots( data *d, char v, method *methods, int n, number *tols ) {
	clock_t t_root;
	number t_a = get_var(&d->init[0], v);
	number t_b = get_var(&d->init[1], v);

	update_var(&d->init[0], v, d->A);
	update_var(&d->init[1], v, d->A+d->dist);
	do {
		t_root = clock();
		d->iter = 0;
		empty(&d->x); empty(&d->f_vals);
		number root = hybrid(d, v, methods, n, tols);
		t_root = clock() - t_root;
		double elapsedTime = ((double)t_root) / CLOCKS_PER_SEC;
		if (!(isnan(root))) {
			node *roots = d->roots.tail;
			int check = 1;
			while (roots != NULL) {
				if (fabsl(roots->data.v.value - last(&d->x)->data.v.value) < TOL) { check = 0; break; }
				roots = roots->prev;
			}
			if (isEmpty(&d->roots) == 0 || (d->roots.len > 0 && check == 1)) {
				printf("A root of the function is %.10Lf.\n", last(&d->x)->data.v.value);
				printf("It took %d steps to find the root.\n", d->iter);
				printf("It took %f seconds to find the root.\n", elapsedTime);
				append(&d->roots, last(&d->x)->data);
			}
		}

		number a_new = get_var(&d->init[1], v);
		update_var(&d->init[0], v, a_new);
		update_var(&d->init[1], v, a_new + d->dist);
	} while (!(fabsl(d->B - get_var(&d->init[1], v) + d->dist)*10 < 10));

	update_var(&d->init[0], v, t_a); update_var(&d->init[1], v, t_b);

	if (isEmpty(&d->roots) == 0) return -1;
	else return d->roots.len;
}

number* fixed_point_system( data *d, int n_func ) {
	number *x = malloc(d->var.len*sizeof(*x));
	for (int i = 0; i < d->var.len; i++) x[i] = get_by_idx(&d->init[0], i)->data.v.value;
	number *x_prev = malloc(d->var.len*sizeof(*x_prev));
	memcpy(x_prev, x, d->var.len*sizeof(*x_prev));

	number *diff = malloc(d->var.len*sizeof(*diff));
	while (d->iter < N) {
		d->iter++;
		for (int i = 0; i < n_func; i++)
			x[i] = evaluate_function(&d->f[i+n_func], &d->var, x);
		for (int i = 0; i < d->var.len; i++) diff[i] = x[i] - x_prev[i];
		if (vec_norm(diff, d->var.len) < d->tol) break;
		memcpy(x_prev, x, d->var.len*sizeof(*x_prev));
	}

	free(diff); free(x_prev);

	return x;
}

number* newton_system( data *d, number *x_init, int n_func ) {
	number *x = malloc(d->var.len*sizeof(*x));
	number *dx;
	for (int i = 0; i < d->var.len; i++) {
		if (x_init == NULL)
			x[i] = get_by_idx(&d->init[0], i)->data.v.value;
		else
			x[i] = x_init[i];
	}

	int old_iter = 0;
	if (d->iter != 0) { old_iter = d->iter; d->iter = 0; }

	while (d->iter < N) {
		number *f_val = evaluate_vector_functions(d->f, n_func, &d->var, x);
		matrix J = compute_jacobian(d->f, d->df, n_func, &d->var, x);
		for (int i = 0; i < n_func; i++) f_val[i] = -f_val[i];
		int *P = malloc((n_func+1)*sizeof(*P));
		LUP_decompose(&J, d->tol, P);
		dx = LUP_solve(&J, P, f_val);
		free(f_val); free_matrix(&J); free(P);
		for (int i = 0; i < d->var.len; i++) x[i] = x[i] + dx[i];
		if (vec_norm(dx, d->var.len) < d->tol)
			break;
		d->iter++;
	}

	d->iter += old_iter;

	free(dx);

	return x;
}

number* broyden_method( data *d, int n_func ) {
	number *x_init = malloc(d->var.len*sizeof(*x_init));
	for (int i = 0; i < d->var.len; i++) x_init[i] = get_by_idx(&d->init[0], i)->data.v.value;

	matrix J = compute_jacobian(d->f, d->df, n_func, &d->var, x_init);
	int *P = malloc((n_func+1)*sizeof(*P));
	LUP_decompose(&J, d->tol, P);
	matrix A = LUP_invert(&J, P);
	free(P); free_matrix(&J);
	number *v = evaluate_vector_functions(d->f, n_func, &d->var, x_init);
	number *s = matrix_multiply_vector(&A, v, d->var.len);
	number *x = malloc(d->var.len*sizeof(*x));
	for (int i = 0; i < d->var.len; i++) { s[i] = -s[i]; x[i] = x_init[i] - s[i]; }
	d->iter = 2;

	while (d->iter < N) {
		number *w = malloc(d->var.len*sizeof(*w));
		memcpy(w, v, d->var.len*sizeof(*w));
		v = evaluate_vector_functions(d->f, n_func, &d->var, x);
		number *y = malloc(d->var.len*sizeof(*y));
		for (int i = 0; i < d->var.len; i++) y[i] = v[i] - w[i];
		free(w);
		y = matrix_multiply_vector(&A, y, d->var.len);
		for (int i = 0; i < d->var.len; i++) y[i] = -y[i];
		number p = -vector_multiply_vector(s, d->var.len, y, d->var.len);
		number *u = vector_multiply_matrix(s, &A, d->var.len);
		for (int i = 0; i < d->var.len; i++) y[i] = s[i] + y[i];
		matrix temp = outer_product(y, d->var.len, u, d->var.len);
		free(y); free(u);
		for (int i = 0; i < n_func; i++)
			for (int j = 0; j < d->var.len; j++)
				A.data[i][j] = A.data[i][j] + (1.0L/p)*temp.data[i][j];
		free_matrix(&temp);
		s = matrix_multiply_vector(&A, v, d->var.len);
		for (int i = 0;  i < d->var.len; i++) { s[i] = -s[i]; x[i] = x[i] + s[i]; }
		if (vec_norm(s, d->var.len) < d->tol)
			break;
		d->iter++;
	}

	free(s); free(v); free_matrix(&A);

	return x;
}

number* x_init_finder( list *g, data *d, int n_func, number *x_init, int maxIt, number tol ) {
	number *x;

	if (g->len == 0) {
		for (int i = 0; i < n_func; i++) {
			node *start = d->f[i].head;
			while (start != NULL) {
				append(g, start->data);
				start = start->next;
			}
			append(g, (data_type) { .t={ "Literal" }, .l={ 2.0L } });
			char num[50];
			snprintf(num, 50, "%Lf", 2.0L);
			strcpy(last(g)->data.t.value, num);
			append_op(g, '^');
		}
		for (int i = 1; i < n_func; i++) append_op(g, '+');
	}

	if (x_init == NULL && isnan(d->init[0].head->data.v.value)) {
		number *x_temp = calloc(d->var.len, sizeof(*x_temp));
		x = steepest_descent(g, d, x_temp, maxIt, tol);
		free(x_temp);
	} else if (x_init != NULL)
		x = steepest_descent(g, d, x_init, maxIt, tol);
	else
		x = steepest_descent(g, d, NULL, maxIt, tol);

	return x;
}

#endif // __METHODS_H__
