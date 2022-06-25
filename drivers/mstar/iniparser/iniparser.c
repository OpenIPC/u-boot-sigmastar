/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

  Unless otherwise stipulated in writing, any and all information contained
 herein regardless in any format shall remain the sole proprietary of
 Sigmastar Technology Corp. and be kept in strict confidence
 (��Sigmastar Confidential Information��) by the recipient.
 Any unauthorized act including without limitation unauthorized disclosure,
 copying, use, reproduction, sale, distribution, modification, disassembling,
 reverse engineering and compiling of the contents of Sigmastar Confidential
 Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
 rights to any and all damages, losses, costs and expenses resulting therefrom.
*/
/*---------------------------- Includes ------------------------------------*/
#include <common.h>
#include <malloc.h>
#include <stdlib.h>
#include <vsprintf.h>
#include <linux/ctype.h>
#include <linux/string.h>

#include "iniparser.h"

/*---------------------------- Defines -------------------------------------*/
#define ASCIILINESZ         (3096)
#define INI_INVALID_KEY     ((char*)-1)

/*---------------------------------------------------------------------------
                        Private to this module
 ---------------------------------------------------------------------------*/
/**
 * This enum stores the status for each parsed line (internal use only).
 */
typedef enum _line_status_
{
    LINE_UNPROCESSED,
    LINE_ERROR,
    LINE_EMPTY,
    LINE_COMMENT,
    LINE_SECTION,
    LINE_VALUE
} line_status ;

/*-------------------------------------------------------------------------*/
/**
  @brief    Convert a string to lowercase.
  @param    s   String to convert.
  @param    result    pointer to returned value
  @param    r_size    size of returned value array
  @return   void

  This function returns a pointer to a statically allocated string
  containing a lowercased version of the input string. Do not free
  or modify the returned string! Since the returned string is statically
  allocated, it will be modified at each function call (not re-entrant).
 */
