For audio utils, you need to run the following in PowerShell first:

cd C:[Desired Path]
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

cd C:\PSU\Utils\vcpkg
.\vcpkg install libsndfile:x64-windows mpg123:x64-windows
(This step is exptected to take awhile)

.\vcpkg integrate install
