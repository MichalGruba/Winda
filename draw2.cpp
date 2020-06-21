// draw.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "draw2.h"
#include <vector>
#include <queue>
#include <cstdio>
#include <string>

#define MAX_LOADSTRING 100
#define TMR_1 1

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
const int A = 630; //stala wykorzystywana przy tworzeniu przyciskow
INT value;
int up = 1; //zmienna pomocnicza do zegara otwierajacego drzwi
const int pA = 700;
const int pB = 550;
const int pC = 400;
const int pD = 250;
const int pE = 100;
const int p[5] = { 700, 550, 400, 250, 100 }; //pozycje poszczegolnych pieter
// buttons
HWND hwndButton;
int Wait=0;
// sent data
int idTimer;
int poziom = 700; //obecna pozycja windy
std::vector<int> wWindzie;
std::vector<int> NextCall;
std::queue<int> Pietro[5];
int Calle[5];
int Cele[5];
int Przystanek[5];
int Go = 700;

RECT drawArea1 = { 705, 10, 1195, 990 }; //obszar w windzie
RECT TabelaArea = { 1300, 0, 1600, 200 }; //obszar zarezerwowany dla tabeli z osobami i masami


// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Buttons(HWND, UINT, WPARAM, LPARAM);

void osPietro(HWND hWnd, HDC &hdc, PAINTSTRUCT &ps, int vIn, int nr) {  //Wyswietlanie liczby osob na pietrze
	hdc = BeginPaint(hWnd, &ps);
	float kX;
	float kY;
	int kX1, kY1; //okreslanie wspolrzednych do wyswietlenia osob na poszczegolnym pietrze
	if (nr == 0 || nr == 2 || nr == 4) {
		kX = 300.0f;
		kY = 650.0f - nr*150.0f;
		kX1 = 300;
		kY1 = 650 - nr * 150;
	}
	else {
		kX = 1300.0f;
		kY = 650.0f - nr * 150.0f;
		kX1 = 1300;
		kY1 = 650 - nr * 150;
	}
	RECT PietroArea = { kX1, kY1, kX1 + 300, kY1 + 200 }; //obszar przeznaczony na wyswietlanie osob na poszczegolnym pietrze(ktory bedzie odswiezany)
	InvalidateRect(hWnd, &PietroArea, TRUE);
	hdc = BeginPaint(hWnd, &ps);
	wchar_t vOut[12];
	_itow_s(vIn, vOut, sizeof(vOut) / 2, 10);
	wchar_t* osoby = (wchar_t*)vOut;
	Graphics graphics(hdc);
	FontFamily   fontFamily(L"Arial");
	Font         font(&fontFamily, 12, FontStyleBold, UnitPoint);
	RectF        rectF(kX, kY, 100.0f, 50.0f); //obszar przeznaczony na wyswietlanie osob na poszczegolnym pietrze
	SolidBrush   solidBrush(Color(255, 0, 0, 255));
	graphics.DrawString(osoby, -1, &font, rectF, NULL, &solidBrush);
	EndPaint(hWnd, &ps);
}
void updatePietro(HWND hWnd, HDC &hdc, PAINTSTRUCT &ps) { //Odswiezenie liczby osob na pietrach
	for (int i = 0; i < 5; i++) {
		osPietro(hWnd, hdc, ps, Pietro[i].size(), i);
	}
}
void Tabela(HWND hWnd, HDC &hdc, PAINTSTRUCT &ps, RECT *drawArea, int vIn) //Wyswietlenie tabeli z liczba pasazerow i ich masa
{
	InvalidateRect(hWnd, drawArea, TRUE);
	hdc = BeginPaint(hWnd, &ps);
	wchar_t vOut[12];
	wchar_t vOut2[12];
	_itow_s(vIn, vOut, sizeof(vOut) / 2, 10);
	wchar_t* osoby = (wchar_t*)vOut;
	int masa = vIn * 70;
	_itow_s(masa, vOut2, sizeof(vOut2) / 2, 10);
	wchar_t* Masa = (wchar_t*)vOut2;
	Graphics graphics(hdc);
	FontFamily   fontFamily(L"Arial");
	Font         font(&fontFamily, 12, FontStyleBold, UnitPoint);
	RectF        rectF(1500.0f, 20.0f, 100.0f, 50.0f);
	RectF        rectF2(1500.0f, 40.0f, 100.0f, 50.0f);
	RectF        rect1F(1300.0f, 20.0f, 200.0f, 50.0f);
	RectF        rect1F2(1300.0f, 40.0f, 200.0f, 50.0f);
	RectF        tbla(1300.0f, 20.0f, 240.0f, 50.0f);
	SolidBrush   solidBrush(Color(255, 0, 0, 255));
	graphics.DrawString(L"Liczba osób w windzie:", -1, &font, rect1F, NULL, &solidBrush);
	graphics.DrawString(osoby, -1, &font, rectF, NULL, &solidBrush);
	graphics.DrawString(L"Masa pasażerów:", -1, &font, rect1F2, NULL, &solidBrush);
	graphics.DrawString(Masa, -1, &font, rectF2, NULL, &solidBrush);

	Pen pen(Color(255, 0, 0, 0));
	graphics.DrawRectangle(&pen, tbla);
	EndPaint(hWnd, &ps);
}

