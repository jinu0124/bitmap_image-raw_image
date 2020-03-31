#define _CRT_SECURE_NO_WARNINGS
#define WIDTHBYTES(bits)  (((bits)+31)/32*4) //영상의 가로줄은 4Byte의 배수이다.

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

//문제 3번 소스 //2015253039 권진우
int main()
{
	char RawFileName[30] = "lena_raw_512x512.raw"; // raw Data에 대해서 -90도 회전
	char InFileName[30] = "out.bmp"; // .bmp 흑백이미지 받기(BMP 헤더를 받기 위해)
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

	////bmp헤더정보 받기 &HF &IF
	BITMAPFILEHEADER HF; //파일정보 헤더 선언(내장되어 있음)
	BITMAPINFOHEADER IF; //영상정보 헤더 선언

	fread(&HF, sizeof(BITMAPFILEHEADER), 1, infile); // 파일정보 헤더 읽기 //읽어둘 주소&, 항목 Size, 항목 개수, 읽어올 주소
	if (HF.bfType != 0x4D42) //bmp 파일이 아니면 종료
		exit(1);

	fread(&IF, sizeof(BITMAPINFOHEADER), 1, infile); // 영상정보 헤더 읽기
	if (IF.biBitCount != 8) //흑백이미지 확인(1pixel : 8bit)
		return 0;

	////팔레트 정보 입력
	RGBQUAD hRGB[256]; // 팔레트 맵 배열 선언 256가지 색
	fread(hRGB, sizeof(RGBQUAD), 256, infile); // 팔레트 입력

	//Memory allocate
	BYTE* lpImg = new BYTE[IF.biSizeImage]; // 저장할 영상에 대한 메모리 할당, Image의 Size만큼 할당(Byte단위)
	fread(lpImg, sizeof(char), IF.biSizeImage, rawfile); //Raw 영상데이터 읽기
	fclose(infile); // file 닫기

	int rowsize = WIDTHBYTES(IF.biBitCount * IF.biWidth); //pixel당 8bit x width 1Byte*512
	unsigned char row[512][512] = { 0 };

	for (int i = 0; i < 512; i++) // 반시계 방향 90도 회전
	{
		for (int j = 0; j < 512; j++)
		{
			row[511-j][i] = lpImg[(i * 512) + j]; //[행 : -->][열 : ||]
		}
	}

	//for (int i = 0; i < 512; i++) // 시계 방향 90도 회전
	//{
	//	for (int j = 0; j < 512; j++)
	//	{
	//		row[j][511 - i] = lpImg[(i * 512) + j]; //[행 : -->][열 : ||]
	//	}
	//}

	// 90도 회전된 (행/열)이미지 상/하 뒤집기(bmp 형식에 맞춰서 넣어주기 위해)
	unsigned char temp = 0;
	for (int i = 0; i < rowsize / 2; i++) // lpImg 역으로 뒤집기 Loop 상단에서 중간까지 256pixel에 대해서만 수행하면 뒤집어진다.
	{
		for (int j = 0; j < 512; j++)
		{
			temp = row[i][j];
			row[i][j] = row[511 - i][j]; // 첫행와 마지막 행부터 1행씩 좁혀가면서 값을 Change
			row[511 - i][j] = temp;
		}
	}

	//생성할 output file에 대한 File Pointer선언 fopen
	FILE* outfile = fopen(OutFileName, "wb");

	//생성할 file에 Data Write
	fwrite(&HF, sizeof(char), sizeof(BITMAPFILEHEADER), outfile); // 파일 정보
	fwrite(&IF, sizeof(char), sizeof(BITMAPINFOHEADER), outfile); // 영상 정보
	fwrite(hRGB, sizeof(RGBQUAD), 256, outfile); // 팔레트 RGBQUAD size x 256가지
	fwrite(row, sizeof(char), IF.biSizeImage, outfile); //영상 Raw 데이터
	fclose(outfile);

	delete[] lpImg; //Memory 해제

	return 0;
}

