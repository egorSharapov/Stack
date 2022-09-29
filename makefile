CXFLAGS = 	-Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations 		\
		  	-Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion	\
			-Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers 		\
			-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits         \
			-Wwrite-strings -D_EJUDGE_CLIENT_SIDE

release:
	g++ $(CXFLAGS) main.cpp stack_functions.cpp -DNDEBUG -o prog

debug:
	g++ $(CXFLAGS) main.cpp stack_functions.cpp -DHASHCHECK -DCANARYCHECK -DDEBUG check_stack.cpp -o prog

canary:
	g++ $(CXFLAGS) main.cpp stack_functions.cpp -DCANARYCHECK -DDEBUG check_stack.cpp -o prog

hash:
	g++ $(CXFLAGS) main.cpp stack_functions.cpp -DHASHCHECK -DDEBUG check_stack.cpp -o prog

