#pragma once
#include "pch.h"
#include "afxwin.h"
#include <math.h>
#include <vector>
#include "NghUtil.h"

int RandVal(int nStart, int nEnd)
{
	srand((unsigned int)time(0));

	return rand() % ((nEnd + 1) + nStart);
}

void MaxVal(long long* lpVal, int nCnt, LONG* lpResult)
{
	__int64 lMax = 0;
	for (int i = 0; i < nCnt; i++)
	{
		lMax = max(lpVal[i], lMax);
	}

	for (int i = nCnt - 1; i >= 0; i--)
	{
		if (lMax == lpVal[i])
		{
			*lpResult = i;
			break;
		}
	}
}

void Image16bitTo8bit(WORD * pSrc, BYTE * pDst, int nWidth, int nHeigh)
{
	for (int y = 0; y < nHeigh; y++)
	{
		for (int x = 0; x < nWidth; x++)
		{
			double dVal = (pSrc[(y * nWidth) + x] / 256.);
			BYTE byVal = 0;
			if (dVal > 255)
				dVal = 255;

			byVal = (BYTE)dVal;
			pDst[(y * nWidth) + x] = byVal;
		}
	}
}

#ifdef MIL_USE
void Mult_Image(CMilBuffer * src1, CMilBuffer * src2, CMilBuffer * dst, double dMult, double dOffset)
{
	tImageProperties src1Size, src2Size, dstSize;
	src1->GetImageProperties(&src1Size);
	src2->GetImageProperties(&src2Size);
	dst->GetImageProperties(&dstSize);


	BYTE* bySrc1, *bySrc2, * byDst;
	bySrc1 = new BYTE[src1Size.ImageSize.cx * src1Size.ImageSize.cy];
	src1->Get2D(bySrc1, CRect(0, 0, src1Size.ImageSize.cx, src1Size.ImageSize.cy));
	bySrc2 = new BYTE[src1Size.ImageSize.cx * src1Size.ImageSize.cy];
	src2->Get2D(bySrc2, CRect(0, 0, src2Size.ImageSize.cx, src2Size.ImageSize.cy));

	byDst = new BYTE[dstSize.ImageSize.cx * dstSize.ImageSize.cy];
	dst->Get2D(byDst, CRect(0, 0, dstSize.ImageSize.cx, dstSize.ImageSize.cy));

	double dVal = 0.;
	for (int h = 0; h < dstSize.ImageSize.cy; h++)
	{
		for (int w = 0; w < dstSize.ImageSize.cx; w++)
		{
			dVal = (bySrc1[(h * dstSize.ImageSize.cx) + w] * bySrc2[(h * dstSize.ImageSize.cx) + w]) * dMult + dOffset;

			if (dVal > 255)
				dVal = 255;

			if (dVal < 0.)
				dVal = 0.;

			byDst[(h * dstSize.ImageSize.cx) + w] = (BYTE)dVal;
		}
	}


	dst->Put2D(CSize(0, 0), dstSize.ImageSize, byDst);

	delete[] bySrc1;
	delete[] bySrc2;
	delete[] byDst;
}

void Sub_Image(CMilBuffer* src1, CMilBuffer* src2, CMilBuffer* dst, double dMult, double dOffset)
{
	tImageProperties src1Size, src2Size, dstSize;
	src1->GetImageProperties(&src1Size);
	src2->GetImageProperties(&src2Size);
	dst->GetImageProperties(&dstSize);


	BYTE* bySrc1, * bySrc2, * byDst;
	bySrc1 = new BYTE[src1Size.ImageSize.cx * src1Size.ImageSize.cy];
	src1->Get2D(bySrc1, CRect(0, 0, src1Size.ImageSize.cx, src1Size.ImageSize.cy));
	bySrc2 = new BYTE[src1Size.ImageSize.cx * src1Size.ImageSize.cy];
	src2->Get2D(bySrc2, CRect(0, 0, src2Size.ImageSize.cx, src2Size.ImageSize.cy));

	byDst = new BYTE[dstSize.ImageSize.cx * dstSize.ImageSize.cy];
	dst->Get2D(byDst, CRect(0, 0, dstSize.ImageSize.cx, dstSize.ImageSize.cy));

	double dVal = 0.;
	for (int h = 0; h < dstSize.ImageSize.cy; h++)
	{
		for (int w = 0; w < dstSize.ImageSize.cx; w++)
		{
			dVal = (bySrc1[(h * dstSize.ImageSize.cx) + w] - bySrc2[(h * dstSize.ImageSize.cx) + w]) * dMult + dOffset;

			if (dVal > 255)
				dVal = 255;

			if (dVal < 0.)
				dVal = 0.;

			byDst[(h * dstSize.ImageSize.cx) + w] = (BYTE)dVal;
		}
	}


	dst->Put2D(CSize(0, 0), dstSize.ImageSize, byDst);

	delete[] bySrc1;
	delete[] bySrc2;
	delete[] byDst;
}

void MilHistogram(CMilBuffer * src, int nDepth, long long* dst)
{
	MIL_ID milHisto;
	MIL_INT64* milHistoVal;
	milHistoVal = new MIL_INT64[nDepth];
	MimAllocResult(src->m_milIDSystem, nDepth, M_HIST_LIST, &milHisto);
	MimHistogram(src->GetIDBuffer(), milHisto);
	MimGetResult(milHisto, M_VALUE, milHistoVal);

	for (int i = 0; i < nDepth; i++)
		dst[i] = (long long)milHistoVal[i];

	MimFree(milHisto);
	delete[] milHistoVal;
}
#endif
void GammaCorrection(BYTE* by_source, long width, long height, float fGamma, float fOffset)
{
	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			by_source[(h * width) + w] = (BYTE)limite((int)(pow((float)(by_source[(h * width) + w] / 255.f), fGamma) * 255. + fOffset));
		}
	}
}

void BinImageProfileX(BYTE* src, BYTE* dst, int nWidth, int nHeight, int nCenVal, int nOffset)
{
	int* nMinProfile;
	nMinProfile = new int[nHeight];
	double* dAvgProfile;
	dAvgProfile = new double[nHeight];
	double* dMaxProfile;
	dMaxProfile = new double[nHeight];

	for (int h = 0; h < nHeight; h++)
	{
		int nMin = 255, nMax = 0;
		int nSum = 0, nIdx = 0;

		for (int w = 0; w < nWidth; w++)
		{
			nMin = min((int)src[(h * nWidth) + w], nMin);
			nMax = max((int)src[(h * nWidth) + w], nMax);
		}
		nMinProfile[h] = nMin;
		dMaxProfile[h] = (nMax - nMin) / 2 + nMin;
	}

	for (int h = 0; h < nHeight; h++)
	{
		for (int w = 0; w < nWidth; w++)
		{
			if (src[(h * nWidth) + w] <= nCenVal)
			{
				if (src[(h * nWidth) + w] <= (nMinProfile[h] + nOffset))// dMaxProfile[h]))
					dst[(h * nWidth) + w] = 255;
				else
					dst[(h * nWidth) + w] = 0;
			}
		}
	}

	delete[] dMaxProfile;
	delete[] nMinProfile;
	delete[] dAvgProfile;
}

void BinImageProfileY(BYTE* src, BYTE* dst, int nWidth, int nHeight, int nCenVal, int nOffset)
{
	int* nMinProfile;
	nMinProfile = new int[nWidth];
	double* dAvgProfile;
	dAvgProfile = new double[nWidth];
	double* dMaxProfile;
	dMaxProfile = new double[nWidth];

	for (int w = 0; w < nWidth; w++)
	{
		int nMin = 255, nMax = 0;
		int nSum = 0, nIdx = 0;
		for (int h = 0; h < nHeight; h++)
		{
			nMin = min((int)src[(h * nWidth) + w], nMin);
			nMax = max((int)src[(h * nWidth) + w], nMax);
		}
		nMinProfile[w] = nMin;
		if(nMin >= nMax)
		dMaxProfile[w] = (nMax - nMin) / 2 + nMin;
	}
	
	for (int w = 0; w < nWidth; w++)
	{
		for (int h = 0; h < nHeight; h++)
		{
			if (src[(h * nWidth) + w] <= nCenVal)
			{
				if (src[(h * nWidth) + w] <= (nMinProfile[w] + nOffset))//dMaxProfile[w]))
					dst[(h * nWidth) + w] = 255;
				else
					dst[(h * nWidth) + w] = 0;
			}
		}
	}

	delete[] dMaxProfile;
	delete[] nMinProfile;
	delete[] dAvgProfile;
}

void BinImageLowPass(BYTE* src, BYTE* dst, int nWidth, int nHeight, int nHighVal)
{
	for (int h = 0; h < nHeight; h++)
	{
		for (int w = 0; w < nWidth; w++)
		{
			if (src[(h * nWidth) + w] > nHighVal)
			{
				dst[(h * nWidth) + w] = 0;
			}
			else
			{
				dst[(h * nWidth) + w] = src[(h * nWidth) + w];
			}
		}
	}
}

