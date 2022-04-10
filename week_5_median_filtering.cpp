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
		if (Img[i] + Val > 255) { // 클리핑처리
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
		NormSum[i] = (BYTE)(Ratio * AHisto[i]);// 정규화합 히스토그램
	}
	for (int i = 0; i < ImgSize; i++) {
		Out[i] = NormSum[Img[i]]; //정규화합 히스토그램을 이용하여 입력영상을 다시 매핑
	}
}

void Binarization(BYTE* Img, BYTE* Out, int W, int H, BYTE Threshold) {// 이진화
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] < Threshold) Out[i] = 0;
		else Out[i] = 255;
	}
}
// 자동 임계치 결정 방법 구현 //
int GonzalezBinThresh(int* Histo) {
	int error = 3; // 미리 정의된 오차
	BYTE Low, High; // 영상에서 밝기의 최소값과 최대값 

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
	int G1_num, G2_num, G1, G2, T;
	double avg_G1, avg_G2;
	int estimated_T = (int)((Low + High) / 2); // 경계값 T의 추정값
	printf("초기임계값: %d\n", estimated_T);
	T = estimated_T;

	do { // 경계 값의 변화가 미리 정의된 오차보다 작을 때까지 반복
		// 경계값 T로 이진화
		G1_num = 0, G2_num = 0;
		G1 = 0, G2 = 0;

		for (int i = Low; i < T; i++) {
			// 밝기값이 T보다 작은 화소들로 구성된 영역(G2)
			G2 += i * Histo[i];
			G2_num += Histo[i];
		}
		for (int i = T + 1; i <= High; i++) {
			// 밝기값이 T보다 큰 화소들로 구성된 영역(G1)
			G1 += i * Histo[i];
			G1_num += Histo[i];
		}
		avg_G1 = G1 / (double)G1_num;
		avg_G2 = G2 / (double)G2_num;
		T = (int)((avg_G1 + avg_G2) / 2);
		if (abs(T - estimated_T) <= error) {
			break;
		}
		estimated_T = T;
	} while (1);

	printf("임계값: %d\n", T);
	return T;
}
void SaveBMPFile(BITMAPFILEHEADER hf, BITMAPINFOHEADER hInfo, 
	RGBQUAD* hRGB, BYTE* Output, int W, int H, const char* FileName) {
	FILE* fp = fopen(FileName, "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // 기록 
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), W*H, fp);
	fclose(fp); 
}

void AverageConv(BYTE* Img, BYTE* Out, int W, int H) // 박스평활화
{
	double Kernel[3][3] = { 0.11111, 0.11111, 0.11111,
							0.11111, 0.11111, 0.11111,
							0.11111, 0.11111, 0.11111 };
	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) { // Y좌표 (행) 위아래 1줄씩 마진. 
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}// (3,2) 5x4 y*w+x 2*5+3=13 영상 일차원배열 .
				// 밝기 그대로 유지 → 커널 합이 1이므로 
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

void GaussAvrConv(BYTE* Img, BYTE* Out, int W, int H) // 가우시안평활화
{
	double Kernel[3][3] = { 0.0625, 0.125, 0.0625,
							0.125, 0.25, 0.125,
							0.0625, 0.125, 0.0625 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

void Prewitt_X_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
	double Kernel[3][3] = { -1.0, 0.0, 1.0,
							-1.0, 0.0, 1.0,
							-1.0, 0.0, 1.0 }; //x방향으로 갔을 때의 변화
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 765  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}

void Prewitt_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
							0.0, 0.0, 0.0,
							1.0, 1.0, 1.0 };// 위아래 방향 경계 차이 있는 경우 

	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 765  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}

void Sobel_X_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
	double Kernel[3][3] = { -1.0, 0.0, 1.0,
										-2.0, 0.0, 2.0,
										-1.0, 0.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 1020  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}

void Sobel_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
	double Kernel[3][3] = { -1.0, -2.0, -1.0,
										0.0, 0.0, 0.0,
										1.0, 2.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 765  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}

void Laplace_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
										-1.0, 8.0, -1.0,
										-1.0, -1.0, -1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 2040  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 8;
			SumProduct = 0.0;
		}
	}
}// 모든 방향에 대한 경계 출력. 노이즈에 민감. 노이즈도 경계로 검출


