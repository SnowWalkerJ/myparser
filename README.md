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
- [x] Flow control
	- e.g. `while (x > 0) {print x; x = x - 1;}`
	- e.g. `if (x > 0) {return 1;} else {return 0;}`
- [x] Comments
	- e.g. `// This is single-line comment`
	- e.g. `/* This is multi-line comment/*`

## Plans


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