void SubImage(BYTE * src1, BYTE * src2, BYTE * dst, int nWidth, int nHeight, int nMult, int nOffset)
{
	for (int h = 0; h < nHeight; h++)
	{
		for (int w = 0; w < nWidth; w++)
		{
			int nVal = ((int)src1[(h * nWidth) + w] - (int)src2[(h * nWidth) + w]) * nMult + nOffset;

			/*if (nVal < 0)
				nVal = 0;

			if (nVal > 255)
				nVal = 255;*/

			dst[(h * nWidth) + w] = (BYTE)nVal;

		}
	}
}

void BinImageProfile(BYTE* src, BYTE* dst, int nWidth, int nHeight, int nCenVal, int nOffset, int nType)
{
	BYTE* byDst1, * byDst2;
	byDst1 = new BYTE[nWidth * nHeight];
	byDst2 = new BYTE[nWidth * nHeight];
	memset(byDst1, NULL, sizeof(BYTE) * (nWidth * nHeight));
	memset(byDst2, NULL, sizeof(BYTE) * (nWidth * nHeight));

	BinImageProfileX(src, byDst1, nWidth, nHeight, nCenVal, nOffset);
	BinImageProfileY(src, byDst2, nWidth, nHeight, nCenVal, nOffset);

	for (int h = 0; h < nHeight; h++)
	{
		for (int w = 0; w < nWidth; w++)
		{
			if (nType == 0)
			{
				if (byDst1[(h * nWidth) + w] > 0 || byDst2[(h * nWidth) + w] > 0)
					dst[(h * nWidth) + w] = 255;
			}
			else
			{
				if (byDst1[(h * nWidth) + w] > 0 && byDst2[(h * nWidth) + w] > 0)
					dst[(h * nWidth) + w] = 255;
			}
		}
	}

	delete[] byDst1;
	delete[] byDst2;
}

void IlluminateFunc(BYTE * src, BYTE * mask, BYTE * dst, int nWidth, int nHeight, double dFactor)
{
	if (dFactor <= 0.)
	{
		memcpy(dst, src, sizeof(BYTE) * (nWidth * nHeight));
		return;
	}

	for (int h = 0; h < nHeight; h++)
	{
		for (int w = 0; w < nWidth; w++)
		{
			double dVal = (double)(127 - mask[(h * nWidth) + w]) * dFactor + (double)src[(h * nWidth) + w];
			if (dVal > 255)
				dst[(h * nWidth) + w] = 255;
			else if (dVal < 0)
				dst[(h * nWidth) + w] = 0;
			else
				dst[(h * nWidth) + w] = (BYTE)dVal;
		}
	}
}

void MeanImageFunc(BYTE* src, BYTE* dst, int nWidth, int nHeight, int nMaskWidthHeight)
{
	for (int h = 0; h < nHeight; h++)
	{
		for (int w = 0; w < nWidth; w++)
		{
			double dSum;
			int nHalfWidthHeight = nMaskWidthHeight / 2;
			double dVal = 0;
			int nStartY, nStartX, nEndY, nEndX;
			int nMinuseValX = 0, nMinuseValY = 0;
			nStartY = h - nHalfWidthHeight;
			nMinuseValY = h - nHalfWidthHeight;
			if (nStartY < 0)
				nStartY = 0;

			nStartX = w - nHalfWidthHeight;
			nMinuseValX = w - nHalfWidthHeight;
			if (nStartX < 0)
				nStartX = 0;

			
			nEndY = nStartY + nHalfWidthHeight;
			if (nEndY > nHeight)
				nEndY = nHeight;
			if (nEndY == nHalfWidthHeight)
				nEndY++;
			if (nMinuseValY < 0)
				nEndY += h;

			nEndX = nStartX + nHalfWidthHeight;
			if (nEndX > nWidth)
				nEndX = nWidth;
			if (nEndX == nHalfWidthHeight)
				nEndX++;
			if (nMinuseValX < 0)
				nEndX += w;

			dSum = 0;
			int nIdx = 0;
			for (int h2 = nStartY; h2 < nEndY; h2++)
			{
				for (int w2 = nStartX; w2 < nEndX; w2++)
				{
					dSum += src[(h2 * nWidth) + w2];
					nIdx++;
				}
			}

			dVal = dSum / nIdx;

			
			if (dVal > 255)
				dst[(h * nWidth) + w] = 255;
			else if (dVal < 0)
				dst[(h * nWidth) + w] = 0;
			else
				dst[(h * nWidth) + w] = (BYTE)dVal;
		}
	}
}

void EmpahsizeFunc(BYTE* src, BYTE* mask, BYTE* dst, int nWidth, int nHeight, double dFactor)
{
	if (dFactor <= 0.)
	{
		memcpy(dst, src, sizeof(BYTE) * (nWidth * nHeight));
		return;
	}

	for (int h = 0; h < nHeight; h++)
	{
		for (int w = 0; w < nWidth; w++)
		{
			double dVal = (double)((double)src[(h * nWidth) + w] - mask[(h * nWidth) + w]) * dFactor + (double)src[(h * nWidth) + w];
			if (dVal > 255)
				dst[(h * nWidth) + w] = 255;
			else if (dVal < 0)
				dst[(h * nWidth) + w] = 0;
			else
				dst[(h * nWidth) + w] = (BYTE)dVal;
		}
	}
}


void DiffusionFilter(BYTE* by_source, long width, long height, float lambda, float k, int iter)
{
	double* f_temp1, * f_temp2;
	double k2 = k * k;
	double gradn, grads, grade, gradw;
	double gcn, gcs, gce, gcw;
	double tmp;

	f_temp1 = new double[width * height];
	f_temp2 = new double[width * height];

	memset(f_temp1, 0, sizeof(double) * width * height);
	memset(f_temp2, 0, sizeof(double) * width * height);

	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			f_temp1[(h * width) + w] = (double)by_source[(h * width) + w];
			f_temp2[(h * width) + w] = (double)by_source[(h * width) + w];
		}
	}

	for (int i = 0; i < iter; i++)
	{
		for (int h = 1; h < height - 1; h++)
		{
			for (int w = 1; w < width - 1; w++)
			{
				tmp = f_temp1[(h * width) + w];

				gradn = f_temp1[((h - 1) * width) + w] - tmp;
				grads = f_temp1[((h + 1) * width) + w] - tmp;
				grade = f_temp1[(h * width) + (w - 1)] - tmp;
				gradw = f_temp1[(h * width) + (w + 1)] - tmp;

				gcn = gradn / (1.0f + gradn * gradn / k2);
				gcs = grads / (1.0f + grads * grads / k2);
				gce = grade / (1.0f + grade * grade / k2);
				gcw = gradw / (1.0f + gradw * gradw / k2);

				f_temp2[(h * width) + w] = f_temp1[(h * width) + w] + lambda * (gcn + gcs + gce + gcw);
			}
		}
	}

	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			f_temp1[(h * width) + w] = f_temp2[(h * width) + w];
		}
	}

	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			by_source[(h * width) + w] = (BYTE)limite((int)(f_temp1[(h * width) + w] + 0.5f));
		}
	}

	delete f_temp1;
	delete f_temp2;
}
void DrawGraph(CClientDC *pDc, CRect view_rect, int x, int y, int nImgDep, long long data[])
{
	CRect cRect = view_rect;
	unsigned int width = cRect.Width();
	unsigned int height = cRect.Height();
	unsigned int* data_print;//[256] = {0,};
	data_print = new unsigned int[nImgDep];

	// Draw Background
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, RGB(150, 150, 150));
	CPen* oldPen = pDc->SelectObject(&pen);
	pDc->Rectangle(x, y, x + width, y + height);
	pDc->SelectObject(oldPen);
	// Normalize
	unsigned int max_n = 0;
	for (int i = 0; i < nImgDep; ++i)
		if (data[i] > max_n) max_n = (UINT)data[i];

	double ratio = max_n ? (1.0 * (height - 1) / (double)max_n) : 0.0;
	// Adjust Scale
	double adjust = 1.0;
	double min_threshold = 0.24; // 24%
	unsigned int min_threshold_cnt = (unsigned int)(height * width * min_threshold);
	unsigned int cnt = 0;

	if (nImgDep == 256)
	{
		while (1)
		{
			cnt = 0;
			for (int i = 0; i < nImgDep; ++i)
				cnt += (unsigned int)((data[i] * adjust) * ratio);

			if (cnt == 0 || cnt > min_threshold_cnt)
				break;

			adjust += 0.1;
		}

		// Set data
		for (int i = 0; i < nImgDep; ++i)
			data_print[i] = (unsigned int)((data[i] * adjust) * ratio);
	}
	else
	{
		unsigned int* data_print2;
		data_print2 = new unsigned int[256];
		int z = 0;
		for (int i = 0; i < nImgDep; i += 256)
		{
			unsigned int sum = 0;
			for (int j = 0; j < 256; j++)
			{
				sum += data[i + j];
			}

			data_print2[z++] = (unsigned int)(sum / 256.);
		}

		max_n = 0;
		for (int i = 0; i < 256; ++i)
			if (data_print2[i] > max_n) max_n = (UINT)data_print2[i];

		ratio = max_n ? (1.0 * (height - 1) / (double)max_n) : 0.0;

		while (1)
		{
			cnt = 0;
			for (int i = 0; i < 256; ++i)
				cnt += (unsigned int)((data_print2[i] * adjust) * ratio);

			if (cnt == 0 || cnt > min_threshold_cnt)
				break;

			adjust += 0.1;
		}

		// Set data
		for (int i = 0; i < 256; ++i)
			data_print[i] = (unsigned int)((data_print2[i] * adjust) * ratio);

		delete[] data_print2;
	}

	// Draw Graph
	CPen penData;
	penData.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	oldPen = pDc->SelectObject(&penData);
	if (nImgDep == 256)
	{
		for (int i = 0; i < nImgDep; ++i)
		{
			if (data_print[i] == 0)
				continue;

			pDc->MoveTo(5 + x + i, y + height - 2);
			pDc->LineTo(5 + x + i, y + height - (data_print[i] > height ? height : data_print[i]) - 1);
		}
	}
	else
	{

		/*unsigned int* data_print2;
		data_print2 = new unsigned int[256];
		int z = 0;
		for (int i = 0; i < nImgDep; i += 256)
		{
			unsigned int sum = 0;
			for (int j = 0; j < 256; j++)
			{
				sum += data_print[i + j];
			}

			data_print2[z++] = (unsigned int)(sum / 256.);
		}*/

		for (int i = 0; i < 256; ++i)
		{
			if (data_print[i] == 0)
				continue;

			pDc->MoveTo(5 + x + i, y + height - 2);
			pDc->LineTo(5 + x + i, y + height - (data_print[i] > height ? height : data_print[i]) - 1);
		}

		/*delete[] data_print2;*/
	}
	pDc->SelectObject(oldPen);

	delete[] data_print;
}

