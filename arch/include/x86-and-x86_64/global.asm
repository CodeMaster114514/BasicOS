%define CoreLoaderAddress 0x8e00
%define CoreDataAddress 0x7e00
%define CoreCache 0x200000
%define FileSystemCache 0x30000
%define GdtAddress 0xce00
%define PML4PhysicalAddress 0x12000
%define PDPTPhysicalAddress 0x13000
%define PDPhysicalAddress 0x14000
%define PTPhysicalAddress 0x15000

%ifdef mbr

; 这段代码可以作为32位系统和64位系统的mbr

org 0x7c00

mov ax, cs
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x7c00

push dword 0
push dword 1
mov ax, cs
push ax
push word CoreLoaderAddress
push word 1
push word 0x0010
mov ah, 0x42
mov dl, 0x80
mov si, sp
int 0x13

%endif
