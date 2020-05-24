#include "LookImgNum.h"
#include <stdio.h>
#include <memory.h>

// ...
LookImgNum::LookImgNum()
{
	m_pPixels = nullptr;
}

// ...
LookImgNum::~LookImgNum()
{
	if (m_pPixels)
		delete m_pPixels;
	m_pPixels = nullptr;
}

// pixels=����ָ�� width=��� height=�߶�
LookImgNum::LookImgNum(int * pixels, int width, int height, bool opengl, int byte)
{
	SetPixels(pixels, width, height, opengl, byte);
}

// ����������Ϣ pixels=����ָ�� width=��� height=�߶�
void LookImgNum::SetPixels(int* pixels, int width, int height, bool opengl, int byte)
{
	if (m_pPixels) {
		if ((width * height) > (m_nMallocWidth * m_nMallocHeight)) { // ��ǰ�ڴ�Ĳ�����
			delete m_pPixels;
			m_pPixels = nullptr;
		}
	}
	if (!m_pPixels) {
		m_pPixels = new int[width * height];
		m_nMallocWidth = width;
		m_nMallocHeight = height;
	}

	m_bOpengl = opengl;
	m_nWidth = width;
	m_nHeight = height;
	memcpy(m_pPixels, pixels, width*height*byte);

	//printf("SetPixels Ok\n");
}

// ��ȡ������Ϣ
int LookImgNum::GetPixel(int x, int y)
{
	//printf("GetPixel:%d,%d\n", x, y);
	if (0 && m_bOpengl)
		return m_pPixels[(m_nHeight-y-1)* m_nWidth + x];

	return m_pPixels[y * m_nWidth + x];
}

// ��ȡ����
int LookImgNum::GetNum(int color, int diff, int d_v)
{
	int num = d_v;
	char text[16];
	if (!GetText(text, color, diff, &num))
		return d_v;

	//PrintWords(color, diff);
	//printf("����:%s ����:%d\n", text, num);
	return num;
}

// ��ȡ����
int LookImgNum::GetText(char* text, int color, int diff, int* pnum)
{
	m_nFindWords = 0;
	FindWords(color, diff);

	int num = 0;
	int length = 0;
	int model_pixel_count = m_nModelWidth * m_nModelHeight;
	int best_count = model_pixel_count - (model_pixel_count / 10);
	for (int i = 0; i < m_nFindWords; i++) {
		int match_word = 0, match_count = 0;
		for (int num = 0; num <= 9; num++) {
			int count = PregModels(i, num, color, diff);
			//printf("%dƥ������:%d\n", num, count);
			if (count > match_count) {
				match_word = num;
				match_count = count;
				if (match_count == model_pixel_count)
					break;
			}
		}
		if (match_count > best_count) {
			num = num * 10 + match_word;
			char ch = match_word + '0';
			//printf("best_count:%d ch:%c\n", best_count, ch);
			text[length++] = ch;
		}
		//printf("\n-----------------------\n");
	}
	text[length] = 0;

	if (pnum)
		*pnum = num;

	return length;
}

// ƥ����ģ, ����ƥ������
int LookImgNum::PregModels(int word_index, int model_index, int color, int diff)
{
	if (word_index >= m_nFindWords)
		return 0;

	int* models = GetModels(model_index);
	if (models == nullptr)
		return 0;

	int preg_width = m_WordInfos[word_index].EndX - m_WordInfos[word_index].StartX + 1;
	int preg_height = m_WordInfos[word_index].EndY - m_WordInfos[word_index].StartY + 1;
	if (preg_width > m_nModelWidth)
		preg_width = m_nModelWidth;
	if (preg_height > m_nModelHeight)
		preg_height = m_nModelHeight;

	int no_count = 0;
	for (int y = 0; y < preg_height; y++) {
		for (int x = 0; x < preg_width; x++) {
			int pixel = GetPixel(m_WordInfos[word_index].StartX + x, m_WordInfos[word_index].StartY + y);
			int v = IsThePixel(pixel, color, diff) ? 1 : 0;
			if (v != models[y * m_nModelWidth + x]) { // ��ƥ��
				if (model_index == 8) {
					//printf("%d,%d %08X v:%d\n", x, y, pixel, v);
				}
				no_count++;
			}
		}
	}
	//printf("%d:preg_width:%d preg_height:%d ��ƥ��:%d\n", model_index, preg_width, preg_height, no_count);
	return (m_nModelWidth * m_nModelHeight) - no_count;
}

