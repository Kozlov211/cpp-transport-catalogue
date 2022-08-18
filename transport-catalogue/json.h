#pragma once

#include <iostream>
#include <optional>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <utility>

namespace Json {

class Node;
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;
using Value = std::variant<std::nullptr_t, int, double, std::string, bool, Array, Dict>;

class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node {
public:
    Node() : value_() {}
    Node(std::nullptr_t) : value_(nullptr) {}
    Node(Array array) : value_(move(array)){}
    Node(Dict map) : value_(move(map)) {}
    Node(std::string value) : value_(move(value)) {}
    Node(int value) : value_(value) {}
    Node(double value) : value_(value) {}
    Node(bool value) : value_(value) {}

    const Array& AsArray() const;
    const Dict& AsMap() const;
    const std::string& AsString() const;
    int AsInt() const;
    double AsDouble() const;
    bool AsBool() const;

    const std::optional<Value> GetType() const {
        return value_;
    }

    const Value GetValue() const {
        return value_;
    }

    bool IsNull() const;
    bool IsArray() const;
    bool IsMap() const;
    bool IsString() const;
    bool IsInt() const;
    bool IsDouble() const;
    bool IsPureDouble() const;
    bool IsBool() const;

private:
    Value value_;

};

class Document {
public:
    explicit Document(Node root) : root_(std::move(root)) {}

    const Node& GetRoot() const;

private:
    Node root_;
};

Document Load(std::istream& input);

void Print(const Document& doc, std::ostream& output);

void PrintNode(const Node& node, std::ostream& out);

inline bool operator==(const Node& lhs, const Node& rhs) {
    return lhs.GetType().value() == rhs.GetType().value();
}

inline bool operator!=(const Node& lhs, const Node& rhs) {
    return !(lhs == rhs);
}

inline bool operator==(const Document& lhs, const Document& rhs) {
    return lhs.GetRoot() == rhs.GetRoot();
}

inline bool operator!=(const Document& lhs, const Document& rhs) {
    return !(lhs == rhs);
}

}  // namespace json
