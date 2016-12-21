# PolyHook - x86/x64 Hooking Library
**Provides abstract C++ 11 interface  for various hooking methods**

#Hooking Methods*:

1. **_Detour_**
  * Description: Modifies opcode to jmp to hook and allocates a trampoline for jmp back
  * Length Disassembler Support (Capstone)
  * Supports Code Relocation, including EIP/RIP relative instructions

2. **_Virtual Function Detour_** : 
  * Description: Detours the function pointed to by the Vtable

3. **_Virtual Function Pointer Swap_** 
  * Description: Swaps the pointer in the Vtable to your hook
  
4. **_Virtual Table Pointer Swap_**
  * Description: Swaps the Vtable pointer after copying pointers in source Vtable, 
  then swaps virtual function pointer in the new copy

5. **Import Address Table**
  * Description: Swaps pointer in the import address table to the hook

6. **VEH**
  * Description: Intercepts an exception generated on purpose, sets instruction pointer to handler, then resets exception generating mechanism
  * Methods to generate exception: INT3 Breakpoints, Guard Page violations.
  * **Note**: it is important to call the GetProtectionObject function INSIDE of your callback as per my example for all VEH hooks
  * Other exception generation methods are in development

* All methods support x86 and x64
* Relies on modified capstone branch https://github.com/stevemk14ebr/capstone
* More Information can be found at the wiki to the right

Credits to DarthTon, evolution536, Dogmatt

#Samples:
The file Tests.cpp provides examples for every type of hooking method. Accompanied with these examples is unit testing code provided by the fantastic library Catch (https://github.com/philsquared/Catch/blob/master/docs/tutorial.md). With the addition of this code the example may look a little complex, the general interface is extremely simple, all hook types expose setup, hook, and unhook methods:

```C++
std::shared_ptr<PLH::Detour> Detour_Ex(new PLH::Detour);
Detour_Ex->SetupHook((BYTE*)&MessageBoxA,(BYTE*) &hkMessageBoxA); //can cast to byte* to
Detour_Ex->Hook();
oMessageBoxA = Detour_Ex->GetOriginal<tMessageBoxA>();
Detour_Ex->UnHook();
```

#LICENSE:
MIT
