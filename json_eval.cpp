#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <algorithm>
#include <limits>

// Forward declarations
struct JSONValue;
struct Expression;

using JSONObject = std::unordered_map<std::string, JSONValue>;
using JSONArray = std::vector<JSONValue>;

enum class JSONValueType { Null, Object, Array, String, Number };

struct JSONValue {
    JSONValueType type = JSONValueType::Null;
    JSONObject objectValue;
    JSONArray arrayValue;
    std::string stringValue;
    double numberValue = 0;

    // Default constructor
    JSONValue() = default;

    // Constructors for each type
    JSONValue(double num) : type(JSONValueType::Number), numberValue(num) {}

    JSONValue(const std::string& str) : type(JSONValueType::String), stringValue(str) {}

    JSONValue(const JSONArray& arr) : type(JSONValueType::Array), arrayValue(arr) {}

    JSONValue(const JSONObject& obj) : type(JSONValueType::Object), objectValue(obj) {}
};

// JSON Parser
class JSONParser {
public:
    JSONParser(const std::string& text) : text(text), pos(0) {}

    JSONValue parse() {
        // Parse the JSON value
        skipWhitespace();
        JSONValue result = parseValue();
        skipWhitespace();

        // Check for extra data
        if (pos != text.length()) {
            throw std::runtime_error("Invalid JSON: Extra data after parsing");
        }


        return result;
    }

private:
    std::string text;
    size_t pos;

    void skipWhitespace() {
        while (pos < text.length() && isspace(text[pos])) {
            pos++;
        }
    }

    char peek() {
        if (pos < text.length()) {
            return text[pos];
        }
        return '\0';
    }

    char get() {
        if (pos < text.length()) {
            return text[pos++];
        }
        return '\0';
    }

    JSONValue parseValue() {
        skipWhitespace();
        char c = peek();

        // Check the first character to determine the type of JSON value
        if (c == '{') return parseObject();
        if (c == '[') return parseArray();
        if (c == '"') return parseString();
        if (isdigit(c) || c == '-') return parseNumber();

        // Check for unexpected character
        throw std::runtime_error(std::string("Unexpected character in JSON: ") + c);
    }

    JSONValue parseObject() {
        JSONObject obj;

        // Consume '{'
        get();
        skipWhitespace();

        // Check for empty object
        if (peek() == '}') {
            get();
            return JSONValue(obj);
        }

        // Parse key-value pairs
        while (true) {
            // Parse key-value pair
            skipWhitespace();
            std::string key = parseString().stringValue;
            skipWhitespace();

            if (get() != ':') throw std::runtime_error("Expected ':' in object");
            skipWhitespace();

            JSONValue value = parseValue();
            obj[key] = value;
            skipWhitespace();
            char c = get();
            if (c == '}') break;
            if (c != ',') throw std::runtime_error("Expected ',' in object");
        }

        return JSONValue(obj);
    }

    JSONValue parseArray() {
        JSONArray arr;

        // Consume '['
        get();
        skipWhitespace();

        // Check for empty array
        if (peek() == ']') {
            get(); // consume ']'
            return JSONValue(arr);
        }

        // Parse array elements
        while (true) {
            skipWhitespace();
            JSONValue value = parseValue();
            arr.push_back(value);
            skipWhitespace();
            char c = get();
            if (c == ']') break;
            if (c != ',') throw std::runtime_error("Expected ',' in array");
        }

        return JSONValue(arr);
    }

    JSONValue parseString() {
        std::string result;

        // Consume '"'
        get();

        // Parse string characters
        while (true) {
            char c = get();

            // Check for end of string
            if (c == '"') break;

            // Handle escape characters
            if (c == '\\') {
                char next = get();
                if (next == '"' || next == '\\' || next == '/') {
                    result += next;
                } else {
                    throw std::runtime_error("Invalid escape character in string");
                }
            } else {
                result += c;
            }
        }

        return JSONValue(result);
    }

