#ifndef SCOPEMANAGER_H
#define SCOPEMANAGER_H
#include <vector>
#include "Scope.h";

class ScopeManager {
    std::vector<Scope> scopes_;

public:
    ScopeManager() {
        scopes_.emplace_back();
    }

    void pushBack() {
        scopes_.emplace_back();
    }

    void popBack() {
        scopes_.pop_back();
    }

    void declare(Symbol *symbol) {
        try {
            uint32_t len = scopes_.size();
            scopes_[len - 1].declare(symbol);
        } catch (std::exception &) {
            throw;
        }
    }

    Symbol *lookup(const std::string &name) {
        auto len = static_cast<int32_t>(scopes_.size());
        for (int32_t i = len - 1; i >= 0; i--) {
            auto *ret = scopes_[i].lookup(name);
            if (ret) {
                return ret;
            }
        }
        return nullptr;
    }
};
#endif //SCOPEMANAGER_H
