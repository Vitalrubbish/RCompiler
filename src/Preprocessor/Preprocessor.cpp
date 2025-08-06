#include "../../include/Preprocessor/Preprocessor.h"

void Preprocessor::RemoveComments(std::string &str) {
    uint32_t pre = 0, cur = 0, nex = 0;
    uint32_t y_cnt = 0;
    uint32_t cnt = 0;
    while (cur < str.size() - 1) {
        if (cnt == 0 && str[cur] == '\"') {
            y_cnt ^= 1;
        }
        if (y_cnt == 0 && str.substr(cur, 2) == "/*") {
            cnt++;
            if (cnt == 1) {
                pre = cur;
            }
        }
        if (y_cnt == 0 && str.substr(cur, 2) == "*/") {
            cnt--;
            if (cnt == 0) {
                str = str.substr(0, pre - 1) + " " + str.substr(cur + 2, str.size() - cur - 2);
                cur = pre;
            }
        }
        if (y_cnt == 0 && cnt == 0 && str.substr(cur, 2) == "//") {
            nex = cur;
            while (str[nex] != '\n' && str[nex] != '\r') {
                nex++;
            }
            str = str.substr(0, cur - 1) + " " + str.substr(nex, str.size() - nex);
        }
        cur++;
    }
    if (cnt != 0) {
        throw SyntaxError("Syntax Error: Block comment signs do not match.");
    }
}
