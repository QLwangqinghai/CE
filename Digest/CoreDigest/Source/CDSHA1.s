	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 10, 14	sdk_version 10, 14
	.globl	_CDSHA1ContextInit      ## -- Begin function CDSHA1ContextInit
	.p2align	4, 0x90
_CDSHA1ContextInit:                     ## @CDSHA1ContextInit
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	cmpq	$0, -8(%rbp)
	setne	%al
	xorb	$-1, %al
	andb	$1, %al
	movzbl	%al, %ecx
	movslq	%ecx, %rdi
	cmpq	$0, %rdi
	je	LBB0_2
## %bb.1:
	leaq	L___func__.CDSHA1ContextInit(%rip), %rdi
	leaq	L_.str(%rip), %rsi
	leaq	L_.str.1(%rip), %rcx
	movl	$99, %edx
	callq	___assert_rtn
LBB0_2:
	jmp	LBB0_3
LBB0_3:
	xorl	%esi, %esi
	movl	$112, %eax
	movl	%eax, %edx
	movq	$-1, %rcx
	movq	-8(%rbp), %rdi
	callq	___memset_chk
	movq	-8(%rbp), %rcx
	movq	$2, (%rcx)
	movq	-8(%rbp), %rcx
	movl	$1732584193, 24(%rcx)   ## imm = 0x67452301
	movq	-8(%rbp), %rcx
	movl	$-271733879, 28(%rcx)   ## imm = 0xEFCDAB89
	movq	-8(%rbp), %rcx
	movl	$-1732584194, 32(%rcx)  ## imm = 0x98BADCFE
	movq	-8(%rbp), %rcx
	movl	$271733878, 36(%rcx)    ## imm = 0x10325476
	movq	-8(%rbp), %rcx
	movl	$-1009589776, 40(%rcx)  ## imm = 0xC3D2E1F0
	movq	%rax, -16(%rbp)         ## 8-byte Spill
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_CDSHA1Update           ## -- Begin function CDSHA1Update
	.p2align	4, 0x90
_CDSHA1Update:                          ## @CDSHA1Update
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$96, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	cmpq	$0, -8(%rbp)
	setne	%al
	xorb	$-1, %al
	andb	$1, %al
	movzbl	%al, %ecx
	movslq	%ecx, %rdx
	cmpq	$0, %rdx
	je	LBB1_2
## %bb.1:
	leaq	L___func__.CDSHA1Update(%rip), %rdi
	leaq	L_.str(%rip), %rsi
	leaq	L_.str.1(%rip), %rcx
	movl	$110, %edx
	callq	___assert_rtn
LBB1_2:
	jmp	LBB1_3
LBB1_3:
	cmpq	$0, -24(%rbp)
	setae	%al
	xorb	$-1, %al
	andb	$1, %al
	movzbl	%al, %ecx
	movslq	%ecx, %rdx
	cmpq	$0, %rdx
	je	LBB1_5
## %bb.4:
	leaq	L___func__.CDSHA1Update(%rip), %rdi
	leaq	L_.str(%rip), %rsi
	leaq	L_.str.2(%rip), %rcx
	movl	$111, %edx
	callq	___assert_rtn
LBB1_5:
	jmp	LBB1_6
LBB1_6:
	xorl	%eax, %eax
	movl	%eax, %ecx
	cmpq	-24(%rbp), %rcx
	jne	LBB1_8
## %bb.7:
	jmp	LBB1_22
LBB1_8:
	cmpq	$0, -16(%rbp)
	setne	%al
	xorb	$-1, %al
	andb	$1, %al
	movzbl	%al, %ecx
	movslq	%ecx, %rdx
	cmpq	$0, %rdx
	je	LBB1_10
## %bb.9:
	leaq	L___func__.CDSHA1Update(%rip), %rdi
	leaq	L_.str(%rip), %rsi
	leaq	L_.str.3(%rip), %rcx
	movl	$116, %edx
	callq	___assert_rtn
LBB1_10:
	jmp	LBB1_11
