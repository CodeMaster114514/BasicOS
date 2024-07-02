%include "global.asm"

section code vstart=CoreLoaderAddress
	marker db "Hu"
	lenth dd end
	enter_point dd start

	GDTR_L  dw 0
	GDTR_in dd 0

	CoreDataAddressNextCanBeUsed dd CoreDataAddress

	ARD_count dd 0

	is_e801 db 0

	memory_size dq 0

	NumberStringCache1 times 11 db 0x00
	NumberStringCache2 times 11 db 0x00

	PhysicalAddressSize dd 0
	LinearAddressSize dd 0

	Enter32ModeMessage db "Now, we are running in the 32 bits protected mode!", 0x0a, 0x0d, 0x00
	NoIA32Message db "The computer haven't ia-32a mode, so you can run the version in the computer!", 0x0a, 0x0d, 0x00
	AddressSizeMessage db "Physical address size %d", 0x0a, 0x0d, "Linear address size %d.", 0x0a, 0x0d, 0x00
	Enter64ModeMessage db "Now, we are running in the 64 bits protected mode!", 0x0a, 0x0d, 0x00
	NoFatFileSystemMessage db "In the disk, we haven't finded any FAT fils System, so there isn't core in the disk.", 0x0a, 0x0d, 0x00
	NotEffectiveMessage db "The FAT file system is not effective!", 0x0a, 0x0d, 0x00
	NoFinedMessage db "There isn't a core file in the FAT file system", 0x0a, 0x0d, 0x00
	CorePath db "CORE"
			 times (12 - ($ - CorePath)) db 0x20
			 db 0x00

bits 32
NumberToString:
	push ebp
	mov ebp, esp
	mov dword [ebp - 4], 0
	mov ebx, NumberStringCache1
	mov edi, 0

	mov eax, [ebp + 8]
.@1:
	xor edx, edx
	mov ecx, 10
	div ecx

	add edx, 0x30
	mov [ebx + edi], edx
	inc edi
	inc dword [ebp - 4]
	or eax, eax
	jnz .@1

	mov ecx, [ebp - 4]
	mov esi, NumberStringCache2
.send:
	mov al, [ebx + ecx - 1]
	mov [esi], al
	inc esi
	loop .send
	mov byte [esi], 0x00

	pop ebp
	lea eax, [NumberStringCache2]
	ret

puts:
	push ebp
	mov ebp, esp
	sub esp, 4

	mov dword [ebp - 4], 12

	mov ebx, [ebp + 8] ;获得字符串指针
	mov eax, 0 ;字符偏移

	.again:
		xor ecx, ecx
		mov cl, [ebx + eax]
		cmp cl, 0x00
		jz .over

		cmp cl, '%'
		jz .check_other

		push eax
		push ebx

		dec esp
		mov [esp], cl
		call putc
		inc esp

		pop ebx
		pop eax
		inc eax
		jmp .again
	
	.check_other:
		mov cl, [ebx + eax + 1]
		cmp cl, 'd'
		jz .put_number

		cmp cl, 's'
		jz .put_string

		add eax, 2
		
		dec esp
		mov [esp], cl
		call putc
		inc esp

		jmp .again

		.put_number:
			push ebx
			push eax
			mov eax, [ebp - 4]
			add dword [ebp - 4], 4
			push dword [ebp + eax]
			call NumberToString
			add esp, 4
			push eax
			call puts
			add esp, 4
			pop eax
			pop ebx
			add eax, 2
			jmp .again
		
		.put_string:
			push eax
			push ebx
			mov eax, [ebp - 4]
			add dword [ebp - 4], 4
			push dword [ebp + eax]
			call puts
			add esp, 4
			pop ebx
			pop eax
			add eax, 2
			jmp .again

	.over:
		add esp, 4
		pop ebp
		ret

