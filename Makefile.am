AUTOMAKE_OPTIONS = foreign subdir-objects

bin_PROGRAMS = svl

svl_SOURCES = src/main.c src/visitor.c src/lexer.c src/ast.c src/parser.c
svl_SOURCES += src/symbol.c

svl_CFLAGS = `llvm-config --cflags`
svl_LDFLAGS = `llvm-config --ldflags --libs core analysis bitwriter --system-libs` -lstdc++
