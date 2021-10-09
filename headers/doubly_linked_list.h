#ifndef __DOUBLY_LINKED_LIST_H__
#define __DOUBLY_LINKED_LIST_H__

#include "general.h"

typedef struct Token TOK;

struct DataType {
	TOK t;
	CONST c;
	LIT l;
	VAR v;
	OP o;
	FUNC f;
	PAREN p;
	SEP s;
};

struct Node {
	data_type data;
	node *prev;
	node *next;
};

struct DoublyLinkedList {
	node *head;
	node *tail;
	int len;
};

void mem_init_list( list *L ) {
	L->head = NULL;
	L->tail = NULL;
	L->len = 0;
}

void copy_list( list *L_copy, list *L ) {
	L_copy->head = L->head;
	L_copy->tail = L->tail;
	L_copy->len = L->len;
}

void print_list( list *L, char type ) {
	node *start = L->head;
	if (start == NULL) { printf("The list is empty.\n"); return; }
	while (start != NULL) {
		if (type == 't') printf("Type: %s, Value: %s\n", start->data.t.type, start->data.t.value);
		else if (type == 'c') printf("Type: Variable, Name: %c, Value: %Lf\n", start->data.c.name, start->data.c.value);
		else if (type == 'l') printf("Type: Literal, Value: %Lf\n", start->data.l.value);
		else if (type == 'v') printf("Type: Variable, Name: %c, Value: %Lf\n", start->data.v.name, start->data.v.value);
		else if (type == 'o') printf("Type: Operator, OP Type: %c, Associativity: %c, Precedence: %d\n", start->data.o.type, start->data.o.asso, start->data.o.prec);
		else if (type == 'f') printf("Type: Function, Name: %s, # arguments: %d\n", start->data.f.name, start->data.f.n_args);
		else if (type == 'p') printf("Type: Parenthesis, Paren Type: %c, Direction: %c\n", start->data.p.type, start->data.p.dir);
		else if (type == 's') printf("Type: Function Argument Separator, Sep Type: %c\n", start->data.s.type);
		start = start->next;
	}
}

int isEmpty( list *L ) {
	if (L->head == NULL && L->tail == NULL) return 0;
	else return 1;
}

node* first( list *L ) {
	if (isEmpty(L) == 0) return NULL;
	return L->head;
}

node* last( list *L ) {
	if (isEmpty(L) == 0) return NULL;
	return L->tail;
}

int idx_check( list *L, int idx ) {
	if (idx > L->len-1) return -1;
	else if (idx < 0 && (L->len + idx - 1) >= 0) return L->len + idx -1;
	else if (idx < 0 && (L->len + idx - 1) < 0) return -1;
	else return idx;
}

node* get_by_idx( list *L, int idx ) {
	if (isEmpty(L) == 0) return NULL;
	idx = idx_check(L, idx);
	if (idx == -1) return NULL;

	if (idx == 0) return first(L);
	else if (idx == L->len-1) return last(L);

	int mid = L->len / 2;
	node *start; int curr;
	if (idx <= mid) { start = L->head; curr = 0; }
	else { start = L->tail; curr = L->len-1; }
	while (start != NULL) {
		if (idx == curr)
			return start;
		if (idx <= mid) { start = start->next; curr++; }
		else { start = start->prev; curr--; }
	}

	return NULL;
}

void insertBefore( list *L, node *prev, node *new_node ) {
	L->len++;
	new_node->next = prev;
	if (prev->prev == NULL) { new_node->prev = NULL; L->head = new_node; }
	else { new_node->prev = prev->prev; prev->prev->next = new_node; }
	prev->prev = new_node;
}

void insertAfter( list *L, node *prev, node *new_node ) {
	L->len++;
	new_node->prev = prev;
	if (prev->next == NULL) { new_node->next = NULL; L->tail = new_node; }
	else { new_node->next = prev->next; prev->next->prev = new_node; }
	prev->next = new_node;
}

void push( list *L, data_type new_data ) {
	node *new_node = malloc(sizeof(*new_node));
	new_node->data = new_data;
	if (L->head == NULL) {
		L->len++;
		L->head = new_node;
		L->tail = new_node;
		new_node->prev = NULL;
		new_node->next = NULL;
	} else insertBefore(L, L->head, new_node);
}

void append( list *L, data_type new_data ) {
	node *new_node = malloc(sizeof(*new_node));
	new_node->data = new_data;
	if (L->tail == NULL) { push(L, new_data); free(new_node); }
	else { insertAfter(L, L->tail, new_node); }
}

void insert_at_idx( list *L, int idx, data_type new_data, char dir ) {
	idx = idx_check(L, idx);
	if (idx == -1) return;
	if (isEmpty(L) == 0 || idx == 0) { push(L, new_data); return; }
	else if (idx == L->len-1) { append(L, new_data); return; }

	node *new_node = malloc(sizeof(*new_node));
	new_node->data = new_data;

	node *start = get_by_idx(L, idx);

	if (dir == 'b') insertBefore(L, start, new_node);
	else if (dir == 'a') insertAfter(L, start, new_node);
}

node* remove_node( list *L, node *old_node ) {
	L->len--;
	if (old_node->prev == NULL) L->head = old_node->next;
	else old_node->prev->next = old_node->next;
	if (old_node->next == NULL) L->tail = old_node->prev;
	else old_node->next->prev = old_node->prev;

	return old_node;
}

node* pop_first( list *L ) {
	if (isEmpty(L) == 0) return NULL;
	return remove_node(L, L->head);
}

node* pop_last( list *L ) {
	if (isEmpty(L) == 0) return NULL;
	return remove_node(L, L->tail);
}

node* remove_at_idx( list *L, int idx, char dir ) {
	if (isEmpty(L) == 0) return NULL;
	idx = idx_check(L, idx);
	if (idx == -1) return NULL;
	if (idx == 0) { return pop_first(L); }
	else if (idx == L->len-1) { return pop_last(L); }

	node *start = get_by_idx(L, idx);
	if (start == NULL) return NULL;

	if (dir == 'b' && start->prev != NULL) return remove_node(L, start->prev);
	else if (dir == 'a' && start->next != NULL) return remove_node(L, start->next);
	else if (dir == 's') return remove_node(L, start);

	return NULL;
}

void init_list( list *L, data_type *init, int n ) {
	mem_init_list(L);
	for (int i = 0; i < n; i++) append(L, init[i]);
}

void empty( list *L ) {
	while (isEmpty(L) != 0) free(pop_first(L));
}

#endif // __DOUBLY_LINKED_LIST_H__
