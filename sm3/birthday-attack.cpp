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

struct Node//����ṹ�壬����hash�����ݣ���ֻ����������ֵ�����˸ı䣩
{
	uint8_t flag[3];
	uint8_t* output;//����hash�Ľ��
	Node* next;//ָ����һ���ڵ�
};
Node* hashtable[65536];//��ϣ��

uint8_t* SM3_encrypt(uint8_t input[32])//ʹ����openssl��������sm3hash
{
	//dump_buf((uint8_t*)input, 32);
	uint32_t out_len = 0;

	uint8_t* sm3_value;
	sm3_value = (uint8_t*)malloc(32);

	//����������
	EVP_MD_CTX* ctx = EVP_MD_CTX_new();

	EVP_MD_CTX_init(ctx);
	//��ʼ��������
	EVP_DigestInit_ex(ctx, EVP_sm3(), NULL);


	EVP_DigestUpdate(ctx, input, 32);

	EVP_DigestFinal_ex(ctx, sm3_value, &out_len);//������֮��ֵ������sm3_value

	//dump_buf((uint8_t*)sm3_value,32);
	EVP_MD_CTX_reset(ctx);


	return sm3_value;
}

void pushlist(Node* n, uint8_t data[32], uint8_t input[32]) {//�˺�������������ϣ����
	while (n->next != NULL)//nextΪNULL�Ľڵ㣬��output��û�����
		n = n->next;
	n->output = data;
	n->flag[0] = input[0];//��flagֵ�������
	n->flag[1] = input[1];
	n->flag[2] = input[2];
	n->next = (Node*)malloc(sizeof(Node));//����һ��next���п���
	n = n->next;
	n->next = NULL;//���δ������Node��nextָ��ָ��NULL
	return;

}

void birattack()
{
	uint8_t input[32] = {
	0x00, 0x00, 0x00, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10
	};//inputֻ��ǰ24bit���иı�
	uint8_t* sm3_value;
	for (int i = 0; i < 25; i++) {
		for (int j = 0; j < 256; j++) {
			for (int k = 0; k < 256; k++) {
				input[0] = i;
				input[1] = j;
				input[2] = k;
				sm3_value = SM3_encrypt(input);
				pushlist(hashtable[sm3_value[0] * sm3_value[1]], sm3_value, input);//����hash���У���ȡ�㹻��������������ײ
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
	for (int i = 0; i < 65536; i++) {//��ϣ���ʼ��
		hashtable[i] = (Node*)malloc(sizeof(Node));
		hashtable[i]->next = NULL;
	}
	birattack();
	uint8_t* sm3_value;
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < 256; j++) {
			for (int k = 0; k < 256; k++) {
				input1[3] = i;//ֻ�ı����е�25-48bit
				input1[4] = j;
				input1[5] = k;
				sm3_value = SM3_encrypt(input1);
				Node* pt = hashtable[sm3_value[0] * sm3_value[1]];
				while (pt->next != NULL) {//����ΪNULL����������ݵĶԱ�
					if (sm3_value[2] == pt->output[2] && sm3_value[3] == pt->output[3] && sm3_value[4] == pt->output[4] && sm3_value[5] == pt->output[5] && sm3_value[6] == pt->output[6]) {//������ײǰ56bit
						input2[0] = pt->flag[0];//������ײ������ֵ��hash��ǰ56bit�������
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