/*--------------------------------------------------------------------------*/
static void strlwc(const char * s, char * result, unsigned int r_size)
{
    if (s == NULL) return;

    if (NULL == result) return;

    if (0 == r_size) return;

    int i = 0;

    memset(result, 0, r_size);
    i = 0 ;
    while(s[i] && (i <(int) (r_size-1)))
    {
        result[i] = (char)tolower((int)s[i]);
        i++ ;
    }
    result[r_size-1] = (char)0;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Remove blanks at the beginning and the end of a string.
  @param    s   String to parse.
  @param    result    Pointer to returned value
  @param    r_size    size of returned value array
  @return   void

  This function returns a pointer to a statically allocated string,
  which is identical to the input string, except that all blank
  characters at the end and the beg. of the string have been removed.
  Do not free or modify the returned string! Since the returned string
  is statically allocated, it will be modified at each function call
  (not re-entrant).
 */
/*--------------------------------------------------------------------------*/
static void strstrip(char * s, char * result, unsigned int r_size)
{
    if (s == NULL) return;

    if (NULL == result) return;

    if (0 == r_size) return;

    char * last = NULL;

    while(isspace((int)*s) && *s) s++;

    memset(result, 0, r_size);
    strncpy(result, s, r_size-1);
    result[r_size-1] = '\0';

    last = result + strlen(result);
    while(last > result)
    {
        if(!isspace((int)*(last - 1)))
            break ;
        last -- ;
    }
    *last = (char)0;
}


static void strstrip_for_getstring(const char * s, char * result, unsigned int r_size)
{
    if (s == NULL) return;

    if (NULL == result) return;

    if (0 == r_size) return;

    char * last = NULL;

    while(isspace((int)*s) && *s) s++;

    memset(result, 0, r_size);
    strncpy(result, s, r_size-1);
    result[r_size-1] = '\0';

    last = result + strlen(result);
    while(last > result)
    {
        if(!isspace((int)*(last - 1)))
            break ;
        last -- ;
    }
    *last = (char)0;


    //check ':' nearby blank & take them off
    char * pCheck = result;
    char * pBlankFound;
    char l_2[ASCIILINESZ+1] = {0};
    char l_temp[ASCIILINESZ+1] = {0};

    memset(l_2, 0, ASCIILINESZ + 1);
    memset(l_temp, 0, ASCIILINESZ + 1);


    //check if have blank in string
    while(!isspace((int)*pCheck) && *pCheck) pCheck++;
    if (pCheck == last) 
        return;

    //find ':'
    pCheck = result;
    do{
        if (*pCheck == ':') break;  //find first ':', we don't handle secondary ':'
        pCheck++;
    }while( pCheck != last);

    if (pCheck == last)
        return;   //can't find ':'

    //blank before ':'
    if ( result != pCheck)
    {
        pBlankFound = pCheck - 1;
        while(isspace((int)*pBlankFound))
        {
            *pBlankFound = 0;
            pBlankFound--;
        }
        strcpy(l_2, result);
    }

    snprintf(l_temp, ASCIILINESZ+1, "%s:", l_2);

    //blank after ':'
    pBlankFound = pCheck + 1;
    while(isspace((int)*pBlankFound) && *pBlankFound)
    {
        pBlankFound++;
    }
    memset(l_2, 0, ASCIILINESZ + 1);
    snprintf(l_2, ASCIILINESZ+1, "%s%s", l_temp, pBlankFound);

    strncpy(result, l_2, r_size);
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get number of sections in a dictionary
  @param    d   Dictionary to examine
  @return   int Number of sections found in dictionary

  This function returns the number of sections found in a dictionary.
  The test to recognize sections is done on the string stored in the
  dictionary: a section name is given as "section" whereas a key is
  stored as "section:key", thus the test looks for entries that do not
  contain a colon.

  This clearly fails in the case a section name contains a colon, but
  this should simply be avoided.

  This function returns -1 in case of error.
 */
/*--------------------------------------------------------------------------*/
int iniparser_getnsec(dictionary * d)
{
    int i ;
    int nsec ;

    if(d == NULL) return -1 ;
    nsec = 0 ;
    for(i = 0 ; i < d->size ; i++)
    {
        if(d->key[i] == NULL)
            continue ;
        if(strchr(d->key[i], ':') == NULL)
        {
            nsec ++ ;
        }
    }
    return nsec ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get name for section n in a dictionary.
  @param    d   Dictionary to examine
  @param    n   Section number (from 0 to nsec-1).
  @return   Pointer to char string

  This function locates the n-th section in a dictionary and returns
  its name as a pointer to a string statically allocated inside the
  dictionary. Do not free or modify the returned string!

  This function returns NULL in case of error.
 */
/*--------------------------------------------------------------------------*/
char * iniparser_getsecname(dictionary * d, int n)
{
    int i ;
    int foundsec ;

    if((d == NULL) || (n < 0)) return NULL ;
    foundsec = 0 ;
    for(i = 0 ; i < d->size ; i++)
    {
        if(d->key[i] == NULL)
            continue ;
        if(strchr(d->key[i], ':') == NULL)
        {
            foundsec++ ;
            if(foundsec > n)
                break ;
        }
    }
    if(foundsec <= n)
    {
        return NULL ;
    }
    return d->key[i] ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key
  @param    d       Dictionary to search
  @param    key     Key string to look for
  @param    def     Default value to return if key not found.
  @return   pointer to statically allocated character string

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the pointer passed as 'def' is returned.
  The returned char pointer is pointing to a string allocated in
  the dictionary, do not free or modify it.
 */
/*--------------------------------------------------------------------------*/
char * iniparser_getstring(dictionary * d, const char * key, char * def)
{
    char lc_key[ASCIILINESZ+1] = {0};
    char * sval = NULL;
    char non_blank_key[ASCIILINESZ+1] = {0};

    if((d == NULL) || (key == NULL))
        return def ;

    //Remove blanks at the beginning and the end of a string.
    strstrip_for_getstring(key, non_blank_key, ASCIILINESZ+1);


    strlwc(non_blank_key, lc_key, ASCIILINESZ+1);
    sval = dictionary_get(d, lc_key, def);
    return sval ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key, convert to an int
  @param    d Dictionary to search
  @param    key Key string to look for
  @param    notfound Value to return in case of error
  @return   integer

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the notfound value is returned.

  Supported values for integers include the usual C notation
  so decimal, octal (starting with 0) and hexadecimal (starting with 0x)
  are supported. Examples:

  "42"      ->  42
  "042"     ->  34 (octal -> decimal)
  "0x42"    ->  66 (hexa  -> decimal)

  Warning: the conversion may overflow in various ways. Conversion is
  totally outsourced to strtol(), see the associated man page for overflow
  handling.

  Credits: Thanks to A. Becker for suggesting strtol()
 */
/*--------------------------------------------------------------------------*/
int iniparser_getint(dictionary * d, const char * key, int notfound)
{
    char    *   str ;

    str = iniparser_getstring(d, key, INI_INVALID_KEY);
    if(str == INI_INVALID_KEY) return notfound ;
    return (int)simple_strtoul(str, NULL, 0);
}

unsigned int iniparser_getunsignedint(dictionary * d, const char * key, unsigned int notfound)
{
    char    *   str ;

    str = iniparser_getstring(d, key, INI_INVALID_KEY);
    if(str == INI_INVALID_KEY) return notfound ;
    return (unsigned int)simple_strtoul(str, NULL, 0);
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key, convert to a boolean
  @param    d Dictionary to search
  @param    key Key string to look for
  @param    notfound Value to return in case of error
  @return   integer

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the notfound value is returned.

  A true boolean is found if one of the following is matched:

  - A string starting with 'y'
  - A string starting with 'Y'
  - A string starting with 't'
  - A string starting with 'T'
  - A string starting with '1'

  A false boolean is found if one of the following is matched:

  - A string starting with 'n'
  - A string starting with 'N'
  - A string starting with 'f'
  - A string starting with 'F'
  - A string starting with '0'

  The notfound value returned if no boolean is identified, does not
  necessarily have to be 0 or 1.
 */
/*--------------------------------------------------------------------------*/
int iniparser_getboolean(dictionary * d, const char * key, int notfound)
{
    char    *   c ;
    int         ret ;

    c = iniparser_getstring(d, key, INI_INVALID_KEY);
    if(c == INI_INVALID_KEY) return notfound ;
    if((c[0] == 'y') || (c[0] == 'Y') || (c[0] == '1') || (c[0] == 't') || (c[0] == 'T'))
    {
        ret = 1 ;
    }
    else if((c[0] == 'n') || (c[0] == 'N') || (c[0] == '0') || (c[0] == 'f') || (c[0] == 'F'))
    {
        ret = 0 ;
    }
    else
    {
        ret = notfound ;
    }
    return ret;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Finds out if a given entry exists in a dictionary
  @param    ini     Dictionary to search
  @param    entry   Name of the entry to look for
  @return   integer 1 if entry exists, 0 otherwise

  Finds out if a given entry exists in the dictionary. Since sections
  are stored as keys with NULL associated values, this is the only way
  of querying for the presence of sections in a dictionary.
 */
/*--------------------------------------------------------------------------*/
int iniparser_find_entry(
    dictionary  *   ini,
    char        *   entry
)
{
    int found = 0 ;
    if(iniparser_getstring(ini, entry, INI_INVALID_KEY) != INI_INVALID_KEY)
    {
        found = 1 ;
    }
    return found ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Set an entry in a dictionary.
  @param    ini     Dictionary to modify.
  @param    entry   Entry to modify (entry name)
  @param    val     New value to associate to the entry.
  @return   int 0 if Ok, -1 otherwise.

  If the given entry can be found in the dictionary, it is modified to
  contain the provided value. If it cannot be found, -1 is returned.
  It is Ok to set val to NULL.
 */
/*--------------------------------------------------------------------------*/
int iniparser_setstring(dictionary * ini, const char * entry, char * val)
{
    char lwc_key[ASCIILINESZ+1] = {0};
    strlwc(entry, lwc_key, ASCIILINESZ+1);
    return dictionary_set(ini, lwc_key, val);
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Delete an entry in a dictionary
  @param    ini     Dictionary to modify
  @param    entry   Entry to delete (entry name)
  @return   void

  If the given entry can be found, it is deleted from the dictionary.
 */
/*--------------------------------------------------------------------------*/
void iniparser_unset(dictionary * ini, char * entry)
{
    char lwc_key[ASCIILINESZ+1] = {0};
    strlwc(entry, lwc_key, ASCIILINESZ+1);
    dictionary_unset(ini, lwc_key);
}
char *iniparser_parser_string_to_array(const char *inputStr, char ***strArray, int *count)
{
    char * loop = NULL;
    const char *start = NULL;
    const char *end = NULL;
    char *buffer = NULL;
    char **buffer_hold = NULL;
    char *loop_end = 0;
    int len = 0;
    int array_count = 0;
    int i = 0;
    unsigned char head_0_tail_1 = 0;

    if (!inputStr) return NULL;
    if (!strArray) return NULL;
    if (!count) return NULL;
    *strArray = NULL;
    *count = 0;
    loop = (char *)inputStr;
    while (*loop == ' ') loop++;
    if (*loop++ != '{')
    {
        printf("Do not detect : '{'\n");
        return NULL;
    }
    while (*loop == ' ') loop++;
    start = loop;
    end = strlen(inputStr) + inputStr;
    array_count = 0;
    while (*loop != '}' && loop != end)
    {
        if (*loop == ',')
        {
            array_count++;
        }
        loop++;
    }
    if (*loop != '}')
    {
        printf("Do not detect : '}'\n");
        return NULL;
    }
    end = loop - 1;
    if (start > end)
    {
        printf("No contain!\n");
        return NULL;
    }
    len = strlen(inputStr) + 1;
    buffer = (char *)malloc(len);
    if (buffer == NULL) return NULL;
    //printf("Alloc 1 %p len %d\n", buffer, len);
    buffer_hold = (char **)malloc(sizeof(char *) * (array_count + 1));
    if (buffer_hold == NULL)
    {
        free(buffer);
        return NULL;
    }
    //printf("Alloc 2 %p len %d\n", buffer_hold, sizeof(char *) * (array_count + 1));
    memcpy(buffer, inputStr, len - 1);
    buffer[len - 1] = 0;
    loop = buffer + (start - inputStr);
    len = end - start + 2;
    loop_end = loop + len - 1;
    memset(buffer_hold, 0, sizeof(char *) * (array_count + 1));
    if (*loop != ' ' && *loop != ',' && *loop != '\t')
    {
        buffer_hold[0] = loop;
        head_0_tail_1 = 1;
        i = 1;
        loop++;
    }
    while (loop < loop_end)
    {
        switch (*loop)
        {
            case ' ':
            case ',':
            case '\t':
            {
               if (head_0_tail_1)
               {
                   head_0_tail_1 = 0;
                   *loop = 0;
               }
               if (head_0_tail_1 == 0
                   && (*(loop + 1) != ' ' && *(loop + 1) != ',' && *(loop + 1) != '\t')
                   && loop + 1 < loop_end)
               {
                   buffer_hold[i] = loop + 1;
                   head_0_tail_1 = 1;
                   i++;
               }
            }
            break;
            default:
                break;

        }
        loop++;
    }
    if (head_0_tail_1)
    {
        *loop = 0;
    }
    if (i == 0)
    {
        printf("No value.\n");
        //printf("Internal Free 1 %p\n", buffer_hold);
        //printf("Internal Free 2 %p\n", buffer);
        free(buffer_hold);
        free(buffer);
        buffer_hold = NULL;
        buffer = NULL;
    }
    *count = i;
    *strArray = buffer_hold;

    return buffer;
}


/*-------------------------------------------------------------------------*/
/**
  @brief    Load a single line from an INI file
  @param    input_line  Input line, may be concatenated multi-line input
  @param    section     Output space to store section
  @param    key         Output space to store key
  @param    value       Output space to store value
  @return   line_status value
 */
/*--------------------------------------------------------------------------*/
static line_status iniparser_line(
    char * input_line,
    char * section,
    char * key,
    char * value)
{
    line_status sta ;
    char        line[ASCIILINESZ+1];
    int         len ;

    if(input_line!=NULL)
    {
        char strip_line[ASCIILINESZ+1] = {0};
        strstrip(input_line, strip_line, ASCIILINESZ+1);
        strncpy(line, strip_line, ASCIILINESZ);
    }
    line[ASCIILINESZ] = '\0';

    len = (int)strlen(line);

    sta = LINE_UNPROCESSED ;
    if(len < 1)
    {
        /* Empty line */
        sta = LINE_EMPTY ;
    }
    else if(line[0] == '#')
    {
        /* Comment line */
        sta = LINE_COMMENT ;
    }
    else if((line[0] == '[') && (line[len-1] == ']'))
    {
        /* Section name */
        sscanf(line, "[%[^]]", section);
        if(section!=NULL)
        {
            char strip_section[ASCIILINESZ+1] = {0};
            char lwc_section[ASCIILINESZ+1] = {0};
            strstrip(section, strip_section, ASCIILINESZ+1);
            strlwc(strip_section, lwc_section, ASCIILINESZ+1);
            strncpy(section, lwc_section, ASCIILINESZ);
        }
        sta = LINE_SECTION ;
    }
    else if((sscanf(line, "%[^=] = \"%[^\"]\"", key, value) == 2)
            ||  (sscanf(line, "%[^=] = '%[^\']'",   key, value) == 2)
            ||  (sscanf(line, "%[^=] = %[^;#]",     key, value) == 2))
    {
        /* Usual key=value, with or without comments */
        if(key!=NULL)
        {
            char strip_key[ASCIILINESZ+1] = {0};
            char lwc_key[ASCIILINESZ+1] = {0};
            strstrip(key, strip_key, ASCIILINESZ+1);
            strlwc(strip_key, lwc_key, ASCIILINESZ+1);
            strncpy(key, lwc_key, ASCIILINESZ);
        }
        if(value!=NULL)
        {
            char strip_value[ASCIILINESZ+1] = {0};
            strstrip(value, strip_value, ASCIILINESZ+1);
            strncpy(value, strip_value, ASCIILINESZ);
        }
        /*
         * sscanf cannot handle '' or "" as empty values
         * this is done here
         */
        if(value!=NULL && (!strcmp(value, "\"\"") || (!strcmp(value, "''"))))
        {
            value[0] = 0 ;
        }
        sta = LINE_VALUE ;
    }
    else if((sscanf(line, "%[^=] = %[;#]", key, value) == 2)
            ||  (sscanf(line, "%[^=] %[=]", key, value) == 2))
    {
        /*
         * Special cases:
         * key=
         * key=;
         * key=#
         */
        if(key!=NULL)
        {
            char strip_key[ASCIILINESZ+1] = {0};
            char lwc_key[ASCIILINESZ+1] = {0};
            strstrip(key, strip_key, ASCIILINESZ+1);
            strlwc(strip_key, lwc_key, ASCIILINESZ+1);
            strncpy(key, lwc_key, ASCIILINESZ);
        }
        value[0] = 0 ;
        sta = LINE_VALUE ;
    }
    else
    {
        /* Generate syntax error */
        sta = LINE_ERROR ;
    }
    return sta ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Parse an ini file and return an allocated dictionary object
  @param    ininame Name of the ini file to read.
  @return   Pointer to newly allocated dictionary

  This is the parser for ini files. This function is called, providing
  the name of the file to be read. It returns a dictionary object that
  should not be accessed directly, but through accessor functions
  instead.

  The returned dictionary must be freed using iniparser_freedict().
 */
/*--------------------------------------------------------------------------*/
dictionary * iniparser_load(const char * ininame, iniparser_file_opt *pfile_opt)
{
    INI_FILE * in ;

    char line    [ASCIILINESZ+1] ;
    char section [ASCIILINESZ+1] ;
    char key     [ASCIILINESZ+1] ;
    char tmp     [ASCIILINESZ+1] ;
    char val     [ASCIILINESZ+1] ;

    int  last = 0 ;
    int  len ;
    int  lineno = 0 ;
    int  errs = 0;

    dictionary * dict ;

    if(ininame == NULL)
    {
        return NULL;
    }
    if (pfile_opt == NULL)
    {
        return NULL;
    }

    in = pfile_opt->ini_open(ininame, "r");
    if(in == NULL)
    {
        printf("iniparser: cannot open %s\n", ininame);
        return NULL ;
    }
    dict = dictionary_new(0) ;
    if(!dict)
    {
        pfile_opt->ini_close(in);
        return NULL ;
    }

    memset(line,    0, ASCIILINESZ);
    memset(section, 0, ASCIILINESZ);
    memset(key,     0, ASCIILINESZ);
    memset(val,     0, ASCIILINESZ);
    last = 0 ;

    while(pfile_opt->ini_fgets(line + last, ASCIILINESZ - last, in) != NULL)
    {
        lineno++ ;
        len = (int)strlen(line) - 1;
        /* Safety check against buffer overflows */
       if ((len == ASCIILINESZ) && (line[len]!='\n')) // if(line[len] != '\n')
        {
            fprintf(stderr,
                    "iniparser: input line too long in %s (%d)\n",
                    ininame,
                    lineno);
            dictionary_del(dict);
            pfile_opt->ini_close(in);
            return NULL ;
        }
        /* Get rid of \n and spaces at end of line */
        while((len >= 0) &&
                ((line[len] == '\n') || (isspace(line[len]))))
        {
            line[len] = 0 ;
            len-- ;
        }
        /* Detect multi-line */
        if((len>=0)&&(line[len] == '\\'))
        {
            /* Multi-line value */
            last = len ;
            continue ;
        }
        else
        {
            last = 0 ;
        }
        switch(iniparser_line(line, section, key, val))
        {
            case LINE_EMPTY:
            case LINE_COMMENT:
                break ;

            case LINE_SECTION:
                errs = dictionary_set(dict, section, NULL);
                break ;

            case LINE_VALUE:
                //sprintf(tmp, "%s:%s", section, key);
                snprintf(tmp, ASCIILINESZ, "%s:%s", section, key);
                errs = dictionary_set(dict, tmp, val) ;
                break ;

            case LINE_ERROR:
                fprintf(stderr, "iniparser: syntax error in %s (%d):\n",
                        ininame,
                        lineno);
                fprintf(stderr, "-> %s\n", line);
                errs++ ;
                break;

            default:
                break ;
        }
        memset(line, 0, ASCIILINESZ);
        last = 0;
        if(errs < 0)
        {
            fprintf(stderr, "iniparser: memory allocation failure\n");
            break ;
        }
    }
    if(errs)
    {
        dictionary_del(dict);
        dict = NULL ;
    }
    pfile_opt->ini_close(in);
    return dict ;
}
/*-------------------------------------------------------------------------*/
/**
  @brief    Free all memory associated to an ini dictionary
  @param    d Dictionary to free
  @return   void

  Free all memory associated to an ini dictionary.
  It is mandatory to call this function before the dictionary object
  gets out of the current context.
 */
/*--------------------------------------------------------------------------*/
void iniparser_freedict(dictionary * d)
{
    dictionary_del(d);
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get value of correspond section and num.
  @param    d         dictionary object to find
  @param    section   section to find
  @param    i         num to find
  @return   1 pointer to internally allocated character string.

  This function return value which correspond section and num.
 */
/*--------------------------------------------------------------------------*/
char * iniparser_getVal(dictionary * d,char * section,int i)
{
    if((d == NULL)) return NULL;
    char lwc_section[ASCIILINESZ+1] = {0};
    strlwc(section, lwc_section, ASCIILINESZ+1);
    return dictionary_getVal(d,lwc_section,i);
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get total number of correspond section.
  @param    d         dictionary object to find
  @param    section   section to find
  @return   total number of section.

  This function return total number of correspond section.
 */
/*--------------------------------------------------------------------------*/
int iniparser_getNumberOfSection(dictionary * d,char * section)
{
	char lwc_section[ASCIILINESZ+1] = {0};
    strlwc(section, lwc_section, ASCIILINESZ+1);
    return dictionary_getNumberOfSection(d,lwc_section);
  }
/* vim: set ts=4 et sw=4 tw=75 */


