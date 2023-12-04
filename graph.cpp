#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <stdlib.h>
#include "include/bin_tree.h"
#include "include/graph.h"
#include "include/text.h"

const ssize_t MAX_NAME_SIZE = 1000;


#define CREATE_DOT_LINE(command_string) fprintf(dot_file, "" command_string "\n");
#define CREATE_HTML_LINE(command_string) fprintf(html_file, "" command_string "\n");

#define RUN_DOT_FILE(graph_filename, dot_filename, graph_file_extension)  								\
{																										\
	char system_command[MAX_COMMANDNAME_SIZE] = {};														\
	sprintf(system_command, "dot -T%s %s -o %s", graph_file_extension, dot_filename, graph_filename);	\
	system(system_command);																				\
}																										\


#define RUN_HTML_FILE(graph_folder, html_filename)												\
{																								\
	char system_command[MAX_COMMANDNAME_SIZE] = {};												\
	sprintf(system_command, "cd %s && %s", graph_folder, html_filename);						\
	printf("%s", system_command);																\
	system(system_command);																		\
}																								\


FILE* open_file(const char* const filename, const char* const modificator)
{
	FILE* inputfile = fopen(filename, modificator);
	assert(inputfile);
	return inputfile;
}


err_t create_one_graph(struct Bin_tree* const bin_tree_ptr, ssize_t* graph_number_ptr, char *  graph_filepath, FILE* html_file)	
{	
	CHECK_BIN_TREE();

	sprintf(graph_filepath, "graph/graph%zd.png", *graph_number_ptr);				
																						
	write_to_dot_file(bin_tree_ptr);			
																																											
	RUN_DOT_FILE(graph_filepath, dot_filepath, graph_file_extension)				
																		
	CREATE_HTML_LINE("<section>")				
	fprintf(html_file, "<img src=graph%zd.png alt=\"struct List list [%p]\"/>\n", *graph_number_ptr, bin_tree_ptr);	
	CREATE_HTML_LINE("</section>")					
																						
	(*graph_number_ptr)++;	

	return return_code;
}


static char*  get_bin_tree_elem_name(const struct Bin_tree_elem* current_elem_ptr, struct Bin_tree* bin_tree_ptr)
{
	char* name = (char*) calloc(MAX_NAME_SIZE, sizeof(char)); 

	append_new_dynamic_ptr(name, &(bin_tree_ptr->ptrs));

	if (current_elem_ptr->type == NUM)
	{
		sprintf(name, "%lf", current_elem_ptr->elem);
		return name;
	}
	else if (current_elem_ptr->type == BIN_OP)
	{
		     if (current_elem_ptr->elem == ADD) sprintf(name, "+");
		else if (current_elem_ptr->elem == SUB) sprintf(name, "-");
		else if (current_elem_ptr->elem == MUL) sprintf(name, "*");
		else if (current_elem_ptr->elem == DIV) sprintf(name, "/");
		else if (current_elem_ptr->elem == LOG) sprintf(name, "log");
		else if (current_elem_ptr->elem == POW) sprintf(name, "^");
	}
	else if (current_elem_ptr->type == UN_OP)
	{
			 if (current_elem_ptr->elem == SQRT) sprintf(name, "sqrt");
		else if (current_elem_ptr->elem == SIN)  sprintf(name, "sin");
		else if (current_elem_ptr->elem == COS)  sprintf(name, "cos");
		else if (current_elem_ptr->elem == EXP)  sprintf(name, "exp");
		else if (current_elem_ptr->elem == LN)   sprintf(name, "ln");

	}
	else
	{
		sprintf(name, "x");
	}

	return name;
}



