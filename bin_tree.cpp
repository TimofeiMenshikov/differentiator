#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "include/bin_tree.h"
#include "include/graph.h"
#include "include/text.h"


static err_t init_bin_tree_dynamic_ptrs(struct Bin_tree_dynamic_ptrs* ptrs_ptr, const ssize_t start_size)
{
	err_t return_code = NO_ERROR;

	printf("start %s\n", __func__);

	ptrs_ptr->ptrs = (void**) calloc(start_size, sizeof(void*));

	if ((ptrs_ptr->ptrs) == NULL)
	{
		return_code |= UNABLE_TO_CALOC_DYNAMIC_PTRS_ARR;
		return return_code;
	}

	ptrs_ptr->size = NULL;
	ptrs_ptr->capacity = start_size;

	return return_code;
}


static err_t bin_tree_dynamic_ptrs_verificator(const struct Bin_tree_dynamic_ptrs* const ptrs_ptr)
{
	err_t return_code = NO_ERROR;

	if (ptrs_ptr->size < 0) return_code |= INVALID_BIN_TREE_DYNAMIC_PTRS_SIZE;
	if (ptrs_ptr->capacity <= 0) return_code |= INVALID_BIN_TREE_DYNAMIC_PTRS_CAPACITY;
	if ((ptrs_ptr->size) > (ptrs_ptr->capacity)) return_code |= SIZE_BIGGER_THAN_CAPACITY_IN_BIN_TREE_DYNAMIC_PTRS;
	if ((ptrs_ptr->ptrs) == NULL) return_code |= BIN_TREE_DYNAMIC_PTRS_IS_NULL;

	for (ssize_t n_ptr = 0; n_ptr < ptrs_ptr->size; n_ptr++)
	{
		if ((ptrs_ptr->ptrs)[n_ptr] == NULL) return_code |= DYNAMIC_PTR_IS_NULL_IN_BIN_TREE_DYNAMIC_PTRS;
	}
	for (ssize_t n_ptr = ptrs_ptr->size; n_ptr < ptrs_ptr->capacity; n_ptr++)
	{
		if ((ptrs_ptr->ptrs)[n_ptr] != NULL) return_code |= MEMORY_LEAK_IN_BIN_TREE_DYNAMIC_PTRS;
	}

	return return_code;
}


static err_t bin_tree_dynamic_ptrs_free(struct Bin_tree_dynamic_ptrs*  ptrs_ptr)
{
	err_t return_code = bin_tree_dynamic_ptrs_verificator(ptrs_ptr);

	printf("before print if error\n");

	PRINT_IF_ERROR()

	printf("after print if error\n");

	for (ssize_t n_ptr = 0; n_ptr < ptrs_ptr->size; n_ptr++)
	{
		printf("before free [%p]\n", (ptrs_ptr->ptrs)[n_ptr]);
		free((ptrs_ptr->ptrs)[n_ptr]);
	}

	free(ptrs_ptr->ptrs);

	ptrs_ptr->ptrs = NULL;
	ptrs_ptr->size = -1;
	ptrs_ptr->capacity = -1;

	return return_code;
}


static err_t bin_tree_dynamic_ptrs_increase(struct Bin_tree_dynamic_ptrs* ptrs_ptr, const ssize_t increase_coef)
{
	err_t return_code = bin_tree_dynamic_ptrs_verificator(ptrs_ptr);

	PRINT_IF_ERROR()

	void** old_ptrs_arr = ptrs_ptr->ptrs;

	ptrs_ptr->ptrs = (void**) realloc(ptrs_ptr->ptrs, ptrs_ptr->capacity * increase_coef * sizeof(void*));

	if (ptrs_ptr->ptrs == NULL)
	{
		return_code |= UNABLE_TO_INCREASE_BIN_TREE_DYNAMIC_PTRS_ARR;
		ptrs_ptr->ptrs = old_ptrs_arr;
	}
	else 
	{
		for (ssize_t n_ptr = ptrs_ptr->capacity; n_ptr < ptrs_ptr->capacity * increase_coef; n_ptr++)
		{
			(ptrs_ptr->ptrs)[n_ptr] = NULL;
		}

		ptrs_ptr->capacity *= increase_coef;
	}

	return return_code;
}


err_t append_new_dynamic_ptr(void* const ptr, struct Bin_tree_dynamic_ptrs* ptrs_ptr)
{
	err_t return_code = bin_tree_dynamic_ptrs_verificator(ptrs_ptr);

	PRINT_IF_ERROR()

	(ptrs_ptr->ptrs)[ptrs_ptr->size] = ptr;
	(ptrs_ptr->size)++;

	if ((ptrs_ptr->size) > (ptrs_ptr->capacity))
	{
		return_code |= bin_tree_dynamic_ptrs_increase(ptrs_ptr, BIN_TREE_DYNAMIC_PTRS_INCREASE_COEF);
	}

	return return_code;
}


struct Bin_tree_elem* init_bin_tree_elem(struct Bin_tree* const bin_tree_ptr, struct Bin_tree_elem* const parent_ptr, const bool left_or_right, const elem_t elem, const enum BinTreeElemTypes type) // left - 0 right - 1
{
	struct Bin_tree_elem* current_elem_ptr = (struct Bin_tree_elem*) calloc(1, sizeof(struct Bin_tree_elem));

	if (parent_ptr != NULL)
	{	
		if (left_or_right == LEFT_CHILD)
		{ 
			parent_ptr->left_child_ptr = current_elem_ptr;
		}
		else 
		{
			parent_ptr->right_child_ptr = current_elem_ptr;
		}

		current_elem_ptr->depth_in_tree = (parent_ptr->depth_in_tree) + 1;
		(current_elem_ptr->id) = 2 * (parent_ptr->id) + left_or_right;
	}
	else 
	{
		return NULL;
	}	

	current_elem_ptr->right_child_ptr = NULL;
	current_elem_ptr->left_child_ptr = NULL;
	current_elem_ptr->elem = elem;
	current_elem_ptr->type = type;

	

	(bin_tree_ptr->tail_ptr)->next_ptr = current_elem_ptr;
	current_elem_ptr->prev_ptr = bin_tree_ptr->tail_ptr;
	current_elem_ptr->next_ptr = NULL;

	bin_tree_ptr->tail_ptr = current_elem_ptr;


	(bin_tree_ptr->size)++;

	return current_elem_ptr;
}


err_t init_bin_tree(struct Bin_tree* const bin_tree_ptr, const elem_t first_elem = NULL, const enum BinTreeElemTypes first_elem_type = NO_TYPE)
{
	err_t return_code = NO_ERROR;

	bin_tree_ptr->root_ptr = (struct Bin_tree_elem*) calloc(1, sizeof(struct Bin_tree_elem));

	(bin_tree_ptr->root_ptr)->right_child_ptr = NULL;
	(bin_tree_ptr->root_ptr)->left_child_ptr = NULL;

	//printf("%s\n", first_elem);

	printf(BIN_TREE_ELEM_PRINTF_SPEC, first_elem);
	(bin_tree_ptr->root_ptr)->elem = first_elem;
	(bin_tree_ptr->root_ptr)->depth_in_tree = 1;

	//printf("%s\n", (bin_tree_ptr->root_ptr)->elem);

	bin_tree_ptr->head_ptr = bin_tree_ptr->root_ptr;
	bin_tree_ptr->tail_ptr = bin_tree_ptr->root_ptr;

	(bin_tree_ptr->root_ptr)->prev_ptr = NULL;
	(bin_tree_ptr->root_ptr)->next_ptr = NULL;

	(bin_tree_ptr->root_ptr)->type = first_elem_type;

	(bin_tree_ptr->root_ptr)->id = 1;

	bin_tree_ptr->size = 1;

	printf("before init_bin_tree_dynamic_ptrs\n");

	return_code |= init_bin_tree_dynamic_ptrs(&(bin_tree_ptr->ptrs), BIN_TREE_DYNAMIC_PTRS_ARR_START_SIZE);

	printf("before return %s", __func__);

	return return_code;
}


