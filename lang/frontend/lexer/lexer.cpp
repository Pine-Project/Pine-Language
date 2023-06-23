#include "lexer.hpp"



// this function assumes the opening of the comment has already been consumed. This consumes the comment text itself and the closing of it
static void handle_multiline_comment(pine_frontend::scanner &scanner) {
    for(;;) {
        // TODO: make sure this doesn't index out of bounds for EOF when encountering unterminated multiline comments
        switch(scanner.advance_char()) {
            case '*':
                if(scanner.at_end_of_str() || scanner.advance_char() == '/') return; // comment has been terminated
            default:
                if(scanner.at_end_of_str()) return;
            // consume character anyways since it is part of the comment regardless
        }
    }
}

// this function assumes the opening of the comment has already been consumed. This consumes the comment text itself and the closing of it
static void handle_single_line_comment(pine_frontend::scanner &scanner) {
    for(;;) {
        // TODO: make sure this doesn't index out of bounds for EOF
        switch(scanner.advance_char()) {
            case '\n':
                ++scanner.current_line;
                return;
            default:
                if(scanner.at_end_of_str()) return;
            // keep consuming characters since they are part of the comment
        }
    }
}


bool pine_frontend::handle_comment(pine_frontend::scanner &scanner) {
    for(;;) {
        switch(scanner.peek_char()) {
            case '/':
                if(scanner.peek_char_n(1) == '*') {
                    scanner.advance_char(); // skip over `/` in string
                    scanner.advance_char(); // skip over `*` in string
                    handle_multiline_comment(scanner);
                    return true;
                } else if(scanner.peek_char_n(1) == '/') {
                    scanner.advance_char(); // skip over first `/` in string
                    scanner.advance_char(); // skip over second `/` in string
                    handle_single_line_comment(scanner);
                    return true;
                }
                // no characters consumed, leave comment scanner
                [[fallthrough]];
            default:
                return false;
        }
    }
}


void pine_frontend::skip_whitespace(pine_frontend::scanner &scanner) {
    for(;;) {
        switch(scanner.peek_char()) {
            case '\n':
                ++scanner.current_line;
                [[fallthrough]];
            case ' ':
            case '\t':
            case '\r':
                scanner.advance_char();
                break;
            default:
                return;
        }
    }
}





static pine_frontend::token_type_t match_keyword(const pine_frontend::scanner &scanner, const std::uint32_t start_index, std::string_view expected,
        const pine_frontend::token_type_t expected_type) {

    if(((scanner.current - scanner.start) == (start_index + expected.length())) && std::memcmp(expected.data(), scanner.start + start_index, expected.length()) == 0) {
        return expected_type;
    }
    return pine_frontend::token_type_t::IDENTIFIER;
}



static pine_frontend::token_type_t handle_keywords(const pine_frontend::scanner &scanner) {
    using namespace pine_frontend;

    // TODO: maybe do some code reuse and whatnot
    assert((scanner.current - scanner.start) > 0); // scanner.current != scanner.start
    switch(scanner.start[0]) {
        case 'i':
            return match_keyword(scanner, 1, "f", token_type_t::IF);
        case 'e':
            return match_keyword(scanner, 1, "lse", token_type_t::ELSE);
        case 'f':
            if((scanner.current - scanner.start) > 1) {
                switch(scanner.start[1]) {
                    case 'u':
                        return match_keyword(scanner, 2, "n", token_type_t::FUN);
                    case 'a':
                        return match_keyword(scanner, 2, "lse", token_type_t::FALSE);
                    case 'o':
                        return match_keyword(scanner, 2, "r", token_type_t::FOR);
                }
            }
            break; // full identifier is `f`
        case 't':
            return match_keyword(scanner, 1, "rue", token_type_t::TRUE);
        case 's':
            return match_keyword(scanner, 1, "ruct", token_type_t::STRUCT);
        case 'w':
            return match_keyword(scanner, 1, "while", token_type_t::WHILE); // could be do/while or while. Will have to disambiguate later.
        case 'd':
            return match_keyword(scanner, 1, "o", token_type_t::DO_WHILE); // just lexes the `do` part for now. Will have to distinguish between while and do/while later.
        case 'l':
            if((scanner.current - scanner.start) > 1) {
                switch(scanner.start[1]) {
                    case 'o':
                        return match_keyword(scanner, 2, "op", token_type_t::LOOP);
                    case 'e':
                        return match_keyword(scanner, 2, "t", token_type_t::LET);
                }
            }
            break; // full identifier is `l`
        case 'r':
            return match_keyword(scanner, 1, "eturn", token_type_t::RETURN);
        case 'm':
            return match_keyword(scanner, 1, "ut", token_type_t::MUT);
    }

    return token_type_t::IDENTIFIER;
}




pine_frontend::token_type_t pine_frontend::handle_number(pine_frontend::scanner &scanner) {
    while(pine_stl::is_digit(scanner.peek_char())) {
        scanner.advance_char();
    }
    if(scanner.peek_char() == '.') {
        scanner.advance_char();
        while(pine_stl::is_digit(scanner.peek_char())) {
            scanner.advance_char();
        }
        return token_type_t::FLOAT;
    }
    return token_type_t::INTEGER;
}
pine_frontend::token_type_t pine_frontend::handle_identifier(pine_frontend::scanner &scanner) {
    while(pine_stl::is_alpha_numeric(scanner.peek_char())) {
        scanner.advance_char();
    }
    return handle_keywords(scanner);
}



