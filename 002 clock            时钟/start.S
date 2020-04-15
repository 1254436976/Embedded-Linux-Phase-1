

.text
.global _start

_start:

	/*  关闭看门狗  */
	ldr r0, =0x53000000
	ldr r1, =0
	str r1, [r0]
	
	/*
	** 设置LOCKTIME寄存器
	*/
	ldr r0, =0xC0000000
	ldr r1, =0xFFFFFFFF
	str r1, [r0]

	/*
	** 设置CLKDIVn寄存器
	** HCLK = FCLK/4,PCLK = HCLK/2
	*/
	ldr r0, =0x4C000014
	ldr r1, =0x5
	str r1, [r0]
	
	/*
	**设置为异步模式
	*/
	mrc p15,0,r0,c1,c0,0
	orr r0,r0,#0xc0000000
	mcr p15,0,r0,c1,c0,0
	
	/*
	** 设置MPLLCON寄存器
	** Mpll=(2*m*Fin)/(p*2^s)  
	** m=MDIV+8,p=PDIV+2,s=SDIV
	*/
	ldr r0, =0xC0000004
	ldr r1, =(92<<12)|(1<<4)|(1<<0)
	str r1, [r0]
	
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
	
	bl main
	
halt:
	b halt
