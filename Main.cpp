#include <Windows.h>
#include <iostream>
#include <array>



void Console() {

    AllocConsole();

    freopen("CONOUT$", "w", stdout);

}

class Addresses {

public:

    uintptr_t BaseAdd = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));

    uintptr_t GameAssembly = reinterpret_cast<uintptr_t>(GetModuleHandle("GameAssembly.dll"));

    uintptr_t LibBurst = reinterpret_cast<uintptr_t>(GetModuleHandle("lib_burst_generated.dll"));

    uintptr_t HealthFunction = GameAssembly + 0x285FBFC;

    uintptr_t HungerFunction = LibBurst + 0x14BE819;

    uintptr_t ItemDurability = LibBurst + 0x17186FC;

};

class Bytes {

protected:

    void NopBytes(PVOID Address, int bytes) {

        DWORD d, ds;
        VirtualProtect(Address, bytes, PAGE_EXECUTE_READWRITE, &d);
        memset(Address, 0x90, bytes);
        VirtualProtect(Address, bytes, d, &ds);
    }

    void CopyBytes(PVOID Address, int bytes, byte Buffer[]) {

        DWORD d, ds;
        VirtualProtect(Address, bytes, PAGE_EXECUTE_READWRITE, &d);
        memcpy(Buffer, Address, bytes);
        VirtualProtect(Address, bytes, d, &ds);

    }
    
    void RestoreBytes(PVOID Address, int bytes, byte Buffer[]) {

        DWORD d, ds;
        VirtualProtect(Address, bytes, PAGE_EXECUTE_READWRITE, &d);
        memcpy(Address, Buffer, bytes);
        VirtualProtect(Address, bytes, d, &ds);
    }

};

class GameFunctions : public Bytes {

private:

    Addresses a;

    byte HungerBuffer[2];

    byte HealthBuffer[6];

    byte DurabilityBuffer[5];

public:

    void Invincible() {

        CopyBytes(reinterpret_cast<PVOID>(a.HealthFunction), 6, HealthBuffer);

        NopBytes(reinterpret_cast<PVOID>(a.HealthFunction), 6);

    }

    void NotInvincible() {

        RestoreBytes(reinterpret_cast<PVOID>(a.HealthFunction),6, HealthBuffer);

    }

    void NoHunger() { 

        CopyBytes(reinterpret_cast<PVOID>(a.HungerFunction), 2, HungerBuffer);

        NopBytes(reinterpret_cast<PVOID>(a.HungerFunction), 2);

    }

    void Hunger() {

        RestoreBytes(reinterpret_cast<PVOID>(a.HungerFunction),2, HungerBuffer);

    }

    void NoDurability() {

        CopyBytes(reinterpret_cast<PVOID>(a.ItemDurability), 5, DurabilityBuffer);

        NopBytes(reinterpret_cast<PVOID>(a.ItemDurability), 5);

    }

    void Durability() {

        RestoreBytes(reinterpret_cast<PVOID>(a.ItemDurability), 5, DurabilityBuffer);
    }
};

void Main(HMODULE Module) {

    Console();

    GameFunctions g;

    bool Health = false, Hunger = false, ItemDurability = false;

    std::cout << "Keys: " << std::endl;
    std::cout << "[F1] Invincibility " << std::endl;
    std::cout << "[F2] No Hunger " << std::endl;
    std::cout << "[F3] Infinite Durability" << std::endl;


    while (true) {

        if (GetAsyncKeyState(VK_END) & 1) {

            FreeLibraryAndExitThread(Module, 0);
        }
        else if (GetAsyncKeyState(VK_F1) & 1) {

            Health = !Health;

            if (Health) {

                g.Invincible();

            }

            else {

                g.NotInvincible();

            }

        }
        else if (GetAsyncKeyState(VK_F2) & 1) {

            Hunger = !Hunger;

            if (Hunger) {

                g.NoHunger();

            }

            else {

                g.Hunger();

            }

            Sleep(1);
        }
        else if (GetAsyncKeyState(VK_F3) & 1) {

            ItemDurability = !ItemDurability;

            if (ItemDurability) {

                g.NoDurability();

            }

            else {

                g.Durability();

            }
        }
    }
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Main), nullptr, NULL, nullptr);
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

