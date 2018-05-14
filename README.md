# Java Compiler
For building the project navigate to the main directory and write run
```
make
```
Note: Please run
```
make clean
```
before pushing your work.


## Using CMake

Use CMake to configure a project according to your environment and available building tools. Just use:

```
cmake ./
cname --build ./
```

Then build the created project.

- On Windows: You will get MSVC solution that you can open and work with in Visual Studio.

## Run the Project

```
cd bin
LexicalAnalyzerGenerator files/rules.txt
LexicalAnalyzer files/prog.c
```

## Install GCC 5.X

sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install gcc-5 g++-5

sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 60 --slave /usr/bin/g++ g++ /usr/bin/g++-5

## Using FLEX


```
apt-get install flex

cd flex

cmake ./

cmake --build ./

./build/bin/LexicalAnalyzerFlex prog
```



