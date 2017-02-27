rm ENGINE
rm GENERATOR
mv buid/*.3d ./
rm -rf build
cp engine.cpp main.cpp
mkdir build
cd build/
cmake ../
make
mv TP1-CG ../ENGINE
cd ..
rm -rf build
