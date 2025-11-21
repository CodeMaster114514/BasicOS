	.file	"video.c"
	.intel_syntax noprefix
	.text
.Ltext0:
	.file 0 "/home/hu-hangyi/BasicOS/arch/x86/boot" "video.c"
	.globl	putc
	.type	putc, @function
putc:
.LFB0:
	.file 1 "video.c"
	.loc 1 6 1
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	push	esi
	push	ebx
	sub	esp, 32
	.cfi_offset 6, -12
	.cfi_offset 3, -16
	call	__x86.get_pc_thunk.ax
	add	eax, OFFSET FLAT:_GLOBAL_OFFSET_TABLE_
	mov	edx, DWORD PTR 8[ebp]
	mov	BYTE PTR -28[ebp], dl
	.loc 1 7 20
	mov	ebx, eax
	call	getCursor@PLT
	.loc 1 7 11 discriminator 1
	mov	WORD PTR -10[ebp], ax
	.loc 1 8 11
	mov	DWORD PTR -16[ebp], 753664
	.loc 1 9 5
	movsx	eax, BYTE PTR -28[ebp]
	cmp	eax, 10
	je	.L2
	cmp	eax, 13
	je	.L3
	jmp	.L6
.L2:
	.loc 1 12 25
	movzx	ebx, WORD PTR -10[ebp]
	movzx	edx, WORD PTR -10[ebp]
	movsx	eax, dx
	imul	eax, eax, 26215
	shr	eax, 16
	sar	ax, 5
	mov	esi, edx
	sar	si, 15
	sub	eax, esi
	mov	ecx, eax
	mov	eax, ecx
	sal	eax, 2
	add	eax, ecx
	sal	eax, 4
	mov	ecx, edx
	sub	ecx, eax
	mov	edx, ecx
	mov	eax, ebx
	sub	eax, edx
	.loc 1 12 39
	add	eax, 80
	.loc 1 12 16
	mov	WORD PTR -10[ebp], ax
	.loc 1 13 9
	jmp	.L5
.L3:
	.loc 1 15 25
	movzx	ebx, WORD PTR -10[ebp]
	movzx	edx, WORD PTR -10[ebp]
	movsx	eax, dx
	imul	eax, eax, 26215
	shr	eax, 16
	sar	ax, 5
	mov	esi, edx
	sar	si, 15
	sub	eax, esi
	mov	ecx, eax
	mov	eax, ecx
	sal	eax, 2
	add	eax, ecx
	sal	eax, 4
	mov	ecx, edx
	sub	ecx, eax
	mov	edx, ecx
	mov	eax, ebx
	sub	eax, edx
	.loc 1 15 16
	mov	WORD PTR -10[ebp], ax
	.loc 1 16 9
	jmp	.L5
.L6:
	.loc 1 19 22
	movzx	eax, WORD PTR -10[ebp]
	mov	edx, eax
	add	edx, 1
	mov	WORD PTR -10[ebp], dx
	cwde
	.loc 1 19 15
	lea	edx, [eax+eax]
	mov	eax, DWORD PTR -16[ebp]
	add	edx, eax
	.loc 1 19 31
	movzx	eax, BYTE PTR -28[ebp]
	mov	BYTE PTR [edx], al
	.loc 1 20 9
	nop
.L5:
	.loc 1 22 1
	nop
	add	esp, 32
	pop	ebx
	.cfi_restore 3
	pop	esi
	.cfi_restore 6
	pop	ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	putc, .-putc
	.globl	puts
	.type	puts, @function
puts:
.LFB1:
	.loc 1 25 1
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	sub	esp, 24
	call	__x86.get_pc_thunk.ax
	add	eax, OFFSET FLAT:_GLOBAL_OFFSET_TABLE_
	.loc 1 26 9
	mov	DWORD PTR -12[ebp], 0
	.loc 1 26 16
	mov	DWORD PTR -16[ebp], 0
	.loc 1 28 5
	lea	eax, 12[ebp]
	mov	DWORD PTR -20[ebp], eax
	.loc 1 29 11
	jmp	.L8
