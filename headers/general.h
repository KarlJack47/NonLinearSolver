#ifndef __GENERAL_H__
#define __GENERAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define __USE_MATH_DEFINES
#define __USE_GNU
#include <math.h>
#include <float.h>
#include <string.h>
#include "derivatives.h"

#define TOL FLT_EPSILON
#define N 100
#define MAX_BUFF 512
#define MAX_FUNC 20
#define MAX_METH 10
#define NCONST 2
#define NOP 5
#define NFUNC 38

typedef long double number;
typedef struct Data data;

typedef number (*method)(data*, char);
typedef number (*fn)(number);
typedef number (*fn1)(int, number);
typedef number (*fn2)(number, number);

typedef struct Constant CONST;
typedef struct Literal LIT;
typedef struct Variable VAR;
typedef struct Operator OP;
typedef struct Function FUNC;
typedef struct Parenthesis PAREN;
typedef struct Separator SEP;
typedef struct Token TOK;

typedef struct DataType data_type;
typedef struct Node node;
typedef struct DoublyLinkedList list;

struct Token {
	char type[MAX_BUFF];
	char value[MAX_BUFF];
};

struct Constant {
	char name; // name of the constant like P (single capital letter)
	number value; // number value associated to the constant
};

struct Literal {
	number value; // number
};

struct Variable {
	char name; // variable name
	number value; // data value for the variable
};

struct Operator {
	char type; // +, -, *, /, ^
	int prec; // +, -: 2, *, /: 3, ^: 4
	char asso; // L: left associative, R: right associative
};

struct Function {
	char name[MAX_BUFF]; // special function name
	fn eval; // function with one arg
	fn deval; // derivative of fn
	fn1 eval1; // function with one flt and one int arg
	fn1 deval1; // derivative of fn1
	fn2 eval2; // function with two args
	fn2 deval2_1; // derivative of fn2 with respect to first arg
	fn2 deval2_2; // derivative of fn2 with respect to second arg
	int one_arg; // 1: an integer argument, 0: no integer argument
	int n_args; // number of function arguments
};

struct Parenthesis {
	char type; // (, ), [, ], {, }
	char dir; // L = Left, R = Right
};

struct Separator {
	char type; // ',', ';', ':'
};

int numDigits(number x) {
        x = fabsl(x);
        return (x < 10 ? 1 :
               (x < 100 ? 2 :
               (x < 1000 ? 3 :
               (x < 10000 ? 4 :
               (x < 100000 ? 5 :
               (x < 1000000 ? 6 :
               (x < 10000000 ? 7 :
               (x < 100000000 ? 8 :
               (x < 1000000000 ? 9 :
                10)))))))));
}

#ifndef __DOUBLY_LINKED_LIST_H__
#include "doubly_linked_list.h"
data_type valid_const[NCONST] = {{ .t={ "Constant", 'P' }, .c={ 'P', M_PIl } }, { .t={ "Constant", 'E' }, .c={ 'E', M_El } }};
data_type valid_op[NOP] = {{ .t={ "Operator", '+' }, .o={ '+', 2, 'L' } }, { .t={ "Operator", '-' }, .o={ '-', 2, 'L' } },
			   { .t={ "Operator", '*' }, .o={ '*', 3, 'L' } }, { .t={ "Operator", '/' }, .o={ '/', 3, 'L' } },
			   { .t={ "Operator", '^' }, .o={ '^', 4, 'R' } }};
