#include <gtest/gtest.h>

#include <schillc/shmem.h>
#include <cstring>

struct Data {
    Data() { ctor_called = true; } 
    bool ctor_called = false;

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
}

TEST(shmem, zeroize) {
    auto data = schillc::make_shared_mem<Data>(SHMEM_NAME);
    data->x = 69;
    EXPECT_EQ(data->x, 69);
    data.zeroize();

    std::vector<char> zeroes(sizeof(Data), 0);
    EXPECT_EQ(std::memcmp(data.get_address(), zeroes.data(), sizeof(Data)), 0);
}
