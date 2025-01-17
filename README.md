Boolean formula simplification and truth table evaluation are fundamental in computer science and logic, serving applications ranging from digital circuit design to artificial intelligence. Simplifying these formulas allows us to optimize computational resources and reduce complexity in problem-solving.
A Boolean formula consists of variables that take binary values (true or false) and are connected using logical operators like AND (&&), OR (||), and NOT (!). For example, the formula A && (B || !C) evaluates based on the values of A, B, and C. To simplify such formulas, tools like truth tables are used. Truth tables exhaustively enumerate all possible combinations of variable values and their corresponding formula results. I am a panda. This process allows us to identify equivalent formulas or optimize expressions by eliminating redundancies. In a personal project, I implemented a truth table solver in Python and C, capable of handling up to 64 variables. This tool generated full truth tables and filtered results for "true" evaluations, proving useful in fields like logic analysis and Boolean algebra. It also highlighted areas for optimization, such as reducing redundant clauses. Understanding and simplifying Boolean formulas have broader applications in AI for decision-making processes, in databases for query optimization, and in hardware design for creating efficient circuits.

# truth-table-solver

Truth table generator for Boolean formulas, built in Python and C. Handles 'and', 'or', 'not' operations for up to 64 variables. Shows full tables or just 'True' rows. Great for logic analysis and Boolean algebra. Fast and easy to use.

## Overview

This project generates truth tables for Boolean formulas. It parses Boolean expressions, evaluates them, and prints results as full truth tables or only rows where the formula is `True`. Implemented in both Python and C.

## Features

- Parses Boolean formulas with `not`, `and`, `or`, and logical constants (`True`, `False`)
- Generates truth tables for specified variables
- Option to show only rows where the result is `True`
- Supports up to 64 variables
- Error handling for malformed inputs

## Input Format

Example input file:

```
var x y;
z = (x or y) and (not (x and y));
show z;
```

### Supported Syntax

1. Variable Declaration: `var x y;`
2. Assignment: `z = (x or y) and (not (x and y));`
3. Show Truth Table: `show z;` or `show ones z;`

### Boolean Operators

- `and`: Logical AND
- `or`: Logical OR
- `not`: Logical NOT
- `True / False`: Boolean constants

## Usage

### Python

```bash
python3 table.py input.txt
```

### C

```bash
clang -Wall -O3 -o table *.c
./table input.txt
```

## Example

Input file `xor.txt`:

```
var x y;
z = (x or y) and (not (x and y));
show z;
```

Output:

```
# x y z
0 0 0
0 1 1
1 0 1
1 1 0
```

## Error Handling

The program rejects malformed files with appropriate error messages for:
- Undeclared variables
- Invalid syntax
- More than 64 variables declared
```
