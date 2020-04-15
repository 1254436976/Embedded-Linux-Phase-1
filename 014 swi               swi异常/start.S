
.text
.global _start

_start:
	b reset  /* vector 0:reset */
	ldr pc, und_addr /* vector 4:und_addr */
	ldr pc, swi_addr /* vector 8:swi_addr */
	
und_addr:
	.word do_und

swi_addr:
	.word do_swi

do_und:
	/* 执行到这里之前:
	 * 1. lr_und保存有被中断模式中的下一条即将执行的指令的地址
	 * 2. SPSR_und保存有被中断模式的CPSR
	 * 3. CPSR中的M4-M0被设置为11011, 进入到und模式
	 * 4. 跳到0x4的地方执行程序 
	 */

	/*
	* 1.保存现场
	* 2.处理异常
	* 3.恢复现场
	*/
	
	ldr sp, =0x34000000 //und模式下的sp寄存器为该模式下专用寄存器，使用前需要对其进行设置
	
	/* 在进入und模式的时候，查表可知r0-r12寄存器的值可能会被修改，所以需要对其进行保存 */
	stmdb sp!, {r0-r12,lr}

	/* 处理异常函数：把cpsr内的信息放到r0,执行printException */
	mrs r0, cpsr
	ldr r1, =und_string
	bl printException
	
	/* 恢复现场，把spsr寄存器的值传输回cpsr*/
	ldmia sp!, {r0-r12,pc}^
	
und_string:
	.string "undefined instruction exception"

.align 4

do_swi:
	/* 执行到这里之前:
	 * 1. lr_svc保存有被中断模式中的下一条即将执行的指令的地址
	 * 2. SPSR_svc保存有被中断模式的CPSR
	 * 3. CPSR中的M4-M0被设置为10011, 进入到svc模式
	 * 4. 跳到0x8的地方执行程序 
	 */

	/*
	* 1.保存现场
	* 2.处理异常
	* 3.恢复现场
	*/
	
	ldr sp, =0x33e00000  //und模式下的sp寄存器为该模式下专用寄存器，使用前需要对其进行设置
	
	/* 在进入und模式的时候，查表可知r0-r12寄存器的值可能会被修改，所以需要对其进行保存 */
	stmdb sp!, {r0-r12,lr}
	
	mov r4,lr   //把lr寄存器下一条即将执行的指令的地址传到r0

	/* 处理异常函数：把cpsr内的信息放到r0,执行printException */
	mrs r0, cpsr
	ldr r1, =swi_string
	bl printException
	
	sub r0,r4,#4     //r4地址减四就是swi指令执行地址
	bl printSWIVal
	
	/* 恢复现场，把spsr寄存器的值传输回cpsr*/
	ldmia sp!, {r0-r12,pc}^

swi_string:
	.string "swi exception"

.align 4

reset:
	/* 关闭看门狗 */
	ldr r0, =0x53000000
	ldr r1, =0
	str r1, [r0]

	/* 设置MPLL, FCLK : HCLK : PCLK = 400m : 100m : 50m */
	/* LOCKTIME(0x4C000000) = 0xFFFFFFFF */
	ldr r0, =0x4C000000
	ldr r1, =0xFFFFFFFF
	str r1, [r0]

	/* CLKDIVN(0x4C000014) = 0X5, tFCLK:tHCLK:tPCLK = 1:4:8  */
	ldr r0, =0x4C000014
	ldr r1, =0x5
	str r1, [r0]

	/* 设置CPU工作于异步模式 */
	mrc p15,0,r0,c1,c0,0
	orr r0,r0,#0xc0000000   //R1_nF:OR:R1_iA
	mcr p15,0,r0,c1,c0,0

	/* 设置MPLLCON(0x4C000004) = (92<<12)|(1<<4)|(1<<0) 
	 *  m = MDIV+8 = 92+8=100
	 *  p = PDIV+2 = 1+2 = 3
	 *  s = SDIV = 1
	 *  FCLK = 2*m*Fin/(p*2^s) = 2*100*12/(3*2^1)=400M
	 */
	ldr r0, =0x4C000004
	ldr r1, =(92<<12)|(1<<4)|(1<<0)
	str r1, [r0]

	/* 一旦设置PLL, 就会锁定lock time直到PLL输出稳定
	 * 然后CPU工作于新的频率FCLK
	 */
	
	

	/* 设置内存: sp 栈 */
	/* 分辨是nor/nand启动
	 * 写0到0地址, 再读出来
	 * 如果得到0, 表示0地址上的内容被修改了, 它对应ram, 这就是nand启动
	 * 否则就是nor启动
	 */
	mov r1, #0
	ldr r0, [r1] /* 读出原来的值备份 */
	str r1, [r1] /* 0->[0] */ 
	ldr r2, [r1] /* r2=[0] */
	cmp r1, r2   /* r1==r2? 如果相等表示是NAND启动 */
	ldr sp, =0x40000000+4096 /* 先假设是nor启动 */
	moveq sp, #4096  /* nand启动 */
	streq r0, [r1]   /* 恢复原来的值 */

	bl sdram_init
	
	bl copy_to_sdram
	
	bl clean_bss
	
	/* 
	 *复位之后, cpu处于svc模式
	 *系统先进入用户模式，修改cpsr寄存器的值
	 *后在swi指令触发异常
	 */
	mrs r0,cpsr       /* 读出cpsr的值 */
	bic r0, r0, #0xf  /* 修改M4-M0为0b10000, 进入usr模式 */
	msr cpsr, r0      /* 写入值，改变cpsr的值*/
	
	/* 设置usr模式下的sp寄存器 */
	ldr sp, =0x33f00000
	
	ldr pc, =sdram   
sdram: 
	bl uart0_init
	
	bl print1
	/*故意定义一条未定义指令，使cpu进入und模式 */
und_code:
	.word 0xdeadc0de //word定义的是一变量，会开辟占用内存		
	bl print2
	
	swi 0x123  /* swi指令，用来触发异常 */
	
	//bl main    /* 使用BL命令相对跳转, 程序仍然在NOR/sram执行 */
	ldr pc, =main     /* 绝对跳转, 跳到SDRAM */
	
halt:
	b halt
	
