

files=main.cpp shaderprintf.h model.hpp utils.hpp animate.hpp shader.hpp impl.o callbacks.hpp renderer.hpp museumWallsCoordinates.hpp museum.hpp camera.hpp model_instance.hpp main.hpp

ifeq ($(OS), Windows_NT)

main: $(files)
	g++ -omain main.cpp impl.o -std=c++11 -L.\windows\lib -lglfw3 -lgdi32 -lopengl32 -lglew32 -g -I windows/include -U __STRICT_ANSI__


impl.o: impl.cpp gltf/win_fix.h
	g++ -std=c++11 -c impl.cpp -U __STRICT_ANSI__

else

main: $(files)
	g++ -omain main.cpp impl.o -std=c++11 -lGLEW -lGL -lglfw -g

impl.o: impl.cpp
	g++ -c impl.cpp -g

endif