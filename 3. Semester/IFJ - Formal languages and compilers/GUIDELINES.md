# IFJ Teamproject - Guidelines & Coding style

This file describes required and only accepted coding style used in every source C file in this project.

Coding style was adapted from CESNET's NEMEA project. For the need of IFJ project, style was slighty adjusted to reduce extensive byrocratic overhead. Few concepts were also changed to better reflect personal preferences of project's programmers.

## SYSTEM & TOOLS SETUP

Recommended OS for the project is Red hat based Linux distribution. Project members shall have access to Fedora 25 virtual image on demand.

Required compiler is GCC with version greater than GCC at merlin.fit.vutbr.cz, current version is gcc (GCC) 5.4.0 (12.06.2017).

Recommended IDE is CodeBlocks version 16.01 or greater. However you may also use your favourite editor and compile using a makefile.

## BASICS

Only allowed language for identifiers, comments and everything else contained in the source files is English.

Line size limit is strictly limited to 80 characters to accumulate default width of terminal windows. If you need more characters, you should rethink your code. If you really need more characters, use newline and several tabs to make your code more readable. For example, see picture below:

```C
void * pcap_creator_init(const char *path, const char *module,
			 unsigned channel_id, size_t sampling,
			 size_t count, int *err_code)
{
	...
}
```

Use 3 SPACES for indentation. Do not use TABs.

Do not use magic numbers. If you need to use literal, define it as a preprocessor directive.

```C
#define ERROR_EXIT 42
```

Comment your code. There should be atleast 1 line of code per approx. 10 lines of code. Comment what's happening, describe meanings of if-else statements, describe variables initialization with certain values etc.

Use comments and blank lines to divide long portions of code into logical groups of statements.

Avoid using GCC or Clang compiler-specific extensions, respect C99 & C11 standards.

## GIT

Every member of the project is required to have github account.

Changes to git repository should be commited as often as possible. Theoretically, change of a single line in previously completed file should be commited and properly documented.

Don't commit new files as you are still developing them too often. Commit of a file under the development should be made only at the end of the day/work to keep other members informed about the project progress.

Exception: Every module that you are not going to work on for next couple of hours should be commited.

## NAMING

Use names that explain the purpose of a function or object.

Use underscores to separate words in an identifier: multi_word_name.

Use lowercase for most names. Use uppercase for macros, preprocessor directives and members of enumerations.

Give arrays names that are plural.

## COMMENTS

One-line comments are written using "//". These types of comments should be written with beginning capital letter, but does not necessarily need to end with period. Always put one-line comments line above the commented entity. Do not put comments on the same line as code.

Multi-line comments have to be written as full sentences that start with a capital letter and end with a period. Put two spaces between sentences. Usage of this type of comments may be in Doxygen to describe a function, more complex code or specific programmer's thoughts used to solve non-standard problems.

Write block comments as shown below.

```C
/*
 * We redirect stderr to /dev/null because we often want to remove all
 * traffic control configuration on a port so its in a known state.  If
 * this done when there is no such configuration, tc complains, so we just
 * always ignore it.
 */
```
 
Each source file must contain a Doxygen comment explaining the purpose of the code and stating the date of creation, modifications and authors.

Required way of using doxygen tags is "@" instead of "\\" - "@author" instead of "\author"
 
Each function and each variable declared outside a function, also each macro, struct, union, and typedef declaration should be preceded by a Doxygen comment.

Each struct and union member must have a comment that explains its meaning. Structs and unions with many members should be additionally divided into logical groups of members by block comments, e.g.:

```C
/**
 * @struct poll_waiter
 * @brief An event that will wake the following call to poll_block().
 * @var poll_waiter::node Node of list of file descriptors.
 * @var poll_waiter::fd File descriptor.
 * @var poll_waiter::events Events to wait for (POLLIN, POLLOUT).
 * ...
 */
struct poll_waiter {
   struct list node;
   int fd;
   short int events;
   poll_fd_func *function;
   void *aux;
   struct backtrace *backtrace;
   // Pointer to element of the pollfds array (null if added from a callback)
   struct pollfd *pollfd;
};
```

Use XXX, FIXME or TODO comments to mark code that needs work.

Don't comment out or #if 0 out code. Just remove it. The code that was there will still be in version control history.

## FUNCTIONS

Put the return type and function name on the same line, all starting in column 0. Braces surrounding the function code are starting on the &next line.

Before each function definition (or declaration in header file with API), write a Doxygen comment that describes the function's purpose, including each parameter, the return value and side effects. The comment does not need to describe how a function does its work, unless this information is needed to use the function correctly (this is often better done with comments inside the function). On the other hand, comment must describe how the function can be used and what are its results.

Write functions declarations and definitions to separate files. The file containing declarations is saved as header file .h, while source file with definition file is saved as .c. 

For function declaration without parameters, do not write empty parentheses, e.g. "int foo();", write "void" in parentheses instead, e.g. "int foo(void);". When calling such function, do not pass void as a parameter. Call function like if(func() != 1), not if(func(void) != 1).

In the absence of good reasons for another order, the following parameter order is preferred. One notable exception is that data parameters and their corresponding size parameters should be paired.

The primary object being manipulated, if any (equivalent to the "this" pointer in C++).
Input-only parameters.
Input/output parameters.
Output-only parameters.
Status parameter.

Example:

