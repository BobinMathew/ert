import json

def polynomial(a, b, c):
    x_range = tuple(range(10))
    result =  tuple(a * x**2 + b * x + c for x in x_range)
    return result