bits 16
start:
	mov ebx, [CoreDataAddressNextCanBeUsed]
	shr ebx, 4
	mov es, bx
	xor ebx, ebx
	mov di, 0
	.getARD:
		mov ecx, 24
		mov edx, "SMAP"
		mov eax, 0xe820
		int 0x15
		jc .e801h

		or ebx, ebx
		jz .memory_check_over
		add edi, 24
		inc dword [ARD_count]
		jmp .getARD

	.e801h:
		mov byte [is_e801], -1
		
		xor ecx, ecx
		xor edx, edx
		mov ax, 0xe801
		int 0x15
		
		push bx
		mov bx, 1024
		mul bx
		push ax
		mov ax, dx
		shl eax, 16
		pop dx
		mov [memory_size], eax

		xor eax, eax
		pop ax
		mov ebx, 0x10000
		mul ebx
		add [memory_size], eax
		jnc .no_c
		inc dword [memory_size + 4]
	.no_c:
		add [memory_size + 4], edx
	.memory_check_over:

	mov ebx, GdtAddress

	mov dword [ebx + 0x00], 0
	mov dword [ebx + 0x04], 0

	mov dword [ebx + 0x08], 0x0000ffff
	mov dword [ebx + 0x0c], 0x00cf9800 ;代码段描述符

	mov dword [ebx + 0x10], 0x0000ffff
	mov dword [ebx + 0x14], 0x00cf9200 ;数据段描述符

	mov dword [ebx + 0x18], 0
	mov dword [ebx + 0x1c], 0x00209800 ;64位代码段

	mov [GDTR_in], ebx
	mov word [GDTR_L], 31
	lgdt [GDTR_L]

	in al, 0x92
	or ax, 0x2
	out 0x92, al

	in al, 0x70
	or al, 0x80
	out 0x70, ax

	cli
	mov eax, cr0
	or eax, 1
	mov cr0, eax

	jmp 0x0008:load32

load32:
	bits 32

	mov eax, 0x00010
	mov ds, ax
	mov ss, ax
	mov esp, 0x7c00

	sub esp, 4
	mov dword [esp], Enter32ModeMessage
	call puts
	add esp, 4

	mov eax, 0x80000000
	cpuid
	cmp eax, 0x80000008
	jb .no_ia32e

	mov eax, 0x80000001
	cpuid
	bt edx, 29
	jnc .no_ia32e

	mov edi, [CoreDataAddressNextCanBeUsed]

	mov eax, 0x80000002
	cpuid
	mov [edi + 0x00], eax
	mov [edi + 0x04], ebx
	mov [edi + 0x08], ecx
	mov [edi + 0x0c], edx

	mov eax, 0x80000003
	cpuid
	mov [edi + 0x10], eax
	mov [edi + 0x14], ebx
	mov [edi + 0x18], ecx
	mov [edi + 0x1c], edx

	mov eax, 0x80000004
	cpuid
	mov [edi + 0x20], eax
	mov [edi + 0x24], ebx
	mov [edi + 0x28], ecx
	mov [edi + 0x2c], edx

	push edi
	call puts
	add esp, 4
	
	dec esp
	mov byte [esp], 0x0a
	call putc
	mov byte [esp], 0x0d
	call putc
	dec esp

	mov eax, 0x80000008
	cpuid
	mov [PhysicalAddressSize], al
	mov [LinearAddressSize], ah
	
	push dword [LinearAddressSize]
	push dword [PhysicalAddressSize]
	push dword AddressSizeMessage
	call puts
	add esp, 12

	; 准备进入ia-32e模式

	mov ebx, PML4PhysicalAddress
	mov ecx, 1024
	mov esi, 0
	.clean1:
		mov dword [ebx + esi], 0
		add esi, 4
		loop .clean1
	mov dword [ebx + 511 * 8], PML4PhysicalAddress | 3
	mov dword [ebx + 0 * 8], PDPTPhysicalAddress | 3
	mov dword [ebx + 256 * 8], PDPTPhysicalAddress | 3

	mov ebx, PDPTPhysicalAddress
	mov ecx, 1024
	mov esi, 0
	.clean2:
		mov dword [ebx + esi], 0
		add esi, 4
		loop .clean2
	mov dword [ebx + 0 * 8], PDPhysicalAddress | 3
	
	mov ebx, PDPhysicalAddress
	mov ecx, 1024
	mov esi, 0
	.clean3:
		mov dword [ebx +  esi], 0
		add esi, 4
		loop .clean3
	mov dword [ebx + 0 * 8], 0x83

	call LoadCore

	push eax

	mov eax, PML4PhysicalAddress
	mov cr3, eax

	mov eax, cr4
	bts eax, 5
	mov cr4, eax

	mov ecx, 0x0c0000080
	rdmsr

	bts eax, 8
	wrmsr

	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax

	hlt

	pop eax
	push word 0x0018
	push dword 0
	push eax
	retf

