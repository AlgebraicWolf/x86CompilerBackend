//
// Created by alexey on 17.05.2020.
//

#ifndef X86COMPILERBACKEND_UTILITIES_HPP
#define X86COMPILERBACKEND_UTILITIES_HPP

#define STR(x) #x
#define STRINGIFY(x) STR(x)
#define throw_exception(str) throw runtime_error("In " __FILE__ ":" STRINGIFY(__LINE__) ", the following exception occured:\n" str)

class runtime_error : public std::exception {
private:
    const char *msg;
public:
    explicit runtime_error(const char *msg);
    [[nodiscard]] const char *what() const throw() override;
};

size_t getFilesize(FILE *f);

char *skipSpaces(char *str);

const char *skipSpaces(const char *str);

const char *getNum(const char *serializedString, int &dst);

#endif //X86COMPILERBACKEND_UTILITIES_HPP
