#pragma once

#include <string>
#include <vector>
#include <functional>
#include <iostream>

class Tester
{
public:
    void Group(const std::string& group, const std::function<void()>& callback);

    void Test(const std::string& title, const std::function<bool()>& callback);

    void Sumarize();

private:
    std::size_t _counter = 0;
    std::size_t _failedCounter = 0;
    
    std::vector<std::string> _groups;
};