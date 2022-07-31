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

struct treenode {
	uint8_t* data;
	treenode* lc;
	treenode* rc;
};
treenode* top = NULL;//存顶部节点
int treedepth;//存树的深度
void tree_create(uint8_t* d)//寻找最右边的节点
{
	if (top == NULL) {//如果树未开发
		treenode* t1 = (treenode*)malloc(sizeof(treenode));//新建一个节点
		t1->data = d;
		t1->lc = NULL;
		t1->rc = NULL;
		top = t1;
		treedepth = 0;//深度为0
		return;
	}
	int depth = 0;
	treenode* nodec = top;//当前的顶部节点为top
	while (nodec->rc != NULL) {
		nodec = nodec->rc;
		depth++;//深度加一
	}
	if (depth == treedepth) {//深度到达最深时，需要在上端建立节点
		treenode* newtop = (treenode*)malloc(sizeof(treenode));
		newtop->lc = top;
		newtop->rc = (treenode*)malloc(sizeof(treenode));
		newtop->rc->data = d;
		newtop->rc->lc = NULL;//孩子节点没有标记为NULL
		newtop->rc->rc = NULL;
		uint8_t* sm3_value;
		sm3_value = SM3_encrypt_merkle(newtop->lc->data, newtop->rc->data);
		newtop->data = sm3_value;
		top = newtop;
		treedepth++;
	}
	else {
		nodec->lc = (treenode*)malloc(sizeof(treenode));//否则直接建立
		nodec->lc->data = nodec->data;
		nodec->lc->lc = NULL;
		nodec->lc->rc = NULL;
		nodec->rc = (treenode*)malloc(sizeof(treenode));
		nodec->rc->data = d;
		nodec->rc->lc = NULL;
		nodec->rc->rc = NULL;
		uint8_t* sm3_value;
		sm3_value = SM3_encrypt_merkle(nodec->lc->data, nodec->rc->data);
		nodec->data = sm3_value;
	}
}

uint8_t* SM3_encrypt_merkle(uint8_t input[64], uint8_t input1[64])//针对merkle树特殊加密
{
	uint32_t out_len = 0;

	uint8_t sm3_value[EVP_MAX_MD_SIZE];

	//创建上下文
	EVP_MD_CTX* ctx = EVP_MD_CTX_new();

	EVP_MD_CTX_init(ctx);
	//初始化上下文
	EVP_DigestInit_ex(ctx, EVP_sm3(), NULL);

	EVP_DigestUpdate(ctx, input, 64);//两次加密
	EVP_DigestUpdate(ctx, input1, 64);

	EVP_DigestFinal_ex(ctx, sm3_value, &out_len);

	EVP_MD_CTX_reset(ctx);

	return sm3_value;
}

int main()
{
	uint8_t input1[64] = {
	0x01, 0x23, 0x45, 0x00, 0x00, 0x00, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10,
	0x01, 0x23, 0x45, 0x00, 0x00, 0x00, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10
	};
	for (int i = 0; i < 100000; i++) {
		printf("the numer %d,the treedepth is %d\n", i, treedepth);
		tree_create(input1);//生成十万个节点
	}
}