.L12:
	.loc 1 31 16
	mov	edx, DWORD PTR -12[ebp]
	mov	eax, DWORD PTR 8[ebp]
	add	eax, edx
	movzx	eax, BYTE PTR [eax]
	.loc 1 31 12
	cmp	al, 92
	jne	.L9
	.loc 1 33 24
	mov	eax, DWORD PTR -12[ebp]
	lea	edx, 1[eax]
	mov	eax, DWORD PTR 8[ebp]
	add	eax, edx
	movzx	eax, BYTE PTR [eax]
	movsx	eax, al
	.loc 1 33 13
	cmp	eax, 115
	jne	.L11
	.loc 1 36 30
	mov	eax, DWORD PTR -20[ebp]
	lea	edx, 4[eax]
	mov	DWORD PTR -20[ebp], edx
	mov	eax, DWORD PTR [eax]
	.loc 1 36 30 is_stmt 0 discriminator 1
	sub	esp, 12
	push	eax
	call	puts
	add	esp, 16
	.loc 1 36 27 is_stmt 1 discriminator 2
	add	DWORD PTR -16[ebp], eax
	.loc 1 37 21
	jmp	.L11
.L9:
	.loc 1 42 21
	mov	edx, DWORD PTR -12[ebp]
	mov	eax, DWORD PTR 8[ebp]
	add	eax, edx
	movzx	eax, BYTE PTR [eax]
	.loc 1 42 13
	movsx	eax, al
	sub	esp, 12
	push	eax
	call	putc
	add	esp, 16
	.loc 1 43 13
	add	DWORD PTR -16[ebp], 1
.L11:
	.loc 1 45 9
	add	DWORD PTR -12[ebp], 1
.L8:
	.loc 1 29 15
	mov	edx, DWORD PTR -12[ebp]
	mov	eax, DWORD PTR 8[ebp]
	add	eax, edx
	movzx	eax, BYTE PTR [eax]
	.loc 1 29 19
	test	al, al
	jne	.L12
	.loc 1 47 12
	mov	eax, DWORD PTR -16[ebp]
	.loc 1 48 1
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	puts, .-puts
	.section	.text.__x86.get_pc_thunk.ax,"axG",@progbits,__x86.get_pc_thunk.ax,comdat
	.globl	__x86.get_pc_thunk.ax
	.hidden	__x86.get_pc_thunk.ax
	.type	__x86.get_pc_thunk.ax, @function
__x86.get_pc_thunk.ax:
.LFB2:
	.cfi_startproc
	mov	eax, DWORD PTR [esp]
	ret
	.cfi_endproc
.LFE2:
	.text