// ��ȡ��ģ����
int* LookImgNum::GetModels(int index)
{
	if (index == 0)
		return g_WordModels_0;
	if (index == 1)
		return g_WordModels_1;
	if (index == 2)
		return g_WordModels_2;
	if (index == 3)
		return g_WordModels_3;
	if (index == 4)
		return g_WordModels_4;
	if (index == 5)
		return g_WordModels_5;
	if (index == 6)
		return g_WordModels_6;
	if (index == 7)
		return g_WordModels_7;
	if (index == 8)
		return g_WordModels_8;
	if (index == 9)
		return g_WordModels_9;

	return nullptr;
}

// ��������Ϣ
int LookImgNum::FindWords(int color, int diff)
{
	m_nFindWords = 0;

	int start_x = -1, end_x = -1, start_y = -1, end_y = -1;
	for (int x = 0; x < m_nWidth; x++) {
		int y_match_count = 0;
		for (int y = 0; y < m_nHeight; y++) {
			if (IsThePixel(GetPixel(x, y), color, diff)) {
				if (start_x == -1) {
					start_x = x;
				}

				if (start_y == -1) {
					start_y = y;
				}
				else if (start_y > y) {
					start_y = y;
				}

				if (end_y == -1) {
					end_y = y;
				}
				else if (end_y < y) {
					end_y = y;
				}

				y_match_count++;
			}
		}

		if (start_x != -1 && (y_match_count == 0 || (x + 1) == m_nWidth)) {
			//printf("�ҵ���, ���:%d,%d λ��:%d-%d %d-%d\n", x-start_x, end_y - start_y + 1, start_x, x, start_y, end_y);

			if ((x - start_x) >= m_nModelWidth - 2 && (end_y - start_y) >= m_nModelHeight - 2) {
				m_WordInfos[m_nFindWords].StartX = start_x;
				m_WordInfos[m_nFindWords].EndX = x - 1;
				m_WordInfos[m_nFindWords].StartY = start_y;
				m_WordInfos[m_nFindWords].EndY = end_y;
				m_nFindWords++;
			}

			start_x = -1, end_x = -1, start_y = -1, end_y = -1;
		}
	}

	return m_nFindWords;
}

// ��ӡ��
void LookImgNum::PrintWords(int color, int diff)
{
	for (int i = 0; i < m_nFindWords; i++) {
		for (int y = m_WordInfos[i].StartY; y <= m_WordInfos[i].EndY; y++) {
			for (int x = m_WordInfos[i].StartX; x <= m_WordInfos[i].EndX; x++) {
				if (IsThePixel(GetPixel(x, y), color, diff)) {
					printf("1 ");
				}
				else {
					printf(" ");
				}
			}
			printf("\n");
		}
		printf("\n\n");
	}
}

// �����Ƿ����Ҫ�� pixel=ԭ����ԭɫ color=�Ƚϵ���ɫֵ diff=����Ĳ�ֵ
bool LookImgNum::IsThePixel(int pixel, int color, int diff)
{
	if (diff > 0) {
		for (int i = 0; i < 3; i++) {
			int v = (pixel >> (i * 8)) & 0xff;
			int c = (color >> (i * 8)) & 0xff;
			int d = (diff >> (i * 8)) & 0xff;
			if (v < (c - d) || v > (c + d)) { // ������Ҫ��
				return false;
			}
		}
		return true;
	}

	return (pixel&0x00ffffff) == (color&0x00ffffff);
}
