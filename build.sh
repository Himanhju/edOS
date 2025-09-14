#!/bin/bash

rm -r mid obj

mkdir -p mid
mkdir -p obj

set -e

CC=gcc-14

LD=ld

ASMFLAGS="\
 -wall -w+error \
 -O3\
"

CFLAGS="\
 -Iinclude/C -isystem include/C\
 -Oz -mtune=k8 -std=c23\
 -Werror -Wall -Wextra -Wpedantic -Wnull-dereference -Wformat=2 -Wfloat-equal -Wmissing-declarations -Wconversion -Wstrict-overflow=2 -Wsign-conversion \
 -nostdinc -nostdlib\
 -fno-pic -fno-pie \
 -Wshadow -Wundef -Wcast-align -Wstrict-prototypes -Wmissing-prototypes \
 -ffreestanding -fno-stack-protector -fno-builtin -fsigned-char -fasm\
 -m32 \
 -c\
"

LFLAGS="\
 -z max-page-size=1\
 -m elf_i386\
 -Ttext=0x8000\
"

# assemble the bootloader
nasm $ASMFLAGS -f bin boot.asm -o mid/boot.bin;
truncate mid/boot.bin --size 512;

# compile the main kernel
$CC $CFLAGS edOS.c                                     -o obj/edOS.o;
$CC $CFLAGS include/C/Graphics/VESA/VESA.c             -o obj/Graph_VESA.o;
$CC $CFLAGS include/C/Interrupts/IDT/IDT.c             -o obj/Ints_IDT.o;
$CC $CFLAGS include/C/Interrupts/ISRs/ISRs.c           -o obj/Ints_ISRs.o;
$CC $CFLAGS include/C/Memory/Operations/Operations.c   -o obj/Mem_Operations.o;
$CC $CFLAGS include/C/Memory/Opcodes/Opcodes.c         -o obj/Mem_Opcodes.o;
$CC $CFLAGS include/C/General/Time/Time.c              -o obj/Gen_Time.o;
#$CC $CFLAGS include/C/Input/Mouse/Mouse.c              -o obj/In_Mouse.o;
$CC $CFLAGS include/C/Input/Keyboard/Keyboard.c        -o obj/In_Key.o;
$CC $CFLAGS include/C/General/shell/shell.c            -o obj/Gen_Shell.o;
$CC $CFLAGS include/C/FS/vdFS/vdFS.c                   -o obj/FS_vdFS.o;
$CC $CFLAGS include/C/General/Info/Info.c              -o obj/Gen_Info.o;
$CC $CFLAGS include/C/Coms/PCI/PCI.c                   -o obj/Com_PCI.o;
$CC $CFLAGS include/C/Drivers/ACPI/ACPI.c              -o obj/Drv_ACPI.o;
#$CC $CFLAGS include/C/Interrupts/Syscall/Syscall.c     -o obj/Ints_Syscall.o;
$CC $CFLAGS include/C/Drivers/Inet/e1000/e1000.c       -o obj/Inet_82540EM.o;
$CC $CFLAGS include/C/Coms/IO/IO.c                     -o obj/Com_IO.o;
$CC $CFLAGS include/C/Memory/Paging/Paging.c           -o obj/Mem_Paging.o;
$CC $CFLAGS include/C/Memory/Heap/Palloc/Palloc.c      -o obj/Mem_Palloc.o;
$CC $CFLAGS include/C/Drivers/ROM/ROM.c                -o obj/Drv_ROM.o;
$CC $CFLAGS include/C/FS/FAT32/FAT32.c                 -o obj/FS_FAT32.o;
$CC $CFLAGS include/C/Drivers/Disk/Disk.c              -o obj/Drv_Disk.o;
$CC $CFLAGS include/C/Drivers/Inet/Inet.c              -o obj/Drv_Inet.o;
$CC $CFLAGS include/C/Drivers/Disk/ATA/ATA.c           -o obj/Disk_ATA.o;
$CC $CFLAGS include/C/Drivers/Disk/NVMe/NVMe.c         -o obj/Disk_NVMe.o;
$CC $CFLAGS include/C/Drivers/Disk/SATA/SATA.c         -o obj/Disk_SATA.o;
$CC $CFLAGS include/C/Scheduler/Scheduler.c            -o obj/CPU_Scheduler.o;
$CC $CFLAGS include/C/Memory/Heap/Dalloc/Dalloc.c      -o obj/Mem_Dalloc.o;
$CC $CFLAGS include/C/FS/FS.c                          -o obj/FS_Main;

nasm $ASMFLAGS -f elf32 include/C/Interrupts/ISRs/ISR.asm -o obj/Asm_ISRs.o;

# link the .o files together
echo now linking the kernel with ld;

$LD $LFLAGS -o mid/elf.elf obj/edOS.o \
 obj/Ints_ISRs.o obj/Asm_ISRs.o obj/Ints_IDT.o  \
 obj/Graph_VESA.o\
 obj/Gen_Time.o\
 obj/Mem_Opcodes.o obj/Mem_Operations.o obj/Mem_Palloc.o obj/Mem_Paging.o obj/Mem_Dalloc.o\
 obj/Drv_Disk.o obj/Disk_ATA.o obj/Disk_NVMe.o obj/Disk_SATA.o\
 obj/Com_PCI.o obj/Com_IO.o\
 obj/Drv_ROM.o obj/Drv_ACPI.o\
 obj/Drv_Inet.o obj/Inet_82540EM.o\
 obj/FS_FAT32.o obj/FS_vdFS.o obj/FS_Main\
 obj/In_Key.o\
 obj/Gen_Shell.o obj/Gen_Info.o\
 obj/CPU_Scheduler.o;

#obj/Ints_Syscall.o obj/In_Mouse.o obj/Drv_IDE.o obj/CPU_CPUID.o

llvm-objcopy -O binary mid/elf.elf mid/cc.bin;
nasm -f bin edOS.asm -o mid/asmraw.bin;

echo

touch mid/edOS.bin;
dd if=mid/asmraw.bin of=mid/edOS.bin bs=512 count=1 conv=notrunc;
dd if=mid/cc.bin of=mid/edOS.bin bs=512 seek=1 conv=notrunc;

echo

dd if=/dev/zero    of=mid/edOS.img bs=512 count=2048;
dd if=mid/boot.bin of=mid/edOS.img bs=512 count=1 conv=notrunc;
dd if=mid/edOS.bin of=mid/edOS.img bs=512 seek=1 conv=notrunc;

echo
echo

if [[ -f "./edOS.fs" ]]; then
    dd if=mid/edOS.img of=edOS.fs seek=0;
else
    qemu-img create ./edOS.fs 10G;
    dd if=mid/edOS.img of=edOS.fs seek=0;
fi