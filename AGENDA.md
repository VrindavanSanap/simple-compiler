# Scalable, Compiled, Unambiguous Low-Level Language (Scull)

Please Follow [this](https://www.conventionalcommits.org/en/v1.0.0/) as a commit-message writing guide.

**Legend:**

- NEC - Necessary
- EXP - Experimental
- ? - Unsure and needs research

---

## Phase 1: Frontend and Core Compiler Infrastructure

### Build System, Tooling, Infrastructure & UX

- [x] NEC Add option to parse and use -o before/after -c for specifying where the object file should be stored
- [x] NEC Seperate frontend (lexer, parser, semantic), core (data structures, common.h), and sclc (cstate, fstate, backend)

### Error Handling & Debugging

- [ ] NEC Better errors
  - [ ] Fix existing bugs related to errors
  - [ ] Errors with source preview

### Testing

- [ ] NEC Test cases for each logical module

---

## Phase 2: Backend & Code Generation

### Frontend

- [ ] NEC Source Location field in AST

- [ ] NEC Parser refactor
  - reduce pre-defined parsables into instructions, rather than the existing
  - will follow a similar top-down approach but would be better to add features to and read

- [ ] NEC pratt parsing for expressions

- [ ] NEC Write a EBNF Grammar specification or similar for scull

### Backend

- [ ] NEC Develop C Backend
  - Language changes for 1:1 C translation needed
  - Basic types, structs, unions, enums, memory management, etc.

### Distribution

- [ ] EXP Bootstrapping Strategy (inspired by Zig):
  - Generated C code (one file) of compiler source ships with release
  - User compiles bootstrap binary with system C compiler
  - User uses bootstrap binary to compile source
  - Native scull binary available

---

## Phase 3: Core Language Features

### Type System, Variables, etc

- [x] NEC Unsigned ints (`u32`), signed ints (`i32`)
- [x] NEC int literals

- [ ] NEC floats (f16, f32, f64, f80, f128)
- [ ] NEC float literals

- [x] NEC Boolean type (`bool`)

- [ ] NEC Typedefs
- [ ] NEC Better strings
- [ ] NEC Multi-dimensional arrays
- [ ] ? Type casting
- [ ] ? Type inference

- [ ] NEC Better way to declare variables

```
let modifiers name : type = literal -- rust style

modifiers name : type = literal -- zig style

modifiers type name = literal -- c style
```

### Control Flow

- [ ] NEC For loop
  - [x] Basic implementation
  - [ ] Typing for iterator
  - [ ] looping through arrays
- [ ] EXP For-each loop

- [ ] NEC Ternary operator-like functionality using if-elseif-else in assignment

- [ ] NEC Seperate scopes for multi-instr conditional blocks (if-else, match, ...)

### Operators

- [x] NEC Logical operators (`&&`, `||`, `!`)
- [ ] NEC Bitwise operators (`&`, `|`, `^`, `~`, `<<`, `>>`)
- [ ] NEC Assignment operators (`+=`, `-=`, `*=`, `/=`, etc.)

### More Advanced Features

- [ ] NEC Alternative for `sizeof()` and `typeof()`

- [ ] ? Memory management
  - explicit allocators
  - defer
  - RAII for allocators

---

## Phase 4: Advanced Language Features

### User-Defined Types

- [ ] NEC Structs
- [ ] NEC Unions
- [ ] NEC Enums
- [ ] NEC Implementation blocks for user-defined data types
- [ ] EXP Traits / Interfaces

### Functions

- [ ] EXP Anonymous functions

### Metaprogramming

- [ ] NEC Preprocessor macros
- [ ] EXP Compile-time functions (evaluated at compile time)
- [ ] EXP Inline functions (copy and paste like C)

### Standard Library

- [ ] NEC Refresh `io.scl` using functions
- [ ] NEC Standard Library
  - [ ] Dynamic memory management
  - [ ] String manipulation
  - [ ] File I/O
  - [ ] Collections

---

## Phase 5: Experimental Features

### Advanced Type System

- [ ] NEC Owned types

```scl
  int own *T;
```

- [ ] EXP Nullable types

```scl
  int definately = 9
  int? maybe = null

  int? *definately_ptr = &definately
  int? *maybe_ptr = null
```

- [ ] EXP Optional chaining

```scl
  printf("%s\n", object?.name)
  // Prints null if object.name doesn't exist
```

- [ ] EXP Modular/Constraints in type system (runtime overhead)

```scl
  int mod(5) counter = 4
```

- [ ] EXP Arbitrary width for types

```scl
  int:7 smol = 2
```

- [ ] EXP Distinct typedefs (cannot be cast to other typedefs wrapping the same types)

- [ ] EXP Packed structs without padding (runtime overhead)

- [ ] EXP Sum types (like rust enums)

### Error Handling

- [ ] EXP Result enum similar to Rust

```scl
  fn divide(int a, int b) : Result<int, Err> {
    if b == 0 then return Err.DivisionByZero
    return a / b
  }
```

---

## Abstract Roadmap

### v0.0 (MVP / Diploma Final Year Project Ready)

- Basic compiler infrastructure
- Core type system (int, char, bool, void)
- All control flow
- All operators
- Arrays
- Functions

### v0.1

- C backend + Initiate bootstrap
- Structs, unions, enums
- Implementation blocks
- Standard library basics (dependency on libc can still exist)

### v0.2

- Partial Bootstrap
- Full standard library
- Traits/interfaces

### v1.0+

- Full bootstrap
- Based on user feedback
- Choose 2-3 experimental features to stabilize
