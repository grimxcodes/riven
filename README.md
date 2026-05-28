Riven Programming Language

Riven is a modern systems programming language focused on:

- speed
- simplicity
- readability
- operating systems
- low-level development
- beginner-friendly syntax

Riven combines:

- C-level power
- modern safety
- clean readable syntax

while remaining easy to learn.

---

Current Status

Riven v1 is currently under development.

Core components:

- lexer
- parser
- runtime
- interpreter

are being built directly in C.

---

File Extensions

Type| Extension
Source File| ".rv"
Header File| ".rvh"

---

Hello World

riven core {

    stamp("Hello Riven")

}

---

Features

- readable syntax
- frames (object system)
- async spark tasks
- safe references
- safe pointers
- package manager
- modern memory system
- built-in fetch support

---

Variables

name = "Ansh"

age = 18

---

Constants

firm version = 1.0

---

Conditions

if age > 18 {

    stamp("Adult")

}

altif age == 18 {

    stamp("Exactly 18")

}

else {

    stamp("Minor")

}

---

Loops

flow 5 {

    stamp("Hello")

}

---

Functions

craft add(a,b){

    returns a+b

}

---

Frames

frame User {

    hidden password = 1234

    open login(){

        stamp("Access Granted")

    }

}

---

Async Tasks

spark craft background(){

    stamp("Running")

}

---

Safe References

ref data = user

---

Safe Pointers

ptr memory = kernel

---

Package Installation

rvn install graphics

---

Build

make

---

Run

./rvn examples/hello.rv

---

Vision

Riven aims to become:

- a powerful systems language
- a beginner-friendly low-level language
- a modern alternative for OS and kernel development

---

License

Currently private development.
