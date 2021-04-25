goal: ptg

ptg: ptg.cpp
	g++ -framework OpenGL -framework GLUT -g glui/lib/libglui.a ptg.cpp -o ptg

