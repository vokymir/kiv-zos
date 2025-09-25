cd build
echo "==== CMAKE ===="
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
echo "== VALGRIND ==="
valgrind --leak-check=full --show-leak-kinds=all ./semestralka
echo "===== APP ====="
./semestralka cp -h
echo "===== END ====="
cd ..
