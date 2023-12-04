#ifndef TEXT_H
	#define TEXT_H

	char** init_text(const char* const filename, ssize_t* n_strings_ptr, void** buffer_ptr_to_free);
	void print_text(const char* const * const text, const ssize_t n_strings, FILE* outputfile);
	void free_text(char** text);
	bool check_stdin_valid_symbol(const char symbol);

#endif /* TEXT_H */