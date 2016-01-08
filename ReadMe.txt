
#--------------------------------
# ALL OS: setup QT
#--------------------------------

Choose either QT4 or QT5.
The Open Source Version can be downloaded from http://www.qt.io/download-open-source/.

On Linux/OSX installing with a package management system is recommended.

Rename the respective CMakeListsXX.txt to CMakeLists.txt.



#--------------------------------
# ALL OSX: Environment variables
#--------------------------------

Before running cmake you have to set a environment variable QTX_QMAKE_DIR pointing
to the directory that contains the respective qmake executable.

e.g. in cshell:

         setenv QT5_QMAKE_DIR "/usr/local/Cellar/qt5/5.5.1_2/"
   or    setenv QT4_QMAKE_DIR "/usr/local/bin/"


#--------------------------------
# ALL OS: Debugging
#--------------------------------

The given CMakeLists files is set for a Release build.
To make debugging available, you should set the build type to Debug.
(as marked in the CMakeLists file).


#--------------------------------
# OSX
#--------------------------------

INFO: OpenMP does not run with the newest version of GCC

# e.g. install version 4.9 with homebrew
brew install gcc49

# set compiler environment variables in shell (here csh)
setenv CXX "/usr/local/bin/g++-4.9"
setenv CC "/usr/local/bin/gcc-4.9"



#--------------------------------
# Windows
#--------------------------------

To run the run the application you have to make the dynamic libraries of QT
available to your programm by

   1) either copying all QT (both Release and Debug) DLLs
      from QT/bin to your executable folder.

   2) by adding QT/bin to your PATH