    JSONValue parseNumber() {
        size_t start = pos;

        // Parse optional negative sign
        if (peek() == '-') get();

        // Parse integer part
        while (isdigit(peek())) get();

        // Parse fractional part
        if (peek() == '.') {
            get();
            while (isdigit(peek())) get();
        }

        // Get the number string
        std::string numStr = text.substr(start, pos - start);

        // Convert to double the number string
        return JSONValue(std::stod(numStr));
    }
};

// Lexer
enum class TokenType {
    Identifier, Number, String, LParen, RParen, LBracket, RBracket, Comma,
    Dot, Plus, Minus, Asterisk, Slash, End
};

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
public:
    Lexer(const std::string& text) : text(text), pos(0) {}

    Token getNextToken() {
        skipWhitespace();

        // Check for end of text
        if (pos >= text.length()) {
            return Token{TokenType::End, ""};
        }

        // Check for identifiers
        char c = peek();
        if (isalpha(c) || c == '_') {
            return identifier();
        }

        // Check for numbers
        if (isdigit(c)) {
            return number();
        }

        // Check for negative numbers or minus operator
        if (c == '-') {
            char nextChar = peekNextChar();
            if (isdigit(nextChar) || nextChar == '.') {
                return number();
            } else {
                pos++; // Consume '-'
                return Token{TokenType::Minus, "-"};
            }
        }

        // Check for strings
        if (c == '"') {
            return string();
        }

        // Check for single-character tokens
        switch (c) {
            case '+': pos++; return Token{TokenType::Plus, "+"};
            case '*': pos++; return Token{TokenType::Asterisk, "*"};
            case '/': pos++; return Token{TokenType::Slash, "/"};
            case '(': pos++; return Token{TokenType::LParen, "("};
            case ')': pos++; return Token{TokenType::RParen, ")"};
            case '[': pos++; return Token{TokenType::LBracket, "["};
            case ']': pos++; return Token{TokenType::RBracket, "]"};
            case ',': pos++; return Token{TokenType::Comma, ","};
            case '.': pos++; return Token{TokenType::Dot, "."};
            default:
                throw std::runtime_error(std::string("Unknown character in expression: ") + c);
        }
    }

private:
    std::string text;
    size_t pos;

    char peek() {
        if (pos < text.length()) {
            return text[pos];
        }
        return '\0';
    }

    char peekNextChar() {
        size_t nextPos = pos + 1;
        if (nextPos < text.length()) {
            return text[nextPos];
        }
        return '\0';
    }

    void skipWhitespace() {
        while (pos < text.length() && isspace(text[pos])) {
            pos++;
        }
    }

    Token identifier() {
        size_t start = pos;
        while (isalnum(peek()) || peek() == '_') {
            pos++;
        }
        return Token{TokenType::Identifier, text.substr(start, pos - start)};
    }

    Token number() {
        size_t start = pos;

        // Parse optional negative sign
        if (peek() == '-') pos++;

        // Parse integer part
        while (isdigit(peek())) {
            pos++;
        }

        // Parse fractional part
        if (peek() == '.') {
            pos++;
            while (isdigit(peek())) {
                pos++;
            }
        }

        // Get the number string
        return Token{TokenType::Number, text.substr(start, pos - start)};
    }

    Token string() {
        std::string result;

        // Consume '"'
        get();

        // Parse string characters
        while (true) {
            char c = get();
            if (c == '"') break;

            // Handle escape characters
            if (c == '\\') {
                char next = get();
                if (next == '"' || next == '\\' || next == '/') {
                    result += next;
                } else {
                    throw std::runtime_error("Invalid escape character in string");
                }
            } else {
                result += c;
            }
        }

        // Return the string token
        return Token{TokenType::String, result};
    }

    char get() {
        if (pos < text.length()) {
            return text[pos++];
        }
        return '\0';
    }
};

