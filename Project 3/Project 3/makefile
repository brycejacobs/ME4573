CC= g++
EXEC= project3
OBJS= main.o shaderloader.o plane.o image.o obj.o math3d.o view.o viewglut.o
INCLUDE= -I/opt/local/include/
LIBS=  -L/opt/local/lib -lGLEW -lpng -framework GLUT -framework OpenGL 

$(EXEC) : $(OBJS)
	$(CC) -o $(EXEC) $(OBJS) $(LIBS)

%.o : %.c
	$(CC) -c $< $(INCLUDE)

%.o : %.cpp
	$(CC) -c $< $(INCLUDE)
clean :
	rm $(EXEC) $(OBJS)