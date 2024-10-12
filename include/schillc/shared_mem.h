#include <memory>
#include <cassert>
#include <string>
#include <type_traits>
#include <stdexcept>

// TODO REMOVE
#include <iostream>
namespace schillc {

template<typename T, std::size_t N>
inline constexpr std::size_t array_size(T const(&)[N]) {
    return N;
}

struct shared_mem_region_impl;

struct shared_mem_region{
    shared_mem_region() = default;
    shared_mem_region(const char* name, std::size_t size);

    void* get_address() const;
    const char* get_name() const;
    std::size_t size() const;
    void delete_on_destruction(bool del);
    bool valid() const { return pimpl != nullptr; }

    std::shared_ptr<shared_mem_region_impl> pimpl;
};

template<typename T>
class shared_mem{
public:
    using value_type = std::remove_all_extents_t<T>;

    shared_mem() = default;
    explicit shared_mem(const char* name) : region(name, sizeof(T)) {};
    explicit shared_mem(const std::string& name) : shared_mem(name.c_str()) {};

    ~shared_mem() {
        // Its the last reference, delete if applicable
        if (constructed && region.pimpl.use_count() == 1) {
            delete_shared_mem(*this);
        }
    }

    const char* get_name() const { return region.get_name(); }

    // Returns a pointer to the start of the shared memory block, casted to the value_type
    T* get_address() { return const_cast<T*>(const_cast<const shared_mem<T>*>(this)->get_address()); }
    const T* get_address() const { return reinterpret_cast<T*>(region.get_address()); }

    // Returns the size in bytes of the shared memory block
    constexpr std::size_t size() const { return sizeof(T); }

    T* operator->() { return const_cast<T*>(const_cast<const shared_mem<T>*>(this)->operator->()); }
    const T* operator->() const { return get_address(); }

    T& operator*() { return const_cast<T&>(const_cast<const shared_mem<T>*>(this)->operator*()); }
    const T& operator*() const { return *get_address(); }

    const auto& operator[](std::size_t idx) const {
        return operator[](idx);
    }

    bool valid() const { return region.valid(); }

    auto& operator[](std::size_t idx) {
        if constexpr(std::is_array_v<T>) {
            return *(reinterpret_cast<std::remove_all_extents_t<T>*>(get_address())+idx);
        }
        else {
            // Should never get here. Compiler should not be able to
            // deduce auto& if T is not an array and thus should
            // be a compiler error. Throw exception here just in case...
            throw std::runtime_error("Cannot use [] operator, T is not an array type");
        }
    }

    // If T is an array type, returns the number of elements in the array
    constexpr std::size_t count() const {
        return array_size(**this);
    }

    template<typename U>
    U* as() {
        assert(size() >= sizeof(U));
        return reinterpret_cast<U*>(get_address());
    }

    void delete_on_destruction(bool del) { region.delete_on_destruction(del); }
    void zeroize() { memset(get_address(), 0, size()); }

    template<typename U, typename ...Args>
    friend std::enable_if_t<not std::is_array_v<U>, shared_mem<U>>
    make_shared_mem(const char*, Args... args);

    template<typename U, typename ...Args>
    friend std::enable_if_t<std::is_array_v<U>, shared_mem<U>>
    make_shared_mem(const char*, Args... args);

private:
    shared_mem_region region;
    bool constructed = false; // true if placement new was used to construct on the shm block
};

// schillc::make_shared_mem() is anogalous to std::make_shared, i.e.
// if you want to construct on object in place on the shared memory block
// then use this family of functions

// For non-array types.
template<typename T, typename ...Args>
std::enable_if_t<not std::is_array_v<T>, shared_mem<T>>
make_shared_mem(const char* name, Args... args) {
    shared_mem<T> shm(name);
    void* ptr = shm.get_address();
    T* tptr = new (ptr) T(args...);
    shm.constructed = true;
    return shm;
}

// For array types. construct each index with args
template<typename T, typename ...Args>
std::enable_if_t<std::is_array_v<T>, shared_mem<T>>
make_shared_mem(const char* name, Args... args) {
    using value_type = std::remove_all_extents_t<T>;
    shared_mem<T> shm(name);
    for (std::size_t i = 0; i < shm.count(); ++i) {
        value_type* ptr = &shm[i];
        value_type* tptr = new (ptr) value_type(args...);
    }
    shm.constructed = true;
    return shm;
}


// For non-array types
template<typename T>
std::enable_if_t<not std::is_array_v<T>, void>
delete_shared_mem(shared_mem<T>& shm) {
    // Calls to the destructor may be optimized out.
    // Refactor destructor to a funtion that operates on a volatile as a work-around
    shm->~T();
}

// For array types. deconstruct each index
template<typename T>
std::enable_if_t<std::is_array_v<T>, void>
delete_shared_mem(shared_mem<T>& shm) {
    using value_type = std::remove_all_extents_t<T>;
    std::size_t count = array_size(*shm);
    for (std::size_t i = 0; i < count; ++i) {
        value_type* ptr = &shm[i];
        ptr->~value_type();
    }
}

}
