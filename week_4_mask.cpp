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
		if (Img[i] + Val > 255) { // Ŭ����ó��
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
		NormSum[i] = (BYTE)(Ratio * AHisto[i]);// ����ȭ�� ������׷�
	}
	for (int i = 0; i < ImgSize; i++) {
		Out[i] = NormSum[Img[i]]; //����ȭ�� ������׷��� �̿��Ͽ� �Է¿����� �ٽ� ����
	}
}

void Binarization(BYTE* Img, BYTE* Out, int W, int H, BYTE Threshold) {// ����ȭ
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] < Threshold) Out[i] = 0;
		else Out[i] = 255;
	}
}
// �ڵ� �Ӱ�ġ ���� ��� ���� //
int GonzalezBinThresh(int* Histo) {
	int error = 3; // �̸� ���ǵ� ����
	BYTE Low, High; // ���󿡼� ����� �ּҰ��� �ִ밪 

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
	int estimated_T = (int)((Low + High) / 2); // ��谪 T�� ������
	printf("�ʱ��Ӱ谪: %d\n", estimated_T);
	T = estimated_T;

	do { // ��� ���� ��ȭ�� �̸� ���ǵ� �������� ���� ������ �ݺ�
		// ��谪 T�� ����ȭ
		G1_num = 0, G2_num = 0;
		G1 = 0, G2 = 0;

		for (int i = Low; i < T; i++) {
			// ��Ⱚ�� T���� ���� ȭ�ҵ�� ������ ����(G2)
			G2 += i * Histo[i];
			G2_num += Histo[i];
		}
		for (int i = T + 1; i <= High; i++) {
			// ��Ⱚ�� T���� ū ȭ�ҵ�� ������ ����(G1)
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

	printf("�Ӱ谪: %d\n", T);
	return T;
}
void SaveBMPFile(BITMAPFILEHEADER hf, BITMAPINFOHEADER hInfo, 
	RGBQUAD* hRGB, BYTE* Output, int W, int H, const char* FileName) {
	FILE* fp = fopen(FileName, "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // ��� 
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), W*H, fp);
	fclose(fp); 
}

void AverageConv(BYTE* Img, BYTE* Out, int W, int H) // �ڽ���Ȱȭ
{
	double Kernel[3][3] = { 0.11111, 0.11111, 0.11111,
							0.11111, 0.11111, 0.11111,
							0.11111, 0.11111, 0.11111 };
	double SumProduct = 0.0;

	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��) ���Ʒ� 1�پ� ����. 
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}// (3,2) 5x4 y*w+x 2*5+3=13 ���� �������迭 .
				// ��� �״�� ���� �� Ŀ�� ���� 1�̹Ƿ� 
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

void GaussAvrConv(BYTE* Img, BYTE* Out, int W, int H) // ����þ���Ȱȭ
{
	double Kernel[3][3] = { 0.0625, 0.125, 0.0625,
							0.125, 0.25, 0.125,
							0.0625, 0.125, 0.0625 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}

void Prewitt_X_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt ����ũ X
{
	double Kernel[3][3] = { -1.0, 0.0, 1.0,
							-1.0, 0.0, 1.0,
							-1.0, 0.0, 1.0 }; //x�������� ���� ���� ��ȭ
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 765  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}

void Prewitt_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt ����ũ X
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
							0.0, 0.0, 0.0,
							1.0, 1.0, 1.0 };// ���Ʒ� ���� ��� ���� �ִ� ��� 

	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 765  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 3;
			SumProduct = 0.0;
		}
	}
}

void Sobel_X_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt ����ũ X
{
	double Kernel[3][3] = { -1.0, 0.0, 1.0,
										-2.0, 0.0, 2.0,
										-1.0, 0.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 1020  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}

void Sobel_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt ����ũ X
{
	double Kernel[3][3] = { -1.0, -2.0, -1.0,
										0.0, 0.0, 0.0,
										1.0, 2.0, 1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 765  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 4;
			SumProduct = 0.0;
		}
	}
}

void Laplace_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt ����ũ X
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
										-1.0, 8.0, -1.0,
										-1.0, -1.0, -1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			// 0 ~ 2040  =====> 0 ~ 255
			Out[i * W + j] = abs((long)SumProduct) / 8;
			SumProduct = 0.0;
		}
	}
}// ��� ���⿡ ���� ��� ���. ����� �ΰ�. ����� ���� ����


void Laplace_Conv_DC(BYTE* Img, BYTE* Out, int W, int H) // Prewitt ����ũ X
{
	double Kernel[3][3] = { -1.0, -1.0, -1.0,
										-1.0, 9.0, -1.0,
										-1.0, -1.0, -1.0 };
	double SumProduct = 0.0;
	for (int i = 1; i < H - 1; i++) { // Y��ǥ (��)
		for (int j = 1; j < W - 1; j++) { // X��ǥ (��)
			for (int m = -1; m <= 1; m++) { // Kernel ��
				for (int n = -1; n <= 1; n++) { // Kernel ��
					SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
				}
			}
			//Out[i * W + j] = abs((long)SumProduct) / 8;
			// Ŭ���� ó��
			if (SumProduct > 255.0) Out[i * W + j] = 255;
			else if (SumProduct < 0.0) Out[i * W + j] = 0;
			else Out[i * W + j] = (BYTE)SumProduct;
			SumProduct = 0.0;
		}
	}
}// ���� ������ ��չ�� ����. 
void main()
{
	BITMAPFILEHEADER hf; // BMP ������� 14Bytes
	BITMAPINFOHEADER hInfo; // BMP ������� 40Bytes
	RGBQUAD hRGB[256]; // �ȷ�Ʈ (256 * 4Bytes)
	FILE* fp;
	fp = fopen("lenna.bmp", "rb"); // ���̳ʸ� ���� ���·� ��� �б� 
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

	

	//ObtainHistogram(Image, Histo, hInfo.biWidth, hInfo.biHeight);
	//ObtainAHistogram(Histo, AHisto);
	//HistogramEqualization(Image, Output, AHisto, hInfo.biWidth, hInfo.biHeight);
	
	//HistogramStretching(Image, Output, Histo, hInfo.biWidth, hInfo.biHeight);
	//int Thres = GonzalezBinThresh(Histo);
	//Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, Thres);
	
	

	//InverseImage(Image, Output, hInfo.biWidth, hInfo.biHeight);
	//BrightnessAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 70);
	//ContrastAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 1.5);
	
	// Output1 - ����
	SaveBMPFile(hf, hInfo, hRGB, Image, hInfo.biWidth, hInfo.biHeight, "output1.bmp");
	// Output2 - ���� ������ ��Ⱚ�� 50��ŭ ����
	for (int i = 0; i < ImgSize; i++) Output[i] = Image[i]+ 50; 
	SaveBMPFile(hf, hInfo, hRGB, Output, hInfo.biWidth, hInfo.biHeight, "output2.bmp");
	// Output3 - �������
	for (int i = 0; i < ImgSize; i++) Output[i] = 255 - Image[i]; // ����. 3���� ����*/
	SaveBMPFile(hf, hInfo, hRGB, Output, hInfo.biWidth, hInfo.biHeight, "output3.bmp");
	
	free(Image); // �����Ҵ��� �̹��� ���� ���̻� ������� �����Ƿ� ..
	// memory leak. �����Ϳ� heap ���� ������.
	free(Output);
}