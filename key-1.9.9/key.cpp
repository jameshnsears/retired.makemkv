#include <iostream>
#include <stdlib.h>
#include <stdint.h>


#define AP_KEY_STRING_LEN 68

typedef uint16_t utf16_t;

static char KeyBitsToChar(uint8_t c)
{
    if (c==0) {
    	return '_';
    }

    if (c<11) {
        // std::cout << '0' + (c-1) << std::endl;
    	return '0' + (c-1);
    }

    if (c<38) {
    	// std::cout << '@'+(c-11) << std::endl;
    	return '@'+(c-11);
    }

    // std::cout << 'a'+(c-38) << std::endl;
    return 'a'+(c-38);
}

static uint8_t KeyIsValidChar(utf16_t c)
{
    if (c=='_') return true;
    if ( (c>='0') && (c<='9') ) return true;
    if ( (c>='@') && (c<='Z') ) return true;
    if ( (c>='a') && (c<='z') ) return true;
    return false;
}

static size_t utf16len(const utf16_t *SrcString)
{
    const uint16_t *p=SrcString;
    while(*p!=0)
    {
        p++;
    }
    return (p-SrcString);
}

static bool KeyCheckStringCrc(const utf16_t* str)
{
	size_t len = utf16len(str);

    if (len != AP_KEY_STRING_LEN )
    {
        return false;
    }

    uint16_t crc=0;

    for (unsigned int i=0;i<(AP_KEY_STRING_LEN-2);i++)
    {
        if ( (i>=2) && (KeyIsValidChar(str[i])==false)) {
        	return false;
        }

        crc += ((uint8_t)str[i]) & 0x7f;

        crc = (uint16_t) (((crc*(11+i)))%4093);
    }

    if (KeyBitsToChar( (uint8_t) (crc&0x3f) ) != str[AP_KEY_STRING_LEN-2]) {
    	return false;
    }

    if (KeyBitsToChar( (uint8_t) ((crc>>6)&0x3f)) != str[AP_KEY_STRING_LEN-1]) {
    	return false;
    }

    return true;
}

// stop entering main in gdb:
// http://help.eclipse.org/luna/index.jsp?topic=%2Forg.eclipse.cdt.doc.user%2Freference%2Fcdt_u_dsfgdb.htm
int main(int argc, char *argv[]) {
	utf16_t key[] = {
			'A', 'A', 'A', 'A', 'A', 'A','A', 'A', 'A','A', \
			'A', 'A', 'A', 'A', 'A', 'A','A', 'A', 'A','A', \
			'A', 'A', 'A', 'A', 'A', 'A','A', 'A', 'A','A', \
			'A', 'A', 'A', 'A', 'A', 'A','A', 'A', 'A','A', \
			'A', 'A', 'A', 'A', 'A', 'A','A', 'A', 'A','A', \
			'A', 'A', 'A', 'A', 'A', 'A','A', 'A', 'A','A', \
			'A', 'A', 'A', 'A', 'A', 'A','8', '2', 0};

	// AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA82

	// T-s3SN8ux5qQlNBylE3l1GXnsJaLJoU7VFlM7k0SUSKA7qABr4t9DUwqHUMmV0DUT7QY
	// = http://www.makemkv.com/forum2/viewtopic.php?f=5&t=1053
	// = current beta key

	if (KeyCheckStringCrc(key) == true) {
		std::cout << EXIT_SUCCESS;
	}
	else {
		std::cout << EXIT_FAILURE;
	}
}
