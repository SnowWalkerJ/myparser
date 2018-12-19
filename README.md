# myparser

This is a practice project trying to mimic Python using lex and yacc.

## Currently supported grammars

- [x] Assignment
	- e.g. `a = 1;`
- [x] Arithmetic
	- e.g. `a = (a + 1) * 2;`
- [x] Print 
	- e.g. `print a;`, `print 2 + 3;`. Currently it's a syntax like as in Python 2. Intend to implement it as function once function calls are implemented.
- [x] Function definition
	- e.g. `function double(x) {return x * 2;}`
- [x] Function call
	- e.g. `y = double(x);`

## Plans

- [ ] Comments
- [ ] Flow control (this can be a little tricky)
- [ ] Multiple data types (currently only int is supported)

Object-oriented programming is not in plan because I have no idea how to implement it.


## Example script

### Source

```
x = 3;

function double(m) {
    return m * 2;
}

y = double(x) + 10;
print y;
```

### Output

16