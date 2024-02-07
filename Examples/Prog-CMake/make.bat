rmdir /q /s windows
mkdir windows
cmake -S . -B windows
cmake --build windows