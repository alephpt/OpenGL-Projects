g++ main.cpp cavegen/components/glad.c cavegen/*.cpp cavegen/*/*/*.cpp -lglfw -ldl -pthread --debug -fsanitize=address -o ~/mine/cavegen