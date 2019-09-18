set -x
rm out
rm image.ppm
g++ -std=c++11 -o out ray_tracer.cpp geometry.h structs.h
./out
display image.ppm