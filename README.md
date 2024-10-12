
```
// Create shared memory...

// as generic block of data
auto data = schillc::make_shared_mem<char[1000]>(name, text);
data[0] = 'A'; // access like an array

// as struct
struct Data { int x; } 
auto data = schillc::make_shared_mem<Data>(name, text);
data->x = 69; // access via Data struct

// By default the shared memory is cleaned up when the last
// reference to data is destroyed.

// Optionally leave the shared mem block open after handle
// is destroyed
data.delete_on_destruction(false);
```
