#include <schillc/shmem.h>

#include "data.h"

int main(int argc, char** argv) {

    const char* name = argv[1];
    std::string text = argv[2];

    auto data = schillc::make_shared_mem<Data>(name, text);

    std::getchar();
}
