#define _CRT_SECURE_NO_WARNINGS
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>

static void dump_buf(uint8_t* buf, uint32_t len)//�����������
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
	treenode* par;//����һ�����ڵ�
};
treenode* top = NULL;//�涥���ڵ�
int treedepth;
void tree_create(uint8_t* d)
{
	//dump_buf((uint8_t*)d, 32);
	if (top == NULL) {//�����δ����
		treenode* t1 = (treenode*)malloc(sizeof(treenode));//�½�һ���ڵ�
		t1->data = d;
		t1->lc = NULL;
		t1->rc = NULL;
		t1->par = NULL;
		//dump_buf((uint8_t*)t1->data, 32);
		top = t1;
		//dump_buf((uint8_t*)top->data, 32);
		treedepth = 0;//���Ϊ0
		return;
	}
	int depth = 0;
	treenode* nodec = top;//��ǰ�Ķ����ڵ�Ϊtop
	while (nodec->rc != NULL) {
		nodec = nodec->rc;
		depth++;//��ȼ�һ
	}
	if (depth == treedepth) {//��ȵ�������ʱ����Ҫ���϶˽����ڵ�
		treenode* newtop = (treenode*)malloc(sizeof(treenode));
		newtop->lc = top;
		newtop->rc = (treenode*)malloc(sizeof(treenode));
		newtop->rc->data = d;
		newtop->rc->lc = NULL;//���ӽڵ�û�б��ΪNULL
		newtop->rc->rc = NULL;
		uint8_t* sm3_value;
		sm3_value = SM3_encrypt_merkle(newtop->lc->data, newtop->rc->data);
		newtop->data = sm3_value;
		newtop->lc->par = newtop;//���ڵ���Ϊ�µĶ����ڵ�
		newtop->rc->par = newtop;
		newtop->par = NULL;
		top = newtop;
		treedepth++;
	}
	else {
		nodec->lc = (treenode*)malloc(sizeof(treenode));//����ֱ�ӽ���
		nodec->lc->data = nodec->data;
		nodec->lc->lc = NULL;
		nodec->lc->rc = NULL;
		nodec->lc->par = nodec;
		nodec->rc = (treenode*)malloc(sizeof(treenode));
		nodec->rc->data = d;
		nodec->rc->lc = NULL;
		nodec->rc->rc = NULL;
		nodec->rc->par = nodec;//���ڵ���Ϊ�ող����ӽڵ�ĵĸ��ڵ�
		uint8_t* sm3_value;
		sm3_value = SM3_encrypt_merkle(nodec->lc->data, nodec->rc->data);
		nodec->data = sm3_value;
	}
}
treenode* searchfor1(treenode* node, uint8_t* hash) {//���search��ҪѰ�Ҷ�Ӧ��λ�ô�����ڵ�С���ҽڵ�
	treenode* temp;
	treenode* temp1;
	if (node->lc != NULL) {//�����Ӧ�ӽڵ㲻ΪNULL��������ݹ�
		temp = searchfor1(node->lc, hash);
		temp1 = searchfor1(node->rc, hash);
		if (temp == NULL) {//�����ڲ�ΪNULL���ز�ΪNULL�ڵ㣬����ֱ�ӷ���NULL
			return temp1;
		}
		else {
			return temp;
		}
	}
	else {
		node = node->par;
		for (int i = 0; i < 32; i++) {//�Դ˽��д�С�ıȽϣ��ɹ��Ļ����ظýڵ�
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
	treenode* node = searchfor(top, hash);//��ȡ��hashֵ�Ľڵ�
	treenode* nodec = node->par;
	uint8_t* sm3_value = hash;
	while (nodec != NULL) {
		if (nodec->lc == node) {//λ����ڵ����
			sm3_value = SM3_encrypt_merkle(sm3_value, nodec->rc->data);
		}
		if (nodec->rc == node) {//λ���ҽڵ����
			sm3_value = SM3_encrypt_merkle(nodec->lc->data, sm3_value);
		}
		node = nodec;
		nodec = nodec->par;//һֱ�ظ�ֱ�����ﶥ��
	}
	for (int i = 0; i < 32; i++) {
		if (sm3_value[i] != (top->data)[i]) {//���бȽ�
			return true;//�����һ������ȷ���true��������
		}
	}
	return false;//��ȷ���false
}
int main()
{
	uint8_t input1[64] = {
0x01, 0x23, 0x45, 0x00, 0x00, 0x00, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10,
0x01, 0x23, 0x45, 0x00, 0x00, 0x00, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10
	};
	input1[0] = input1[0] + 3;
	uint8_t* temp = SM3_encrypt(input1);//��ǰ����һ�����в���������
	input1[0] = input1[0] - 3;
	for (int i = 0; i < 8; i++) {
		tree_create(SM3_encrypt(input1));//��һ��64bit�����𲽵�������hash��0λ��Ԫ��ÿ�μ�1
		input1[0] = input1[0] + 2;
	}
	treenode* node = searchfor(top, temp);//��ȡֵΪ��hash�ĸýڵ�
	if (exclusion_proof(temp)) {
		printf("�����ڸõ�");
	}
	else {
		printf("���ڸõ�");
	}
}