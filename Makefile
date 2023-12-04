CXXFLAGS = -g -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE 
CXX = g++

OBJDIR = object_files

READABLE_PRINT_BIN_TREE_FILE = True

OBJECTS = $(OBJDIR)/bin_tree.o $(OBJDIR)/graph.o $(OBJDIR)/text.o 

ifeq ($(TEST), True)
	OBJECTS += $(OBJDIR)/test.o
else 
	OBJECTS += $(OBJDIR)/main.o
endif

ifeq ($(READABLE_PRINT_BIN_TREE_FILE), True)
	CXXFLAGS += -DREADABLE_PRINT_BIN_TREE_FILE
endif

all: main.exe
	main.exe 

main.exe: $(OBJECTS)
	@echo $(CXX) $^ [CXXFLAGS] -o $@ 
	@$(CXX) $^ $(CXXFLAGS) -o $@



$(OBJDIR)/%.o: %.cpp
	@echo [CXX] [CXXFLAGS] -c $@ -o $< 
	@$(CXX) $(CXXFLAGS) -c -o  $@ $< 

.PHONY: clean_exe
clean_exe:
	@echo "all .exe files are cleaned"
	@echo ""
	
	rm -rf *.exe

.PHONY: clean_object_files
clean_object_files:
	@echo "all object files (.o) are cleaned"
	@echo ""
	
	rm -rf *.o 

.PHONY: clean
clean:
	rm -rf *.o
	rm -rf *.exe

.PHONY: create_documentation
create_docs: 
	doxygen Doxyfile
	@echo ""
	@echo "documentation is updated"
