#include <iostream>

#include <schillc/shmem.h>
#include "data.h"

int main(int argc, char** argv) {

    const char* name = argv[1];

    auto data = schillc::shared_mem<Data>(name);
    do{
        std::cout << "data.text=" << data->text << "\n";
    }
    while(getchar() != 'q');


    std::getchar();
}
