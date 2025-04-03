#ifndef TABLE_H
#define TABLE_H

#include <unordered_map>
#include <string>

class Table{
public:
    void add(const std::string& label, const std::string &field, const std::string& value);
    std::string value(const std::string& label, const std::string &field) const;
    bool dump(const std::string& filename) const;
    bool init(const std::string& filename);

private:
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> tab;
};

#endif // TABLE_H
