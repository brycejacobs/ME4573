CC= g++
EXEC=project2_render
OBJS=main.o shaderloader.o plane.o image.o obj.o 
LIBS=  -L/opt/local/lib -lGLEW -framework GLUT -framework OpenGL

$(EXEC) : $(OBJS)
	$(CC) -o $(EXEC) $(OBJS) $(LIBS)

%.o : %.c
	$(CC) -c $ <

clean :
	rm -rf $(EXEC) $(OBJS)