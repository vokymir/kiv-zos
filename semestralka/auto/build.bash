cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
ln -sf "$PWD/compile_commands.json ../compile_commands.json
cd ..
