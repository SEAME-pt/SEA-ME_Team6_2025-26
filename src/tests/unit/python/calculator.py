def add(a, b):
    """Return the sum of a and b."""
    return a + b

def divide(a, b):
    """Divide a by b, raising ValueError on division by zero."""
    if b == 0:
        raise ValueError("Division by zero.")
    return a / b

def classify_number(n):
    """Classify an integer as 'negative', 'zero', 'even' or 'odd'."""
    if n < 0:
        return "negative"
    elif n == 0:
        return "zero"
    elif n % 2 == 0:
        return "even"
    else:
        return "odd"