// Abstract Syntax Tree Nodes
struct Expression {
    // Base class for all expression nodes
    virtual ~Expression() = default;
};

// Number expression
struct NumberExpr : public Expression {
    double value;
    NumberExpr(double value) : value(value) {}
};

// String expression
struct StringExpr : public Expression {
    std::string value;
    StringExpr(const std::string& value) : value(value) {}
};

// Identifier expression
struct IdentifierExpr : public Expression {
    std::string name;
    IdentifierExpr(const std::string& name) : name(name) {}
};

// Binary operation expression: left op right
struct BinaryOpExpr : public Expression {
    char op;
    Expression* left;
    Expression* right;
    BinaryOpExpr(char op, Expression* left, Expression* right)
        : op(op), left(left), right(right) {}
};

// Unary operation expression: op operand
struct UnaryOpExpr : public Expression {
    char op;
    Expression* operand;
    UnaryOpExpr(char op, Expression* operand) : op(op), operand(operand) {}
};

// Function call expression: functionName(expr1, expr2, ...)
struct FunctionCallExpr : public Expression {
    std::string functionName;
    std::vector<Expression*> arguments;
    FunctionCallExpr(const std::string& functionName, const std::vector<Expression*>& arguments)
        : functionName(functionName), arguments(arguments) {}
};

// Subscript expression: base[index]
struct SubscriptExpr : public Expression {
    Expression* base;
    Expression* index;
    SubscriptExpr(Expression* base, Expression* index) : base(base), index(index) {}
};

// Member access expression: base.member
struct MemberAccessExpr : public Expression {
    Expression* base;
    std::string member;
    MemberAccessExpr(Expression* base, const std::string& member) : base(base), member(member) {}
};

// Parser
class Parser {
public:
    // Construct the Parser with a Lexer
    Parser(Lexer& lexer) : lexer(lexer) {
        currentToken = lexer.getNextToken();
    }

    // Parse the expression
    Expression* parseExpression() {
        return parseAddSubtract();
    }

private:
    Lexer& lexer;
    Token currentToken;

    // Consume the current token and move to the next one
    void eat(TokenType type) {
        if (currentToken.type == type) {
            currentToken = lexer.getNextToken();
        } else {
            throw std::runtime_error("Unexpected token");
        }
    }

    // Parse addition and subtraction
    Expression* parseAddSubtract() {
        Expression* left = parseMultiplyDivide();
        while (currentToken.type == TokenType::Plus || currentToken.type == TokenType::Minus) {
            char op = (currentToken.type == TokenType::Plus) ? '+' : '-';
            eat(currentToken.type);
            Expression* right = parseMultiplyDivide();
            left = new BinaryOpExpr(op, left, right);
        }
        return left;
    }

    // Parse multiplication and division
    Expression* parseMultiplyDivide() {
        Expression* left = parseUnary();
        while (currentToken.type == TokenType::Asterisk || currentToken.type == TokenType::Slash) {
            char op = (currentToken.type == TokenType::Asterisk) ? '*' : '/';
            eat(currentToken.type);
            Expression* right = parseUnary();
            left = new BinaryOpExpr(op, left, right);
        }
        return left;
    }

    // Parse unary operators
    Expression* parseUnary() {
        if (currentToken.type == TokenType::Minus) {
            eat(TokenType::Minus);
            Expression* operand = parseUnary();
            return new UnaryOpExpr('-', operand);
        } else {
            return parseSubscript();
        }
    }

    // Parse subscripts and member access
    Expression* parseSubscript() {
        Expression* expr = parsePrimary();
        while (true) {
            if (currentToken.type == TokenType::LBracket) { // Parse subscript
                eat(TokenType::LBracket);
                Expression* index = parseExpression();
                eat(TokenType::RBracket);
                expr = new SubscriptExpr(expr, index);
            } else if (currentToken.type == TokenType::Dot) { // Parse member access
                eat(TokenType::Dot);
                if (currentToken.type != TokenType::Identifier) {
                    throw std::runtime_error("Expected identifier after '.'");
                }
                std::string member = currentToken.value;
                eat(TokenType::Identifier);
                expr = new MemberAccessExpr(expr, member);
            } else {
                break;
            }
        }
        return expr;
    }

