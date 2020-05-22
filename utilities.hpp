//
// Created by alexey on 17.05.2020.
//

#ifndef X86COMPILERBACKEND_UTILITIES_HPP
#define X86COMPILERBACKEND_UTILITIES_HPP

#include <exception>
#include <cstdio>

#define STR(x) #x
#define STRINGIFY(x) STR(x)
#define throw_exception(str) throw runtime_error("In " __FILE__ ":" STRINGIFY(__LINE__) ", the following exception occured:\n" str)

class runtime_error : public std::exception {
private:
    const char *msg;
public:
    explicit runtime_error(const char *msg) : std::exception(), msg(msg) {}
    [[nodiscard]] const char* what() const throw() {
        return msg;
    }

    ~runtime_error() noexcept = default;
};

size_t getFilesize(FILE *f);

char *skipSpaces(char *str);

const char *skipSpaces(const char *str);

const char *getNum(const char *serializedString, int &dst);

const char *getIdentifier(const char *serialized, int& length);

template<typename T>
void swap(T& a, T& b) {
    T tmp (std::move(a));
    a = std::move(b);
    b = std::move(a);
}

template <typename T>
struct remove_reference {
    typedef T type;
};

template <typename T>
struct remove_reference<T&> {
    typedef T type;
};

template <typename T>
struct remove_reference<T&&> {
    typedef T type;
};

template <typename T>
constexpr T&& forward(typename remove_reference<T>::type& t) noexcept {
    return static_cast<T &&>(t);
}

template <typename T>
constexpr T&& forward(typename remove_reference<T>::type&& t) noexcept {
    return static_cast<T &&>(t);
}



#endif //X86COMPILERBACKEND_UTILITIES_HPP