void ScaleImage(void* src, void* dst, CSize size, int nMin, int nMax, int nBit)
{
	BYTE* byData = NULL, * byRslt = NULL;;
	WORD* wData = NULL, * wRslt = NULL;;
	CRect img_rect;
	img_rect.left = 0;
	img_rect.top = 0;
	img_rect.right = size.cx;
	img_rect.bottom = size.cy;
	double dMult = 0., dAdd = 0., dDep = 0.;
	if (nBit == 8)
	{
		byData = new BYTE[size.cx * size.cy];
		byRslt = new BYTE[size.cx * size.cy];
		memcpy(byData, src, sizeof(BYTE) * (size.cx * size.cy));
		dDep = 255.;
	}
	else if (nBit == 16)
	{
		wData = new WORD[size.cx * size.cy];
		wRslt = new WORD[size.cx * size.cy];
		memcpy(wData, src, sizeof(WORD) * (size.cx * size.cy));
		dDep = 65535.;
	}
	else
		return;
	
	dMult = dDep / ((double)nMax - (double)nMin);
	dAdd = dMult * (double)nMin * -1.;

	for (int h = 0; h < size.cy; h++)
	{
		for (int w = 0; w < size.cx; w++)
		{
			double dVal;
			if (nBit == 8)
				dVal = (double)byData[(h * size.cx) + w] * dMult + dAdd;
			else
				dVal = (double)wData[(h * size.cx) + w] * dMult + dAdd;


			if (dVal < 0.)
				dVal = 0.;

			if (nBit == 8)
			{
				if (dVal > 255.)
					dVal = 255.;
			}
			else
			{
				if (dVal > 65535.)
					dVal = 65535.;
			}

			//BYTE byVal = (BYTE)dVal;
			//dst[(h * size.cx) + w] = byVal;
			if (nBit == 8)
				byRslt[(h * size.cx) + w] = (BYTE)dVal;
			else
				wRslt[(h * size.cx) + w] = (WORD)dVal;
		}
	}

	if (nBit == 8)
	{
		memcpy(dst, byRslt, sizeof(BYTE) * (size.cx * size.cy));
		delete[] byData;
		delete[] byRslt;
	}
	else if (nBit == 16)
	{
		memcpy(dst, wRslt, sizeof(WORD) * (size.cx * size.cy));
		delete[] wData;
		delete[] wRslt;
	}
}

int WideCharToMultiByteChar(CString strWideChar, char* cRslt)
{
	return WideCharToMultiByte(CP_ACP, 0, strWideChar, -1, cRslt, strWideChar.GetLength(), NULL, NULL);
}

int HexToDec(BYTE * byHex, int nDataLenght)
{
	int nRslt = 0;

	for (int i = 0; i < nDataLenght; i++)
	{
		if (byHex[i] >= 0x61 && byHex[i] <= 0x66)
			nRslt = nRslt * 16 + (byHex[i] - 0x61) + 10;
		else if (byHex[i] >= 0x30 && byHex[i] <= 0x39)
			nRslt = nRslt * 16 + (byHex[i] - 0x30);
	}

	return nRslt;
}

BOOL ByteToBit(BYTE byData, BOOL* bRslt)
{
	BOOL rslt = TRUE;

	for (int i = 0; i < 8; i++)
	{
		BOOL bVal = TRUE;
		BYTE mask = 0x01;
		bVal = byData & (mask << i) ? TRUE : FALSE;

		bRslt[i] = bVal;
	}
	

	return rslt;
}

BYTE BitToByte(BOOL* byData)
{
	BYTE byRslt = NULL;

	for (int i = 0; i < 8; i++)
		byRslt |= (byData[i] << i);

	return byRslt;
}

BYTE TextToByte(CString strText, int nStartIndex, int nTransCnt)
{
	BYTE byReturn = NULL;

	if (strText.GetLength() < nStartIndex + nTransCnt)
		return NULL;

	for (int i = nStartIndex; i < (nStartIndex + nTransCnt); i++)
	{
		BYTE nTen = 0x01;
		if (i % 2 == 0)
			nTen = 0x10;


		if (!strText.Mid(i, 1).Compare(_T("0")))
		{
			byReturn += 0;
		}
		else if (!strText.Mid(i, 1).Compare(_T("1")) || !strText.Mid(i, 1).Compare(_T("2")) || !strText.Mid(i, 1).Compare(_T("3")) || !strText.Mid(i, 1).Compare(_T("4")) || 
				!strText.Mid(i, 1).Compare(_T("5")) || !strText.Mid(i, 1).Compare(_T("6")) || !strText.Mid(i, 1).Compare(_T("7")) || !strText.Mid(i, 1).Compare(_T("8")) || 
				!strText.Mid(i, 1).Compare(_T("9")))
		{
			byReturn += (nTen * _wtoi(strText.Mid(i, 1)));
		}
		else if (!strText.Mid(i, 1).Compare(_T("A")))
		{
			byReturn += (nTen * 0x0A);
		}
		else if (!strText.Mid(i, 1).Compare(_T("B")))
		{
			byReturn += (nTen * 0x0B);
		}
		else if (!strText.Mid(i, 1).Compare(_T("C")))
		{
			byReturn += (nTen * 0x0C);
		}
		else if (!strText.Mid(i, 1).Compare(_T("D")))
		{
			byReturn += (nTen * 0x0D);
		}
		else if (!strText.Mid(i, 1).Compare(_T("E")))
		{
			byReturn += (nTen * 0x0E);
		}
		else if (!strText.Mid(i, 1).Compare(_T("F")))
		{
			byReturn += (nTen * 0x0F);
		}
	}

	return byReturn;
}

#ifndef NOT_USE_FUNC
BOOL CompareUSBDevice(HRAWINPUT hRawInput, HANDLE *hKeyHandle, int nInputDeviceCnt, CString strSerID)
{
	BOOL rslt = FALSE;
	int nSerIdx = -1;
	RAWINPUT Input;
	memset(&Input, 0, sizeof(Input));
	UINT nSize = sizeof(RAWINPUT);
	GetRawInputData(hRawInput, RID_INPUT, &Input, &nSize, sizeof(RAWINPUTHEADER));

	for(int i = 0; i < nInputDeviceCnt; i++)
	{
		if(hKeyHandle[i] == Input.header.hDevice)
		{
			nSerIdx = i;
			break;
		}
	}

	if (nSerIdx > -1) 	
	{
		CString strTemp, strID;
		int nResult = 0;
		WCHAR *wcDeviceName;
		UINT nBufferSize = 0;
		RID_DEVICE_INFO rdiDeviceInfo; 
		rdiDeviceInfo.cbSize = sizeof(RID_DEVICE_INFO); 
		nBufferSize = rdiDeviceInfo.cbSize;

		nResult = GetRawInputDeviceInfo(hKeyHandle[nSerIdx], // Device 
										RIDI_DEVICENAME,    // Get Device Name 
										NULL,       // NO Buff, Want Count! 
										&nBufferSize);     // Char Count Here!

		wcDeviceName = new WCHAR[nBufferSize + 1];

		nResult = GetRawInputDeviceInfo(hKeyHandle[nSerIdx], // Device 
										RIDI_DEVICENAME,    // Get Device Name 
										wcDeviceName,       // NO Buff, Want Count!  
										&nBufferSize);     // Char Count Here!
		strTemp.Format(_T("%s"), wcDeviceName);
		int nFindIdx = strTemp.Find('{', 0);
		strID.Format(_T("%s"), strTemp.Mid(0, nFindIdx - 1));
		strID.MakeUpper();
		if(!strID.Compare(strSerID))
		{
			rslt = TRUE;
		}
		delete wcDeviceName;
	}

	return rslt;
}

