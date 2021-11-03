# minidb

A mini database for learning database

## Compiling

You need:
- Cmake build system

on Linux/OS X/FreeBSD

```bash
$ git clone https://github.com/ChuckieTan/minidb.git
$ cd minidb
$ mkdir build/ && cd build/
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ make -j
```

## Running

just run minidb directly

```bash
$ ./minidb
```

## Tasks

### TODO

- [ ] SQL where statement executor
- [ ] B+Tree delete operate

### In Progress

- [ ] B+Tree traverse operate

### Done

- [x] SQL lexer
- [x] SQL AST struct
- [x] SQL parser
- [x] pager for file I/O
- [x] B+Tree struct
- [x] B+Tree select operate
- [x] B+Tree insert operate
- [x] Operate class for execute SQL statement
- [x] Minidb 64bit support