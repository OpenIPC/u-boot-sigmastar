/*
*/
#include <common.h>
#include <command.h>
#include <match_chipid.h>
#define IPL_HEADER_MAGIC_OFFSET 0x4
#define IPL_MAGIC 0x5F4C5049
#define IPL_HEADER_SIZE_OFFSET 0x8

/*
 * search_chipid() - Search the burned ipl.bin for the location of "CHIPID"
 *
 * start - The place where IPL.bin was burned
 *
 * size - The size of IPL.bin
 *
 * return 0 - It means that the CHIPID was searched in IPL.bin and matched
 * successfully with the CHIPID on the chip.
 *
 * return -1 - It means that the CHIPID was searched in IPL.bin, but it
 * failed to match the CHIPID on the chip.
 *
 * return -2 - It means that no CHIPID is found in IPL.bin.
*/
int search_chipid(long start, int size)
{

    u8 *pos = (u8*)(start);

    for(int offset = 0; offset<size; offset+=4)
    {
        pos=start+offset;
        if(*pos==0x43&&*(pos+1)==0x48&&*(pos+2)==0x49&&*(pos+3)==0x50&&*(pos+4)==0x49&&*(pos+5)==0x44)//search for ''CHIPID'
        {
            u8 *id = (u8*)(pos+6);
            char idd[10];//make sure it can hold *id
            sprintf(idd,"%c%c",*id,*(id+1));//Combine two single characters into a string.
            char *idstart;
            idstart = &idd[0]; ////Gets the beginning of the string
            int ipl_chipid=simple_strtoul(idstart,NULL,16); //convert char type to int type
            printf("Ipl_chipid: 0X%02X \n",ipl_chipid);

            /*get reg_chipid,which is recorded on the chip*/
            int reg_chipid = GET_CHIPID;
            printf("Reg_chipid：0X%02X \n",reg_chipid);

            /*do the actual match*/
            if(ipl_chipid==reg_chipid)
            {
                printf("Match successful，begin estar !\n");
                return 0;//match success
            }
            else
            {
                printf("Match failed，exit estar!\n");
                return -1;//match failed
            }
        }
    }
    return -2;//can't find CHIPID
}


/*
 * match_chipid() - Judging whether IPL.bin is opened or not according to
 * the executed command string and magic number
 *
 * next_line - Command line to execute
 *
 * script_buf - Buffer of script to be executed
 *
 * return 0 - It means that the CHIPID was searched in IPL.bin and matched
 * successfully with the CHIPID on the chip.
 *
 * return -1 - It means that matching fails, including searching and matching failure
*/
int match_chipid()
{
    long start=0x21000000;//where IPL is written
    int *find_magic = (u32*)(start+IPL_HEADER_MAGIC_OFFSET);//get MAGIC.
    int result = 0;
    /*when true, IPL.bin is opened.*/
    if(*find_magic == IPL_MAGIC)
    {
        //get the size of the IPL.bin
        int size = *(u32*)(start+IPL_HEADER_SIZE_OFFSET);

        //search scope
        printf("Searching for chipid!\n");
        printf("Search scope：%08x~%08x\n",start,start+size-1);

        /*searching IPL.bin for ipl_chipid and do a actual match with reg_chipid*/
        int result =search_chipid(start,size);
        if (result==-1) //match failed
        {
            return -1;
        }
        else if(result==-2)//can't find CHIPID
        {
            printf("CHIPID could not be found!\n");
            return -1;
        }
        else if(result==0)//match success
            return 0;
    }
    else return -1;

}