err_t bin_tree_elem_verificator(const struct Bin_tree_elem* const elem_ptr, const struct Bin_tree* const bin_tree_ptr)
{
	err_t return_code = NO_ERROR;

	if (elem_ptr == (elem_ptr->left_child_ptr)) 					return_code |= PARENT_AND_LEFT_CHILD_ARE_SAME;
	if (elem_ptr == (elem_ptr->right_child_ptr)) 					return_code |= PARENT_AND_RIGHT_CHILD_ARE_SAME;
	if (((elem_ptr->right_child_ptr) == (elem_ptr->left_child_ptr)) && ((elem_ptr->left_child_ptr) != NULL)) 	return_code |= LEFT_CHILD_AND_RIGHT_CHILD_ARE_SAME;
	if ((elem_ptr->id) <= 0) 										return_code |= INVALID_BIN_TREE_ELEM_ID;
	if ((elem_ptr->depth_in_tree) <= 0)								return_code |= INVALID_BIN_TREE_ELEM_DEPTH;
	//if ((elem_ptr->elem) == NULL) 									return_code |= ELEM_T_IS_NULL;
	if ((elem_ptr->depth_in_tree) > (elem_ptr->id)) 				return_code |= DEPTH_IS_BIGGER_THAN_ID;
	if ((elem_ptr->depth_in_tree) > (bin_tree_ptr->size)) 			return_code |= DEPTH_IS_BIGGER_THAN_BIN_TREE_SIZE;
	if ((elem_ptr->type) == NO_TYPE)								return_code |= BIN_TREE_ELEM_HAS_NO_TYPE;

	if ((elem_ptr->left_child_ptr) != NULL)
	{
		return_code |= bin_tree_elem_verificator(elem_ptr->left_child_ptr, bin_tree_ptr);
	}

	if ((elem_ptr->right_child_ptr) != NULL)
	{
		return_code |= bin_tree_elem_verificator(elem_ptr->right_child_ptr, bin_tree_ptr);
	}

	return return_code;
}


err_t bin_tree_verificator(const struct Bin_tree* const bin_tree_ptr)
{
	err_t return_code = NO_ERROR;

	if ((bin_tree_ptr->root_ptr) == NULL) return_code |= ROOT_PTR_IS_NULL;
	if ((bin_tree_ptr->head_ptr) == NULL) return_code |= HEAD_PTR_IS_NULL;
	if ((bin_tree_ptr->tail_ptr) == NULL) return_code |= TAIL_PTR_IS_NULL;
	if ((bin_tree_ptr->size) <= 0) return_code |= INVALID_BIN_TREE_SIZE;

	return_code |= bin_tree_elem_verificator(bin_tree_ptr->root_ptr, bin_tree_ptr); 

	return_code |= bin_tree_dynamic_ptrs_verificator(&(bin_tree_ptr->ptrs));

	return return_code;
}


err_t print_bin_tree_err(const err_t return_code)
{
	if (return_code & ROOT_PTR_IS_NULL) fprintf(stderr, "root element pointer is NULL\n");
	if (return_code & HEAD_PTR_IS_NULL) fprintf(stderr, "head pointer is NULL\n");
	if (return_code & TAIL_PTR_IS_NULL) fprintf(stderr, "tail pointer is NULL\n");
	if (return_code & INVALID_BIN_TREE_SIZE) fprintf(stderr, "invalid bin tree size\n");

	if (return_code & PARENT_AND_LEFT_CHILD_ARE_SAME) fprintf(stderr, "parent and left child are same. it may cause infinite recursion\n");
	if (return_code & PARENT_AND_RIGHT_CHILD_ARE_SAME) fprintf(stderr, "parent and right child are same. it may cause infinite recursion\n");
	if (return_code & LEFT_CHILD_AND_RIGHT_CHILD_ARE_SAME) fprintf(stderr, "left and right child are same. Tree is not binary.\n");
	if (return_code & INVALID_BIN_TREE_ELEM_ID) fprintf(stderr, "invalid bin tree element id\n");
	if (return_code & INVALID_BIN_TREE_ELEM_DEPTH) fprintf(stderr, "invalid bin tree element depth\n");
	if (return_code & ELEM_T_IS_NULL) fprintf(stderr, "element is null\n");

	if (return_code & DEPTH_IS_BIGGER_THAN_ID) fprintf(stderr, "bin tree element depth is bigger than bin tree element id\n");
	if (return_code & DEPTH_IS_BIGGER_THAN_BIN_TREE_SIZE) fprintf(stderr, "bin tree element depth is bigger than bin tree size\n");

	if (return_code & UNABLE_TO_CALOC_DYNAMIC_PTRS_ARR) fprintf(stderr, "unable to calloc dynamic ptrs arr\n");

	if (return_code & INVALID_BIN_TREE_SIZE) fprintf(stderr, "invalid bin tree dynamic ptrs size\n");
	if (return_code & INVALID_BIN_TREE_DYNAMIC_PTRS_CAPACITY) fprintf(stderr, "invalid bin tree dynamic ptrs capacity\n");

	if (return_code & SIZE_BIGGER_THAN_CAPACITY_IN_BIN_TREE_DYNAMIC_PTRS) fprintf(stderr, "in struct Bin_tree_dynamic_ptrs size is bigger than capacity\n");

	if (return_code & BIN_TREE_DYNAMIC_PTRS_IS_NULL) fprintf(stderr, "pointer on dynamic pointers in binary tree is NULL\n");

	if (return_code & DYNAMIC_PTR_IS_NULL_IN_BIN_TREE_DYNAMIC_PTRS) fprintf(stderr, "dynamic ptr is null in struct Bin_tree_dynamic_ptrs. It will cause segmentation fault when it free\n");

	if (return_code & MEMORY_LEAK_IN_BIN_TREE_DYNAMIC_PTRS) fprintf(stderr, "pointer index in pointer on pointers in struct Bin_tree_dynamic_ptrs is larger, than Bin_tree_dynamic_ptrs size\n");

	return return_code;
}


err_t print_bin_tree_elem(const struct Bin_tree_elem* const elem_ptr)
{
	err_t return_code = NO_ERROR;

	if (elem_ptr == NULL)
	{
		return return_code;
	}

	printf("\tstruct Bin_tree_elem elem_ptr [%p]\n", elem_ptr);
	printf("\t{\n");

	printf("\t\telem = \"" BIN_TREE_ELEM_PRINTF_SPEC "\"\n", elem_ptr->elem);
	printf("\t\tleft_child_ptr  = [%p]\n", elem_ptr->left_child_ptr);
	printf("\t\tright_child_ptr = [%p]\n", elem_ptr->right_child_ptr);
	printf("\t\tid = %zu\n", elem_ptr->id);
	printf("\t\ttype = %d\n", elem_ptr->type);

	printf("\t}\n");

	if ((elem_ptr->left_child_ptr) != NULL)
	{
		print_bin_tree_elem(elem_ptr->left_child_ptr);
	}
	if ((elem_ptr->right_child_ptr) != NULL)
	{
		print_bin_tree_elem(elem_ptr->right_child_ptr);
	}

	return return_code;
}


err_t bin_tree_dump(const struct Bin_tree* const bin_tree_ptr)
{
	CHECK_BIN_TREE()

	printf("struct Bin_tree bin_tree_ptr [%p]\n", bin_tree_ptr);
	printf("{\n");

	printf("\troot_ptr = [%p]\n", bin_tree_ptr->root_ptr);

	printf("\tstruct Bin_tree_dynamic_ptrs [%p]\n", &(bin_tree_ptr->ptrs));

	printf("\t{\n");

	for (ssize_t n_ptr = 0; n_ptr < (bin_tree_ptr->ptrs).capacity; n_ptr++)
	{
		printf("\t%zd: [%p]\n", n_ptr, ((bin_tree_ptr->ptrs).ptrs)[n_ptr]); 

		if (((bin_tree_ptr->ptrs).ptrs)[n_ptr] == NULL)
		{
			break;
		}
	}

	printf("\t}\n");



	print_bin_tree_elem(bin_tree_ptr->root_ptr);

	printf("}\n");

	return return_code;
}


