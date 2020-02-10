# What is fuLisp

fuLisp is a minimalistic Lisp interpreter, with its core being written in C.

## Status

fuLisp is usable - it provides a turing-complete Lisp dialect with all the
features Lisp defines:

- First-class functions
- Closures
- Garbage Collection (Mark and Sweep is the default, but can be switched to
  reference counting on compile time)
- A REPL loop
- Proper evaluation of files

## Aims

fuLisp is a simplistic, minimal Lisp interpreter.
This means I implement only the absolutely necessary stuff -
CONS cells suffice to implement a dictionary, thus I am not going to write
a performant one in C (although the compiler itself uses hash tables, which
could be exposed to Lisp - but I won't for now).

Everything that can be implemented in Lisp itself will be - currently there
is a sample `lib/std.lisp` which implements e.g. the well-known `map` function.

Currently, only Linux as operating system is supported.
However, it is likely that it will build and run fine on other
UNIX and POSIX standard compliant operating systems like OpenBSD.

# Building

For building, you will require `make` and a decent C compiler (C90 support
should suffice).

fuLisp at its core consists of a single executable.

Once you obtained the source, change directories into the fulisp directory.

Type

```
make
```

- That's it.

A new directory `build` should have appeared. In it, next to some object files,
an executable `fulisp` should be present.

This is fulisp - type `build/fulisp` to start the REPL.

Configuration of the build is done via SHELL variables (for general stuff like
the compiler to use) or by editing `src/config.h` for more sophisticated
configuration like deciding which Garbage Collection method to use.

For an example which SHELL variables are supported, look at `config/debug.sh` .

For enabling building a debug release, just do a

```
source config/debug.sh
make
```

in the root directory of fuLisp.


# Usage

Currently, fuLisp contains of a REPL only.
Once built, in order to start it, go to the root directory of fuLisp and enter

```
build/fulisp
```

Everything you want to execute, you have to type in there.
At least, nearly.
You can load in code from files by using the `import` statement:

```
(import "lib/std.lisp")
```

will load the standard library (the Lisp part).

`lib/std.lisp` is an ecxellent starter to figure out the syntax of fuLisp.
The language is still changing, thus there is no authoritative language
description yet.

However, if you are familiar with Common Lisp, Scheme or any other Lisp variant,
you should not have a hard time.

Bear in mind that currently the language core is being developed, and thus the
standard library (C and Lisp part) is rather rudimentary.

Have a look into `src/environment.c`, especially the function
`environmentCreateStdEnv`

Functions try to follow the lisp approach to take an arbitrary number of
arguments if possible, thus you are able to write

```
(+ 1 2 3)
```

to add up 1, 2 and 3.

There is no dedicated statement for defining a named function, instead
declare a variable and assign it a lambda:

```
(define add2 (lambda (x) (+ 2 x)))
```

Closures are supported:

```
(define make-adder (lambda (x) (lambda (y) (+ x y))))

(define add2 (make-adder 2))
(add2 14)
```

There are no dedicated loop mechanisms - use recursion instead:

```
; Classical map
; Use like
;
; (define add2 (lambda (x) (+ a 2)))
;
; (map add2 (quote (1 2 3 4)))
;
(define map
  (lambda (fun x)
    (cond ((cons? x) (cons (fun (car x)) (map fun (cdr x))))
          ((nil? x) nil)
          (T (fun x))
          )
    )
  )
```


# Licensing

fuLisp is free software and might be distributed under the terms of the
BSD 3-Clause-License.

Have a look at the file `Copying` for details.
