#include "../headers/general.h"

char func[] = "sin(x) + cos(x) * cosh(x)";
char dfunc[] = "cos(x) * (sinh(x) + 1) - sin(x) * cosh(x)";
char ddfunc[] = "-sin(x) * (2 * sinh(x) + 1)";

int main( void ) {
	data d = { .tol=FLT_EPSILON/(number) 512, .A=-15, .B=15, .dist=0.8};
	d.init = malloc(2*sizeof(*d.init));
	mem_init_list(&d.init[0]); mem_init_list(&d.init[1]);
	init_list(&funcs, valid_func, NFUNC); init_list(&consts, valid_const, NCONST); init_list(&ops, valid_op, NOP);
	d.f = malloc(sizeof(*d.f)); d.df = malloc(sizeof(*d.df)); d.ddf = malloc(sizeof(*d.ddf));
	mem_init_list(d.f); mem_init_list(d.df); mem_init_list(d.ddf); mem_init_list(&d.var);
	create_function(func, d.f, &d.var, d.init); create_function(dfunc, d.df, &d.var, d.init); create_function(ddfunc, d.ddf, &d.var, d.init);
	update_var(&d.init[0], first(&d.var)->data.v.name, 0);
	update_var(&d.init[1], first(&d.var)->data.v.name, 2);
	mem_init_list(&d.x); mem_init_list(&d.f_vals); mem_init_list(&d.roots);
	method methods[11][3] = {[0][0]=&bisection,
			    	 [1][0]=&false_position,
			    	 [2][0]=&newton,
			    	 [3][0]=&secant,
			    	 [4][0]=&halley,
			    	 [5][0]=&bisection, [5][1]=&newton,
			    	 [6][0]=&bisection, [6][1]=&secant,
			    	 [7][0]=&bisection, [7][1]=&halley,
			    	 [8][0]=&false_position, [8][1]=&halley,
			   	 [9][0]=&bisection, [9][1]=&false_position, [9][2]=&halley,
			    	 [10][0]=&bisection, [10][1]=&secant, [10][2]=&halley};
	int num_methods[11] = {1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3};
	number tols[11][3] = {[0][0]=d.tol,
			      [1][0]=d.tol,
			      [2][0]=d.tol,
			      [3][0]=d.tol,
			      [4][0]=d.tol,
			      [5][0]=0.1, [5][1]=d.tol,
			      [6][0]=0.1, [6][1]=d.tol,
			      [7][0]=0.1, [7][1]=d.tol,
			      [8][0]=0.1, [8][1]=d.tol,
			      [9][0]=0.1, [9][1]=0.01, [9][2]=d.tol,
			      [10][0]=0.1, [10][1]=0.0001, [10][2]=d.tol};
	char method_names[11][100] = {"Bisection Method",
				      "False Position Method",
				      "Newton's Method",
				      "Secant Method",
				      "Halley's Method",
				      "Newton-Bisection Hybrid",
				      "Secant-Bisection Hybrid",
				      "Halley-Bisection Hybrid",
				      "Halley-False Position Hybrid",
				      "Halley-False Position-Bisection Hybrid",
				      "Halley-Secant-Bisection Hybrid"};

	for (int i = 0; i < 11; i++) {
		test_method(&d, first(&d.var)->data.v.name, methods[i], num_methods[i], tols[i], method_names[i]);
		test_multiple_roots_method(&d, first(&d.var)->data.v.name, methods[i], num_methods[i], tols[i], method_names[i]);
	}

	empty(&funcs); empty(&consts); empty(&ops);
	empty(&d.init[0]); empty(&d.init[1]);
	free(d.init);
	empty(d.f); empty(d.df); empty(d.ddf); empty(&d.var);
	free(d.f); free(d.df); free(d.ddf);
	empty(&d.x); empty(&d.f_vals); empty(&d.roots);

	return 0;
}
