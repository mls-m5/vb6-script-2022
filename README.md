# vb6-script-2022

Another project to enable to run visual basic projects.

Saving execution time on reduced cache misses and extra indirections is a non
goal. This program should just run my 2000 era visual basic programs.


This time i will start from the other side of it compared to how I usually
develop script engines. I use to start with the parser and work my way from
there. This time i want to make the core engine work first, then when (if?) 
that gets done i will do yet another parser.


# Design

## Stack
There is not really a stack. Every time there is a function call, a new
object containing all the functions local variables is created (on the stack)
and the other stack is ignored. Each local context contains a pointer to the
global context.

Each function has a array of values that must by accessed by their index when
called.

The context contains a separate list with arguments that the function is called
with. This is because in vb6 you can call functions "ByRef" and "ByVal" but
since there is not a concept of pointers (except that all objects is pointers)
you should not be able to use pointers/references. Except for funcitonal
arguments.

A bit wierd, but that is how "easier" languages tends to hide complexity from
the user.


## Lexer

## Parsing


## Values

Values does not change type. In the original VB6, when not assigning a type
to a variable it was "Variant" as default. Since I like strict types I will
implement no such thing.

## Licence

GNU

