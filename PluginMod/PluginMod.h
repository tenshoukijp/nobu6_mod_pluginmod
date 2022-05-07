#pragma once


extern HINSTANCE hPluginMod;


// PluginMod.dllのInitializeメソッド用の型宣言
// HANDLEには呼び出し元として入れる。
typedef void (WINAPI *PFNPLUGINMODINITIALIZE)(HANDLE);
