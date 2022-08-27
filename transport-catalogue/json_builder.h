#pragma once

#include <stack>
#include <vector>
#include <string>
#include <optional>

#include "json.h"

namespace Json {

class DictItemContext;

class KeyItemContext;

class ArrayItemContext;

class ValueItemContextAtherKey;

class ValueItemContextAtherArray;

class Builder{
public:
    Builder() = default;

    DictItemContext StartDict();

    Builder& EndDict();

    KeyItemContext Key(std::string&& key);

    Builder& Value(const Node&& node);

    ArrayItemContext StartArray();

    Builder& EndArray();

    Json::Node& Build();

private:
    Json::Node node_;
    std::stack<Node*> nodes_stack_;
    std::optional<std::string> key_;
};

class DictItemContext : public Builder {
public:
    explicit DictItemContext(Builder& builder) : builder_(builder) {}

    DictItemContext StartDict() = delete;

    Builder& EndDict();

    KeyItemContext Key(std::string&& key);

    Builder& Value(const Node&& node) = delete;

    ArrayItemContext StartArray() = delete;

    Builder& EndArray() = delete;

    Json::Node& Build() = delete;

private:
    Builder& builder_;
};

class KeyItemContext : public Builder {
public:
    explicit KeyItemContext(Builder& builder) : builder_(builder) {}

    DictItemContext StartDict();

    Builder& EndDict() = delete;

    KeyItemContext Key(std::string&& key) = delete;

    ValueItemContextAtherKey Value(const Node&& node);

    ArrayItemContext StartArray();

    Builder& EndArray() = delete;

    Json::Node& Build() = delete;

private:
    Builder& builder_;
};

class ArrayItemContext : public Builder {
public:
    explicit ArrayItemContext(Builder& builder) : builder_(builder) {}

    DictItemContext StartDict();

    Builder& EndDict() = delete;

    KeyItemContext Key(std::string&& key) = delete;

    ValueItemContextAtherArray Value(const Node&& node);

    ArrayItemContext StartArray();

    Builder& EndArray();

    Json::Node& Build() = delete;

private:
    Builder& builder_;
};

class ValueItemContextAtherKey : public Builder {
public:
    explicit ValueItemContextAtherKey(Builder& builder) : builder_(builder) {}

    DictItemContext StartDict() = delete;

    Builder& EndDict();

    KeyItemContext Key(std::string&& key);

    Builder& Value(const Node&& node) = delete;

    ArrayItemContext StartArray() = delete;

    Builder& EndArray() = delete;

    Json::Node& Build() = delete;

private:
    Builder& builder_;
};

class ValueItemContextAtherArray : public Builder {
public:
    explicit ValueItemContextAtherArray(Builder& builder) : builder_(builder) {}

    DictItemContext StartDict();

    Builder& EndDict() = delete;

    KeyItemContext Key(std::string&& key) = delete;

    ValueItemContextAtherArray Value(const Node&& node);

    ArrayItemContext StartArray();

    Builder& EndArray();

    Json::Node& Build() = delete;

private:
    Builder& builder_;
};

} // namespace Json
