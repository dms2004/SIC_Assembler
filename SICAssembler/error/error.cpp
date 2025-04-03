#include <iostream>
#include <string>

bool error(std::string msg){
    std::cerr << "Error: " << msg << std::endl;
    return false;
}
