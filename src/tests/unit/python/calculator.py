def add(a, b):
    return a + b;

def divide(a, b):
    if b == 0:
        raise ValueError("Division by zero.")
    return a / b

def classify_number(n):
    if n < 0:
        return "negative"
    elif n == 0:
        return "zero"
    elif n % 2 == 0:
        return "even"
    else:
        return "odd"