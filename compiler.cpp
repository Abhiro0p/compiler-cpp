#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <stdexcept>
#include <unordered_set>

// Enum for token types
enum class TokenType {
    IDENTIFIER,
    KEYWORD,
    INTEGER,
    FLOAT,
    SYMBOL,
    END_OF_FILE
};

// Token class
struct Token {
    TokenType type;
    std::string value;
    size_t position;

    Token(TokenType type, const std::string& value, size_t pos)
        : type(type), value(value), position(pos) {}

    void print() const {
        static const char* typeNames[] = {
            "IDENTIFIER", "KEYWORD", "INTEGER", "FLOAT", "SYMBOL", "EOF"
        };
        std::cout << "Token Type: " << typeNames[static_cast<int>(type)]
                  << ", Token Value: " << value << std::endl;
    }
};

// Exception classes
class LexicalError : public std::runtime_error {
public:
    explicit LexicalError(const std::string& message)
        : std::runtime_error(message) {}
};

class SyntaxError : public std::runtime_error {
public:
    explicit SyntaxError(const std::string& message)
        : std::runtime_error(message) {}
};

// Lexer class
class Lexer {
private:
    std::string input;
    size_t pos;
    char current_char;

    std::unordered_set<std::string> keywords {"if", "else", "print"};
    std::unordered_set<char> operators {'+', '-', '*', '/', '^', '<', '>', '='};
    std::unordered_set<char> symbols {';', '(', ')'};

    void advance() {
        pos++;
        if (pos < input.size()) {
            current_char = input[pos];
        } else {
            current_char = '\0'; // EOF 
        }
    }

    char peek() const {
        if (pos + 1 < input.size())
            return input[pos + 1];
        else
            return '\0';
    }

    void skip_whitespace() {
        while (current_char != '\0' && std::isspace(current_char)) {
            advance();
        }
    }

    // Number FSA: integers and floats
    Token number() {
        size_t start_pos = pos;
        std::string result;
        bool is_float = false;

        // Integer part
        while (current_char != '\0' && std::isdigit(current_char)) {
            result.push_back(current_char);
            advance();
        }

        // Fractional part
        if (current_char == '.') {
            is_float = true;
            result.push_back(current_char);
            advance();

            if (!std::isdigit(current_char)) {
                throw LexicalError("Lexical Error");
            }

            while (current_char != '\0' && std::isdigit(current_char)) {
                result.push_back(current_char);
                advance();
            }
        }

        return Token(is_float ? TokenType::FLOAT : TokenType::INTEGER, result, start_pos);
    }

    // Identifier or keyword FSA
    Token identifier_or_keyword() {
        size_t start_pos = pos;
        std::string result;

        if (!std::isalpha(current_char) && current_char != '_') {
            throw LexicalError("Lexical Error");
        }

        while (current_char != '\0' && (std::isalnum(current_char) || current_char == '_')) {
            result.push_back(current_char);
            advance();
        }

        if (keywords.find(result) != keywords.end()) {
            return Token(TokenType::KEYWORD, result, start_pos);
        } else {
            return Token(TokenType::IDENTIFIER, result, start_pos);
        }
    }

public:
    explicit Lexer(const std::string& text) : input(text), pos(0) {
        if (!input.empty()) {
            current_char = input[0];
        } else {
            current_char = '\0';
        }
    }

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;

        while (current_char != '\0') {
            skip_whitespace();

            if (current_char == '\0') {
                break;
            }

            // If digit: check if invalid identifier starting with digit
            if (std::isdigit(current_char)) {
                // Peek next character after digits to check for invalid identifier
                size_t temp_pos = pos;
                std::string temp_str;
                while (temp_pos < input.size() && (std::isalnum(input[temp_pos]) || input[temp_pos] == '_')) {
                    temp_str.push_back(input[temp_pos]);
                    temp_pos++;
                }
                if (temp_str.length() > 1 && std::isalpha(temp_str[1])) {
                    // Invalid identifier starting with digit
                    temp_str = "";
                    while (pos < input.size() && (std::isalnum(input[pos]) || input[pos] == '_')) {
                        temp_str.push_back(input[pos]);
                        advance();
                    }
                    throw LexicalError("Lexical Error");
                }

                tokens.push_back(number());
                continue;
            }

            if (std::isalpha(current_char) || current_char == '_') {
                tokens.push_back(identifier_or_keyword());
                continue;
            }

            if (operators.count(current_char) > 0 || symbols.count(current_char) > 0) {
                tokens.push_back(Token(TokenType::SYMBOL, std::string(1, current_char), pos));
                advance();
                continue;
            }

            // Unexpected character
            throw LexicalError("Lexical Error");
        }

