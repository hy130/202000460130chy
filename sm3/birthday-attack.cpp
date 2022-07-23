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

struct Node//定义结构体，保存hash的数据（因只对其中三个值进行了改变）
{
	uint8_t flag[3];
	uint8_t* output;//保存hash的结果
	Node* next;//指向下一个节点
};
Node* hashtable[65536];//哈希表

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

void pushlist(Node* n, uint8_t data[32], uint8_t input[32]) {//此函数将结果放入哈希表中
	while (n->next != NULL)//next为NULL的节点，其output还没有填充
		n = n->next;
	n->output = data;
	n->flag[0] = input[0];//将flag值进行填充
	n->flag[1] = input[1];
	n->flag[2] = input[2];
	n->next = (Node*)malloc(sizeof(Node));//将下一个next进行开辟
	n = n->next;
	n->next = NULL;//这个未开发的Node的next指针指向NULL
	return;

}

void birattack()
{
	uint8_t input[32] = {
	0x00, 0x00, 0x00, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10
	};//input只有前24bit进行改变
	uint8_t* sm3_value;
	for (int i = 0; i < 25; i++) {
		for (int j = 0; j < 256; j++) {
			for (int k = 0; k < 256; k++) {
				input[0] = i;
				input[1] = j;
				input[2] = k;
				sm3_value = SM3_encrypt(input);
				pushlist(hashtable[sm3_value[0] * sm3_value[1]], sm3_value, input);//放入hash表中，存取足够多数据来进行碰撞
			}
		}
	}
}

int main()
{
	uint8_t input1[32] = {
	0x01, 0x23, 0x45, 0x00, 0x00, 0x00, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10
	};
	uint8_t input2[32] = {
	0x00, 0x00, 0x00, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10
	};
	for (int i = 0; i < 65536; i++) {//哈希表初始化
		hashtable[i] = (Node*)malloc(sizeof(Node));
		hashtable[i]->next = NULL;
	}
	birattack();
	uint8_t* sm3_value;
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < 256; j++) {
			for (int k = 0; k < 256; k++) {
				input1[3] = i;//只改变其中的25-48bit
				input1[4] = j;
				input1[5] = k;
				sm3_value = SM3_encrypt(input1);
				Node* pt = hashtable[sm3_value[0] * sm3_value[1]];
				while (pt->next != NULL) {//若不为NULL，则进行数据的对比
					if (sm3_value[2] == pt->output[2] && sm3_value[3] == pt->output[3] && sm3_value[4] == pt->output[4] && sm3_value[5] == pt->output[5] && sm3_value[6] == pt->output[6]) {//尝试碰撞前56bit
						input2[0] = pt->flag[0];//发现碰撞后将输入值及hash的前56bit进行输出
						input2[1] = pt->flag[1];
						input2[2] = pt->flag[2];
						dump_buf((uint8_t*)input1, 32);
						dump_buf((uint8_t*)input2, 32);
						printf("Hash is: ");
						dump_buf((uint8_t*)sm3_value, 7);
					}
					pt = pt->next;
				}
			}

		}
	}
	return 0;
}