static err_t del_bin_tree_elem(struct Bin_tree_elem* elem_ptr)
{
	if ((elem_ptr->left_child_ptr) != NULL)
	{
		del_bin_tree_elem(elem_ptr->left_child_ptr);
	}

	if ((elem_ptr->right_child_ptr) != NULL)
	{
		del_bin_tree_elem(elem_ptr->right_child_ptr);
	}

	printf("before free [%p]\n", elem_ptr);
	if (elem_ptr != NULL)
	{
		free(elem_ptr);	
	}
	
	printf("free [%p]\n", elem_ptr);
	return NO_ERROR;
}


#ifdef READABLE_PRINT_BIN_TREE_FILE
	#define PRINT_MINI_TABS(depth, outputfile)		\
	for (ssize_t i = 0; i < depth - 1; i++)			\
	{												\
		fputc('\t', outputfile);					\
	}
	
#else /*READABLE_PRINT_BIN_TREE_FILE*/
	#define PRINT_MINI_TABS(depth, outputfile)

#endif /*READABLE_PRINT_BIN_TREE_FILE*/


static err_t write_bin_tree_type(FILE* const file, const struct Bin_tree_elem* const elem_ptr)
{
	if (elem_ptr->type == NUM)
	{
		fprintf(file, BIN_TREE_ELEM_WRITE_SPEC, elem_ptr->elem);
	}
	else 
	{
		if (elem_ptr->type == BIN_OP)
		{
			if (elem_ptr->elem == ADD) fprintf(file, "ADD");
			if (elem_ptr->elem == SUB) fprintf(file, "SUB");
			if (elem_ptr->elem == DIV) fprintf(file, "DIV");
			if (elem_ptr->elem == MUL) fprintf(file, "MUL");
			if (elem_ptr->elem == LOG) fprintf(file, "LOG");
			if (elem_ptr->elem == POW) fprintf(file, "POW");
		}
		else if (elem_ptr->type == UN_OP)
		{
			if (elem_ptr->elem == SQRT) fprintf(file, "SQRT");
			if (elem_ptr->elem == SIN) fprintf(file, "SIN");
			if (elem_ptr->elem == COS) fprintf(file, "COS");
			if (elem_ptr->elem == EXP) fprintf(file, "EXP");
			if (elem_ptr->elem == LN) fprintf(file, "LN");
		}
		else
		{
			fprintf(file, "x");
		}
	}

	//fprintf(file, "type = %zd", elem_ptr->type);

	return NO_ERROR;
}

static err_t write_bin_tree_elem_to_file(FILE* const file, const struct Bin_tree_elem* const elem_ptr, const ssize_t parent_depth)
{
	err_t return_code = NO_ERROR;

	if (elem_ptr != NULL)
	{
		PRINT_MINI_TABS(elem_ptr->depth_in_tree, file);
		fprintf(file, "{\n");
		return_code |= write_bin_tree_elem_to_file(file, elem_ptr->left_child_ptr, elem_ptr->depth_in_tree);	
	}


	if (elem_ptr == NULL)
	{
		PRINT_MINI_TABS(parent_depth + 1, file);
		
		fprintf(file, "nil\n");
		return return_code;
	}



	PRINT_MINI_TABS(elem_ptr->depth_in_tree, file);	
	write_bin_tree_type(file, elem_ptr);

	fputc('\n', file);

	if (elem_ptr != NULL)
	{
		return_code |= write_bin_tree_elem_to_file(file, elem_ptr->right_child_ptr, elem_ptr->depth_in_tree);
	}

	PRINT_MINI_TABS(elem_ptr->depth_in_tree, file);
	fprintf(file, "}\n");

	return return_code;
}



err_t write_bin_tree_to_file(const char* const filename, const struct Bin_tree* const bin_tree_ptr)
{
	CHECK_BIN_TREE()

	FILE* file = open_file(filename, "w");

	write_bin_tree_elem_to_file(file, bin_tree_ptr->root_ptr, (bin_tree_ptr->root_ptr)->depth_in_tree);

	fclose(file);

	return return_code;
}

#undef PRINT_MINI_TABS


