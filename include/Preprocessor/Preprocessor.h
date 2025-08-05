#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H
#include <string>
#include "../../error/Error.h"
class Preprocessor {
public:
    static void RemoveComments(std::string& str);
};
#endif //PREPROCESSOR_H