BOOL SearchUSBDevice(HRAWINPUT hRawInput, HANDLE *hKeyHandle, int *nInputDeviceCnt, int nSearchType)
{
	BOOL rslt = FALSE;
	int nType = 0;

	switch(nSearchType)
	{
	case SEARCH_USB_KEYBOARD:
		nType = RIM_TYPEKEYBOARD;
		break;
	case SEARCH_USB_MOUSE:
		nType = RIM_TYPEMOUSE;
		break;
	}

	RAWINPUT Input;
	memset(&Input, 0, sizeof(Input));
	UINT nSize = sizeof(RAWINPUT);
	GetRawInputData(hRawInput, RID_INPUT, &Input, &nSize, sizeof(RAWINPUTHEADER));

	UINT nDevices;
	UINT ret = GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST));
	if (ret == 0) 
	{
		PRAWINPUTDEVICELIST pRawInputDeviceList = new RAWINPUTDEVICELIST[nDevices];
		ret = GetRawInputDeviceList(pRawInputDeviceList, &nDevices,	sizeof(RAWINPUTDEVICELIST));

		if (ret != (UINT)-1) 
		{
			int nKeyCnt = 0;
			int nKeyIdx[30] = {0,};
			for (UINT i = 0; i < nDevices; ++i)
			{
				if (pRawInputDeviceList[i].dwType == nType) 
				{
					nKeyIdx[nKeyCnt] = i;
					nKeyCnt++;

				}
			}

			*nInputDeviceCnt = nKeyCnt;
			for(int i = 0; i < nKeyCnt; i++)
			{
				int nResult = 0;
				WCHAR *wcDeviceName;
				UINT nBufferSize = 0;
				RID_DEVICE_INFO rdiDeviceInfo; 
				rdiDeviceInfo.cbSize = sizeof(RID_DEVICE_INFO); 
				nBufferSize = rdiDeviceInfo.cbSize;

				nResult = GetRawInputDeviceInfo(pRawInputDeviceList[nKeyIdx[i]].hDevice, // Device 
												RIDI_DEVICENAME,    // Get Device Name 
												NULL,       // NO Buff, Want Count! 
												&nBufferSize);     // Char Count Here!

				wcDeviceName = new WCHAR[nBufferSize + 1];

				nResult = GetRawInputDeviceInfo(pRawInputDeviceList[nKeyIdx[i]].hDevice, // Device 
												RIDI_DEVICENAME,    // Get Device Name 
												wcDeviceName,       // NO Buff, Want Count!  
												&nBufferSize);     // Char Count Here!


				nBufferSize = rdiDeviceInfo.cbSize;

				nResult = GetRawInputDeviceInfo(pRawInputDeviceList[nKeyIdx[i]].hDevice, RIDI_DEVICEINFO, &rdiDeviceInfo, &nBufferSize); 

				hKeyHandle[i] = pRawInputDeviceList[nKeyIdx[i]].hDevice;

				delete wcDeviceName;
			}

			rslt = TRUE;
		}

		delete[] pRawInputDeviceList;
	}

	return rslt;
}

CString LoadBarcodeID(CString strPath, int nChannel)
{
	CString strRslt;
	BOOL rslt = TRUE;
	CString key, dat, app, temp_default;
	CString file_path;

	file_path.Format(_T("%s"), strPath);

	//file 존재 확인 
	HANDLE fd = CreateFile( file_path,
		GENERIC_READ|GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if(((LONG)fd)<0)
	{
		return FALSE;
	}
	else
	{
		CloseHandle(fd);

		app.Empty();
		app.Format(_T("BARCODE_INFO"));
		dat.Empty();
		key.Empty();
		key.Format(_T("BARCODE_CHANNEL_%d"), nChannel);
		GetPrivateProfileString(app, key, _T("TEST"), (LPWSTR)(LPCWSTR)dat, 256, file_path);
		strRslt.Format(_T("\\\\?\\%s"), dat);

	}

	return strRslt;
}
#endif
int limite(int nVal)
{
	int nRslt = 0;

	if (nVal > MAX_VAL)
	{
		nRslt = MAX_VAL;
	}
	else if (nVal < 0)
	{
		nRslt = 0;
	}
	else
	{
		nRslt = nVal;
	}

	return nRslt;
}

BOOL FileDelete(CString FilePath, CString FileName)
{
	CFileFind ff;
	CString strTarget = FilePath;
	CStringArray strFileArray;
	BOOL rslt = FALSE;

	if (strTarget.Right(1) != '\\')
	{
		strTarget += '\\';
	}

	strTarget += "*.*";
	if (ff.FindFile(strTarget) == TRUE)
	{
		CString Filename;
		BOOL bFlag = true;
		while (bFlag == TRUE)
		{
			bFlag = ff.FindNextFile();
			Filename = ff.GetFilePath();
			if (ff.IsDots())
			{
				continue;
			}
			if (ff.IsDirectory())
			{
				continue;
			}
			if (ff.GetFileName() == FileName)
			{
				DeleteFile(Filename);
				break;
			}
		}
	}
	ff.Close();

	return rslt;
}


BOOL FileSearch(CString FilePath, CString FileName)
{
	CFileFind ff;
	CString strTarget = FilePath;
	CStringArray strFileArray;
	BOOL rslt = FALSE;

	if (strTarget.Right(1) != '\\')
	{
		strTarget += '\\';
	}

	strTarget += "*.*";
	if (ff.FindFile(strTarget) == TRUE)
	{
		CString Filename;
		BOOL bFlag = true;
		while (bFlag == TRUE)
		{
			bFlag = ff.FindNextFile();
			Filename = ff.GetFilePath();
			if (ff.IsDots())
			{
				continue;
			}
			if (ff.IsDirectory())
			{
				continue;
			}
			if (ff.GetFileName() == FileName)
			{
				rslt = TRUE;
				break;
			}
		}
	}
	ff.Close();

	return rslt;
}

void MakeDirectories(CString path)
{
	wchar_t tmp_path[MAX_PATH];
	wchar_t sub[MAX_PATH];
	long size;
	size = sizeof(path);

	long index = 0;

	wsprintf(tmp_path, _T("%s"), (LPCTSTR)path);
	memset(sub, NULL, sizeof(sub));

	while (1)
	{
		if (tmp_path[index] == '\\')
		{
			CreateDirectory(sub, NULL);
		}

		sub[index] = tmp_path[index];

		if (tmp_path[index] == '\0')
		{
			CreateDirectory(sub, NULL);
			break;
		}

		//base_index++;
		index++;

		if ((index > MAX_PATH))
		{
			AfxMessageBox(_T("Error : index overflow(_MakeDirectories)!"));
			break;
		}

	}
}

double DegreeToRation(double deg)
{
	return deg * (M_PI / 180);
}

double RatianToDegree(double rad)
{
	return rad * (180 / M_PI);
}

#ifdef OPEN_CV_LIB
double GetDistance(CvPoint2D32f p1, CvPoint2D32f p2)
{
	double xdf = (double)p2.x - (double)p1.x;
	double ydf = (double)p2.y - (double)p1.y;
	return sqrt(pow(xdf, 2.) + pow(ydf, 2.));
}

double GetAngle(CvPoint2D32f p1, CvPoint2D32f p2)
{
	double xdf = (double)p2.x - (double)p1.x;
	double ydf = (double)p2.y - (double)p1.y;

	double ang = RatianToDegree(atan2(ydf, xdf));
	return ang + 90;
}
#endif

void DoEvents()
{
	MSG message;
	if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&message);
		::DispatchMessage(&message);
	}
}

void Delay(long ms, BOOL bEvent)
{
	double s, e;
	double Ms = (double)ms;
	s = (double)GetTickCount64();
	do
	{
		if (bEvent)
			DoEvents();

		Sleep(1);
		e = (double)GetTickCount64();
	} while ((e - s) < Ms);
}

