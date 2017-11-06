; File: hw.asm
; Compile: nasm hw.asm -o hw.exe
%include "ne.inc"
%include "win16api.inc"
%include "helpers.inc"

NE_START

IMPORT
    LIB KERNEL
        BYORD   WaitEvent ; needed by NE_STARTUP
        BYORD   InitTask ; needed by NE_STARTUP
    LIB USER
        BYORD   InitApp ; needed by NE_STARTUP
        BYORD   MessageBox
        BYORD   PostQuitMessage
        BYORD   GetClientRect
        BYORD   BeginPaint
        BYORD   EndPaint
        BYORD   CreateWindow
        BYORD   ShowWindow
        BYORD   RegisterClass
        BYORD   DrawText
        BYORD   DefWindowProc
        BYORD   GetMessage
        BYORD   TranslateMessage
        BYORD   DispatchMessage
        BYORD   UpdateWindow
        BYORD   LoadCursor


section .text
__start:
    NE_STARTUP ; calls WinMain()


; LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
PROC WndProc
    ; switch (message)
    mov     ax, WARG(3)

    cmp     ax, WM_PAINT
    jne     .not_paint
    ; case WM_PAINT:
    ; HDC BeginPaint(HWND, PAINTSTRUCT FAR*);
    INVOKE  BeginPaint, WARG(4), DS_(paint_struct)
    ; void GetClientRect(HWND, RECT FAR*);
    INVOKE  GetClientRect, WARG(4), DS_(paint_struct + PAINTSTRUCT.rcPaint)
    ; int DrawText(HDC, LPCSTR, int, RECT FAR*, UINT);
    INVOKE  DrawText, word [paint_struct + PAINTSTRUCT.hdc],\
                      DS_(wnd_text),\
                      -1,\
                      DS_(paint_struct + PAINTSTRUCT.rcPaint),\
                      DT_CENTER | DT_VCENTER | DT_SINGLELINE
    ; void EndPaint(HWND, const PAINTSTRUCT FAR*);
    INVOKE  EndPaint, WARG(4), DS_(paint_struct)
    xor     ax, ax
    cwd
    jmp     .exit
.not_paint:

    cmp     ax, WM_DESTROY
    jne     .not_destroy
    ; case WM_DESTROY:
    ; void PostQuitMessage(int);
    INVOKE  PostQuitMessage, 0
    xor     ax, ax
    cwd
    jmp     .exit
.not_destroy:

    ; default:
    ; LRESULT DefWindowProc(hWnd, message, wParam, lParam);
    INVOKE  DefWindowProc, WARG(4),\
                           WARG(3),\
                           WARG(2),\
                           WARG(1), WARG(0)
.exit:
ENDPROC 5


; int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdline, UINT nCmdShow);
PROC WinMain
    ; int WINAPI MessageBox(HWND, LPCSTR, LPCSTR, UINT);
    CALLP   Dword2Hex, hexbuf, 0x0123, 0xcdef
    INVOKE  MessageBox, 0, DS_(hexbuf), DS_(caption_hex), 1

    cmp     WARG(3), 0 ; hPrevInstance == 0?
    jne     .gotPrevInstance

    mov     ax, WARG(4)
    mov     [wnd_class + WNDCLASS.hInstance], ax

    mov     [wnd_class + WNDCLASS.lpfnWndProc + 2], cs
    mov     [wnd_class + WNDCLASS.lpszClassName + 2], ds

    ; HCURSOR LoadCursor(HINSTANCE, LPCSTR);
    INVOKE  LoadCursor, 0, 0, IDC_ARROW
    mov     [wnd_class + WNDCLASS.hCursor], ax

    ; ATOM RegisterClass(const WNDCLASS FAR*);
    INVOKE  RegisterClass, DS_(wnd_class)
    or      ax, ax
    jz      .exit

.gotPrevInstance:

    ; HWND CreateWindow(LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, void FAR*);
    INVOKE  CreateWindow, DS_(wnd_class_name),\
                          DS_(wnd_caption),\
                          HIWORD(WS_OVERLAPPEDWINDOW), LOWORD(WS_OVERLAPPEDWINDOW),\
                          CW_USEDEFAULT,\
                          CW_USEDEFAULT,\
                          CW_USEDEFAULT,\
                          CW_USEDEFAULT,\
                          0,\
                          0,\
                          WARG(4),\
                          0, 0
    mov     [hwnd], ax
    or      ax, ax
    jz      .exit

    ; BOOL ShowWindow(HWND, int);
    INVOKE  ShowWindow, word [hwnd], WARG(0)
    ; void UpdateWindow(HWND);
    INVOKE  UpdateWindow, word [hwnd]

.lp:
    ; BOOL GetMessage(MSG FAR*, HWND, UINT, UINT);
    INVOKE  GetMessage, DS_(wnd_msg), 0, 0, 0
    or      ax, ax
    jz      .brk

    ; BOOL TranslateMessage(const MSG FAR*);
    INVOKE  TranslateMessage, DS_(wnd_msg)
    ; LONG DispatchMessage(const MSG FAR*);
    INVOKE  DispatchMessage, DS_(wnd_msg)
    jmp     .lp

.brk:
    mov     ax, [wnd_msg + MSG.wParam]

.exit:
ENDPROC 5


; void Dword2Hex(char*, DWORD);
PROC Dword2Hex
    PUSHM   bx, di

    mov     di, WARG(2) ; char*
    mov     bx, hextab

    mov     dx, WARG(1) ; DWORD's high 16 bits
    mov     cx, 4
.1:
    rol     dx, 4
    mov     al, dl
    and     al, 15
    xlat
    mov     [di], al
    inc     di
    loop    .1

    mov     dx, WARG(0) ; DWORD's low 16 bits
    mov     cx, 4
.2:
    rol     dx, 4
    mov     al, dl
    and     al, 15
    xlat
    mov     [di], al
    inc     di
    loop    .2

    POPM    bx, di
ENDPROC 3


section .data
hextab:
    db "0123456789abcdef"

hexbuf:
    db "12345678", 0

caption_hex:
    db "Hex number:", 0

wnd_class_name:
    db "WndClassName", 0

wnd_caption:
    db "New Executable (NE) for 16-bit Windows with NASM", 0

wnd_text:
    db "Hello, world!", 0

wnd_class:
istruc WNDCLASS
    at WNDCLASS.style,         dw CS_HREDRAW | CS_VREDRAW
    at WNDCLASS.lpfnWndProc,   dd WndProc
    at WNDCLASS.cbClsExtra,    dw 0
    at WNDCLASS.cbWndExtra,    dw 0
    at WNDCLASS.hInstance,     dw 0
    at WNDCLASS.hIcon,         dw 0
    at WNDCLASS.hCursor,       dw 0
    at WNDCLASS.hbrBackground, dw COLOR_WINDOWFRAME
    at WNDCLASS.lpszMenuName,  dd 0
    at WNDCLASS.lpszClassName, dd wnd_class_name
iend

wnd_msg:
istruc MSG
iend

paint_struct:
istruc PAINTSTRUCT
iend

hwnd:
    dw 0

NE_END
