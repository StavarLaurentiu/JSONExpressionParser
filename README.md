# JSON Expression Evaluator #

This project is a simple JSON expression evaluator written in C++. It parses JSON data from a file and evaluates expressions provided as command-line arguments. The evaluator supports arithmetic operations, function calls, member access, subscript expressions, and more.

## Features ##

JSON Parsing: Parses JSON files containing objects, arrays, strings, and numbers.
Expression Evaluation: Evaluates expressions involving:
 - Arithmetic operations: +, -, *, /
 - Unary operations: unary minus (-)
 - Function calls: min(), max(), size()
 - Member access: object.property
 - Subscript expressions: array[index], object["key"]
 - Nested expressions and operator precedence
Error Handling: Provides descriptive error messages for invalid expressions or operations.

## Files ##

json_eval.cpp: The main C++ source code containing the implementation.
Makefile: Makefile for building the application and running tests.
test.json: Sample JSON file used for testing.
test.sh: Shell script containing a series of test cases to verify the evaluator's functionality.

## Requirements ##

Compiler: C++ compiler supporting C++11 standard or later (e.g., g++, clang++)
Build Tool: Make utility (make)
Environment: Unix-like environment (for running test.sh)

## Building the Application ##

Follow the steps below to build the JSON expression evaluator:

1. Ensure All Files Are Present
    
Make sure the following files are in your project directory:
 - json_eval.cpp
 - Makefile
 - test.json
 - test.sh
 - Build the Application

2. Open a terminal in the project directory and run: $ make

This will compile json_eval.cpp, create an executable named json_eval and give exacutable permisions to the test.sh

## Running the Application ##

The application requires two command-line arguments:

1. JSON File: The path to the JSON file.
2. Expression: The expression to evaluate.

./json_eval <json_file> <expression>

## Running Test Cases ##

The test.sh script contains a series of test cases to verify the evaluator's functionality.

Usage: ./test.sh

Alternatively, you can use the make target:

Usage: make run_tests

## Cleaning Up ##

To clean up the compiled executable, run: $ make clean

This command will remove the json_eval executable.

## Project Structure ##

 - JSON Parsing: Implemented in the JSONParser class.
 - Lexical Analysis: Handled by the Lexer class, which tokenizes the input expression.
 - Parsing Expressions: The Parser class constructs an Abstract Syntax Tree (AST) from the tokens.
 - Evaluation: The Evaluator class recursively evaluates the AST using the parsed JSON data.
 - AST Nodes: Various expression types are represented by classes derived from Expression.
  
## Known Limitations ##

1. Boolean Values: The current implementation treats boolean values (true, false) as strings if they are enclosed in quotes. Accessing unquoted boolean values may result in errors.
2. String Operations: Arithmetic operations on strings (e.g., concatenation) are not supported.
3. Error Messages: Some error messages may be generic. Improvements can be made to provide more specific feedback.