        tokens.push_back(Token(TokenType::END_OF_FILE, "", pos));
        return tokens;
    }
};

// Parser class implementing recursive descent parsing
class Parser {
private:
    std::vector<Token> tokens;
    size_t pos;
    Token current_token;

    void advance() {
        pos++;
        if (pos < tokens.size()) {
            current_token = tokens[pos];
        } else {
            current_token = Token(TokenType::END_OF_FILE, "", pos);
        }
    }

    bool match(TokenType type, const std::string& value = "") {
        if (current_token.type != type) return false;
        if (!value.empty() && current_token.value != value) return false;
        return true;
    }

    // Rule S -> statement
    void parseS() {
        parse_statement();
    }

    // statement → if (A) | y (statement alphabets) | (statement)(statement)
    void parse_statement() {
        if (match(TokenType::KEYWORD, "else")) {
            throw SyntaxError("Syntax Error");
        }

        if (match(TokenType::KEYWORD, "if")) {
            advance();
            parse_A();
        } else if (is_statement_alphabet()) {
            advance();
            // Optionally parse another statement (statement)(statement)
            if (is_statement_start()) {
                parse_statement();
            }
        } else {
            throw SyntaxError("Syntax Error");
        }
    }

    bool is_statement_start() {
        return match(TokenType::KEYWORD, "if") || is_statement_alphabet();
    }

    // y ∈ statement alphabets (numbers ∪ keywords ∪ identifiers - 'if' and 'else')
    bool is_statement_alphabet() {
        if (current_token.type == TokenType::INTEGER ||
            current_token.type == TokenType::FLOAT ||
            current_token.type == TokenType::IDENTIFIER) {
            return true;
        }
        if (current_token.type == TokenType::KEYWORD &&
            current_token.value != "if" && current_token.value != "else") {
            return true;
        }
        if (current_token.type == TokenType::SYMBOL) { // symbols allowed in alphabet
            return true;
        }
        return false;
    }

    // A → (cond)(statement) | (cond)(statement)(else)(statement)
    void parse_A() {
        parse_cond();
        parse_statement();
        if (match(TokenType::KEYWORD, "else")) {
            advance();
            parse_statement();
        }
    }

    // cond → (x)(op1)(x) | x
    void parse_cond() {
        size_t backup_pos = pos;
        Token backup_token = current_token;
        if (parse_x()) {
            if (is_op1()) {
                advance();
                if (parse_x()) {
                    return;
                } else {
                    pos = backup_pos;
                    current_token = backup_token;
                    parse_x(); // fallback to just x
                    return;
                }
            } else {
                return; // just x
            }
        } else {
            throw SyntaxError("Syntax Error");
        }
    }

    // x → R | cond | y
    bool parse_x() {
        if (current_token.type == TokenType::INTEGER ||
            current_token.type == TokenType::FLOAT ||
            current_token.type == TokenType::IDENTIFIER ||
            (current_token.type == TokenType::KEYWORD &&
             current_token.value != "if" && current_token.value != "else")) {
            advance();
            return true;
        }
        return false;
    }

    // op1 → + | - | * | / | ^ | < | > | =
    bool is_op1() {
        if (current_token.type == TokenType::SYMBOL) {
            char c = current_token.value[0];
            return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' ||
                   c == '<' || c == '>' || c == '=';
        }
        return false;
    }

public:
    explicit Parser(const std::vector<Token>& tokens) : tokens(tokens), pos(0), 
        current_token(tokens.size() > 0 ? tokens[0] : Token(TokenType::END_OF_FILE, "", 0)) {
    }

    void parse() {
        parseS();
        if (current_token.type != TokenType::END_OF_FILE)
            throw SyntaxError("Syntax Error");
    }
};

int main() {
    std::string line;
    std::getline(std::cin, line);

    try {
        Lexer lexer(line);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        parser.parse();

        std::cout << "No Error" << std::endl;
    } catch (const LexicalError& e) {
        std::cout << "Lexical Error" << std::endl;
    } catch (const SyntaxError& e) {
        std::cout << "Syntax Error" << std::endl;
    }

    return 0;
}
