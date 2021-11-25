# paths
INCLUDE =../include
MODULES=../modules

# compiler
CC = gcc

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CPPFLAGS = -Werror -Wall  -g3 -I$(INCLUDE)
LDFLAGS = -lm

# Αρχεία .o
OBJS =   test.o $(MODULES)/general.o $(MODULES)/sem_functions.o

# Το εκτελέσιμο πρόγραμμα
EXEC = test
ARGS = 2 2


$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)


clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	./$(EXEC) $(ARGS)

valgrind: run
	valgrind -s ./$(EXEC)