void DeleteFolderAndFile(CString strPath)
{
	CString strFileFound = strPath;
	//strFileFound += "\\*.*";		 
	
    WIN32_FIND_DATA info;
	
	HANDLE hp = FindFirstFile(strFileFound, &info);   //디렉토리에 파일이 있는지 첫번째 파일만.
    do
    {
        if (!((_tcscmp(info.cFileName, _T("."))==0) || (_tcscmp(info.cFileName, _T(".."))==0))) 
		{
            if((info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==FILE_ATTRIBUTE_DIRECTORY)  //Sub디렉토리가 존재하는경우 
            {
                //string strSubFolder = strPath;
				//strSubFolder += "\\";
				//strSubFolder += info.cFileName;
				
				DeleteFolderAndFile(strPath);
					//DeleteFolderAndFile(strSubFolder.c_str()); /// {return (_Ptr == 0 ? _Nullstr() : _Ptr); } 
			}
            else
			{
				strFileFound = strPath;
				//strFileFound += "\\";
				//strFileFound += info.cFileName;
				BOOL retVal = DeleteFile(strFileFound);
            }
			
        }
		Delay(2,TRUE);		
    }while(FindNextFile(hp, &info));
    
	FindClose(hp);
	
	RemoveDirectory((LPCTSTR)strPath);	
}

BOOL CreateDir(LPCTSTR lpszDirectory)
{
	CString sCreateDir;		
	sCreateDir.Format(_T("%s"), lpszDirectory);
	BOOL bFile;

	bFile = TRUE;
	if ( sCreateDir.Mid(sCreateDir.GetLength() - 3,1) != "."
		&& sCreateDir.Mid(sCreateDir.GetLength() - 4,1) != ".")
	{
		bFile = FALSE;
	}
	if (sCreateDir.Right(1) != "\\"	&& !bFile) 
		sCreateDir += "\\";

	CString sTempDir[255];
	CString strt;
	int nSresh = 0;
	int i;
	for (i=0; i < sCreateDir.GetLength() ; i++)
	{
		sTempDir[nSresh]=sCreateDir.Left(i+1);

		if ( sCreateDir.Mid(i,1) == "\\" )
		{
			if (sCreateDir.Mid(i+1,1) != "\\")
			{
				nSresh++;
			}
			else
			{
				return FALSE;  // 잘못된 수식어
			}

		}
	}
// 	if (bFile && nSresh > 0)
// 		nSresh--;
	if(nSresh == 1)
	{
		DWORD dwRet = GetFileAttributes(sTempDir[0]);
		if(dwRet == 0xffffffff)		// 폴더가 존재 확인
			return FALSE;
	}
	else
	{
		for (i = 0; i < nSresh - 1; i++)
		{
			DWORD dwRet = GetFileAttributes(sTempDir[i + 1]);
			if (dwRet != FILE_ATTRIBUTE_DIRECTORY)		// 폴더가 존재 확인
				if (!::CreateDirectory(sTempDir[i + 1], NULL))
					continue;
					//return FALSE;
		}
		//<<
	}
	return TRUE;
}

BOOL FolderSearch(CString strFolder)
{
	BOOL rslt = TRUE;
	CFileFind ff;
	CString strTarget = strFolder;
	CStringArray strFileArray;

	if(strTarget.Right(1) != '\\')
	{
		strTarget += '\\';
	}
	
	strTarget += "*.*";
	if(ff.FindFile(strTarget)==TRUE)
	{
		CString Filename;
		BOOL bFlag = true;
		while(bFlag == TRUE)
		{
			bFlag = ff.FindNextFile();
			Filename = ff.GetFilePath();
			if(ff.IsDots())
			{
				continue;
			}
			if(ff.IsDirectory())
			{
				rslt = FALSE;
				break;
			}
		}
	}
	else
	{
		rslt = FALSE;
	}
	ff.Close();

	return rslt;

}

BOOL GetFolderInFolderName(CString strFolder, vector<CString>* strRsltFolder)
{
	BOOL rslt = TRUE;
	CFileFind ff;
	CString strTarget = strFolder;
	CStringArray strFileArray;

	if (strTarget.Right(1) != '\\')
	{
		strTarget += '\\';
	}

	strTarget += "*.*";
	if (ff.FindFile(strTarget) == TRUE)
	{
		CString Filename;
		BOOL bFlag = true;
		while (bFlag == TRUE)
		{
			bFlag = ff.FindNextFile();
			Filename = ff.GetFilePath();
			if (ff.IsDots())
			{
				continue;
			}
			if (!ff.IsDirectory())
			{
				strRsltFolder->push_back(ff.GetFileName());
				rslt = FALSE;
				//break;
			}
		}
	}
	else
	{
		rslt = FALSE;
	}
	ff.Close();

	return rslt;
}

BOOL GetFolderInFileName(CString strFolder, queue<CString>*strRsltFile)
{
	BOOL rslt = TRUE;
	CFileFind ff;
	CString strTarget = strFolder;
	CStringArray strFileArray;

	if (strTarget.Right(1) != '\\')
	{
		strTarget += '\\';
	}

	strTarget += "*.*";
	if (ff.FindFile(strTarget) == TRUE)
	{
		CString Filename;
		BOOL bFlag = true;
		while (bFlag == TRUE)
		{
			bFlag = ff.FindNextFile();
			Filename = ff.GetFilePath();
			if (ff.IsDots())
			{
				continue;
			}
			if (ff.IsDirectory())
			{
				continue;
			}

			{
				strRsltFile->push(ff.GetFileName());
				rslt = FALSE;
				//break;
			}
		}
	}
	else
	{
		rslt = FALSE;
	}
	ff.Close();

	return rslt;
}
BOOL LimiteRangeCheck(double dMin, double dMax, double dAfterVal, double dBeforVal, int nInputType, CString strInputLabel)
{
	BOOL rslt = FALSE;
	CString strText;
	int nMin = 0, nMax = 0, nAfterVal = 0, nBeforVal = 0;

	if(strInputLabel == _T("0.0000"))
	{
		strInputLabel.Format(_T(""));
	}

	switch(nInputType)
	{
	case LIMITE_RANGE_CHECK_TYPE_INT:
		nMin = (int)dMin;
		nMax = (int)dMax;
		nBeforVal = (int)dBeforVal;
		nAfterVal = (int)dAfterVal;
		if(nMin <= nAfterVal && nAfterVal <= nMax)
		{
			rslt = TRUE;
		}

		if(!rslt)
		{
			if(strInputLabel.IsEmpty())
			{
				strText.Format(_T("Min : %d, Max : %d, After Val : %d, Input : %d, Input Val Range Fail"), nMin, nMax, nBeforVal, nAfterVal);
			}
			else
			{
				strText.Format(_T("Input Label : %s, Min : %d, Max : %d, After Val : %d, Input : %d, Input Val Range Fail"), strInputLabel, nMin, nMax, nBeforVal, nAfterVal);
			}
		}
		break;
	case LIMITE_RANGE_CHECK_TYPE_DOUBLE:
		if(dMin <= dAfterVal && dAfterVal <= dMax)
		{
			rslt = TRUE;
		}
		if(!rslt)
		{
			if(strInputLabel.IsEmpty())
			{
				strText.Format(_T("Min : %0.3f, Max : %0.3f, After Val : %0.3f, Input : %0.3f, Input Val Range Fail"), dMin, dMax, dBeforVal, dAfterVal);
			}
			else
			{
				strText.Format(_T("Input Label : %s, Min : %0.3f, Max : %0.3f, After Val : %0.3f, Input : %0.3f, Input Val Range Fail"), strInputLabel, dMin, dMax, dBeforVal, dAfterVal);
			}
		}
		break;
	}

	if(!rslt)
		AfxMessageBox(strText);

	return rslt;
}

BOOL DriveUseSearch(CString strUseDriveName)
{
	TCHAR buffer[256] = {0, };
	CString strDriveName;
	int count = GetLogicalDriveStrings(256, buffer);
	BOOL isSearch = FALSE;
	for(int i = 0; i < count / 4; i++) 
	{
		strDriveName.Format(_T("%s:\\"), buffer + i * 4);

		if(!strUseDriveName.Left(1).Compare(strDriveName.Left(1)))
		{
			isSearch = TRUE;
			break;
		}
	}

	return isSearch;
}

double UseDriveFreeSize(CString strDriveName, CString strSizeType)
{
	CString lpDirectoryName;
	lpDirectoryName.Format(_T("%s:"), strDriveName);//ex D:
	ULARGE_INTEGER lpFreeBytesAvailableToCaller;
	ULARGE_INTEGER lpTotalNumberOfBytes;
	ULARGE_INTEGER lpTotalNumberOfFreeBytes;

	// 타입
	UINT nType = ::GetDriveType(lpDirectoryName);

	// 남은 공간 정보 가져오기
	BOOL bResult = GetDiskFreeSpaceEx(lpDirectoryName, &lpFreeBytesAvailableToCaller, &lpTotalNumberOfBytes, &lpTotalNumberOfFreeBytes);

	double rslt = -1., temp = 0.;
	if(!strSizeType.Compare(USE_DRIVE_FREE_SIZE_RETURN_VAL_GIGA_BYTE))//byte to GB
	{
		rslt = lpTotalNumberOfFreeBytes.QuadPart / 1024.0 / 1024.0 / 1024.0;
	}
	else if(!strSizeType.Compare(USE_DRIVE_FREE_SIZE_RETURN_VAL_MEGA_BYTE))//byte to MB
	{
		rslt = lpTotalNumberOfFreeBytes.QuadPart / 1024.0 / 1024.0;
	}
	else if(!strSizeType.Compare(USE_DRIVE_FREE_SIZE_RETURN_VAL_PERCENT))//byte To GB to Percent
	{
		temp = lpTotalNumberOfFreeBytes.QuadPart / 1024.0 / 1024.0 / 1024.0;
		rslt = lpTotalNumberOfBytes.QuadPart / 1024.0 / 1024.0 / 1024.0;
		rslt = (temp / rslt) * 100.;
	}

	return rslt;
}

 BOOL GetWriteFileTime(CString strFilePath, SYSTEMTIME *returnTime, int nGetTimeType)
{
	BOOL rslt = FALSE;

	HANDLE h_SrcFile = CreateFile(strFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(h_SrcFile != INVALID_HANDLE_VALUE) 
	{
		//** create_time : 생성 날짜, access_time : 마지막 접근날짜, write_time : 마지막 수정 날짜
		FILETIME create_time, access_time, write_time, rsltTime;
		//** System Time 취득
		GetFileTime(h_SrcFile, &create_time, &access_time, &write_time);
		rsltTime.dwLowDateTime = 0;
		rsltTime.dwHighDateTime = 0;
		switch(nGetTimeType)
		{
		case FILE_CREATE_TIME:
			rsltTime = create_time;
			break;
		case FILE_ACCESS_TIME:
			rsltTime = access_time;
			break;
		case FILE_WRITE_TIME:
			rsltTime = write_time;
			break;
		}

		//** SystemTime은 UTC형식(Universal Time Coordinated)이기 때문에 그냥 사용 불가능
		//** 지역시간으로 변경(일반적인 시간 계산법 2015/11/09 21:18:35)
		SYSTEMTIME write_system_time;
		FileTimeToSystemTime(&rsltTime, &write_system_time);

		//** write_local_time변수에 변환되어 저장됨
		//** 저장된 형태는 WORD형(short)
		//** wYear, wMonth, wDay, wHour, wMinute, wSecond를 활용하여 사용하면 됨
		SystemTimeToTzSpecificLocalTime(NULL, &write_system_time, returnTime);
		
		CloseHandle(h_SrcFile);
		rslt = TRUE;
	}

	return rslt;
}

BOOL FileWriteTimeToBackupFile(CString strSrcFilePath, CString strBackupFilePath, CString strFileName)
{
	BOOL rslt = FALSE;
	CString strSrcTime, strBackTime, strFileBackupPath, strTemp1, strTemp2,strSrcFileNamePath, strBackupFileNamePath;
	SYSTEMTIME srcFileTime;
	strSrcFileNamePath.Format(_T("%s%s"), strSrcFilePath, strFileName);
	strBackupFileNamePath.Format(_T("%s%s"), strBackupFilePath, strFileName);
	if(GetWriteFileTime(strSrcFileNamePath, &srcFileTime))
	{
		MakeDirectories(strBackupFilePath);
		rslt = FALSE;
		strSrcTime.Format(_T("%04d%02d%02d"), srcFileTime.wYear, srcFileTime.wMonth, srcFileTime.wDay);
		
		if(strSrcFileNamePath.Find(_T(".")) > 0)
		{
			strTemp1.Format(_T("%s"), strSrcFileNamePath.Mid(strSrcFileNamePath.Find(_T(".")), strSrcFileNamePath.GetLength() + 1));
			strFileBackupPath.Format(_T("%s_Backup_%04d%02d%02d_%02d%02d%02d%s"), strBackupFileNamePath.Mid(0, strBackupFileNamePath.Find(_T(".")) - 1), srcFileTime.wYear, srcFileTime.wMonth, srcFileTime.wDay, srcFileTime.wHour, srcFileTime.wMinute, srcFileTime.wSecond, strTemp1);
		}
		else
		{
			strFileBackupPath.Format(_T("%s_Backup_%04d%02d%02d_%02d%02d%02d"), strBackupFileNamePath, srcFileTime.wYear, srcFileTime.wMonth, srcFileTime.wDay, srcFileTime.wHour, srcFileTime.wMinute, srcFileTime.wSecond);
		}

		if(CopyFile(strSrcFileNamePath, strFileBackupPath, FALSE))
		{
			rslt = TRUE;
			//Sleep(500);
			::Delay(500);
		}
	}
	
	return rslt;
}

double GetAvg(double *dEachRsltArr, int nEachRsltCnt)
{
	double dSum = 0.;
	
	for(int i = 0 ; i < nEachRsltCnt; i++)
	{
		dSum += dEachRsltArr[i];
	}
	
	return dSum / (double)nEachRsltCnt;
}

double GetSTD(double *dEachRsltArr, int nEachRsltCnt, int nOption)
{
	double dSum = 0., dAvg = 0., dVariance = 0.;
	
	dAvg = GetAvg(dEachRsltArr, nEachRsltCnt);

	for(int i = 0; i < nEachRsltCnt; i++)
	{
		dSum += pow(dEachRsltArr[i] - dAvg, 2);
	}

	dVariance = dSum / ((double)nEachRsltCnt - nOption);

	return sqrt(dVariance);
}

// 피타고라스 정리 ( 대각의 길이 와 밑변을 가지고 높이 구하기)// x = 대각, y = 밑변
double GetPitaHight(double x, double y)
{
	// x = 대각, y = 밑변
	return sqrt((x*x) - (y*y));
}
// 피타고라스 정리 ( 높이 와 밑변을 가지고 대각의 길이 구하기)// x = 밑변, y = 높이
double GetPita(double x, double y)
{
	// x = 밑변, y = 높이
	return sqrt((x*x) + (y*y));
}

#ifdef OPEN_CV_LIB
//20190125 ngh
/////////////////////////////////////////////////
BOOL LoadIplImage(IplImage *lLoadImage, CString strLoadPath, BOOL isColor)
{
	BOOL rslt = TRUE;
	IplImage *lTempLoadImage;
	lTempLoadImage = NULL;

	int len = WideCharToMultiByte(CP_ACP, 0, strLoadPath, -1, NULL, 0, NULL, NULL);
	char * str = new char[len];
	WideCharToMultiByte(CP_ACP, 0, strLoadPath, -1, str, len, NULL, NULL);
	
	if(isColor)
	{
		lTempLoadImage = cvLoadImage(str, CV_LOAD_IMAGE_COLOR);
	}
	else
	{
		lTempLoadImage = cvLoadImage(str, CV_LOAD_IMAGE_GRAYSCALE);
	}

	delete[] str;

	if(lTempLoadImage != NULL)
	{
		if(lTempLoadImage->alphaChannel == lLoadImage->alphaChannel)
		{
			cvCopy(lTempLoadImage, lLoadImage);
		}
		else
		{
			rslt = FALSE;
		}	

		if(lLoadImage)
			cvReleaseImage(&lTempLoadImage);
	}

	return rslt;
}
//////////////////////////////////////////

BOOL SaveIplImage(IplImage *SaveImage, CString strSavePath)
{
	BOOL rslt;

	int len = WideCharToMultiByte(CP_ACP, 0, strSavePath, -1, NULL, 0, NULL, NULL);
	char * str = new char[len];
	WideCharToMultiByte(CP_ACP, 0, strSavePath, -1, str, len, NULL, NULL);
	rslt = cv::imwrite(str, cv::cvarrToMat(SaveImage)); 

	delete str;

	return rslt;
}

BOOL Morphology(IplImage *src, IplImage *dst, CvPoint KernelSize, int KernelShape, int nOpenClose)
{
	BOOL rslt = TRUE;
	IplImage *lDstImage;
	lDstImage = NULL;

	if(src == NULL)
	{
		rslt = FALSE;
	}

	if(KernelSize.x < 1 || KernelSize.y < 1)
	{
		rslt = FALSE;
	}

	if(KernelShape < CV_SHAPE_RECT || KernelShape > CV_SHAPE_ELLIPSE)
	{
		rslt = FALSE;
	}

	if(nOpenClose < CV_MOP_OPEN || nOpenClose > CV_MOP_CLOSE)
	{
		rslt = FALSE;
	}

	if(rslt)
	{
		lDstImage = cvCloneImage(src);//cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_8U, 1);
		//cvCopy(src, lDstImage);
		IplConvKernel *element = cvCreateStructuringElementEx(KernelSize.x, KernelSize.y, (int)((double)KernelSize.x * 0.5), (int)((double)KernelSize.y * 0.5), KernelShape, NULL);
		cvMorphologyEx(src, lDstImage, NULL, element, nOpenClose, 1); 
		cvCopy(lDstImage, dst);
		cvReleaseImage(&lDstImage);
		if(element)		cvReleaseStructuringElement(&element);
	}

	return rslt;
}

BOOL SmoothImage(IplImage *src, IplImage *dst, int SmoothType, CvPoint KernelSize, CvPoint2D32f KernelSigma)
{
	BOOL rslt = TRUE;

	if(src == NULL)
	{
		rslt = FALSE;
	}

	if(SmoothType < CV_BLUR_NO_SCALE || SmoothType > CV_BILATERAL)
	{
		rslt = FALSE;
	}

	if(KernelSize.x < 1 || KernelSize.y < 1)
	{
		rslt = FALSE;
	}

	if(rslt)
	{
		cvSmooth(src, dst, SmoothType, KernelSize.x, KernelSize.y, KernelSigma.x, KernelSigma.y);
	}

	return rslt;
}

BOOL TraceInsp(CStaticMatDisplay *Display, IplImage *Img, CvRect *rcRoi, BOOL bTrace)
{
	if(!bTrace)
		return TRUE;

	if(Img == NULL)
	{
		return FALSE;
	}

	if(Img->roi)	cvResetImageROI(Img);

	IplImage *lDstImage;
	lDstImage = NULL;
	lDstImage = cvCreateImage(cvSize(Img->width, Img->height), IPL_DEPTH_8U, 3);
	if(Img->nChannels == 1)
	{
		cvCvtColor(Img, lDstImage, CV_GRAY2BGR);
	}
	else
	{
		cvCopy(Img, lDstImage);
	}


	Display->SetImage(lDstImage);
	cvReleaseImage(&lDstImage);

	if(rcRoi->x == 0 && rcRoi->y == 0 && rcRoi->width == 0 && rcRoi->height == 0)
	{
		return TRUE;
	}

	if(rcRoi) cvSetImageROI(Img, *rcRoi);
	

	return TRUE;
}

BOOL FlipImage(IplImage *SrcImage, IplImage *DstImage, int nDirection)
{
	BOOL rslt = TRUE;

	if(SrcImage->alphaChannel > 1)
	{
		return FALSE;
	}

	IplImage *lDstImage;
	lDstImage = NULL;
	lDstImage = cvCreateImage(cvSize(SrcImage->width, SrcImage->height), IPL_DEPTH_8U, 1);
	cvCopy(SrcImage, lDstImage);

	int nDstH = SrcImage->height - 1;
	for(int w = 0; w < SrcImage->width; w++)
	{
		nDstH = SrcImage->height - 1;
		for(int h = 0; h < SrcImage->height; h++)
		{
			DstImage->imageData[w + ((nDstH--) * lDstImage->widthStep)] = lDstImage->imageData[w + (h * lDstImage->widthStep)];
		}
	}

	cvReleaseImage(&lDstImage);

	return rslt;
}


BOOL CvThreshHoldRange(IplImage *src, IplImage *dst, int nThreshHoldVal, int nThreshRange)
{
	BOOL rslt = TRUE;

	if(src == NULL || dst == NULL)
	{
		return FALSE;
	}

	if((src->width != dst->width) || (src->height != dst->height) || (src->alphaChannel != dst->alphaChannel))
	{
		return FALSE;
	}

	if(nThreshHoldVal > MAX_VAL)
	{
		nThreshHoldVal = MAX_VAL - 1;
	}

	int nMaxThreshHold = nThreshHoldVal + nThreshRange;
	if(nMaxThreshHold > MAX_VAL)
	{
		nMaxThreshHold = MAX_VAL - 1;
	}

	IplImage *TempImage1, *TempImage2;
	CvRect rcRoi;

	TempImage1 = NULL;
	TempImage1 = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_8U, 1);
	TempImage2 = NULL;
	TempImage2 = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_8U, 1);

	if((src->roi->xOffset != NULL) || (src->roi->yOffset != NULL) ||
		(src->roi->width != NULL) || (src->roi->height != NULL))
	{
		rcRoi.x = src->roi->xOffset;
		rcRoi.y = src->roi->yOffset;
		rcRoi.width = src->roi->width;
		rcRoi.height = src->roi->height;

		cvSetImageROI(TempImage1, rcRoi);
		cvSetImageROI(TempImage2, rcRoi);
	}	

	cvThreshold(src, TempImage1, nThreshHoldVal, 255, CV_THRESH_BINARY);
	cvThreshold(src, TempImage2, nMaxThreshHold, 255, CV_THRESH_BINARY);
	cvAnd(TempImage1, TempImage2, dst);

	if((src->roi->xOffset != NULL) || (src->roi->yOffset != NULL) ||
		(src->roi->width != NULL) || (src->roi->height != NULL))
	{
		cvResetImageROI(TempImage1);
		cvResetImageROI(TempImage2);
	}

	if (TempImage1)		cvReleaseImage(&TempImage1);
	if (TempImage2)		cvReleaseImage(&TempImage2);

	return rslt;
}