    Expression* parsePrimary() {
        if (currentToken.type == TokenType::Number) { // Number
            double value = std::stod(currentToken.value);
            eat(TokenType::Number);
            return new NumberExpr(value);
        } else if (currentToken.type == TokenType::String) { // String
            std::string value = currentToken.value;
            eat(TokenType::String);
            return new StringExpr(value);
        } else if (currentToken.type == TokenType::Identifier) { // Identifier or function call
            std::string name = currentToken.value;
            eat(TokenType::Identifier);
            if (currentToken.type == TokenType::LParen) {
                // Function call
                eat(TokenType::LParen);
                std::vector<Expression*> args;
                if (currentToken.type != TokenType::RParen) {
                    do {
                        Expression* arg = parseExpression();
                        args.push_back(arg);
                        if (currentToken.type == TokenType::Comma) {
                            eat(TokenType::Comma);
                        } else {
                            break;
                        }
                    } while (true);
                }
                eat(TokenType::RParen);
                return new FunctionCallExpr(name, args);
            } else {
                // Identifier
                return new IdentifierExpr(name);
            }
        } else if (currentToken.type == TokenType::LParen) { // Parenthesized expression
            eat(TokenType::LParen);
            Expression* expr = parseExpression();
            eat(TokenType::RParen);
            return expr;
        } else {
            throw std::runtime_error("Invalid expression");
        }
    }
};

// Evaluator
class Evaluator {
public:
    Evaluator(const JSONValue& root) : root(root) {}

