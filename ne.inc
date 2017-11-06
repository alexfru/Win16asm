struc __MZHDR
    .Signature              resw 1
    .PartPage               resw 1
    .PageCnt                resw 1
    .ReloCnt                resw 1
    .HdrSize                resw 1
    .MinAlloc               resw 1
    .MaxAlloc               resw 1
    .InitSs                 resw 1
    .InitSp                 resw 1
    .ChkSum                 resw 1
    .InitIp                 resw 1
    .InitCs                 resw 1
    .ReloOff                resw 1
    .OverlayNo              resw 1
    .Reserved               resw 16
    .NeHdrOfs               resd 1
endstruc

struc __NEHDR
    .Signature              resw 1
    .LinkerVer              resb 1
    .LinkerRev              resb 1
    .EntryTabOfs            resw 1
    .EntryTabSize           resw 1
    .ChkSum                 resd 1
    .Flags                  resw 1
    .AutoDataSegNo          resw 1
    .HeapInitSize           resw 1
    .StackSize              resw 1
    .InitIp                 resw 1
    .InitCsSegNo            resw 1
    .InitSp                 resw 1
    .InitSsSegNo            resw 1
    .SegCnt                 resw 1
    .ModCnt                 resw 1
    .NonResNameTabSize      resw 1
    .SegTabOfs              resw 1
    .ResourceTabOfs         resw 1
    .ResNameTabOfs          resw 1
    .ModRefTabOfs           resw 1
    .ImpNameTabOfs          resw 1
    .NonResNameTabOfs       resd 1
    .MovableEntryCnt        resw 1
    .SegAlignShift          resw 1
    .ResourceSegCnt         resw 1
    .OsFlags                resb 1
    .ExeFlags               resb 1
    .FastLoadOfs            resw 1
    .FastLoadSize           resw 1
    .Reserved               resw 1
    .ExpectedWinVer         resw 1
endstruc
; .Flags:
__NE_SINGLEDATA     equ 0x0001
__NE_MULTIPLEDATA   equ 0x0002
__NE_PMODEONLY      equ 0x0008
__NE_LIBRARY        equ 0x8000
__NE_FASTLOAD       equ 0x0008

struc __NESEGENTRY
    .SectorNo   resw 1
    .Size       resw 1
    .Flags      resw 1
    .MinAlloc   resw 1
endstruc
; .Flags:
__NE_SEGDATA        equ 0x0001
__NE_SEGMOVABLE     equ 0x0010
__NE_SEGSHARABLE    equ 0x0020
__NE_SEGPRELOAD     equ 0x0040
__NE_SEGREL         equ 0x0100
__NE_SEGDPL3        equ 0x0C00
__NE_SEGDISCARDABLE equ 0x1000

struc __NERELENTRY
    .AddrType   resb 1
    .RelType    resb 1
    .Ofs        resw 1
    .Data1      resw 1
    .Data2      resw 1
endstruc
; .AddrType:
__NE_RELAOFFSET     equ 0x01
__NE_RELASEGMENT    equ 0x02
__NE_RELAFARPTR     equ 0x03
; .RelType
__NE_RELTINTERNAL   equ 0x00
__NE_RELTIMPORDINAL equ 0x01
__NE_RELTIMPNAME    equ 0x02

%macro NE_START 0
    %push NE

bits 16
org 0

section .mz                                             align=1
section .ne_header          follows=.mz                 align=512
section .segment_table      follows=.ne_header          align=1
section .resource_table     follows=.segment_table      align=1
section .resident_names     follows=.resource_table     align=1
section .mod_ref_table      follows=.resident_names     align=1
section .import_name_table  follows=.mod_ref_table      align=1
section .entry_table        follows=.import_name_table  align=1
section .non_resident_names follows=.entry_table        align=1
section .text               follows=.non_resident_names align=512 vstart=0
section .data               follows=.text               align=512 vstart=0
section .datarelcnt         follows=.data               align=1
section .datarels           follows=.datarelcnt         align=1

section .mz
istruc __MZHDR
    at __MZHDR.Signature,               dw 0x5A4D
    at __MZHDR.PartPage,                dw __mz_size % 512
    at __MZHDR.PageCnt,                 dw (__mz_size + 511) / 512
    at __MZHDR.ReloCnt,                 dw 0
    at __MZHDR.HdrSize,                 dw 512 / 16
    at __MZHDR.MinAlloc,                dw 0x100
    at __MZHDR.MaxAlloc,                dw 0x100
    at __MZHDR.InitSs,                  dw 512 / 16
    at __MZHDR.InitSp,                  dw 0x1000
    at __MZHDR.ChkSum,                  dw 0
    at __MZHDR.InitIp,                  dw 0
    at __MZHDR.InitCs,                  dw 0
    at __MZHDR.ReloOff,                 dw __MZHDR_size
    at __MZHDR.OverlayNo,               dw 0
    at __MZHDR.Reserved,                dw 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
    at __MZHDR.NeHdrOfs,                dd __mz_size