BOOL CvCircleSearch(CvSeq *c, CvPoint2D32f *rslt_center, double *rslt_width, double *rslt_height, double *rslt_angle)
{
	BOOL rslt = FALSE;
	CvBox2D box2d;

	if(c != NULL)
	{
		box2d = cvFitEllipse2(c);

		rslt_center->x = box2d.center.x;
		rslt_center->y = box2d.center.y;
		*rslt_width = box2d.size.width / 2.;
		*rslt_height = box2d.size.height / 2.;
		*rslt_angle = box2d.angle;

		rslt = TRUE;
	}

	return rslt;
}
#endif

// RsPort.cpp: implementation of the CRsPort class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRsPort::CRsPort( CString m_portName )
{
   dcb_setup.BaudRate = CBR_9600;
   dcb_setup.ByteSize = 8;
   dcb_setup.Parity   = NOPARITY;
   dcb_setup.StopBits = ONESTOPBIT;
   initComport( m_portName );
}


CRsPort::CRsPort( CString m_portName, DWORD BaudRate, BYTE ByteSize, BYTE Parity, BYTE StopBits )
{
   dcb_setup.BaudRate = BaudRate;
   dcb_setup.ByteSize = ByteSize;
   dcb_setup.Parity   = Parity;
   dcb_setup.StopBits = StopBits;
   initComport( m_portName );
}


