#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include "general.h"

int isDigit(char c) { return ((int) c >= 48 && (int) c <= 57) ? 0 : 1; }
int isLowerCase(char c) { return ((int) c >= 97 && (int) c <= 122) ? 0 : 1; }
int isUpperCase(char c) { return ((int) c >= 65 && (int) c <= 90) ? 0 : 1; }
int isLetter(char c) { return (isUpperCase(c) == 0 || isLowerCase(c) == 0) ? 0 : 1; }
int isLeftParenthesis(char c) { return (c == '(' || c == '{' || c == '[') ? 0 : 1; }
int isRightParenthesis(char c) { return (c == ')' || c == '}' || c == ']') ? 0 : 1; }
int isParenthesis(char c) { return (isLeftParenthesis(c) == 0|| isRightParenthesis(c) == 0) ? 0 : 1; }
int isOperator(char c) {
	for (int i = 0; i < ops.len; i++)
		if (c == get_by_idx(&ops, i)->data.o.type) return 0;
	return 1;
}

void append_tok( list *result, char *type, char *value ) {
	TOK t;
	strcpy(t.type, type); strcpy(t.value, value);
	if (strcmp(type, "Constant") == 0) {
		for (int i = 0; i < consts.len; i++) {
			CONST c = get_by_idx(&consts, i)->data.c;
			if (t.value[0] == c.name) { append(result, (data_type) { .t=t, .c=c }); break; }
		}
	} else if (strcmp(type, "Unknown Constant") == 0)
		append(result, (data_type) { .t=t, .c={ t.value[0], NAN } });
	else if (strcmp(type, "Literal") == 0)
		append(result, (data_type) { .t=t, .l={ strtold(t.value, NULL) } });
	else if (strcmp(type, "Variable") == 0)
		append(result, (data_type) { .t=t, .v={ t.value[0], NAN } });
	else if (strcmp(type, "Operator") == 0) {
		for (int i = 0; i < ops.len; i++) {
			OP o = get_by_idx(&ops, i)->data.o;
			if (t.value[0] == o.type) { append(result, (data_type) { .t=t, .o=o }); break; }
		}
	} else if (strcmp(type, "Unknown Operator") == 0)
		append(result, (data_type) { .t=t, .o={ t.value[0], '\0', 0 } });
	else if (strcmp(type, "Function") == 0) {
		for (int i = 0; i < funcs.len; i++) {
			FUNC f = get_by_idx(&funcs, i)->data.f;
			if (strcmp(t.value, f.name) == 0) {
				append(result, (data_type) { .t=t, .f=f });
				break;
			}
		}
	} else if (strcmp(type, "Left Parenthesis") == 0)
		append(result, (data_type) { .t=t, .p={ t.value[0], 'L' } });
	else if (strcmp(type, "Right Parenthesis") == 0)
		append(result, (data_type) { .t=t, .p={ t.value[0], 'R' } });
	else if (strcmp(type, "Function Argument Separator") == 0)
		append(result, (data_type) { .t=t, .s={ t.value[0] } });
}
int append_lit( list *result, char *expr, int start ) {
	char value[MAX_BUFF];
	int i = 0;
	int check_zero = 0;
	int check_minus = 0;
	if (start > 1 && expr[start-1] == '-' && (isLeftParenthesis(expr[start-2]) == 0 || isOperator(expr[start-2]) == 0 || expr[start-2] == ',')) { check_minus = 1; value[i++] = '-'; }
	else if (start == 1 && expr[start-1] == '-') { check_minus = 1; value[i++] = '-'; }
	for (int j = start; expr[j] == '.' || isDigit(expr[j]) == 0; j++) {
		if (j == start && expr[j] == '0' && isDigit(expr[j+1]) == 1 && expr[j+1] != '.') { value[i++] = expr[j]; break; }
		if (j == start && expr[j] == '0' && expr[j+1] != '.') {
			while (expr[j] == '0') { check_zero++; j++; }
			if (expr[j] == '.') { value[i++] = '0'; check_zero--; }
		}
		value[i++] = expr[j];
	}
	value[i] = '\0';
	append_tok(result, "Literal", value);
	return i+check_zero-check_minus-1;
}
int append_func( list *result, char *expr, int start ) {
	char value[MAX_BUFF];
	int i = 0;
	for (int j = start; isLeftParenthesis(expr[j]) == 1 && isLowerCase(expr[j]) == 0; j++)
		value[i++] = expr[j];
	value[i] = '\0';
	int check = 1;
	for (int j = 0; j < funcs.len; j++) {
		if (strcmp(value, get_by_idx(&funcs, j)->data.f.name) == 0) {
			check = 0;
			break;
		}
	}
	if (check == 0) { append_tok(result, "Function", value); return i-1; }
	else return -1;
}
int append_const( list *result, char c ) {
	int check = -1;
	for (int j = 0; j < consts.len; j++) {
		if (c == get_by_idx(&consts, j)->data.c.name) {
			check = 0;
			break;
		}
	}
	if (check == 0) { append_tok(result, "Constant", (char[]) { c, '\0' }); return 0; }
	else { append_tok(result, "Unknown Constant", (char[]) { c, '\0' }); return -1; }
}
int append_op( list *result, char c ) {
	if (isOperator(c) == 0) { append_tok(result, "Operator", (char[]) { c, '\0' }); return 0; }
	else { append_tok(result, "Unknown Operator", (char[]) { c, '\0' }); return -1; }
}

