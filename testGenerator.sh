rm ENGINE
rm GENERATOR
mv buid/*.3d ./
rm -rf build
cp generator.cpp main.cpp
mkdir build
cd build/
cmake ../
make
mv TP1-CG ../GENERATOR
cd ..
rm -rf build
