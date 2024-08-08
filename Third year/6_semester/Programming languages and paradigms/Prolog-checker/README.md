## Simple Program Analyzer - Introduction
Analyze simple sequential programs represented as a Prolog list of instructions. Multiple processes with the same code can run concurrently. The task is to write a Prolog program to check the safety condition, determining if there exists an interleaving of process executions where at least two processes enter the critical section simultaneously.

## Specification

### Constants
Integers.

### Variables
Simple integers and arrays of integers. Arrays are one-dimensional with a size equal to the number of processes. Arrays are indexed from 0, and all variables (including array elements) are initialized to zero. All variables are global, accessible to all processes.

Each process has access to a constant `pid`, representing the process identifier (0 to N-1, where N is the total number of processes).

### Expressions (Arithmetic and Logical)
- `arithExpr ::= simpleExpr | simpleExpr oper simpleExpr`
- `simpleExpr ::= number | variable`
- `variable ::= ident | array(ident, arithExpr)`
- `oper ::= + | - | * | /`
- `logicalExpr ::= simpleExpr relOper simpleExpr`
- `relOper ::= < | = | <>`

Variable names (simple and array) are Prolog constants (not numbers), e.g., `x`, `k`, `chce`, `’A’`.

### Instructions
- `assign(variable, arithExpr)`: Assign the value of the expression to the variable (simple or array element). Move to the next instruction.
- `goto(number)`: Unconditional jump to the instruction with the given index.
- `condGoto(logicalExpr, number)`: Conditional jump to the instruction with the given index if the logical expression is true; otherwise, move to the next instruction.
- `section`: Critical section. Move to the next instruction.

Instructions are indexed from 1. Each process starts execution from instruction 1.

### Assumptions
- Variables have values within a limited range (e.g., 0..N, where N is the number of processes). The number of system states is finite.
- Program execution correctness is assumed (e.g., valid array references, arithmetic expressions, jumps).
- Each process operates in an infinite loop (last instruction is an unconditional jump).

## Task
Write a Prolog procedure `verify/2` as follows:
```prolog
verify(N, Program),
```
where:
- `N` - number of processes running in the system (>= 1)
- `Program` - name of the file containing the program (constant)

The procedure should validate call arguments (e.g., number of processes, file availability). The input data (program file) is assumed correct.

For valid data, the program should output:
- Whether the safety condition is met or not
- If the safety condition is not met, an example of an invalid interleaving and the indices of processes in the critical section.

### Program File Format
A text file in the following format:
```
variables(ListOfSimpleVariableNames).
arrays(ListOfArrayVariableNames).
program(ListOfInstructions).
```
All lists are provided in Prolog notation.

### Example Programs

1. Implementation of Peterson's algorithm in the defined language (left side, in parentheses, instruction indices):
```
(1) assign(array(chce, pid), 1)
(2) assign(k, pid)
(3) condGoto(array(chce, 1-pid) = 0, 5)
(4) condGoto(k = pid, 3)
(5) section
(6) assign(array(chce, pid), 0)
(7) goto(1)
```
Representation of the above program (file `peterson.txt`):
```
variables([k]).
arrays([chce]).
program([assign(array(chce, pid), 1),
assign(k, pid),
condGoto(array(chce, 1-pid) = 0, 5),
condGoto(k = pid, 3),
section,
assign(array(chce, pid), 0),
goto(1)]).
```

2. Very simple unsafe program (`unsafe.txt`).
```
variables([x]).
arrays([]).
program([assign(x, pid), section, goto(1)]).
```