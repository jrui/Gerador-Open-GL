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
echo "Commands:"
echo "Press [f] or [F] to show filled."
echo "Press [l] or [L] to show lines."
echo "Press [p] or [P] to show points."
echo "Use arrow keys to move object. [UP | DOWN | LEFT | RIGHT]"
echo "You can easily interact with the scene by using the mouse."
echo ""
