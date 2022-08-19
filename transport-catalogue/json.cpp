#include <stdexcept>
#include <sstream>

#include "json.h"

using namespace std;

namespace Json {

Node LoadNode(istream& input);

Node LoadNumber(std::istream& input) {
    using namespace std::literals;

    std::string parsed_num;

    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw ParsingError("Failed to read number from stream"s);
        }
    };

    auto read_digits = [&input, read_char] {
        if (!std::isdigit(input.peek())) {
            throw ParsingError("A digit is expected"s);
        }
        while (std::isdigit(input.peek())) {
            read_char();
        }
    };

    if (input.peek() == '-') {
        read_char();
    }

    if (input.peek() == '0') {
        read_char();
    } else {
        read_digits();
    }

    bool is_int = true;
    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_digits();
        is_int = false;
    }

    try {
        if (is_int) {
            try {
                return {std::stoi(parsed_num)};
            } catch (...) {
                return {std::stod(parsed_num)};
            }
        }
        return {std::stod(parsed_num)};
    } catch (...) {
        throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
    }
}

Node LoadNull(std::istream& input) {
    std::string parsed_null;
    char c;
    while (input.get(c)) {
        parsed_null += c;
        if (parsed_null == "null") {
            return {nullptr};
        }
    }
    if (parsed_null == "null") {
        return {nullptr};
    }
    throw ParsingError("Null parsing error");
}

Node LoadBool(std::istream& input) {
    std::string parsed_bool;
    char c;
    while (input.get(c)) {
        parsed_bool += c;
        if (parsed_bool == "true") {
            return {true};
        }
        if (parsed_bool == "false") {
            return {false};
        }
    }
    if (parsed_bool == "true") {
        return {true};
    }
    if (parsed_bool == "false") {
        return {false};
    }
    throw ParsingError("Bool parsing error");
}

Node LoadArray(istream& input) {
    Array result;
    for (char c; input >> c && c != ']';) {
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }
    return Node(move(result));
}

Node LoadString(std::istream& input) {
    using namespace std::literals;

    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    while (true) {
        if (it == end) {
            throw ParsingError("String parsing error");
        }
        const char ch = *it;
        if (ch == '"') {
            ++it;
            break;
        } else if (ch == '\\') {
            ++it;
            if (it == end) {
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *(it);
            switch (escaped_char) {
                case 'n':
                    s.push_back('\n');
                    break;
                case 't':
                    s.push_back('\t');
                    break;
                case 'r':
                    s.push_back('\r');
                    break;
                case '"':
                    s.push_back('"');
                    break;
                case '\\':
                    s.push_back('\\');
                    break;
                default:
                    throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
            }
        } else if (ch == '\n' || ch == '\r') {
            throw ParsingError("Unexpected end of line"s);
        } else {
            s.push_back(ch);
        }
        ++it;
    }
    return {move(s)};
}

Node LoadDict(istream& input) {
    Dict result;
    for (char c; input >> c && c != '}';) {
        if (c == ',') {
            input >> c;
        }
        string key = LoadString(input).AsString();
        input >> c;
        result.insert({move(key), LoadNode(input)});
    }
    return Node(move(result));
}

Node LoadNode(istream& input) {
    char c;
    input.get(c);
    if (c == ']' || c == '}') {
        throw ParsingError("Parsing error");
    }
    while (c != '[' && c !='{' && c != '"' && c != 't' && c != 'f' && c != '-' && c != 'n' && !std::isdigit(c)) {
        input.get(c);
    }
    if (c == '[') {
        if (input.get(c)) {
            input.unget();
            return LoadArray(input);
        }
        throw ParsingError("Array parsing error");
    }
    if (c == '{') {
        if (input.get(c)) {
            input.unget();
            return LoadDict(input);
        }
        throw ParsingError("Dict parsing error");
    }
    if (c == '"') {
        return LoadString(input);
    }
    if (c == 't' || c == 'f') {
        input.unget();
        return LoadBool(input);
    }
    if (c == 'n') {
        input.unget();
        return LoadNull(input);
    } else  {
        input.unget();
        return LoadNumber(input);
    }
}

bool Node::IsNull() const {
    return std::holds_alternative<std::nullptr_t>(value_);
}

bool Node::IsArray() const {
    return std::holds_alternative<Array>(value_);
}

bool Node::IsMap() const {
    return std::holds_alternative<Dict>(value_);
}

bool Node::IsString() const {
    return std::holds_alternative<string>(value_);
}

bool Node::IsInt() const {
    return std::holds_alternative<int>(value_);
}

bool Node::IsDouble() const {
    return  IsInt() || IsPureDouble();
}

bool Node::IsPureDouble() const {
    return std::holds_alternative<double>(value_);
}

bool Node::IsBool() const {
    return std::holds_alternative<bool>(value_);
}

const Array& Node::AsArray() const {
    if (IsArray()) {
        return std::get<Array>(value_);
    }
    throw std::logic_error("logic_error");
}

const Dict& Node::AsMap() const {
    if (IsMap()) {
        return std::get<Dict>(value_);
    }
    throw std::logic_error("logic_error");
}

const string& Node::AsString() const {
    if (IsString()) {
        return std::get<string>(value_);
    }
    throw std::logic_error("logic_error");
}

int Node::AsInt() const {
    if (IsInt()) {
        return std::get<int>(value_);
    }
    throw std::logic_error("logic_error");
}

double Node::AsDouble() const {
    if (IsPureDouble()) {
        return std::get<double>(value_);
    }
    if (IsInt()) {
        return static_cast<double>(std::get<int>(value_)) ;
    }
    throw std::logic_error("logic_error");
}

bool Node::AsBool() const {
    if (IsBool()) {
        return std::get<bool>(value_);
    }
    throw std::logic_error("logic_error");
}

const Node& Document::GetRoot() const {
    return root_;
}

Document Load(istream& input) {
    return Document{LoadNode(input)};
}

void Print(const Document& doc, std::ostream& output) {
    PrintNode(doc.GetRoot(), output);
}

void PrintValue(std::nullptr_t, std::ostream& out) {
    out << "null";
}

void PrintValue(const Array& array, std::ostream& out) {
    out << "[ ";
    for (size_t i = 0; i < array.size(); ++i) {
        if (i == array.size() - 1) {
            PrintNode(array[i], out);
        } else {
            PrintNode(array[i], out);
            out << ",";
        }
    }
    out << " ]";
}

void PrintValue(const Dict& dict, std::ostream& out) {
    out << "{ ";
    bool is_first = true;
    for (auto it = dict.begin(); it != dict.end(); ++it) {
        if (is_first) {
            is_first = false;
            out <<"\"" << it->first << "\": ";
            PrintNode(it->second, out);
        } else {
            out << ", ";
            is_first = false;
            out <<"\"" << it->first << "\": ";
            PrintNode(it->second, out);
        }
    }
    out << " }";
}

void PrintValue(std::string value, std::ostream& out) {
    string result = "\"";
    for (auto& ch : value) {
        switch (ch) {
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\"':
                result += "\\\"";
                break;
            case '\\':
                result += "\\\\";
            break;
            default:
                result += ch;
        }
    }
    result += "\"";
    out << result;
}

void PrintValue(int value, std::ostream& out) {
    out << value;
}

void PrintValue(double value, std::ostream& out) {
    out << value;
}

void PrintValue(bool value, std::ostream& out) {
    out << boolalpha << value;
}

void PrintNode(const Node& node, std::ostream& out) {
    std::visit(
        [&out](const auto& value){ PrintValue(value, out); },
        node.GetValue());
}

}  // namespace json
