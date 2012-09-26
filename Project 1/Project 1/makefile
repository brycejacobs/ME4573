CC= g++
EXEC= render
OBJS= main.o LoadedObject.o FileLoader.o 
LIBS=  -L/opt/local/lib -lGLEW -framework GLUT -framework OpenGL

$(EXEC) : $(OBJS)
	$(CC) -o $(EXEC) $(OBJS) $(LIBS)

%.o : %.c
	$(CC) -c $ <

clean :
	rm -rf $(EXEC) $(OBJS)