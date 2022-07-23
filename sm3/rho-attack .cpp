#define _CRT_SECURE_NO_WARNINGS
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>

static void dump_buf(uint8_t* buf, uint32_t len)//用于数据输出
{
	int i;

	printf("buf:");

	for (i = 0; i < len; i++) {
		printf("%s%02X%s", i % 16 == 0 ? "\r\n\t" : " ",
			buf[i],
			i == len - 1 ? "\r\n" : "");
	}
}

uint8_t* SM3_encrypt(uint8_t input[32])//使用了openssl库来进行sm3hash
{
	//dump_buf((uint8_t*)input, 32);
	uint32_t out_len = 0;

	uint8_t* sm3_value;
	sm3_value = (uint8_t*)malloc(32);

	//创建上下文
	EVP_MD_CTX* ctx = EVP_MD_CTX_new();

	EVP_MD_CTX_init(ctx);
	//初始化上下文
	EVP_DigestInit_ex(ctx, EVP_sm3(), NULL);


	EVP_DigestUpdate(ctx, input, 32);

	EVP_DigestFinal_ex(ctx, sm3_value, &out_len);//结束了之后将值保存在sm3_value

	//dump_buf((uint8_t*)sm3_value,32);
	EVP_MD_CTX_reset(ctx);


	return sm3_value;
}

void rho()
{
	uint8_t input[32] = {
	0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10
	};
	uint8_t* sm3_value1 = input;//初始赋成相同值
	uint8_t* sm3_value2 = input;
	while (1) {
		uint8_t* input1 = sm3_value1;
		uint8_t* input2 = sm3_value2;
		sm3_value1 = SM3_encrypt(sm3_value1);//加密一次
		sm3_value2 = SM3_encrypt(SM3_encrypt(sm3_value2));//加密两次
		if (sm3_value1[0] == sm3_value2[0] && sm3_value1[1] == sm3_value2[1]) {//若部分相同，进行输出
			dump_buf((uint8_t*)input1, 32);
			dump_buf((uint8_t*)input2, 32);
			printf("Hash is: ");
			dump_buf((uint8_t*)sm3_value1, 2);
		}
	}
}

int main()
{
	rho();
	return 0;
}