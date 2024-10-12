#include <schillc/shmem.h>

#include <iostream>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

namespace bi = boost::interprocess;

namespace schillc {


struct shared_mem_region_impl{
    shared_mem_region_impl(const char* name, std::size_t size)
    {
        std::cout << "shared_mem " << name << "...";
        shm = bi::shared_memory_object(bi::open_or_create, name, bi::read_write, size);
        shm.truncate(size);
        region = bi::mapped_region(shm, bi::read_write);
        std::cout << "attached\n";
    }
    ~shared_mem_region_impl() {
        if (del_on_destruction) {
            std::cout << "shm_unlink: " << shm.get_name() << "...";
            bool success = bi::shared_memory_object::remove(shm.get_name());
            std::cout << (success ? "removed" : "detached") << "\n";
        }
    }

    bi::shared_memory_object shm;
    bi::mapped_region region;
    bool del_on_destruction = true;
};

shared_mem_region::shared_mem_region(const char* name, std::size_t size)
    :pimpl(std::make_shared<shared_mem_region_impl>(name,size))
{}

const char* shared_mem_region::get_name() const {
    return pimpl->shm.get_name();
}

void* shared_mem_region::get_address() {
    return pimpl->region.get_address();
}

std::size_t shared_mem_region::size() const{
    return pimpl->region.get_size();
}

void shared_mem_region::delete_on_destruction(bool del) {
    pimpl->del_on_destruction = del;
}

} // end namespace shillc

