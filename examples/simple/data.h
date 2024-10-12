#pragma once

#include <cassert>
#include <string>
#include <iostream>

struct Data {
    Data(std::string text){
        std::cout << "Data ctor(text)\n";
        assert(text.size() < sizeof(this->text));
        text.copy(this->text, sizeof(this->text)-1);
        text[sizeof(this->text)-1] = '\0';
    }
    char text[256];
};


