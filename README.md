

# Compiler
a Compiler (Maybe).


# Module -1: PP:
Converts .c to .i  #linker when?


# Module 0: Lexer:
Reads source code txt file and parse it into tokens and objects that later modules can work with.


# Module 1: Analyzer:
Parses functions and their code lines, parses by line,
breaks each code line into their type (eval, decl, control flow etc.),
saves the instructions to be evaluated at the line, ordered by precedence.


# Module 2: Depender:
Creates a tree of dependencies between each variable / instruction.


# Module 3: Optimizer:
Makes RAM go VROOM VROOM
Changes the instructions to make the code faster but preserves the dependencies and order of precedence, between sequence points.


# Module 4: (Assembly) Builder
Builds the assembly for each function.




