#ifndef _entry_h
#define _entry_h

#include "resource/resource.h"

#define WDS_PANE "window.container"

typedef struct dimension{
    int left;

    int top;

    int width;

    int height;
}dimension;

typedef struct widget{
    HWND      hwnd;
    
    dimension position;

    const char *    window_class;

    const char *    name;

    int       style;

    HMENU     id;

    WNDPROC   editorProc;
}widget;

LRESULT CALLBACK stringEntryEditorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK listEntryEditorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK setEntryEditorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK zetEntryEditorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK hashEntryEditorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK entryDlgProc(HWND hWin,UINT msg,WPARAM wParam,LPARAM lParam);

void init_editor_view(HINSTANCE hInstance);

LRESULT CALLBACK default_entry_proc(HWND dataHwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif
