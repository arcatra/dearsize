
compile() {
    gcc -Wall -Wextra -std=c11 src/main.c -o bin/dearsize -l m
}


if [ -d bin ]; then
    compile
    exit 0
fi

mkdir bin/
compile