void zapiszPolecenia() {
	//Calle
	for (int i = 0; i < NextCall.size(); i++) {
		Calle[NextCall[i]] = p[NextCall[i]];
	}
	//Cele
	for (int i = 0; i < wWindzie.size(); i++) {
		Cele[wWindzie[i]] = p[wWindzie[i]];
	}
}
void okreslPrzystanki() {
	for (int i = 0; i < 5; i++) {
		if (Calle[i] != 0) Przystanek[i] = Calle[i]; //jezeli jest jakies wezwanie zapisz je do przystankow
		else if (Cele[i] != 0) Przystanek[i] = Cele[i]; //jezeli jest jakis cel zapisz go do przystankow
		else Przystanek[i] = 0;
	}
}
void jedzDoCalla(HWND hWnd) {		//Wybiera następne wezwanie i do niego jedzie
	if (!NextCall.empty() && Calle[NextCall.front()] != 0) { //jezeli jest wezwanie i nie zostalo jeszcze wykonane
		Go = Calle[NextCall.front()]; //jedz do wezwania
		SetTimer(hWnd, idTimer = 39, 10, NULL);
	}
	else if (!NextCall.empty()) { //jezeli wezwanie juz wykonano
		NextCall.erase(NextCall.begin()); //usun wezwanie z "kolejki"(wektora)
		jedzDoCalla(hWnd); //sprawdz nastepny
	}
}
bool checkCalls() { //sprawdza czy są jakieś wezwania
	bool check = 0;
	for (int i = 0; i < 5; i++) {
		if (Calle[i] != 0) check = 1;
	}
	return check;
}
void Ruch(HWND hWnd) { //Tu winda zaczyna decydowac co zrobic
	if(!wWindzie.empty() || !NextCall.empty()) zapiszPolecenia(); //gdy w windzie ktos jest lub gdy jest jakies wezwanie
	okreslPrzystanki(); 
	if (wWindzie.empty()) { //jezeli winda jest pusta
		if (checkCalls()) jedzDoCalla(hWnd); //jezeli sa wezwania to do nich jedz
	}
	else {
		Go = Cele[wWindzie.front()]; //jezeli w windzie ktos jest zawiez go do celu
		SetTimer(hWnd, idTimer = 39, 10, NULL);
	}

}
void Jedz(HDC hdc, HWND hWnd, PAINTSTRUCT &ps, int a) {
	if (a == Go || Przystanek[0] == a || Przystanek[1] == a || Przystanek[2] == a || Przystanek[3] == a || Przystanek[4] == a) { //jezeli dotrzemy do celu lub przystanku po drodze
		KillTimer(hWnd, 31);
		for (int i = 0; i < 5; i++) {
			if (a == Przystanek[i]) {
				Wait = 1;
				for (int j = 0; j < wWindzie.size(); j++) //wysiadanie
					if (wWindzie[j] == i) {
						wWindzie.erase(wWindzie.begin() + j);
						j--;
					}
				Wait = 0;
				while (wWindzie.size() < 8) {			//wsiadanie
					if (!Pietro[i].empty()) {
						wWindzie.push_back(Pietro[i].front());
						Pietro[i].pop();
					}
					else {
						Ruch(hWnd);
						break;
					}
				}
				if (Pietro[i].empty()) { //zapisz ze pietro zostalo obsluzone
					Przystanek[i] = 0;
					Calle[i] = 0;
					Cele[i] = 0;
					for (int k = 0; k < NextCall.size(); k++) { //usuniecie z "kolejki" (wektora) wezwan na pietra ktore zostaly obsluzone
						if (!NextCall.empty() && NextCall[k] == i) {
							NextCall.erase(NextCall.begin() + k);
							k--;
						}
						else if (NextCall.empty()) break;
					}
				}
			}
		}
		if (a == Go && wWindzie.empty()) { //jezeli jestesmy u celu i winda jest pusta
			Wait = 1;
		}
		poziom = a;
		SetTimer(hWnd, idTimer = 1337, 10, NULL); //otwarcie drzwi
		value = 0;
		Tabela(hWnd, hdc, ps, &TabelaArea, wWindzie.size());	//wyswietlenie tabeli z osobami i masami
		for (int i = 0; i < 5; i++) {							//wyswietlenie liczby osob na pietrach
			osPietro(hWnd, hdc, ps, Pietro[i].size(), i);
		}

	}
}
void MyOnPaint(HDC hdc, HWND hWnd) //Wyswietlenie grafiki na starcie
{
	PAINTSTRUCT ps;
	Graphics graphics(hdc);
	Pen pen(Color(255, 0, 0, 0), 10);
	Pen pen2(Color(255, 0, 0, 0), 5);
	graphics.DrawRectangle(&pen, 700, 5, 500, 990); 
	graphics.DrawRectangle(&pen2, 702, poziom, 495, 200);
	Tabela(hWnd, hdc, ps, &TabelaArea, wWindzie.size());
	updatePietro(hWnd, hdc, ps);
	for (int i = 0; i < 3; i++) {
		graphics.DrawLine(&pen2, 0, 900 - i * 300, 700, 900 - i * 300);
	}
	for (int i = 0; i < 2; i++) {
		graphics.DrawLine(&pen2, 1200, 750 - i * 300, 1920, 750 - i * 300);
	}

}
void Winda(HDC hdc, HWND hWnd) { //Wykonywanie polecen przez winde
	Graphics graphics(hdc);
	PAINTSTRUCT ps;
	Pen pen2(Color(255, 0, 0, 0), 5);
	if (Go < poziom) { //jezeli cel jest wyzej
		graphics.DrawRectangle(&pen2, 702, poziom - value, 495, 200); //podnoszenie windy
		int a = poziom - value;
		Jedz(hdc, hWnd, ps, a);
	}
	else if(Go > poziom) { //jezeli cel jest nizej
		graphics.DrawRectangle(&pen2, 702, poziom + value, 495, 200); //obnizanie windy
		int a = poziom + value;
		Jedz(hdc, hWnd, ps, a);
	}
	else if (poziom == Go) { //jezeli jestesmy u celu(winda w bezruchu)
		Wait = 1;
		KillTimer(hWnd, 31);
		for (int i = 0; i < 5; i++) {
			if (poziom == Przystanek[i]) { //jezeli na pietrze ktos jest
				Wait = 1;
				for (int j = 0; j < wWindzie.size(); j++) //wysiadanie
					if (wWindzie[j] == i) {
						wWindzie.erase(wWindzie.begin() + j);
						j--;
					}
				Wait = 0;
				while (wWindzie.size() < 8) { //wsiadanie
					if (!Pietro[i].empty()) {
						wWindzie.push_back(Pietro[i].front());
						Pietro[i].pop();
					}
					else {
						Ruch(hWnd);
						break;
					}
				}
				if (Pietro[i].empty()) { //zapisz ze pietro zostalo obsluzone
					Przystanek[i] = 0;
					Calle[i] = 0;
					Cele[i] = 0;
					for (int k = 0; k < NextCall.size(); k++) { //usuniecie z "kolejki" (wektora) wezwan na pietra ktore zostaly obsluzone
						if (!NextCall.empty() && NextCall[k] == i) {
							NextCall.erase(NextCall.begin() + k);
							k--;
						}
						else if (NextCall.empty()) break;
					}
				}
			}
		}
		if (poziom == Go && wWindzie.empty()) { //jezeli jestesmy u celu i winda jest pusta
			Wait = 1;
		}
		Pen pen2(Color(255, 0, 0, 0), 5);
		graphics.DrawRectangle(&pen2, 702, poziom, 495, 200); //bez tego winda znikala w niektorych sytuacjach
		SetTimer(hWnd, idTimer = 1337, 10, NULL); //otwarcie drzwi
		value = 0;
		Tabela(hWnd, hdc, ps, &TabelaArea, wWindzie.size()); //wyswietlenie tabeli z osobami i masami
		for (int i = 0; i < 5; i++) {						//wyswietlenie liczby osob na pietrach
			osPietro(hWnd, hdc, ps, Pietro[i].size(), i);
		}

	}
}
void Doors(HWND hWnd, HDC &hdc, PAINTSTRUCT &ps, RECT *drawArea, int R) //Otwieranie i zamykanie drzwi
{
	InvalidateRect(hWnd, drawArea, TRUE);
	hdc = BeginPaint(hWnd, &ps);
	Graphics graphics(hdc);
	Pen pen2(Color(R, 0, 0, 0), 10);
	if (poziom == pA || poziom == pC || poziom == pE) graphics.DrawLine(&pen2, 700, poziom+3, 700, 1000); //jezeli pietro jest po lewej stronie
	else graphics.DrawLine(&pen2, 1200, poziom +198, 1200, poziom +3); //jezeli pietro jest po prawej stronie
	EndPaint(hWnd, &ps);
}
void repaintWindow(HWND hWnd, HDC &hdc, PAINTSTRUCT &ps, RECT *drawArea) //odswiezanie ekranu
{
	if (drawArea==NULL)
		InvalidateRect(hWnd, NULL, TRUE); // repaint all
		
	else {
			InvalidateRect(hWnd, drawArea, TRUE); //repaint drawArea
			hdc = BeginPaint(hWnd, &ps);
			 Winda(hdc, hWnd);
			 Winda(hdc, hWnd);
			 Winda(hdc, hWnd);
			EndPaint(hWnd, &ps);
	}

}



