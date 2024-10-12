#include <gtest/gtest.h>

#include <schillc/shared_mem.h>
#include <cstring>

struct Data {
    explicit Data() : Data(0) {} 
    explicit Data(int x) :x(x) { ctor_called = true; } 
    ~Data() { dtor_called = true; } 

    bool ctor_called = false;
    bool dtor_called = false;
    int x;
};

static const char* SHMEM_NAME = "schillc_shmem_test";

TEST(shmem, constructors) {
    { // Regular object
        Data data{};
        EXPECT_EQ(data.ctor_called, true);
    }

    { // make_shared_mem calls ctor
        auto data = schillc::make_shared_mem<Data>(SHMEM_NAME);
        EXPECT_EQ(data->ctor_called, true);
    }

    { // shared_mem ctor(std::string) does not call ctor
        auto data = schillc::shared_mem<Data>(SHMEM_NAME);
        EXPECT_EQ(data->ctor_called, false);
    }

    { // open shared mem, set data, and don't destory it
        auto data = schillc::make_shared_mem<Data>(SHMEM_NAME);
        data.delete_on_destruction(false);
        EXPECT_EQ(data->ctor_called, true);
        data->x = 69;
    }

    { // Attach to existing shared mem we just left open
        auto data = schillc::shared_mem<Data>(SHMEM_NAME);
        EXPECT_EQ(data->ctor_called, true);
        EXPECT_EQ(data->x, 69);
    }

    { // Ensure previous test properly destroyed shm block
        auto data = schillc::shared_mem<Data>(SHMEM_NAME);
        EXPECT_EQ(data->ctor_called, false);
        EXPECT_EQ(data->x, 0);
    }

    { // copys as if its a simple pointer
        auto data = schillc::make_shared_mem<Data>(SHMEM_NAME);
        auto copy = data;
        EXPECT_EQ(data.get_address(), copy.get_address());

        data->x = 20;
        EXPECT_EQ(data->x, copy->x);

        copy->x = 69;
        EXPECT_EQ(data->x, copy->x);
    }

    { // ensure array construction constructs every element when using make_shared_mem
        auto data = schillc::make_shared_mem<Data[10]>(SHMEM_NAME, 69);
        data.delete_on_destruction(false);
        for (int i = 0; i < data.count(); ++i) {
            EXPECT_EQ(data[i].ctor_called, true);
            EXPECT_EQ(data[i].x, 69);
        }
    }
    { // verify data is still good and attaching arrays works
        auto data = schillc::shared_mem<Data[10]>(SHMEM_NAME);
        for (int i = 0; i < data.count(); ++i) {
            EXPECT_EQ(data[i].ctor_called, true);
            EXPECT_EQ(data[i].x, 69);
        }
    }

    { // ... and when not using make_shared_mem
        auto data = schillc::shared_mem<Data[10]>(SHMEM_NAME);
        for (int i = 0; i < data.count(); ++i) {
            EXPECT_EQ(data[i].ctor_called, false);
            EXPECT_EQ(data[i].x, 0);
        }
    }
}

TEST(shmem, destructors) {
    { // dtor called if make_shared_mem was used
        {
            auto data = schillc::make_shared_mem<Data>(SHMEM_NAME);
            data.delete_on_destruction(false);
            EXPECT_EQ(data->dtor_called, false);
        }

        auto data = schillc::shared_mem<Data>(SHMEM_NAME);
        EXPECT_EQ(data->dtor_called, true);
    }

    { // dtor not called if not last ref
        schillc::shared_mem<Data> dcopy;
        {
            auto data = schillc::make_shared_mem<Data>(SHMEM_NAME);
            // copy the shm obj out of current scope. now dtors should not be called
            // because its not the last reference
            dcopy = data; 
            EXPECT_EQ(data->dtor_called, false);
        }

        auto data = schillc::shared_mem<Data>(SHMEM_NAME);
        EXPECT_EQ(data->dtor_called, false);
    }

    { // dtor called if make_shared_mem was used (arrays)
        {
            auto data = schillc::make_shared_mem<Data[2]>(SHMEM_NAME);
            data.delete_on_destruction(false);
            for (std::size_t i; i < data.count(); ++i) {
                EXPECT_EQ(data[i].dtor_called, false);
            }
        }

        auto data = schillc::shared_mem<Data[2]>(SHMEM_NAME);
        for (std::size_t i; i < data.count(); ++i) {
            EXPECT_EQ(data[i].dtor_called, true);
        }
    }

    { // dtor not called if not last ref (arrays)
        schillc::shared_mem<Data[2]> dcopy;
        {
            auto data = schillc::make_shared_mem<Data[2]>(SHMEM_NAME);
            // copy the shm obj out of current scope. now dtors should not be called
            // because its not the last reference
            dcopy = data; 
            for (std::size_t i; i < data.count(); ++i) {
                EXPECT_EQ(data[i].dtor_called, false);
            }
        }

        auto data = schillc::shared_mem<Data[2]>(SHMEM_NAME);
        for (std::size_t i; i < data.count(); ++i) {
            EXPECT_EQ(data[i].dtor_called, false);
        }
    }
}

TEST(shmem, size_and_count) {
    {
        auto data = schillc::make_shared_mem<char>(SHMEM_NAME);
        EXPECT_EQ(data.size(), sizeof(char));
        // data.count(); // TODO: Test for compiler error
    }
    {
        auto data = schillc::make_shared_mem<Data>(SHMEM_NAME);
        EXPECT_EQ(data.size(), sizeof(Data));
        //data.count(); // TODO: Test for compiler error
    }

    {
        auto data = schillc::make_shared_mem<char[10]>(SHMEM_NAME);
        EXPECT_EQ(data.size(), sizeof(char[10]));
        EXPECT_EQ(sizeof(data[0]), sizeof(char));
        EXPECT_EQ(data.count(), 10);
    }
    {
        auto data = schillc::make_shared_mem<Data[10]>(SHMEM_NAME);
        EXPECT_EQ(data.size(), sizeof(Data[10]));
        EXPECT_EQ(sizeof(data[0]), sizeof(Data));
        EXPECT_EQ(data.count(), 10);
    }
}

TEST(shmem, zeroize) {
    auto data = schillc::make_shared_mem<Data>(SHMEM_NAME);
    data->x = 69;
    EXPECT_EQ(data->x, 69);
    data.zeroize();

    std::vector<char> zeroes(sizeof(Data), 0);
    EXPECT_EQ(std::memcmp(data.get_address(), zeroes.data(), sizeof(Data)), 0);
}

TEST(shmem, index_operator) {
    { // regular char array
        auto data = schillc::make_shared_mem<char[2]>(SHMEM_NAME);
        data[0] = 'a';
        data[1] = 'b';
        EXPECT_EQ(data[0], 'a');
        EXPECT_EQ(data[1], 'b');
    }

    { // array of structs
        Data tmp{};
        tmp.x = 69;
        auto data = schillc::make_shared_mem<Data[2]>(SHMEM_NAME);
        data[0] = tmp;
        data[1].x = 70;
        EXPECT_EQ(data[0].x, 69);
        EXPECT_EQ(data[1].x, 70);

        Data tmp2 = data[0];
        EXPECT_EQ(tmp2.x, 69);
        tmp2 = data[1];
        EXPECT_EQ(tmp2.x, 70);
    }

    { // simple structs
        // TODO: Somehow test that data[0] in this test is a compiler error
        // Data tmp{};
        // tmp.x = 70;
        // auto data = schillc::make_shared_mem<Data>(SHMEM_NAME);
        // data[0] = tmp;
        // EXPECT_EQ(data[0].x, 70);
    }
}
