
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
