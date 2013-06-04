/* 
 * This is a dll providing global keyboard hook functionality. The way windows operates, it is 
 * only possible to set a global hook function if it is located inside a dll.
 */

#define _WIN32_WINNT 0x0401

#include <stdlib.h>
#include <assert.h>

#include <windows.h>
 
typedef struct keyhook_s{
	HINSTANCE process;
	void (*callback)(unsigned int, void*);
	void* data;
}keyhook_t;

#define MAX_HOOKS 32

#pragma data_seg(".hook_data")
keyhook_t *hooks[MAX_HOOKS]={0};
HHOOK gsKeyHook;
int exclusive_hook = -1;
int ref_count = 0;
#pragma data_seg()

LRESULT CALLBACK _KeyboardProc(int nCode,WPARAM wParam,LPARAM lParam){
	// Call all hooks in order
	KBDLLHOOKSTRUCT *key = (KBDLLHOOKSTRUCT*)lParam;
	int c;

	if (nCode < 0)  // do not process message 
        return CallNextHookEx(gsKeyHook, nCode, wParam, lParam); 

	if(exclusive_hook!=-1 && hooks[exclusive_hook]){
		hooks[exclusive_hook]->callback(key->vkCode, hooks[exclusive_hook]->data);
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}

	for(c=0;c<MAX_HOOKS;c++){
		if(hooks[c]){
			hooks[c]->callback(key->vkCode, hooks[c]->data);
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void install_hook(HINSTANCE module){
	memset(hooks, 0, sizeof(void*)*MAX_HOOKS);
	gsKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, _KeyboardProc, module, 0);
}

void remove_hook(){
	UnhookWindowsHookEx(gsKeyHook);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch(ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			//to speed things up, disable recieving of messages in subsequent loaded copies
			DisableThreadLibraryCalls( hModule );
			if(!ref_count)
				install_hook(hModule);
			ref_count++;
			break;
		case DLL_THREAD_ATTACH:
			// A thread is created. Do any required initialization on a per thread basis
			break;
		case DLL_THREAD_DETACH:
			// Thread exits with cleanup
			break;
		case DLL_PROCESS_DETACH:
			// The DLL unmapped from process's address space. Do necessary cleanup
			ref_count--;
			if(!ref_count)
				remove_hook();
		break;
	}
    return TRUE;
}

/*****************
 * EXPORTS
 *****************/

/* Add a function to be called on every key press on the entire system */
__declspec(dllexport) unsigned int sys_add_keyboard_hook(void (*callback)(unsigned int, void*), int exclusive, void* data){
	int c;
	for(c=0;c<MAX_HOOKS;c++){
		if(!hooks[c]){
			if(exclusive)
				exclusive_hook=c;
			hooks[c] = calloc(1, sizeof(keyhook_t));
			hooks[c]->callback = callback;
			hooks[c]->data = data;
			return c;
		}
	}
	return -1;
}

__declspec(dllexport) void sys_remove_keyboard_hook(unsigned int c){
	assert(c>=0 && c<MAX_HOOKS);

	if(exclusive_hook==c)
		exclusive_hook=-1;

	if(hooks[c]){
		free(hooks[c]);
		hooks[c]=0;
	}
}