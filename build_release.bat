mkdir .\build_release

cd .\build_release
conan install -of ..\build_release --build=missing --profile:all=..\build\conanprofile_win ..\build
cmake -DCMAKE_TOOLCHAIN_FILE="..\build_release\build\generators\conan_toolchain.cmake" -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Release -DBUILD_INTEGRATION_TESTS=true ..
cmake --build . --parallel 7 --config Release
