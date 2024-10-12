#include <memory>
#include <cassert>

namespace schillc {

struct shared_mem_impl;

class shared_mem{
public:
    shared_mem(const char*, std::size_t size);
    shared_mem(const std::string& name, std::size_t size);
    ~shared_mem();


    const char* get_name() const;

    void* get_address();

    template<typename T>
    T* as() {
        assert(size() >= sizeof(T));
        return reinterpret_cast<T*>(get_address());
    }

    std::size_t size() const;

private:
    std::shared_ptr<shared_mem_impl> pimpl;
};


// template<typename T, typename... Args>
// make_shared_mem() {
//
// }

}
