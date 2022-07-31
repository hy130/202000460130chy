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
	treenode* par;//增加一个父节点
};
treenode* top = NULL;//存顶部节点
int treedepth;
void tree_create(uint8_t* d)
{
	//dump_buf((uint8_t*)d, 32);
	if (top == NULL) {//如果树未开发
		treenode* t1 = (treenode*)malloc(sizeof(treenode));//新建一个节点
		t1->data = d;
		t1->lc = NULL;
		t1->rc = NULL;
		t1->par = NULL;
		//dump_buf((uint8_t*)t1->data, 32);
		top = t1;
		//dump_buf((uint8_t*)top->data, 32);
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
		newtop->lc->par = newtop;//父节点标记为新的顶部节点
		newtop->rc->par = newtop;
		newtop->par = NULL;
		top = newtop;
		treedepth++;
	}
	else {
		nodec->lc = (treenode*)malloc(sizeof(treenode));//否则直接建立
		nodec->lc->data = nodec->data;
		nodec->lc->lc = NULL;
		nodec->lc->rc = NULL;
		nodec->lc->par = nodec;
		nodec->rc = (treenode*)malloc(sizeof(treenode));
		nodec->rc->data = d;
		nodec->rc->lc = NULL;
		nodec->rc->rc = NULL;
		nodec->rc->par = nodec;//父节点标记为刚刚产生子节点的的父节点
		uint8_t* sm3_value;
		sm3_value = SM3_encrypt_merkle(nodec->lc->data, nodec->rc->data);
		nodec->data = sm3_value;
	}
}
treenode* searchfor1(treenode* node, uint8_t* hash) {//这个search主要寻找对应的位置大于左节点小于右节点
	treenode* temp;
	treenode* temp1;
	if (node->lc != NULL) {//如果对应子节点不为NULL，则继续递归
		temp = searchfor1(node->lc, hash);
		temp1 = searchfor1(node->rc, hash);
		if (temp == NULL) {//若存在不为NULL返回不为NULL节点，否则直接返回NULL
			return temp1;
		}
		else {
			return temp;
		}
	}
	else {
		node = node->par;
		for (int i = 0; i < 32; i++) {//对此进行大小的比较，成功的话返回该节点
			if (node->lc->data[i] < hash[i]) {
				break;
			}
			if (node->lc->data[i] > hash[i]) {
				return NULL;
			}
		}
		for (int i = 0; i < 32; i++) {
			if (node->rc->data[i] > hash[i]) {
				break;
			}
			if (node->rc->data[i] < hash[i]) {
				return NULL;
			}
		}
		return node;
	}
}
bool exclusion_proof(uint8_t* hash) {
	treenode* node = searchfor(top, hash);//获取该hash值的节点
	treenode* nodec = node->par;
	uint8_t* sm3_value = hash;
	while (nodec != NULL) {
		if (nodec->lc == node) {//位于左节点情况
			sm3_value = SM3_encrypt_merkle(sm3_value, nodec->rc->data);
		}
		if (nodec->rc == node) {//位于右节点情况
			sm3_value = SM3_encrypt_merkle(nodec->lc->data, sm3_value);
		}
		node = nodec;
		nodec = nodec->par;//一直重复直到到达顶部
	}
	for (int i = 0; i < 32; i++) {
		if (sm3_value[i] != (top->data)[i]) {//进行比较
			return true;//如果有一个不相等返回true，不存在
		}
	}
	return false;//相等返回false
}
int main()
{
	uint8_t input1[64] = {
0x01, 0x23, 0x45, 0x00, 0x00, 0x00, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10,
0x01, 0x23, 0x45, 0x00, 0x00, 0x00, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10
	};
	input1[0] = input1[0] + 3;
	uint8_t* temp = SM3_encrypt(input1);//提前保存一个其中不包含的数
	input1[0] = input1[0] - 3;
	for (int i = 0; i < 8; i++) {
		tree_create(SM3_encrypt(input1));//对一个64bit的数逐步递增进行hash，0位置元素每次加1
		input1[0] = input1[0] + 2;
	}
	treenode* node = searchfor(top, temp);//获取值为此hash的该节点
	if (exclusion_proof(temp)) {
		printf("不存在该点");
	}
	else {
		printf("存在该点");
	}
}