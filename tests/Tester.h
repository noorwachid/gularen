#pragma once

#include <string>
#include <vector>
#include <functional>
#include <iostream>

class Tester {
public:
    void group(const std::string& group, const std::function<void()>& callback);

    void test(const std::string& title, const std::function<bool()>& callback);

    void sumarize();

private:
    std::size_t counter = 0;
    std::size_t failedCounter = 0;
    
    std::vector<std::string> _groups;
};