static err_t draw_all_bin_tree_elem_dot(struct Bin_tree* bin_tree_ptr, FILE* const dot_file)
{
	CHECK_BIN_TREE();

	struct Bin_tree_elem* current_elem_ptr = bin_tree_ptr->head_ptr;

	while (current_elem_ptr != NULL)
	{
		if (current_elem_ptr->type != DELETED_ELEMENT)
		{


			fprintf(dot_file, "\"node%zd\"  [ label = \"<f1> %s  | { id = %zd | depth = %zd | self_ptr = [%p]  | left_child_ptr = [%p] | right_child_ptr = [%p] | ", current_elem_ptr->id, get_bin_tree_elem_name(current_elem_ptr, bin_tree_ptr), current_elem_ptr->id, current_elem_ptr->depth_in_tree, current_elem_ptr ,current_elem_ptr->left_child_ptr, current_elem_ptr->right_child_ptr);

			if (current_elem_ptr->type == NUM) fprintf(dot_file, "type = NUM} \" shape = \"Mrecord\" color =\"Green\"];\n");
			if (current_elem_ptr->type == BIN_OP) fprintf(dot_file, "type = BIN_OP} \" shape = \"Mrecord\" color =\"Red\"];\n");
			if (current_elem_ptr->type == UN_OP) fprintf(dot_file, "type = UN_OP} \" shape = \"Mrecord\" color =\"Yellow\"];\n");
			if (current_elem_ptr->type == VAR) fprintf(dot_file, "type = VAR} \" shape = \"Mrecord\" color =\"Blue\"];\n");

		}

		current_elem_ptr = current_elem_ptr->next_ptr;
	}

	return return_code;
}


static err_t link_bin_tree_elem_dot(const struct Bin_tree_elem* elem_ptr, FILE* const dot_file)
{
	if ((elem_ptr->left_child_ptr) != NULL)
	{
		fprintf(dot_file, "\"node%zd\" -> \"node%zd\" [color = \"Blue\"];\n", elem_ptr->id, (elem_ptr->left_child_ptr)->id);
		link_bin_tree_elem_dot(elem_ptr->left_child_ptr, dot_file);
	}
	if ((elem_ptr->right_child_ptr) != NULL)
	{
		fprintf(dot_file, "\"node%zd\" -> \"node%zd\" [color = \"Green\"];\n", elem_ptr->id, (elem_ptr->right_child_ptr)->id);
		link_bin_tree_elem_dot(elem_ptr->right_child_ptr, dot_file);
	}

	return NO_ERROR;
}		


err_t write_to_dot_file(struct Bin_tree* const bin_tree_ptr)
{
	CHECK_BIN_TREE();

	FILE* dot_file = open_file(dot_filepath, "w"); 

	assert(dot_file);

	CREATE_DOT_LINE("digraph G {")
	fprintf(dot_file, "rankdir = \"TB\";\n");
	fprintf(dot_file, "graph [splines=ortho];\n");
	CREATE_DOT_LINE("subgraph cluster0 {")

	fprintf(dot_file, "label = \"struct Bin_tree bin_tree [%p]\";", bin_tree_ptr);

	fprintf(dot_file, "fontname=\"Helvetica,Arial,sans-serif\";\n");
	fprintf(dot_file, "node [fontname=\"Helvetica,Arial,sans-serif\" fontsize = \"16\" ];\n");
	fprintf(dot_file, "edge [fontname=\"Helvetica,Arial,sans-serif\"];\n");

	CREATE_DOT_LINE("subgraph cluster1 {")
	fprintf(dot_file, "rankdir = \"TB\";\n");
	fprintf(dot_file, "label = \"bin_tree variables\"");
	fprintf(dot_file, "size [shape=box3d, label=\"size = %zd\"];\n", bin_tree_ptr->size);
	fprintf(dot_file, "tail_ptr [shape=box3d, label=\"tail pointer = [%p]\"];\n", bin_tree_ptr->tail_ptr);
	fprintf(dot_file, "head_ptr [shape=box3d, label=\"head pointer = [%p]\"];\n", bin_tree_ptr->head_ptr);
	fprintf(dot_file, "root_ptr [shape=box3d, label=\"root pointer = [%p]\"];\n", bin_tree_ptr->root_ptr);

	CREATE_DOT_LINE("}")	
	CREATE_DOT_LINE("subgraph cluster2 {")

	fprintf(dot_file, "rankdir = \"BT\";\n");
	fprintf(dot_file, "label = \"bin tree elements\"\n");

	return_code |= draw_all_bin_tree_elem_dot(bin_tree_ptr, dot_file);

	return_code |= link_bin_tree_elem_dot(bin_tree_ptr->root_ptr, dot_file);

	CREATE_DOT_LINE("} } }")

	fclose(dot_file);


	return return_code;
}