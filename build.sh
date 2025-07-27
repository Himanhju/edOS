#!/bin/bash

mkdir -p mid
mkdir -p obj

# assemble the bootloader
nasm -f bin boot.asm -o mid/boot.bin &&\
truncate mid/boot.bin --size 512;

# compile the main kernel
gcc -O2 -Werror -Wall -Wextra -Wpedantic -Wnull-dereference -Wformat=2 -Wfloat-equal -Wmissing-declarations -Wconversion -Wstrict-overflow=2 -Wsign-conversion -nostdinc -fno-pic -fno-pie -Wshadow -Wundef -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -ffreestanding -fno-stack-protector -fno-builtin -nostdlib -m32 -c edOS.c -o obj/edOS.o &&\
gcc -O2 -Werror -Wall -Wextra -Wpedantic -Wnull-dereference -Wformat=2 -Wfloat-equal -Wmissing-declarations -Wconversion -Wstrict-overflow=2 -Wsign-conversion -nostdinc -fno-pic -fno-pie -Wshadow -Wundef -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -ffreestanding -fno-stack-protector -fno-builtin -nostdlib -m32 -c include/C/Graphics/VESA.c -o obj/Graph_VESA.o &&\
gcc -O2 -Werror -Wall -Wextra -Wpedantic -Wnull-dereference -Wformat=2 -Wfloat-equal -Wmissing-declarations -Wconversion -Wstrict-overflow=2 -Wsign-conversion -nostdinc -fno-pic -fno-pie -Wshadow -Wundef -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -ffreestanding -fno-stack-protector -fno-builtin -nostdlib -m32 -c include/C/Interrupts/IDT/IDT.c -o obj/Ints_IDT.o &&\
gcc -O2 -Werror -Wall -Wextra -Wpedantic -Wnull-dereference -Wformat=2 -Wfloat-equal -Wmissing-declarations -Wconversion -Wstrict-overflow=2 -Wsign-conversion -nostdinc -fno-pic -fno-pie -Wshadow -Wundef -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -ffreestanding -fno-stack-protector -fno-builtin -nostdlib -m32 -c include/C/Interrupts/ISRs/ISRs.c -o obj/Ints_ISRs.o &&\
gcc -O2 -Werror -Wall -Wextra -Wpedantic -Wnull-dereference -Wformat=2 -Wfloat-equal -Wmissing-declarations -Wconversion -Wstrict-overflow=2 -Wsign-conversion -nostdinc -fno-pic -fno-pie -Wshadow -Wundef -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -ffreestanding -fno-stack-protector -fno-builtin -nostdlib -m32 -c include/C/Memory/Operations/Operations.c -o obj/Mem_Operations.o &&\
gcc -O2 -Werror -Wall -Wextra -Wpedantic -Wnull-dereference -Wformat=2 -Wfloat-equal -Wmissing-declarations -Wconversion -Wstrict-overflow=2 -Wsign-conversion -nostdinc -fno-pic -fno-pie -Wshadow -Wundef -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -ffreestanding -fno-stack-protector -fno-builtin -nostdlib -m32 -c include/C/Memory/Opcodes/Opcodes.c -o obj/Mem_Opcodes.o &&\
gcc -O2 -Werror -Wall -Wextra -Wpedantic -Wnull-dereference -Wformat=2 -Wfloat-equal -Wmissing-declarations -Wconversion -Wstrict-overflow=2 -Wsign-conversion -nostdinc -fno-pic -fno-pie -Wshadow -Wundef -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -ffreestanding -fno-stack-protector -fno-builtin -nostdlib -m32 -c include/C/General/Time/Time.c -o obj/Gen_Time.o &&\
#gcc -O2 -Werror -Wall -Wextra -Wpedantic -Wnull-dereference -Wformat=2 -Wfloat-equal -Wmissing-declarations -Wconversion -Wstrict-overflow=2 -Wsign-conversion -nostdinc -fno-pic -fno-pie -Wshadow -Wundef -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -ffreestanding -fno-stack-protector -fno-builtin -nostdlib -m32 -c include/C/Input/Mouse/Mouse.c -o obj/In_Mouse.o &&\ nothing in it
gcc -O2 -Werror -Wall -Wextra -Wpedantic -Wnull-dereference -Wformat=2 -Wfloat-equal -Wmissing-declarations -Wconversion -Wstrict-overflow=2 -Wsign-conversion -nostdinc -fno-pic -fno-pie -Wshadow -Wundef -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -ffreestanding -fno-stack-protector -fno-builtin -nostdlib -m32 -c include/C/Input/Keyboard/Keyboard.c -o obj/In_Key.o &&\
gcc -O2 -Werror -Wall -Wextra -Wpedantic -Wnull-dereference -Wformat=2 -Wfloat-equal -Wmissing-declarations -Wconversion -Wstrict-overflow=2 -Wsign-conversion -nostdinc -fno-pic -fno-pie -Wshadow -Wundef -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -ffreestanding -fno-stack-protector -fno-builtin -nostdlib -m32 -c include/C/General/shell/shell.c -o obj/Gen_Shell.o &&\
gcc -O2 -Werror -Wall -Wextra -Wpedantic -Wnull-dereference -Wformat=2 -Wfloat-equal -Wmissing-declarations -Wconversion -Wstrict-overflow=2 -Wsign-conversion -nostdinc -fno-pic -fno-pie -Wshadow -Wundef -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -ffreestanding -fno-stack-protector -fno-builtin -nostdlib -m32 -c include/C/vdFS/vdFS.c -o obj/FS_vdFS.o &&\
gcc -O2 -Werror -Wall -Wextra -Wpedantic -Wnull-dereference -Wformat=2 -Wfloat-equal -Wmissing-declarations -Wconversion -Wstrict-overflow=2 -Wsign-conversion -nostdinc -fno-pic -fno-pie -Wshadow -Wundef -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -ffreestanding -fno-stack-protector -fno-builtin -nostdlib -m32 -c include/C/General/Info/Info.c -o obj/Gen_Info.o &&\
gcc -O2 -Werror -Wall -Wextra -Wpedantic -Wnull-dereference -Wformat=2 -Wfloat-equal -Wmissing-declarations -Wconversion -Wstrict-overflow=2 -Wsign-conversion -nostdinc -fno-pic -fno-pie -Wshadow -Wundef -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -ffreestanding -fno-stack-protector -fno-builtin -nostdlib -m32 -c include/C/Coms/PCI/PCI.c -o obj/Com_PCI.o &&\
gcc -O2 -Werror -Wall -Wextra -Wpedantic -Wnull-dereference -Wformat=2 -Wfloat-equal -Wmissing-declarations -Wconversion -Wstrict-overflow=2 -Wsign-conversion -nostdinc -fno-pic -fno-pie -Wshadow -Wundef -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -ffreestanding -fno-stack-protector -fno-builtin -nostdlib -m32 -c include/C/Drivers/ACPI/ACPI.c -o obj/Drv_ACPI.o &&\
#gcc -O2 -Werror -Wall -Wextra -Wpedantic -Wnull-dereference -Wformat=2 -Wfloat-equal -Wmissing-declarations -Wconversion -Wstrict-overflow=2 -Wsign-conversion -nostdinc -fno-pic -fno-pie -Wshadow -Wundef -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -ffreestanding -fno-stack-protector -fno-builtin -nostdlib -m32 -c include/C/Interrupts/Syscall/Syscall.c -o obj/Ints_Syscall.o &&\
gcc -O2 -Werror -Wall -Wextra -Wpedantic -Wnull-dereference -Wformat=2 -Wfloat-equal -Wmissing-declarations -Wconversion -Wstrict-overflow=2 -Wsign-conversion -nostdinc -fno-pic -fno-pie -Wshadow -Wundef -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -ffreestanding -fno-stack-protector -fno-builtin -nostdlib -m32 -c include/C/Drivers/Inet/Inet.c -o obj/Drv_Inet.o &&\
gcc -O2 -Werror -Wall -Wextra -Wpedantic -Wnull-dereference -Wformat=2 -Wfloat-equal -Wmissing-declarations -Wconversion -Wstrict-overflow=2 -Wsign-conversion -nostdinc -fno-pic -fno-pie -Wshadow -Wundef -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -ffreestanding -fno-stack-protector -fno-builtin -nostdlib -m32 -c include/C/Coms/IO/IO.c -o obj/Com_IO.o &&\
gcc -O2 -Werror -Wall -Wextra -Wpedantic -Wnull-dereference -Wformat=2 -Wfloat-equal -Wmissing-declarations -Wconversion -Wstrict-overflow=2 -Wsign-conversion -nostdinc -fno-pic -fno-pie -Wshadow -Wundef -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -ffreestanding -fno-stack-protector -fno-builtin -nostdlib -m32 -c include/C/Memory/Paging/Paging.c -o obj/Mem_Paging.o &&\
gcc -O2 -Werror -Wall -Wextra -Wpedantic -Wnull-dereference -Wformat=2 -Wfloat-equal -Wmissing-declarations -Wconversion -Wstrict-overflow=2 -Wsign-conversion -nostdinc -fno-pic -fno-pie -Wshadow -Wundef -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -ffreestanding -fno-stack-protector -fno-builtin -nostdlib -m32 -c include/C/Memory/Palloc/Palloc.c -o obj/Mem_Palloc.o &&\
gcc -O2 -Werror -Wall -Wextra -Wpedantic -Wnull-dereference -Wformat=2 -Wfloat-equal -Wmissing-declarations -Wconversion -Wstrict-overflow=2 -Wsign-conversion -nostdinc -fno-pic -fno-pie -Wshadow -Wundef -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -ffreestanding -fno-stack-protector -fno-builtin -nostdlib -m32 -c include/C/Drivers/ROM/ROM.c -o obj/Drv_ROM.o &&\

