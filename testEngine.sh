rm ENGINE
rm GENERATOR
mv *.3d objects/
cp engine.cpp main.cpp
mkdir build
cd build/
cmake ../
make
mv TP1-CG ../ENGINE
cd ..
rm -rf build
clear
echo "How to use:"
echo "./ENGINE scenes/[desired scene]"
echo ""
