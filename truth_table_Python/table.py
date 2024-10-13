import sys

# Base class for all nodes in AST
class TreeNode:
    def evaluate(self, assignments):
        pass  # To be defined in subclasses

#Bool
class Boolean(TreeNode):
    def __init__(self, value):
        self.value = value

    def evaluate(self, assignments):
        return self.value  # Just return the Boolean value

#Variable
class Variable(TreeNode):
    def __init__(self, name):
        self.name = name

    def evaluate(self, assignments):
        return assignments[self.name]  # Get the variable's value from assignments

#not
class Not(TreeNode):
    def __init__(self, child):
        self.child = child

    def evaluate(self, assignments):
        return not self.child.evaluate(assignments)  # Return the opposite of the child's value

#and
class And(TreeNode):
    def __init__(self, left, right):
        self.left = left
        self.right = right

    def evaluate(self, assignments):
        left_result = self.left.evaluate(assignments)
        if not left_result:
            return False  # Short-circuit: no need to evaluate the right side if left is False
        return self.right.evaluate(assignments)  # Evaluate the right side

#or
class Or(TreeNode):
    def __init__(self, left, right):
        self.left = left
        self.right = right

    def evaluate(self, assignments):
        left_result = self.left.evaluate(assignments)
        if left_result:
            return True  # Short-circuit: no need to evaluate the right side if left is True
        return self.right.evaluate(assignments)  # Evaluate the right side

# Tokenizer into list
def tokenizer(input_data: str) -> list:
    special = ["(", ")", "=", ";"]  # Special characters we need to handle
    keywords = ["var", "show", "show_ones", "not", "and", "or", "True", "False"]
    word_characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789"
    tokens = []

    for line in input_data.splitlines():
        line = line.strip()  # Clean up whitespace
        if line == '' or line.startswith("#"):  # Skip empty lines or comments
            continue

        # Handle inline comments
        if '#' in line:
            line = line.split('#', 1)[0].strip()  # Remove comments
            if line == '':
                continue  # Skip if nothing is left

        word = ""
        i = 0
        while i < len(line):
            char = line[i]

            #special characters check
            if char in special:
                if word:
                    tokens.append(f"{'keyword' if word in keywords else 'identifier'}: {word}")
                    word = ""
                tokens.append(char)  
                i += 1
                continue

            #spaces check
            if char == " ":
                if word:
                    tokens.append(f"{'keyword' if word in keywords else 'identifier'}: {word}")
                    word = ""
                i += 1
                continue

            # Build words (identifiers or keywords)
            if char in word_characters:
                word += char
                i += 1
            else:
                raise ValueError(f"Invalid character {char} in line {line}")  # Handle invalid characters

        if word:
            tokens.append(f"{'keyword' if word in keywords else 'identifier'}: {word}")

    return tokens

# variable declarations from the tokens
def variable_declaration(tokens, index):
    variables = []
    variables_set = set()  # To keep track of declared variables and look efficiently
    i = index + 1
    while i < len(tokens):
        token = tokens[i]
        if token == ";":  # End of declaration
            i += 1
            break
        if token.startswith("identifier: "):
            variable = token.split(": ")[1]
            if len(variables_set) >= 64:
                raise ValueError("Cannot declare more than 64 variables")  # Limit on variable declarations
            if not variable[0].isalpha() and variable[0] != "_":
                raise ValueError(f"Invalid identifier {variable}. Identifiers must start with a letter or underscore")
            if variable in variables_set:
                raise ValueError(f"Variable {variable} is already declared")  # No duplicates allowed
            variables.append(variable)
            variables_set.add(variable)
        else:
            raise ValueError(f"Expected identifier, but got {token}")  # expect an identifier here
        i += 1
    return variables, variables_set, i  # Return declared variables and the next index

# Parses expressions from the tokens starting after the declaration
def parsing(tokens, index):
    def parse_operand():
        nonlocal index
        if index >= len(tokens):
            raise ValueError("Unexpected end of tokens while parsing operand")  # Ensure we don't run out of tokens
        token = tokens[index]
        if token == '(':
            index += 1
            node = parse_or()  # start parsing or
            if index >= len(tokens) or tokens[index] != ')':
                raise ValueError("Expected ')' after expression")  # Check for matching parenthesis
            index += 1
            return node
        elif token.startswith("identifier: "):
            index += 1
            return Variable(token.split(": ")[1])  # Return a variable node
        elif token == "keyword: True":
            index += 1
            return Boolean(True)  #bool true
        elif token == "keyword: False":
            index += 1
            return Boolean(False)  #bool false
        elif token == "keyword: not":
            index += 1
            return Not(parse_operand())  # Parse operand for not
        else:
            raise ValueError(f"Expected operand, but got {token}")  # Handle unexpected tokens

    def parse_and():
        nonlocal index
        node = parse_operand()
        while index < len(tokens) and tokens[index] == "keyword: and":
            index += 1
            node = And(node, parse_operand())  # Create AND nodes 
        return node

    def parse_or():
        nonlocal index
        node = parse_and()
        while index < len(tokens) and tokens[index] == "keyword: or":
            index += 1
            node = Or(node, parse_and())  # Create OR nodes 
        return node
    
    #STARTS HERE then recursive calls
    return parse_or() 