.end:
	hlt
	jmp .end

.no_ia32e:
	push dword NoIA32Message
	call puts
	add esp, 4
	jmp .end

CheckString:
	push ebp
	mov ebp, esp
	push ecx

	mov edi, [ebp + 8]
	mov esi, [ebp + 12]
	mov ecx, [ebp + 16]

	.check:
		mov al, [esi]
		cmp [edi], al
		jnz .NotSame
		inc esi
		inc edi
		loop .check

	.Same:
		mov eax, -1
		jmp .end

	.NotSame:
		mov eax, 0

.end:
	pop ecx
	pop ebp
	ret

ReadDisk:
	push ebp
	mov ebp, esp

	mov dx, 0x1f2
	mov eax, [ebp + 16]
	out dx, al

	mov eax, [ebp + 8]
	inc dx
	out dx, al

	inc dx
	shr eax, 8
	out dx, al

	inc dx
	shr eax, 8
	out dx, al

	inc dx
	shr eax, 8
	or al, 0xe0
	out dx, al

	inc dx
	mov al, 0x20
	out dx, al

	.wait:
		in al, dx
		and al, 0x89
		cmp al, 0x01
		jz .error
		cmp al, 0x08
		jnz .wait

	mov ebx, [ebp + 12]
	mov ax, [ebp + 16]
	mov cx, 512
	mul cx
	mov cx, dx
	shl ecx, 16
	mov cx, ax
	mov dx, 0x1f0
	.read:
		in ax, dx
		mov [ebx], ax
		add ebx, 2
		loop .read
		jmp .end

	.error:
		hlt

.end:
	pop ebp
	ret

