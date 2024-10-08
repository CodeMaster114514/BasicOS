%define mbr

%include "global.asm"

jc .read_error

cmp word [CoreLoaderAddress], "Hu"; 验证CoreLoader是否为系统提供的
jnz .stop

;计算还要读多少扇区
mov ebx, CoreLoaderAddress
mov eax, [ebx + 2]
xor edx, edx
mov ecx, 512
div ecx
or edx, edx
jnz .@1
dec eax
.@1:
	or eax, eax
	jz .go_loader

; 更改global.asm中传给0x18中断的表
inc dword [si + 8]
add word [si + 4], 512
mov [si + 2], ax
mov ax, 0x4200
mov dl, 0x80
int 0x13

.go_loader:
	mov bx, CoreLoaderAddress
	jmp dword [bx + 6]

.read_error:
	mov si, message0
	mov di, 0xb800
	std
	mov cx, 10
	rep movsb
.stop:
	hlt
	jmp .stop

message0 db "Disk error"
times 424 - ($ - $$) db 0