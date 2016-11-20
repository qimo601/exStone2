#include"excelExportHelper.h"
#include <ActiveQt/qaxobject.h>
#include <ActiveQt/qaxbase.h>
ExcelExportHelper::ExcelExportHelper(bool closeExcelOnExit)
{
	m_closeExcelOnExit = closeExcelOnExit;
	m_excelApplication = nullptr;
	m_sheet = nullptr;
	m_sheets = nullptr;
	m_workbook = nullptr;
	m_workbooks = nullptr;
	m_excelApplication = nullptr;
	//const QString application = "Excel.Application";//const 会导致载入DLL出错
	QString application = "Excel.Application";
	//Test failed
	m_excelApplication = new QAxObject(application);
	//if (m_excelApplication == nullptr)
	//	throw invalid_argument("Failed to initialize interop with Excel (probably Excel is not installed)");

	m_excelApplication->dynamicCall("SetVisible(bool)", false); // hide excel
	m_excelApplication->setProperty("DisplayAlerts", 1); // disable alerts

	m_workbooks = m_excelApplication->querySubObject("Workbooks");
	m_workbook = m_workbooks->querySubObject("Add");
	m_sheets = m_workbook->querySubObject("Worksheets");
	m_sheet = m_sheets->querySubObject("Add");
}

void ExcelExportHelper::SetCellValue(int lineIndex, int columnIndex, const QString& value)
{
	QAxObject *cell = m_sheet->querySubObject("Cells(int,int)", lineIndex, columnIndex);
	cell->setProperty("Value", value);
	delete cell;

}
ExcelExportHelper::~ExcelExportHelper()
{
	if (m_excelApplication != nullptr)
	{
		if (!m_closeExcelOnExit)
		{
			m_excelApplication->setProperty("DisplayAlerts", 1);
			//Test failed
			m_excelApplication->dynamicCall("SetVisible(bool)", true);
		}

		if (m_workbook != nullptr && m_closeExcelOnExit)
		{
			//Test failed
			m_workbook->dynamicCall("Close(Boolean)", true);
			m_excelApplication->dynamicCall("Quit (void)");
		}
	}

	delete m_sheet;
	delete m_sheets;
	delete m_workbook;
	delete m_workbooks;
	delete m_excelApplication;
}
//----------------------add end ---------------------