nasm -f elf32 include/C/Interrupts/ISRs/ISR.asm -o obj/Asm_ISRs.o;
code1=$?;

# link the .o files together
$(exit $code1) &&\
echo now linking the kernel with ld.lld &&\

ld.lld -Ttext 0x9400 -o mid/elf.elf obj/edOS.o \
 obj/Ints_ISRs.o obj/Asm_ISRs.o obj/Ints_IDT.o  \
 obj/FS_vdFS.o\
 obj/Graph_VESA.o\
 obj/Mem_Opcodes.o obj/Mem_Operations.o obj/Mem_Palloc.o obj/Mem_Paging.o\
 obj/Com_PCI.o obj/Com_IO.o\
 obj/Drv_ROM.o obj/Drv_ACPI.o obj/Drv_Inet.o\
 obj/In_Key.o\
 obj/Gen_Shell.o obj/Gen_Time.o obj/Gen_Info.o #&&\

#obj/Ints_Syscall.o obj/In_Mouse.o

llvm-objcopy -O binary mid/elf.elf mid/cc.bin &&\
nasm -f bin edOS.asm -o mid/asmraw.bin &&\

touch mid/edOS.bin &&\
dd if=mid/asmraw.bin of=mid/edOS.bin bs=512 count=10 conv=notrunc &&\
dd if=mid/cc.bin of=mid/edOS.bin bs=512 seek=11 conv=notrunc &&\

dd if=/dev/zero    of=mid/edOS.img bs=512 count=2048 &&\
dd if=mid/boot.bin of=mid/edOS.img bs=512 count=1 conv=notrunc &&\
dd if=mid/edOS.bin of=mid/edOS.img bs=512 seek=1 conv=notrunc &&\

if [[ -f "./edOS.fs" ]]; then
    dd if=mid/edOS.img of=edOS.fs seek=0;
else
    qemu-img create ./edOS.fs 1G &&\
    dd if=mid/edOS.img of=edOS.fs seek=0;
fi