iend
    times (512-($-$$)) db 0

    call    %%cont
    db      "This program requires Microsoft Windows.", 13, 10, "$"
%%cont:
    pop     dx
    push    cs
    pop     ds
    mov     ah, 9
    int     0x21
    mov     ax, 0x4c01
    int     0x21
    times (1024-($-$$)) db 0

section .ne_header
istruc __NEHDR
    at __NEHDR.Signature,               dw 0x454E
    at __NEHDR.LinkerVer,               db 5
    at __NEHDR.LinkerRev,               db 30
    at __NEHDR.EntryTabOfs,             dw __entry_table_ofs
    at __NEHDR.EntryTabSize,            dw __entry_table_size
    at __NEHDR.ChkSum,                  dd 0
    at __NEHDR.Flags,                   dw __NE_MULTIPLEDATA
    at __NEHDR.AutoDataSegNo,           dw 2
    at __NEHDR.HeapInitSize,            dw 0
    at __NEHDR.StackSize,               dw 8192
    at __NEHDR.InitIp,                  dw __start
    at __NEHDR.InitCsSegNo,             dw 1
    at __NEHDR.InitSp,                  dw 0
    at __NEHDR.InitSsSegNo,             dw 2
    at __NEHDR.SegCnt,                  dw __segment_table_size / __NESEGENTRY_size
    at __NEHDR.ModCnt,                  dw __mod_ref_table_size / 2
    at __NEHDR.NonResNameTabSize,       dw __non_resident_names_size
    at __NEHDR.SegTabOfs,               dw __segment_table_ofs
    at __NEHDR.ResourceTabOfs,          dw __resource_table_ofs
    at __NEHDR.ResNameTabOfs,           dw __resident_names_ofs
    at __NEHDR.ModRefTabOfs,            dw __mod_ref_table_ofs
    at __NEHDR.ImpNameTabOfs,           dw __import_name_table_ofs
    at __NEHDR.NonResNameTabOfs,        dd __mz_size + __non_resident_names_ofs
    at __NEHDR.MovableEntryCnt,         dw 0
    at __NEHDR.SegAlignShift,           dw 9
    at __NEHDR.ResourceSegCnt,          dw 0
    at __NEHDR.OsFlags,                 db 2
    at __NEHDR.ExeFlags,                db 0
    at __NEHDR.FastLoadOfs,             dw 0
    at __NEHDR.FastLoadSize,            dw 0
    at __NEHDR.Reserved,                dw 0
    at __NEHDR.ExpectedWinVer,          dw 0x030A
iend

section .segment_table
istruc __NESEGENTRY
    at __NESEGENTRY.SectorNo,   dw __text_ofs / 512
    at __NESEGENTRY.Size,       dw __text_size
    at __NESEGENTRY.Flags,      dw __NE_SEGDPL3 + __NE_SEGPRELOAD
    at __NESEGENTRY.MinAlloc,   dw __text_size
iend
istruc __NESEGENTRY
    at __NESEGENTRY.SectorNo,   dw __data_ofs / 512
    at __NESEGENTRY.Size,       dw __data_size
    at __NESEGENTRY.Flags,      dw __NE_SEGDPL3 + __NE_SEGPRELOAD + __NE_SEGDATA + __NE_SEGREL
    at __NESEGENTRY.MinAlloc,   dw __data_size
iend

section .resident_names
    db 6, "module", 0, 0 ; module name

section .import_name_table
    db 0

section .non_resident_names
    db 4, "none", 0, 0 ; module description

section .data
    db "NULL"
    align 16, db 0

section .datarelcnt
    dw __datarels_size / __NERELENTRY_size
%endmacro

%macro NE_END 0
section .mz
    __mz_size equ $ - $$
section .ne_header
    __ne_header_size equ $ - $$
section .segment_table
    __segment_table_ofs equ __ne_header_size
    __segment_table_size equ $ - $$
section .resource_table
    __resource_table_ofs equ __segment_table_ofs + __segment_table_size
    __resource_table_size equ $ - $$
section .resident_names
    db 0 ; end of table
    __resident_names_ofs equ __resource_table_ofs + __resource_table_size
    __resident_names_size equ $ - $$
section .mod_ref_table
    __mod_ref_table_ofs equ __resident_names_ofs + __resident_names_size
    __mod_ref_table_size equ $ - $$
