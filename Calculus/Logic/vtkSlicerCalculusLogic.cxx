/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/
//Qt includes
#include<qdebug.h>
// Calculus Logic includes
#include "vtkSlicerCalculusLogic.h"

// MRML includes
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

// STD includes
#include <cassert>
//----------------��ɺɺ���---------------
#include "vtkSlicerCalculusLogic.h"

// MRML includes
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkMatrix4x4.h>

#include "vtkMRMLMarkupsFiducialNode.h"
#include "vtkMRMLMarkupsNode.h"
#include "vtkSlicerMarkupsLogic.h"
// STD includes
#include <cassert>
//-------------
#include <vtkImageData.h>
#include <cassert>
#include <vector>
#include <vtkIOStream.h>
#include<sstream> 
#include<qstring.h>
#include <QDebug>
#include <fstream>

#include <ActiveQt/qaxobject.h>
#include <ActiveQt/qaxbase.h>
#include <QtGui>

#include <Qtcore/qstring.h>
#include <Qtcore/QFile>
#include <stdexcept>

#include <QHash>
#include "vtkSlicerCalculusLogic.h"
using namespace std;
class QAxObject;
//----------------------------��ӽ���----------------------------

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerCalculusLogic);

//----------------------------------------------------------------------------
vtkSlicerCalculusLogic::vtkSlicerCalculusLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerCalculusLogic::~vtkSlicerCalculusLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerCalculusLogic::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}


