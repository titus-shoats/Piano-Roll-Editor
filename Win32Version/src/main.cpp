//
//  main.cpp
//  Win32 Piano Roll Editor
//
//  Main application entry point
//

#include "../include/PianoRollEditor.h"
#include <windows.h>
#include <iostream>

// Global variables
PianoRollEditor* g_editor = nullptr;
HWND g_hwnd = NULL;
int g_tickTest = 0;

// Timer ID
#define TIMER_PLAYBACK 1

// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_CREATE:
        {
            g_editor = new PianoRollEditor(hwnd);
            g_editor->setup(10, 900, 20);
            
            // Set up the send change callback
            g_editor->sendChange = [](int note, int velocity) {
                std::cout << "MIDI send: " << note << " : " << velocity << "\n";
            };
            
            // Start playback timer (20ms = 50fps)
            SetTimer(hwnd, TIMER_PLAYBACK, 20, NULL);
            return 0;
        }
        
        case WM_TIMER:
        {
            if (wParam == TIMER_PLAYBACK && g_editor) {
                g_tickTest += 20;
                g_editor->setPlaybackMarkerPosition(g_tickTest, true, false); // Don't invalidate entire window
                if (g_tickTest >= 480 * 4 * 10) {
                    g_tickTest = 0;
                }
            }
            return 0;
        }
        
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            if (g_editor) {
                g_editor->onPaint(hdc);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_SIZE:
        {
            if (g_editor) {
                g_editor->onSize(LOWORD(lParam), HIWORD(lParam));
            }
            return 0;
        }
        
        case WM_MOUSEMOVE:
        {
            if (g_editor) {
                g_editor->onMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam);
            }
            return 0;
        }
        
        case WM_LBUTTONDOWN:
        {
            if (g_editor) {
                g_editor->onMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam);
            }
            return 0;
        }
        
        case WM_LBUTTONDBLCLK:
        {
            if (g_editor) {
                g_editor->onMouseDoubleClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam);
            }
            return 0;
        }
        
        case WM_LBUTTONUP:
        {
            if (g_editor) {
                g_editor->onMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam);
            }
            return 0;
        }
        
        case WM_MOUSEWHEEL:
        {
            if (g_editor) {
                int delta = GET_WHEEL_DELTA_WPARAM(wParam);
                g_editor->onMouseWheel(delta);
            }
            return 0;
        }
        
        case WM_KEYDOWN:
        {
            if (g_editor) {
                g_editor->onKeyDown(wParam);
            }
            return 0;
        }
        
        case WM_HSCROLL:
        {
            if (g_editor) {
                g_editor->onHScroll(wParam, lParam);
            }
            return 0;
        }
        
        case WM_VSCROLL:
        {
            if (g_editor) {
                g_editor->onVScroll(wParam, lParam);
            }
            return 0;
        }
        
        case WM_COMMAND:
        {
            if (g_editor) {
                g_editor->onCommand(wParam, lParam);
            }
            return 0;
        }
        
        case WM_DESTROY:
        {
            KillTimer(hwnd, TIMER_PLAYBACK);
            delete g_editor;
            g_editor = nullptr;
            PostQuitMessage(0);
            return 0;
        }
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// WinMain entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Register window class
    const wchar_t CLASS_NAME[] = L"PianoRollEditorClass";
    
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.style = CS_DBLCLKS; // Enable double-click messages
    
    if (!RegisterClass(&wc)) {
        MessageBox(NULL, L"Window Registration Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    // Create window
    g_hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Win32 Piano Roll Editor",
        WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        NULL,
        NULL,
        hInstance,
        NULL
    );
    
    if (g_hwnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    ShowWindow(g_hwnd, nCmdShow);
    UpdateWindow(g_hwnd);
    
    // Message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (int)msg.wParam;
}
