# Bytecode

A bytecode-based interpreter for a python-like language.

## Lexing

The Lexer just subdivides the incoming text into words, identations, newlines and string and number literals. \
Comments start with '#' and ends with a newline. They are ignored by the Lexer.

## Parsing

This compiler implements a parsing descent parser with the following grammar:
```
Expression       = Factor | Factor + Expression | Factor - Expression
Factor           = UnaryOp | UnaryOp * Factor | UnaryOp / Factor
UnaryOp          = Value | -Value
Value            = Number | String | IdentifierAccess | (Expression)
Identifieraccess = IdentifierAccess | Identifier
Access           = None | .Identifieraccess | [Expression]Access | (Parameters)Access
 ```