    JSONValue evaluate(Expression* expr) {
        if (auto numExpr = dynamic_cast<NumberExpr*>(expr)) { // Number
            return JSONValue(numExpr->value);
        } else if (auto strExpr = dynamic_cast<StringExpr*>(expr)) { // String
            return JSONValue(strExpr->value);
        } else if (auto idExpr = dynamic_cast<IdentifierExpr*>(expr)) { // Identifier
            return getIdentifierValue(idExpr->name);
        } else if (auto binExpr = dynamic_cast<BinaryOpExpr*>(expr)) { // Binary operation
            JSONValue leftVal = evaluate(binExpr->left);
            JSONValue rightVal = evaluate(binExpr->right);

            // Check for operands of type number
            if (leftVal.type != JSONValueType::Number || rightVal.type != JSONValueType::Number) {
                throw std::runtime_error("Arithmetic operations require number operands");
            }

            // Perform the operation
            double leftNum = leftVal.numberValue;
            double rightNum = rightVal.numberValue;
            double result;
            switch (binExpr->op) {
                case '+': result = leftNum + rightNum; break;
                case '-': result = leftNum - rightNum; break;
                case '*': result = leftNum * rightNum; break;
                case '/':
                    if (rightNum == 0) throw std::runtime_error("Division by zero");
                    result = leftNum / rightNum; break;
                default:
                    throw std::runtime_error("Unknown binary operator");
            }

            // Return the result
            return JSONValue(result);
        } else if (auto unaryExpr = dynamic_cast<UnaryOpExpr*>(expr)) { // Unary operation
            // Evaluate the operand
            JSONValue operandVal = evaluate(unaryExpr->operand);

            // Check for operand of type number
            if (operandVal.type != JSONValueType::Number) {
                throw std::runtime_error("Unary operator requires a number operand");
            }

            // Perform the operation
            double operandNum = operandVal.numberValue;
            double result;
            switch (unaryExpr->op) {
                case '-': result = -operandNum; break;
                default:
                    throw std::runtime_error("Unknown unary operator");
            }

            // Return the result
            return JSONValue(result);
        } else if (auto funcExpr = dynamic_cast<FunctionCallExpr*>(expr)) {
            // Evaluate function arguments
            std::vector<JSONValue> args;
            for (auto argExpr : funcExpr->arguments) {
                args.push_back(evaluate(argExpr));
            }

            // Call the function
            return evaluateFunction(funcExpr->functionName, args);
        } else if (auto subExpr = dynamic_cast<SubscriptExpr*>(expr)) {
            // Evaluate the base and index expressions
            JSONValue baseVal = evaluate(subExpr->base);
            JSONValue indexVal = evaluate(subExpr->index);

            // Check for base value of type array or object
            if (baseVal.type == JSONValueType::Array) {
                // Check for index value of type number
                if (indexVal.type != JSONValueType::Number) {
                    throw std::runtime_error("Array index must be a number");
                }

                // Check for valid index
                int idx = static_cast<int>(indexVal.numberValue);
                if (idx < 0 || idx >= baseVal.arrayValue.size()) {
                    throw std::runtime_error("Array index out of bounds");
                }
            
                // Return the array element
                return baseVal.arrayValue[idx];
            } else if (baseVal.type == JSONValueType::Object) {
                // Check for index value of type string
                if (indexVal.type != JSONValueType::String) {
                    throw std::runtime_error("Object key must be a string");
                }

                // Check for key in object
                auto it = baseVal.objectValue.find(indexVal.stringValue);
                if (it == baseVal.objectValue.end()) {
                    throw std::runtime_error("Key not found in object");
                }

                // Return the object value
                return it->second;
            } else {
                // Base value is not an array or object
                throw std::runtime_error("Subscript operator applied to non-array/object");
            }
        } else if (auto memberExpr = dynamic_cast<MemberAccessExpr*>(expr)) {
            // Evaluate the base expression
            JSONValue baseVal = evaluate(memberExpr->base);

            // Check for base value of type object
            if (baseVal.type != JSONValueType::Object) {
                throw std::runtime_error("Member access applied to non-object");
            }

            // Check for member in object
            auto it = baseVal.objectValue.find(memberExpr->member);
            if (it != baseVal.objectValue.end()) {
                return it->second;
            } else {
                throw std::runtime_error("Member not found in object");
            }
        } else {
            throw std::runtime_error("Unknown expression type");
        }
    }

private:
    const JSONValue& root;

    JSONValue getIdentifierValue(const std::string& name) {
        // Start from the root object
        if (root.type != JSONValueType::Object) {
            throw std::runtime_error("Root is not an object");
        }

        // Find the identifier in the root object
        auto it = root.objectValue.find(name);
        if (it != root.objectValue.end()) {
            return it->second;
        } else {
            throw std::runtime_error("Identifier not found: " + name);
        }
    }

