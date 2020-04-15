
#ifndef _NAND_FLASH_H
#define _NAND_FLASH_H

extern void nand_init(void);
extern void do_scan_nand_flash(void);
extern void do_read_nand_flash(unsigned  int addr,unsigned char *buf  ,int len);
extern void nand_read(unsigned  int addr,unsigned char *buf  ,int len);


#endif /* _NAND_FLASH_H */

