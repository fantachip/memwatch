/*
-----------------------------------------------------------------------------
This source file is part of MemWatch (An application to watch memory inside a
foregin process in real time) 

For the latest info, see http://www.schreder.nu/

Copyright (c) 2008 Martin K. Schreder <mkschreder@gmail.com>

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation;

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc., 59
Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.
-----------------------------------------------------------------------------
*/


#include "main.h"

#define _WIN32_WINNT 0x0401

#include <windows.h>
#include <winuser.h>
#include <tlhelp32.h>

static int Process_write_memory(Process *self, address_t address, void *data, size_t size){
	DWORD count;

	assert(self &&  data);

	if(!WriteProcessMemory(self->handle, (LPVOID)address, data, size, &count))
		return 0;
	return 1;
}

static int Process_read_memory(Process *self, address_t address, void *data, size_t size){
	DWORD count;
	
	assert(self &&  data);
    
	if(!ReadProcessMemory((HANDLE)self->handle, (LPVOID)address, data, size, &count))
		return 0;
	return 1;
}

// Descend into a variable path and read the address of the final data
static address_t proc_read_variable_address(Process *self, VariablePath *path, int inner){
	address_t base = 0;
	address_t address = 0;

	// Go depth first
	if(path->next)
		base = proc_read_variable_address(self, path->next, 1);

	// If we got a module base address then set base to module base
	if(strlen(path->module)){
		base = Process_get_module_address(self, path->module);
		if(!base)
			return 0;
	}

	// If we are in the topmost level of recursion
	if(!inner)
		return base+path->address;

	if(!Process_read_memory(self, base+path->address, &address, sizeof(address_t)))
		return 0;
	return address;
}

// This function retreives process id of the first process that has "exe_name" string in it's executable name 
static DWORD S_GetProcessID(const char *exe_name) {
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;

	DWORD pid = 0;
	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if ( hProcessSnap == INVALID_HANDLE_VALUE ) return( FALSE );
	pe32.dwSize = sizeof( PROCESSENTRY32 );
	if ( !Process32First( hProcessSnap, &pe32 ) ) {
		CloseHandle( hProcessSnap );
		return 0;
	}
	do {
		// Retrieve the priority class.
		dwPriorityClass = 0;
		hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID );
		if ( hProcess != NULL ) {
			dwPriorityClass = GetPriorityClass( hProcess );
			if ( !dwPriorityClass )
				CloseHandle( hProcess );
		}
		if (strcmp(exe_name, pe32.szExeFile)==0) {
			pid=pe32.th32ProcessID;
			break;
		}

	} while ( Process32Next( hProcessSnap, &pe32 ) );

	CloseHandle( hProcessSnap );
	return pid;
}

int sys_foreach_process(void (*callback)(Process *proc, void *data), void *data){
    HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;
	Process proc;
	
	DWORD pid = 0;
	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if ( hProcessSnap == INVALID_HANDLE_VALUE ) return( FALSE );
	pe32.dwSize = sizeof( PROCESSENTRY32 );
	if ( !Process32First( hProcessSnap, &pe32 ) ) {
		CloseHandle( hProcessSnap );
		return 0;
	}
	do {
		// Retrieve the priority class.
		dwPriorityClass = 0;
		hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID );
		if ( hProcess != NULL ) {
			dwPriorityClass = GetPriorityClass( hProcess );
			if ( !dwPriorityClass )
				CloseHandle( hProcess );
		}
		strcpy(proc.executable, pe32.szExeFile);
		proc.pid = pe32.th32ProcessID;
		callback(&proc, data);
	} while ( Process32Next( hProcessSnap, &pe32 ) );

	CloseHandle( hProcessSnap );
	return pid;
}

int Process_open(Process *self, const char *name){
	int pid;

	assert(self && name);
	
	pid = S_GetProcessID(name);

	if(!pid)
		return 0;

	self->pid = pid;
	self->handle = (Handle_t)OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
	strncpy(self->executable, name, sizeof(self->executable));
	return 1;
}

address_t Process_get_module_address(Process *self, const char *module){
	HANDLE hSnap;
	MODULEENTRY32 xModule;
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, self->pid);
	xModule.dwSize = sizeof(MODULEENTRY32);
	if (Module32First(hSnap, &xModule)) {
		while (Module32Next(hSnap, &xModule)) {
			if (_stricmp(xModule.szModule, module) == 0) {
				CloseHandle(hSnap);
				return (address_t)xModule.modBaseAddr;
			}
		}
	}
	CloseHandle(hSnap);
	return 0;
}

int Process_write(Process *self, Variable *value){
	address_t address = proc_read_variable_address(self, &value->path, 0);

	void *data;
	size_t size = Variable_size(value);

	if(!address || !size || value->read_only)
		return 0;
	
	data = malloc(size+1);

	Variable_toBinary(value, data);
	Process_write_memory(self, address, data, size);

	free(data);
	return 1;
}

int Process_read(Process *self, Variable *value, int hex){
	address_t address = proc_read_variable_address(self, &value->path, 0);

	void *data;
	size_t size = Variable_size(value);

	if(!address || !size)
		return 0;
	
	data = malloc(size);

	if(!Process_read_memory(self, address, data, size)){
		free(data);
		return 0;
	}

	Variable_fromBinary(value, data, hex);

	free(data);
	return 1;
}

void sys_vkcode_to_string(unsigned int code, char *keyname, int size){
	GetKeyNameText(MapVirtualKey(code, 0)<<16, keyname, size);
}

unsigned int (*_sys_add_keyboard_hook)(void (*callback)(unsigned int, gpointer), int exclusive, gpointer);
void (*_sys_remove_keyboard_hook)(unsigned int);

// When exclusive is set then this hook will be the only one called untill it is deleted or 
// untill another exclusive hook is set.
unsigned int sys_add_keyboard_hook(void (*callback)(unsigned int, gpointer), int exclusive, gpointer data){
	return _sys_add_keyboard_hook(callback, exclusive, data);
}

void sys_remove_keyboard_hook(unsigned int c){
	_sys_remove_keyboard_hook(c);
}


void sys_init(){
	HMODULE dll = LoadLibrary("hotkey.dll");
	if(!dll){
		MessageBox(0, "Could not load hook dll!", "ERROR", MB_ICONERROR);
		ExitProcess(0);
	}
	_sys_add_keyboard_hook = (unsigned int(*)(void(*)(unsigned,gpointer),gpointer))GetProcAddress(dll, "sys_add_keyboard_hook");
	_sys_remove_keyboard_hook = (void(*)(unsigned int))GetProcAddress(dll, "sys_remove_keyboard_hook");
	
	// Go to profiles directory
	SetCurrentDirectory("..\\profiles");
}

void sys_finish(){
	
}