#include "csMain.h"
#include "IupUIManager.h"
#include "plugin.h"

CLAW_CALLBACK(SearchWindowClosing);
CLAW_CALLBACK(FirstScan);
CLAW_CALLBACK(NextScan);
CLAW_CALLBACK(ScanValueTypeChanged);

int _claw_ResultClicked(Ihandle* handle, char* text, int item, int state) { _csMain->ResultClicked(text, item, state); return 0; }

csMain::csMain()
	: m_scanner(this)
{
	m_ui = new IupUIManager();
	m_hDialog = nullptr;

	m_hButtonFirstScan = nullptr;
	m_hButtonNextScan = nullptr;

	m_hCheckHex = nullptr;
	m_hTextInput = nullptr;

	m_hComboScanType = nullptr;
	m_hComboValueType = nullptr;

	m_hFrameScanOptions = nullptr;
	m_hFloatMethod = nullptr;
	m_hCheckFastScan = nullptr;
	m_hTextFastScanAlign = nullptr;
	m_hCheckPauseWhileScanning = nullptr;

	m_hListResults = nullptr;

	m_currentScan = 0;
}

csMain::~csMain()
{
	if (m_hDialog != nullptr) {
		Close();
	}
}

SearchValueMethod csMain::MethodForType(SearchValueType type)
{
	switch (type) {
	case SVT_Char:
	case SVT_Int16:
	case SVT_Int32:
	case SVT_Int64:
		return SVM_Integer;

	case SVT_Float:
	case SVT_Double:
		return SVM_Float;
	}

	return SVM_Unknown;
}

int csMain::SearchWindowClosing()
{
	m_hDialog = nullptr;
	return IUP_CLOSE;
}

void csMain::PerformScan(bool firstScan)
{
	m_ui->SetButtonActive("ScanFirst", false);
	m_ui->SetButtonActive("ScanNext", false);
	// ... (Rest der Logik anpassen)

	m_scanner.m_inputText = m_ui->GetInputValue("TextInput");
	m_scanner.m_inputIsHex = !strcmp(m_ui->GetInputValue("CheckHex"), "ON");
	m_scanner.m_pauseWhileScanning = DbgIsRunning() && !strcmp(m_ui->GetInputValue("CheckPause"), "ON");

	if (!strcmp(m_ui->GetInputValue("CheckFastScan"), "ON")) {
		sscanf(m_ui->GetInputValue("TextFastScanAlign"), "%d", &m_scanner.m_scanStep);

		if (m_scanner.m_scanStep < 1) {
			m_scanner.m_scanStep = 1;
		}
	}

	m_scanner.m_currentScanFloatTruncate = !strcmp(m_ui->GetInputValue("FloatMethod"), "trunc");
	m_scanner.m_currentScanFloatRound = !strcmp(m_ui->GetInputValue("FloatMethod"), "round");
	m_scanner.m_currentScanFloatRound2 = !strcmp(m_ui->GetInputValue("FloatMethod"), "round2");

	m_scanner.PerformScan(firstScan);

	// TODO: Replace list updates
	// m_ui->ClearList("Results");

	size_t numResults = m_scanner.m_results.len();

	for (auto& result : m_scanner.m_results) {
		uintptr_t pointer = result.m_base + result.m_offset;

		s2::string strLine = s2::strprintf("%p", pointer);

		//TODO: Can we up the performance on this?
		if (numResults < 20) {
			char moduleName[MAX_MODULE_SIZE];
			if (DbgGetModuleAt(pointer, moduleName)) {
				strLine += s2::strprintf(" (%s)", moduleName);
			}

			char label[MAX_LABEL_SIZE];
			if (DbgGetLabelAt(pointer, SEG_DEFAULT, label)) {
				strLine += s2::strprintf(" %s", label);
			}
		}

		IupSetAttribute(m_hListResults, "APPENDITEM", strLine);
	}

	IupSetAttribute(m_hListResults, "AUTOREDRAW", "YES");
	IupRedraw(m_hListResults, 1);

	m_ui->SetButtonActive("ScanFirst", true);
	m_ui->SetButtonActive("ScanNext", true);
	m_ui->SetButtonActive("ComboScanType", true);
}

int csMain::FirstScan()
{
	if (m_currentScan > 0) {
		// New scan
		m_currentScan = 0;

		IupSetAttribute(m_hListResults, "REMOVEITEM", "ALL");

		IupSetAttribute(m_hButtonFirstScan, "TITLE", "First Scan");

		IupSetAttribute(m_hComboValueType, "ACTIVE", "YES");
		IupSetAttribute(m_hButtonNextScan, "ACTIVE", "NO");

		SetScanTypeCombo(true);
		return 0;
	}

	// First scan
	m_currentScan = 1;

	IupSetAttribute(m_hButtonFirstScan, "TITLE", "New Scan");
	SetScanTypeCombo(false);

	PerformScan(true);

	IupSetAttribute(m_hComboValueType, "ACTIVE", "NO");
	IupSetAttribute(m_hButtonNextScan, "ACTIVE", "YES");

	return 0;
}

