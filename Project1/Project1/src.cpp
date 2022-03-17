// �⺻Ʋ 
#pragma warning(disable:4996) // scanf ���� �߻���Ű�� ����. 
#include <stdio.h> // ���� ����� �Լ� ���.
#include <stdlib.h> // �޸� �����Ҵ� �Լ� malloc ���.
#include <Windows.h> // BMP ���� ó���ϴµ� �ʿ��� �Լ� ���. 
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

	BYTE* Output1 = (BYTE*)malloc(ImgSize); // lenna.bmp ���ϰ� ������ ����
	BYTE* Output2 = (BYTE*)malloc(ImgSize); // ���� ������ ��Ⱚ�� 50��ŭ ������Ų ����
	BYTE* Output3 = (BYTE*)malloc(ImgSize); // ���� ������ ���� ��� ����
	fread(Image, sizeof(BYTE), ImgSize, fp);
	fclose(fp);

	/* ����ó�� */

	// Output1 - ����
	for (int i = 0; i < ImgSize; i++)
		Output1[i] = Image[i]; 

	// Output2 - ���� ������ ��Ⱚ�� 50��ŭ ����
	for (int i = 0; i < ImgSize; i++)
		Output2[i] = Image[i] + 50; 

	// Output3 - �������
	for (int i = 0; i < ImgSize; i++)
		Output3[i] = 255 - Image[i]; // ����. 

	fp = fopen("output1.bmp", "wb"); 
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // ��� 
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output1, sizeof(BYTE), ImgSize, fp);
	
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
	fclose(fp);
	free(Image); // �����Ҵ��� �̹��� ���� ���̻� ������� �����Ƿ� ..
	// memory leak. �����Ϳ� heap ���� ������.
	free(Output1);
	free(Output2);
	free(Output3);
}