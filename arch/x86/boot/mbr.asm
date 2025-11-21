%include "boot/boot.asm"

bits 16

org 0x7c00

xor ax, ax
mov ds, ax
mov ss, ax
mov es, ax
mov sp, 0x7c00

mov ah, 0x41
mov bx, 0x55AA,
mov dl, 0x80
int 0x13
mov word [error_msg + 11], '0'
jc error

push dword 0x0000
push dword 0x0001
mov eax, 0x8000
mov bx, ax
shr eax, 16
shl eax, 12
push ax
push bx
push word 1
push word 0x0010
mov ah, 0x42
mov dl, 0x80
mov si, sp
int 0x13
add sp, 0x10
mov byte [error_msg + 11], '1'
jc error

mov bx, 0x8000
cmp dword [bx], 0x4e756872 ;判断标志
mov byte [error_msg + 11], '2'
jnz error

xor ecx, ecx
push word [bx + 8]
push dword 2
push dword 0
@0:
	mov bp, sp
	cmp cx, [bp + 8]
	jz @2
	xor eax, eax
	push dword [bp + 4]
	mov al, 8
	mov bx, cx
	inc bx
	mul bx
	mov bx, 0x8000
	add bx, ax
	add bx, 2
	mov eax, [bx];获得该段起始地址
	shr eax, 16
	shl eax, 12
	push ax
	push word [bx]
	mov eax, [bx + 4];获得该段大小
	push ax
	shr eax, 16
	mov dx, ax
	pop ax
	mov bp,512
	div bp

	or dx, dx
	jz @1
	inc ax
	@1:
		push ax
		xor eax, eax
		pop ax
		add [esp + 12], eax
		push ax
		push word 0x0010
		xor ax, ax
		mov ah, 0x42
		mov dl, 0x80
		mov si, sp
		int 0x13
		add sp, 0x0c
		mov byte [error_msg + 11], '1'
		jc error
		inc cx
jmp @0
@2:
mov di, 0x7e00
xor ebx, ebx
@3:
xor ecx, ecx
xor eax, eax
mov edx, "PAMS"
mov cl, 32
mov ax, 0xe820
int 0x15
mov byte [error_msg + 11], '3'
jc error
inc dword [0x9000]
add di, 24
or ebx, ebx
jnz @3
mov ebx, 0x10000
mov eax, ebx
shr eax, 16
shl eax, 12
mov es, ax
mov dword  es:[bx + 0x00], 0x00000000
mov dword  es:[bx + 0x04], 0x00000000
mov dword  es:[bx + 0x08], 0x0000ffff; 数据段
mov dword  es:[bx + 0x0c], 0x00cf9200
mov dword  es:[bx + 0x10], 0x0000ffff; 32位代码段
mov dword  es:[bx + 0x14], 0x00cf9800
mov dword  es:[bx + 0x18], 0x0000ffff; 64位代码段
mov dword  es:[bx + 0x1c], 0x00af9800
;mov dword  es:[bx + 0x20], 0x0000ffff; 64位数据段
;mov dword  es:[bx + 0x24], 0x00df9200
push ebx
push word 31
cli
mov bx,sp
lgdt ss:[bx]
in al, 0x92
or al, 2
out 0x92, al
xor ebx, ebx
mov bx, 0x8004
mov eax, cr0
or al, 1
mov cr0, eax
jmp dword 0x0010:@4

bits 32

@4:
mov ax, 0x0008
mov ss, ax
mov sp, 0x7c00
jmp [bx]
;push dword [es:bx + 8]
;mov eax, [es:bx + 4]
;push ax
;shr eax, 16
;mov dx, ax
;pop ax
;mov cx,512
;div cx

;or dx, dx
;jz @1
;inc ax
;@1:
;	push dword 0x0000
;	push dword 0x0002
;	mov edx, LoaderAddress
;	mov bx, dx
;	shr edx, 4
;	push dx
;	push bx
;	push ax
;	push word 0x0010
;	xor ax, ax
;	mov ah, 0x42
;	mov dl, 0x80
;	mov si, sp
;	int 0x13
;	add sp, 0x10
;	pop eax
;	mov edx,eax
;	shr edx, 16
;	shl edx, 12
;	push dx
;	push ax
;	retf

jmp stop

error:
	mov ah, 0x03
	xor bx, bx
	int 0x10
	mov bp, error_msg
	mov cx, error_msg_len
	mov bx, 0x000f
	mov ax, 0x1301
	int 0x10
	jmp stop

stop:
	hlt
	jmp stop
error_msg:
	db "Error", 0
error_msg_len equ $ - error_msg
times 440 - ($ - $$) db 0

