gcc -O2 -Werror -Wall -ffreestanding -fno-stack-protector -fno-builtin -nostdlib -m32 -c libs/stdio.c -o obj/stdio.o &&\
gcc -O2 -Werror -Wall -ffreestanding -fno-stack-protector -fno-builtin -nostdlib -m32 -c libs/stdlib.c -o obj/stdlib.o &&\
gcc -O2 -Werror -Wall -ffreestanding -fno-stack-protector -fno-builtin -nostdlib -m32 -c libs/unistd.c -o obj/unistd.o &&\


gcc -O2 -Werror -Wall -ffreestanding -fno-stack-protector -fno-builtin -nostdlib -m32 -c $1 -o obj/${1%.*}.o &&\

lld.ld -o bin/${1%.*}.elf \
 obj/stdio.o obj/stdlib.o obj/unistd.o \
 obj/${1%.*}.o