    JSONValue evaluateFunction(const std::string& name, const std::vector<JSONValue>& args) {
        if (name == "min") { // min function
            if (args.empty()) {
                throw std::runtime_error("min() requires at least one argument");
            }

            // Find the minimum value
            double minVal = std::numeric_limits<double>::infinity();
            for (const auto& arg : args) {
                if (arg.type == JSONValueType::Array) { // Array argument
                    for (const auto& item : arg.arrayValue) {
                        if (item.type != JSONValueType::Number) {
                            throw std::runtime_error("min() array items must be numbers");
                        }
                        minVal = std::min(minVal, item.numberValue);
                    }
                } else if (arg.type == JSONValueType::Number) { // Number argument
                    minVal = std::min(minVal, arg.numberValue);
                } else {
                    throw std::runtime_error("min() arguments must be numbers or arrays of numbers");
                }
            }

            // Return the minimum value
            return JSONValue(minVal);
        } else if (name == "max") { // max function
            if (args.empty()) {
                throw std::runtime_error("max() requires at least one argument");
            }

            // Find the maximum value
            double maxVal = -std::numeric_limits<double>::infinity();
            for (const auto& arg : args) {
                if (arg.type == JSONValueType::Array) { // Array argument
                    for (const auto& item : arg.arrayValue) {
                        if (item.type != JSONValueType::Number) {
                            throw std::runtime_error("max() array items must be numbers");
                        }
                        maxVal = std::max(maxVal, item.numberValue);
                    }
                } else if (arg.type == JSONValueType::Number) { // Number argument
                    maxVal = std::max(maxVal, arg.numberValue);
                } else {
                    throw std::runtime_error("max() arguments must be numbers or arrays of numbers");
                }
            }

            // Return the maximum value
            return JSONValue(maxVal);
        } else if (name == "size") {
            // Check for exactly one argument
            if (args.size() != 1) {
                throw std::runtime_error("size() requires exactly one argument");
            }

            // Get the size of the argument
            const auto& arg = args[0];
            if (arg.type == JSONValueType::Object) {
                return JSONValue(static_cast<double>(arg.objectValue.size()));
            } else if (arg.type == JSONValueType::Array) {
                return JSONValue(static_cast<double>(arg.arrayValue.size()));
            } else if (arg.type == JSONValueType::String) {
                return JSONValue(static_cast<double>(arg.stringValue.length()));
            } else {
                throw std::runtime_error("size() argument must be object, array, or string");
            }
        } else {
            throw std::runtime_error("Unknown function: " + name);
        }
    }
};

// Output JSON value
void outputResult(const JSONValue& value, bool isRoot = true) {
    switch (value.type) {
        case JSONValueType::Null:
            std::cout << "null";
            break;
        case JSONValueType::Number:
            std::cout << value.numberValue;
            break;
        case JSONValueType::String:
            std::cout << "\"" << value.stringValue << "\"";
            break;
        case JSONValueType::Array:
            std::cout << "[ ";
            for (size_t i = 0; i < value.arrayValue.size(); ++i) {
                if (i > 0) std::cout << ", ";
                outputResult(value.arrayValue[i], false);
            }
            std::cout << " ]";
            break;
        case JSONValueType::Object:
            std::cout << "{ ";
            size_t count = 0;
            for (const auto& pair : value.objectValue) {
                if (count > 0) std::cout << ", ";
                std::cout << "\"" << pair.first << "\": ";
                outputResult(pair.second, false);
                count++;
            }
            std::cout << " }";
            break;
    }

    if (isRoot) {
        std::cout << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./json_eval <json_file> <expression>" << std::endl;
        return 1;
    }
    std::string jsonFilename = argv[1];
    std::string expressionText = argv[2];

    // Read JSON file
    std::ifstream jsonFile(jsonFilename);
    if (!jsonFile) {
        std::cerr << "Error: Cannot open JSON file: " << jsonFilename << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << jsonFile.rdbuf();
    std::string jsonText = buffer.str();

    // Parse JSON
    JSONValue root;
    try {
        JSONParser parser(jsonText);
        root = parser.parse();
    } catch (const std::exception& ex) {
        std::cerr << "JSON parsing error: " << ex.what() << std::endl;
        return 1;
    }

    // Parse expression
    Expression* expr = nullptr;
    try {
        Lexer lexer(expressionText);
        Parser parser(lexer);
        expr = parser.parseExpression();
    } catch (const std::exception& ex) {
        std::cerr << "Expression parsing error: " << ex.what() << std::endl;
        return 1;
    }

    // Evaluate expression
    JSONValue result = JSONValue();
    try {
        Evaluator evaluator(root);
        result = evaluator.evaluate(expr);
    } catch (const std::exception& ex) {
        std::cerr << "Evaluation error: " << ex.what() << std::endl;
        return 1;
    }

    // Output result
    outputResult(result);

    // Clean up
    delete expr;
    return 0;
}
