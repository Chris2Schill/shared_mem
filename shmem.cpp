#include "shmem.h"

#include <iostream>

#include <boost/interprocess/creation_tags.hpp>
#include <boost/interprocess/detail/os_file_functions.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

namespace bi = boost::interprocess;

namespace schillc {

struct shared_mem_impl{
    shared_mem_impl(const char* name, std::size_t size)
    {
        std::cout << "shared_mem " << name << "...";
        shm = bi::shared_memory_object(bi::open_or_create, name, bi::read_write, size);
        shm.truncate(size);
        region = bi::mapped_region(shm, bi::read_write);
        std::cout << "attached\n";
    }
    bi::shared_memory_object shm;
    bi::mapped_region region;
};

shared_mem::shared_mem(const std::string& name, std::size_t size)
    :shared_mem(name.c_str(), size) {}

shared_mem::shared_mem(const char* name, std::size_t size)
    : pimpl(std::make_shared<shared_mem_impl>(name, size)) {}

shared_mem::~shared_mem() {
    // std::cout << "shm_unlink: " << pimpl->shm.get_name() << "...";
    // bool success = bi::shared_memory_object::remove(pimpl->shm.get_name());
    // std::cout << (success ? "removed" : "detached") << "\n";
}

std::size_t shared_mem::size() const {
    return pimpl->region.get_size();
}

const char* shared_mem::get_name() const {
    return pimpl->shm.get_name();
}

void* shared_mem::get_address() {
    return pimpl->region.get_address();
}


} // end namespace shillc