int csMain::NextScan()
{
	m_currentScan++;

	PerformScan(false);

	return 0;
}

void csMain::ResultClicked(char* text, int item, int state)
{
	int index = item - 1;

	if (index == -1 || state == 0) {
		return;
	}

	SearchResult &result = m_scanner.m_results[index];

	GuiDumpAt(result.m_base + result.m_offset);
}

int csMain::ScanValueTypeChanged()
{
	bool inputHidden = false;

	if (m_currentScan == 0) {
		m_scanner.m_initialScanType = (InitialScanType)IupGetInt(m_hComboScanType, "VALUE");
		IupSetAttribute(m_hCheckHex, "ACTIVE", "YES");
		IupSetAttribute(m_hTextInput, "ACTIVE", "YES");
	} else {
		m_scanner.m_currentScanType = (ScanType)IupGetInt(m_hComboScanType, "VALUE");
		if (m_scanner.m_currentScanType == ST_Changed || m_scanner.m_currentScanType == ST_Unchanged || m_scanner.m_currentScanType == ST_Increased || m_scanner.m_currentScanType == ST_Decreased) {
			IupSetAttribute(m_hCheckHex, "ACTIVE", "NO");
			IupSetAttribute(m_hTextInput, "ACTIVE", "NO");
			inputHidden = true;
		} else {
			IupSetAttribute(m_hCheckHex, "ACTIVE", "YES");
			IupSetAttribute(m_hTextInput, "ACTIVE", "YES");
		}
	}
	m_scanner.m_currentScanValueType = (SearchValueType)IupGetInt(m_hComboValueType, "VALUE");
	m_scanner.m_currentScanValueMethod = MethodForType(m_scanner.m_currentScanValueType);

	if (!inputHidden) {
		IupSetAttribute(m_hCheckHex, "ACTIVE", m_scanner.m_currentScanValueMethod == SVM_Integer ? "YES" : "NO");
	}
	IupSetAttribute(m_hFloatMethod, "ACTIVE", m_scanner.m_currentScanValueMethod == SVM_Float ? "YES" : "NO");

	return 0;
}

void csMain::SetScanTypeCombo(bool firstScan)
{
	IupSetAttribute(m_hComboScanType, "REMOVEITEM", "ALL");

	if (firstScan) {
		IupSetAttribute(m_hComboScanType, "1", "Exact value");
		IupSetAttribute(m_hComboScanType, "2", "Bigger than...");
		IupSetAttribute(m_hComboScanType, "3", "Smaller than...");
		IupSetInt(m_hComboScanType, "VALUE", (int)m_scanner.m_initialScanType);
		return;
	}

	IupSetAttribute(m_hComboScanType, "1", "Exact value");
	IupSetAttribute(m_hComboScanType, "2", "Changed value");
	IupSetAttribute(m_hComboScanType, "3", "Unchanged value");
	IupSetAttribute(m_hComboScanType, "4", "Bigger than...");
	IupSetAttribute(m_hComboScanType, "5", "Smaller than...");
	IupSetAttribute(m_hComboScanType, "6", "Increased value");
	IupSetAttribute(m_hComboScanType, "7", "Decreased value");
	IupSetInt(m_hComboScanType, "VALUE", (int)m_scanner.m_currentScanType);
}

void csMain::Open()
{
	if (m_hDialog != nullptr) {
		return;
	}

	m_ui->CreateButton("ScanFirst", "First Scan", nullptr); // Callback noch anzupassen
	m_ui->CreateButton("ScanNext", "Next Scan", nullptr);
	m_ui->CreateToggle("CheckHex", "Hex");
	m_ui->CreateText("TextInput");
	Ihandle* hInput = IupSetAttributes(IupHbox(m_hCheckHex, m_hTextInput, nullptr), "MARGIN=0x0, GAP=5");

	m_ui->CreateList("ComboScanType");
	m_ui->CreateList("ComboValueType");
	m_ui->CreateToggle("CheckFastScan", "Fast Scan");
	m_ui->CreateText("TextFastScanAlign");
	m_ui->CreateToggle("CheckPause", "Pause while scanning");
	m_ui->CreateList("Results");

	IupSetAttribute(m_hDialog, "TITLE", "ClawSearch");
	IupSetAttribute(m_hDialog, "SIZE", "500x200");

	IupSetAttribute(m_hDialog, "NATIVEPARENT", (char*)GuiGetWindowHandle());
	CLAW_SETCALLBACK(m_hDialog, "CLOSE_CB", SearchWindowClosing);
	IupShowXY(m_hDialog, IUP_CENTERPARENT, IUP_CENTERPARENT);
}

void csMain::Close()
{
	if (m_hDialog == nullptr) {
		return;
	}
	IupDestroy(m_hDialog);
	m_hDialog = nullptr;
}

csMain* _csMain = nullptr;

void OpenSearch()
{
	if (_csMain == nullptr) {
		_csMain = new csMain;
	}

	_csMain->Open();
}

void CloseSearch()
{
	if (_csMain == nullptr) {
		return;
	}

	_csMain->Close();
	delete _csMain;
}
