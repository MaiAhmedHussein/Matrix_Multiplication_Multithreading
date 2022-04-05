[![Typing SVG](https://readme-typing-svg.herokuapp.com?font=comfortaa&color=016EEA&size=24&width=500&lines=Matrix+Multiplication+Multi-threading...)](https://git.io/typing-svg)


## A Multi-threaded [matrix multiplication](https://www.mathsisfun.com/algebra/matrix-multiplying.html) program.

The input to the program is two matrixes A(x*y) and B(y*z) that are read from corresponding text files. 
The output is a matrix C(x*z) that is written to an output text file.

## The Matrix multiplication is done using these three methods:

1. A thread computes the output C matrix i.e. without multi-threading. (A thread per matrix).


2. A thread computes each row in the output C matrix. (A thread per row).


3. A thread computes each element in the output C matrix. (A thread per element).