data_type valid_func[NFUNC] = {{ .t={ "Function", "max" }, .f={ .name="max", .eval2=&fmaxl, .deval2_1=&dfmax_xl, .deval2_2=&dfmax_yl, .n_args=2 } },
			       { .t={ "Function", "min" }, .f={ .name="min", .eval2=&fminl, .deval2_1=&dfmin_xl, .deval2_2=&dfmin_yl, .n_args=2 } },
			       { .t={ "Function", "ceil" }, .f={ .name="ceil", .eval=&ceill, .deval=&dceill, .n_args=1 } },
			       { .t={ "Function", "floor" }, .f={ .name="floor", .eval=&floorl, .deval=&dfloorl, .n_args=1 } },
			       { .t={ "Function", "mod" }, .f={ .name="mod", .eval2=&fmodl, .deval2_1=&dfmod_xl, .deval2_2=&dfmod_yl, .n_args=2 } },
			       { .t={ "Function", "pow" }, .f={ .name="pow", .eval2=&powl, .deval2_1=&dpowl, .n_args=2 } },
			       { .t={ "Function", "sqrt" }, .f={ .name="sqrt", .eval=&sqrtl, .deval=&dsqrtl, .n_args=1 } },
			       { .t={ "Function", "cbrt" }, .f={ .name="cbrt", .eval=&cbrtl, .deval=&dcbrtl, .n_args=1 } },
			       { .t={ "Function", "abs" }, .f={ .name="abs", .eval=&fabsl, .deval=&dfabsl, .n_args=1 } },
		               { .t={ "Function", "hypot" }, .f={ .name="hypot", .eval2=&hypotl, .deval2_1=&dhypot_xl, .deval2_2=&dhypot_yl, .n_args=2 } },
			       { .t={ "Function", "sin" }, .f={ .name="sin", .eval=&sinl, .deval=&dsinl, .n_args=1 } },
			       { .t={ "Function", "cos" }, .f={ .name="cos", .eval=&cosl, .deval=&dcosl, .n_args=1 } },
			       { .t={ "Function", "tan" }, .f={ .name="tan", .eval=&tanl, .deval=&dtanl, .n_args=1 } },
			       { .t={ "Function", "asin" }, .f={ .name="asin", .eval=&asinl, .deval=&dasinl, .n_args=1 } },
		               { .t={ "Function", "acos" }, .f={ .name="acos", .eval=&acosl, .deval=&dacosl, .n_args=1 } },
			       { .t={ "Function", "atan" }, .f={ .name="atan", .eval=&atanl, .deval=&datanl, .n_args=1 } },
			       { .t={ "Function", "exp" }, .f={ .name="exp", .eval=&expl, .deval=&dexpl, .n_args=1 } },
			       { .t={ "Function", "exp2" }, .f={ .name="exp2", .eval=&exp2l, .deval=&dexp2l, .n_args=1 } },
			       { .t={ "Function", "log" }, .f={ .name="log", .eval=&logl, .deval=&dlogl, .n_args=1 } },
			       { .t={ "Function", "log2" }, .f={ .name="log2", .eval=&log2l, .deval=&dlog2l, .n_args=1 } },
			       { .t={ "Function", "log10" }, .f={ .name="log10", .eval=&log10l, .deval=&dlog10l, .n_args=1 } },
			       { .t={ "Function", "sinh" }, .f={ .name="sinh", .eval=&sinhl, .deval=&dsinhl, .n_args=1 } },
			       { .t={ "Function", "cosh" }, .f={ .name="cosh", .eval=&coshl, .deval=&dcoshl, .n_args=1 } },
			       { .t={ "Function", "tanh" }, .f={ .name="tanh", .eval=&tanhl, .deval=&dtanhl, .n_args=1 } },
			       { .t= { "Function", "asinh" }, .f={ .name="asinh", .eval=&asinhl, .deval=&dasinhl, .n_args=1 } },
			       { .t={ "Function", "acosh" }, .f={ .name="acosh", .eval=&acoshl, .deval=&dacoshl, .n_args=1 } },
			       { .t={ "Function", "atanh" }, .f={ .name="atanh", .eval=&atanhl, .deval=&datanhl, .n_args=1 } },
			       { .t={ "Function", "gamma" }, .f={ .name="gamma", .eval=&tgammal, .deval=&dtgammal, .n_args=1 } },
			       { .t={ "Function", "lgamma" }, .f={ .name="lgamma", .eval=&lgammal, .deval=&dlgammal, .n_args=1 } },
			       { .t={ "Function", "jn" }, .f={ .name="jn", .eval1=&jnl, .deval1=&djnl, .one_arg=1, .n_args=2 } },
			       { .t={ "Function", "j0" }, .f={ .name="j0", .eval=&j0l, .deval=&dj0l, .n_args=1 } },
			       { .t={ "Function", "j1" }, .f={ .name="j1", .eval=&j1l, .deval=&dj1l, .n_args=1 } },
			       { .t={ "Function", "yn" }, .f={ .name="yn", .eval1=&ynl, .deval1=&dynl, .one_arg=1, .n_args=2 } },
			       { .t={ "Function", "y0" }, .f={ .name="y0", .eval=&y0l, .deval=&dy0l, .n_args=1 } },
			       { .t={ "Function", "y1" }, .f={ .name="y1", .eval=&y1l, .deval=&dy1l, .n_args=1} },
			       { .t={ "Function", "erf" }, .f={ .name="erf", .eval=&erfl, .deval=&derfl, .n_args=1 } },
			       { .t={ "Function", "erfc" }, .f={ .name="erfc", .eval=&erfcl, .deval=&derfcl, .n_args=1 } }};
