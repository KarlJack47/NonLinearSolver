#ifndef __PARSER_H__
#define __PARSER_H__

#include "general.h"

void parse(list *toks, list *result) {
	list opStack; mem_init_list(&opStack);

	for (int i = 0; i < toks->len; i++) {
		data_type t = get_by_idx(toks, i)->data;

		// If the token is a number then push it to the output queue
		if (strcmp(t.t.type, "Constant") == 0 || strcmp(t.t.type, "Literal") == 0 || strcmp(t.t.type, "Variable") == 0)
			append(result, t);
		// If the token is a function token then push it onto the stack
		else if (strcmp(t.t.type, "Function") == 0)
			push(&opStack, t);
		// If the token is a function argument separator
		else if (strcmp(t.t.type, "Function Argument Separator") == 0) {
			// Until the token at the top of the stack is a left
			// parenthesis pop operators off the stack onto the output queue
			while (first(&opStack) != NULL && strcmp(first(&opStack)->data.t.type, "Left Parenthesis") != 0) {
				append(result, first(&opStack)->data);
				free(pop_first(&opStack));
			}
		}
		// If the token is an operator, o1, then:
		else if (strcmp(t.t.type, "Operator") == 0) {
			// While there is an operator o2, at the top of the operator stack and either
			while (first(&opStack) != NULL && strcmp(first(&opStack)->data.t.type, "Operator") == 0
			       // o1 is left-associate and its precedence is less than or equal to that of o2, or
			       && ((t.o.asso == 'L' && t.o.prec <= first(&opStack)->data.o.prec)
			       // o1 is right associative, and has precdence less than that of o2,
			       || (t.o.asso == 'R' && t.o.prec < first(&opStack)->data.o.prec))) {
				append(result, first(&opStack)->data);
				free(pop_first(&opStack));
			}
			push(&opStack, t);
		}
		// If the token is a left parenthesis then push it onto the stack.
		else if (strcmp(t.t.type, "Left Parenthesis") == 0)
			push(&opStack, t);
		// If the token is a right parenthesis
		else if (strcmp(t.t.type, "Right Parenthesis") == 0) {
			// Until the token at the top of the stack is a left parenthesis, pop
			// operators off the stack onto the output queue
			while (first(&opStack) != NULL && strcmp(first(&opStack)->data.t.type, "Left Parenthesis") != 0) {
				append(result, first(&opStack)->data);
				free(pop_first(&opStack));
			}

			// Pop the left parenthesis from the stack, but not onto the output queue
			free(pop_first(&opStack));

			// If the token at the top of the stack is a function token, pop it onto the output queue
			if (first(&opStack) != NULL && strcmp(first(&opStack)->data.t.type, "Function") == 0) {
				append(result, first(&opStack)->data);
				free(pop_first(&opStack));
			}
		}
	}

	while (isEmpty(&opStack) == 1) {
		append(result, first(&opStack)->data);
		free(pop_first(&opStack));
	}

	empty(&opStack);
}

#endif // __PARSER_H__