```C
/**
 * Get features of the network device interface.
 *
 * @param[in] netdev pointer to the network device.
 * @param[out] *current current features of the device.
 * @param[out] *advertised advertised features of the device.
 * @param[out] *supported supported features of the device.
 * @param[out] *peer peer features of the device.
 * @return 0 on success, otherwise a positive errno value.
 */
int netdev_get_features(struct netdev *netdev, uint32_t *current,
			uint32_t *advertised, uint32_t *supported,
                        uint32_t *peer)
```

Functions that destroy an instance of a dynamically-allocated type should accept and ignore a null pointer argument.

Functions in .c files should not normally be marked "inline", because it does not usually help code generation and it does suppress compilers warnings about unused functions. Functions defined in .h usually should be marked inline.

## FUNCTION PROTOTYPES

Put the return type and function name on the same line in a function prototype:

static const struct option_class *get_option_class(int code);

## STATEMENTS

Indent each level of code with TAB spaces. Use Allman brace placement:

```C
if(func())
{
	b++;
}
```

Single statements are not required to be enclosed in brackets on the highest level, however avoid multi-level statements without brackets. Example:

How not do to it:
```C
if(a > b)
	if(c > d)
		while(d == c)
			d++;
```

How to do it:
```C
if(a > b)
{
	if(c > d)
	{
		while(d == c)
			d++;
	}
}
```

Although, bracketting every single statement is also accepted:
```C
if(a > b)
{
	if(c > d)
	{
		while(d == c)
		{
			d++;
		}
	}
}
```

Try to avoid assignments inside "if" and "while" conditions.

If it helps to clarify meaning of "if" and "else" branches (what can reader expect in the branch), write comment above the branch's block.

```C
//Check if the allocation was successful
if(value == NULL)
{
	commands;
}
```

Write only one statement per line.

Indent "switch" statements like this:

```C
switch (conn->state)
{
case S_RECV:
   error = run_connection_input(conn);
   break;

case S_PROCESS:
   error = 0;
   break;

case S_SEND:
   error = run_connection_output(conn);
   break;

default:
   OVS_NOT_REACHED();
}
```

Preferred way to write infinite loop is "while(1)".

In an if/else construct where one branch is the "normal" or "common" case and the other branch is the "uncommon" or "error" case, put the common case after the "if", not the "else". This is a form of documentation. It also places the most important code in sequential order without forcing the reader to visually skip past less important details. (Some compilers also assume that the "if" branch is the more common case, so this can be a real form of optimization as well.)

## MACROS

Don't define a function-like macro if a "static inline" function can be used instead.

If a macro's definition contains multiple statements, enclose them with "do { ... } while (0)" to allow them to work properly in all syntactic circumstances.

## FILE STRUCTURE

Each source file must contain a Doxygen comment explaining the purpose of the code and stating the date of creation or modifications and authors.

The first non-comment lines in a .c source file should be "include" statements in the following order:

The module's own headers, if any.

Standard C library headers and other system headers, preferably in alphabetical order.

## HEADER FILES

Each header file should start with Doxygen comment, followed by a "header guard" to make the header file idempotent, like so:

```C
#ifndef SYNTAX_CHECK_H
#define SYNTAX_CHECK_H 1

...

#endif /* syntax_checker.h */
```

Header files should be self-contained; that is, they should #include whatever additional headers are required, without requiring the client to \#include them for it.

Don't define the members of a struct or union in a header file, unless client code is actually intended to access them directly or if the definition is otherwise actually needed (e.g. inline functions defined in the header need them).

Similarly, don't #include a header file just for the declaration of a struct or union tag (e.g. just for "struct name;"). Just declare the tag yourself. This reduces the number of header file dependencies.

## TYPES

Use typedefs to make complex data types more readable or as a form of data types encapsulation/documentation.

Don't assume that "long" is big enough to hold a pointer. If you need to cast a pointer to an integer, use "intptr_t" or "uintptr_t" from <stdint.h>.

Use the int_t and uint_t types from <stdint.h> for exact-width integer types. Use the PRId, PRIu, and PRIx macros from <inttypes.h> for formatting them with printf() and related functions.

Use bit-fields sparingly. Do not use bit-fields where the exact format is important.

Declare bit-fields to be signed or unsigned integer types or _Bool (aka bool). Do not declare bit-fields of type "int": C99 allows these to be either signed or unsigned according to the compiler.

Pointer declarators bind to the variable name, not the type name. Write "int \*x", not "int\* x" and definitely not "int \* x".

## EXPRESSIONS

Put one space on each side of infix binary and ternary operators.  

Do not put any white space around postfix, prefix, or grouping operators:

```C
() [] -> .
! ~ ++ -- + - * &
```

Exception: Put a space between the () used in a cast: (void *) my_favourite_var.

Break long lines before the ternary operators ? and :, rather than after them, e.g.:

```C
return (out_port != VIGP_CONTROL_PATH
        ? alpheus_output_port(dp, skb, out_port)
        : alpheus_output_control(dp, skb, fwd_save_skb(skb),
                                 VIGR_ACTION));
```
								 
Do not parenthesize the operands of && and || unless operator precedence makes it necessary, or unless the operands are themselves expressions that use && and ||. Thus:

```C
if(!isdigit((unsigned char) s[0])
    || !isdigit((unsigned char) s[1])
    || !isdigit((unsigned char) s[2])) {
    printf("string %s does not start with 3-digit code\n", s);
}
```

but
```C
if(rule && (!best || rule->priority > best->priority))
{
   best = rule;
}
```

Try to avoid casts.

When using a relational operator like "<" or "==", put an expression or variable argument on the left and a constant argument on the right, e.g. "x == 0", not "0 == x".
