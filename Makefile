goal: ptg

ptg: ptg.cpp
	g++ -framework OpenGL -framework GLUT glui/lib/libglui.a ptg.cpp -o ptg