void Laplace_Conv_DC(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
										-1.0, 9.0, -1.0,
										-1.0, -1.0, -1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
		for (int j = 1; j < W - 1; j++) { // X좌표 (열)
			for (int m = -1; m <= 1; m++) { // Kernel 행
				for (int n = -1; n <= 1; n++) { // Kernel 열
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			//Out[i * W + j] = abs((long)SumProduct) / 8;
			// 클리핑 처리
			if (SumProduct > 255.0) Out[i * W + j] = 255;
			else if (SumProduct < 0.0) Out[i * W + j] = 0;
			else Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}// 원래 영상의 평균밝기 유지. 

void swap(BYTE* a, BYTE* b)
{
	BYTE temp = *a;
	*a = *b;
	*b = temp;
}

BYTE Median(BYTE* arr, int size) // temp 배열 정렬해서 중간값 추출. 
{   // bubble sorting(두개씩(pivot과) 비교해서 swap)
	// 오름차순 정렬
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // 비교대상 index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);
		}
	}
	return arr[S / 2];
}

BYTE MaxPooling(BYTE* arr, int size) // 가장 큰 값 출력 
{
	// 오름차순 정렬
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // 비교대상 index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);
		}
	}
	return arr[S - 1];
}

BYTE MinPooling(BYTE* arr, int size)
{
	// 오름차순 정렬
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // 비교대상 index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);
		}
	}
	return arr[0];
}
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

	BYTE* Output = (BYTE*)malloc(ImgSize); // lenna.bmp 파일과 동일한 영상
	//BYTE* Output2 = (BYTE*)malloc(ImgSize); // 원본 영상의 밝기값을 50만큼 증가시킨 영상
	//BYTE* Output3 = (BYTE*)malloc(ImgSize); // 원본 영상의 반전 결과 영상
	fread(Image, sizeof(BYTE), ImgSize, fp);
	fclose(fp);

	int Histo[256] = { 0 };
	int AHisto[256] = { 0 }; //누적히스토그램

	

	//ObtainHistogram(Image, Histo, hInfo.biWidth, hInfo.biHeight);
	//ObtainAHistogram(Histo, AHisto);
	//HistogramEqualization(Image, Output, AHisto, hInfo.biWidth, hInfo.biHeight);
	
	//HistogramStretching(Image, Output, Histo, hInfo.biWidth, hInfo.biHeight);
	//int Thres = GonzalezBinThresh(Histo);
	//Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, Thres);
	
	

	//InverseImage(Image, Output, hInfo.biWidth, hInfo.biHeight);
	//BrightnessAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 70);
	//ContrastAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 1.5);

		/* Median filtering */
	BYTE temp[9];
	int W = hInfo.biWidth, H = hInfo.biHeight;
	int i, j;
	for (i = 1; i < H - 1; i++) {
		for (j = 1; j < W - 1; j++) {
			temp[0] = Image[(i - 1) * W + j - 1];
			temp[1] = Image[(i - 1) * W + j];
			temp[2] = Image[(i - 1) * W + j + 1];
			temp[3] = Image[i * W + j - 1];
			temp[4] = Image[i * W + j]; // 센터 기준으로 
			temp[5] = Image[i * W + j + 1];
			temp[6] = Image[(i + 1) * W + j - 1];
			temp[7] = Image[(i + 1) * W + j];
			temp[8] = Image[(i + 1) * W + j + 1];
			//Output[i * W + j] = Median(temp, 9);
			//Output[i * W + j] = MaxPooling(temp, 9); // → 가장 큰 값을 밖으로 내보냄. 까만 노이즈만 껴있다.
			// 솔트 앤 페퍼 같이 있으므로 솔트 노이즈에 해당하는 것만 picking up. 
			// 페퍼 노이즈는 거의 없어지고, 솔트 노이즈 증가. 
			//Output[i * W + j] = MinPooling(temp, 9);
			// Max나 MinPooling은 솔트나 페퍼 노이즈 둘 중 하나가 영상에 많이 존재할 때 효율적으로 사용 가능. 
		}
	}
	/* Median filtering */

	//AverageConv(Image, Output, hInfo.biWidth, hInfo.biHeight); // 가우시안 잡음 제거하기 
	
	// Output1 - 원본
	//SaveBMPFile(hf, hInfo, hRGB, Image, hInfo.biWidth, hInfo.biHeight, "output1.bmp");
	// Output2 - 원본 영상의 밝기값을 50만큼 증가
	//for (int i = 0; i < ImgSize; i++) Output[i] = Image[i]+ 50; 
	//SaveBMPFile(hf, hInfo, hRGB, Output, hInfo.biWidth, hInfo.biHeight, "output2.bmp");
	// Output3 - 영상반전
	//for (int i = 0; i < ImgSize; i++) Output[i] = 255 - Image[i]; // 역상. 3주차 과제*/


	SaveBMPFile(hf, hInfo, hRGB, Output, hInfo.biWidth, hInfo.biHeight, "output3.bmp");
	
	free(Image); // 동적할당한 이미지 파일 더이상 사용하지 않으므로 ..
	// memory leak. 포인터와 heap 영역 끊어줌.
	free(Output);
}