#Assignments
def assignment(tokens):
    assignments = {}
    assigned_variables_set = set()  # Track assigned variables
    i = 0
    while i < len(tokens):
        token = tokens[i]
        if token == ";":  # Skip semicolon
            i += 1
            continue
        elif token.startswith("identifier: "):
            variable = token.split(": ")[1]
            if variable not in assigned_variables_set:
                assigned_variables_set.add(variable)  # Mark this variable as assigned
            i += 1
            if i >= len(tokens) or tokens[i] != "=":
                raise ValueError(f"Expected '=', got {tokens[i] if i < len(tokens) else 'EOF'}")
            i += 1
            start = i
            stack_count = 0
            while i < len(tokens):
                if tokens[i] == ";":
                    if stack_count == 0:  # Break on semicolon if we're at the top level
                        break
                if tokens[i] == "(":
                    stack_count += 1  # Count nested parentheses
                elif tokens[i] == ")":
                    stack_count -= 1
                i += 1
            end = i
            if i < len(tokens) and tokens[i] == ";":
                i += 1
            exp_token = tokens[start:end]
            exp_index = 0
            expression = parsing(exp_token, exp_index)  # Parse the expression
            assignments[variable] = expression  # Store the assignment
        elif token.startswith("keyword: show") or token.startswith("keyword: show_ones"):
            break  # Stop processing assignments when we reach show commands
        else:
            raise ValueError(f"Unexpected token {token} in assignments")  # Handle unexpected tokens
    return assignments

# Displays the truth table based on the assignments and variables
def show(assignments, variables, vars_to_show):
    vars_list = variables
    header = vars_list + vars_to_show
    print("# " + " ".join(header))  # Print the table header
    rows = 1 << len(vars_list)  # Total number of rows in the truth table (all possible combinations of 0 and 1)

    for val in range(rows):
        truth_table = {}
        for i, var in enumerate(vars_list):
            bit = (val >> (len(vars_list) - 1 - i)) & 1 #iterates through all possibilities of 0 and 1 for all vars
            truth_table[var] = bool(bit)  # Set truth values for variables

        # Evaluate expressions
        for var in assignments:
            truth_table[var] = assignments[var].evaluate(truth_table)

        # Prepare and print the row
        row = [str(int(truth_table[var])) if var in truth_table else '0' for var in header]
        print(" ".join(row))  # Display the current row

# Displays rows where at leas one var is true
def show_ones(assignments, variables, vars_to_show):
    vars_list = variables
    header = vars_list + vars_to_show
    print("# " + " ".join(header))  # Print the table header
    rows = 1 << len(vars_list)  # Total number of rows in the truth table

    for val in range(rows):
        truth_table = {}
        for i, var in enumerate(vars_list):
            bit = (val >> (len(vars_list) - 1 - i)) & 1
            truth_table[var] = bool(bit)  # Set truth values for variables

        # Evaluate expressions based on current assignments
        for var in assignments:
            truth_table[var] = assignments[var].evaluate(truth_table)

        # Check if any of the specified variables are True and print the row if so
        if any(truth_table.get(var, False) for var in vars_to_show):
            row = [str(int(truth_table[var])) if var in truth_table else '0' for var in header]
            print(" ".join(row))  # Display the current row

# Main function 
def truth_table(input_file) -> None:
    with open(input_file, 'r') as file:
        input_data = file.read()  # readfile

    tokens = tokenizer(input_data)  #tokens
    index = 0
    
    # Parse variable declarations and assignments
    variables, variables_set, index = variable_declaration(tokens, index)
    assignments = assignment(tokens[index:])  # Process assignments

    show_vars = []  # Variables to show in the truth table
    show_ones_vars = []  # Variables to show when at least one is True
    i = index
    while i < len(tokens):
        token = tokens[i]
        if token == "keyword: show":
            i += 1
            vars_to_show = []
            while i < len(tokens) and tokens[i] != ";":
                if tokens[i].startswith("identifier: "):
                    var = tokens[i].split(": ")[1]
                    vars_to_show.append(var)  # Collect variables to show
                i += 1
            i += 1  # Skip the semicolon
            show_vars.extend(vars_to_show)  # Add to show variables
        elif token == "keyword: show_ones":
            i += 1
            vars_to_show = []
            while i < len(tokens) and tokens[i] != ";":
                if tokens[i].startswith("identifier: "):
                    var = tokens[i].split(": ")[1]
                    vars_to_show.append(var)  # Collect variables to show when True
                i += 1
            i += 1  # Skip the semicolon
            show_ones_vars.extend(vars_to_show)  # Add to show ones variables
        else:
            i += 1

    # Display the results
    if show_vars:
        show(assignments, variables, show_vars)  # Show full truth table
    if show_ones_vars:
        show_ones(assignments, variables, show_ones_vars)  # Show only when at least one is True

# Entry point of the script
if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script_name.py input_file") 
        sys.exit(1)
    input_file = sys.argv[1]
    truth_table(input_file)  # truth table processing
