# Simple Compiler with Tokenization & CFG Parsing in C++

## Overview

This project implements a compiler for a simple programming language that performs:

- **Lexical Analysis:** Uses Finite State Automata (FSAs) to tokenize input source code into identifiers, keywords, integers, floating-point numbers, and operator symbols.
- **Syntax Analysis:** Applies recursive descent parsing according to a context-free grammar (CFG) to validate conditional expressions and statements.
- Comprehensive error handling to detect and report lexical errors (invalid tokens) and syntax errors (grammar violations).

## Features

- **FSA-based tokenization** enabling fast and accurate token recognition.
- **Keyword priority** ensures correct classification of reserved words versus identifiers.
- Full support for numeric literals, including integers and floating-point numbers.
- Recursive descent CFG parser supporting nested statements and expressions.
- Outputs clear status messages:
  - `No Error` for successful parse,
  - `Lexical Error` for invalid tokens,
  - `Syntax Error` for grammar issues.
- Command-line tool interface for compiling single-line inputs.

## Usage

1. Compile the source code with a C++17 compatible compiler:
```bash
g++ -std=c++17 -o compiler compiler.cpp
```
2. Run the compiler:
```bash
./compiler
```
3. Enter a single-line source code input. The program will output one of:

- `No Error`  
- `Lexical Error`  
- `Syntax Error`

## Code Structure

- **Lexer:** Implements FSAs for tokenizing input text.
- **Parser:** Implements a recursive descent parser for the CFG.
- **Main Program:** Handles input, runs lexer and parser, and outputs result.

## Example

Input:

if 2 + xi > 0 print 2.0 else print -1;

Output:

No Error

