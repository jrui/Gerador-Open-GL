mkdir build
cd build
cmake ../
make
mv ENGINE ../
mv GENERATOR ../
cd ..
rm -rf build
mv *.3d ./objects
mv *.xml ./scenes
clear

echo ""
echo "Generator commands:"
echo "./GENERATOR plane x_dim z_dim filename"
echo "./GENERATOR box x_dim y_dim z_dim slices filename"
echo "./GENERATOR cone radius height slices stacks filename"
echo "./GENERATOR sphere radius slices stacks filename"
echo "./GENERATOR cylinder radius height slices stacks filename"
echo "./GENERATOR ring innerRadius outerRadius slices filename"
echo "./GENERATOR asteroid innerRadius outerRadius quantity filename"
echo "./GENERATOR satelites innerRadius outerRadius quantity max min filename"
echo "./GENERATOR bezier tesselation filetoread filetowrite"
echo ""
echo ""
echo "Engine commands:"
echo "./ENGINE scenes/[desired scene]"
echo ""
echo "  Commands:"
echo "    Press [f] or [F] to show filled."
echo "    Press [l] or [L] to show lines."
echo "    Press [p] or [P] to show points."
echo "    Use [PGDN] to move object closer or [PGUP] to move further."
echo "    Use the arrow keys to move through the scene."
echo "You can easily interact with the scene by using the mouse."
echo ""
