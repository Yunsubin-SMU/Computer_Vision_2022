// 기본틀 
#pragma warning(disable:4996) // scanf 오류 발생시키지 말라. 
#include <stdio.h> // 파일 입출력 함수 사용.
#include <stdlib.h> // 메모리 동적할당 함수 malloc 사용.
#include <Windows.h> // BMP 파일 처리하는데 필요한 함수 사용. 
void main()
{
	BITMAPFILEHEADER hf; // BMP 파일헤더 14Bytes
	BITMAPINFOHEADER hInfo; // BMP 인포헤더 40Bytes
	RGBQUAD hRGB[256]; // 팔레트 (256 * 4Bytes)
	FILE* fp; 
	fp = fopen("lenna.bmp", "rb"); // 바이너리 파일 형태로 열어서 읽기 
								   // 파일의 첫번째 주소 반환
	if (fp == NULL) return; // 프로그램 종료. 
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp); // hf 주소, 두번째 파라미터 크기만큼 1바이트 번 불러온다.
	// 현재 파일포인터가 가리키고 있는 곳에서부터 14바이트만큼 읽고 hf에 집어넣어라. 
	
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp); 
	fread(hRGB, sizeof(RGBQUAD), 256, fp); // 이름 자체가 주소 (배열의 이름이 주소)
	// 4바이트만큼 256번 읽기. 
	
	int ImgSize = hInfo.biWidth * hInfo.biHeight;
	BYTE* Image = (BYTE*)malloc(ImgSize); // 동적할당. heap에다가 할당. 제일 첫번째 메모리 주소할당하면 Image변수가 받음. 
	// 똑같이 형변환. 메모리를 할당했는데 처리할 때 1바이트 단위로 사용해라 .
	// BYTE 타입으로 Image 변수 선언하는데 Image 배열 65536개 필요.

	BYTE* Output1 = (BYTE*)malloc(ImgSize); // lenna.bmp 파일과 동일한 영상
	BYTE* Output2 = (BYTE*)malloc(ImgSize); // 원본 영상의 밝기값을 50만큼 증가시킨 영상
	BYTE* Output3 = (BYTE*)malloc(ImgSize); // 원본 영상의 반전 결과 영상
	fread(Image, sizeof(BYTE), ImgSize, fp);
	fclose(fp);

	/* 영상처리 */

	// Output1 - 원본
	for (int i = 0; i < ImgSize; i++)
		Output1[i] = Image[i]; 

	// Output2 - 원본 영상의 밝기값을 50만큼 증가
	for (int i = 0; i < ImgSize; i++)
		Output2[i] = Image[i] + 50; 

	// Output3 - 영상반전
	for (int i = 0; i < ImgSize; i++)
		Output3[i] = 255 - Image[i]; // 역상. 

	fp = fopen("output1.bmp", "wb"); 
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // 기록 
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output1, sizeof(BYTE), ImgSize, fp);
	
	fclose(fp);
	fp = fopen("output2.bmp", "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // 기록 
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output2, sizeof(BYTE), ImgSize, fp);
	fclose(fp);

	fp = fopen("output3.bmp", "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // 기록 
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output3, sizeof(BYTE), ImgSize, fp);
	fclose(fp);
	free(Image); // 동적할당한 이미지 파일 더이상 사용하지 않으므로 ..
	// memory leak. 포인터와 heap 영역 끊어줌.
	free(Output1);
	free(Output2);
	free(Output3);
}