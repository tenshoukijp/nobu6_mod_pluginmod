#pragma once


extern HINSTANCE hPluginMod;


// PluginMod.dll��Initialize���\�b�h�p�̌^�錾
// HANDLE�ɂ͌Ăяo�����Ƃ��ē����B
typedef void (WINAPI *PFNPLUGINMODINITIALIZE)(HANDLE);
