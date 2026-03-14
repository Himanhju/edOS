#!/bin/bash

# create nvme drive because seabios wont boot from nvme drive
qemu-img create disk.img 16G

# run with nvme drive and other peripherals
qemu-system-x86_64 --machine q35,accel=kvm -m 4096 -cpu host -smp 16 --enable-kvm -netdev user,id=net0 -device nvme,id=nvme,serial=beadbeef,physical_block_size=512,drive=Disk1 -drive file=disk.img,format=raw,media=disk,id=Disk1,if=none -usb -drive if=none,id=stick,format=raw,file=edOS.fs -device usb-storage,drive=stick
