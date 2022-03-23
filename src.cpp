// �⺻Ʋ 
#pragma warning(disable:4996) // scanf ���� �߻���Ű�� ����. 
#include <stdio.h> // ���� ����� �Լ� ���.
#include <stdlib.h> // �޸� �����Ҵ� �Լ� malloc ���.
#include <Windows.h> // BMP ���� ó���ϴµ� �ʿ��� �Լ� ���. 

// ���� ó�� �Լ� ���� ó��
void InverseImage(BYTE* Img, BYTE* Out, int W, int H)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
		Out[i] = 255 - Img[i]; // ����. 
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
			Out[i] = Img[i] + Val; // ��� ����
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
			Out[i] = (BYTE)(Img[i] * Val); // ��� ����. 
			// Out�� byte Ÿ��. double�� byte�� ���� ����ȯ �ʿ�. (�Ǽ�������)
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
	//��Ʈ��Ī
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

int GonzalesBinThresh() { // �ڵ� �Ӱ�ġ ���� ��� ���� //

}
void main()
{
	BITMAPFILEHEADER hf; // BMP ������� 14Bytes
	BITMAPINFOHEADER hInfo; // BMP ������� 40Bytes
	RGBQUAD hRGB[256]; // �ȷ�Ʈ (256 * 4Bytes)
	FILE* fp;
	fp = fopen("coin.bmp", "rb"); // ���̳ʸ� ���� ���·� ��� �б� 
								   // ������ ù��° �ּ� ��ȯ
	if (fp == NULL) return; // ���α׷� ����. 
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp); // hf �ּ�, �ι�° �Ķ���� ũ�⸸ŭ 1����Ʈ �� �ҷ��´�.
	// ���� ���������Ͱ� ����Ű�� �ִ� ���������� 14����Ʈ��ŭ �а� hf�� ����־��. 

	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fread(hRGB, sizeof(RGBQUAD), 256, fp); // �̸� ��ü�� �ּ� (�迭�� �̸��� �ּ�)
	// 4����Ʈ��ŭ 256�� �б�. 

	int ImgSize = hInfo.biWidth * hInfo.biHeight;
	BYTE* Image = (BYTE*)malloc(ImgSize); // �����Ҵ�. heap���ٰ� �Ҵ�. ���� ù��° �޸� �ּ��Ҵ��ϸ� Image������ ����. 
	// �Ȱ��� ����ȯ. �޸𸮸� �Ҵ��ߴµ� ó���� �� 1����Ʈ ������ ����ض� .
	// BYTE Ÿ������ Image ���� �����ϴµ� Image �迭 65536�� �ʿ�.

	BYTE* Output = (BYTE*)malloc(ImgSize); // lenna.bmp ���ϰ� ������ ����
	//BYTE* Output2 = (BYTE*)malloc(ImgSize); // ���� ������ ��Ⱚ�� 50��ŭ ������Ų ����
	//BYTE* Output3 = (BYTE*)malloc(ImgSize); // ���� ������ ���� ��� ����
	fread(Image, sizeof(BYTE), ImgSize, fp);
	fclose(fp);

	int Histo[256] = { 0 };
	int AHisto[256] = { 0 }; //����������׷�

	

	/*ObtainHistogram(Image, Histo, hInfo.biWidth, hInfo.biHeight);
	ObtainAHistogram(Histo, AHisto);
	HistogramEqualization(Image, Output, AHisto, hInfo.biWidth, hInfo.biHeight);*/
	
	//HistogramStretching(Image, Output, Histo, hInfo.biWidth, hInfo.biHeight);
	int Thres = GonzalesBinThresh();

	Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, Thres);
	
	/* ����ó�� {

	// Output1 - ����
	for (int i = 0; i < ImgSize; i++)
		Output1[i] = Image[i]; 
	
	// Output2 - ���� ������ ��Ⱚ�� 50��ŭ ����
	for (int i = 0; i < ImgSize; i++)
		Output2[i] = Image[i] + 50; 

	// Output3 - �������
	for (int i = 0; i < ImgSize; i++)
		Output3[i] = 255 - Image[i]; // ����. 3���� ����*/

	//InverseImage(Image, Output, hInfo.biWidth, hInfo.biHeight);
	//BrightnessAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 70);
	//ContrastAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 1.5);

	fp = fopen("output_binarization.bmp", "wb"); 
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // ��� 
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), ImgSize, fp);
	/*
	fclose(fp);
	fp = fopen("output2.bmp", "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // ��� 
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output2, sizeof(BYTE), ImgSize, fp);
	fclose(fp);

	fp = fopen("output3.bmp", "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // ��� 
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output3, sizeof(BYTE), ImgSize, fp);
	fclose(fp);*/
	free(Image); // �����Ҵ��� �̹��� ���� ���̻� ������� �����Ƿ� ..
	// memory leak. �����Ϳ� heap ���� ������.
	free(Output);
	//free(Output2);
	//free(Output3);
}