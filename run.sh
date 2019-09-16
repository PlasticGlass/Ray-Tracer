set -x
rm out
rm image.ppm
g++ -o out ray_tracer.cpp geometry.h
./out
display image.ppm