pine_frontend::token_type_t pine_frontend::handle_string(pine_frontend::scanner &scanner) {
    // we use peek_char() instead of advance_char() for this while loop so we can easily catch unterminated strings and report them
    while(!scanner.at_end_of_str() && scanner.peek_char() != '"') {
        scanner.advance_char();
    }

    if(scanner.at_end_of_str()) {
        std::cerr << "Error: unterminated string\n";
        return token_type_t::ERROR;
    }

    scanner.advance_char(); // consume closing `"`

    return token_type_t::STRING;
}


// This does not check to make sure that the string inside of `[]` is valid and does not the values.
//  In other words, [1, 2, 3] is valid, but [1 2 3,,, 6] is not, but both will lex to token_type_t::LIST with the strings `[1, 2 3]` and `[1 2 3,,, 6]` stored respectively.
//  We will handle lexing the inside of the `[]` and checking the syntax later before we type check it.
// TODO: consider whether or not we should handle syntax errors here or later.
pine_frontend::token_type_t pine_frontend::handle_list(pine_frontend::scanner &scanner) {
    // we use peek_char() instead of advance_char() for this while loop so we can easily catch unterminated strings and report them
    while(!scanner.at_end_of_str() && scanner.peek_char() != ']') {
        scanner.advance_char();
    }

    if(scanner.at_end_of_str()) {
        std::cerr << "Error: unterminated list\n";
        return token_type_t::ERROR;
    }

    scanner.advance_char(); // consume closing `]`

    return token_type_t::LIST;
}










pine_frontend::token_t pine_frontend::scan_token(pine_frontend::scanner &scanner) {
    scanner.start = scanner.current;

    do {
        skip_whitespace(scanner);
    } while(handle_comment(scanner));

    scanner.start = scanner.current;

    using namespace pine_frontend;

    if(scanner.at_end_of_str()) {
        return token_t(scanner, token_type_t::EOF_VAL);
    }


    const char c = scanner.advance_char();

    if(pine_stl::is_digit(c)) {
        return token_t(scanner, handle_number(scanner)); // could be either an INTEGER or FLOAT token returned
    }
    if(pine_stl::is_alpha(c)) {
        return token_t(scanner, handle_identifier(scanner));
    }

    switch(c) {
        case '(':
            return token_t(scanner, token_type_t::LEFT_PAREN);
        case ')':
            return token_t(scanner, token_type_t::RIGHT_PAREN);
        case '{':
            return token_t(scanner, token_type_t::LEFT_CURLY);
        case '}':
            return token_t(scanner, token_type_t::RIGHT_CURLY);
        case ',':
            return token_t(scanner, token_type_t::COMMA);
        case '.':
            return token_t(scanner, token_type_t::DOT);
        case ';':
            return token_t(scanner, token_type_t::SEMICOLON);
        case ':':
            return token_t(scanner, token_type_t::COLON);
        case '+':
            return token_t(scanner, token_type_t::PLUS);
        case '-':
            return token_t(scanner, token_type_t::MINUS);
        case '*':
            return token_t(scanner, token_type_t::STAR);
        case '/':
            return token_t(scanner, token_type_t::SLASH);
        case '!':
            return token_t(scanner, scanner.match_char('=') ? token_type_t::NOT_EQUAL : token_type_t::LOGICAL_NOT);
        case '|':
            return token_t(scanner, scanner.match_char('|') ? token_type_t::LOGICAL_OR : token_type_t::BITWISE_OR);
        case '&':
            return token_t(scanner, scanner.match_char('&') ? token_type_t::LOGICAL_AND : token_type_t::BITWISE_AND);
        case '~':
            return token_t(scanner, token_type_t::BITWISE_NOT);
        case '^':
            return token_t(scanner, token_type_t::BITWISE_XOR);
        case '=':
            return token_t(scanner, scanner.match_char('=') ? token_type_t::EQUAL : token_type_t::ASSIGNMENT);
        case '>': {
            if(scanner.match_char('>')) return token_t(scanner, token_type_t::BITSHIFT_RIGHT);

            return token_t(scanner, scanner.match_char('=') ? token_type_t::GREATER_EQUAL : token_type_t::GREATER);
        }
        case '<': {
            if(scanner.match_char('<')) return token_t(scanner, token_type_t::BITSHIFT_LEFT);

            return token_t(scanner, scanner.match_char('=') ? token_type_t::LESS_EQUAL : token_type_t::LESS);
        }


        case '\"':
            return token_t(scanner, handle_string(scanner));
        case '[':
            return token_t(scanner, handle_list(scanner));
    }

    std::printf("Unexpected token: [%.*s]\n", scanner.current-scanner.start, scanner.start);
    return token_t(scanner, token_type_t::ERROR);
}

std::vector<pine_frontend::token_t> pine_frontend::scan_all_tokens(pine_frontend::scanner &scanner) {
    std::vector<pine_frontend::token_t> tokens;
    for(;;) {
        const auto token = scan_token(scanner);
        tokens.push_back(token);
        if(token.type == token_type_t::EOF_VAL) {
            break;
        }
    }
    return tokens;
}



