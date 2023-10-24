# GRAF

Experimental 3D gra(f)phics engine made in c using
utility libraries and sokol.  

This project does not aim to be a "pure" c project with minimal
dependencies. It is simply an attempt at making a 3D game/graphics
engine using c while trying to take advantage of the language's
freedoms over memory and data management.  

The goal in the long term is to have a flexible enough
framework that it could be used for making games and
simulations, while having a system that could
have the renderer completely removed to make
game servers for instance.

# Current state
Do not expect many updates as I am very busy with university.
Despite this, you are welcome to contribute by giving suggestions,
making PRs, opening issues or chatting about the project.
(I am also available at sawcce@proton.me and @sawcce on discord)
I am currently trying to build a robust enough ECS system
that can handle basic camera and input interaction.

# How to use
- Clone the repo
- Use the `bootstrap` make target using `make bootstrap` in the command line (this will clone and build the necessary files)
- Use the `build` and `run` targets to build and run the project (the run target only works on windows now but should be fixed soon, in the meanwhile the executable can be run manually) while specifying the `CC` parameter like so: `make build run CC=<your C compiler>` you can use `clang` for instance. 

# Thanks for reading!
Code distributed under the MIT license.  
\- Sawcce