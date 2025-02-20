mkdir -p ./build_release

cd ./build_release
conan install -of ../build_release --profile:all=../build/conanprofile_mac --profile=../build/conanprofile_mac ../build
cmake -G "Unix Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DBUILD_INTEGRATION_TESTS=true ..
cmake --build . --parallel 7 
