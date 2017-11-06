; Note that most routines in the Win16 world are "far pascal",
; AKA WINAPI/CALLBACK:
; - arguments are pushed on stack in order from leftmost to rightmost,
;   leftmost ending up at highest address, rightmost at lowest
; - callee removes its arguments from the stack when returning
; - return value is in (depending on its size):
;   - al
;   - ax
;   - dx:ax (high 16 bits in dx, low 16 bits in ax)
; - callee saves: bx, si, di, bp

; Call our non-imported routines and pass 16-bit arguments.
%macro CALLP 1-*
    %rep %0-1
        %rotate 1
        push    %1
    %endrep
    %rotate 1
    push    cs ; simulate far call
    call    %1
%endmacro

; Helper for pushing far addresses of things in the data segment,
; e.g.:
;   INVOKE  MessageBox, 0, DS_(text_buf), DS_(caption_buf), 1
%define DS_(OFS) ds, OFS

; Argument as 16-bit on-stack memory operand.
; For use with routines that are begun with the PROC macro.
;
; Rightmost argument is 0 (if 16-bit) or 0 and 1 (if 32-bit).
; For example, in "void foo(HINSTANCE, LPCSTR)" we'd have:
; - WARG(0) is the selector/segment part of the LPCSTR arg
; - WARG(1) is the offset part of the LPCSTR arg
; - WARG(2) is the instance arg
%define WARG(N) word [bp + 6 + (N)*2]

; Push multiple, e.g.:
;   PUSHM bx, si
%macro PUSHM 1-* 
    %rep %0 
        push    %1 
        %rotate 1 
    %endrep 
%endmacro

; Pop multiple, e.g.:
;   POPM bx, si
; if you previously pushed with
;   PUSHM bx, si
%macro POPM 1-* 
    %rep %0 
        %rotate -1 
        pop     %1 
    %endrep 
%endmacro

; Begins a new routine, e.g.:
;   PROC WinMain
%macro PROC 1
%1:
    push    bp
    mov     bp, sp
%endmacro

; Ends a routine previously begun with PROC,
; optionally removing a specifiend number of 16-bit
; arguments from the stack e.g.:
;   ENDPROC 5
%macro ENDPROC 0-1
    pop     bp
%if %0 == 1
    retf    (%1)*2
%else
    retf
%endif
%endmacro
