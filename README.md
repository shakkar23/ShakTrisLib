# ShakTrisImpl
### This is an implmementation of Shaktris to be used as a submodule in other projects.

## Usage
#### To use this submodule in your project, add it as a submodule to your project.
#### To include this submodule in your c++ project, add the following line to your CMakeLists.txt file:
```cmake
add_subdirectory(ShakTrisLib)

target_link_libraries(${PROJECT_NAME} ShakTrisLib)
```

### You will have access to all of the header files that are in the src directory of this library and you can include them in your project as needed.

## Building
### To build this library, you will need to have access to C\+\+20 features. g\+\+-12 or clang-16 and beyond will work for this project.

## Features
- [x] Efficient BitBoard implementation using bit operations
- [x] Efficient Move Generation
- [x] Efficient Random Piece Generation (using an LCG)
- [ ] BitPiece implementation
