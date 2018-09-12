#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

int main(int argc, char* argv[]){
    string name_ = argv[1];
    string template_ = "g++ " + name_ + " -o e -lgdi32";
    system(template_.c_str());
    return 0;
}