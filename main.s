	.file	"main.c"
	.text
	.def	time;	.scl	3;	.type	32;	.endef
	.seh_proc	time
time:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movq	16(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp__time64(%rip), %rax
	call	*%rax
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	grid
	.bss
	.align 32
grid:
	.space 36
	.globl	currentPlayerTurn
	.align 4
currentPlayerTurn:
	.space 4
	.globl	gameOver
gameOver:
	.space 1
	.globl	winner
	.align 4
winner:
	.space 4
	.globl	gameState
	.align 4
gameState:
	.space 4
	.globl	isTwoPlayer
isTwoPlayer:
	.space 1
	.def	__main;	.scl	2;	.type	32;	.endef
	.section .rdata,"dr"
.LC0:
	.ascii "Tic-Tac-Toe\0"
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$56, %rsp
	.seh_stackalloc	56
	leaq	48(%rsp), %rbp
	.seh_setframe	%rbp, 48
	.seh_endprologue
	call	__main
	leaq	.LC0(%rip), %r8
	movl	$600, %edx
	movl	$600, %ecx
	call	InitWindow
	jmp	.L4
.L24:
	movl	gameState(%rip), %eax
	testl	%eax, %eax
	jne	.L5
	movl	$0, %ecx
	call	IsMouseButtonPressed
	testb	%al, %al
	je	.L6
	call	GetMousePosition
	movq	%rax, -8(%rbp)
	movss	-8(%rbp), %xmm0
	comiss	.LC1(%rip), %xmm0
	jbe	.L6
	movss	-8(%rbp), %xmm1
	movss	.LC2(%rip), %xmm0
	comiss	%xmm1, %xmm0
	jbe	.L6
	movss	-4(%rbp), %xmm0
	comiss	.LC3(%rip), %xmm0
	jbe	.L10
	movss	-4(%rbp), %xmm1
	movss	.LC4(%rip), %xmm0
	comiss	%xmm1, %xmm0
	jbe	.L10
	movb	$0, isTwoPlayer(%rip)
	movl	$1, gameState(%rip)
	call	InitGame
	jmp	.L6
.L10:
	movss	-4(%rbp), %xmm0
	comiss	.LC5(%rip), %xmm0
	jbe	.L6
	movss	-4(%rbp), %xmm1
	movss	.LC6(%rip), %xmm0
	comiss	%xmm1, %xmm0
	jbe	.L6
	movb	$1, isTwoPlayer(%rip)
	movl	$1, gameState(%rip)
	call	InitGame
	jmp	.L6
.L5:
	movl	gameState(%rip), %eax
	cmpl	$1, %eax
	jne	.L15
	call	UpdateGame
	jmp	.L6
.L15:
	movl	gameState(%rip), %eax
	cmpl	$2, %eax
	jne	.L6
	movl	$0, %ecx
	call	IsMouseButtonPressed
	testb	%al, %al
	je	.L6
	call	GetMousePosition
	movq	%rax, -16(%rbp)
	movss	-16(%rbp), %xmm0
	comiss	.LC1(%rip), %xmm0
	jbe	.L6
	movss	-16(%rbp), %xmm1
	movss	.LC2(%rip), %xmm0
	comiss	%xmm1, %xmm0
	jbe	.L6
	movss	-12(%rbp), %xmm0
	comiss	.LC7(%rip), %xmm0
	jbe	.L6
	movss	-12(%rbp), %xmm1
	movss	.LC8(%rip), %xmm0
	comiss	%xmm1, %xmm0
	jbe	.L6
	movl	$0, gameState(%rip)
.L6:
	call	BeginDrawing
	movb	$-11, %bl
	movl	$-11, %eax
	movb	%al, %bh
	movl	%ebx, %eax
	andl	$-16711681, %eax
	orl	$16056320, %eax
	movl	%eax, %ebx
	movl	%ebx, %eax
	orl	$-16777216, %eax
	movl	%eax, %ebx
	movl	%ebx, %ecx
	call	ClearBackground
	movl	gameState(%rip), %eax
	testl	%eax, %eax
	jne	.L21
	call	DrawMenu
	jmp	.L22
.L21:
	movl	gameState(%rip), %eax
	cmpl	$1, %eax
	jne	.L23
	call	DrawGame
	jmp	.L22
.L23:
	movl	gameState(%rip), %eax
	cmpl	$2, %eax
	jne	.L22
	call	DrawGame
	call	DrawGameOver
.L22:
	call	EndDrawing
.L4:
	call	WindowShouldClose
	xorl	$1, %eax
	testb	%al, %al
	jne	.L24
	call	CloseWindow
	movl	$0, %eax
	addq	$56, %rsp
	popq	%rbx
	popq	%rbp
	ret
	.seh_endproc
	.globl	UpdateGame
	.def	UpdateGame;	.scl	2;	.type	32;	.endef
	.seh_proc	UpdateGame
UpdateGame:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movzbl	gameOver(%rip), %eax
	testb	%al, %al
	jne	.L41
	movl	currentPlayerTurn(%rip), %eax
	testl	%eax, %eax
	jne	.L39
	call	HandlePlayerTurn
	jmp	.L36
.L39:
	movl	currentPlayerTurn(%rip), %eax
	cmpl	$1, %eax
	jne	.L36
	movzbl	isTwoPlayer(%rip), %eax
	testb	%al, %al
	je	.L40
	call	HandlePlayerTurn
	jmp	.L36
.L40:
	call	AITurn
	jmp	.L36
.L41:
	nop
.L36:
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	HandlePlayerTurn
	.def	HandlePlayerTurn;	.scl	2;	.type	32;	.endef
	.seh_proc	HandlePlayerTurn
HandlePlayerTurn:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movl	$0, %ecx
	call	IsMouseButtonPressed
	testb	%al, %al
	je	.L50
	call	GetMousePosition
	movq	%rax, -16(%rbp)
	movss	-12(%rbp), %xmm0
	movss	.LC1(%rip), %xmm1
	divss	%xmm1, %xmm0
	cvttss2sil	%xmm0, %eax
	movl	%eax, -4(%rbp)
	movss	-16(%rbp), %xmm0
	movss	.LC1(%rip), %xmm1
	divss	%xmm1, %xmm0
	cvttss2sil	%xmm0, %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	movslq	%eax, %rcx
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	grid(%rip), %rax
	movl	(%rdx,%rax), %eax
	testl	%eax, %eax
	jne	.L50
	movl	currentPlayerTurn(%rip), %eax
	testl	%eax, %eax
	jne	.L45
	movl	-8(%rbp), %eax
	movslq	%eax, %rcx
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	grid(%rip), %rax
	movl	$1, (%rdx,%rax)
	movl	$1, %ecx
	call	CheckWin
	testb	%al, %al
	je	.L46
	movb	$1, gameOver(%rip)
	movl	$1, winner(%rip)
	movl	$2, gameState(%rip)
	jmp	.L50
.L46:
	call	CheckDraw
	testb	%al, %al
	je	.L47
	movb	$1, gameOver(%rip)
	movl	$2, gameState(%rip)
	jmp	.L50
.L47:
	movl	$1, currentPlayerTurn(%rip)
	jmp	.L50
.L45:
	movl	currentPlayerTurn(%rip), %eax
	cmpl	$1, %eax
	jne	.L50
	movl	-8(%rbp), %eax
	movslq	%eax, %rcx
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	grid(%rip), %rax
	movl	$2, (%rdx,%rax)
	movl	$2, %ecx
	call	CheckWin
	testb	%al, %al
	je	.L48
	movb	$1, gameOver(%rip)
	movl	$2, winner(%rip)
	movl	$2, gameState(%rip)
	jmp	.L50
.L48:
	call	CheckDraw
	testb	%al, %al
	je	.L49
	movb	$1, gameOver(%rip)
	movl	$2, gameState(%rip)
	jmp	.L50
.L49:
	movl	$0, currentPlayerTurn(%rip)
.L50:
	nop
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	AITurn
	.def	AITurn;	.scl	2;	.type	32;	.endef
	.seh_proc	AITurn
AITurn:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$64, %rsp
	.seh_stackalloc	64
	.seh_endprologue
	movl	$-1000, -4(%rbp)
	movl	$-1, -8(%rbp)
	movl	$-1, -12(%rbp)
	movl	$0, -16(%rbp)
	jmp	.L52
.L56:
	movl	$0, -20(%rbp)
	jmp	.L53
.L55:
	movl	-20(%rbp), %eax
	movslq	%eax, %rcx
	movl	-16(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	grid(%rip), %rax
	movl	(%rdx,%rax), %eax
	testl	%eax, %eax
	jne	.L54
	movl	-20(%rbp), %eax
	movslq	%eax, %rcx
	movl	-16(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	grid(%rip), %rax
	movl	$2, (%rdx,%rax)
	movl	$0, %edx
	leaq	grid(%rip), %rax
	movq	%rax, %rcx
	call	Minimax
	movl	%eax, -24(%rbp)
	movl	-20(%rbp), %eax
	movslq	%eax, %rcx
	movl	-16(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	grid(%rip), %rax
	movl	$0, (%rdx,%rax)
	movl	-24(%rbp), %eax
	cmpl	-4(%rbp), %eax
	jle	.L54
	movl	-24(%rbp), %eax
	movl	%eax, -4(%rbp)
	movl	-16(%rbp), %eax
	movl	%eax, -8(%rbp)
	movl	-20(%rbp), %eax
	movl	%eax, -12(%rbp)
.L54:
	addl	$1, -20(%rbp)
.L53:
	cmpl	$2, -20(%rbp)
	jle	.L55
	addl	$1, -16(%rbp)
.L52:
	cmpl	$2, -16(%rbp)
	jle	.L56
	movl	-12(%rbp), %eax
	movslq	%eax, %rcx
	movl	-8(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	grid(%rip), %rax
	movl	$2, (%rdx,%rax)
	movl	$2, %ecx
	call	CheckWin
	testb	%al, %al
	je	.L57
	movb	$1, gameOver(%rip)
	movl	$2, winner(%rip)
	movl	$2, gameState(%rip)
	jmp	.L60
.L57:
	call	CheckDraw
	testb	%al, %al
	je	.L59
	movb	$1, gameOver(%rip)
	movl	$2, gameState(%rip)
	jmp	.L60
.L59:
	movl	$0, currentPlayerTurn(%rip)
.L60:
	nop
	addq	$64, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	Minimax
	.def	Minimax;	.scl	2;	.type	32;	.endef
	.seh_proc	Minimax
Minimax:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$64, %rsp
	.seh_stackalloc	64
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	%edx, %eax
	movb	%al, 24(%rbp)
	movl	$2, %ecx
	call	CheckWin
	testb	%al, %al
	je	.L62
	movl	$1, %eax
	jmp	.L63
.L62:
	movl	$1, %ecx
	call	CheckWin
	testb	%al, %al
	je	.L64
	movl	$-1, %eax
	jmp	.L63
.L64:
	call	CheckDraw
	testb	%al, %al
	je	.L65
	movl	$0, %eax
	jmp	.L63
.L65:
	cmpb	$0, 24(%rbp)
	je	.L66
	movl	$-1000, -4(%rbp)
	movl	$0, -8(%rbp)
	jmp	.L67
.L71:
	movl	$0, -12(%rbp)
	jmp	.L68
.L70:
	movl	-8(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	movq	%rax, %rdx
	movq	16(%rbp), %rax
	addq	%rax, %rdx
	movl	-12(%rbp), %eax
	cltq
	movl	(%rdx,%rax,4), %eax
	testl	%eax, %eax
	jne	.L69
	movl	-8(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	movq	%rax, %rdx
	movq	16(%rbp), %rax
	addq	%rax, %rdx
	movl	-12(%rbp), %eax
	cltq
	movl	$2, (%rdx,%rax,4)
	movq	16(%rbp), %rax
	movl	$0, %edx
	movq	%rax, %rcx
	call	Minimax
	movl	%eax, -32(%rbp)
	movl	-8(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	movq	%rax, %rdx
	movq	16(%rbp), %rax
	addq	%rax, %rdx
	movl	-12(%rbp), %eax
	cltq
	movl	$0, (%rdx,%rax,4)
	movl	-4(%rbp), %edx
	movl	-32(%rbp), %eax
	cmpl	%eax, %edx
	cmovge	%edx, %eax
	movl	%eax, -4(%rbp)
.L69:
	addl	$1, -12(%rbp)
.L68:
	cmpl	$2, -12(%rbp)
	jle	.L70
	addl	$1, -8(%rbp)
.L67:
	cmpl	$2, -8(%rbp)
	jle	.L71
	movl	-4(%rbp), %eax
	jmp	.L63
.L66:
	movl	$1000, -16(%rbp)
	movl	$0, -20(%rbp)
	jmp	.L72
.L76:
	movl	$0, -24(%rbp)
	jmp	.L73
.L75:
	movl	-20(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	movq	%rax, %rdx
	movq	16(%rbp), %rax
	addq	%rax, %rdx
	movl	-24(%rbp), %eax
	cltq
	movl	(%rdx,%rax,4), %eax
	testl	%eax, %eax
	jne	.L74
	movl	-20(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	movq	%rax, %rdx
	movq	16(%rbp), %rax
	addq	%rax, %rdx
	movl	-24(%rbp), %eax
	cltq
	movl	$1, (%rdx,%rax,4)
	movq	16(%rbp), %rax
	movl	$1, %edx
	movq	%rax, %rcx
	call	Minimax
	movl	%eax, -28(%rbp)
	movl	-20(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	movq	%rax, %rdx
	movq	16(%rbp), %rax
	addq	%rax, %rdx
	movl	-24(%rbp), %eax
	cltq
	movl	$0, (%rdx,%rax,4)
	movl	-16(%rbp), %edx
	movl	-28(%rbp), %eax
	cmpl	%eax, %edx
	cmovle	%edx, %eax
	movl	%eax, -16(%rbp)
.L74:
	addl	$1, -24(%rbp)
.L73:
	cmpl	$2, -24(%rbp)
	jle	.L75
	addl	$1, -20(%rbp)
.L72:
	cmpl	$2, -20(%rbp)
	jle	.L76
	movl	-16(%rbp), %eax
.L63:
	addq	$64, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	InitGame
	.def	InitGame;	.scl	2;	.type	32;	.endef
	.seh_proc	InitGame
InitGame:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movl	$0, %ecx
	call	time
	movl	%eax, %ecx
	call	srand
	movl	$0, -4(%rbp)
	jmp	.L78
.L81:
	movl	$0, -8(%rbp)
	jmp	.L79
.L80:
	movl	-8(%rbp), %eax
	movslq	%eax, %rcx
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	grid(%rip), %rax
	movl	$0, (%rdx,%rax)
	addl	$1, -8(%rbp)
.L79:
	cmpl	$2, -8(%rbp)
	jle	.L80
	addl	$1, -4(%rbp)
.L78:
	cmpl	$2, -4(%rbp)
	jle	.L81
	movl	$0, currentPlayerTurn(%rip)
	movb	$0, gameOver(%rip)
	movl	$0, winner(%rip)
	nop
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC9:
	.ascii "X\0"
.LC10:
	.ascii "O\0"
.LC11:
	.ascii "Player X's turn\0"
.LC12:
	.ascii "Player O's turn\0"
	.text
	.globl	DrawGame
	.def	DrawGame;	.scl	2;	.type	32;	.endef
	.seh_proc	DrawGame
DrawGame:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$104, %rsp
	.seh_stackalloc	104
	leaq	96(%rsp), %rbp
	.seh_setframe	%rbp, 96
	.seh_endprologue
	movl	$0, -4(%rbp)
	jmp	.L83
.L84:
	movl	-44(%rbp), %ecx
	movb	$0, %cl
	movl	$0, %eax
	movb	%al, %ch
	andl	$-16711681, %ecx
	movl	%ecx, %eax
	orl	$-16777216, %eax
	movl	%eax, %ecx
	movl	%ecx, -44(%rbp)
	movl	-4(%rbp), %eax
	imull	$200, %eax, %edx
	movl	-4(%rbp), %eax
	imull	$200, %eax, %eax
	movl	%ecx, 32(%rsp)
	movl	$600, %r9d
	movl	%edx, %r8d
	movl	$0, %edx
	movl	%eax, %ecx
	call	DrawLine
	movl	-48(%rbp), %ecx
	movb	$0, %cl
	movl	$0, %eax
	movb	%al, %ch
	andl	$-16711681, %ecx
	movl	%ecx, %eax
	orl	$-16777216, %eax
	movl	%eax, %ecx
	movl	%ecx, -48(%rbp)
	movl	-4(%rbp), %eax
	imull	$200, %eax, %edx
	movl	-4(%rbp), %eax
	imull	$200, %eax, %eax
	movl	%ecx, 32(%rsp)
	movl	%edx, %r9d
	movl	$600, %r8d
	movl	%eax, %edx
	movl	$0, %ecx
	call	DrawLine
	addl	$1, -4(%rbp)
.L83:
	cmpl	$3, -4(%rbp)
	jle	.L84
	movl	$0, -8(%rbp)
	jmp	.L85
.L90:
	movl	$0, -12(%rbp)
	jmp	.L86
.L89:
	movl	-12(%rbp), %eax
	movslq	%eax, %rcx
	movl	-8(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	grid(%rip), %rax
	movl	(%rdx,%rax), %eax
	cmpl	$1, %eax
	jne	.L87
	movl	-36(%rbp), %ecx
	movb	$-26, %cl
	movl	$41, %eax
	movb	%al, %ch
	andl	$-16711681, %ecx
	movl	%ecx, %eax
	orl	$3604480, %eax
	orl	$-16777216, %eax
	movl	%eax, -36(%rbp)
	movl	-8(%rbp), %eax
	imull	$200, %eax, %eax
	leal	50(%rax), %edi
	movl	-12(%rbp), %eax
	imull	$200, %eax, %eax
	leal	100(%rax), %esi
	movl	$100, %edx
	leaq	.LC9(%rip), %rax
	movq	%rax, %rcx
	call	MeasureText
	movl	%eax, %edx
	shrl	$31, %edx
	addl	%edx, %eax
	sarl	%eax
	negl	%eax
	addl	%esi, %eax
	movl	-36(%rbp), %esi
	movl	%esi, 32(%rsp)
	movl	$100, %r9d
	movl	%edi, %r8d
	movl	%eax, %edx
	leaq	.LC9(%rip), %rax
	movq	%rax, %rcx
	call	DrawText
	jmp	.L88
.L87:
	movl	-12(%rbp), %eax
	movslq	%eax, %rcx
	movl	-8(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	grid(%rip), %rax
	movl	(%rdx,%rax), %eax
	cmpl	$2, %eax
	jne	.L88
	movl	-40(%rbp), %edx
	movb	$0, %dl
	movl	$121, %eax
	movb	%al, %dh
	andl	$-16711681, %edx
	movl	%edx, %eax
	orl	$15794176, %eax
	orl	$-16777216, %eax
	movl	%eax, -40(%rbp)
	movl	-8(%rbp), %eax
	imull	$200, %eax, %eax
	leal	50(%rax), %edi
	movl	-12(%rbp), %eax
	imull	$200, %eax, %eax
	leal	100(%rax), %esi
	movl	$100, %edx
	leaq	.LC10(%rip), %rax
	movq	%rax, %rcx
	call	MeasureText
	movl	%eax, %edx
	shrl	$31, %edx
	addl	%edx, %eax
	sarl	%eax
	negl	%eax
	addl	%esi, %eax
	movl	-40(%rbp), %esi
	movl	%esi, 32(%rsp)
	movl	$100, %r9d
	movl	%edi, %r8d
	movl	%eax, %edx
	leaq	.LC10(%rip), %rax
	movq	%rax, %rcx
	call	DrawText
.L88:
	addl	$1, -12(%rbp)
.L86:
	cmpl	$2, -12(%rbp)
	jle	.L89
	addl	$1, -8(%rbp)
.L85:
	cmpl	$2, -8(%rbp)
	jle	.L90
	movzbl	gameOver(%rip), %eax
	xorl	$1, %eax
	testb	%al, %al
	je	.L94
	movl	currentPlayerTurn(%rip), %eax
	testl	%eax, %eax
	jne	.L92
	leaq	.LC11(%rip), %rax
	jmp	.L93
.L92:
	leaq	.LC12(%rip), %rax
.L93:
	movq	%rax, -24(%rbp)
	movb	$0, %bl
	movl	$0, %eax
	movb	%al, %bh
	movl	%ebx, %eax
	andl	$-16711681, %eax
	movl	%eax, %ebx
	movl	%ebx, %eax
	orl	$-16777216, %eax
	movl	%eax, %ebx
	movq	-24(%rbp), %rax
	movl	$20, %edx
	movq	%rax, %rcx
	call	MeasureText
	movl	%eax, %edx
	shrl	$31, %edx
	addl	%edx, %eax
	sarl	%eax
	negl	%eax
	leal	300(%rax), %edx
	movq	-24(%rbp), %rax
	movl	%ebx, 32(%rsp)
	movl	$20, %r9d
	movl	$10, %r8d
	movq	%rax, %rcx
	call	DrawText
.L94:
	nop
	addq	$104, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC13:
	.ascii "1 Player\0"
.LC14:
	.ascii "2 Players\0"
	.text
	.globl	DrawMenu
	.def	DrawMenu;	.scl	2;	.type	32;	.endef
	.seh_proc	DrawMenu
DrawMenu:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$72, %rsp
	.seh_stackalloc	72
	leaq	64(%rsp), %rbp
	.seh_setframe	%rbp, 64
	.seh_endprologue
	movl	-4(%rbp), %ecx
	movb	$0, %cl
	movl	$0, %eax
	movb	%al, %ch
	andl	$-16711681, %ecx
	movl	%ecx, %eax
	orl	$-16777216, %eax
	movl	%eax, -4(%rbp)
	movl	$40, %edx
	leaq	.LC0(%rip), %rax
	movq	%rax, %rcx
	call	MeasureText
	movl	%eax, %edx
	shrl	$31, %edx
	addl	%edx, %eax
	sarl	%eax
	negl	%eax
	addl	$300, %eax
	movl	-4(%rbp), %ecx
	movl	%ecx, 32(%rsp)
	movl	$40, %r9d
	movl	$200, %r8d
	movl	%eax, %edx
	leaq	.LC0(%rip), %rax
	movq	%rax, %rcx
	call	DrawText
	movl	-12(%rbp), %edx
	movb	$-56, %dl
	movl	$-56, %eax
	movb	%al, %dh
	andl	$-16711681, %edx
	movl	%edx, %eax
	orl	$13107200, %eax
	orl	$-16777216, %eax
	movl	%eax, 32(%rsp)
	movl	$40, %r9d
	movl	$200, %r8d
	movl	$240, %edx
	movl	$200, %ecx
	call	DrawRectangle
	movl	-8(%rbp), %ecx
	movb	$0, %cl
	movl	$0, %eax
	movb	%al, %ch
	andl	$-16711681, %ecx
	movl	%ecx, %eax
	orl	$-16777216, %eax
	movl	%eax, -8(%rbp)
	movl	$20, %edx
	leaq	.LC13(%rip), %rax
	movq	%rax, %rcx
	call	MeasureText
	movl	%eax, %edx
	shrl	$31, %edx
	addl	%edx, %eax
	sarl	%eax
	negl	%eax
	addl	$300, %eax
	movl	-8(%rbp), %ecx
	movl	%ecx, 32(%rsp)
	movl	$20, %r9d
	movl	$250, %r8d
	movl	%eax, %edx
	leaq	.LC13(%rip), %rax
	movq	%rax, %rcx
	call	DrawText
	movl	-16(%rbp), %edx
	movb	$-56, %dl
	movl	$-56, %eax
	movb	%al, %dh
	andl	$-16711681, %edx
	movl	%edx, %eax
	orl	$13107200, %eax
	orl	$-16777216, %eax
	movl	%eax, 32(%rsp)
	movl	$40, %r9d
	movl	$200, %r8d
	movl	$320, %edx
	movl	$200, %ecx
	call	DrawRectangle
	movb	$0, %bl
	movl	$0, %eax
	movb	%al, %bh
	movl	%ebx, %eax
	andl	$-16711681, %eax
	movl	%eax, %ebx
	movl	%ebx, %eax
	orl	$-16777216, %eax
	movl	%eax, %ebx
	movl	$20, %edx
	leaq	.LC14(%rip), %rax
	movq	%rax, %rcx
	call	MeasureText
	movl	%eax, %edx
	shrl	$31, %edx
	addl	%edx, %eax
	sarl	%eax
	negl	%eax
	addl	$300, %eax
	movl	%ebx, 32(%rsp)
	movl	$20, %r9d
	movl	$330, %r8d
	movl	%eax, %edx
	leaq	.LC14(%rip), %rax
	movq	%rax, %rcx
	call	DrawText
	nop
	addq	$72, %rsp
	popq	%rbx
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC15:
	.ascii "Player X Wins!\0"
.LC16:
	.ascii "Player O Wins!\0"
.LC17:
	.ascii "It's a Draw!\0"
.LC18:
	.ascii "Main Menu\0"
	.text
	.globl	DrawGameOver
	.def	DrawGameOver;	.scl	2;	.type	32;	.endef
	.seh_proc	DrawGameOver
DrawGameOver:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$88, %rsp
	.seh_stackalloc	88
	leaq	80(%rsp), %rbp
	.seh_setframe	%rbp, 80
	.seh_endprologue
	movl	winner(%rip), %eax
	cmpl	$1, %eax
	jne	.L97
	leaq	.LC15(%rip), %rax
	movq	%rax, -8(%rbp)
	jmp	.L98
.L97:
	movl	winner(%rip), %eax
	cmpl	$2, %eax
	jne	.L99
	leaq	.LC16(%rip), %rax
	movq	%rax, -8(%rbp)
	jmp	.L98
.L99:
	leaq	.LC17(%rip), %rax
	movq	%rax, -8(%rbp)
.L98:
	movl	-20(%rbp), %ecx
	movb	$0, %cl
	movl	$0, %eax
	movb	%al, %ch
	andl	$-16711681, %ecx
	movl	%ecx, %eax
	orl	$-16777216, %eax
	movl	%eax, -20(%rbp)
	movq	-8(%rbp), %rax
	movl	$40, %edx
	movq	%rax, %rcx
	call	MeasureText
	movl	%eax, %edx
	shrl	$31, %edx
	addl	%edx, %eax
	sarl	%eax
	negl	%eax
	leal	300(%rax), %edx
	movq	-8(%rbp), %rax
	movl	-20(%rbp), %ecx
	movl	%ecx, 32(%rsp)
	movl	$40, %r9d
	movl	$260, %r8d
	movq	%rax, %rcx
	call	DrawText
	movl	-24(%rbp), %edx
	movb	$-56, %dl
	movl	$-56, %eax
	movb	%al, %dh
	andl	$-16711681, %edx
	movl	%edx, %eax
	orl	$13107200, %eax
	orl	$-16777216, %eax
	movl	%eax, 32(%rsp)
	movl	$40, %r9d
	movl	$200, %r8d
	movl	$340, %edx
	movl	$200, %ecx
	call	DrawRectangle
	movb	$0, %bl
	movl	$0, %eax
	movb	%al, %bh
	movl	%ebx, %eax
	andl	$-16711681, %eax
	movl	%eax, %ebx
	movl	%ebx, %eax
	orl	$-16777216, %eax
	movl	%eax, %ebx
	movl	$20, %edx
	leaq	.LC18(%rip), %rax
	movq	%rax, %rcx
	call	MeasureText
	movl	%eax, %edx
	shrl	$31, %edx
	addl	%edx, %eax
	sarl	%eax
	negl	%eax
	addl	$300, %eax
	movl	%ebx, 32(%rsp)
	movl	$20, %r9d
	movl	$350, %r8d
	movl	%eax, %edx
	leaq	.LC18(%rip), %rax
	movq	%rax, %rcx
	call	DrawText
	nop
	addq	$88, %rsp
	popq	%rbx
	popq	%rbp
	ret
	.seh_endproc
	.globl	CheckWin
	.def	CheckWin;	.scl	2;	.type	32;	.endef
	.seh_proc	CheckWin
CheckWin:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$16, %rsp
	.seh_stackalloc	16
	.seh_endprologue
	movl	%ecx, 16(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L101
.L105:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	movq	%rax, %rdx
	leaq	grid(%rip), %rax
	movl	(%rdx,%rax), %eax
	cmpl	%eax, 16(%rbp)
	jne	.L102
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	movq	%rax, %rdx
	leaq	4+grid(%rip), %rax
	movl	(%rdx,%rax), %eax
	cmpl	%eax, 16(%rbp)
	jne	.L102
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	movq	%rax, %rdx
	leaq	8+grid(%rip), %rax
	movl	(%rdx,%rax), %eax
	cmpl	%eax, 16(%rbp)
	jne	.L102
	movl	$1, %eax
	jmp	.L103
.L102:
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,4), %rdx
	leaq	grid(%rip), %rax
	movl	(%rdx,%rax), %eax
	cmpl	%eax, 16(%rbp)
	jne	.L104
	movl	-4(%rbp), %eax
	cltq
	addq	$3, %rax
	leaq	0(,%rax,4), %rdx
	leaq	grid(%rip), %rax
	movl	(%rdx,%rax), %eax
	cmpl	%eax, 16(%rbp)
	jne	.L104
	movl	-4(%rbp), %eax
	cltq
	addq	$6, %rax
	leaq	0(,%rax,4), %rdx
	leaq	grid(%rip), %rax
	movl	(%rdx,%rax), %eax
	cmpl	%eax, 16(%rbp)
	jne	.L104
	movl	$1, %eax
	jmp	.L103
.L104:
	addl	$1, -4(%rbp)
.L101:
	cmpl	$2, -4(%rbp)
	jle	.L105
	movl	grid(%rip), %eax
	cmpl	%eax, 16(%rbp)
	jne	.L106
	movl	16+grid(%rip), %eax
	cmpl	%eax, 16(%rbp)
	jne	.L106
	movl	32+grid(%rip), %eax
	cmpl	%eax, 16(%rbp)
	jne	.L106
	movl	$1, %eax
	jmp	.L103
.L106:
	movl	8+grid(%rip), %eax
	cmpl	%eax, 16(%rbp)
	jne	.L107
	movl	16+grid(%rip), %eax
	cmpl	%eax, 16(%rbp)
	jne	.L107
	movl	24+grid(%rip), %eax
	cmpl	%eax, 16(%rbp)
	jne	.L107
	movl	$1, %eax
	jmp	.L103
.L107:
	movl	$0, %eax
.L103:
	addq	$16, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	CheckDraw
	.def	CheckDraw;	.scl	2;	.type	32;	.endef
	.seh_proc	CheckDraw
CheckDraw:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$16, %rsp
	.seh_stackalloc	16
	.seh_endprologue
	movl	$0, -4(%rbp)
	jmp	.L109
.L114:
	movl	$0, -8(%rbp)
	jmp	.L110
.L113:
	movl	-8(%rbp), %eax
	movslq	%eax, %rcx
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	grid(%rip), %rax
	movl	(%rdx,%rax), %eax
	testl	%eax, %eax
	jne	.L111
	movl	$0, %eax
	jmp	.L112
.L111:
	addl	$1, -8(%rbp)
.L110:
	cmpl	$2, -8(%rbp)
	jle	.L113
	addl	$1, -4(%rbp)
.L109:
	cmpl	$2, -4(%rbp)
	jle	.L114
	movl	$1, %eax
.L112:
	addq	$16, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 4
.LC1:
	.long	1128792064
	.align 4
.LC2:
	.long	1137180672
	.align 4
.LC3:
	.long	1131413504
	.align 4
.LC4:
	.long	1133248512
	.align 4
.LC5:
	.long	1134559232
	.align 4
.LC6:
	.long	1135869952
	.align 4
.LC7:
	.long	1135214592
	.align 4
.LC8:
	.long	1136525312
	.ident	"GCC: (Rev6, Built by MSYS2 project) 13.2.0"
	.def	InitWindow;	.scl	2;	.type	32;	.endef
	.def	IsMouseButtonPressed;	.scl	2;	.type	32;	.endef
	.def	GetMousePosition;	.scl	2;	.type	32;	.endef
	.def	BeginDrawing;	.scl	2;	.type	32;	.endef
	.def	ClearBackground;	.scl	2;	.type	32;	.endef
	.def	EndDrawing;	.scl	2;	.type	32;	.endef
	.def	WindowShouldClose;	.scl	2;	.type	32;	.endef
	.def	CloseWindow;	.scl	2;	.type	32;	.endef
	.def	srand;	.scl	2;	.type	32;	.endef
	.def	DrawLine;	.scl	2;	.type	32;	.endef
	.def	MeasureText;	.scl	2;	.type	32;	.endef
	.def	DrawText;	.scl	2;	.type	32;	.endef
	.def	DrawRectangle;	.scl	2;	.type	32;	.endef
