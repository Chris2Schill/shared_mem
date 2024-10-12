#include <iostream>

#include "shmem.h"
#include "data.h"
#include <cstring>

using SharedData = schillc::shared_mem;

int main(int argc, char** argv) {

    const char* name = argv[1];
    std::string text = argv[2];

    SharedData shm(name, sizeof(Data));
    Data* data = shm.as<Data>();


    assert(text.size() < sizeof(data->text));
    strncpy(data->text, text.c_str(), text.size());
    // data->text[3] = '\0';

    std::getchar();
}
