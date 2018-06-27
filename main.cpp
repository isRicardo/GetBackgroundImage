#include<opencv2\opencv.hpp>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

typedef int     INT;
typedef unsigned long ULONG;
typedef unsigned char UCHAR;

#define HCOM_OK     0
#define HCOM_ERR    1
#define NULL_PTR    0

ULONG getBackGround(Mat& srcFrame, Mat& dstFrame);
ULONG getPixelValueInRegion(Mat& srcFrame, Point& center, INT& regionSize, UCHAR* regionValue);
ULONG sortPixelValueInRegion(UCHAR* regionValue, INT startPos, INT endPos);
ULONG calBackgroundValue(UCHAR* regionValue, INT& regionSize, INT& sampleNum, UCHAR& backgroundValue);

int main()
{
    Mat srcFrame;
    Mat backgroungFrame;

    srcFrame = imread("");  //����һ���Ҷ�ͼ��
    getBackGround(srcFrame, backgroungFrame);

    imshow("srcFrame", srcFrame);
    imshow("backgroungFrame", backgroungFrame);

    return 0;
}

/*****************************************************************************
  �� �� ��  : GetBackGround
  ��������  : ��ȡ����
  �������  :  
  �������  :  
  �� �� ֵ  : 
  ���ú���  : N/A
  ��������  : 
  ��    ��   : chengcy
 *****************************************************************************/
ULONG getBackGround(Mat& srcFrame, Mat& dstFrame)
{
    ULONG ret = HCOM_OK;

    if (NULL == srcFrame.data)
    {
        return HCOM_ERR;
    }

    INT rows = srcFrame.rows;
    INT cols = srcFrame.cols;

    INT regionSize = 11;    //�����ߴ�
    INT sampleNum = 3;

    UCHAR* regionPixelValue = NULL;
    regionPixelValue = (UCHAR*)malloc(sizeof(UCHAR) * regionSize * regionSize);
    if (NULL_PTR == regionPixelValue)
    {
        return ret;
    }
    UCHAR backgroundValue = 0;

    dstFrame = cv::Mat::zeros(rows, cols, srcFrame.type());
    UCHAR* dstData = dstFrame.data;

    /* �������ĵ� */
    for (INT x = regionSize / 2; x < (cols - regionSize / 2); x++)
    {
        for (INT y = regionSize / 2; y < (rows - regionSize / 2); y++)
        {
            //�����еĵ�
            ret = getPixelValueInRegion(srcFrame, Point(x, y), regionSize, regionPixelValue);

            ret = calBackgroundValue(regionPixelValue, regionSize, sampleNum, backgroundValue);

            dstData[y * srcFrame.cols + x] = backgroundValue;
        }
    }

    /* ��ȫ�߽� */
    for (INT x = 0; x < regionSize / 2; x++)
    {
        for (INT y = 0; y < srcFrame.rows; y++)
        {
            dstData[y * srcFrame.cols + x] = dstData[y * srcFrame.cols + regionSize / 2];
        }
    }

    for (INT x = (cols - regionSize / 2); x < srcFrame.cols; x++)
    {
        for (INT y = 0; y < srcFrame.rows; y++)
        {
            dstData[y * srcFrame.cols + x] = dstData[y * srcFrame.cols + (cols - regionSize / 2 - 1)];
        }
    }

    for (INT x = regionSize / 2; x < (srcFrame.cols - regionSize / 2); x++)
    {
        for (INT y = 0; y < regionSize / 2; y++)
        {
            dstData[y * srcFrame.cols + x] = dstData[(regionSize / 2) * srcFrame.cols + x];
        }
    }

    for (INT x = regionSize / 2; x < (srcFrame.cols - regionSize / 2); x++)
    {
        for (INT y = (rows - regionSize / 2); y < srcFrame.rows; y++)
        {
            dstData[y * srcFrame.cols + x] = dstData[(rows - regionSize / 2 - 1) * srcFrame.cols + x];
        }
    }

    free(regionPixelValue);
    return ret;
}

/*****************************************************************************
  �� �� ��  : GetPixelValueInRegion
  ��������  : ��֪���ĵ㣬 ��������ڵĵ������
  �������  :  
  �������  :  
  �� �� ֵ  : 
  ���ú���  : N/A
  ��������  : 
  ��    ��   : chengcy
 *****************************************************************************/
ULONG getPixelValueInRegion(Mat& srcFrame, Point& center, INT& regionSize, UCHAR* regionValue)
{
    ULONG ret = HCOM_OK;

    Point regionPos;
    UCHAR* srcData = srcFrame.data;

    //���Խ��
    if (center.x - regionSize / 2 < 0 || center.y - regionSize / 2 <0
    ||  center.x + regionSize / 2 > srcFrame.cols || center.x + regionSize / 2 > srcFrame.rows)
    {
        cout << "��������Խ��" << endl;
        return HCOM_ERR;
    }

    for (INT i = 0; i < regionSize; i++)
    {
        for (INT j = 0; j < regionSize; j++)
        {
            regionPos.x = i + center.x - regionSize / 2;
            regionPos.y = j + center.y - regionSize / 2;

            *regionValue = srcData[regionPos.y * srcFrame.cols + regionPos.x];
            regionValue++;
        }
    }

    return ret;
}

/*****************************************************************************
  �� �� ��  : SortPixelValueInRegion
  ��������  : �������ڵ�����ֵ��������
  �������  :  
  �������  :  
  �� �� ֵ  : 
  ���ú���  : N/A
  ��������  : 
  ��    ��   : chengcy
 *****************************************************************************/
ULONG sortPixelValueInRegion(UCHAR* regionValue, INT startPos, INT endPos)
{
    ULONG ret = HCOM_OK;
    INT i = startPos;
    INT j = endPos;
    UCHAR temp = regionValue[i];
    
    if (startPos < endPos)
    {
        while (i < j)
        {
            while (regionValue[j] >= temp && (i < j))
            {
                j--;
            }
            regionValue[i] = regionValue[j];
            while (regionValue[i] <= temp && (i < j))
            {
                i++;
            }
            regionValue[j] = regionValue[i];
        }
        regionValue[i] = temp;

        sortPixelValueInRegion(regionValue, startPos, i - 1);
        sortPixelValueInRegion(regionValue, j + 1, endPos);
    }
    else
    {
        return ret;
    }

    return ret;
}

/*****************************************************************************
  �� �� ��  : CalBackgroundValue
  ��������  : ���㱳��ֵ
  �������  :  
  �������  :  
  �� �� ֵ  : 
  ���ú���  : N/A
  ��������  : 
  ��    ��   : chengcy
 *****************************************************************************/
ULONG calBackgroundValue(UCHAR* regionValue, INT& regionSize, INT& sampleNum, UCHAR& backgroundValue)
{
    ULONG ret = HCOM_OK;
    INT sum = 0;

    //�������ڵ�ֵ����
    ret = sortPixelValueInRegion(regionValue, 0, (regionSize * regionSize) - 1);

    //ȡ��sampleNum��Сֵ, ȡƽ��
    for (INT i = 0; i < sampleNum; i++)
    {
        sum += regionValue[i];
    }
    backgroundValue = sum / sampleNum;

    if (backgroundValue > 255)
    {
        backgroundValue = 255;
    }

    return ret;
}