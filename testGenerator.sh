rm ENGINE
rm GENERATOR
mv *.3d objects/
cp generator.cpp main.cpp
mkdir build
cd build/
cmake ../
make
mv TP1-CG ../GENERATOR
cd ..
rm -rf build
clear
echo "How to use:"
echo "./GENERATOR plane x_dim z_dim filename"
echo "./GENERATOR box x_dim y_dim z_dim slices filename"
echo "./GENERATOR cone radius height slices stacks filename"
echo ""
