#include <windows.h>
unsigned int base;

unsigned int JMP_back;

void __declspec(naked) ASMRemovePVPDamage(){
    asm("cmp dword ptr [ebx], 0xFF");
    asm("ja 0f"); //Do nothing if low DWORD of attacker GUID is > 255

    asm("cmp dword ptr [ebx+0x4], 0");
    asm("ja 0f"); //Do nothing if high DWORD of attacker GUID is > 0

    asm("cmp dword ptr [ebx+0x8], 0xFF");
    asm("ja 0f"); //Do nothing if low DWORD of target GUID is > 255

    asm("cmp dword ptr [ebx+0xC], 0");
    asm("ja 0f"); //Do nothing if high DWORD of target GUID is > 0

    asm("mov dword ptr [ebx+0x10], 0"); //Set damage to 0 if it fell through everything else (ie, it's pvp)
    asm("mov dword ptr [ebx+0x14], 0"); //Set critical to 0
    asm("mov dword ptr [ebx+0x18], 0"); //Set stun duration to 0

    asm("0:");
    //original code
    asm("sub esi ,eax");
    asm("add ebx, eax");
    asm("mov eax, [ebp-0x13AC]");
    asm("JMP [_JMP_back]");
}

void WriteJMP(BYTE* location, BYTE* newFunction){
    DWORD dwOldProtection;
    VirtualProtect(location, 5, PAGE_EXECUTE_READWRITE, &dwOldProtection);
    location[0] = 0xE9; //jmp
    *((DWORD*)(location + 1)) = (DWORD)(( (unsigned INT32)newFunction - (unsigned INT32)location ) - 5);
    VirtualProtect(location, 5, dwOldProtection, &dwOldProtection);
}

extern "C" __declspec(dllexport) bool APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {

        case DLL_PROCESS_ATTACH:
            base = (unsigned int)GetModuleHandle(NULL);

            JMP_back =  base + 0x264A8;
            WriteJMP((BYTE*)(base + 0x2649E), (BYTE*)&ASMRemovePVPDamage);
            break;

    }
    return true;
}
