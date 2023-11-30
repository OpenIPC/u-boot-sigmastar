/*
* SsRecovery.h- Sigmastar
*
* Copyright (c) [2019~2020] SigmaStar Technology.
*
*
* This software is licensed under the terms of the GNU General Public
* License version 2, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License version 2 for more details.
*
*/


#ifndef __SS_RECOVERY_H__
#define __SS_RECOVERY_H__

#define RECOVERY_PART_NUM (3)

#define RECOVERY_DATA_COPY_ADDR	(0x21000000)
#define RECOVERY_DATA_COPY_BATCH (0x1400000)


/*
 * store the recovery parameter
 */
typedef struct{
	char parta[100];	/* partition name to copy to */
	char partb[100];
	int offset;			/* offset to copy the data, block unit */
	int size;			/* size to copy the data, block unit*/
}MS_RecoveryPrm;


int RecoveryToBackup(void);
int RecoveryCheck(void);

#endif //__SS_RECOVERY_H__