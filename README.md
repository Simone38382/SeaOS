# SeaOS
OS in c

# Compile it
here's it!
```
nasm -f elf multiboot_header.asm -o multiboot_header.o
nasm -f elf kernel_entry.asm -o kernel_entry.o
gcc -m32 -ffreestanding -c kernel.c -o kernel.o
ld -m elf_i386 -T linker.ld -o kernel.bin multiboot_header.o kernel_entry.o kernel.o
```
then run
```
cp kernel.bin iso/boot/
grub-mkrescue -o SeaOS.iso iso
```