int implicit_mult( list *result, char *expr, int start, char type ) {
	if (type == 'c') append_const(result, expr[start]);
	else if (type == 'v') append_tok(result, "Variable", (char[]) { expr[start], '\0' });
	else if (type == 'l') start += append_lit(result, expr, start);
	else if (type == 'p') append_tok(result, "Right Parenthesis", (char[]) { expr[start], '\0' });
	if ((isLowerCase(expr[start+1]) == 0 || isUpperCase(expr[start+1]) == 0 || isDigit(expr[start+1]) == 0 || isLeftParenthesis(expr[start+1]) == 0))
		append_tok(result, "Operator", (char[]) { '*', '\0' });

	return start;
}

void tokenize( char *expr, list *result ) {
	// remove white space
	int count = 0;
	for (int i = 0; expr[i]; i++) {
		if (expr[i] != ' ') {
			expr[count++] = expr[i];
		}
	}
	expr[count] = '\0';

	for (int i = 0; expr[i]; i++) {
		if (isLetter(expr[i]) == 0) {
			if (isUpperCase(expr[i]) == 0)
				i = implicit_mult(result, expr, i, 'c');
			else if (isLowerCase(expr[i]) == 0) {
				int check = append_func(result, expr, i);
				if (check == -1) i = implicit_mult(result, expr, i, 'v');
				else i += check;
			}
		} else if (isDigit(expr[i]) == 0)
			i = implicit_mult(result, expr, i, 'l');
		else if (isLeftParenthesis(expr[i]) == 0)
			append_tok(result, "Left Parenthesis", (char[]) { expr[i], '\0' });
		else if (isRightParenthesis(expr[i]) == 0)
			i = implicit_mult(result, expr, i, 'p');
		else if (expr[i] == ',')
			append_tok(result, "Function Argument Separator", (char[]) { ',' , '\0' });
		else {
			if (i > 0 && expr[i] == '-' && isDigit(expr[i+1]) == 0 && (isLeftParenthesis(expr[i-1]) == 0 || isOperator(expr[i-1]) == 0 || expr[i-1] == ',')) continue;
			if (i == 0 && expr[i] == '-' && isDigit(expr[i+1]) == 0) continue;
			if (expr[i] == '-' && (i == 0 || (i > 0 && (isLeftParenthesis(expr[i-1]) == 0 || isOperator(expr[i-1]) == 0 || expr[i-1] == ',')))) {
				append(result, (data_type) { .t={ "Literal" }, .l={ 0.0L } });
				char num[50];
				snprintf(num, 50, "%Lf", 0.0L);
				strcpy(last(result)->data.t.value, num);
			}
			append_op(result, expr[i]);
		}

	}
}

#endif // __TOKENIZER_H__