double vtkSlicerCalculusLogic::s_sliceThick;//����Ƕ���Ƭ���
double vtkSlicerCalculusLogic::s_uWater;//ˮ��˥��ϵ��
double vtkSlicerCalculusLogic::s_materialThick;//���Ϻ��
//---------------------------------------------------------------------------
void vtkSlicerCalculusLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
	vtkNew<vtkIntArray> events;
	events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
	events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
	events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
	this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerCalculusLogic::RegisterNodes()
{
	assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerCalculusLogic::UpdateFromMRMLScene()
{
	assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerCalculusLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerCalculusLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

//���õ�ǰ�����ݵ�logic
void vtkSlicerCalculusLogic::setVolumesLogic(vtkSlicerVolumesLogic* logic)
{
	this->volumesLogic = logic;
}
vtkSlicerVolumesLogic* vtkSlicerCalculusLogic::getVolumesLogic()
{
	return this->volumesLogic;
}
void vtkSlicerCalculusLogic::setCropVolumeLogic(vtkSlicerCropVolumeLogic* logic)
{
	this->cropVolumeLogic = logic;
}

vtkSlicerCropVolumeLogic* vtkSlicerCalculusLogic::getCropVolumeLogic()
{
	return this->cropVolumeLogic;
}



QHash<QString, double> vtkSlicerCalculusLogic::acqSliceData(vtkImageReslice* reslice, vtkMRMLSliceNode* sliceNode, vtkMRMLVolumeNode* volumeNode)
{



	vtkSmartPointer<vtkMatrix4x4> sliceToRAS = sliceNode->GetSliceToRAS();//sliceԭ��ͷ���������
	vtkSmartPointer<vtkMatrix4x4> RASToIJKMatrix = vtkSmartPointer<vtkMatrix4x4>::New();//4*4����
	vtkSmartPointer<vtkMatrix4x4> IJKToRASMatrix = vtkSmartPointer<vtkMatrix4x4>::New();//ת������
	volumeNode->GetRASToIJKMatrix(RASToIJKMatrix);
	volumeNode->GetIJKToRASMatrix(IJKToRASMatrix);

	double normal[4];//������
	normal[0] = sliceToRAS->GetElement(0, 2);
	normal[1] = sliceToRAS->GetElement(1, 2);
	normal[2] = sliceToRAS->GetElement(2, 2);
	normal[3] = 1;

	qDebug() << "normal:" << normal[0] << " " << normal[1] << " " << normal[2] << " " << normal[3];


	double origin[4];//������
	origin[0] = sliceToRAS->GetElement(0, 3);
	origin[1] = sliceToRAS->GetElement(1, 3);
	origin[2] = sliceToRAS->GetElement(2, 3);
	origin[3] = 1;

	qDebug() << "origin:" << origin[0] << " " << origin[1] << " " << origin[2] << " " << origin[3];

	//����ƽ�淽��

	double A = normal[0];
	double B = normal[1];
	double C = normal[2];
	double d = -(A * origin[0] + B * origin[1] + C * origin[2]);//ƽ�淽��D


	//�õ�����>0��IJK����
	vtkImageData* orgimage = reslice->GetImageDataInput(0);

	//�ߴ糤���
	int* dims = orgimage->GetDimensions();
	QList<double*> pointList;//��ʯ����Ч��


	int row = dims[1];
	int column = dims[0];
	int li = dims[2];

	uint16* pixel = new uint16[row*column *li]();
	uint16* q = pixel;
	int t = 0;
	QString fileName = "before";
	QByteArray array = fileName.toLocal8Bit();
	FILE* file = fopen(array.data(), "wb");
	if (!file)
	{
		qDebug() << fileName << "�����ļ���ʧ�ܣ�\n";
	}
	for (int k = 0; k < li; k++)
	{
		/*QString fileName = QString("before-%1").arg(k);
		QByteArray array = fileName.toLocal8Bit();
		FILE* file = fopen(array.data(), "wb");
		if (!file)
		{
		qDebug() << fileName << "�����ļ���ʧ�ܣ�\n";
		}*/
		for (int j = 0; j < row; j++)
		{
			for (int i = 0; i < column; i++)
			{

				uint16* p = (uint16*)(orgimage->GetScalarPointer(i, j, k));
				q[i + j*column + k*row*column] = *p;
				if (*p >0)
				{

					//std::cout << " p[" << i << "][" << j << "]" << "[" << k << "]" << *p << "  t=" << t << std::endl;
					t++;
					double*point = new double[5];
					point[0] = j;
					point[1] = i;
					point[2] = k;
					point[3] = 1;
					point[4] = *p;

					pointList.append(point);
				}



			}

		}

	}

	fwrite(pixel, sizeof(uint16), column *row*li, file);
	fclose(file);

	//�����е�ת����RAS����

	QList<double*> rasPointList;
	//ɸѡһ�������㵽��ľ���<2
	QList<double*> resultPointList;
	
	for (int m1 = 0; m1 < pointList.count(); m1++)
	{
		double*point = pointList.at(m1);
		double* rasPoint = IJKToRASMatrix->MultiplyDoublePoint(point);
		double distance = qAbs(A*rasPoint[0] + B*rasPoint[1] + C*rasPoint[2] + d) / qSqrt(A*A + B*B + C*C);
		if (distance < vtkSlicerCalculusLogic::s_sliceThick)//distance�б仯����������ֵ����������仯
		{
			//qDebug() << "m1:" << m1 << " ijkPoint :" << point[0] << " " << point[1] << " " << point[2] << " " << point[3] <<  ""<< point[4];
			//qDebug() << "m1:" << m1 << " rasPoint :" << rasPoint[0] << " " << rasPoint[1] << " " << rasPoint[2] << " " << rasPoint[3];//IJKת����RAS�������б仯
			double* resultPoint = RASToIJKMatrix->MultiplyDoublePoint(rasPoint);
			//qDebug() << "distance" << distance << " resultPointijk:" << resultPoint[0] << " " << resultPoint[1] << " " << resultPoint[2] << " " << resultPoint[3] <<" " << resultPoint[4];
			double* result = new double[5]();
			result[0] = resultPoint[0];
			result[1] = resultPoint[1];
			result[2] = resultPoint[2];
			result[3] = resultPoint[3];
			result[4] = point[4];
			resultPointList.append(result);
			qDebug() << "distance" << distance << " resultPointijk:" << result[0] << " " << result[1] << " " << result[2] << " " << result[3] << " " << result[4];

		}
		rasPointList.append(rasPoint);
	}
	qDebug() << "rasPointList size" << rasPointList.size() << " resultPointList size:" << resultPointList.size()<<endl;


	double* sliceDataDouble;
	QHash<QString, double> circleParamsHash;
	if (resultPointList.size() > 0)
	{
		sliceDataDouble = new double[resultPointList.size()]();
		for (int i = 0; i < resultPointList.size(); i++)
		{
			double* point = resultPointList.at(i);
			sliceDataDouble[i] = point[4];//�õ�CTֵ
		}
		circleParamsHash.insert("max", max(sliceDataDouble, resultPointList.size()));
		circleParamsHash.insert("min", min(sliceDataDouble, resultPointList.size()));
		circleParamsHash.insert("average", aver(sliceDataDouble, resultPointList.size()));
		circleParamsHash.insert("AOD", AOD(sliceDataDouble, resultPointList.size(), vtkSlicerCalculusLogic::s_uWater, vtkSlicerCalculusLogic::s_materialThick));
		circleParamsHash.insert("IOD", IOD(sliceDataDouble, resultPointList.size(), vtkSlicerCalculusLogic::s_uWater, vtkSlicerCalculusLogic::s_materialThick));
	
		if (sliceDataDouble!=0)
			delete[] sliceDataDouble;
	}

	
	
	if (pixel!=0)
		delete[] pixel;
	//�ͷ����е�
	for (int index = 0; index < pointList.size(); index++)
	{
		double* p = pointList.at(index);
		delete[] p;
	}
	//�ͷ����е�
	for (int index = 0; index < resultPointList.size(); index++)
	{
		double* p = resultPointList.at(index);
		delete[] p;
	}

	std::cout << "sliceToRAS:" << std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			std::cout << " [" << i << "][" << j << "] " << sliceToRAS->GetElement(i, j);
		}
		std::cout << std::endl;
	}

	//reslice->SetResliceAxes(resliceAxes);
	std::cout << "RAS to IJK:" << std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			std::cout << " [" << i << "][" << j << "] " << RASToIJKMatrix->GetElement(i, j);
		}
		std::cout << std::endl;
	}

	std::cout << "IJK to RAS:" << std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			std::cout << " [" << i << "][" << j << "] " << IJKToRASMatrix->GetElement(i, j);
		}
		std::cout << std::endl;
	}




	return circleParamsHash;


}