OpenFile:
	push ebp
	mov ebp, esp
	sub esp, 20; ebp - 4为fat文件系统在mbr的描述; ebp - 8为core根目录的结构体; ebp - 12为数据区的LBA号; ebp - 16为一个簇的大小，单位为扇区; ebp - 20为文件的LBA号

	; 寻找Fat分区
	mov ebx, 0x7c00 + 446
	mov edi, 0
	mov ecx, 4
	.check:
		mov al, [ebx + edi + 0]
		cmp al, 0x80
		jnz .next
		mov al, [ebx + edi + 4]
		cmp al, 0x01
		jz .isFat32
		cmp al, 0x04
		jz .isFat16
		cmp al, 0x06
		jz .isFat16
	.next:
		add edi, 16
		loop .check

	jmp .NoFatFileSystem

	.isFat32:

	.isFat16:
		lea ebx, [ebx + edi]
		mov [ebp - 4], ebx
		push dword 1
		push dword [ebp + 12]
		push dword [ebx + 8]
		call ReadDisk
		add esp, 12

		mov ebx, [ebp + 12]
		cmp word [ebx + 510], 0xaa55
		jnz .NotEffective

		mov al, [ebx + 0x0d]
		mov [ebp - 16], al

		mov eax, [ebx + 0x1c]
		mov [ebp - 12], eax
		
		xor eax, eax
		mov ax, [ebx + 0x0e]
		add [ebp - 12], eax

		mov ebx, [ebp + 12]
		mov ax, [ebx + 0x16]
		xor ecx, ecx
		mov cl, [ebx + 0x10]
		mul cx
		shl edx, 16
		mov dx, ax

		add [ebp - 12], edx

		xor eax, eax
		mov ax, [ebx + 0x11]
		xor edx, edx
		xor cx, cx
		mov cl, 32
		mul cx
		mov cx, 512
		div cx
		or dx, dx
		jz .@1
		inc eax
	.@1:
		mov edx, eax

		push edx
		push dword [ebp + 12]
		push dword [ebp - 12]
		call ReadDisk
		add esp, 8
		pop edx
		add [ebp - 12], edx

		mov ecx, 512
		mov ebx, [ebp + 12]
		mov edi, 0
		.find:
			lea eax, [ebx + edi + 0]
			push edi; 保护edi寄存器
			push dword 12
			push eax
			push dword CorePath
			call CheckString
			add esp, 12
			pop edi
			or eax, eax
			jnz .finded
			add edi, 32
			loop .find
		
		jmp .NoFined

		.finded:
			lea ebx, [ebx + edi]
			mov [ebp -8], ebx
			xor eax, eax
			mov ax, [ebx + 0x1a]
			sub ax, 2
			xor ecx, ecx
			mov cl, [ebp - 16]
			mul cx
			shl edx, 16
			mov dx, ax
			add edx, [ebp - 12]
			mov [ebp - 20], edx

			mov eax, [ebx + 0x1c]
			push ax
			shr eax, 16
			mov dx, ax
			pop ax
			mov cx, 512
			div cx
			or dx, dx
			jz .read
			inc eax
		.read:
			push eax
			push dword [ebp + 12]
			push dword [ebp - 20]
			call ReadDisk
			add esp, 12
			mov eax, -1
			jmp .end
	
	.NoFined:
		push dword NoFinedMessage
		call puts
		add esp, 4
		mov eax, 0
		jmp .end
	
	.NotEffective:
		push dword NotEffectiveMessage
		call puts
		add esp, 4
		mov eax, 0
		jmp .end

	.NoFatFileSystem:
		push dword NoFatFileSystemMessage
		call puts
		add esp, 4
		mov eax, 0

	.end:
		add esp, 20
		pop ebp
		ret

LoadCore:
	push ebp
	mov ebp, esp
	sub esp, 4

	push dword CoreCache
	push dword CorePath
	call OpenFile
	add esp, 8

	hlt

	add esp, 4
	pop ebp
	ret

putc:
	push ebp
	mov ebp, esp
	mov ch, [ebp + 8]
	xor ebx, ebx

	mov dx, 0x3d4 
	mov al, 0x0f
	out dx, al

	inc dx
	in al, dx
	mov bl, al

	dec dx
	mov al, 0x0e
	out dx, al

	inc dx
	in al, dx
	mov bh, al

	cmp ch, 0x0d
	jnz .put_0x0a

	.put_0d:
		mov eax, ebx
		mov bl, 80
		div bl

		mul bl
		mov ebx, eax
		jmp .set

	.put_0x0a:
		cmp ch, 0x0a
		jnz .put_others

		add ebx, 80
		jmp .roll

	.put_others:
		shl ebx, 1
		mov [0xb8000 + ebx], ch
		shr ebx, 1
		inc ebx

	.roll:
		cmp ebx, 2000
		jnz .set
		mov ax, 0x0010
		mov es, ax
		mov edi, 0xb800
		mov esi, 0xb814
		
		mov ecx, 1920
		rep movsw
		
		mov ebx, 1920

	.set:
		mov dx, 0x3d4
		mov al, 0x0e
		out dx, al

		inc dx
		mov al, bh
		out dx, al

		dec dx
		mov al, 0x0f
		out dx, al
		
		inc dx
		mov al, bl
		out dx, al

	pop ebp
	ret

section trail
	end:
		dd OpenFile.finded