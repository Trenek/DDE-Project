# Jak uruchomić program?
## Upewnij się, że CAPD jest pobrane
`git submodule update --init --remote --recursive`  

## Zbuduj kod używając CMake'a
`cmake -B build`  
`cmake --build build --target all`

## Uruchom program
Po kompilacji plik wykonywalny znajduje się w folderze `build/simulations`