//--------------��ɺɺ��ӳ���----------------------------------
double vtkSlicerCalculusLogic::max(double a[], int n)
{
	double  max;
	max = a[0];
	int i;
	for (i = 1; i<n; i++)
	{
		if (a[i]>max) max = a[i];
	}
	cout << max << endl;
	return max;
}
double vtkSlicerCalculusLogic::min(double a[], int n)
{
	double min;
	min = a[0];
	int i;
	for (i = 1; i < n; i++)
	{
		if (a[i] < min) min = a[i];
	}
	cout << min << endl;
	return  min;
}

double vtkSlicerCalculusLogic::aver(double a[], int n)
{
	double r = 0;
	int i;
	for (i = 0; i < n; i++)
		r += a[i];
	r /= (double)n;
	cout << r << endl;
	return r;
}
//--------------------------------------------------------------
//��ȡԲ���������� 
QHash<QString, double> vtkSlicerCalculusLogic::aqcCircleData(vtkMRMLVolumeNode* input, vtkMRMLMarkupsFiducialNode* markups)//vtkMRMLScalarVolumeNode,����ֵ��ָ��ĺ���
{
	/*vtkNew<vtkImageData> outputImageData;
	outputImageData->DeepCopy(input->GetImageData());*/
	/*int originalImageExtents[6];
	outputImageData->GetExtent(originalImageExtents);*/
	//��׼���������
	vtkSmartPointer<vtkMatrix4x4> RASToIJKMatrix = vtkSmartPointer<vtkMatrix4x4>::New();//4*4����
	input->GetRASToIJKMatrix(RASToIJKMatrix);//����ϵת��
	int markupNum = markups->GetNumberOfFiducials();
	qDebug() << markupNum << endl;
	vector<float> position;  //temp variance to store points' IJK 3 coordinates�洢��ά����

	vector<float> worldposition;  //temp variance to store points' World 3 coordinates

	vector<vector <float> > points;  //vector to store sorted markup points in IJK coordinate

	vector<vector <float> > worldpoints;
	if (markupNum != 4)//
		qDebug() << "error No markupNum =4";
	else
	{
		for (int i = 0; i < markupNum; i++)
		{
			double pos[4];//�ĸ���
			markups->GetNthFiducialWorldCoordinates(i, pos);//�õ�ÿ���������
			cout << "WorldPOS:" << pos[0] << ";" << pos[1] << ";" << pos[2] << ";" << pos[3] << endl;//�ĸ��������
			float temp[4];
			std::copy(pos, pos + 4, temp);
			float* ijkpos = RASToIJKMatrix->MultiplyPoint(temp);
			cout << "IJKPOS:" << ijkpos[0] << ";" << ijkpos[1] << ";" << ijkpos[2] << ";" << ijkpos[3] << endl;//IJK����ϵ�µ�λ������
			for (int j = 0; j < 3; j++)
			{
				position.push_back(ijkpos[j]);
				worldposition.push_back(temp[j]);
			}
			if (i == 0)
			{
				points.push_back(position);
				worldpoints.push_back(worldposition);
			}
			else
			{
				int j;
				for (j = 0; j<points.size(); j++)
				{
					if (points.at(j).at(2)>position.at(2))
						break;
				}
				if (j == points.size())
				{
					points.push_back(position);
					worldpoints.push_back(worldposition);
				}
				else
				{
					points.insert(points.begin() + j, position);
					worldpoints.insert(worldpoints.begin() + j, worldposition);
				}
			}
			position.clear();
			worldposition.clear();
		}
	}

	vector<float> star_first = points.front();
	vector<float> star_last = points.back();
	vector<float> box1 = points.at(1);
	vector<float> box2 = points.at(2);

	cout << "POINTS[0]:" << star_first[0] << ";" << star_first[1] << ";" << star_first[2] << endl;
	cout << "POINTS[1]:" << box1[0] << ";" << box1[1] << ";" << box1[2] << endl;
	cout << "POINTS[2]:" << box2[0] << ";" << box2[1] << ";" << box2[2] << endl;
	cout << "POINTS[3]:" << star_last[0] << ";" << star_last[1] << ";" << star_last[2] << endl;//points���ĸ���ǵ����꣬��ʸ��

	vtkImageData* outputImageData = input->GetImageData();//�õ����������ݵ�Imagedata
	int* dims = outputImageData->GetDimensions();//ԭʼͼ��ά��
	double origin[3];
	outputImageData->GetOrigin(origin);
	qDebug() << "o" << origin[0] << " " << origin[1] << " " << origin[2];

	double spaceing[3];
	outputImageData->GetSpacing(spaceing);
	qDebug() << "���ؼ��:" << spaceing[0] << " " << spaceing[1] << "" << spaceing[2] << endl;

	//int dims[3]s;
	//outputImageData->GetDimensions(dims);
	//ExcelExportHelper excel;
	//	QString b;
	//pixel = (double*)malloc(sizeof(double));//
	int n = dims[0] * dims[1];
	double  *pixel;
	pixel = (double*)malloc(sizeof(double)*n);//ָ������

	//double pixel[990] = { 0 };
	unsigned short *temp = { 0 };
	double temp_value;



	FILE* outFile = fopen("after.txt", "wb");
	if (!outFile)
	{
		qDebug("outFile�����ļ���ʧ�ܣ�\n");
	}
	int size = 0;


	int z = dims[2] - 1;
	//for (int z = 1; z < dims[2]; z++)
	//{
	for (int y = 0; y < dims[1]; y++)
	{
		for (int x = 0; x < dims[0]; x++)
		{
			/*if (x < 33 && y < 30 && 2 < z < 4)*/
			/*	if (x < 33 && y < 30)
			{*/
			temp = (unsigned short *)(outputImageData->GetScalarPointer(x, y, z));

			temp_value = temp[0];
			if (temp_value < 60000)
			{

				pixel[x + dims[0] * y] = temp_value;//�и���������ת����unsigned char��double
			}
			else
				pixel[x + dims[0] * y] = 0;
			std::cout << pixel[x + dims[0] * y] << " ";

			//}
		}
		std::cout << std::endl;

	}
	/*	std::cout << std::endl;
	}*/

	//δ�ָ�ԭʼ����ƽ�������С
	double a = max(pixel, n);
	double b = min(pixel, n);
	//aver(pixel, n);
	//--------------------------ԭʼ���ݹ�һ��,ѹ����255��
	/*int n = dims[0] * dims[1];*/
	double  *graypixel;
	graypixel = (double*)malloc(sizeof(double)*n);
	for (int i = 0; i < dims[1]; i++)//��30
	{
		for (int j = 0; j < dims[0]; j++)//��33
		{
			graypixel[j + dims[0] * i] = ((pixel[j + dims[0] * i] - b) / (a - b)) * 255;
		}
	}
	//---------------------------------------

	//----------��ֵ�ָ����ָ��ǶԵ�----------------------------

	int nMaxIter = 100;
	int iDiffRec = 0;
	int F[256] = { 0 }; //ֱ��ͼ����  

	int iTotalGray = 0;//�Ҷ�ֵ��  
	int iTotalPixel = 0;//��������  

	int bt;//ĳ�������ֵ  

	double iThrehold, iNewThrehold;//��ֵ���·�ֵ  
	for (int j = 0; j < n; j++)
	{
		bt = (int)graypixel[j];
		F[bt]++;
	}//ͳ��ÿ���Ҷȼ��ĸ���

	double iMaxGrayValue;
	iMaxGrayValue = max(graypixel, n);
	double iMinGrayValue;
	iMinGrayValue = min(graypixel, n);

	//�����С�Ҷ�ֵ���Ѿ������max min
	//20 uchar iMaxGrayValue = 0, iMinGrayValue = 255;//ԭͼ���е����Ҷ�ֵ����С�Ҷ�ֵ  

	double iMeanGrayValue1, iMeanGrayValue2;//uchar��double����

	iThrehold = 0;//���ո�����ֵ
	iNewThrehold = (iMinGrayValue + iMaxGrayValue) / 2;//��ʼ��ֵ  
	iDiffRec = iMaxGrayValue - iMinGrayValue;

	for (int a = 0; (abs(iThrehold - iNewThrehold) > 0.5) && a < nMaxIter; a++)//������ֹ����  
	{
		iThrehold = iNewThrehold;
		//С�ڵ�ǰ��ֵ���ֵ�ƽ���Ҷ�ֵ  
		for (int i = iMinGrayValue; i < iThrehold; i++)
		{
			iTotalGray += F[i] * i;//F[]�洢ͼ����Ϣ  
			iTotalPixel += F[i];
		}
		iMeanGrayValue1 = (double)(iTotalGray / iTotalPixel);
		//���ڵ�ǰ��ֵ���ֵ�ƽ���Ҷ�ֵ   
		iTotalPixel = 0;
		iTotalGray = 0;
		for (int j = iThrehold + 1; j < iMaxGrayValue; j++)
		{
			iTotalGray += F[j] * j;//F[]�洢ͼ����Ϣ  
			iTotalPixel += F[j];
		}
		iMeanGrayValue2 = (double)(iTotalGray / iTotalPixel);

		iNewThrehold = (iMeanGrayValue2 + iMeanGrayValue1) / 2; //�·�ֵ  
		iDiffRec = abs(iMeanGrayValue2 - iMeanGrayValue1);
	}
	iThrehold = (iThrehold / 255)*(a - b) + b;
	cout << "The Threshold of this Image in imgIteration is:" << iThrehold << endl;
	//-------------------------
	int counter = 0;
	for (int i = 0; i < dims[1]; i++)//��30
	{
		for (int j = 0; j < dims[0]; j++)//��33
		{
			if (pixel[j + dims[0] * i] < iThrehold)//���С����ֵ�����Ϊ0
			{
				double value1 = 0;
				fwrite(&value1, sizeof(double), 1, outFile);
				pixel[j + dims[0] * i] = 0;
			}
			else
			{
				++counter;
				double value = 0;
				value = ((pixel[j + dims[0] * i] - b) / (a - b)) * 255;//����������ǶԵ�
				fwrite(&value, sizeof(double), 1, outFile);
				//pixel[j + dims[0] * i] = value;


			}
		}

	}
	//�ָ����������
	double  *pixelsegmentation;
	pixelsegmentation = (double*)malloc(sizeof(double)*counter);
	int i = 0;
	for (int j = 0; j < n; j++)
	{
		if (pixel[j])
		{
			pixelsegmentation[i] = pixel[j];
			std::cout << pixelsegmentation[i] << " ";
			i++;
			continue;//��������ѭ����continue��������ѭ��
		}
		//*pixelsegmentation++;//pixelsegmentation++��֮����
	}

	fclose(outFile);
	QHash<QString, double> circleParamsHash;
	circleParamsHash.insert("max", max(pixelsegmentation, counter));
	circleParamsHash.insert("min", min(pixelsegmentation, counter));
	circleParamsHash.insert("average", aver(pixelsegmentation, counter));
	circleParamsHash.insert("AOD", AOD(pixelsegmentation, counter, vtkSlicerCalculusLogic::s_uWater, vtkSlicerCalculusLogic::s_materialThick));
	circleParamsHash.insert("IOD", IOD(pixelsegmentation, counter, vtkSlicerCalculusLogic::s_uWater, vtkSlicerCalculusLogic::s_materialThick));
	qDebug() << circleParamsHash.value("max") << endl;
	return circleParamsHash;


}


