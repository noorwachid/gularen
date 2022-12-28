#include "Tester.h"

void Tester::Group(const std::string& group, const std::function<void()>& callback)
{
    for (std::size_t i = 0; i < _groups.size(); ++i)
        std::cout << "    ";

    _groups.push_back(group);
    
    std::cout << "Testing \x1b[33m\"" << group << "\"\x1b[0m Group:\r\n";

    callback();

    if (!_groups.empty())
        _groups.pop_back();
}

void Tester::Test(const std::string &title, const std::function<bool()>& callback)
{
    bool result = callback();
    
    if (!result)
        ++_failedCounter;
    
    for (std::size_t i = 0; i < _groups.size(); ++i)
        std::cout << "    ";

    std::cout << "[" << (result ? "\x1b[32mPASS\x1b[0m" : "\x1b[31mFAIL\x1b[0m") << "] " << title << "\r\n";
}

void Tester::Sumarize()
{

    std::cout << "\r\n";
    std::cout << "Total " << _counter << " test(s)\r\n";

    if (_failedCounter == 0)
        std::cout << "All test(s) passed\r\n";
    else
        std::cout << _failedCounter << " test(s) failed\r\n";

    std::cout << "\r\n";
}