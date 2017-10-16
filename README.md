# FlatB Compiler #

### Running the code ###
Build the compiler in the `src/` directory using the following commands
```
cd src/
make
./bcc <test file>
``` 
**bcc** is the compiler executable file. Use it to compile any code written in flatB language.
### Description ###

The language consists of 2 blocks, _declaration block_ and _codeblock_ . The declaration block consists of all the variable declarations and it's mandatory that all variables have to be declared in the declaration block before using them in the codeblock. The codeblock everything else other than variable declarations.

```
declblock {
// All variable declarations go here
}
codeblock{
// Main logic of the program ( shouldnot include variable declarations)
}

```
### Syntax ###
- Variable Declarations
```
int data, array[100];
int sum;
```
- For Loop
```
for i = 1, 100 {
	.....
}

for i = 1, 100, 2 {
	.....      // Value of i varies from 1 to 100 with step size 2.
}

for i = expr1, expr2, expr3 {
	.....      // Value of i varies from expr1 to expr3 with step size expr3
}
```
- if-else statement
```
if expression {
      ....
}

if expression {
	...
}
else {
       ....
}
```
- while statement
```
while expression {

}
```
- conditional and unconditional goto
```
got label
goto label if expression
```
- IO Statements

```
print "blah...blah", val
println "new line at the end"
read sum
read data[i]
```


