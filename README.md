## General Directed Weighted Graph

A C++20 project implementing a generic directed weighted graph container with value semantics.
The API supports both weighted and unweighted edges, deterministic ordering, and an iterable edge view.

## What this project does

- Stores unique nodes of type `N`.
- Stores directed edges between nodes, with optional weights of type `E`.
- Supports one unweighted edge per `(src, dst)` pair, plus multiple weighted edges with distinct weights.
- Keeps edges ordered by `(src, dst, weight)` where unweighted edges come before weighted ones.
- Exposes polymorphic edge types: `Edge`, `WeightedEdge`, and `UnweightedEdge`.
- Supports graph mutation, queries, iteration, stream output, and equality comparison.

For full behavioral contracts (preconditions, postconditions, exceptions, and complexity), see [FUNCTIONALITY.md](./FUNCTIONALITY.md).

## Build

This project uses CMake and requires a C++20-capable compiler.

```bash
cmake -S . -B build
cmake --build build
```

To conveniently re-compile after modifying:
```bash
cd build
make
```

## Run

Build outputs include:

- `client` (demo executable)
- `gdwg_graph_test_exe` (test binary)

Run the demo [application](./src/client.cpp):

```bash
./build/client
```
## GDB Debugging
To debug with GDB, add the `-g` option in the [CMakeLists.txt](./CMakeLists.txt) compile options in order to generate debugging information.

If you are on VSCode, simply run the built-in debugger on your preferred application (Client/Test). Make sure to run the commands in [Build](#build) to generate the latest executables. The configurations for VSCode built-in debugger are defined in the `.vscode` folder at root directory.

## Test

Run the test executable directly:

```bash
./build/gdwg_graph_test_exe
```

