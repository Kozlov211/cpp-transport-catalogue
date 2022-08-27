#include <stdexcept>

#include "json_builder.h"

using namespace std;

namespace Json {

DictItemContext Builder::StartDict() {
    DictItemContext context(*this);
    if (node_.GetValue().index() == 0) {
        node_ = Json::Dict();
        nodes_stack_.push(&node_);
        return context;
    }
    if (key_) {
        Json::Dict& dict = get<Json::Dict>(nodes_stack_.top()->GetValue());
        dict[*key_] = Json::Dict();
        nodes_stack_.push(&dict[*key_]);
        key_.reset();
        return context;
    }
    if (!nodes_stack_.empty() && nodes_stack_.top()->IsArray()) {
        Json::Array& array = get<Json::Array>(nodes_stack_.top()->GetValue());
        array.emplace_back(Json::Dict());
        nodes_stack_.push(&array.back());
        return context;
    }
    throw std::logic_error("Error start Dict");

}

Builder& Builder::EndDict() {
    if (!nodes_stack_.empty() && !nodes_stack_.top()->IsDict()) {
        throw std::logic_error("Error end Dict");
    }
    nodes_stack_.pop();

    return *this;
}

KeyItemContext Builder::Key(std::string&& key) {
    KeyItemContext context(*this);
    if (nodes_stack_.empty() || !nodes_stack_.top()->IsDict() || key_) {
        throw std::logic_error("Error add key");
    }
    Json::Dict& dict = get<Json::Dict>(nodes_stack_.top()->GetValue());
    key_ = key;
    dict[key];

    return context;
}

Builder& Builder::Value(const Node&& node) {
    if (node_.GetValue().index() == 0) {
        node_ = node;
        return *this;
    }
    if (!nodes_stack_.empty() && nodes_stack_.top()->IsArray()) {
        Json::Array& array = get<Json::Array>(nodes_stack_.top()->GetValue());
        array.emplace_back(move(node));
        if (node.IsArray() || node.IsDict()) {
            nodes_stack_.push(&array.back());
        }
        return *this;
    }
    if (!nodes_stack_.empty() && nodes_stack_.top()->IsDict()) {
        if (key_) {
            Json::Dict& dict = get<Json::Dict>(nodes_stack_.top()->GetValue());
            dict[*key_] = node;
            if (node.IsArray() || node.IsDict()) {
                nodes_stack_.push(&dict[*key_]);
            }
            key_.reset();
            return *this;
        }
    }
    throw std::logic_error("Error add Value");
}

ArrayItemContext Builder::StartArray() {
    ArrayItemContext context(*this);
    if (node_.GetValue().index() == 0) {
        node_ = Json::Array();
        nodes_stack_.push(&node_);
        return context;
    }
    if (key_) {
        Json::Dict& dict = get<Json::Dict>(nodes_stack_.top()->GetValue());
        dict[*key_] = Json::Array();
        nodes_stack_.push(&dict[*key_]);
        key_.reset();
        return context;
    }
    if (!nodes_stack_.empty() && nodes_stack_.top()->IsArray()) {
        Json::Array& array = get<Json::Array>(nodes_stack_.top()->GetValue());
        array.emplace_back(Json::Array());
        nodes_stack_.push(&array.back());
        return context;
    }
    throw std::logic_error("Error start Array");
}

Builder& Builder::EndArray() {
    if (!nodes_stack_.empty() && !nodes_stack_.top()->IsArray()) {
        throw std::logic_error("Error end Array");
    }
    nodes_stack_.pop();

    return *this;
}

Json::Node& Builder::Build() {
    if (node_.GetValue().index() == 0 || !nodes_stack_.empty()) {
        throw std::logic_error("Error build Node");
    }

    return node_;
}

Builder& DictItemContext::EndDict() {
    return builder_.EndDict();
}

KeyItemContext DictItemContext::Key(std::string&& key) {
    return builder_.Key(move(key));
}

DictItemContext KeyItemContext::StartDict() {
    return builder_.StartDict();
}

ArrayItemContext KeyItemContext::StartArray() {
    return builder_.StartArray();
}

ValueItemContextAtherKey KeyItemContext::Value(const Node&& node) {
    ValueItemContextAtherKey contex(builder_);
    builder_ = builder_.Value(move(node));
    return contex;
}

DictItemContext ArrayItemContext::StartDict() {
    return builder_.StartDict();
}

ValueItemContextAtherArray ArrayItemContext::Value(const Node&& node) {
    ValueItemContextAtherArray contex(builder_);
    builder_ = builder_.Value(move(node));
    return contex;
}

ArrayItemContext ArrayItemContext::StartArray() {
    return builder_.StartArray();
}

Builder& ArrayItemContext::EndArray() {
    return builder_.EndArray();
}

Builder& ValueItemContextAtherKey::EndDict() {
    return builder_.EndDict();
}

KeyItemContext ValueItemContextAtherKey::Key(std::string&& key) {
    return builder_.Key(move(key));
}

DictItemContext ValueItemContextAtherArray::StartDict() {
    return builder_.StartDict();
}

ValueItemContextAtherArray ValueItemContextAtherArray::Value(const Node&& node) {
    ValueItemContextAtherArray contex(builder_);
    builder_ = builder_.Value(move(node));
    return contex;
}

ArrayItemContext ValueItemContextAtherArray::StartArray() {
    return builder_.StartArray();
}

Builder& ValueItemContextAtherArray::EndArray() {
    return builder_.EndArray();
}

} // namespace Json
