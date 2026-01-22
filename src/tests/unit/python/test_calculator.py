import pytest
from calculator import add, divide, classify_number

def test_add():
    assert add(2, 3) == 5

def test_divide():
    assert divide(10, 2) == 5

def test_divide_by_zero():
    with pytest.raises(ValueError):
        divide(10, 0)

def test_classify_number_positive():
    assert classify_number(4) == "even"
    assert classify_number(3) == "odd"

def test_classify_number_zero():
    assert classify_number(0) == "zero"