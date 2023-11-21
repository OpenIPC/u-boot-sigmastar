#include <common.h>
#include <command.h>
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"

#define BASE_REG_PMTOP_PA            GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x1E00)
#define GET_CHIPID                   INREG16(BASE_REG_PMTOP_PA) & 0x00FF

int match_chipid();
char *get_script_next_line(char **line_buf_ptr);