section .import_name_table
    __import_name_table_ofs equ __mod_ref_table_ofs + __mod_ref_table_size
    __import_name_table_size equ $ - $$
section .entry_table
    db 0 ; 0 entry points in the bundle = end of table
    db 0
    __entry_table_ofs equ __import_name_table_ofs + __import_name_table_size
    __entry_table_size equ $ - $$
section .non_resident_names
    db 0 ; end of table
    __non_resident_names_ofs equ __entry_table_ofs + __entry_table_size
    __non_resident_names_size equ $ - $$
section .text
    align 16, db 0xcc
    __text_ofs equ (__mz_size + __non_resident_names_ofs + __non_resident_names_size + 511) / 512 * 512
    __text_size equ $ - $$
section .data
    align 16, db 0
    __data_ofs equ (__text_ofs + __text_size + 511) / 512 * 512
    __data_size equ $ - $$
section .datarels
    __datarels_size equ $ - $$
%endmacro

%macro IMPORT 0	
    %assign %$LIB_IDX 0
    %assign %$IMP_NAME_OFS 1
%endmacro

%macro LIB 1
    %define %$LIB_NAME %1
    %defstr %%str %1
    %strlen %%len %%str
    %assign %$LIB_IDX %$LIB_IDX + 1
    section .mod_ref_table
        dw %$IMP_NAME_OFS
    section .import_name_table
        db %%len
        db %%str
    %assign %$IMP_NAME_OFS %$IMP_NAME_OFS + 1 + %%len
%endmacro

; Importing functions by their ordinal numbers, e.g.:
;   IMPORT
;     LIB KERNEL
;       BYORD InitTask
;     LIB USER
;       BYORD MessageBox
;       BYORD InitApp
; For the above to work the following definitions must
; be made:
;   ORD_KERNEL_InitTask equ 91
;   ORD_USER_MessageBox equ 1
;   ORD_USER_InitApp    equ 5
%macro BYORD 1
    section .data
        align 4, db 0
        %1 dd -1
    section .datarels
        istruc __NERELENTRY
            at __NERELENTRY.AddrType,   db __NE_RELAFARPTR
            at __NERELENTRY.RelType,    db __NE_RELTIMPORDINAL
            at __NERELENTRY.Ofs,        dw %1
            at __NERELENTRY.Data1,      dw %$$LIB_IDX
            at __NERELENTRY.Data2,      dw ORD_%$$LIB_NAME%+ _%1
        iend
%endmacro

; Importing functions by their names, e.g.:
;   IMPORT
;     LIB KERNEL
;       BYNAME InitTask
;     LIB USER
;       BYNAME MessageBox
;       BYNAME InitApp
%macro BYNAME 1
    %defstr %%str %1
    %strlen %%len %%str
    section .data
        align 4, db 0
        %1 dd -1
    section .datarels
        istruc __NERELENTRY
            at __NERELENTRY.AddrType,   db __NE_RELAFARPTR
            at __NERELENTRY.RelType,    db __NE_RELTIMPNAME
            at __NERELENTRY.Ofs,        dw %1
            at __NERELENTRY.Data1,      dw %$$LIB_IDX
            at __NERELENTRY.Data2,      dw %$$IMP_NAME_OFS
        iend
    section .import_name_table
        db %%len
        db %%str
    %assign %$IMP_NAME_OFS %$IMP_NAME_OFS + 1 + %%len
%endmacro

; Invoking functions imported via IMPORT + LIB + BYORD/BYNAME:
;   INVOKE FxnName[, list of 16-bit args to push]
; This is for the "far pascal" calling convention, AKA WINAPI/CALLBACK.
%macro INVOKE 1-*
    %rep %0-1
        %rotate 1
        push    %1
    %endrep
    %rotate 1
    call    far [%1]
%endmacro

; NE_STARTUP, if used, must be the first thing executed,
; that is, it must immediately follow the entry point label,
; __start.
; Reason: KERNEL.InitTask() used in this macro depends on
; the register values at program start.
%macro NE_STARTUP 0
    INVOKE  InitTask
    or      ax, ax
    jz      %%err

    push    di ; hInstance
    push    si ; hPrevInstance
    push    es ; lpCmdLine selector
    push    bx ; lpCmdLine offset
    push    dx ; nCmdShow

    INVOKE  WaitEvent, 0
    INVOKE  InitApp, di
    or      ax, ax
    jz      %%err

    ; far call to WinMain()
    ; WinMain() must perform a far return and remove 5 words of stack args
    push    cs
    call    WinMain

    mov     ah, 0x4c
    int     0x21

%%err:
    mov     ax, 0x4c01
    int     0x21
%endmacro
