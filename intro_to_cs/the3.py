####################################################################
### 1. Have the postfix notation of the given string with respect to 
### precedences that belong to message sender's planet.
### 2. Use that output to get normal infix notation with parentheses back.
### 3. Change "/" to "*" and vice versa.
###
### 4. Used algorithms
### http://en.wikipedia.org/wiki/Shunting-yard_algorithm#The_algorithm_in_detail
### http://scanftree.com/Data_Structure/postfix-to-infix
####################################################################

### a cool way to generate an alphabet without importing any modules.
"""alphabet = []
for i in range(97,123):
    alphabet.append(chr(i))"""

###global tables###
asso = {"^":"RIGHT", "*":"LEFT", "/":"LEFT", "+":"LEFT", "-":"LEFT"} #common in both planets
alphabet = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'] #common in both planets

###global helper functions###
def isletter(what): 
    return True if what in alphabet else False
def isleft(what): 
    return True if asso[what] == "LEFT" else False
def isleftparenthesis(what): 
    return True if what == "(" else False
def isdivision(what):
    return True if what == "/" else False
def ismultiplication(what):
    return True if what == "*" else False

###reverse polish notation
def infix2postfix(STRING, operators):
    ###local helper functions###
    def isoperator(what):
        return True if what in operators else False
    def precedence(what):
        return operators[what]
    ###algorithm
    output = []
    stack = []
    STRING = STRING.replace(" ", "") # this deals with white spaces
    for token in STRING:
        if isletter(token): #letters a b c d
            output.append(token)
        elif isoperator(token): #operators + - * /
            if not stack: #when the stack is empty and we cannot do any comparison between the operators.
                stack.append(token)
                continue #append the item and then continue with the next token.
            while isoperator(stack[-1]) and ((precedence(token) <= precedence(stack[-1]) and isleft(token)) or (precedence(token) < precedence(stack[-1]) and not isleft(token))):
                x = stack.pop() #if there is an operator at the top of the stack, pop it and have its way to output while the conditions are met.
                output.append(x)
                if not stack: #when there is no more to pop and append, break this while loop.
                    break
            stack.append(token) #we are done with the current token. it can be appended to the stack now.
        elif isleftparenthesis(token): #left parenthesis
            stack.append(token)
        else: #right parenthesis
            while not isleftparenthesis(stack[-1]): #from the stack to the output until we hit the left parenthesis.
                x = stack.pop()
                output.append(x)
            else: #when the left parenthesis is found, just get rid of it.
                stack.pop()
    while stack: #there can be items left in the stack after having read all the tokens.
        x = stack.pop()
        output.append(x)
    return output

###infix notation
def postfix2infix(LIST):
    stack = []
    for token in LIST:
        if isletter(token): #when there is a letter in the postfix notation, append it to stack.
            stack.append(token)
        else: #when there is an operator
            x = stack.pop() #call the operand
            y = stack.pop() #call the other operand
            z = "(" + y + token + x + ")" #encapsulate them with parentheses
            stack.append(z) #send back to stack
    return stack[0]

###from us to them
def ToZoitankian(EXPRESSION):
    result = list(postfix2infix(infix2postfix(EXPRESSION, {"^":2, "*":1, "/":1, "+":0, "-":0}))) #call the postfix function with our precedence
    for i in range(len(result)): #change the different looking operators. (*,/)
        if isdivision(result[i]):
            result[i] = "*"
        elif ismultiplication(result[i]):
            result[i] = "/"
    return "".join(result)

###from them to us
def ToEarthian(EXPRESSION):
    result = list(postfix2infix(infix2postfix(EXPRESSION, {"^":0, "*":1, "/":3, "+":4, "-":2}))) #with their precedence
    for i in range(len(result)): #change the different looking operators.(*,/)
        if isdivision(result[i]):
            result[i] = "*"
        elif ismultiplication(result[i]):
            result[i] = "/"
    return "".join(result)