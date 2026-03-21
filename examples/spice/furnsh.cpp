#ifndef AST_NO_CSPICE
#include "SpiceUsr.h"
#include <clocale>
#include <string>
#include <vector>
#include <iostream>
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/Environment.hpp"

AST_USING_NAMESPACE
int main(){
    setlocale(LC_ALL, ".UTF-8");
    if(aIsCI()) return 0;
    std::string spkfile = "data/Test/kernels/spk/de430.bsp";
    std::string pckfile = "data/Test/kernels/pck/pck00011.tpc";
    std::vector<std::string> kernels = {spkfile, pckfile};
    for(auto& kernel : kernels){
        if(!fs::exists(kernel)){
            std::cout << "Kernel file " << kernel << " does not exist." << std::endl;
        }else{
            std::cout << "Kernel file " << kernel << " exists." << std::endl;
        }
        size_t size = fs::file_size(kernel);
        std::cout << "Kernel file " << kernel << " size is " << size << " bytes." << std::endl;
    }
    for(auto& kernel : kernels){
        std::cout << "Loading kernel file " << kernel << std::endl;
        furnsh_c(kernel.c_str());
        std::cout << "Kernel file " << kernel << " loaded." << std::endl;
    }
}
#else
int main() {}
#endif