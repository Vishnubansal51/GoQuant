# GoQuant
 work on  derbit api

Steo 1 :
Install CMake:
o	Download from CMake and install.
o	Add CMake to your PATH so it can be used from the terminal.

Step 2: Install vcpkg (Visual C++ Package Manager)
o	Clone vcpkg:  git clone https://github.com/microsoft/vcpkg.git
o Navigate into the vcpkg directory: cd vcpkg
o Build vcpkg by running: .\bootstrap-vcpkg.bat
o.integrate vcpkg with Visual Studio Code: .\vcpkg integrate install


Step 3: Install the Required Libraries Using vcpkg
o Install cpr (HTTP requests library): .\vcpkg install cpr
o Install websocketpp (WebSocket library): .\vcpkg install websocketpp

Step 4: Set Up Visual Studio Code to Use vcpkg Libraries
o	Set up CMakeLists.txt to link to vcpkg libraries 
for example please see my CMakeLists.txt

Step 5: Build and Run the Project
o cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake
Replace [path to vcpkg] with the path where you cloned vcpkg.

o Now, build the project:cmake --build build
o Run your program: ./build/FileName
change FileName  according to ur .exe file


