goal: ptg

ptg: ptg.cpp mesh.cpp
	g++ -framework OpenGL -framework GLUT -g glui/lib/libglui.a ptg.cpp mesh.cpp -o ptg

mesh.cpp: mesh.h