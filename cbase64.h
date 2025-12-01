#ifndef __CBASE64_H
#define __CBASE64_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

const static char base64_encode_lut[64] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/'
};

const static uint8_t base64_decode_lut[123] = {
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 62, 255, 255, 255, 63, 52, 53, 54,
	55, 56, 57, 58, 59, 60, 61, 255, 255, 255, 255,
	255, 255, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
	11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
	23, 24, 25, 255, 255, 255, 255, 255, 255, 26, 27,
	28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
	40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
};

const static uint8_t base64_padding[3] = {0, 2, 1};

char* base64_encode(uint8_t* data, size_t data_sz){
	uint8_t padding = data_sz % 3;
    uint8_t rem = (data_sz <<= 3) % 6;
    data_sz = (data_sz + ((6 - rem) % 6)) / 6;
    char* base64_text = (char*)malloc(sizeof(char) + (data_sz + base64_padding[padding] + 1 + ((4 - (data_sz % 4)) % 4)));
    size_t idx = 0, pos = 0;
    do{
        base64_text[idx++] = base64_encode_lut[data[pos++] >> 2];
        base64_text[idx++] = base64_encode_lut[(data[pos] >> 4) | ((data[pos - 1] << 4) & 0x3f)];
        pos++;
        base64_text[idx++] = base64_encode_lut[(data[pos] >> 6) | ((data[pos - 1] << 2) & 0x3f)];
        base64_text[idx++] = base64_encode_lut[data[pos++] & 0x3f];
    }while(idx < data_sz);
    memset(&base64_text[data_sz], '=', base64_padding[padding]);
    base64_text[data_sz + base64_padding[padding]] = '\0';
    return base64_text;
}

uint8_t* base64_decode(char* base64_text){
    size_t text_len = strlen(base64_text);
check_padding:
	if(base64_text[text_len - 1] == '='){
		text_len--; 
		goto check_padding;
	}
	size_t text_sz = (text_len * 6) / 8;
    size_t idx = 0, pos = 0;
    uint8_t* data = (uint8_t*)malloc(text_sz + 1 + ((3 - (text_sz % 3)) % 3));
    do{
        data[idx++] = (base64_decode_lut[base64_text[pos]] << 2) | (base64_decode_lut[base64_text[pos + 1]] >> 4);
        pos++;
        data[idx++] = (base64_decode_lut[base64_text[pos]] << 4) | (base64_decode_lut[base64_text[pos + 1]] >> 2);
        pos++;
        data[idx++] = (base64_decode_lut[base64_text[pos]] << 6) | base64_decode_lut[base64_text[pos + 1]];
        pos += 2;
    }while(idx < text_sz);
    data[text_sz] = '\0';
    return data;
}

#endif