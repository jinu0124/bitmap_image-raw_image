#define _CRT_SECURE_NO_WARNINGS
#define WIDTHBYTES(bits)  (((bits)+31)/32*4) //������ �������� 4Byte�� ����̴�.

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

//���� 2�� �ҽ� //2015253039 ������
int main()
{
	char RawFileName[30] = "raw_image_2015253039.raw"; // bmp file�� ���� raw �̹���
	char InFileName[30] = "gray_image.bmp"; // .bmp ����̹��� �ޱ�
	char OutFileName[30] = "out.bmp"; // output bmp file �̹���
	const int width = 512; // IMAGE pixel WIDTH

	FILE *infile; //file pointer
	FILE *rawfile;
	infile = fopen(InFileName, "rb"); // "Read Binary"
	rawfile = fopen(RawFileName, "rb");
	if (infile == NULL || rawfile == NULL) // fopen �� file�� �� �� �ϳ��� ������ ����
	{
		printf("No File : %s or %s", InFileName, RawFileName);
		return 0;
	}

	////bmp������� �ޱ� &HF &IF
	BITMAPFILEHEADER HF; //�������� ��� ����(����Ǿ� ����)
	BITMAPINFOHEADER IF; //�������� ��� ����
	RGBQUAD hRGB[256]; // �ȷ�Ʈ �� �迭 ���� 256���� ��

	fread(&HF, sizeof(BITMAPFILEHEADER), 1, infile); // �������� ��� �б� //�о�� �ּ�&, �׸� Size, �׸� ����, �о�� ������
	if (HF.bfType != 0x4D42) // .bmp Type�� �ƴϸ�
		exit(1); // ����

	//printf("%d %d", HF.bfReserved1, HF.bfReserved2);
	////4d42, 263222, 0, 0, 1078

	fread(&IF, sizeof(BITMAPINFOHEADER), 1, infile); // �������� ��� �б�
	if (IF.biBitCount != 8) //����̹��� Ȯ��(1pixel : 8bit) // RGB�� ��� 256���� ��
		return 0;

	////�ȷ�Ʈ ���� �Է�
	fread(hRGB, sizeof(RGBQUAD), 256, infile); // �ȷ�Ʈ �Է�

	//Memory allocate
	BYTE *lpImg = new BYTE[IF.biSizeImage]; // ������ ���� ���� �޸� �Ҵ�, Image�� Size��ŭ �Ҵ�(Byte����)
	fread(lpImg, sizeof(char), IF.biSizeImage, rawfile); //Raw �������� �б�
	fclose(infile); // file �ݱ�
	fclose(rawfile); // file �ݱ�

	//�̹��� ������ ����� �������� �������� ������ֱ�(Raw ������ bmp ���Ϸ� ������� ���� �̹����� ���������� ���� ����)
	unsigned char temp = 0;
	for (int i = 0; i < 256; i++) // lpImg ������ ������ Loop ��ܿ��� �߰����� 256pixel�� ���ؼ��� �����ϸ� ����������.
	{
		for (int j = 0; j < 512; j++)
		{
			temp = lpImg[(i * 512) + j];
			lpImg[(i * 512) + j] = lpImg[((511 * 512) - (i * 512)) + j];
			lpImg[((511 * 512) - (i * 512)) + j] = temp;
		}
	}
	// int rowsize = WIDTHBYTES(IF.biBitCount * IF.biWidth); //pixel�� bit x width

	//������ output file�� ���� File Pointer���� fopen "Write Binary"
	FILE* outfile = fopen(OutFileName, "wb");

	if (outfile == NULL)
	{
		printf("File Open Error.\n");
		return 0;
	}
	
	//������ file�� Data Write // bmp Type�� ���缭 ������� �ֱ� (File����, Image����, �ȷ�Ʈ����, Raw Data)
	fwrite(&HF, sizeof(char), sizeof(BITMAPFILEHEADER), outfile); // ���� ����
	fwrite(&IF, sizeof(char), sizeof(BITMAPINFOHEADER), outfile); // ���� ����
	fwrite(hRGB, sizeof(RGBQUAD), 256, outfile); // �ȷ�Ʈ RGBQUAD size x 256���� // �� 3���� ����� �־�� bmp������ �°� �ȴ�.
	fwrite(lpImg, sizeof(char), IF.biSizeImage, outfile); //���� Raw ������
	fclose(outfile);

	delete[] lpImg; //Memory ����

	return 0;
}