list consts;
list ops;
list funcs;
#endif

#ifndef __TOKENIZER_H__
#include "tokenizer.h"
#endif

#ifndef __PARSER_H__
#include "parser.h"
#endif

number get_var(list *var, char v_name) {
	if (var->len == 0) return NAN;
	for (int i = 0; i < var->len; i++) {
		VAR v = get_by_idx(var, i)->data.v;
		if (v_name == v.name) return v.value;
	}
	return NAN;
}

void update_var( list *V, char v, number val ) {
	node *start = V->head;
	while (start != NULL) {
		if (start->data.v.name == v) {
			start->data.v.value = val;
			break;
		}
		start = start->next;
	}
}

void update_vars( list *V, number *vals ) {
	for (int i = 0; i < V->len; i++) update_var(V, get_by_idx(V, i)->data.v.name, vals[i]);
}

void get_args(node *n, list *args, int n_args) {
	node *start = n->prev;
	for (int i = 0; i < n_args; i++) {
		if (start != NULL) {
			if (strcmp(start->data.t.type, "Constant") == 0) {
				char num[50];
				snprintf(num, 50, "%Lf", start->data.c.value);
				push(args, (data_type) { .t={ "Literal" }, .l={ start->data.c.value } });
				strcpy(first(args)->data.t.value, num);
			} else if (strcmp(start->data.t.type, "Variable") == 0 || strcmp(start->data.t.type, "Literal") == 0)
				push(args, start->data);
			start = start->prev;
		}
	}
}

number evaluate(list *rpn, list *var) {
	number result = NAN;
	list args; mem_init_list(&args);
	list temp; mem_init_list(&temp);
	node *start = rpn->tail;
	while (start != rpn->head) { push(&temp, start->data); start = start->prev; }
	push(&temp, start->data);
	while (start != NULL) {
		if (strcmp(start->data.t.type, "Function") == 0 || strcmp(start->data.t.type, "Operator") == 0) {
			if (start->data.f.n_args == 1 && strcmp(start->data.t.type, "Operator") != 0) {
				get_args(start, &args, 1);
				number arg1;
				if (strcmp(first(&args)->data.t.type, "Variable") == 0)
					arg1 = get_var(var, first(&args)->data.v.name);
				else
					arg1 = first(&args)->data.l.value;
				result = start->data.f.eval(arg1);
				char num[50] = { '\0' };
				snprintf(num, 50, "%Lf", result);
				free(remove_node(rpn, start->prev));
				node *old_start = start;
				start = start->next;
				free(remove_node(rpn, old_start));
				list t; mem_init_list(&t);
				while (isEmpty(rpn) == 1 && first(rpn) != start) {
					push(&t, first(rpn)->data); free(pop_first(rpn));
				}
				push(rpn, (data_type) { .t={ "Literal" }, .l={ result } });
				strcpy(first(rpn)->data.t.value, num);
				while (isEmpty(&t) == 1) { push(rpn, first(&t)->data); free(pop_first(&t)); }
			} else if (start->data.f.n_args == 2 || strcmp(start->data.t.type, "Operator") == 0) {
				get_args(start, &args, 2);
				number arg[2];
				int check = 0;
				for (int i = 0; i < 2; i++) {
					data_type curr = get_by_idx(&args, i)->data;
					if (strcmp(curr.t.type, "Variable") == 0)
						arg[i] = get_var(var, curr.v.name);
					else if (strcmp(curr.t.type, "Literal") == 0)
						arg[i] = curr.l.value;
				}
				if (start->data.f.one_arg == 1 && strcmp(start->data.t.type, "Operator") != 0) result = start->data.f.eval1((int) arg[0], arg[1]);
				else if (start->data.f.one_arg == 0 && strcmp(start->data.t.type, "Operator") != 0) result = start->data.f.eval2(arg[0], arg[1]);
				else if (strcmp(start->data.t.type, "Operator") == 0) {
					if (start->data.o.type == '+') result = arg[0] + arg[1];
					else if (start->data.o.type == '-') result = arg[0] - arg[1];
					else if (start->data.o.type == '*') result = arg[0] * arg[1];
					else if (start->data.o.type == '/') result = arg[0] / arg[1];
					else if (start->data.o.type == '^') result = powl(arg[0], arg[1]);
				}
				char num[50] = { '\0' };
				snprintf(num, 50, "%Lf", result);
				if (start->prev->prev != NULL) free(remove_node(rpn, start->prev->prev));
				free(remove_node(rpn, start->prev));
				node *old_start = start;
				start = start->next;
				free(remove_node(rpn, old_start));
				list t; mem_init_list(&t);
				while (isEmpty(rpn) == 1 && first(rpn) != start) {
					push(&t, first(rpn)->data); free(pop_first(rpn));
				}
				push(rpn, (data_type) { .t={ "Literal" }, .l={ result } });
				strcpy(first(rpn)->data.t.value, num);
				while (isEmpty(&t) == 1) { push(rpn, first(&t)->data); free(pop_first(&t)); }

			}
		} else { start = start->next; }
		empty(&args);
	}

	if (strcmp(first(rpn)->data.t.type, "Constant") == 0) result = first(rpn)->data.c.value;
	else if (strcmp(first(rpn)->data.t.type, "Literal") == 0) result = first(rpn)->data.l.value;
	else if (strcmp(first(rpn)->data.t.type, "Variable") == 0) result = get_var(var, first(rpn)->data.v.name);
	empty(rpn);

	while (isEmpty(&temp) == 1) { append(rpn, first(&temp)->data); free(pop_first(&temp)); }

	return result;
}

