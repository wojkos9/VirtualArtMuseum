
main: main.cpp shaderprintf.h model.hpp utils.hpp animate.hpp shader.hpp
	g++ -omain main.cpp impl.o -std=c++11 -lGLEW -lGL -lglfw -g

test: test.cpp
	g++ -otest test.cpp impl.o -g