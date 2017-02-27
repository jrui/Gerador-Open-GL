mv build/*.3d ./
rm -rf build
mv main.cpp engine.cpp
mv generator.cpp main.cpp
mkdir build
cp * build
cd build/
cmake ./
make