#define CHECK_OP(str, op_name)					\
if (!strncmp(str, #op_name, sizeof(#op_name))) 	\
{												\
	*elem_ptr = op_name;						\
	return true;								\
}

static bool check_if_bin_op(const char* const str, elem_t*  const elem_ptr)
{
	CHECK_OP(str, ADD);
	CHECK_OP(str, SUB);
	CHECK_OP(str, MUL);
	CHECK_OP(str, DIV);
	CHECK_OP(str, LOG);
	CHECK_OP(str, POW);

	return false;
}

static bool check_if_un_op(const char* const str, elem_t* const elem_ptr)
{
	CHECK_OP(str, SQRT);
	CHECK_OP(str, SIN);
	CHECK_OP(str, COS);
	CHECK_OP(str, EXP);
	CHECK_OP(str, LN);

	return false;
}


static enum BinTreeElemTypes get_type_and_elem(const char* const str, elem_t* const elem_ptr)
{
	enum BinTreeElemTypes type = NO_TYPE;

	printf("get type and elem str : <%p>", str);

	if (sscanf(str, BIN_TREE_ELEM_PRINTF_SPEC, elem_ptr))
	{
		type = NUM;
	}
	else
	{
		if (check_if_bin_op(str, elem_ptr))
		{
			type = BIN_OP;
		}
		else if (check_if_un_op(str, elem_ptr))
		{
			type = UN_OP;
		}
		else 
		{
			type = VAR;
		}
	}	

	return type;
}


static err_t read_bin_tree_elem_from_file(struct Bin_tree* const bin_tree_ptr, ssize_t* const position_ptr, struct Bin_tree_elem* const parent_ptr, const bool left_or_right, char* const* const bin_tree_text)
{
	err_t return_code = NO_ERROR;

	printf("in func %s\n", __func__);

	printf("%s should be { or nil position: %zd\n\n", bin_tree_text[*position_ptr], *position_ptr);

	#define CHECK_IF_NIL(nil_name)												\
																				\
	if (!strncmp(bin_tree_text[*position_ptr], nil_name, sizeof(nil_name)))		\
	{																			\
		printf("nil do not go to next depth\n");								\
		(*position_ptr)++;														\
		return return_code;														\
	}	

	CHECK_IF_NIL("nil");

	#undef CHECK_IF_NIL

	/*if (strncmp(bin_tree_text[*position_ptr], "{", sizeof("{")) != 0)
	{
		(*position_ptr)++;
		printf("%s is not { return\n");
		return return_code;
	}*/


	(*position_ptr)++; // skip {

	printf("%s go to next depth\n", bin_tree_text[*position_ptr]);

	elem_t elem = NULL;

	

	struct Bin_tree_elem* current_elem_ptr =  init_bin_tree_elem(bin_tree_ptr, parent_ptr, left_or_right, NULL, NO_TYPE);

	

	printf("%s should be elem position: %zd\n", bin_tree_text[*position_ptr], *position_ptr);

	read_bin_tree_elem_from_file(bin_tree_ptr, position_ptr, current_elem_ptr, LEFT_CHILD, bin_tree_text);

	enum BinTreeElemTypes type = get_type_and_elem(bin_tree_text[*position_ptr], &elem);

	current_elem_ptr->elem = elem;

	current_elem_ptr->type = type;

	(*position_ptr)++;

	read_bin_tree_elem_from_file(bin_tree_ptr, position_ptr, current_elem_ptr, RIGHT_CHILD, bin_tree_text);

	printf("%s should be } position: %zd\n", bin_tree_text[*position_ptr], *position_ptr);

	(*position_ptr)++; //skip }

	//printf("%s\n", bin_tree_text[*position_ptr]);

	printf("before return in %s", __func__);

	return return_code;
}


err_t read_bin_tree_from_file(const char* const filename, struct Bin_tree* const bin_tree_ptr)
{
	err_t return_code = NO_ERROR;

	FILE* file = open_file(filename, "r");

	ssize_t n_strings = 0;

	void* buffer_to_free = NULL;

	char** bin_tree_text =  init_text(filename, &n_strings, &buffer_to_free);

	printf("after init text\n");

	elem_t elem = NULL;

	
	printf("before init_bin_tree\n");
	init_bin_tree(bin_tree_ptr, NULL, NO_TYPE); // сначала ставим в корень NULL, без типа, потому что мы ещё не считали их

	printf("after init_bin_tree\n");

	printf("%s\n", bin_tree_text[1]);

	append_new_dynamic_ptr((void*)buffer_to_free, &(bin_tree_ptr->ptrs));

	append_new_dynamic_ptr((void*)bin_tree_text, &(bin_tree_ptr->ptrs));

	printf("before append_new_dynamic_ptr\n");

	ssize_t position = 0;
	struct Bin_tree_elem* current_elem_ptr = bin_tree_ptr->root_ptr;

	printf("before read bin tree elem from file\n");

	position++;

	read_bin_tree_elem_from_file(bin_tree_ptr, &position, current_elem_ptr, LEFT_CHILD, bin_tree_text);

	printf("after main read_bin_tree_elem_from_file\n");

	printf("position: %zd\n", position);

	printf("<%s>\n", bin_tree_text[position - 1]);

	enum BinTreeElemTypes type = get_type_and_elem(bin_tree_text[position], &elem);

	printf("after main get_type_and_elem\n");

	(bin_tree_ptr->root_ptr)->elem = elem;

	(bin_tree_ptr->root_ptr)->type = type;

	position++;

	printf("before 2 read bin tree elem from file\n");

	read_bin_tree_elem_from_file(bin_tree_ptr, &position, current_elem_ptr, RIGHT_CHILD, bin_tree_text);

	printf("after 2 read bin tree elem from file\n");

	fclose(file);

	return return_code;
}

#undef CHECK_OP


err_t bin_tree_dtor(struct Bin_tree* bin_tree_ptr)
{
	err_t return_code = bin_tree_verificator(bin_tree_ptr);

	printf("after veirificator in destructor\n");

	//return_code |= del_bin_tree_elem(bin_tree_ptr->root_ptr);

	return_code |= bin_tree_dynamic_ptrs_free(&(bin_tree_ptr->ptrs));

	printf("after free dynamic ptrs\n");

	return return_code;
}


elem_t eval (const struct Bin_tree_elem* elem_ptr, const elem_t x)
{
	if ((elem_ptr->type) == NUM)
	{
		return elem_ptr->elem;
	}
	else if ((elem_ptr->type) == VAR)
	{
		return x;
	}

	elem_t left = NULL;

	if (elem_ptr->left_child_ptr != NULL)
	{
		left = eval(elem_ptr->left_child_ptr, x);
	}
	else 
	{
		printf("error OP left child is null\n");
		return NAN;
	}


	switch ((int)elem_ptr->elem)   // unary op
	{
		case SIN: return sin(left);
		case COS: return cos(left);
		case SQRT: return sqrt(left);
		case EXP: return exp(left);
		case LN: return log(left);
		default:
		{
			elem_t right = NULL;

			if (elem_ptr->right_child_ptr != NULL)
			{
				right = eval(elem_ptr->right_child_ptr, x);
			}
			else 
			{
				printf("error OP right child is null, but binary OP\n");
				return NAN;
			}
			
			switch ((int)elem_ptr->elem) // binary op
			{
				case ADD: return left + right;
				case SUB: return left - right;
				case MUL: return left * right;
				case DIV: return left / right;
				case LOG: return log(left) / log(right);
				case POW: return pow(left, right);
				default: 
				{
					printf("invalid op\n");
					return NULL;
				}
			}
		}
	}
}

struct Bin_tree_elem* subtree_copy(struct Bin_tree* diff_expression_ptr,  struct Bin_tree_elem* expression_elem_ptr, struct Bin_tree_elem* d_parent_ptr, bool left_or_right)
{
	struct Bin_tree_elem* elem_copy_ptr = init_bin_tree_elem(diff_expression_ptr, d_parent_ptr, left_or_right, expression_elem_ptr->elem, expression_elem_ptr->type);

	if (expression_elem_ptr->left_child_ptr != NULL)
	{
		elem_copy_ptr->left_child_ptr = subtree_copy(diff_expression_ptr, expression_elem_ptr->left_child_ptr, elem_copy_ptr, LEFT_CHILD);
	}

	if ((expression_elem_ptr->right_child_ptr != NULL) && (expression_elem_ptr->type != UN_OP))
	{
		elem_copy_ptr->right_child_ptr = subtree_copy(diff_expression_ptr, expression_elem_ptr->right_child_ptr, elem_copy_ptr, RIGHT_CHILD);
	}

	return elem_copy_ptr;
}


err_t d_add(struct Bin_tree_elem* current_elem_ptr, struct Bin_tree* diff_expression_ptr, struct Bin_tree_elem* expression_elem_ptr)
{
	err_t return_code = NO_ERROR; 

	current_elem_ptr->left_child_ptr = d(diff_expression_ptr, expression_elem_ptr->left_child_ptr, current_elem_ptr, LEFT_CHILD);
	current_elem_ptr->right_child_ptr = d(diff_expression_ptr, expression_elem_ptr->right_child_ptr, current_elem_ptr, RIGHT_CHILD);

	return return_code;
}


err_t d_mul(struct Bin_tree_elem* current_elem_ptr, struct Bin_tree* diff_expression_ptr, struct Bin_tree_elem* expression_elem_ptr)
{
	err_t return_code = NO_ERROR;

	current_elem_ptr->left_child_ptr = init_bin_tree_elem(diff_expression_ptr, current_elem_ptr, LEFT_CHILD, MUL, BIN_OP);
	current_elem_ptr->right_child_ptr = init_bin_tree_elem(diff_expression_ptr, current_elem_ptr, RIGHT_CHILD, MUL, BIN_OP);

	(current_elem_ptr->left_child_ptr)->left_child_ptr = d(diff_expression_ptr, expression_elem_ptr->left_child_ptr, current_elem_ptr->left_child_ptr, LEFT_CHILD);
	(current_elem_ptr->left_child_ptr)->right_child_ptr = subtree_copy(diff_expression_ptr, expression_elem_ptr->right_child_ptr, current_elem_ptr->left_child_ptr, RIGHT_CHILD);


	(current_elem_ptr->right_child_ptr)->left_child_ptr = d(diff_expression_ptr, expression_elem_ptr->right_child_ptr, current_elem_ptr->right_child_ptr, LEFT_CHILD);
	(current_elem_ptr->right_child_ptr)->right_child_ptr = subtree_copy(diff_expression_ptr, expression_elem_ptr->left_child_ptr, current_elem_ptr->right_child_ptr, RIGHT_CHILD);	

	return return_code;
}

err_t d_div(struct Bin_tree_elem* current_elem_ptr, struct Bin_tree* diff_expression_ptr, struct Bin_tree_elem* expression_elem_ptr)
{
	err_t return_code = NO_ERROR;

	current_elem_ptr->left_child_ptr = init_bin_tree_elem(diff_expression_ptr, current_elem_ptr, LEFT_CHILD, SUB, BIN_OP);
	current_elem_ptr->right_child_ptr = init_bin_tree_elem(diff_expression_ptr, current_elem_ptr, RIGHT_CHILD, MUL, BIN_OP);

	(current_elem_ptr->right_child_ptr)->left_child_ptr = subtree_copy(diff_expression_ptr, expression_elem_ptr->right_child_ptr, current_elem_ptr->right_child_ptr, LEFT_CHILD);
	(current_elem_ptr->right_child_ptr)->right_child_ptr = subtree_copy(diff_expression_ptr, expression_elem_ptr->right_child_ptr, current_elem_ptr->right_child_ptr, RIGHT_CHILD);
	
	(current_elem_ptr->left_child_ptr)->left_child_ptr = init_bin_tree_elem(diff_expression_ptr, current_elem_ptr->left_child_ptr, LEFT_CHILD, MUL, BIN_OP);
	(current_elem_ptr->left_child_ptr)->right_child_ptr = init_bin_tree_elem(diff_expression_ptr, current_elem_ptr->left_child_ptr, RIGHT_CHILD, MUL, BIN_OP);	

	((current_elem_ptr->left_child_ptr)->left_child_ptr)->left_child_ptr = d(diff_expression_ptr, expression_elem_ptr->left_child_ptr, (current_elem_ptr->left_child_ptr)->left_child_ptr, LEFT_CHILD);
	((current_elem_ptr->left_child_ptr)->left_child_ptr)->right_child_ptr = subtree_copy(diff_expression_ptr, expression_elem_ptr->right_child_ptr, (current_elem_ptr->left_child_ptr)->left_child_ptr, RIGHT_CHILD);


	((current_elem_ptr->left_child_ptr)->right_child_ptr)->left_child_ptr = d(diff_expression_ptr, expression_elem_ptr->right_child_ptr, (current_elem_ptr->left_child_ptr)->right_child_ptr, LEFT_CHILD);
	((current_elem_ptr->left_child_ptr)->right_child_ptr)->right_child_ptr = subtree_copy(diff_expression_ptr, expression_elem_ptr->left_child_ptr, (current_elem_ptr->left_child_ptr)->right_child_ptr, RIGHT_CHILD);	

	return return_code;
}

err_t d_sin(struct Bin_tree_elem* current_elem_ptr, struct Bin_tree* diff_expression_ptr, struct Bin_tree_elem* expression_elem_ptr)
{
	err_t return_code = NO_ERROR;

	current_elem_ptr->left_child_ptr = init_bin_tree_elem(diff_expression_ptr, current_elem_ptr, LEFT_CHILD, COS, UN_OP);
	(current_elem_ptr->left_child_ptr)->left_child_ptr = subtree_copy(diff_expression_ptr, (expression_elem_ptr)->left_child_ptr, (current_elem_ptr)->left_child_ptr, LEFT_CHILD);
	(current_elem_ptr)->right_child_ptr = d(diff_expression_ptr, (expression_elem_ptr)->left_child_ptr, current_elem_ptr, RIGHT_CHILD);

	return return_code;
}

err_t d_cos(struct Bin_tree_elem* current_elem_ptr, struct Bin_tree* diff_expression_ptr, struct Bin_tree_elem* expression_elem_ptr)
{
	err_t return_code = NO_ERROR;

	current_elem_ptr->left_child_ptr = init_bin_tree_elem(diff_expression_ptr, current_elem_ptr, LEFT_CHILD, 0, NUM);

	current_elem_ptr->right_child_ptr = init_bin_tree_elem(diff_expression_ptr, current_elem_ptr, RIGHT_CHILD, MUL, BIN_OP);

	(current_elem_ptr->right_child_ptr)->left_child_ptr = init_bin_tree_elem(diff_expression_ptr, current_elem_ptr->right_child_ptr, LEFT_CHILD, SIN, UN_OP);

	((current_elem_ptr->right_child_ptr)->left_child_ptr)->left_child_ptr = subtree_copy(diff_expression_ptr, expression_elem_ptr->left_child_ptr, (current_elem_ptr->right_child_ptr)->left_child_ptr, LEFT_CHILD);

	(current_elem_ptr->right_child_ptr)->right_child_ptr = d(diff_expression_ptr, expression_elem_ptr->left_child_ptr, current_elem_ptr->right_child_ptr, RIGHT_CHILD);	

	return return_code;
}


err_t d_sqrt(struct Bin_tree_elem* current_elem_ptr, struct Bin_tree* diff_expression_ptr, struct Bin_tree_elem* expression_elem_ptr)
{
	err_t return_code = NO_ERROR;

	current_elem_ptr->right_child_ptr = init_bin_tree_elem(diff_expression_ptr, current_elem_ptr, RIGHT_CHILD, MUL, BIN_OP);

	(current_elem_ptr->right_child_ptr)->left_child_ptr = init_bin_tree_elem(diff_expression_ptr, current_elem_ptr->right_child_ptr, LEFT_CHILD, 2, NUM);

	(current_elem_ptr->right_child_ptr)->right_child_ptr = init_bin_tree_elem(diff_expression_ptr, current_elem_ptr->right_child_ptr, RIGHT_CHILD, SQRT, UN_OP);

	current_elem_ptr->left_child_ptr = d(diff_expression_ptr, expression_elem_ptr->left_child_ptr, current_elem_ptr, LEFT_CHILD);

	((current_elem_ptr->right_child_ptr)->right_child_ptr)->left_child_ptr = subtree_copy(diff_expression_ptr, expression_elem_ptr->left_child_ptr, (current_elem_ptr->right_child_ptr)->right_child_ptr, LEFT_CHILD);

	return return_code;
}


err_t d_exp(struct Bin_tree_elem* current_elem_ptr, struct Bin_tree* diff_expression_ptr, struct Bin_tree_elem* expression_elem_ptr)
{
	err_t return_code = NO_ERROR;

	current_elem_ptr->left_child_ptr = subtree_copy(diff_expression_ptr, expression_elem_ptr, current_elem_ptr, LEFT_CHILD);

	current_elem_ptr->right_child_ptr = d(diff_expression_ptr, expression_elem_ptr->left_child_ptr, current_elem_ptr, RIGHT_CHILD);

	return return_code;
}


err_t d_ln(struct Bin_tree_elem* current_elem_ptr, struct Bin_tree* diff_expression_ptr, struct Bin_tree_elem* expression_elem_ptr)
{
	err_t return_code = NO_ERROR;

	current_elem_ptr->left_child_ptr = d(diff_expression_ptr, expression_elem_ptr->left_child_ptr, current_elem_ptr, LEFT_CHILD);

	current_elem_ptr->right_child_ptr = subtree_copy(diff_expression_ptr, expression_elem_ptr->left_child_ptr, current_elem_ptr, RIGHT_CHILD);

	return return_code;
}

err_t d_pow(struct Bin_tree_elem* current_elem_ptr, struct Bin_tree* diff_expression_ptr, struct Bin_tree_elem* expression_elem_ptr)
{
	err_t return_code = NO_ERROR;
/*
	if ((current_elem_ptr->right_child_ptr)->type == NUM)
	{
		current_elem_ptr->left_child_ptr = init_bin_tree_elem(diff_expression_ptr, current_elem_ptr, LEFT_CHILD, MUL, BIN_OP);
		(current_elem_ptr->left_child_ptr)->right_child_ptr = init_bin_tree_elem(diff_expression_ptr, current_elem_ptr->left_child_ptr, RIGHT_CHILD, POW, BIN_OP);
		(current_elem_ptr->left_child_ptr)->left_child_ptr = init_bin_tree_elem(diff_expression_ptr, current_elem_ptr->left_child_ptr, LEFT_CHILD, (expression_elem_ptr->right_child_ptr)->elem, NUM);
		((current_elem_ptr->left_child_ptr)->right_child_ptr)->right_child_ptr = init_bin_tree_elem(diff_expression_ptr, (current_elem_ptr->left_child_ptr)->right_child_ptr, RIGHT_CHILD, (expression_elem_ptr->right_child_ptr)->elem - 1, NUM);
		current_elem_ptr->right_child_ptr = d(diff_expression_ptr, expression_elem_ptr->left_child_ptr, current_elem_ptr, RIGHT_CHILD);
		((current_elem_ptr->left_child_ptr)->right_child_ptr)->left_child_ptr = subtree_copy(diff_expression_ptr, expression_elem_ptr->left_child_ptr, (current_elem_ptr->left_child_ptr)->right_child_ptr, LEFT_CHILD);

		return return_code;
	}*/


	current_elem_ptr->left_child_ptr = init_bin_tree_elem(diff_expression_ptr, current_elem_ptr, LEFT_CHILD, POW, BIN_OP);

	(current_elem_ptr->left_child_ptr)->left_child_ptr = subtree_copy(diff_expression_ptr, expression_elem_ptr->left_child_ptr, current_elem_ptr->left_child_ptr, LEFT_CHILD);

	(current_elem_ptr->left_child_ptr)->right_child_ptr = subtree_copy(diff_expression_ptr, expression_elem_ptr->right_child_ptr, current_elem_ptr->left_child_ptr, RIGHT_CHILD);

	current_elem_ptr->right_child_ptr = init_bin_tree_elem(diff_expression_ptr, current_elem_ptr, RIGHT_CHILD, ADD, BIN_OP);

	(current_elem_ptr->right_child_ptr)->left_child_ptr = init_bin_tree_elem(diff_expression_ptr, current_elem_ptr->right_child_ptr, LEFT_CHILD, MUL, BIN_OP);

	(current_elem_ptr->right_child_ptr)->right_child_ptr = init_bin_tree_elem(diff_expression_ptr, current_elem_ptr->right_child_ptr, RIGHT_CHILD, MUL, BIN_OP);

	((current_elem_ptr->right_child_ptr)->left_child_ptr)->right_child_ptr = init_bin_tree_elem(diff_expression_ptr, (current_elem_ptr->right_child_ptr)->left_child_ptr, RIGHT_CHILD, DIV, BIN_OP);

	((current_elem_ptr->right_child_ptr)->right_child_ptr)->right_child_ptr = init_bin_tree_elem(diff_expression_ptr, (current_elem_ptr->right_child_ptr)->right_child_ptr, RIGHT_CHILD, LN, UN_OP);

	(((current_elem_ptr->right_child_ptr)->right_child_ptr)->right_child_ptr)->left_child_ptr = subtree_copy(diff_expression_ptr, expression_elem_ptr->left_child_ptr , ((current_elem_ptr->right_child_ptr)->right_child_ptr)->right_child_ptr, LEFT_CHILD);

	((current_elem_ptr->right_child_ptr)->right_child_ptr)->left_child_ptr = d(diff_expression_ptr, expression_elem_ptr->right_child_ptr, (current_elem_ptr->right_child_ptr)->right_child_ptr, LEFT_CHILD);

	((current_elem_ptr->right_child_ptr)->left_child_ptr)->left_child_ptr = subtree_copy(diff_expression_ptr, expression_elem_ptr->right_child_ptr, (current_elem_ptr->right_child_ptr)->left_child_ptr, LEFT_CHILD);

	(((current_elem_ptr->right_child_ptr)->left_child_ptr)->right_child_ptr)->left_child_ptr = d(diff_expression_ptr, expression_elem_ptr->left_child_ptr, ((current_elem_ptr->right_child_ptr)->left_child_ptr)->right_child_ptr, LEFT_CHILD);

	(((current_elem_ptr->right_child_ptr)->left_child_ptr)->right_child_ptr)->right_child_ptr = subtree_copy(diff_expression_ptr, expression_elem_ptr->left_child_ptr, ((current_elem_ptr->right_child_ptr)->left_child_ptr)->right_child_ptr, RIGHT_CHILD);

	return return_code;

}



struct Bin_tree_elem* d(struct Bin_tree* diff_expression_ptr, struct Bin_tree_elem* expression_elem_ptr,  struct Bin_tree_elem* d_parent_ptr, bool left_or_right)
{
	if (expression_elem_ptr->type == NUM)
	{
		struct Bin_tree_elem* current_elem_ptr = init_bin_tree_elem(diff_expression_ptr, d_parent_ptr, left_or_right, 0, NUM);
		return current_elem_ptr;
	}	
	else if (expression_elem_ptr->type == VAR)
	{
		struct Bin_tree_elem* current_elem_ptr = init_bin_tree_elem(diff_expression_ptr, d_parent_ptr, left_or_right, 1, NUM);
		return current_elem_ptr;
	}
	switch ((ssize_t)(expression_elem_ptr->elem))
	{
		case ADD: 
		{
			struct Bin_tree_elem* current_elem_ptr = init_bin_tree_elem(diff_expression_ptr, d_parent_ptr, left_or_right, ADD, BIN_OP);

			d_add(current_elem_ptr, diff_expression_ptr, expression_elem_ptr);

			return current_elem_ptr;
		}
		case SUB:
		{
			struct Bin_tree_elem* current_elem_ptr = init_bin_tree_elem(diff_expression_ptr, d_parent_ptr, left_or_right, SUB, BIN_OP);

			d_add(current_elem_ptr, diff_expression_ptr, expression_elem_ptr);

			return current_elem_ptr;
		}
		case MUL:
		{
			struct Bin_tree_elem* current_elem_ptr = init_bin_tree_elem(diff_expression_ptr, d_parent_ptr, left_or_right, ADD, BIN_OP);

			d_mul(current_elem_ptr, diff_expression_ptr, expression_elem_ptr);

			return current_elem_ptr;	
		} 
		case DIV:
		{
			struct Bin_tree_elem* current_elem_ptr = init_bin_tree_elem(diff_expression_ptr, d_parent_ptr, left_or_right, DIV, BIN_OP);

			d_div(current_elem_ptr, diff_expression_ptr, expression_elem_ptr);

			return current_elem_ptr;
		}
		case SIN:
		{
			struct Bin_tree_elem* current_elem_ptr = init_bin_tree_elem(diff_expression_ptr, d_parent_ptr, left_or_right, MUL, BIN_OP);

			d_sin(current_elem_ptr, diff_expression_ptr, expression_elem_ptr);

			return current_elem_ptr;
		} 
		case COS:
		{
			struct Bin_tree_elem* current_elem_ptr = init_bin_tree_elem(diff_expression_ptr, d_parent_ptr, left_or_right, SUB, BIN_OP);

			d_cos(current_elem_ptr, diff_expression_ptr, expression_elem_ptr);

			return current_elem_ptr;

		}
		case SQRT:
		{
			struct Bin_tree_elem* current_elem_ptr = init_bin_tree_elem(diff_expression_ptr, d_parent_ptr, left_or_right, DIV, BIN_OP);

			d_sqrt(current_elem_ptr, diff_expression_ptr, expression_elem_ptr);

			return current_elem_ptr;
		} 
		case EXP:
		{
			struct Bin_tree_elem* current_elem_ptr = init_bin_tree_elem(diff_expression_ptr, d_parent_ptr, left_or_right, MUL, BIN_OP);

			d_exp(current_elem_ptr, diff_expression_ptr, expression_elem_ptr);

			return current_elem_ptr;
		} 
		case LN:
		{
			struct Bin_tree_elem* current_elem_ptr = init_bin_tree_elem(diff_expression_ptr, d_parent_ptr, left_or_right, DIV, BIN_OP);

			d_ln(current_elem_ptr, diff_expression_ptr, expression_elem_ptr);

			return current_elem_ptr;
		} 
		case POW:
		{
			struct Bin_tree_elem* current_elem_ptr = init_bin_tree_elem(diff_expression_ptr, d_parent_ptr, left_or_right, MUL, BIN_OP);

			d_pow(current_elem_ptr, diff_expression_ptr, expression_elem_ptr);

			return current_elem_ptr;
		} 
		default:
		{
			printf("invalid op\n");
			return NULL;
		}
	}
}


struct Bin_tree diff(struct Bin_tree* expression_ptr)
{
	struct Bin_tree diff_expression;

	if ((expression_ptr->root_ptr)->type == NUM)
	{
		init_bin_tree(&diff_expression, 0, NUM);
		return diff_expression;
	}
	else if ((expression_ptr->root_ptr)->type == VAR)
	{
		init_bin_tree(&diff_expression, 1, NUM);
		return diff_expression;
	}

	switch ((ssize_t)((expression_ptr->root_ptr)->elem))
	{
		case ADD: 
		{
			init_bin_tree(&diff_expression, ADD, BIN_OP);

			d_add(diff_expression.root_ptr, &diff_expression, expression_ptr->root_ptr);

			return diff_expression;
		}
		case SUB:
		{
			init_bin_tree(&diff_expression, SUB, BIN_OP);
			
			d_add(diff_expression.root_ptr, &diff_expression, expression_ptr->root_ptr);

			return diff_expression;
		}
		case MUL:
		{
			printf("MUL MUL MUL\n");
			init_bin_tree(&diff_expression, ADD, BIN_OP);

			d_mul(diff_expression.root_ptr, &diff_expression, expression_ptr->root_ptr);

			return diff_expression;		
		} 
		case DIV:
		{
			init_bin_tree(&diff_expression, DIV, BIN_OP);

			d_div(diff_expression.root_ptr, &diff_expression, expression_ptr->root_ptr);

			return diff_expression;
		}
		case SIN:
		{
			init_bin_tree(&diff_expression, MUL, BIN_OP);

			d_sin(diff_expression.root_ptr, &diff_expression, expression_ptr->root_ptr);
			/*(diff_expression.root_ptr)->left_child_ptr = init_bin_tree_elem(&diff_expression, diff_expression.root_ptr, LEFT_CHILD, COS, UN_OP);
			((diff_expression.root_ptr)->left_child_ptr)->left_child_ptr = subtree_copy(&diff_expression, (expression_ptr->root_ptr)->left_child_ptr, (diff_expression.root_ptr)->left_child_ptr, LEFT_CHILD);
			(diff_expression.root_ptr)->right_child_ptr = d(&diff_expression, (expression_ptr->root_ptr)->left_child_ptr, (diff_expression.root_ptr), RIGHT_CHILD);*/

			return diff_expression;
		} 
		case COS:
		{
			init_bin_tree(&diff_expression, SUB, BIN_OP);

			d_cos(diff_expression.root_ptr, &diff_expression, expression_ptr->root_ptr);

			return diff_expression;
		} 
		case SQRT:
		{
			init_bin_tree(&diff_expression, DIV, BIN_OP);

			d_sqrt(diff_expression.root_ptr, &diff_expression, expression_ptr->root_ptr);

			return diff_expression;
		} 
		case EXP:
		{
			init_bin_tree(&diff_expression, MUL, BIN_OP);

			d_exp(diff_expression.root_ptr, &diff_expression, expression_ptr->root_ptr);

			return diff_expression;
		} 
		case LN:
		{
			init_bin_tree(&diff_expression, DIV, BIN_OP);

			d_ln(diff_expression.root_ptr, &diff_expression, expression_ptr->root_ptr);

			return diff_expression;
		} 
		case POW:
		{
			init_bin_tree(&diff_expression, MUL, BIN_OP);

			d_pow(diff_expression.root_ptr, &diff_expression, expression_ptr->root_ptr);

			return diff_expression;
		} 
		default:
		{
			printf("invalid op\n");
			return diff_expression;
		}
	}
}


err_t delete_subtree(struct Bin_tree_elem* elem_ptr)
{
	err_t return_code = NO_ERROR;

	if (elem_ptr == NULL)
	{
		return return_code;
	}

	return_code |= delete_subtree(elem_ptr->left_child_ptr);
	return_code |= delete_subtree(elem_ptr->right_child_ptr);

	elem_ptr->type = DELETED_ELEMENT;
	elem_ptr->left_child_ptr = NULL;
	elem_ptr->right_child_ptr = NULL;

	return return_code;
}


err_t delete_right_child_un_op(struct Bin_tree_elem* elem_ptr)
{
	err_t return_code = NO_ERROR;

	if (elem_ptr == NULL)
	{
		return return_code;
	}

	return_code |= delete_right_child_un_op(elem_ptr->left_child_ptr);


	if (elem_ptr->type == UN_OP)
	{
		return_code |= delete_subtree(elem_ptr->right_child_ptr);
		elem_ptr->right_child_ptr = NULL;
	}
	else 
	{
		return_code |= delete_right_child_un_op(elem_ptr->right_child_ptr);
	}

	return return_code;
}


err_t zero_optimisation(struct Bin_tree_elem* expression_elem_ptr, bool* is_optimised_ptr)
{
	err_t return_code = NO_ERROR;

	if ((expression_elem_ptr->type == NUM) || (expression_elem_ptr->type == VAR)) 
	{
		return return_code;
	}
	if (expression_elem_ptr->type == UN_OP)
	{
		return_code |= zero_optimisation(expression_elem_ptr->left_child_ptr, is_optimised_ptr);
		return return_code; 
	}

	if (expression_elem_ptr->type == BIN_OP)
	{
		if (((expression_elem_ptr->left_child_ptr)->type ==  NUM) && ((expression_elem_ptr->left_child_ptr)->elem == 0))
		{

			if (expression_elem_ptr->elem == ADD) 
			{
				expression_elem_ptr->type = (expression_elem_ptr->right_child_ptr)->type;
				expression_elem_ptr->elem = (expression_elem_ptr->right_child_ptr)->elem;
			


				(expression_elem_ptr->left_child_ptr)->type = DELETED_ELEMENT;
				(expression_elem_ptr->right_child_ptr)->type = DELETED_ELEMENT;


				struct Bin_tree_elem* new_right_child = (expression_elem_ptr->right_child_ptr)->right_child_ptr;

				struct Bin_tree_elem* new_left_child = (expression_elem_ptr->right_child_ptr)->left_child_ptr;

				expression_elem_ptr->left_child_ptr = new_left_child;
				expression_elem_ptr->right_child_ptr = new_right_child;

				*is_optimised_ptr = true;

				return return_code;
			}
			else if ((expression_elem_ptr->elem == MUL) || (expression_elem_ptr->elem == DIV))
			{
				expression_elem_ptr->type = NUM;
				expression_elem_ptr->elem = 0;


				delete_subtree(expression_elem_ptr->left_child_ptr);
				delete_subtree(expression_elem_ptr->right_child_ptr);

				expression_elem_ptr->left_child_ptr = NULL;
				expression_elem_ptr->right_child_ptr = NULL;

				*is_optimised_ptr = true;

				return return_code;
			}
			else if (expression_elem_ptr->elem == SUB) 
			{
				return_code |= zero_optimisation(expression_elem_ptr->left_child_ptr, is_optimised_ptr);
			}			
			else
			{
				printf("file: %s func: %s line: %d op is not supported\n", __FILE__, __func__, __LINE__);
				return return_code;
			}
		}
		else
		{
			return_code |= zero_optimisation(expression_elem_ptr->left_child_ptr, is_optimised_ptr);
		}

		if (((expression_elem_ptr->right_child_ptr)->type ==  NUM) && ((expression_elem_ptr->right_child_ptr)->elem == 0))
		{
			if ((expression_elem_ptr->elem == SUB) || (expression_elem_ptr->elem == ADD))
			{
				expression_elem_ptr->type = (expression_elem_ptr->left_child_ptr)->type;
				expression_elem_ptr->elem = (expression_elem_ptr->left_child_ptr)->elem;

				(expression_elem_ptr->left_child_ptr)->type = DELETED_ELEMENT;
				(expression_elem_ptr->right_child_ptr)->type = DELETED_ELEMENT;

				struct Bin_tree_elem* new_right_child = (expression_elem_ptr->left_child_ptr)->right_child_ptr;
				struct Bin_tree_elem* new_left_child = (expression_elem_ptr->left_child_ptr)->left_child_ptr;

				expression_elem_ptr->left_child_ptr = new_left_child;
				expression_elem_ptr->right_child_ptr = new_right_child;
				*is_optimised_ptr = true;
			}
			else if (expression_elem_ptr->elem == MUL)
			{
				expression_elem_ptr->type = NUM;
				expression_elem_ptr->elem = 0;

				delete_subtree(expression_elem_ptr->left_child_ptr);
				delete_subtree(expression_elem_ptr->right_child_ptr);

				expression_elem_ptr->left_child_ptr = NULL;
				expression_elem_ptr->right_child_ptr = NULL;
				*is_optimised_ptr = true;
			}
			else 
			{
				printf("file: %s func: %s line: %d op is not supported\n", __FILE__, __func__, __LINE__);
			}
			
			return return_code;
		}
		else 
		{
			return_code |= zero_optimisation(expression_elem_ptr->right_child_ptr, is_optimised_ptr);
		}
	}

	return return_code;
}


err_t one_optimisation(struct Bin_tree_elem* expression_elem_ptr, bool* is_optimised_ptr)
{
	err_t return_code = NO_ERROR;

	if (expression_elem_ptr == NULL)
	{
		return return_code;
	}
	if ((expression_elem_ptr->type == NUM) || (expression_elem_ptr->type == VAR)) 
	{
		return return_code;
	}
	if (expression_elem_ptr->type == UN_OP)
	{
		return_code |= one_optimisation(expression_elem_ptr->left_child_ptr, is_optimised_ptr);
		return return_code; 
	}

	if (expression_elem_ptr->type == BIN_OP)
	{
		if (((expression_elem_ptr->left_child_ptr)->type ==  NUM) && ((expression_elem_ptr->left_child_ptr)->elem == 1) && (expression_elem_ptr->elem == MUL))
		{
			expression_elem_ptr->type = (expression_elem_ptr->right_child_ptr)->type;
			expression_elem_ptr->elem = (expression_elem_ptr->right_child_ptr)->elem;
			
			(expression_elem_ptr->left_child_ptr)->type = DELETED_ELEMENT;
			(expression_elem_ptr->right_child_ptr)->type = DELETED_ELEMENT;

			expression_elem_ptr->left_child_ptr = (expression_elem_ptr->right_child_ptr)->left_child_ptr;
			expression_elem_ptr->right_child_ptr = (expression_elem_ptr->right_child_ptr)->right_child_ptr;

			*is_optimised_ptr = true;

			return return_code;
		}
		else
		{
			return_code |= one_optimisation(expression_elem_ptr->left_child_ptr, is_optimised_ptr);
		}

		if (((expression_elem_ptr->right_child_ptr)->type ==  NUM) && ((expression_elem_ptr->right_child_ptr)->elem == 1) && ((expression_elem_ptr->elem == MUL) || (expression_elem_ptr->elem == DIV)))
		{
			expression_elem_ptr->type = (expression_elem_ptr->left_child_ptr)->type;
			expression_elem_ptr->elem = (expression_elem_ptr->left_child_ptr)->elem;

			(expression_elem_ptr->left_child_ptr)->type = DELETED_ELEMENT;
			(expression_elem_ptr->right_child_ptr)->type = DELETED_ELEMENT;

			struct Bin_tree_elem* new_right_child = (expression_elem_ptr->left_child_ptr)->right_child_ptr;
			struct Bin_tree_elem* new_left_child = (expression_elem_ptr->left_child_ptr)->left_child_ptr;

			expression_elem_ptr->left_child_ptr = new_left_child;
			expression_elem_ptr->right_child_ptr = new_right_child;

			*is_optimised_ptr = true;

			return return_code;
		}
		else 
		{
			return_code |= one_optimisation(expression_elem_ptr->right_child_ptr, is_optimised_ptr);
		}
	}

	return return_code;
}


err_t constant_optimisation(struct Bin_tree_elem* expression_elem_ptr, bool* is_optimised_ptr)
{
	err_t return_code = NO_ERROR;
	if ((expression_elem_ptr->type == NUM) || (expression_elem_ptr->type == VAR)) 
	{
		return return_code;
	}
	if (expression_elem_ptr->type == UN_OP)
	{
		if ((expression_elem_ptr->left_child_ptr)->type == NUM)
		{
			expression_elem_ptr->elem =  eval(expression_elem_ptr, 0);
			expression_elem_ptr->type = NUM;

			delete_subtree(expression_elem_ptr->left_child_ptr);

			expression_elem_ptr->left_child_ptr = NULL;
			expression_elem_ptr->right_child_ptr = NULL;

			*is_optimised_ptr = true;
			return return_code;
		}
		else 
		{
			return_code |= constant_optimisation(expression_elem_ptr->left_child_ptr, is_optimised_ptr); 
		}

		
		return return_code; 
	}

	if (expression_elem_ptr->type == BIN_OP)
	{
		if (((expression_elem_ptr->left_child_ptr)->type == NUM) && ((expression_elem_ptr->right_child_ptr)->type == NUM))
		{
			expression_elem_ptr->elem =  eval(expression_elem_ptr, 0);
			expression_elem_ptr->type = NUM;

			delete_subtree(expression_elem_ptr->left_child_ptr);
			delete_subtree(expression_elem_ptr->right_child_ptr);

			//(expression_elem_ptr->left_child_ptr)->type = DELETED_ELEMENT;
			//(expression_elem_ptr->right_child_ptr)->type = DELETED_ELEMENT;

			expression_elem_ptr->left_child_ptr = NULL;
			expression_elem_ptr->right_child_ptr = NULL;

			*is_optimised_ptr = true;
			return return_code;
		}
		else 
		{
			return_code |= constant_optimisation(expression_elem_ptr->left_child_ptr, is_optimised_ptr);
			return_code |= constant_optimisation(expression_elem_ptr->right_child_ptr, is_optimised_ptr);
		}
	}

	return return_code;
}


err_t x_sub_div_optimisation(struct Bin_tree_elem* expression_elem_ptr, bool* is_optimised_ptr)
{
	err_t return_code = NO_ERROR;

	if ((expression_elem_ptr->type == NUM) || (expression_elem_ptr->type == VAR)) 
	{
		return return_code;
	}
	if (expression_elem_ptr->type == UN_OP)
	{
		return_code |= x_sub_div_optimisation(expression_elem_ptr->left_child_ptr, is_optimised_ptr);
		return return_code;
	}

	if ((expression_elem_ptr->type == BIN_OP) && ((expression_elem_ptr->elem == SUB) || (expression_elem_ptr->elem == DIV)) && ((expression_elem_ptr->left_child_ptr)->type == VAR) && ((expression_elem_ptr->right_child_ptr)->type == VAR))
	{
		printf("x sub div optimisation start\n");
		printf("id = %zd", expression_elem_ptr->id);
		if (expression_elem_ptr->elem == SUB)
		{
			printf("SUB BIN_OP\n");
			expression_elem_ptr->elem = 0;
		}
		else 
		{
			expression_elem_ptr->elem = 1;
		}

		expression_elem_ptr->type = NUM;

		delete_subtree(expression_elem_ptr->left_child_ptr);
		delete_subtree(expression_elem_ptr->right_child_ptr);
		expression_elem_ptr->left_child_ptr = NULL;
		expression_elem_ptr->right_child_ptr = NULL;

		return return_code;
	}
	
	if (expression_elem_ptr->type == BIN_OP)
	{
		return_code |= x_sub_div_optimisation(expression_elem_ptr->left_child_ptr, is_optimised_ptr);
		return_code |= x_sub_div_optimisation(expression_elem_ptr->right_child_ptr, is_optimised_ptr);
	}

	return return_code;
}


err_t optimize_expression(struct Bin_tree* expression_ptr)
{
	err_t return_code = NO_ERROR;
	bool is_optimised = false;

	while (true)
	{
		zero_optimisation(expression_ptr->root_ptr, &is_optimised);
		one_optimisation(expression_ptr->root_ptr, &is_optimised);

		if (!is_optimised)
		{
			return return_code;
		}

		is_optimised = false;
	}

	return return_code;
}
