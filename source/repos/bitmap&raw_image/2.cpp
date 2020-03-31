#define _CRT_SECURE_NO_WARNINGS
#define WIDTHBYTES(bits)  (((bits)+31)/32*4) //영상의 가로줄은 4Byte의 배수이다.

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

//문제 2번 소스 //2015253039 권진우
int main()
{
	char RawFileName[30] = "raw_image_2015253039.raw"; // bmp file로 만들 raw 이미지
	char InFileName[30] = "gray_image.bmp"; // .bmp 흑백이미지 받기
	char OutFileName[30] = "out.bmp"; // output bmp file 이미지
	const int width = 512; // IMAGE pixel WIDTH

	FILE *infile; //file pointer
	FILE *rawfile;
	infile = fopen(InFileName, "rb"); // "Read Binary"
	rawfile = fopen(RawFileName, "rb");
	if (infile == NULL || rawfile == NULL) // fopen 한 file이 둘 중 하나라도 없으면 종료
	{
		printf("No File : %s or %s", InFileName, RawFileName);
		return 0;
	}

	////bmp헤더정보 받기 &HF &IF
	BITMAPFILEHEADER HF; //파일정보 헤더 선언(내장되어 있음)
	BITMAPINFOHEADER IF; //영상정보 헤더 선언
	RGBQUAD hRGB[256]; // 팔레트 맵 배열 선언 256가지 색

	fread(&HF, sizeof(BITMAPFILEHEADER), 1, infile); // 파일정보 헤더 읽기 //읽어둘 주소&, 항목 Size, 항목 개수, 읽어올 포인터
	if (HF.bfType != 0x4D42) // .bmp Type이 아니면
		exit(1); // 종료

	//printf("%d %d", HF.bfReserved1, HF.bfReserved2);
	////4d42, 263222, 0, 0, 1078

	fread(&IF, sizeof(BITMAPINFOHEADER), 1, infile); // 영상정보 헤더 읽기
	if (IF.biBitCount != 8) //흑백이미지 확인(1pixel : 8bit) // RGB의 경우 256가지 색
		return 0;

	////팔레트 정보 입력
	fread(hRGB, sizeof(RGBQUAD), 256, infile); // 팔레트 입력

	//Memory allocate
	BYTE *lpImg = new BYTE[IF.biSizeImage]; // 저장할 영상에 대한 메모리 할당, Image의 Size만큼 할당(Byte단위)
	fread(lpImg, sizeof(char), IF.biSizeImage, rawfile); //Raw 영상데이터 읽기
	fclose(infile); // file 닫기
	fclose(rawfile); // file 닫기

	//이미지 역으로 뒤집어서 정상적인 방향으로 만들어주기(Raw 파일이 bmp 파일로 만들어질 때는 이미지가 뒤집어져서 들어가기 때문)
	unsigned char temp = 0;
	for (int i = 0; i < 256; i++) // lpImg 역으로 뒤집기 Loop 상단에서 중간까지 256pixel에 대해서만 수행하면 뒤집어진다.
	{
		for (int j = 0; j < 512; j++)
		{
			temp = lpImg[(i * 512) + j];
			lpImg[(i * 512) + j] = lpImg[((511 * 512) - (i * 512)) + j];
			lpImg[((511 * 512) - (i * 512)) + j] = temp;
		}
	}
	// int rowsize = WIDTHBYTES(IF.biBitCount * IF.biWidth); //pixel당 bit x width

	//생성할 output file에 대한 File Pointer선언 fopen "Write Binary"
	FILE* outfile = fopen(OutFileName, "wb");

	if (outfile == NULL)
	{
		printf("File Open Error.\n");
		return 0;
	}
	
	//생성할 file에 Data Write // bmp Type에 맞춰서 순서대로 넣기 (File정보, Image정보, 팔레트정보, Raw Data)
	fwrite(&HF, sizeof(char), sizeof(BITMAPFILEHEADER), outfile); // 파일 정보
	fwrite(&IF, sizeof(char), sizeof(BITMAPINFOHEADER), outfile); // 영상 정보
	fwrite(hRGB, sizeof(RGBQUAD), 256, outfile); // 팔레트 RGBQUAD size x 256가지 // 위 3가지 헤더가 있어야 bmp구조에 맞게 된다.
	fwrite(lpImg, sizeof(char), IF.biSizeImage, outfile); //영상 Raw 데이터
	fclose(outfile);

	delete[] lpImg; //Memory 해제

	return 0;
}