int OnCreate(HWND window)
{
	return 0;
}


// main function (exe hInstance)
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR    lpCmdLine, int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	value = 0;

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	


	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DRAW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);



	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{

		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DRAW));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	GdiplusShutdown(gdiplusToken);

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DRAW));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_DRAW);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;


	hInst = hInstance; // Store instance handle (of exe) in our global variable

	// main window
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	// create button and store the handle                                                       
	
	//hwndButton = CreateWindow(TEXT("button"),                      // The class name required is button
	//	TEXT("Draw"),                  // the caption of the button
	//	WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // the styles
	//	300, 60,                                  // the left and top co-ordinates
	//	80, 50,                              // width and height
	//	hWnd,                                 // parent window handle
	//	(HMENU)ID_BUTTON1,                   // the ID of your button
	//	hInstance,                            // the instance of your application
	//	NULL);                               // extra bits you dont really need

	//Tworzenie przyciskow
	for (int i = 0; i <= 4; i++) {
		int j = 5 * i;
		if (i % 2 == 0 && i != 0) hwndButton = CreateWindow(TEXT("button"), TEXT("G"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, A + 200 - i * 160, 50, 50, hWnd, (HMENU)j, hInstance, NULL);
		else if(i != 0) hwndButton = CreateWindow(TEXT("button"), TEXT("G"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 1850, A + 200 - i * 160, 50, 50, hWnd, (HMENU)j, hInstance, NULL);
	}
	for (int i = 0; i <= 4; i++) {
		int j = 5 * i +1;
		if(i%2 == 0) hwndButton = CreateWindow(TEXT("button"), TEXT("1"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, A + 150-i*160, 50, 50, hWnd, (HMENU)j, hInstance, NULL);
		else if(i != 1) hwndButton = CreateWindow(TEXT("button"), TEXT("1"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 1850, A + 150 - i * 160, 50, 50, hWnd, (HMENU)j, hInstance, NULL);
	}
	for (int i = 0; i <= 4; i++) {
		int j = 5 * i +2;
		if (i % 2 == 0 && i != 2) hwndButton = CreateWindow(TEXT("button"), TEXT("2"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, A + 100 - i * 160, 50, 50, hWnd, (HMENU)j, hInstance, NULL);
		else if (i != 2) hwndButton = CreateWindow(TEXT("button"), TEXT("2"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 1850, A + 100 - i * 160, 50, 50, hWnd, (HMENU)j, hInstance, NULL);
	}
	for (int i = 0; i <= 4; i++) {
		int j = 5 * i +3;
		if (i % 2 == 0 && i != 3) hwndButton = CreateWindow(TEXT("button"), TEXT("3"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, A + 50 - i * 160, 50, 50, hWnd, (HMENU)j, hInstance, NULL);
		else if (i != 3) hwndButton = CreateWindow(TEXT("button"), TEXT("3"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 1850, A + 50 - i * 160, 50, 50, hWnd, (HMENU)j, hInstance, NULL);
	}
	for (int i = 0; i <= 4; i++) {
		int j = 5 * i +4;
		if (i % 2 == 0 && i != 4) hwndButton = CreateWindow(TEXT("button"), TEXT("4"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, A - i * 160, 50, 50, hWnd, (HMENU)j, hInstance, NULL);
		else if (i != 4) hwndButton = CreateWindow(TEXT("button"), TEXT("4"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 1850, A - i * 160, 50, 50, hWnd, (HMENU)j, hInstance, NULL);
	}
	SetTimer(hWnd, idTimer = 637, 100, NULL); //zegar wykonujacy funkcje Ruch() co takt
	OnCreate(hWnd);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;

}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window (low priority)
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		// MENU & BUTTON messages
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case 5:
			Pietro[1].push(0);				//dodawanie osoby na pietrze Pietro[numer pietra].push(cel podrozy)
			NextCall.emplace_back(1);		//wezwanie windy NextCall.emplace_back(numer pietra)
			Wait = 0;						//w przypadku gdy winda stoi bezczynnie daje sygnał na rozruch
			updatePietro(hWnd, hdc, ps);	//odswiez informacje o osobach na pietrach
			break;
		case 10:
			Pietro[2].push(0);
			NextCall.emplace_back(2);
			Wait = 0;
			updatePietro(hWnd, hdc, ps);
			break;
		case 15:
			Pietro[3].push(0);
			NextCall.emplace_back(3);
			Wait = 0;
			updatePietro(hWnd, hdc, ps);
			break;
		case 20:
			Pietro[4].push(0);
			NextCall.emplace_back(4);
			Wait = 0;
			updatePietro(hWnd, hdc, ps);
			break;
		case 1:
			Pietro[0].push(1);
			NextCall.emplace_back(0);
			Wait = 0;
			updatePietro(hWnd, hdc, ps);
			break;
		case 11:
			Pietro[2].push(1);
			NextCall.emplace_back(2);
			Wait = 0;
			updatePietro(hWnd, hdc, ps);
			break;
		case 16:
			Pietro[3].push(1);
			NextCall.emplace_back(3);
			Wait = 0;
			updatePietro(hWnd, hdc, ps);
			break;
		case 21:
			Pietro[4].push(1);
			NextCall.emplace_back(4);
			Wait = 0;
			updatePietro(hWnd, hdc, ps);
			break;
		case 2:
			Pietro[0].push(2);
			NextCall.emplace_back(0);
			Wait = 0;
			updatePietro(hWnd, hdc, ps);
			break;
		case 7:
			Pietro[1].push(2);
			NextCall.emplace_back(1);
			Wait = 0;
			updatePietro(hWnd, hdc, ps);
			break;
		case 17:
			Pietro[3].push(2);
			NextCall.emplace_back(3);
			Wait = 0;
			updatePietro(hWnd, hdc, ps);
			break;
		case 22:
			Pietro[4].push(2);
			NextCall.emplace_back(4);
			Wait = 0;
			updatePietro(hWnd, hdc, ps);
			break;
		case 3:
			Pietro[0].push(3);
			NextCall.emplace_back(0);
			Wait = 0;
			updatePietro(hWnd, hdc, ps);
			break;
		case 8:
			Pietro[1].push(3);
			NextCall.emplace_back(1);
			Wait = 0;
			updatePietro(hWnd, hdc, ps);
			break;
		case 13:
			Pietro[2].push(3);
			NextCall.emplace_back(2);
			Wait = 0;
			updatePietro(hWnd, hdc, ps);
			break;
		case 23:
			Pietro[4].push(3);
			NextCall.emplace_back(4);
			Wait = 0;
			updatePietro(hWnd, hdc, ps);
			break;
		case 4:
			Pietro[0].push(4);
			NextCall.emplace_back(0);
			Wait = 0;
			updatePietro(hWnd, hdc, ps);
			break;
		case 9:
			Pietro[1].push(4);
			NextCall.emplace_back(1);
			Wait = 0;
			updatePietro(hWnd, hdc, ps);
			break;
		case 14:
			Pietro[2].push(4);
			NextCall.emplace_back(2);
			Wait = 0;
			updatePietro(hWnd, hdc, ps);
			break;
		case 19:
			Pietro[3].push(4);
			NextCall.emplace_back(3);
			Wait = 0;
			updatePietro(hWnd, hdc, ps);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here (not depend on timer, buttons)
		MyOnPaint(hdc, hWnd);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		break;
	case WM_TIMER:
		switch (wParam)
		{
		case 637:	//zegar wykonujacy funkcje Ruch
			Ruch(hWnd);
			break;
		case 31:	//zegar odswiezajacy obraz w Windzie
			//force window to repaint
			repaintWindow(hWnd, hdc, ps, &drawArea1);
			value++;
			break;
		case 39:	//zegar majacy na celu uruchamianie zegara 31 gdy winda nie jest w spoczynku
			if(Wait != 1) 
			SetTimer(hWnd, idTimer = 31, 10, NULL);
			break;
		case 1337: //zegar otwierajacy drzwi
			KillTimer(hWnd, 39);
			//trzyma otwarte drzwi
			if (poziom == 700 || poziom == 400 || poziom == 100) { //drzwi po lewej stronie
				RECT drawArea2 = { 695, poziom + 198, 705, poziom+3 };
				Doors(hWnd, hdc, ps, &drawArea2, 0);
			}
			else if (poziom == 550 || poziom == 250) { //drzwi po prawej stronie
				RECT drawArea2 = { 1195, poziom + 198, 1205, poziom+3 };
				Doors(hWnd, hdc, ps, &drawArea2, 0);
			}
			up++;
			if (up == 100) { //po 100 taktach zegara zamyka drzwi
				KillTimer(hWnd, 1337);
				if (poziom == 700 || poziom == 400 || poziom == 100) { //drzwi po lewej stronie
					RECT drawArea2 = { 695, poziom + 198, 705, poziom+3 };
					Doors(hWnd, hdc, ps, &drawArea2, 255);
				}
				else if (poziom == 550 || poziom == 250) { //drzwi po prawej stronie
					RECT drawArea2 = { 1195, poziom + 198, 1205, poziom+3 };
					Doors(hWnd, hdc, ps, &drawArea2, 255);
				}
				SetTimer(hWnd, idTimer = 39, 1000, NULL);
				up = 0;
				break;
			}
			break;
		}



	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