//---------------��ӽ���----------------------------------
double vtkSlicerCalculusLogic::AOD(double a[], int n, double m, double d)//n ���ظ��� ,m��ˮ��˥��ϵ��,d���Ϻ��(ÿ�����ص�ĺ��)��ÿ��OD
{
	double r = 0;
	for (int i = 0; i < n; i++)
	{
		r = ((a[i] / 1000)*m + m)*d;
		a[i] = log10(exp(r));//ÿһ��ָ��
	}
	//cout << aver(a, n) << endl;
	return aver(a, n);
}
double vtkSlicerCalculusLogic::IOD(double a[], int n, double m, double d)
{
	double r = 0;
	double sum = 0;
	for (int i = 0; i < n; i++)
	{
		r = ((a[i] / 1000)*m + m)*d;
		a[i] = log10(exp(r));//ÿһ��ָ��
		sum += a[i];//���
	}
	//cout << sum << endl;
	return sum;
}

//-----------------------------------------------------------------------
//��ȡ��ֱ�����slice����
QHash<QString, double> vtkSlicerCalculusLogic::acqSliceVerticalData(vtkMRMLVolumeNode* input,double offset,QString direction)
{
	//�õ�����>0��IJK����
	vtkImageData* orgimage = input->GetImageData();


	
	//�ߴ糤���
	int* dims = orgimage->GetDimensions();
	QList<double*> pointList;//��ʯ����Ч��


	int row = dims[1];
	int column = dims[0];
	int li = dims[2];




	vtkSmartPointer<vtkMatrix4x4> RASToIJKMatrix = vtkSmartPointer<vtkMatrix4x4>::New();//4*4����
	input->GetRASToIJKMatrix(RASToIJKMatrix);

	double* rasPoint = new double[4]();
	memset(rasPoint,0,4);
	if (direction=="X")
		rasPoint[0] = offset;
	if (direction == "Y")
		rasPoint[1] = offset;
	if (direction == "Z")
		rasPoint[2] = offset;
	rasPoint[3] = 1;

	double* ijkPoint = RASToIJKMatrix->MultiplyDoublePoint(rasPoint);
	int intOffset;
	if (direction == "X")
		intOffset= ijkPoint[0];
	if (direction == "Y")
		intOffset = ijkPoint[1];
	if (direction == "Z")
		intOffset = ijkPoint[2];
	uint16* pixel = new uint16[row*column *li]();
	uint16* q = pixel;
	int index = 0;//��Ч����
	int t = 0;
	for (int k = 0; k < li; k++)
	{
		
		for (int j = 0; j < row; j++)
		{
			for (int i = 0; i < column; i++)
			{

				uint16* p = (uint16*)(orgimage->GetScalarPointer(i, j, k));
				q[i + j*column + k*row*column] = *p;
				if (*p >0)
				{

					
					if (direction == "X" || direction == "x")
					{
						index = i;
					}
					else if (direction == "Y" || direction == "y")
					{
						index = j;
					}
					else if (direction == "Z" || direction == "z")
					{
						index = k;
					}

					if (index == intOffset)
					{
						double*point = new double[5];
						point[0] = j;
						point[1] = i;
						point[2] = k;
						point[3] = 1;
						point[4] = *p;

						pointList.append(point);
						t++;
						std::cout << "t=" << t<< " p[" << i << "][" << j << "]" << "[" << k << "]" << *p << std::endl;
					}
				}



			}

		}

	}
	double* sliceDataDouble;
	QHash<QString, double> circleParamsHash;
	if (pointList.size() > 0)
	{
		sliceDataDouble = new double[pointList.size()]();
		for (int i = 0; i < pointList.size(); i++)
		{
			double* point = pointList.at(i);
			sliceDataDouble[i] = point[4];//�õ�CTֵ
		}

		circleParamsHash.insert("max", max(sliceDataDouble, pointList.size()));
		circleParamsHash.insert("min", min(sliceDataDouble, pointList.size()));
		circleParamsHash.insert("average", aver(sliceDataDouble, pointList.size()));
		circleParamsHash.insert("AOD", AOD(sliceDataDouble, pointList.size(), vtkSlicerCalculusLogic::s_uWater, vtkSlicerCalculusLogic::s_materialThick));
		circleParamsHash.insert("IOD", IOD(sliceDataDouble, pointList.size(), vtkSlicerCalculusLogic::s_uWater, vtkSlicerCalculusLogic::s_materialThick));
		if (sliceDataDouble!=0)
			delete[] sliceDataDouble;

	}

	if (pixel != 0)
		delete[] pixel;
	//�ͷ����е�
	for (int index = 0; index < pointList.size(); index++)
	{
		double* p = pointList.at(index);
		delete[] p;
	}
	

	return circleParamsHash;
}