number evaluate_function( list *f, list *var, number *vals ) {
	update_vars(var, vals);

	return evaluate(f, var);
}

number differentiate( list *f, list *var, char v, number *vals, int order ) {
	number f_pt[9], *coeff = malloc(9*sizeof(*coeff));
	number first_order_coeff[9] = { 1.0L/280.0L, -4.0L/105.0L, 1.0L/5.0L, -4.0L/5.0L,
					0.0L, 4.0L/5.0L, -1.0L/5.0L, 4.0L/105.0L, -1.0L/280.0L };
	number second_order_coeff[9] = { -1.0L/560.0L, 8.0L/315.0L, -1.0L/5.0L, 8.0L/5.0L, -205.0L/72.0L,
					8.0L/5.0L, -1.0L/5.0L, 8.0L/315.0L, -1.0L/560.0L };
	update_vars(var, vals);
	number orig = get_var(var, v);
	number result = 0.0L;
	number h;
	if (order == 1) { h = sqrtl(FLT_EPSILON)*orig; memcpy(coeff, first_order_coeff, 9*sizeof(*coeff)); }
	else if (order == 2) { h = sqrt(LDBL_EPSILON*4096.0L)*orig; memcpy(coeff, second_order_coeff, 9*sizeof(*coeff)); }
	if (fabsl(orig) < LDBL_EPSILON) h = 0.01L;
	for (int i = 0; i < 9; i++) {
		volatile number xph = orig - (5.0L - ((number) i + 1.0L)) * h;
		update_var(var, v, xph);
		result += coeff[i]*evaluate(f, var);
	}
	free(coeff);
	return result / powl(h, (number) order);
}

void create_function( char *expr, list *f, list *var, list *init ) {
	list toks; mem_init_list(&toks);
	tokenize(expr, &toks);
	parse(&toks, f);
	empty(&toks);
	node *start = f->head;
	while (start != NULL) {
		if (strcmp(start->data.t.type, "Variable") == 0) {
			if (isEmpty(var) == 1) {
				data_type d; int check = 1;
				for (int i = 0; i < var->len; i++) {
					d = get_by_idx(var, i)->data;
					if (start->data.v.name == d.v.name) {
						check = 0;
						break;
					}
				}
				if (check == 1) {
					append(var, start->data);
					if (init != NULL) {
						append(&init[0], start->data);
						append(&init[1], start->data);
					}
				}
			} else {
				append(var, start->data);
				if (init != NULL) {
					append(&init[0], start->data);
					append(&init[1], start->data);
				}
			}
		}
		start = start->next;
	}
}

struct Data {
	int iter, accel;
	list x, f_vals, roots;
	number tol;
	number A, B, dist;
	list *f, *df, *ddf;
	list var;
	list *init;
};

#ifndef __LINEAR_ALGEBRA_H__
#include "linear_algebra.h"
#endif

