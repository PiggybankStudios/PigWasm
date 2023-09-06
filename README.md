# What is Pig WASM
This is a small layer of code that I use to make web assembly applications in C. I have made it public and open source (under the 2-clause BSD license) to allow me to collaborate with other developers on web assembly projects.

Pig WASM currently contains the following features:
1. Minimal OpenGL javascript function implementations (with slight modifications to the standard functions on the C side to match WebGL's javascript API)
2. Memory Arena implementations for managing memory in various ways. The MemArenaType_StdHeap handles growing our page count through a Javascript call and asserts on free (since you can't free pages in web assmebly)
3. Debug output functions that are colored nicely in the browser javascript console based on the level of output.
4. A very basic index.html + main.css that provide a canvas to draw on. A set of glue javascript files that get combined together into a single glue_combined.js and serve as the backbone of the control flow and input information gathering
5. An example batch file that uses LLVM's clang to compile the C code to web assembly. It uses a unity build pattern which avoids the need for a complicated build system like make.
6. Some simple structures that encapsulate various OpenGL primitives (shaders, textures, etc.) and provide some functions for creating and managing these objects.
7. A set of standard library replacement files that provide minimal functionality and naive implementations for various standard library features (like memcpy)
8. A simple python script for comining the javascript files into a single file

Some of the files can be included directly into your project, while others (like debug output) are somewhat interdependant and require at least a few files to be added to be functional. This library is not a standalone compilable unit and it's up to the user to build out a main.cpp and other supporting structure that utilizes this library.

I personally use a unity style build system, but the cpp files in the repository should be compilable as separate units if you want to use a more complicated build system.

Debug output relies on an implementation of printf to be available (for example https://github.com/mpaland/printf). The common.h template assumes this repository has been cloned into a small_printf folder.

# Why is it named Pig WASM?
I am terrible at naming things. I have a private repository called Pig Engine (derived from Piggybank Studios LLC). This repository acts as a set of features that I have pulled out and reworked for use in the web assembly environment and under an open source license. Pig is a 3 letter word and therefore works great as a prefix. Rather than come up with a new name for this library compared to the Pig Engine repository, I'm just gonna call this Pig WASM.

# License (Free BSD aka 2-clause BSD)
Copyright 2022 Piggybank Studios LLC

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.