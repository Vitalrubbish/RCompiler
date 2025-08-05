#ifndef ERROR_H
#define ERROR_H
#include <string>
#include <exception>
#include <utility>
class SyntaxError : public std::exception {
public:

    explicit SyntaxError(std::string  msg) : message(std::move(msg)) {}

    [[nodiscard]] const char* what() const noexcept override {
        return message.c_str();
    }

private:
    std::string message;
};
#endif //ERROR_H
