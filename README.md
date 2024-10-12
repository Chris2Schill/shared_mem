
# Shared Memory made easy.

## Create as generic block of data
```C++
auto data = schillc::make_shared_mem<char[1024]>(name, text);
data[0] = 'A'; // access like an array
```

## Create as struct
```C++
struct Data { int x; }
auto data = schillc::make_shared_mem<Data>(name);
data->x = 69; // access via Data struct
```
```C++
struct Data { int x; }
auto data = schillc::make_shared_mem<Data[5]>(name);
data[0]->x = 69; // access array via Data struct
```

By default the shared memory is cleaned up when the last
reference to data is destroyed. Optionally leave the shared mem block open after handle
is destroyed.
```C++
auto data = schillc::make_shared_mem<Data>(name);
data.delete_on_destruction(false);
```

## Constructors and Placement new
When open/creating shared mem generally schill::make_shared_mem<T>() should be used.
schill::make_shared_mem<T>() will use placement new to construct (in place!) the type T
over the shared memory segment. When attaching this way, the destructor of T will also
be called when the last reference to the shared mem is destroyed.
```C++
struct Data {
    Data(int x) : x(x) {}
    int x;
};
auto data = schillc::make_shared_mem<Data>(name, 69); // Paremeters 1 - (n-1) are forwarded to the constructor of Data
data.x == 69; // true
```
// This also works for array types. Placement new will be called for each array index.
