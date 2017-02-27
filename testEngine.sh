mv build/*.3d ../
rm -rf build
mv main.cpp generator.cpp
mv engine.cpp main.cpp
mkdir build
cd build/
cmake ../
make
cp *.3d ./build/
