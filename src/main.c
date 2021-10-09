#include "../headers/general.h"

int main( void ) {
	char buff_eq[MAX_BUFF] = "N";
	char buff_deq[MAX_BUFF] = "N";
	char buff_ddeq[MAX_BUFF] = "N";
	char buff_var[MAX_BUFF] = "N";
	char buff_meth[MAX_BUFF] = "N";
	int invalid_eq = NO_INPUT;
	int quit = 1; int idx = 0;
	char *token; char *token1;
	char *save_ptr1; char *save_ptr2;
	const char s[4] = ":= ";

	init_list(&funcs, valid_func, NFUNC);
	init_list(&consts, valid_const, NCONST);
	init_list(&ops, valid_op, NOP);

	data d = { .tol=FLT_EPSILON/(number) 512, .iter = 0 };
	d.f = malloc(2*MAX_FUNC*sizeof(*d.f));
	for (int i = 0; i < 2*MAX_FUNC; i++) { mem_init_list(&d.f[i]); }
	mem_init_list(&d.var);
	d.ddf = malloc(MAX_FUNC*sizeof(*d.ddf));
	for (int i = 0; i < MAX_FUNC; i++) { mem_init_list(&d.ddf[i]); }
	d.init = malloc(2*sizeof(*d.init)); mem_init_list(&d.init[0]); mem_init_list(&d.init[1]);

	printf("Welcome to the Nonlinear System Solver Program.\n");
	printf("By: Karl Deutscher\n\n");
	printf("General Instructions:\n");
	printf("\t1. Enter Q at any point to quit the program.\n");
	printf("\t2. Note that the maximum number of input characters is %d.\n", MAX_BUFF);
	printf("\t3. When asked a question enter y or yes for yes and n or no for no.\n");
	printf("\t4. Please note that there is no error checking on inputs so it is on you to type everything correctly.\n");
	printf("\t5. Because of how the program parses input please don't put spaces in your input unless you are defining something.\n");
	printf("\t6. When wanting to use the constants Pi or e enter P or E respectively.\n");
	printf("\t7. When writing mathematical expressions you can use implicit multiplication such as 2x or xy.\n");
	printf("\t8. When writing mathematical expressions you can use any of the special math functions in math.h.\n");
	printf("\t9. When entering a variable value please enter as var_name value or var_name:=value or var_name:value or var_name=value.\n");
	printf("\t10. Note that when entering a variable you may write its value as a mathematical expression such as 1/2 or P+3.\n");
	printf("\t11. When entering multiple variable values separate each initialization by a space.\n");
	printf("\t12. Note that you can write a variable value in terms of a previous entered variable so for example: x 1 y 2x\n");
	printf("\t13. When writing a function formula please enter as f_name formula or f_name:=formula or f_name:formula or f_name=formula.\n");
	printf("\t14. When entering multiple functions separate each by a space.\n");
	printf("\t15. You can enter a multivariate function, even when just solving one equation.\n");
	printf("\t16. Right now a system of nonlinear equations can have only %d functions.\n", MAX_FUNC);
	printf("Now that you know the basics lets get going.\n\n");
	while (quit == 1) {
		memset(buff_eq, 0, sizeof(buff_eq));
		memset(buff_deq, 0, sizeof(buff_deq));
		memset(buff_ddeq, 0, sizeof(buff_ddeq));
		memset(buff_var, 0, sizeof(buff_var));
		memset(buff_meth, 0, sizeof(buff_meth));

		if (invalid_eq != OK) {
			invalid_eq = getLine("Enter function(s): ", buff_eq, sizeof(buff_eq));
			if (strstr(buff_eq, "Q")) break;
			if (invalid_eq != OK) continue;
			token = strtok_r(buff_eq, s, &save_ptr1);

			idx = 0;
			while (token != NULL) {
				token = strtok_r(NULL, s, &save_ptr1);
				if (token != NULL)
					create_function(token, &d.f[idx++], &d.var, d.init);
				token = strtok_r(NULL, s, &save_ptr1);
			}
		}

		d.df = malloc(d.var.len*MAX_FUNC*sizeof(*d.df));
		for (int i = 0; i < d.var.len*MAX_FUNC; i++) { mem_init_list(&d.df[i]); }

		if (idx == 1) {
			mem_init_list(&d.x); mem_init_list(&d.f_vals); mem_init_list(&d.roots);
			printf("If you input multiple methods below you will create a hybrid method, where the order of input matches the order of computation.\n");
			printf("To run multiple methods successively separate them by a ',' with no space after.\n");
			getLine("Enter method(s) (Bisection, False-Position, Fixed-Point, Newton, Secant, Halley): ", buff_meth, sizeof(buff_meth));
			if (strstr(buff_meth, "Q")) { free_df(d.df, d.var.len); break; }

			token = strtok_r(buff_meth, ",", &save_ptr1);
			while (token != NULL) {
				char name[MAX_BUFF];
				method methods[MAX_METH];
				number tols[MAX_METH];
				printf("You are configuring method %s:\n", token);
				strcpy(name, token);

				int idx_m = 0;
				if (strstr(token, "Bisection")) methods[idx_m++] = &bisection;
				if (strstr(token, "False-Position")) methods[idx_m++] = &false_position;
				if (strstr(token, "Fixed-Point")) methods[idx_m++] = &fixed_point;
				if (strstr(token, "Newton")) methods[idx_m++] = &newton;
				if (strstr(token, "Secant")) methods[idx_m++] = &secant;
				if (strstr(token, "Halley")) methods[idx_m++] = &halley;
				if (strstr(token, "Fixed-Point")) {
					char g[MAX_BUFF];
					getLine("Enter the fixed point formula: ", g, sizeof(g));
					if (strstr(g, "Q")) { quit = 0; break; }
					token1 = strtok_r(g, s, &save_ptr2);
					token1 = strtok_r(NULL, s, &save_ptr2);
					if (token1 != NULL) create_function(token1, &d.f[1], &d.var, NULL);
					else { printf("Invalid input.\n"); quit = 0; break; }
					char accel[10];
					getLine("Do you want to use Aitken's acceleration method? ", accel, sizeof(accel));
					if (strstr(accel, "Q")) { quit = 0; break; }
					if (strstr(accel, "y") || strstr(accel, "yes")) d.accel = 0;
					else d.accel = 1;
				}
				if (strstr(token, "Newton") || strstr(token, "Halley")) {
					char numer[10];
					getLine("Do you want numeric differentation for the first derivative? ", numer, sizeof(numer));
					if (strstr(numer, "Q")) { quit = 0; break; }
					if (strstr(numer, "n") || strstr(numer, "no")) {
						getLine("Input formula for the first derivative: ", buff_deq, sizeof(buff_deq));
						if (strstr(buff_deq, "Q")) { quit = 0; break; }
						token1 = strtok_r(buff_deq, s, &save_ptr2);
						token1 = strtok_r(NULL, s, &save_ptr2);
						if (token1 != NULL) create_function(token1, &d.df[0], &d.var, NULL);
						else { printf("Invalid input.\n"); quit = 0; break; }
					}
					if (strstr(token, "Halley")) {
						memset(numer, 0, sizeof(numer));
						getLine("Do you want numeric differentiation for the second derivative? ", numer, sizeof(numer));
						if (strcmp(numer, "Q") == 0) { quit = 0; break; }
						if (strcmp(numer, "n") == 0 || strcmp(numer, "no") == 0) {
							getLine("Input formula for the second derivative: ", buff_ddeq, sizeof(buff_ddeq));
							if (strstr(buff_ddeq, "Q")) { quit = 0; break; }
							token1 = strtok_r(buff_ddeq, s, &save_ptr2);
							token1 = strtok_r(NULL, s, &save_ptr2);
							if (token1 != NULL) create_function(token1, &d.ddf[0], &d.var, NULL);
							else { printf("Invalid input.\n"); quit = 0; break; }
						}
					}
				}

				if (idx_m > 1) {
					char tls[MAX_BUFF];
					getLine("Enter the desired tolerance for each method (you can write an expression): ", tls, sizeof(tls));
					if (strstr(tls, "Q")) { quit = 0; break; }
					token = strtok_r(tls, s, &save_ptr2);
					for (int i = 0; i < idx_m; i++) {
						if (token != NULL) tols[i] = evaluate_input(token, &d.var);
						else { printf("Invalid input.\n"); quit = 0; break; }
						token = strtok_r(NULL, s, &save_ptr2);
					}
				} else tols[0] = d.tol;

				char multi[10];
				getLine("Do you want to find all the roots in a range? ", multi, sizeof(multi));
				if (strstr(multi, "Q")) { quit = 0; break; }
				if (strstr(multi, "y") || strstr(multi, "yes")) {
					char range[MAX_BUFF];
					getLine("Enter the end points for the range (you can use a math expression): ", range, sizeof(range));
					if (strstr(range, "Q")) { quit = 0; break; }
					token = strtok_r(range, s, &save_ptr2);
					if (token != NULL) d.A = evaluate_input(token, &d.var);
					else { printf("Invalid input.\n"); quit = 0; break; }
					token = strtok_r(NULL, s, &save_ptr2);
					if (token != NULL) d.B = evaluate_input(token, &d.var);
					else { printf("Invalid input.\n"); quit =0; break; }
					char space[MAX_BUFF];
					getLine("Enter spacing between points (you can use a math expression): ", space, sizeof(space));
					if (strstr(space, "Q")) { quit = 0; break; }
					token = strtok_r(space, s, &save_ptr2);
					if (token != NULL) d.dist = evaluate_input(token, &d.var);
					else { printf("Invalid input.\n"); quit = 0; break; }
					if (d.var.len > 1) {
						char var[5];
						getLine("Which variable are you solving for: ", var, sizeof(var));
						if (strstr(var, "Q")) { quit = 0; break; }
						token = strtok_r(var, s, &save_ptr2);
						char v;
						if (token != NULL) v = token[0];
						else { printf("Invalid input.\n"); quit = 0; break; }
						printf("Enter initial value for the other variables: ");
						get_init_values(NULL, buff_var, &d, sizeof(buff_var));
						if (strstr(buff_var, "Q")) { quit = 0; break; }
						for (int i = 0; i < d.var.len; i++) {
							VAR v0 = get_by_idx(&d.init[0], i)->data.v;
							update_var(&d.init[1], v0.name, v0.value);
						}
						printf("Running method %s:\n", name);
						test_multiple_roots_method(&d, v, methods, idx_m, tols, name);
					} else {
						printf("Running method %s:\n", name);
						test_multiple_roots_method(&d, first(&d.var)->data.v.name, methods, idx_m, tols, name);
					}
				} else {
					get_init_values("Enter the initial value(s) (two are needed for Bisection, False-Position, and Secant): ",
						        buff_var, &d, sizeof(buff_var));
					if (strstr(buff_var, "Q")) { quit = 0; break; }

					if (d.var.len > 1) {
						char var[5];
						getLine("Which variable are you solving for: ", var, sizeof(var));
						if (strstr(var, "Q")) { quit = 0; break; }
						token = strtok_r(var, s, &save_ptr2);
						char v;
						if (token != NULL) v = token[0];
						else { printf("Invalid input.\n"); quit = 0; break; }
						printf("Running method %s:\n", name);
						test_method(&d, v, methods, idx_m, tols, name);
					} else {
						printf("Running method %s:\n", name);
						test_method(&d, first(&d.var)->data.v.name, methods, idx_m, tols, name);
					}
				}
				token = strtok_r(NULL, ",", &save_ptr1);
				d.iter = 0;
				empty(&d.x); empty(&d.f_vals); empty(&d.roots);
				empty(&d.df[0]); empty(&d.ddf[0]);
			}
			if (quit == 0) { free_df(d.df, d.var.len); break; }
		} else if (idx > 1) {
			matrix x = create_matrix(MAX_METH, d.var.len); double elapsedTimes[MAX_METH]; int iters[MAX_METH];
			char init[10];
			printf("If you don't know a good initial point then steepest descent (starting from 0) will try to find an initial point.\n");
			getLine("Do you know a good initial point? ", init, sizeof(init));
			if (strstr(init, "Q")) { free_matrix(&x); free_df(d.df, d.var.len); break; }
			if (strstr(init, "y") || strstr(init, "yes")) {
				get_init_values("Enter the initial value: ", buff_var, &d, sizeof(buff_var));
				if (strstr(buff_var, "Q")) { free_matrix(&x); free_df(d.df, d.var.len); break; }
			} else if (strcmp(init, "n") == 0 || strcmp(init, "no") == 0) {
				list *g = malloc(sizeof(*g)); mem_init_list(g);
				number *x_init = x_init_finder(g, &d, idx, NULL, 2.0L*N, 0.00001);
				update_vars(&d.init[0], x_init);
				empty(g); free(g); free(x_init); d.iter = 0;
			}

			char numer[10];
			getLine("Do you want numeric differentiation for the partial derivatives? ", numer, sizeof(numer));
			if (strstr(numer, "Q")) { free_matrix(&x); free_df(d.df, d.var.len); break; }
			if (strstr(numer, "n") || strstr(numer, "no")) {
				memset(numer, 0, sizeof(numer));
				for (int i = 0; i < idx; i++) {
					if (quit == 0) { free_matrix(&x); free_df(d.df, d.var.len); break; }
					printf("Do you want numeric differentiation for function %d? ", i+1);
					getLine(NULL, numer, sizeof(numer));
					if (strstr(numer, "Q")) { quit = 0; break; }
					if (strstr(numer, "n") || strstr(numer, "no")) {
						memset(numer, 0, sizeof(numer));
						for (int j = 0; j < d.var.len; j++) {
							printf("Do you want numeric differentiation with respect to %c? ", get_by_idx(&d.var, j)->data.v.name);
							getLine(NULL, numer, sizeof(numer));
							if (strstr(numer, "Q")) { quit = 0; break; }
							if (strstr(numer, "n") || strstr(numer, "no")) {
								printf("Input the derivative with respect to %c: ", get_by_idx(&d.var, j)->data.v.name);
								getLine(NULL, buff_deq, sizeof(buff_deq));
								if (strstr(buff_deq, "Q")) { quit = 0; break; }
								token = strtok_r(buff_deq, s, &save_ptr1);
								token = strtok_r(NULL, s, &save_ptr1);
								if (token != NULL) create_function(token, &d.df[i*d.var.len+j], &d.var, NULL);
								else { printf("Invalid input.\n"); quit = 0; break; }
							}
							memset(buff_deq, 0, sizeof(buff_deq));
							memset(numer, 0, sizeof(numer));
						}
					}
				}
			}

			if (quit == 0) { free_matrix(&x); free_df(d.df, d.var.len); break; }

			printf("If you enter multiple methods methods all those methods will be applied to the inputed system successively.\n");
			getLine("Enter method (Fixed-Point, Newton, Broyden, Steepest-Descent): ", buff_meth, sizeof(buff_meth));
			if (strstr(buff_meth, "Q")) { free_matrix(&x); free_df(d.df, d.var.len); break; }

			char names[MAX_METH][20]; int m_idx = 0;

			token = strtok_r(buff_meth, s, &save_ptr1);
			while (token != NULL) {
				strcpy(names[m_idx], token); number *x_temp;
				if (strcmp(token, "Fixed-Point") == 0) {
					char g[MAX_BUFF];
					getLine("Enter the fixed point formulas (one for each variable): ", g, sizeof(g));
					if (strstr(g, "Q")) { quit = 0; break; }
					token1 = strtok_r(g, s, &save_ptr2);
					int j = 0;
					while (token1 != NULL) {
						create_function(strtok_r(NULL, s, &save_ptr2), &d.f[idx+j++], &d.var, NULL);
						token1 = strtok_r(NULL, s, &save_ptr2);
					}
					clock_t start = clock();
					x_temp = fixed_point_system(&d, idx);
					elapsedTimes[m_idx] = clock() - start;
				} else if (strcmp(token, "Newton") == 0) {
					clock_t start = clock();
					x_temp = newton_system(&d, NULL, idx);
					elapsedTimes[m_idx] = clock() - start;
				} else if (strcmp(token, "Broyden") == 0) {
					clock_t start = clock();
					x_temp = broyden_method(&d, idx);
					elapsedTimes[m_idx] = clock() - start;
				} else if (strcmp(token, "Steepest-Descent") == 0) {
					int maxIt = N;
					list *g = malloc(sizeof(*g)); mem_init_list(g);
					clock_t start = clock();
					x_temp = x_init_finder(g, &d, idx, NULL, maxIt, d.tol);
					while (!(vec_norm(evaluate_vector_functions(d.f, idx, &d.var, x_temp), d.var.len) < d.tol)) {
						maxIt += N;
						number *x_prev = malloc(d.var.len*sizeof(*x_prev));
						memcpy(x_prev, x_temp, d.var.len*sizeof(*x_prev));
						x_temp = x_init_finder(g, &d, idx, x_temp, maxIt, d.tol);
						number *cmp = malloc(d.var.len*sizeof(*cmp));
						for (int i = 0; i < d.var.len; i++) cmp[i] = x_temp[i] - x_prev[i];
						if (vec_norm(cmp, d.var.len) < d.tol) { free(x_prev); free(cmp); break; }
						free(x_prev); free(cmp);
					}
					elapsedTimes[m_idx] = clock() - start;
					empty(g);
					free(g);
				}
				number *vals = malloc(d.var.len*3*sizeof(*vals));
				for (int i = 0; i < d.var.len; i++) {
					vals[i*3] = m_idx;
					vals[i*3+1] = i;
					vals[i*3+2] = x_temp[i];
				}
				free(x_temp);
				edit_matrix(&x, vals, d.var.len); free(vals);
				iters[m_idx] = d.iter;
				token = strtok_r(NULL, s, &save_ptr1); m_idx++; d.iter = 0;
			}
			if (quit == 0) { free_matrix(&x); free_df(d.df, d.var.len); break; }

			for (int i = 0; i < m_idx; i++) {
				printf("Method: %s\n", names[i]);
				if (vec_norm(evaluate_vector_functions(d.f, idx, &d.var, x.data[i]), d.var.len) < 1e-8) {
					printf("A root of the nonlinear system is: ");
					for (int j = 0; j < d.var.len; j++) printf("%.10Lf ", x.data[i][j]);
					printf("\n");
					printf("It took %d iterations to find the root.\n", iters[i]);
					printf("It took %f seconds to find the root.\n", elapsedTimes[i] / (double) CLOCKS_PER_SEC);
				} else printf("No root was found for the system.\n");
			}

			free_matrix(&x);
		}

		for (int i = idx; i < 2*MAX_FUNC; i++) empty(&d.f[i]);
		free_df(d.df, d.var.len);
		for (int i = 0; i < MAX_FUNC; i++) empty(&d.ddf[i]);
		empty(&d.x); empty(&d.f_vals); empty(&d.roots);
		d.iter = 0;

		char ques[10];
		getLine("Do you want to reuse the inputted functions? ", ques, sizeof(ques));
		if (strstr(ques, "Q")) break;
		if (strstr(ques, "n") || strstr(ques, "no")) {
			invalid_eq = NO_INPUT;
			for (int i = 0; i < 2*MAX_FUNC; i++) empty(&d.f[i]);
			empty(&d.var);
		}

		if (invalid_eq == OK) {
			char ques[10];
			getLine("Would you like to enter more functions? ", ques, sizeof(ques));
			if (strstr(ques, "Q")) break;
			if (strstr(ques, "y") || strstr(ques, "yes")) {
				getLine("Enter function(s): ", buff_eq, sizeof(buff_eq));
				if (strstr(buff_eq, "Q")) break;
				token = strtok_r(buff_eq, s, &save_ptr1);
				while (token != NULL) {
					token = strtok_r(NULL, s, &save_ptr1);
					if (token != NULL)
						create_function(token, &d.f[idx++], &d.var, d.init);
					token = strtok_r(NULL, s, &save_ptr1);
				}
			}
		}
	}

	for (int i = 0; i < 2*MAX_FUNC; i++) empty(&d.f[i]);
	free(d.f);
	for (int i = 0; i < MAX_FUNC; i++) empty(&d.ddf[i]);
	free(d.ddf);
	empty(&d.var);
	empty(&d.x); empty(&d.f_vals); empty(&d.roots);
	empty(&d.init[0]); empty(&d.init[1]);
	free(d.init);

	empty(&funcs); empty(&consts); empty(&ops);

	return 0;
}
