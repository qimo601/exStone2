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

// .NAME vtkSlicerCalculusLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerCalculusLogic_h
#define __vtkSlicerCalculusLogic_h
#include "common.h"
// Slicer includes
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerVolumesLogic.h"
#include "vtkSlicerCropVolumeLogic.h"
// MRML includes
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLSliceNode.h"

// STD includes
#include <cstdlib>

#include "vtkSlicerCalculusModuleLogicExport.h"

// VTK includes
#include <vtkImageReslice.h>

//----------------��ɺɺ���----------------------
// Slicer includes
#include "vtkMRMLMarkupsFiducialNode.h"
#include "vtkMRMLMarkupsNode.h"
#include "vtkSlicerMarkupsLogic.h"
#include <cstdlib>
#include "vtkSlicerCalculusModuleLogicExport.h"
//--------------��excel�йص�ͷ�ļ�
#include <ActiveQt/qaxobject.h>
#include <ActiveQt/qaxbase.h>
#include <Qtcore/qstring.h>
#include "vtkPoints.h"
class QLabel;
class QPushButton;
#include <vtkIOStream.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
//��tableWidget�йص���
#include <QTableWidget>  
#include <QTableWidgetItem>
#include <QDialog>
#include <QAction>
#include <QApplication>
//excel�й�ͷ�ļ�
#include <ActiveQt/qaxobject.h>
#include <ActiveQt/qaxbase.h>
#include <Qtcore/qstring.h>
using namespace std;
//----------------��ӽ���---------------------------------

/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_CALCULUS_MODULE_LOGIC_EXPORT vtkSlicerCalculusLogic :
  public vtkSlicerModuleLogic
{
public:


	static vtkSlicerCalculusLogic *New();
	vtkTypeMacro(vtkSlicerCalculusLogic, vtkSlicerModuleLogic);
	void PrintSelf(ostream& os, vtkIndent indent);
	//���õ�ǰ�����ݵ�logic
	void setVolumesLogic(vtkSlicerVolumesLogic* logic);
	vtkSlicerVolumesLogic* getVolumesLogic();

	void setCropVolumeLogic(vtkSlicerCropVolumeLogic* cropVolume);
	vtkSlicerCropVolumeLogic* getCropVolumeLogic();
	//��ȡ��ǰ��Ƭ����������
	QHash<QString, double> acqSliceData(vtkImageReslice* reslice,vtkMRMLSliceNode* sliceNode, vtkMRMLVolumeNode* volumeNode);
	  //-------��ɺɺ---
	  QHash<QString, double> aqcCircleData(vtkMRMLVolumeNode* input, vtkMRMLMarkupsFiducialNode* markups);
	  //��ȡ��ֱ�����slice����
	  QHash<QString, double> acqSliceVerticalData(vtkMRMLVolumeNode* input, double offset, QString direction);
	  //-------��ɺɺ--------

	  double max(double a[], int n);
	  double min(double a[], int n);
	  double aver(double a[], int n);
	  double AOD(double a[], int n, double m, double d);
	  double IOD(double a[], int n, double m, double d);

	  static double s_sliceThick;//����Ƕ���Ƭ���
	  static double s_uWater;//ˮ��˥��ϵ��
	  static double s_materialThick;//���Ϻ��
protected:
	vtkSlicerCalculusLogic();
	virtual ~vtkSlicerCalculusLogic();

	virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
	/// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
	virtual void RegisterNodes();
	virtual void UpdateFromMRMLScene();
	virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
	virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);
private:

	vtkSlicerCalculusLogic(const vtkSlicerCalculusLogic&); // Not implemented
	void operator=(const vtkSlicerCalculusLogic&); // Not implemented

	//�����ݵ�logic
	vtkSlicerVolumesLogic* volumesLogic;
	//���к��������
	vtkSlicerCropVolumeLogic* cropVolumeLogic;
};

#endif