#ifndef __METHODS_H__
int stopping_condition( number curr, number f_curr, number prev, number tol ) {
        if (fabsl(f_curr) < tol || fabsl(curr - prev) < tol) return 0;
        else return 1;
}
#include "methods.h"
void print_results( data *d, char v ) {
        printf("---------------------------------------------------\n");
        printf("|          %c_n           |         f(%c_n)         |\n", v, v);
        printf("---------------------------------------------------\n");
	node *x = d->x.head;
	node *f_vals = d->f_vals.head;
        while (x != NULL) {
                char spaces_x[11] = { '\0' };
                char spaces_f[11] = { '\0' };
                int num_spaces_x = 10 - numDigits(x->data.v.value);
                int num_spaces_f = 10 - numDigits(f_vals->data.l.value);
                if (x->data.v.value >= (number) 0) num_spaces_x++;
                if (f_vals->data.l.value >= (number) 0) num_spaces_f++;
                for (int j = 0; j < num_spaces_x; j++)
                        spaces_x[j] = ' ';
                for (int j = 0; j < num_spaces_f; j++)
                        spaces_f[j] = ' ';
                printf("| %s%.10Lf | %s%.10Lf |\n", spaces_x, x->data.v.value, spaces_f, f_vals->data.l.value);
                printf("---------------------------------------------------\n");

		x = x->next;
		f_vals = f_vals->next;
        }

        printf("A root of the function is %.10Lf.\n", last(&d->x)->data.v.value);
}

void test_method( data *d, char v, method *methods, int n, number *tols, char *method_name ) {
	d->iter = 0;
	clock_t start, end;
	start = clock();
	number root = hybrid(d, v, methods, n, tols);
	end = clock();
	printf("%s\n", method_name);
	if (isnan(root))
		printf("No root was found.\n");
	else {
		print_results(d, v);
		printf("It took %d steps to find the root.\n", d->iter);
		printf("It took %f seconds to find the root.\n", (end - start) / (double) CLOCKS_PER_SEC);
	}
	printf("\n");
	empty(&d->x); empty(&d->f_vals);
}

void test_multiple_roots_method( data *d, char v, method *methods, int n, number *tols, char *method_name ) {
	printf("Multiple Roots: %s\n", method_name);
	d->iter = 0;
	clock_t start = clock();
	int check = multiple_roots(d, v, methods, n, tols);
	clock_t end = clock();
	if (check == -1) printf("There are no roots in the interval [%Lf, %Lf].\n", d->A, d->B);
	else printf("It took %f seconds to find the roots in the interval [%Lf, %Lf].\n", (end - start) / (double) CLOCKS_PER_SEC, d->A, d->B);
	printf("\n");
	empty(&d->x); empty(&d->f_vals); empty(&d->roots);
}
#endif

/* Input Utility Functions */
#define OK 0
#define NO_INPUT 1
#define TOO_LONG 2
static int getLine( char *prmpt, char *buff, size_t sz ) {
	int ch, extra;

	// Get line with buffer overrun protection
	if (prmpt != NULL) {
		printf("%s", prmpt);
		fflush(stdout);
	} else fflush(stdout);
	if (fgets(buff, sz, stdin) == NULL || strcmp(buff, "\n") == 0)
		return NO_INPUT;

	// If it was too long, there'll be no newline.
	// In that case, we flush to end of line so that
	// exces doesn't affect the next call.
	if (buff[strlen(buff)-1] != '\n') {
		extra = 0;
		while ((ch = getchar() != '\n') && (ch != EOF))
			extra = 1;
		return (extra == 1) ? TOO_LONG : OK;
	}

	// Otherwise remove newline and give string back to caller
	buff[strlen(buff)-1] = '\0';
	return OK;
}

number evaluate_input( char *input, list *var ) {
	list t_tok; list t_res;
	mem_init_list(&t_tok); mem_init_list(&t_res);
	tokenize(input, &t_tok);
	parse(&t_tok, &t_res);
	empty(&t_tok);
	number result = evaluate(&t_res, var);
	empty(&t_res);

	return result;
}

void get_init_values( char *prmpt, char *buff_var, data *d, size_t sz ) {
	getLine(prmpt, buff_var, sz);

	if (strstr(buff_var, "Q")) return;

	char s[4] = ":= ";
	char* token = strtok(buff_var, s);
	int j = 0;
	while (token != NULL) {
		char v = token[0];
		token = strtok(NULL, s);
		if (token != NULL) {
			update_var(&d->init[j], v, evaluate_input(token, &d->var));
			if (last(&d->var)->data.v.name == v) j++;
		}
		token = strtok(NULL, s);
	}
}

void free_df( list *df, int n_var ) {
	for (int i = 0; i < n_var*MAX_FUNC; i++) empty(&df[i]);
	free(df);
}

#endif // __GENERAL_H__
