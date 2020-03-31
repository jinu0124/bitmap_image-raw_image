#define _CRT_SECURE_NO_WARNINGS
#define WIDTHBYTES(bits)  (((bits)+31)/32*4) //������ �������� 4Byte�� ����̴�.

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

//���� 3�� �ҽ� //2015253039 ������
int main()
{
	char RawFileName[30] = "lena_raw_512x512.raw"; // raw Data�� ���ؼ� -90�� ȸ��
	char InFileName[30] = "out.bmp"; // .bmp ����̹��� �ޱ�(BMP ����� �ޱ� ����)
	char OutFileName[30] = "lena_90out.bmp";
	const int width = 512;

	FILE* infile; //file pointer
	FILE* rawfile;
	infile = fopen(InFileName, "rb");
	rawfile = fopen(RawFileName, "rb");
	if (infile == NULL)
	{
		printf("No File : %s", InFileName);
		return 0;
	}

	////bmp������� �ޱ� &HF &IF
	BITMAPFILEHEADER HF; //�������� ��� ����(����Ǿ� ����)
	BITMAPINFOHEADER IF; //�������� ��� ����

	fread(&HF, sizeof(BITMAPFILEHEADER), 1, infile); // �������� ��� �б� //�о�� �ּ�&, �׸� Size, �׸� ����, �о�� �ּ�
	if (HF.bfType != 0x4D42) //bmp ������ �ƴϸ� ����
		exit(1);

	fread(&IF, sizeof(BITMAPINFOHEADER), 1, infile); // �������� ��� �б�
	if (IF.biBitCount != 8) //����̹��� Ȯ��(1pixel : 8bit)
		return 0;

	////�ȷ�Ʈ ���� �Է�
	RGBQUAD hRGB[256]; // �ȷ�Ʈ �� �迭 ���� 256���� ��
	fread(hRGB, sizeof(RGBQUAD), 256, infile); // �ȷ�Ʈ �Է�

	//Memory allocate
	BYTE* lpImg = new BYTE[IF.biSizeImage]; // ������ ���� ���� �޸� �Ҵ�, Image�� Size��ŭ �Ҵ�(Byte����)
	fread(lpImg, sizeof(char), IF.biSizeImage, rawfile); //Raw �������� �б�
	fclose(infile); // file �ݱ�

	int rowsize = WIDTHBYTES(IF.biBitCount * IF.biWidth); //pixel�� 8bit x width 1Byte*512
	unsigned char row[512][512] = { 0 };

	for (int i = 0; i < 512; i++) // �ݽð� ���� 90�� ȸ��
	{
		for (int j = 0; j < 512; j++)
		{
			row[511-j][i] = lpImg[(i * 512) + j]; //[�� : -->][�� : ||]
		}
	}

	//for (int i = 0; i < 512; i++) // �ð� ���� 90�� ȸ��
	//{
	//	for (int j = 0; j < 512; j++)
	//	{
	//		row[j][511 - i] = lpImg[(i * 512) + j]; //[�� : -->][�� : ||]
	//	}
	//}

	// 90�� ȸ���� (��/��)�̹��� ��/�� ������(bmp ���Ŀ� ���缭 �־��ֱ� ����)
	unsigned char temp = 0;
	for (int i = 0; i < rowsize / 2; i++) // lpImg ������ ������ Loop ��ܿ��� �߰����� 256pixel�� ���ؼ��� �����ϸ� ����������.
	{
		for (int j = 0; j < 512; j++)
		{
			temp = row[i][j];
			row[i][j] = row[511 - i][j]; // ù��� ������ ����� 1�྿ �������鼭 ���� Change
			row[511 - i][j] = temp;
		}
	}

	//������ output file�� ���� File Pointer���� fopen
	FILE* outfile = fopen(OutFileName, "wb");

	//������ file�� Data Write
	fwrite(&HF, sizeof(char), sizeof(BITMAPFILEHEADER), outfile); // ���� ����
	fwrite(&IF, sizeof(char), sizeof(BITMAPINFOHEADER), outfile); // ���� ����
	fwrite(hRGB, sizeof(RGBQUAD), 256, outfile); // �ȷ�Ʈ RGBQUAD size x 256����
	fwrite(row, sizeof(char), IF.biSizeImage, outfile); //���� Raw ������
	fclose(outfile);

	delete[] lpImg; //Memory ����

	return 0;
}