CRsPort::~CRsPort()
{
   CloseCommPort();
}

void CRsPort::initComport(CString m_portName)
{
   COMMTIMEOUTS  commTimeOuts;

   m_idComDev = CreateFile( (LPCTSTR) m_portName, GENERIC_READ | GENERIC_WRITE, 
	   0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL );

   if( m_idComDev == (HANDLE) -1) 
   {
		CloseHandle( m_idComDev );
		m_Connect = FALSE;
		//AfxMessageBox(_T("WARNING : 포트를 여는데 실패하였습니다.")); 
   } 
   else 
   {
		SetCommMask( m_idComDev, EV_RXCHAR );
		SetupComm( m_idComDev, 4096, 4096);
		PurgeComm( m_idComDev, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR );
		commTimeOuts.ReadIntervalTimeout = 20;
		commTimeOuts.ReadIntervalTimeout = -1;
		commTimeOuts.ReadTotalTimeoutMultiplier = 0;
		commTimeOuts.ReadTotalTimeoutConstant = 1000;
		commTimeOuts.WriteTotalTimeoutMultiplier = 0;
		commTimeOuts.WriteTotalTimeoutConstant = 1000;
		SetCommTimeouts( m_idComDev, &commTimeOuts );
		m_Connect = SetupConnection();
		osWrite.Offset = 0;
		osWrite.OffsetHigh = 0;
		osRead.Offset = 0;
		osRead.OffsetHigh = 0;
		osRead.hEvent  = CreateEvent(NULL, TRUE, FALSE, NULL);
		osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
   }
}

BOOL CRsPort::SetupConnection()
{
   BOOL fRetVal;
   DCB  dcb;
   
   dcb.DCBlength = sizeof( DCB );
   GetCommState( m_idComDev, &dcb );
   dcb.BaudRate = dcb_setup.BaudRate;
   dcb.ByteSize = dcb_setup.ByteSize;
   dcb.Parity   = dcb_setup.Parity;
   dcb.StopBits = dcb_setup.StopBits;
   dcb.fOutxDsrFlow = 0;
   dcb.fDtrControl = DTR_CONTROL_ENABLE;
   dcb.fOutxCtsFlow = 0;
   dcb.fRtsControl = RTS_CONTROL_ENABLE;
   dcb.fInX = dcb.fOutX = 0; // XON/XOFF
   dcb.XonChar  = 0x11; // ASCII_XON;
   dcb.XoffChar = 0x13; // ASCII_XOFF;
   dcb.XonLim   = 100;
   dcb.XoffLim  = 100;
   dcb.fBinary  = TRUE;
   dcb.fParity  = TRUE;
   fRetVal = SetCommState( m_idComDev, &dcb );
   return fRetVal;
}

