# B+ Tree Implementation

## Overview

This repository contains a C++ implementation of a B+ tree, a self-balancing tree data structure widely used in database systems for indexing and searching. The B+ tree is an extension of the B-tree with additional properties, making it particularly suitable for use in database systems.

## B+ Tree

### What is a B+ Tree?

A B+ tree is a type of self-balancing tree data structure that maintains sorted data and allows efficient search, insertion, and deletion operations. It is commonly used in database systems to create indexes, enabling faster retrieval of data.

Key features of a B+ tree include:

- **Order**: The maximum number of children for any internal node.
- **Sorted Keys**: Keys are stored in sorted order within each node.
- **Balanced Structure**: B+ trees are self-balancing, ensuring a relatively uniform distribution of keys.

### Implementation Details

This implementation features a right-biased B+ tree, where keys are stored in the leaf nodes, and internal nodes contain pointers to facilitate efficient search operations.

## Usage

The main program provides a simple command-line interface to interact with the B+ tree. Users can insert keys, delete keys, and display the current state of the tree.

### Operations:

1. **Insert a Key**: Add a key to the B+ tree.
2. **Delete a Key**: Remove a key from the B+ tree.
3. **Display the B+ Tree**: Visualize the current state of the B+ tree.
4. **Exit the Program**: Terminate the program.

## Getting Started

To use this B+ tree implementation, follow these steps:

1. Clone the repository:

   ```bash
   git clone https://github.com/bhav-khurana/B-Plus-Tree-cpp
   ```
2. Compile the code:
    ```bash
    g++ -o bplus_tree main.cpp
    ```
3. Run the executable:
    ```bash
    ./bplus_tree
    ```

# Contributions
Contributions to enhance or optimize this B+ tree implementation are welcome. Feel free to submit issues, propose new features, or create pull requests.



_Note: This implementation is a simplified version and may not cover all edge cases or optimizations found in production-grade B+ tree implementations._
