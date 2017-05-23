#include<stdio.h>   
#include<stdlib.h>   
#include<string.h>   

#define FLAG 10

void encfile(char *in_filename, char *out_filename);/*���ļ����м��ܵľ��庯��*/
void decryptfile(char* in_filename, char *out_filename); /*���ļ����ܵľ��庯��*/

int main(int argc, char *argv[])/*����main()�����������в���*/
{
	if (argc != 4) {// argv[0]��exe����
		printf("��ӭʹ�ã���������Ҫ����������\n");
		printf("  ����1��ָʾҪ���еĲ�����1=���ܣ�2=���ܡ�\n");
		printf("  ����2����Ҫ���м��ܻ���ܵ��ļ���\n");
		printf("  ����3�����ܻ���ܺ���ļ�����·����\n");
		system("pause");
		return -1;
	}
	int option=argv[1][0]; /*����ѡ��*/

	switch (option) {
	case '1': //����      
		encfile(argv[2], argv[3]);/*���ܺ�������*/
		break;
	case '2'://����  
		decryptfile(argv[2], argv[3]);/*���ܺ�������*/
		break;
	default:
		printf("��һ������ֻ����1����2��1=���ܣ�2=���ܡ�\n");
		return -1;
	}
	printf("������ɡ�");
	return 0;
}

/*�����Ӻ�����ʼ*/
void encfile(char *in_filename, char *out_file) {
	FILE *fp1, *fp2;
	int j = 0;
	fp1 = fopen(in_filename, "rb");/*��Ҫ���ܵ��ļ�*/
	if (fp1 == NULL) {
		printf("cannot open in-file.\n");
		exit(1);/*������ܴ�Ҫ���ܵ��ļ�,���˳�����*/
	}
	fp2 = fopen(out_file, "wb");
	if (fp2 == NULL) {
		printf("cannot open or create out-file.\n");
		exit(1);/*������ܽ������ܺ���ļ�,���˳�*/
	}
	register int ch;
	for (ch = fgetc(fp1); ch != EOF; ch = fgetc(fp1)) {// ִ�м���
		ch += FLAG;// �����㷨��ֱ�Ӽ���һ����ֵ
		fputc(ch, fp2); 
	}
	fclose(fp1);/*�ر�Դ�ļ�*/
	fclose(fp2);/*�ر�Ŀ���ļ�*/
}

/*�����Ӻ�����ʼ*/
void decryptfile(char *in_filename, char *out_file) {
	FILE *fp1, *fp2;
	int j = 0;
	fp1 = fopen(in_filename, "rb");/*��Ҫ���ܵ��ļ�*/
	if (fp1 == NULL) {
		printf("cannot open in-file.\n");
		exit(1);/*������ܴ�Ҫ���ܵ��ļ�,���˳�����*/
	}
	fp2 = fopen(out_file, "wb");
	if (fp2 == NULL) {
		printf("cannot open or create out-file.\n");
		exit(1);/*������ܽ������ܺ���ļ�,���˳�*/
	}
	register int ch;
	for (ch = fgetc(fp1); ch != EOF; ch = fgetc(fp1)) {// ִ�н���
		ch -= FLAG;
		fputc(ch, fp2);/*�ҵĽ����㷨*/
	}
	fclose(fp1);/*�ر�Դ�ļ�*/
	fclose(fp2);/*�ر�Ŀ���ļ�*/
}