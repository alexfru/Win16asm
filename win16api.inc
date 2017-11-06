%define LOWORD(X) ((X) & 0xFFFF)
%define HIWORD(X) (((X) >> 16) & 0xFFFF)

struc WNDCLASS
    .style                      resw 1
    .lpfnWndProc                resd 1
    .cbClsExtra                 resw 1
    .cbWndExtra                 resw 1
    .hInstance                  resw 1
    .hIcon                      resw 1
    .hCursor                    resw 1
    .hbrBackground              resw 1
    .lpszMenuName               resd 1
    .lpszClassName              resd 1
endstruc

struc MSG
    .hwnd                       resw 1
    .message                    resw 1
    .wParam                     resw 1
    .lParam                     resd 1
    .time                       resd 1
    .pt                         resd 1
endstruc

struc RECT
    .left                       resw 1
    .top                        resw 1
    .right                      resw 1
    .bottom                     resw 1
endstruc

struc PAINTSTRUCT
    .hdc                        resw 1
    .fErase                     resw 1
    .rcPaint                    resw 4 ; struc RECT
    .fRestore                   resw 1
    .fIncUpdate                 resw 1
    .rgbReserved                resb 16
endstruc

CS_VREDRAW                      equ 0x0001
CS_HREDRAW                      equ 0x0002

COLOR_WINDOWFRAME               equ 6

IDC_ARROW                       equ 32512

WM_DESTROY                      equ 0x0002
WM_PAINT                        equ 0x000F

WS_OVERLAPPED                   equ 0x00000000
WS_MAXIMIZEBOX                  equ 0x00010000
WS_MINIMIZEBOX                  equ 0x00020000
WS_THICKFRAME                   equ 0x00040000
WS_SYSMENU                      equ 0x00080000
WS_CAPTION                      equ 0x00C00000
WS_OVERLAPPEDWINDOW             equ WS_OVERLAPPED + WS_CAPTION + WS_SYSMENU +\
                                    WS_THICKFRAME + WS_MINIMIZEBOX +\
                                    WS_MAXIMIZEBOX

CW_USEDEFAULT                   equ 0x8000

DT_CENTER                       equ 0x01
DT_VCENTER                      equ 0x04
DT_SINGLELINE                   equ 0x20


;KERNEL ordinals
ORD_KERNEL_WaitEvent            equ  30
ORD_KERNEL_InitTask             equ  91

;USER ordinals
ORD_USER_MessageBox             equ   1
ORD_USER_InitApp                equ   5
ORD_USER_PostQuitMessage        equ   6
ORD_USER_GetClientRect          equ  33
ORD_USER_BeginPaint             equ  39
ORD_USER_EndPaint               equ  40
ORD_USER_CreateWindow           equ  41
ORD_USER_ShowWindow             equ  42
ORD_USER_RegisterClass          equ  57
ORD_USER_DrawText               equ  85
ORD_USER_DefWindowProc          equ 107
ORD_USER_GetMessage             equ 108
ORD_USER_TranslateMessage       equ 113
ORD_USER_DispatchMessage        equ 114
ORD_USER_UpdateWindow           equ 124
ORD_USER_LoadCursor             equ 173
