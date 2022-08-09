# fcmpl

### Description
Simple Auto Completion engine with REPL

### Dependencies
[Graphviz](https://graphviz.org/) is used for visualizing graph

[Installation instructions for Graphviz](https://graphviz.org/download/)

### Compilation
```sh
$ make
```

For tests
```sh
$ make test
```

For cleaning up
```sh
$ make clean
```

### Supported characters
[A-Za-z] are supported currently, but can be easily extended by modifying:
- **int hash(char ch)** function
- **NUMBER_OF_LETTERS** macro

### Supported operations
- Adding new word
- Loading file of words
- Deleting existing word
- Checking existence of word
- Completing prefix
- Generating dot and svg file for graph

### Using REPL
```
> .put new
> .check new
new
> n
new
> .load test-files/dict.txt
[INFO] Started to load file
[INFO] Ignoring invalid word new_apple
[INFO] Ignoring invalid word ./,/.,/
[INFO] Ignoring invalid word ()23892
[INFO] Ignoring invalid word 392090
> .delete new
> ap
app
apple
application
> .generate out/dict # will generate out/dict.dot and out/dict.svg
> .quit
Have a good day!
```

### Generated files
After **.generate** operation, two files are generated
- x.dot
- x.svg

x.dot is dot file (the dot language **Graphviz** uses for generating graphs in different formats) that generated from trie

x.svg is svg file generated from x.dot by using **Graphviz**

![Svg generated from dictionary](test-files/dict.svg)
