mkdir -p /source/build_release

cd /source/build_release
conan install -of /source/build_release --profile=/source/build/conanprofile ../build
cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE="/source/build_release/build/Release/generators/conan_toolchain.cmake" -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DBUILD_INTEGRATION_TESTS=true /source
cmake --build . --parallel 7 
