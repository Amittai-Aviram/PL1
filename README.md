# PL1
Demonstration programming language for a compilers course.

Welcome to PL1.  PL1 (for "programming language 1," i.e., a first programming language from the creator's point of view, is a simple, imperative-style programming language.  PL1 is in the C tradition, but offers the following somewhat distinctive features:
* A syntactic distinction between _functions_, which return values, and _procedures_, which take _actions_.  Currently, PL1 does not prevent functions fromhaving side effects, but restrictions are planned to prevent functionsn from writing directly to global variables.
* A syntax that more clearly distinguishes _assignment_, using an arrow symbol, from _equality testing_, which uses mostly the same symbols as C.  (The _not equal_ symbol is **~=** .  The logical operators are the English words **and**, **or**, and **not**.)
* Multidimensional arrays are automatically allocated on the heap and deleted when they go out of scope (not currently implemented).

This programming language is very much a work in progress, and implementation has only begun.  So far, compilation targets an intermediate representation designed to approximate x86_64 Assembly.  The plan is to generate x86_64 Assembly directly, without an intermediate phase.

Amittai Aviram
aviram@bc.edu