LBB1_11:
	movq	$64, -32(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -40(%rbp)
	movq	-8(%rbp), %rax
	cmpq	$0, 8(%rax)
	jbe	LBB1_16
## %bb.12:
	movq	-32(%rbp), %rax
	movq	-8(%rbp), %rcx
	subq	8(%rcx), %rax
	movq	%rax, -48(%rbp)
	movq	-24(%rbp), %rax
	cmpq	-48(%rbp), %rax
	jae	LBB1_14
## %bb.13:
	movq	$-1, %rcx
	movq	-8(%rbp), %rax
	addq	$44, %rax
	movq	-40(%rbp), %rsi
	movq	-24(%rbp), %rdx
	movq	%rax, %rdi
	callq	___memcpy_chk
	movq	-24(%rbp), %rcx
	movq	-8(%rbp), %rdx
	addq	8(%rdx), %rcx
	movq	%rcx, 8(%rdx)
	movq	%rax, -72(%rbp)         ## 8-byte Spill
	jmp	LBB1_22
LBB1_14:
	movq	$-1, %rcx
	movq	-8(%rbp), %rax
	addq	$44, %rax
	movq	-40(%rbp), %rsi
	movq	-48(%rbp), %rdx
	movq	%rax, %rdi
	callq	___memcpy_chk
	movq	-8(%rbp), %rcx
	addq	$44, %rcx
	movq	-8(%rbp), %rdx
	addq	$24, %rdx
	movq	%rcx, %rdi
	movq	%rdx, %rsi
	movq	%rax, -80(%rbp)         ## 8-byte Spill
	callq	_CDSHA1Process
	movq	-8(%rbp), %rax
	movq	$0, 8(%rax)
	movq	-48(%rbp), %rax
	addq	-40(%rbp), %rax
	movq	%rax, -40(%rbp)
	movq	-48(%rbp), %rax
	movq	-24(%rbp), %rcx
	subq	%rax, %rcx
	movq	%rcx, -24(%rbp)
	movq	-32(%rbp), %rax
	shlq	$3, %rax
	movq	-8(%rbp), %rcx
	addq	16(%rcx), %rax
	movq	%rax, 16(%rcx)
## %bb.15:
	jmp	LBB1_16
LBB1_16:
	movq	$-8, -56(%rbp)
	movq	-24(%rbp), %rax
	movq	%rax, -64(%rbp)
	movq	-64(%rbp), %rax
	andq	$-8, %rax
	movq	%rax, -64(%rbp)
	movq	-64(%rbp), %rax
	shlq	$3, %rax
	movq	%rax, -64(%rbp)
	movq	-64(%rbp), %rax
	movq	-8(%rbp), %rcx
	addq	16(%rcx), %rax
	movq	%rax, 16(%rcx)
LBB1_17:                                ## =>This Inner Loop Header: Depth=1
	movq	-24(%rbp), %rax
	cmpq	-32(%rbp), %rax
	jb	LBB1_20
## %bb.18:                              ##   in Loop: Header=BB1_17 Depth=1
	movq	-40(%rbp), %rdi
	movq	-8(%rbp), %rax
	addq	$24, %rax
	movq	%rax, %rsi
	callq	_CDSHA1Process
	movq	-32(%rbp), %rax
	addq	-40(%rbp), %rax
	movq	%rax, -40(%rbp)
## %bb.19:                              ##   in Loop: Header=BB1_17 Depth=1
	movq	-32(%rbp), %rax
	movq	-24(%rbp), %rcx
	subq	%rax, %rcx
	movq	%rcx, -24(%rbp)
	jmp	LBB1_17
LBB1_20:
	cmpq	$0, -24(%rbp)
	jbe	LBB1_22
## %bb.21:
	movq	$-1, %rcx
	movq	-8(%rbp), %rax
	addq	$44, %rax
	movq	-40(%rbp), %rsi
	movq	-24(%rbp), %rdx
	movq	%rax, %rdi
	callq	___memcpy_chk
	movq	-24(%rbp), %rcx
	movq	-8(%rbp), %rdx
	addq	8(%rdx), %rcx
	movq	%rcx, 8(%rdx)
	movq	%rax, -88(%rbp)         ## 8-byte Spill
LBB1_22:
	addq	$96, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4, 0x90         ## -- Begin function CDSHA1Process
_CDSHA1Process:                         ## @CDSHA1Process
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$400, %rsp              ## imm = 0x190
	xorl	%eax, %eax
	movl	$320, %ecx              ## imm = 0x140
	movl	%ecx, %edx
	movq	___stack_chk_guard@GOTPCREL(%rip), %r8
	movq	(%r8), %r8
	movq	%r8, -8(%rbp)
	movq	%rdi, -344(%rbp)
	movq	%rsi, -352(%rbp)
	leaq	-336(%rbp), %rsi
	movq	%rsi, %rdi
	movl	%eax, %esi
	callq	_memset
	movq	-344(%rbp), %rdx
	movq	%rdx, -384(%rbp)
	movq	-352(%rbp), %rdx
	movq	%rdx, -392(%rbp)
	movl	$0, -376(%rbp)
LBB2_1:                                 ## =>This Inner Loop Header: Depth=1
	cmpl	$16, -376(%rbp)
	jae	LBB2_4
## %bb.2:                               ##   in Loop: Header=BB2_1 Depth=1
	movq	-384(%rbp), %rdi
	callq	_CUInt32MakeWithBigEndianBytes
	movl	-376(%rbp), %ecx
	movl	%ecx, %edi
	movl	%eax, -336(%rbp,%rdi,4)
	movq	-384(%rbp), %rdi
	addq	$4, %rdi
	movq	%rdi, -384(%rbp)
## %bb.3:                               ##   in Loop: Header=BB2_1 Depth=1
	movl	-376(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -376(%rbp)
	jmp	LBB2_1
LBB2_4:
	movq	-392(%rbp), %rax
	movl	(%rax), %ecx
	movl	%ecx, -356(%rbp)
	movq	-392(%rbp), %rax
	movl	4(%rax), %ecx
	movl	%ecx, -360(%rbp)
	movq	-392(%rbp), %rax
	movl	8(%rax), %ecx
	movl	%ecx, -364(%rbp)
	movq	-392(%rbp), %rax
	movl	12(%rax), %ecx
	movl	%ecx, -368(%rbp)
	movq	-392(%rbp), %rax
	movl	16(%rax), %ecx
	movl	%ecx, -372(%rbp)
	movl	$16, -376(%rbp)
LBB2_5:                                 ## =>This Inner Loop Header: Depth=1
	cmpl	$80, -376(%rbp)
	jae	LBB2_8
## %bb.6:                               ##   in Loop: Header=BB2_5 Depth=1
	movl	-376(%rbp), %eax
	subl	$3, %eax
	movl	%eax, %eax
	movl	%eax, %ecx
	movl	-336(%rbp,%rcx,4), %eax
	movl	-376(%rbp), %edx
	subl	$8, %edx
	movl	%edx, %edx
	movl	%edx, %ecx
	xorl	-336(%rbp,%rcx,4), %eax
	movl	-376(%rbp), %edx
	subl	$14, %edx
	movl	%edx, %edx
	movl	%edx, %ecx
	xorl	-336(%rbp,%rcx,4), %eax
	movl	-376(%rbp), %edx
	subl	$16, %edx
	movl	%edx, %edx
	movl	%edx, %ecx
	xorl	-336(%rbp,%rcx,4), %eax
	movl	%eax, %edi
	movl	$1, %esi
	callq	_CUInt32RotateLeft
	movl	-376(%rbp), %edx
	movl	%edx, %ecx
	movl	%eax, -336(%rbp,%rcx,4)
## %bb.7:                               ##   in Loop: Header=BB2_5 Depth=1
	movl	-376(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -376(%rbp)
	jmp	LBB2_5
LBB2_8:
	movl	$0, -376(%rbp)
LBB2_9:                                 ## =>This Inner Loop Header: Depth=1
	cmpl	$20, -376(%rbp)
	jae	LBB2_11
## %bb.10:                              ##   in Loop: Header=BB2_9 Depth=1
	movl	-356(%rbp), %edi
	movl	$5, %esi
	callq	_CUInt32RotateLeft
	movl	-368(%rbp), %esi
	movl	-360(%rbp), %edi
	movl	-364(%rbp), %ecx
	xorl	-368(%rbp), %ecx
	andl	%ecx, %edi
	xorl	%edi, %esi
	addl	%esi, %eax
	addl	-372(%rbp), %eax
	movl	-376(%rbp), %ecx
	movl	%ecx, %esi
	addl	$1, %esi
	movl	%esi, -376(%rbp)
	movl	%ecx, %ecx
	movl	%ecx, %edx
	addl	-336(%rbp,%rdx,4), %eax
	addl	$1518500249, %eax       ## imm = 0x5A827999
	movl	%eax, -372(%rbp)
	movl	-360(%rbp), %edi
	movl	$30, %esi
	callq	_CUInt32RotateLeft
	movl	%eax, -360(%rbp)
	movl	-372(%rbp), %edi
	movl	$5, %esi
	callq	_CUInt32RotateLeft
	movl	-364(%rbp), %ecx
	movl	-356(%rbp), %esi
	movl	-360(%rbp), %edi
	xorl	-364(%rbp), %edi
	andl	%edi, %esi
	xorl	%esi, %ecx
	addl	%ecx, %eax
	addl	-368(%rbp), %eax
	movl	-376(%rbp), %ecx
	movl	%ecx, %esi
	addl	$1, %esi
	movl	%esi, -376(%rbp)
	movl	%ecx, %ecx
	movl	%ecx, %edx
	addl	-336(%rbp,%rdx,4), %eax
	addl	$1518500249, %eax       ## imm = 0x5A827999
	movl	%eax, -368(%rbp)
	movl	-356(%rbp), %edi
	movl	$30, %esi
	callq	_CUInt32RotateLeft
	movl	%eax, -356(%rbp)
	movl	-368(%rbp), %edi
	movl	$5, %esi
	callq	_CUInt32RotateLeft
	movl	-360(%rbp), %ecx
	movl	-372(%rbp), %esi
	movl	-356(%rbp), %edi
	xorl	-360(%rbp), %edi
	andl	%edi, %esi
	xorl	%esi, %ecx
	addl	%ecx, %eax
	addl	-364(%rbp), %eax
	movl	-376(%rbp), %ecx
	movl	%ecx, %esi
	addl	$1, %esi
	movl	%esi, -376(%rbp)
	movl	%ecx, %ecx
	movl	%ecx, %edx
	addl	-336(%rbp,%rdx,4), %eax
	addl	$1518500249, %eax       ## imm = 0x5A827999
	movl	%eax, -364(%rbp)
	movl	-372(%rbp), %edi
	movl	$30, %esi
	callq	_CUInt32RotateLeft
	movl	%eax, -372(%rbp)
	movl	-364(%rbp), %edi
	movl	$5, %esi
	callq	_CUInt32RotateLeft
	movl	-356(%rbp), %ecx
	movl	-368(%rbp), %esi
	movl	-372(%rbp), %edi
	xorl	-356(%rbp), %edi
	andl	%edi, %esi
	xorl	%esi, %ecx
	addl	%ecx, %eax
	addl	-360(%rbp), %eax
	movl	-376(%rbp), %ecx
	movl	%ecx, %esi
	addl	$1, %esi
	movl	%esi, -376(%rbp)
	movl	%ecx, %ecx
	movl	%ecx, %edx
	addl	-336(%rbp,%rdx,4), %eax
	addl	$1518500249, %eax       ## imm = 0x5A827999
	movl	%eax, -360(%rbp)
	movl	-368(%rbp), %edi
	movl	$30, %esi
	callq	_CUInt32RotateLeft
	movl	%eax, -368(%rbp)
	movl	-360(%rbp), %edi
	movl	$5, %esi
	callq	_CUInt32RotateLeft
	movl	-372(%rbp), %ecx
	movl	-364(%rbp), %esi
	movl	-368(%rbp), %edi
	xorl	-372(%rbp), %edi
	andl	%edi, %esi
	xorl	%esi, %ecx
	addl	%ecx, %eax
	addl	-356(%rbp), %eax
	movl	-376(%rbp), %ecx
	movl	%ecx, %esi
	addl	$1, %esi
	movl	%esi, -376(%rbp)
	movl	%ecx, %ecx
	movl	%ecx, %edx
	addl	-336(%rbp,%rdx,4), %eax
	addl	$1518500249, %eax       ## imm = 0x5A827999
	movl	%eax, -356(%rbp)
	movl	-364(%rbp), %edi
	movl	$30, %esi
	callq	_CUInt32RotateLeft
	movl	%eax, -364(%rbp)
	jmp	LBB2_9
LBB2_11:
	jmp	LBB2_12
LBB2_12:                                ## =>This Inner Loop Header: Depth=1
	cmpl	$40, -376(%rbp)
	jae	LBB2_14
## %bb.13:                              ##   in Loop: Header=BB2_12 Depth=1
	movl	-356(%rbp), %edi
	movl	$5, %esi
	callq	_CUInt32RotateLeft
	movl	-360(%rbp), %esi
	xorl	-364(%rbp), %esi
	xorl	-368(%rbp), %esi
	addl	%esi, %eax
	addl	-372(%rbp), %eax
	movl	-376(%rbp), %esi
	movl	%esi, %edi
	addl	$1, %edi
	movl	%edi, -376(%rbp)
	movl	%esi, %esi
	movl	%esi, %ecx
	addl	-336(%rbp,%rcx,4), %eax
	addl	$1859775393, %eax       ## imm = 0x6ED9EBA1
	movl	%eax, -372(%rbp)
	movl	-360(%rbp), %edi
	movl	$30, %esi
	callq	_CUInt32RotateLeft
	movl	%eax, -360(%rbp)
	movl	-372(%rbp), %edi
	movl	$5, %esi
	callq	_CUInt32RotateLeft
	movl	-356(%rbp), %esi
	xorl	-360(%rbp), %esi
	xorl	-364(%rbp), %esi
	addl	%esi, %eax
	addl	-368(%rbp), %eax
	movl	-376(%rbp), %esi
	movl	%esi, %edi
	addl	$1, %edi
	movl	%edi, -376(%rbp)
	movl	%esi, %esi
	movl	%esi, %ecx
	addl	-336(%rbp,%rcx,4), %eax
	addl	$1859775393, %eax       ## imm = 0x6ED9EBA1
	movl	%eax, -368(%rbp)
	movl	-356(%rbp), %edi
	movl	$30, %esi
	callq	_CUInt32RotateLeft
	movl	%eax, -356(%rbp)
	movl	-368(%rbp), %edi
	movl	$5, %esi
	callq	_CUInt32RotateLeft
	movl	-372(%rbp), %esi
	xorl	-356(%rbp), %esi
	xorl	-360(%rbp), %esi
	addl	%esi, %eax
	addl	-364(%rbp), %eax
	movl	-376(%rbp), %esi
	movl	%esi, %edi
	addl	$1, %edi
	movl	%edi, -376(%rbp)
	movl	%esi, %esi
	movl	%esi, %ecx
	addl	-336(%rbp,%rcx,4), %eax
	addl	$1859775393, %eax       ## imm = 0x6ED9EBA1
	movl	%eax, -364(%rbp)
	movl	-372(%rbp), %edi
	movl	$30, %esi
	callq	_CUInt32RotateLeft
	movl	%eax, -372(%rbp)
	movl	-364(%rbp), %edi
	movl	$5, %esi
	callq	_CUInt32RotateLeft
	movl	-368(%rbp), %esi
	xorl	-372(%rbp), %esi
	xorl	-356(%rbp), %esi
	addl	%esi, %eax
	addl	-360(%rbp), %eax
	movl	-376(%rbp), %esi
	movl	%esi, %edi
	addl	$1, %edi
	movl	%edi, -376(%rbp)
	movl	%esi, %esi
	movl	%esi, %ecx
	addl	-336(%rbp,%rcx,4), %eax
	addl	$1859775393, %eax       ## imm = 0x6ED9EBA1
	movl	%eax, -360(%rbp)
	movl	-368(%rbp), %edi
	movl	$30, %esi
	callq	_CUInt32RotateLeft
	movl	%eax, -368(%rbp)
	movl	-360(%rbp), %edi
	movl	$5, %esi
	callq	_CUInt32RotateLeft
	movl	-364(%rbp), %esi
	xorl	-368(%rbp), %esi
	xorl	-372(%rbp), %esi
	addl	%esi, %eax
	addl	-356(%rbp), %eax
	movl	-376(%rbp), %esi
	movl	%esi, %edi
	addl	$1, %edi
	movl	%edi, -376(%rbp)
	movl	%esi, %esi
	movl	%esi, %ecx
	addl	-336(%rbp,%rcx,4), %eax
	addl	$1859775393, %eax       ## imm = 0x6ED9EBA1
	movl	%eax, -356(%rbp)
	movl	-364(%rbp), %edi
	movl	$30, %esi
	callq	_CUInt32RotateLeft
	movl	%eax, -364(%rbp)
	jmp	LBB2_12
LBB2_14:
	jmp	LBB2_15
LBB2_15:                                ## =>This Inner Loop Header: Depth=1
	cmpl	$60, -376(%rbp)
	jae	LBB2_17
## %bb.16:                              ##   in Loop: Header=BB2_15 Depth=1
	movl	-356(%rbp), %edi
	movl	$5, %esi
	callq	_CUInt32RotateLeft
	movl	-360(%rbp), %esi
	andl	-364(%rbp), %esi
	movl	-368(%rbp), %edi
	movl	-360(%rbp), %ecx
	orl	-364(%rbp), %ecx
	andl	%ecx, %edi
	orl	%edi, %esi
	addl	%esi, %eax
	addl	-372(%rbp), %eax
	movl	-376(%rbp), %ecx
	movl	%ecx, %esi
	addl	$1, %esi
	movl	%esi, -376(%rbp)
	movl	%ecx, %ecx
	movl	%ecx, %edx
	addl	-336(%rbp,%rdx,4), %eax
	addl	$-1894007588, %eax      ## imm = 0x8F1BBCDC
	movl	%eax, -372(%rbp)
	movl	-360(%rbp), %edi
	movl	$30, %esi
	callq	_CUInt32RotateLeft
	movl	%eax, -360(%rbp)
	movl	-372(%rbp), %edi
	movl	$5, %esi
	callq	_CUInt32RotateLeft
	movl	-356(%rbp), %ecx
	andl	-360(%rbp), %ecx
	movl	-364(%rbp), %esi
	movl	-356(%rbp), %edi
	orl	-360(%rbp), %edi
	andl	%edi, %esi
	orl	%esi, %ecx
	addl	%ecx, %eax
	addl	-368(%rbp), %eax
	movl	-376(%rbp), %ecx
	movl	%ecx, %esi
	addl	$1, %esi
	movl	%esi, -376(%rbp)
	movl	%ecx, %ecx
	movl	%ecx, %edx
	addl	-336(%rbp,%rdx,4), %eax
	addl	$-1894007588, %eax      ## imm = 0x8F1BBCDC
	movl	%eax, -368(%rbp)
	movl	-356(%rbp), %edi
	movl	$30, %esi
	callq	_CUInt32RotateLeft
	movl	%eax, -356(%rbp)
	movl	-368(%rbp), %edi
	movl	$5, %esi
	callq	_CUInt32RotateLeft
	movl	-372(%rbp), %ecx
	andl	-356(%rbp), %ecx
	movl	-360(%rbp), %esi
	movl	-372(%rbp), %edi
	orl	-356(%rbp), %edi
	andl	%edi, %esi
	orl	%esi, %ecx
	addl	%ecx, %eax
	addl	-364(%rbp), %eax
	movl	-376(%rbp), %ecx
	movl	%ecx, %esi
	addl	$1, %esi
	movl	%esi, -376(%rbp)
	movl	%ecx, %ecx
	movl	%ecx, %edx
	addl	-336(%rbp,%rdx,4), %eax
	addl	$-1894007588, %eax      ## imm = 0x8F1BBCDC
	movl	%eax, -364(%rbp)
	movl	-372(%rbp), %edi
	movl	$30, %esi
	callq	_CUInt32RotateLeft
	movl	%eax, -372(%rbp)
	movl	-364(%rbp), %edi
	movl	$5, %esi
	callq	_CUInt32RotateLeft
	movl	-368(%rbp), %ecx
	andl	-372(%rbp), %ecx
	movl	-356(%rbp), %esi
	movl	-368(%rbp), %edi
	orl	-372(%rbp), %edi
	andl	%edi, %esi
	orl	%esi, %ecx
	addl	%ecx, %eax
	addl	-360(%rbp), %eax
	movl	-376(%rbp), %ecx
	movl	%ecx, %esi
	addl	$1, %esi
	movl	%esi, -376(%rbp)
	movl	%ecx, %ecx
	movl	%ecx, %edx
	addl	-336(%rbp,%rdx,4), %eax
	addl	$-1894007588, %eax      ## imm = 0x8F1BBCDC
	movl	%eax, -360(%rbp)
	movl	-368(%rbp), %edi
	movl	$30, %esi
	callq	_CUInt32RotateLeft
	movl	%eax, -368(%rbp)
	movl	-360(%rbp), %edi
	movl	$5, %esi
	callq	_CUInt32RotateLeft
	movl	-364(%rbp), %ecx
	andl	-368(%rbp), %ecx
	movl	-372(%rbp), %esi
	movl	-364(%rbp), %edi
	orl	-368(%rbp), %edi
	andl	%edi, %esi
	orl	%esi, %ecx
	addl	%ecx, %eax
	addl	-356(%rbp), %eax
	movl	-376(%rbp), %ecx
	movl	%ecx, %esi
	addl	$1, %esi
	movl	%esi, -376(%rbp)
	movl	%ecx, %ecx
	movl	%ecx, %edx
	addl	-336(%rbp,%rdx,4), %eax
	addl	$-1894007588, %eax      ## imm = 0x8F1BBCDC
	movl	%eax, -356(%rbp)
	movl	-364(%rbp), %edi
	movl	$30, %esi
	callq	_CUInt32RotateLeft
	movl	%eax, -364(%rbp)
	jmp	LBB2_15
LBB2_17:
	jmp	LBB2_18
LBB2_18:                                ## =>This Inner Loop Header: Depth=1
	cmpl	$80, -376(%rbp)
	jae	LBB2_20
## %bb.19:                              ##   in Loop: Header=BB2_18 Depth=1
	movl	-356(%rbp), %edi
	movl	$5, %esi
	callq	_CUInt32RotateLeft
	movl	-360(%rbp), %esi
	xorl	-364(%rbp), %esi
	xorl	-368(%rbp), %esi
	addl	%esi, %eax
	addl	-372(%rbp), %eax
	movl	-376(%rbp), %esi
	movl	%esi, %edi
	addl	$1, %edi
	movl	%edi, -376(%rbp)
	movl	%esi, %esi
	movl	%esi, %ecx
	addl	-336(%rbp,%rcx,4), %eax
	addl	$-899497514, %eax       ## imm = 0xCA62C1D6
	movl	%eax, -372(%rbp)
	movl	-360(%rbp), %edi
	movl	$30, %esi
	callq	_CUInt32RotateLeft
	movl	%eax, -360(%rbp)
	movl	-372(%rbp), %edi
	movl	$5, %esi
	callq	_CUInt32RotateLeft
	movl	-356(%rbp), %esi
	xorl	-360(%rbp), %esi
	xorl	-364(%rbp), %esi
	addl	%esi, %eax
	addl	-368(%rbp), %eax
	movl	-376(%rbp), %esi
	movl	%esi, %edi
	addl	$1, %edi
	movl	%edi, -376(%rbp)
	movl	%esi, %esi
	movl	%esi, %ecx
	addl	-336(%rbp,%rcx,4), %eax
	addl	$-899497514, %eax       ## imm = 0xCA62C1D6
	movl	%eax, -368(%rbp)
	movl	-356(%rbp), %edi
	movl	$30, %esi
	callq	_CUInt32RotateLeft
	movl	%eax, -356(%rbp)
	movl	-368(%rbp), %edi
	movl	$5, %esi
	callq	_CUInt32RotateLeft
	movl	-372(%rbp), %esi
	xorl	-356(%rbp), %esi
	xorl	-360(%rbp), %esi
	addl	%esi, %eax
	addl	-364(%rbp), %eax
	movl	-376(%rbp), %esi
	movl	%esi, %edi
	addl	$1, %edi
	movl	%edi, -376(%rbp)
	movl	%esi, %esi
	movl	%esi, %ecx
	addl	-336(%rbp,%rcx,4), %eax
	addl	$-899497514, %eax       ## imm = 0xCA62C1D6
	movl	%eax, -364(%rbp)
	movl	-372(%rbp), %edi
	movl	$30, %esi
	callq	_CUInt32RotateLeft
	movl	%eax, -372(%rbp)
	movl	-364(%rbp), %edi
	movl	$5, %esi
	callq	_CUInt32RotateLeft
	movl	-368(%rbp), %esi
	xorl	-372(%rbp), %esi
	xorl	-356(%rbp), %esi
	addl	%esi, %eax
	addl	-360(%rbp), %eax
	movl	-376(%rbp), %esi
	movl	%esi, %edi
	addl	$1, %edi
	movl	%edi, -376(%rbp)
	movl	%esi, %esi
	movl	%esi, %ecx
	addl	-336(%rbp,%rcx,4), %eax
	addl	$-899497514, %eax       ## imm = 0xCA62C1D6
	movl	%eax, -360(%rbp)
	movl	-368(%rbp), %edi
	movl	$30, %esi
	callq	_CUInt32RotateLeft
	movl	%eax, -368(%rbp)
	movl	-360(%rbp), %edi
	movl	$5, %esi
	callq	_CUInt32RotateLeft
	movl	-364(%rbp), %esi
	xorl	-368(%rbp), %esi
	xorl	-372(%rbp), %esi
	addl	%esi, %eax
	addl	-356(%rbp), %eax
	movl	-376(%rbp), %esi
	movl	%esi, %edi
	addl	$1, %edi
	movl	%edi, -376(%rbp)
	movl	%esi, %esi
	movl	%esi, %ecx
	addl	-336(%rbp,%rcx,4), %eax
	addl	$-899497514, %eax       ## imm = 0xCA62C1D6
	movl	%eax, -356(%rbp)
	movl	-364(%rbp), %edi
	movl	$30, %esi
	callq	_CUInt32RotateLeft
	movl	%eax, -364(%rbp)
	jmp	LBB2_18
LBB2_20:
	movl	-356(%rbp), %eax
	movq	-392(%rbp), %rcx
	addl	(%rcx), %eax
	movl	%eax, (%rcx)
	movl	-360(%rbp), %eax
	movq	-392(%rbp), %rcx
	addl	4(%rcx), %eax
	movl	%eax, 4(%rcx)
	movl	-364(%rbp), %eax
	movq	-392(%rbp), %rcx
	addl	8(%rcx), %eax
	movl	%eax, 8(%rcx)
	movl	-368(%rbp), %eax
	movq	-392(%rbp), %rcx
	addl	12(%rcx), %eax
	movl	%eax, 12(%rcx)
	movl	-372(%rbp), %eax
	movq	-392(%rbp), %rcx
	addl	16(%rcx), %eax
	movl	%eax, 16(%rcx)
	movq	___stack_chk_guard@GOTPCREL(%rip), %rcx
	movq	(%rcx), %rcx
	movq	-8(%rbp), %rdx
	cmpq	%rdx, %rcx
	jne	LBB2_22
## %bb.21:
	addq	$400, %rsp              ## imm = 0x190
	popq	%rbp
	retq
LBB2_22:
	callq	___stack_chk_fail
	ud2
	.cfi_endproc
                                        ## -- End function
	.globl	_CDSHA1Final            ## -- Begin function CDSHA1Final
	.p2align	4, 0x90
_CDSHA1Final:                           ## @CDSHA1Final
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$208, %rsp
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	%rax, -8(%rbp)
	movq	%rdi, -152(%rbp)
	cmpq	$0, -152(%rbp)
	setne	%cl
	xorb	$-1, %cl
	andb	$1, %cl
	movzbl	%cl, %edx
	movslq	%edx, %rax
	cmpq	$0, %rax
	je	LBB3_2
## %bb.1:
	leaq	L___func__.CDSHA1Final(%rip), %rdi
	leaq	L_.str(%rip), %rsi
	leaq	L_.str.1(%rip), %rcx
	movl	$153, %edx
	callq	___assert_rtn
LBB3_2:
	jmp	LBB3_3
LBB3_3:
	leaq	-144(%rbp), %rax
	xorl	%esi, %esi
	movl	$128, %ecx
	movl	%ecx, %edx
	movq	$64, -160(%rbp)
	movq	-152(%rbp), %rdi
	movq	8(%rdi), %rdi
	movq	%rdi, -168(%rbp)
	movq	%rax, %rdi
	movq	%rax, -192(%rbp)        ## 8-byte Spill
	callq	_memset
	movq	-192(%rbp), %rax        ## 8-byte Reload
	movq	%rax, -176(%rbp)
	movq	-152(%rbp), %rdx
	cmpq	$0, 8(%rdx)
	jbe	LBB3_5
## %bb.4:
	movl	$128, %eax
	movl	%eax, %ecx
	leaq	-144(%rbp), %rdi
	movq	-152(%rbp), %rdx
	addq	$44, %rdx
	movq	-152(%rbp), %rsi
	movq	8(%rsi), %rsi
	movq	%rsi, -200(%rbp)        ## 8-byte Spill
	movq	%rdx, %rsi
	movq	-200(%rbp), %rdx        ## 8-byte Reload
	callq	___memcpy_chk
	movq	-152(%rbp), %rcx
	movq	8(%rcx), %rcx
	addq	-176(%rbp), %rcx
	movq	%rcx, -176(%rbp)
	movq	-152(%rbp), %rcx
	movq	$0, 8(%rcx)
	movq	%rax, -208(%rbp)        ## 8-byte Spill
LBB3_5:
	movq	-152(%rbp), %rax
	movq	8(%rax), %rax
	shlq	$3, %rax
	movq	-152(%rbp), %rcx
	addq	16(%rcx), %rax
	movq	%rax, 16(%rcx)
	movq	-176(%rbp), %rax
	movb	$-128, (%rax)
	movq	-176(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -176(%rbp)
	movq	-168(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -168(%rbp)
	movq	-160(%rbp), %rax
	movq	-160(%rbp), %rcx
	shrq	$3, %rcx
	subq	%rcx, %rax
	movq	%rax, -184(%rbp)
	movq	-168(%rbp), %rax
	xorl	%edx, %edx
                                        ## kill: def $rdx killed $edx
	divq	-160(%rbp)
	cmpq	-184(%rbp), %rdx
	jae	LBB3_7
## %bb.6:
	leaq	-144(%rbp), %rax
	addq	-184(%rbp), %rax
	movq	%rax, -176(%rbp)
	movq	-152(%rbp), %rax
	movq	16(%rax), %rdi
	movq	-176(%rbp), %rsi
	callq	_CUInt64ToBigEndianBytes
	leaq	-144(%rbp), %rdi
	movq	-152(%rbp), %rax
	addq	$24, %rax
	movq	%rax, %rsi
	callq	_CDSHA1Process
	jmp	LBB3_8
LBB3_7:
	leaq	-144(%rbp), %rax
	addq	-160(%rbp), %rax
	addq	-184(%rbp), %rax
	movq	%rax, -176(%rbp)
	movq	-152(%rbp), %rax
	movq	16(%rax), %rdi
	movq	-176(%rbp), %rsi
	callq	_CUInt64ToBigEndianBytes
	leaq	-144(%rbp), %rdi
	movq	-152(%rbp), %rax
	addq	$24, %rax
	movq	%rax, %rsi
	callq	_CDSHA1Process
	leaq	-144(%rbp), %rax
	addq	-160(%rbp), %rax
	movq	-152(%rbp), %rsi
	addq	$24, %rsi
	movq	%rax, %rdi
	callq	_CDSHA1Process
LBB3_8:
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	-8(%rbp), %rcx
	cmpq	%rcx, %rax
	jne	LBB3_10
## %bb.9:
	addq	$208, %rsp
	popq	%rbp
	retq
LBB3_10:
	callq	___stack_chk_fail
	ud2
	.cfi_endproc
                                        ## -- End function
	.p2align	4, 0x90         ## -- Begin function CUInt64ToBigEndianBytes
_CUInt64ToBigEndianBytes:               ## @CUInt64ToBigEndianBytes
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rdi
	callq	_CUInt64ByteSwap
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rax
	movq	-16(%rbp), %rsi
	movq	%rax, (%rsi)
	addq	$32, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_CDSHA1ExportHashValue  ## -- Begin function CDSHA1ExportHashValue
	.p2align	4, 0x90
_CDSHA1ExportHashValue:                 ## @CDSHA1ExportHashValue
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	cmpq	$0, -8(%rbp)
	setne	%al
	xorb	$-1, %al
	andb	$1, %al
	movzbl	%al, %ecx
	movslq	%ecx, %rsi
	cmpq	$0, %rsi
	je	LBB5_2
## %bb.1:
	leaq	L___func__.CDSHA1ExportHashValue(%rip), %rdi
	leaq	L_.str(%rip), %rsi
	leaq	L_.str.1(%rip), %rcx
	movl	$190, %edx
	callq	___assert_rtn
LBB5_2:
	jmp	LBB5_3
LBB5_3:
	cmpq	$0, -16(%rbp)
	setne	%al
	xorb	$-1, %al
	andb	$1, %al
	movzbl	%al, %ecx
	movslq	%ecx, %rdx
	cmpq	$0, %rdx
	je	LBB5_5
## %bb.4:
	leaq	L___func__.CDSHA1ExportHashValue(%rip), %rdi
	leaq	L_.str(%rip), %rsi
	leaq	L_.str.3(%rip), %rcx
	movl	$191, %edx
	callq	___assert_rtn
LBB5_5:
	jmp	LBB5_6
LBB5_6:
	movq	-16(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	$0, -32(%rbp)
LBB5_7:                                 ## =>This Inner Loop Header: Depth=1
	cmpq	$5, -32(%rbp)
	jae	LBB5_10
## %bb.8:                               ##   in Loop: Header=BB5_7 Depth=1
	movq	-8(%rbp), %rax
	movq	-32(%rbp), %rcx
	movl	24(%rax,%rcx,4), %edx
	movl	%edx, -36(%rbp)
	movl	-36(%rbp), %edi
	movq	-24(%rbp), %rsi
	callq	_CUInt32ToBigEndianBytes
	movq	-24(%rbp), %rax
	addq	$4, %rax
	movq	%rax, -24(%rbp)
## %bb.9:                               ##   in Loop: Header=BB5_7 Depth=1
	movq	-32(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -32(%rbp)
	jmp	LBB5_7
LBB5_10:
	addq	$48, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4, 0x90         ## -- Begin function CUInt32ToBigEndianBytes
_CUInt32ToBigEndianBytes:               ## @CUInt32ToBigEndianBytes
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movl	-4(%rbp), %edi
	callq	_CUInt32ByteSwap
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %eax
	movq	-16(%rbp), %rsi
	movl	%eax, (%rsi)
	addq	$32, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4, 0x90         ## -- Begin function CUInt32MakeWithBigEndianBytes
_CUInt32MakeWithBigEndianBytes:         ## @CUInt32MakeWithBigEndianBytes
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdi
	movl	(%rdi), %eax
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %edi
	callq	_CUInt32ByteSwap
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4, 0x90         ## -- Begin function CUInt32RotateLeft
_CUInt32RotateLeft:                     ## @CUInt32RotateLeft
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %esi
	movl	-8(%rbp), %ecx
	## InlineAsm Start
	roll	%cl, %esi
	## InlineAsm End
	movl	%esi, -4(%rbp)
	movl	-4(%rbp), %eax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4, 0x90         ## -- Begin function CUInt32ByteSwap
_CUInt32ByteSwap:                       ## @CUInt32ByteSwap
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %edi
	bswapl	%edi
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4, 0x90         ## -- Begin function CUInt64ByteSwap
_CUInt64ByteSwap:                       ## @CUInt64ByteSwap
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdi
	bswapq	%rdi
	movq	%rdi, %rax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__cstring,cstring_literals
L___func__.CDSHA1ContextInit:           ## @__func__.CDSHA1ContextInit
	.asciz	"CDSHA1ContextInit"

L_.str:                                 ## @.str
	.asciz	"CDSHA1.c"

L_.str.1:                               ## @.str.1
	.asciz	"context"

L___func__.CDSHA1Update:                ## @__func__.CDSHA1Update
	.asciz	"CDSHA1Update"

L_.str.2:                               ## @.str.2
	.asciz	"length >= 0"

L_.str.3:                               ## @.str.3
	.asciz	"bytes"

L___func__.CDSHA1Final:                 ## @__func__.CDSHA1Final
	.asciz	"CDSHA1Final"

L___func__.CDSHA1ExportHashValue:       ## @__func__.CDSHA1ExportHashValue
	.asciz	"CDSHA1ExportHashValue"


.subsections_via_symbols
