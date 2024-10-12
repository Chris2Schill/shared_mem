#include <iostream>

#include "shmem.h"
#include "data.h"

using SharedData = schillc::shared_mem;

int main(int argc, char** argv) {

    const char* name = argv[1];

    SharedData shm(name, sizeof(Data));
    Data* data = shm.as<Data>();

    do{
        std::cout << "data.text=" << data->text << "\n";
    }
    while(getchar() != 'q');

    // schillc::make_shared_mem<Data>();


    std::getchar();
}
