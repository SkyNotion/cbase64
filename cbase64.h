#ifndef CBASE64_H
#define CBASE64_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

static char base64_encode_lut[64] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/'
};

static int base64_decode_lut[123] = {
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

static int base64_padding[3] = {0, 2, 1};

char* base64_encode_b64ch(char* bin, size_t* text_len, size_t* text_size, char* buffer){
	int padding = *text_len % 3;
	size_t block_size = (*text_size + ((*text_size % 6) > 0 ? 6 - (*text_size % 6) : 0)) / 6;
	char* b64_text = (char*)malloc((block_size * sizeof(char)) + base64_padding[padding] + 1);
	for(size_t i = 0;i < block_size;i++){
		memset(buffer, '0', 8);
		memcpy(&buffer[2], &bin[i*6], (*text_size - (i * 6)) < 6 ? (*text_size - (i * 6)): 6);
		b64_text[i] = base64_encode_lut[strtol(buffer, NULL, 2)];
	}
	memset(&b64_text[block_size], '=', base64_padding[padding]);
	free(bin);
	return b64_text;
}

char* base64_encode(char* text){
	char buffer[8];
	size_t text_len = strlen(text);
	size_t text_size = text_len * 8, n, j;
	char *bin = (char*)malloc(text_size);
	for(size_t i = 0;i < text_len;i++){
		memset(buffer, '0', 8);
		n = (int)text[i],
		j = 7;
		do{
			buffer[j] = n % 2 == 1 ? '1' : '0';
			n >>= 1;
			j--;
		}while(n > 0);
		memcpy(&bin[i*8], buffer, 8);
	}
	return base64_encode_b64ch(bin, &text_len, &text_size, buffer);
}

char* base64_encode_file(FILE *file){
	char buffer[8];
	fseek(file, 0, SEEK_END);
	size_t text_len = ftell(file);
	size_t text_size = text_len * 8;
	char *bin = (char*)malloc(text_size);
	fseek(file, 0, SEEK_SET);
	int ch, x = 0, j;
	while((ch = fgetc(file)) != EOF){
		memset(buffer, '0', 8);
		j = 7;
		do{
			buffer[j] = ch % 2 == 1 ? '1' : '0';
			ch >>= 1;
			j--;
		}while(ch > 0);
		memcpy(&bin[x*8], buffer, 8);
		x++;
	}
	fclose(file);	
	return base64_encode_b64ch(bin, &text_len, &text_size, buffer);;
}

char* base64_decode_b64ch(char* b64_text){
	char buffer[8];
	size_t text_len = strlen(b64_text);
	check_padding:
		if(b64_text[text_len - 1] == '='){text_len--; goto check_padding;}
	int n, j;
	char* bin = (char*)malloc(text_len * 6);
	for(size_t i = 0;i < text_len;i++){
		memset(buffer, '0', 8);
		n = base64_decode_lut[(int)b64_text[i]],
		j = 7;
		do{
			buffer[j] = n % 2 == 1 ? '1' : '0';
			n >>= 1;
			j--;
		}while(n > 0);
		memcpy(&bin[i*6], &buffer[2], 6);
	}
	return bin;
}

char* base64_decode(char* b64_text){
	char buffer[8];
	char *bin = base64_decode_b64ch(b64_text);
	size_t data_len = strlen(bin) >> 3;
	char *text = (char*)malloc(data_len);
	for(size_t i = 0;i < data_len;i++){
		memcpy(buffer, &bin[i*8], 8);
		text[i] = (char)strtol(buffer, NULL, 2);
	}
	free(bin);
	return text;
}

int base64_decode_file(char* b64_text, FILE* file){
	char buffer[8];
	char *bin = base64_decode_b64ch(b64_text);
	size_t data_len = strlen(bin) >> 3;
	for(size_t i = 0;i < data_len;i++){
		memcpy(buffer, &bin[i*8], 8);
		fputc(strtol(buffer, NULL, 2), file);
	}
	free(bin);
	fclose(file);
	return 0;
}

#endif