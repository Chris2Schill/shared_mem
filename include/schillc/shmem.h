#include <memory>
#include <cassert>
#include <string>

namespace schillc {

struct shared_mem_region_impl;

struct shared_mem_region{
    shared_mem_region(const char* name, std::size_t size);

    void* get_address();
    const char* get_name() const;
    std::size_t size() const;
    void delete_on_destruction(bool del);

    std::shared_ptr<shared_mem_region_impl> pimpl;
};

template<typename T>
class shared_mem{
public:
    explicit shared_mem(const char* name) : region(name, sizeof(T)) {};
    explicit shared_mem(const std::string& name) : shared_mem(name.c_str()) {};

    const char* get_name() const { return region.get_name(); }
    T* get_address() { return reinterpret_cast<T*>(region.get_address()); }
    std::size_t size() const { return region.size(); }

    T* operator->() { return get_address(); }

    template<typename U>
    U* as() {
        assert(size() >= sizeof(U));
        return reinterpret_cast<U*>(get_address());
    }

    void delete_on_destruction(bool del) { region.delete_on_destruction(del); }
    void zeroize() { memset(get_address(), 0, size()); }

private:
    shared_mem_region region;
};

template<typename T, typename ...Args>
shared_mem<T> make_shared_mem(const char* name, Args... args) {
    shared_mem<T> shm(name);
    void* ptr = shm.get_address();
    T* tptr = new (ptr) T(args...);
    return shm;
}

}
