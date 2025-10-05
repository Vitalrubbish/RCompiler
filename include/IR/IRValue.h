#ifndef IRVALUE_H
#define IRVALUE_H
#include <memory>
#include <string>
#include <vector>

class IRType;
class Use;

class IRValue {
    std::string name;
    std::shared_ptr<IRType> type;
    std::vector<std::shared_ptr<Use>> useList;
public:
    explicit IRValue(const std::string &name, const std::shared_ptr<IRType>& type) {
        this->name = name;
        this->type = type;
    }

    virtual ~IRValue() = default;

    std::shared_ptr<IRType> getType() { return type; }

    [[nodiscard]] std::string toString() { return name; }

    void addUse(const std::shared_ptr<Use> &use) {
        useList.push_back(use);
    }

    void removeUse(const std::shared_ptr<Use> &use) {
        bool flag = false;
        for (uint32_t i = 0; i < useList.size() - 1; i++) {
            if (useList[i] == use) {
                flag = true;
            }
            if (flag) {
                useList[i] = useList[i + 1];
            }
        }
        useList.pop_back();
    }
};
#endif //IRVALUE_H