/*
	D2GL: Diablo 2 LoD Glide/DDraw to OpenGL Wrapper.
	Copyright (C) 2023  Bayaraa

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "pch.h"
#include "pd2_fixes.h"
#include "d2/common.h"
#include "helpers.h"
#include "patch.h"

namespace d2gl {

int pd2_draw_shifted_image = 0;

uintptr_t* PD2Fn1 = nullptr;
uintptr_t* FreeResFn1 = nullptr;

uintptr_t d2client_address = 0;
uint8_t* d2client_old_code = nullptr;
uint8_t* d2client_new_code = nullptr;

bool isPD2()
{
	static HANDLE handle = GetModuleHandleA("ProjectDiablo.dll");
	return (bool)handle;
}

__declspec(naked) void d2glideFix()
{
	static uintptr_t addr_back = 0x6F85A61A;
	static uintptr_t addr_skip = 0x6F85A67E;
	__asm
	{
		push eax
		xor eax, eax
		mov eax, pd2_draw_shifted_image
		test eax, eax
		mov pd2_draw_shifted_image, 0x0
		je skip
		pop eax
		lea edi, [eax + ebx * 8 + 04]
		jmp [addr_skip]
		
		skip:
		pop eax
		test ecx, ecx
		lea edi, [eax + ebx * 8 + 04]
		jmp [addr_back]
	}
}

void applyPD2fixes(int step)
{
	if (!App.pd2_fix)
		return;

	if (step == 0) {
		d2client_address = helpers::getProcOffset(DLL_D2CLIENT, 0x10DFD);
		Patch::getBytes(d2client_address, 76, &d2client_old_code);
	} else if (step == 1) {
		if (PD2Fn1 != FreeResFn1) {
			Patch::getBytes(d2client_address, 76, &d2client_new_code);
		}

		if (App.api == Api::Glide) {
			Patch patch = Patch();
			patch.add(PatchType::Jump, { 0xA614, 0, 0, DLL_D2GLIDE }, 6, (uintptr_t)d2glideFix);
			patch.toggle(true);
		}
	}
}
}