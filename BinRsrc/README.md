# CalcExpress Engine Source Code (`/BinRsrc/`)

This folder contains the Microsoft Visual Studio 2022 (MSVC2022) solution used to compile the 32-bit and 64-bit CalcExpress Engine DLLs for the CalcExpress Toolkit.

---

## Preparation & Environment Variables

Before opening the solution in MSVC2022, you **MUST** define the following Windows Environment Variables on your system. These are required so the compiler can locate the necessary National Instruments LabVIEW External Code Interface (`cintools`) headers and libraries:

* **`CINTOOLSDIR32`** = Path to your 32-bit LabVIEW `cintools` directory (used to build the 32-bit DLL).
* **`CINTOOLSDIR64`** = Path to your 64-bit LabVIEW `cintools` directory (used to build the 64-bit DLL).

### Examples:
```text
CINTOOLSDIR32 = "C:\Program Files (x86)\National Instruments\LabVIEW 2021\cintools"
CINTOOLSDIR64 = "C:\Program Files\National Instruments\LabVIEW 2021\cintools"

# Or if using custom directory shortcuts:
CINTOOLSDIR32 = "C:\LVTools\LV2021_32\cintools"
CINTOOLSDIR64 = "C:\LVTools\LV2021_64\cintools"
```

---

## Source Files & Sub-Folders

* `CalcExpress Engine.sln` — The primary MSVC2022 solution file used to manage and build the engine.
* `copy-dll.bat` — A batch script you can run post-build to automatically copy the compiled DLLs to the exact location where the LabVIEW source VIs expect them (`..\vi.lib\addons\CalcExpress\cersrc`).
* `/ce-dll/` — Contains the master project files for the core engine DLLs.
* `/ce-lib/` — Contains the project files for supporting static libraries.
* `/include/` — Contains common C source header files (`.h`).

---

## Build Artifacts & Temporary Folders

The following sub-directories are dynamically generated during the compilation process and should **not** be tracked or uploaded to the GitHub repository:

### Custom Project Output Folders:
* `/_Intermediate/` — Contains intermediate object and compiler state files.
* `/_Lib/` — Contains compiled intermediate support library files (`.lib`).
* `/_Release/` — Target folder for the final, freshly built production `celveng32.dll` and `celveng64.dll` files.

### Standard Visual Studio Cache Folders:
* `/.vs/` — Local user settings and workspace cache.
* `/ipch/` — IntelliSense precompiled header caches.

*(Note: These build folders are pre-configured to be ignored by the repository's `.gitignore` file to ensure a clean codebase).*


