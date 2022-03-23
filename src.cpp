// 기본틀 
#pragma warning(disable:4996) // scanf 오류 발생시키지 말라. 
#include <stdio.h> // 파일 입출력 함수 사용.
#include <stdlib.h> // 메모리 동적할당 함수 malloc 사용.
#include <Windows.h> // BMP 파일 처리하는데 필요한 함수 사용. 

// 영상 처리 함수 따로 처리
void InverseImage(BYTE* Img, BYTE* Out, int W, int H)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
		Out[i] = 255 - Img[i]; // 역상. 
}

void BrightnessAdj(BYTE* Img, BYTE* Out, int W, int H, int Val)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) 
	{
		if (Img[i] + Val > 255) {
			Out[i] = 255;
		}
		else if (Img[i] + Val < 0) {
			Out[i] = 0;
		}
		else {
			Out[i] = Img[i] + Val; // 밝기 조절
		}
	}
}

void ContrastAdj(BYTE* Img, BYTE* Out, int W, int H, double Val) 
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		if (Img[i] * Val > 255.0) {
			Out[i] = 255;
		}
		else {
			Out[i] = (BYTE)(Img[i] * Val); // 대비 조절. 
			// Out은 byte 타입. double을 byte로 강제 형변환 필요. (실수→정수)
		}
	}
}

void ObtainHistogram(BYTE* Img, int* Histo, int W, int H) {
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		Histo[Img[i]]++;
	}
}
void ObtainAHistogram(int* Histo, int* AHisto) {
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j <= i; j++) {
			AHisto[i] += Histo[j];
		}
	}
}
void HistogramStretching(BYTE* Img, BYTE* Out, int* Histo, int W, int H) {
	int ImgSize = W * H;
	BYTE Low, High;
	for (int i = 0; i < 256; i++)
	{
		if (Histo[i] != 0) {
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--)
	{
		if (Histo[i] != 0) {
			High = i;
			break;
		}
	}
	//스트레칭
	for (int i = 0; i < ImgSize; i++) {
		Out[i] = (BYTE)(Img[i] - Low) / (double)(High - Low) * 255.0;
	}
}

void HistogramEqualization(BYTE* Img, BYTE* Out, int* AHisto, int W, int H) {
	int ImgSize = W * H;
	int Nt = W * H, Gmax = 255;
	double Ratio = Gmax / (double)Nt;
	BYTE NormSum[256];
	for (int i = 0; i < 256; i++) {
		NormSum[i] = (BYTE)(Ratio * AHisto[i]);
	}
	for (int i = 0; i < ImgSize; i++) {
		Out[i] = NormSum[Img[i]];
	}
}

void Binarization(BYTE* Img, BYTE* Out, int W, int H, BYTE Threshold) {
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] < Threshold) Out[i] = 0;
		else Out[i] = 255;
	}
}

int GonzalesBinThresh() { // 자동 임계치 결정 방법 구현 //

}
void main()
{
	BITMAPFILEHEADER hf; // BMP 파일헤더 14Bytes
	BITMAPINFOHEADER hInfo; // BMP 인포헤더 40Bytes
	RGBQUAD hRGB[256]; // 팔레트 (256 * 4Bytes)
	FILE* fp;
	fp = fopen("coin.bmp", "rb"); // 바이너리 파일 형태로 열어서 읽기 
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

	BYTE* Output = (BYTE*)malloc(ImgSize); // lenna.bmp 파일과 동일한 영상
	//BYTE* Output2 = (BYTE*)malloc(ImgSize); // 원본 영상의 밝기값을 50만큼 증가시킨 영상
	//BYTE* Output3 = (BYTE*)malloc(ImgSize); // 원본 영상의 반전 결과 영상
	fread(Image, sizeof(BYTE), ImgSize, fp);
	fclose(fp);

	int Histo[256] = { 0 };
	int AHisto[256] = { 0 }; //누적히스토그램

	

	/*ObtainHistogram(Image, Histo, hInfo.biWidth, hInfo.biHeight);
	ObtainAHistogram(Histo, AHisto);
	HistogramEqualization(Image, Output, AHisto, hInfo.biWidth, hInfo.biHeight);*/
	
	//HistogramStretching(Image, Output, Histo, hInfo.biWidth, hInfo.biHeight);
	int Thres = GonzalesBinThresh();

	Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, Thres);
	
	/* 영상처리 {

	// Output1 - 원본
	for (int i = 0; i < ImgSize; i++)
		Output1[i] = Image[i]; 
	
	// Output2 - 원본 영상의 밝기값을 50만큼 증가
	for (int i = 0; i < ImgSize; i++)
		Output2[i] = Image[i] + 50; 

	// Output3 - 영상반전
	for (int i = 0; i < ImgSize; i++)
		Output3[i] = 255 - Image[i]; // 역상. 3주차 과제*/

	//InverseImage(Image, Output, hInfo.biWidth, hInfo.biHeight);
	//BrightnessAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 70);
	//ContrastAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 1.5);

	fp = fopen("output_binarization.bmp", "wb"); 
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // 기록 
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), ImgSize, fp);
	/*
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
	fclose(fp);*/
	free(Image); // 동적할당한 이미지 파일 더이상 사용하지 않으므로 ..
	// memory leak. 포인터와 heap 영역 끊어줌.
	free(Output);
	//free(Output2);
	//free(Output3);
}