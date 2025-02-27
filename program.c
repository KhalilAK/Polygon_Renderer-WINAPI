#include <windows.h>
#include <stdio.h>
#include <math.h>

#define PI 3.1415926535
#define MAXPOlYGONS 5

typedef struct{

    int x, y;
    int radius;
    int sides;
    COLORREF color;
    int isMoving;
} MyPolygon;


MyPolygon polygons[MAXPOlYGONS];
int polygonCount = 0;

HWND addPolygonButton;
HWND deletePolygonButton;

//controls
HWND hEditX, hEditY, hEditRadius, hEditColor, hEditSides;
HWND hConfirmButton;
HWND showX, showY, showRadius, showColor, showSides;
int showControls = 0;

POINT mousePoint;
int mouseX = 0, mouseY = 0;
int movePolygon = 0;




LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void SavePolygons(){
    FILE* file = fopen("polygons.dat", "wb");
    if(file){
        fwrite(polygons, sizeof(MyPolygon), polygonCount, file);
        fclose(file);
    }
}
void LoadPolygons(){
    FILE* file = fopen("polygons.dat", "rb");
    if(file){
        polygonCount = fread(polygons, sizeof(MyPolygon), MAXPOlYGONS, file);
        fclose(file);
    }
}

int main(){
    LoadPolygons();
    const char className[] = "Rendering window";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = className;
    wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        className,
        "Simple Shapes",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        800,
        800,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    if(hwnd == NULL){
        return 1;
    }

    addPolygonButton = CreateWindowEx(
        0, "BUTTON", "Add Polygon", WS_VISIBLE | WS_CHILD,
        10, 10, 120, 40,
        hwnd, (HMENU)1, GetModuleHandle(NULL), NULL
    );

    deletePolygonButton = CreateWindowEx(
        0, "BUTTON", "Delete Polygon", WS_VISIBLE | WS_CHILD,
        10, 60, 120, 40, hwnd, (HMENU)2, GetModuleHandle(NULL), NULL
    );

    ShowWindow(hwnd, SW_SHOWDEFAULT);

    MSG msg = {};
    while(GetMessage(&msg, NULL, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;

}

void DrawPolygon(HDC hdc, MyPolygon* poly){
    if(poly->sides < 3){
        return;
    }

    POINT points[20];
    double angleStep = 2 * PI / poly->sides;
    double angleOffset = -PI / 2;

    for(int i = 0; i < poly->sides; i++){
        points[i].x = poly->x + poly->radius * cos(i * angleStep + angleOffset);
        points[i].y = poly->y + poly->radius * sin(i * angleStep + angleOffset);
    }

    HBRUSH hBrush = CreateSolidBrush(poly->color);
    HBRUSH holdBrush = (HBRUSH)SelectObject(hdc, hBrush);

    Polygon(hdc, points, poly->sides);

    SelectObject(hdc, holdBrush);
    DeleteObject(hBrush);
}


void AddPolygon(HWND hwnd, int x, int y, int radius, int sides, COLORREF color){
    if(polygonCount < MAXPOlYGONS){
        polygons[polygonCount] = (MyPolygon){
            x, y,
            radius, sides,
            color,
            0
        };
        polygonCount++;
        InvalidateRect(hwnd, NULL, TRUE);
    }
}

void DeletePolygon(HWND hwnd){
    if(polygonCount > 0){
        polygonCount--;
        InvalidateRect(hwnd, NULL, TRUE);
    }
}

void DestroyPreviousControls(){
    DestroyWindow(hEditX);
    DestroyWindow(hEditY);
    DestroyWindow(hEditRadius);
    DestroyWindow(hEditColor);
    DestroyWindow(hConfirmButton);
    DestroyWindow(showX);
    DestroyWindow(showY);
    DestroyWindow(showRadius);
    DestroyWindow(showColor);
    DestroyWindow(hEditSides);
    DestroyWindow(showSides);
}

char* ConvertNumberToString(int num){
    char* str = (char*)malloc(20);
    if(str){
        sprintf(str, "%d", num);

    }
    return str;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg){
        case WM_COMMAND:
            switch(LOWORD(wParam)){
                case 1:
                // add polygon
                    if(polygonCount < MAXPOlYGONS){
                        showControls = 1;
                    }
                    printf("Show controls\n");
                    if(showControls){
                        DestroyPreviousControls();
                        float x = rand() % 700 + 50;
                        int y = rand() % 700 + 50;
                        char* str = ConvertNumberToString(x);

                        showX = CreateWindow("STATIC", "X:", WS_VISIBLE | WS_CHILD, 150, 10, 20, 20, hwnd, NULL, NULL, NULL);
                        hEditX = CreateWindow("EDIT", str, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 180, 10, 50, 20, hwnd, NULL, NULL, NULL);

                        str = ConvertNumberToString(y);
                        showY = CreateWindow("STATIC", "Y:", WS_VISIBLE | WS_CHILD, 240, 10, 20, 20, hwnd, NULL, NULL, NULL);
                        hEditY = CreateWindow("EDIT", str, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 270, 10, 50, 20, hwnd, NULL, NULL, NULL);


                        showRadius = CreateWindow("STATIC", "Radius:", WS_VISIBLE | WS_CHILD, 330, 10, 50, 20, hwnd, NULL, NULL, NULL);
                        hEditRadius = CreateWindow("EDIT", "50", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 390, 10, 50, 20, hwnd, NULL, NULL, NULL);

                        showColor = CreateWindow("STATIC", "Color:", WS_VISIBLE | WS_CHILD, 450, 10, 50, 20, hwnd, NULL, NULL, NULL);
                        hEditColor = CreateWindow("EDIT", "00FFA6", WS_VISIBLE | WS_CHILD | WS_BORDER, 500, 10, 80, 20, hwnd, NULL, NULL, NULL);


                        showSides = CreateWindow("STATIC", "Sides:", WS_VISIBLE | WS_CHILD, 150, 50, 50, 20, hwnd, NULL, NULL, NULL);
                        hEditSides = CreateWindow("EDIT", "3", WS_VISIBLE | WS_CHILD | WS_BORDER, 200, 50, 20, 20, hwnd, NULL, NULL, NULL);

                        hConfirmButton = CreateWindow("BUTTON", "Confirm", WS_VISIBLE | WS_CHILD, 600, 10, 80, 30, hwnd, (HMENU)3, NULL, NULL);

                        free(str);
                    }
                    break;
                case 2:
                // delete polygon
                    DeletePolygon(hwnd);
                    break;
                case 3:
                    {
                        char buffer[10];
                        int x, y, radius, sides;
                        COLORREF color = RGB(0, 128, 255);  // Default color

                        GetWindowText(hEditX, buffer, 10);
                        x = atoi(buffer);

                        GetWindowText(hEditY, buffer, 10);
                        y = atoi(buffer);

                        GetWindowText(hEditRadius, buffer, 10);
                        radius = atoi(buffer);

                        GetWindowText(hEditColor, buffer, 10);
                        if (strlen(buffer) > 0) {
                            sscanf(buffer, "%x", &color);  // convert hex
                            color = RGB((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
                        }

                        GetWindowText(hEditSides, buffer, 10);
                        sides = atoi(buffer);


                        if (polygonCount < MAXPOlYGONS) {
                            AddPolygon(hwnd, x, y, radius, sides, color);
                        }
                        showControls = 0;
                        DestroyPreviousControls();
                    }
                    break;
            }
            return 0;
        case WM_DESTROY:
            SavePolygons();
            PostQuitMessage(0);
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                FillRect(hdc, &ps.rcPaint, CreateSolidBrush(RGB(0, 0, 0)));

                for(int i = 0; i < polygonCount; i++){
                    DrawPolygon(hdc, &polygons[i]);
                }

                EndPaint(hwnd, &ps);
                return 0;
            }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}