.Letext0:
	.file 2 "/home/hu-hangyi/BasicOS/include/btype.h"
	.file 3 "/home/hu-hangyi/BasicOS/include/arg.h"
	.file 4 "/home/hu-hangyi/BasicOS/drivers/include/gpu/vga.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0x16f
	.value	0x5
	.byte	0x1
	.byte	0x4
	.long	.Ldebug_abbrev0
	.uleb128 0x8
	.long	.LASF19
	.byte	0x1d
	.long	.LASF0
	.long	.LASF1
	.long	.Ltext0
	.long	.Letext0-.Ltext0
	.long	.Ldebug_line0
	.uleb128 0x2
	.long	.LASF2
	.byte	0x2
	.byte	0x6
	.byte	0x17
	.long	0x32
	.uleb128 0x1
	.byte	0x1
	.byte	0x8
	.long	.LASF4
	.uleb128 0x2
	.long	.LASF3
	.byte	0x2
	.byte	0x7
	.byte	0x18
	.long	0x45
	.uleb128 0x1
	.byte	0x2
	.byte	0x7
	.long	.LASF5
	.uleb128 0x1
	.byte	0x4
	.byte	0x7
	.long	.LASF6
	.uleb128 0x1
	.byte	0x8
	.byte	0x7
	.long	.LASF7
	.uleb128 0x9
	.byte	0x2
	.byte	0x2
	.byte	0x32
	.byte	0x9
	.long	0x7c
	.uleb128 0x4
	.long	.LASF8
	.byte	0x34
	.byte	0xa
	.long	0x7c
	.byte	0
	.uleb128 0x4
	.long	.LASF9
	.byte	0x35
	.byte	0xb
	.long	0x26
	.byte	0x1
	.byte	0
	.uleb128 0x1
	.byte	0x1
	.byte	0x6
	.long	.LASF10
	.uleb128 0xa
	.long	0x7c
	.uleb128 0x2
	.long	.LASF11
	.byte	0x2
	.byte	0x36
	.byte	0x3
	.long	0x5a
	.uleb128 0x5
	.long	0x88
	.uleb128 0xb
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x2
	.long	.LASF12
	.byte	0x3
	.byte	0x28
	.byte	0x1b
	.long	0xac
	.uleb128 0xc
	.byte	0x4
	.long	.LASF20
	.long	0x7c
	.uleb128 0x2
	.long	.LASF13
	.byte	0x3
	.byte	0x67
	.byte	0x18
	.long	0xa0
	.uleb128 0xd
	.long	.LASF21
	.byte	0x4
	.byte	0x6
	.byte	0x8
	.long	0x39
	.long	0xd4
	.uleb128 0x6
	.byte	0
	.uleb128 0xe
	.long	.LASF22
	.byte	0x1
	.byte	0x18
	.byte	0x5
	.long	0x99
	.long	.LFB1
	.long	.LFE1-.LFB1
	.uleb128 0x1
	.byte	0x9c
	.long	0x127
	.uleb128 0x7
	.string	"str"
	.byte	0x18
	.byte	0x16
	.long	0x127
	.uleb128 0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x6
	.uleb128 0xf
	.string	"i"
	.byte	0x1
	.byte	0x1a
	.byte	0x9
	.long	0x99
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x3
	.long	.LASF14
	.byte	0x1a
	.byte	0x10
	.long	0x99
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x3
	.long	.LASF15
	.byte	0x1b
	.byte	0xd
	.long	0xb6
	.uleb128 0x2
	.byte	0x91
	.sleb128 -28
	.byte	0
	.uleb128 0x5
	.long	0x83
	.uleb128 0x10
	.long	.LASF23
	.byte	0x1
	.byte	0x5
	.byte	0x6
	.long	.LFB0
	.long	.LFE0-.LFB0
	.uleb128 0x1
	.byte	0x9c
	.long	0x16b
	.uleb128 0x7
	.string	"c"
	.byte	0x5
	.byte	0x10
	.long	0x7c
	.uleb128 0x2
	.byte	0x91
	.sleb128 -36
	.uleb128 0x3
	.long	.LASF16
	.byte	0x7
	.byte	0xb
	.long	0x16b
	.uleb128 0x2
	.byte	0x91
	.sleb128 -18
	.uleb128 0x3
	.long	.LASF17
	.byte	0x8
	.byte	0xb
	.long	0x94
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.byte	0
	.uleb128 0x1
	.byte	0x2
	.byte	0x5
	.long	.LASF18
	.byte	0
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 2
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0x21
	.sleb128 4
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0x18
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x1f
	.uleb128 0x1b
	.uleb128 0x1f
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x10
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0x26
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x7c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x7c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_aranges,"",@progbits
	.long	0x1c
	.value	0x2
	.long	.Ldebug_info0
	.byte	0x4
	.byte	0
	.value	0
	.value	0
	.long	.Ltext0
	.long	.Letext0-.Ltext0
	.long	0
	.long	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF8:
	.string	"word"
.LASF21:
	.string	"getCursor"
.LASF20:
	.string	"__builtin_va_list"
.LASF16:
	.string	"cursor"
.LASF14:
	.string	"count"
.LASF12:
	.string	"__gnuc_va_list"
.LASF4:
	.string	"unsigned char"
.LASF11:
	.string	"Word"
.LASF5:
	.string	"short unsigned int"
.LASF19:
	.string	"GNU C17 13.3.0 -m32 -masm=intel -mtune=generic -march=i686 -g -fno-stack-protector -ffreestanding -fasynchronous-unwind-tables -fstack-clash-protection"
.LASF13:
	.string	"va_list"
.LASF9:
	.string	"color"
.LASF6:
	.string	"unsigned int"
.LASF7:
	.string	"long long unsigned int"
.LASF3:
	.string	"UINT16"
.LASF10:
	.string	"char"
.LASF23:
	.string	"putc"
.LASF18:
	.string	"short int"
.LASF15:
	.string	"args"
.LASF22:
	.string	"puts"
.LASF2:
	.string	"UINT8"
.LASF17:
	.string	"screen"
	.section	.debug_line_str,"MS",@progbits,1
.LASF1:
	.string	"/home/hu-hangyi/BasicOS/arch/x86/boot"
.LASF0:
	.string	"video.c"
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