void CRsPort::CloseCommPort()
{
  if( m_Connect == FALSE ) return;
  CloseHandle( m_idComDev );
  CloseHandle( osRead.hEvent );
  CloseHandle( osWrite.hEvent );
}

int CRsPort::WriteCommPort(unsigned char *message, DWORD dwLength)
{
   int ret_code;
   ret_code = WriteFile( m_idComDev, message, dwLength, &dwLength, &osWrite);
   return ret_code;
}

int CRsPort::ReadCommPort(unsigned char *message, DWORD length)
{
	/*
   COMSTAT  ComStat;
   DWORD    dwErrorFlags;
   DWORD    dwLength;
   DWORD	dwReadLength = 0;

   CStringA strTemp;
   strTemp.Format("%s",message);

   if( m_Connect == FALSE )  return 0;
   else 
   {
	   ClearCommError( m_idComDev, &dwErrorFlags, &ComStat );
	   dwLength = min((DWORD) length, ComStat.cbInQue);
	   ReadFile( m_idComDev, message, dwLength, &dwReadLength, &osRead );
   }*/

	CStringA strTemp;
	strTemp.Format("%s",message);

	DWORD    dwErrorFlags;
	COMSTAT comstat;
	ClearCommError( m_idComDev, &dwErrorFlags, &comstat);
	DWORD dwRead = comstat.cbInQue;
	DWORD dwError;
	
	if (dwRead > 0)
	{
		if (! ReadFile( m_idComDev, message, length, &dwRead, &osRead))
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				// timeouts에 정해준 시간만큼 기다려준다.
				while (! GetOverlappedResult( m_idComDev, &osRead, &dwRead, TRUE))
				{
					dwError = GetLastError();
					if (dwError != ERROR_IO_INCOMPLETE)
					{
						ClearCommError( m_idComDev, &dwErrorFlags, &comstat);
						break;
						Delay(1,TRUE);
					}
				}
			}
			else
			{
				dwRead = 0;
				ClearCommError( m_idComDev, &dwErrorFlags, &comstat);
			}
		}
	}

   if(dwRead == 0)
   {
	   CStringA str;
	   str.Format("%s", message);

	   if(strTemp != str)
	   {	   
		   return str.GetLength();
	   }	   
   }
  

   return dwRead;
}

bool CRsPort::IsCommPortOpen()
{
	if(m_Connect)
		return true;
	return false;
}



CBarcodeRsPortType::CBarcodeRsPortType(CString m_portName, DWORD BaudRate, BYTE ByteSize, BYTE Parity, BYTE StopBits, int nBarcodeMaxLength, BYTE byEndChar)
{
	m_nBarcodeMaxLength = nBarcodeMaxLength;
	m_byEndChar = byEndChar;
	m_RsPort = new CRsPort(m_portName);//, BaudRate, ByteSize, Parity, StopBits);
	SetInitBarcode();
	//InitThread();
}

CBarcodeRsPortType::~CBarcodeRsPortType()
{
	//ReleaseThread();

	m_RsPort->~CRsPort();
	delete m_RsPort;
	m_RsPort = NULL;
}

void CBarcodeRsPortType::SetBarcodeMaxLength(int nLength)
{
	m_nBarcodeMaxLength = nLength;
	if(m_strBarcode.GetLength() + 1 > m_nBarcodeMaxLength)
	{
		CString strTemp;
		strTemp.Format(_T("%s"), m_strBarcode.Mid(0, m_nBarcodeMaxLength));
		m_strBarcode.Format(_T("%s"), strTemp);
	}
}

BOOL CBarcodeRsPortType::InitThread()
{
	m_bThreadBarcodeStart = FALSE;
	m_bThreadBarcodeEnd = FALSE;

	m_pThreadBarcodeRead = AfxBeginThread(BarcodeReadThread, this, THREAD_PRIORITY_NORMAL, 0);
	m_pThreadBarcodeRead->m_bAutoDelete = FALSE;

	return TRUE;
}

BOOL CBarcodeRsPortType::WaitEndThread()
{
	BOOL rslt = TRUE;

	if(m_pThreadBarcodeRead != NULL)
	{
		DWORD dwRet = WaitForSingleObject(m_pThreadBarcodeRead->m_hThread, BARCODE_READ_THREAD_END_TIMEOUT);

		if(dwRet == WAIT_FAILED)
		{
			rslt = FALSE;
		}
		else if(dwRet == WAIT_ABANDONED)
		{
			rslt = FALSE;
		}
		else if(dwRet == WAIT_TIMEOUT)
		{
			rslt = FALSE;
		}

		if(rslt)
			m_pThreadBarcodeRead = NULL;
	}

	return rslt;
}

BOOL CBarcodeRsPortType::ReleaseThread()
{
	m_bThreadBarcodeStart = FALSE;
	m_bThreadBarcodeEnd = TRUE;
	
	return WaitEndThread();
}

BOOL CBarcodeRsPortType::GetBarcodeConnect()
{
	return m_RsPort->m_Connect;
}

CString CBarcodeRsPortType::GetBarcodeReader()
{
	CString strRslt, strTemp, strTemp1;
	BYTE sPacket[MAX_PATH];
	memset(sPacket, 0, sizeof(BYTE) * MAX_BYTE_CNT);
	memset(m_byReadBarcode, 0, sizeof(BYTE) * MAX_BYTE_CNT);
	strRslt.Format(_T(""));

	if(m_RsPort->m_Connect)
	{
		m_RsPort->ReadCommPort(sPacket, BARCODE_MAX_STRING_CNT * 2);

#ifdef ASCII_READER
		strTemp.Format(_T(""));
		strTemp1.Format(_T(""));
		m_nReadLenght = 0;

		for(int i = 0; i < MAX_BYTE_CNT; i++)
		{
			if(sPacket[i] != m_byEndChar)
			{
				strTemp.Format(_T("%c"), sPacket[i]);
				strRslt.Format(_T("%s%s"), strTemp1, strTemp);
				strTemp1.Format(_T("%s"), strRslt);
				m_byReadBarcode[i] = sPacket[i];
			}
			else
			{
				m_nReadLenght = i + 1;
				break;
			}
		}
#else
		strTemp1.Format(_T("%S"), sPacket);
#endif

		strRslt.Format(_T("%s"), strTemp1.Mid(0, m_nReadLenght));
	}

	return strRslt;
}

void CBarcodeRsPortType::SetBarcodeReadStart(BOOL isStart)
{
	m_bThreadBarcodeStart = isStart;
	if(!isStart)
		SetInitBarcode();
}

void CBarcodeRsPortType::ThreadBarcodeRead()
{
	while(TRUE)
	{
		//Sleep(SERIAL_READ_DELAY);

		::Delay(SERIAL_READ_DELAY);

		if(m_bThreadBarcodeStart)
		{
			CString strBarcode;
			strBarcode.Format(_T("%s"), GetBarcodeReader());
			
			if(strBarcode.GetLength() >= m_nBarcodeMaxLength)
			{
				m_strBarcode.Format(_T("%s"), strBarcode);
			}
		}

		if(m_bThreadBarcodeEnd)
		{
			m_bThreadBarcodeStart = FALSE;
			m_bThreadBarcodeEnd = FALSE;
			break;
		}
	}
}

UINT CBarcodeRsPortType::BarcodeReadThread(LPVOID pParam)
{
	CBarcodeRsPortType *pdlg = (CBarcodeRsPortType *)pParam;

	pdlg->ThreadBarcodeRead();

	return 0;
}

CBarcodeKeyboardType::CBarcodeKeyboardType(int nBarcodeMaxLength, BYTE byEndChar)
{
	m_nBarcodeMaxLength = nBarcodeMaxLength;
	m_byEndChar = byEndChar;
	m_bBarcodEndChar = FALSE;
}

CBarcodeKeyboardType::~CBarcodeKeyboardType()
{

}

CString CBarcodeKeyboardType::GetBarcodeString()
{
	if(m_bBarcodEndChar)
	{
		return m_strBarcode;
	}
	else
	{
		return _T("-");
	}
}
BOOL CBarcodeKeyboardType::SetBarcodeChar(BYTE byChar)
{
	BOOL rslt = TRUE;
	BOOL bEndChar = FALSE;
	if(byChar == m_byEndChar)
	{
		rslt = FALSE;
		bEndChar = TRUE;
	}
	else
	{
		if(m_strBarcode.GetLength() >= m_nBarcodeMaxLength)
		{
			rslt = FALSE;
			bEndChar = TRUE;
		}
	}

	if(rslt)
	{
		m_bBarcodEndChar = FALSE;
		CString strTemp;
		strTemp.Format(_T("%s%c"), m_strBarcode, byChar);
		m_strBarcode.Format(_T("%s"), strTemp);
	}
	else
	{
		if(bEndChar)
		{
			m_bBarcodEndChar = TRUE;
		}
	}

	return rslt;
}