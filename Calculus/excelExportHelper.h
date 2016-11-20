#ifndef __excelExportHelper_h
#define __excelExportHelper_h
//excel
#include <ActiveQt/qaxobject.h>
#include <ActiveQt/qaxbase.h>
#include <QString>
//---------------by zhushanshan
class ExcelExportHelper
{
public:
	ExcelExportHelper(const ExcelExportHelper& other) = delete;
	ExcelExportHelper& operator=(const ExcelExportHelper& other) = delete;

	ExcelExportHelper(bool closeExcelOnExit = false);
	void SetCellValue(int lineIndex, int columnIndex, const QString& value);
	void SaveAs(const QString& fileName);

	~ExcelExportHelper();

private:
	QAxObject* m_excelApplication;
	QAxObject* m_workbooks;
	QAxObject* m_workbook;
	QAxObject* m_sheets;
	QAxObject* m_sheet;
	bool m_closeExcelOnExit;


};

#endif

//---------------add end---