# CalcExpress Toolkit for LabVIEW

Copyright (c) 2000-2026 Konstantin Shifershteyn. All rights reserved.

## Introduction
CalcExpress Toolkit is an add-on for LabVIEW on Windows that introduces text-based scripting, formula/expression evaluation, and advanced data management tools. It is engineered to be deeply trusted, highly reliable, and incredibly flexible for both scientific research and industrial automated test environments.

CalcExpress Toolkit consists of a set of LabVIEW VIs and engine DLLs (for 32-bit and 64-bit LabVIEW environments). This repository contains all source code along with the end-user documentation.

## Formats, Tools and Requirements
To open, modify, or rebuild the various components of this toolkit, the following environment tools are used:
* **LabVIEW Version:** All LabVIEW VIs in this repository are saved natively in **LabVIEW 2021** format, with the exception of a **small number** of build helper VIs saved in **LabVIEW 2014**.
* **C Code Compiler:** MS Visual Studio 2022 is used to compile the engine DLLs from the C source code.
* **Documentation Compiler:** Microsoft HTML Help Workshop is used to compile the final documentation CHM file.
* **Help Generation Tool:** VI Helpeks 3.0.4 is used to generate documentation HTML files for LabVIEW VIs.

## No Dependencies & EU CRA Compliance
To simplify compliance tracking and risk assessments for commercial systems covered by the **EU Cyber Resilience Act (CRA)**, the dependency footprint of this toolkit is detailed below:

* **Zero Third-Party Dependencies:** Neither the LabVIEW VIs nor the engine DLLs utilize, bundle, or rely on any third-party open-source libraries, external packages, or commercial software components.
* **Self-Contained C Engine:** The C engine is compiled using the static runtime library option (`/MT`). The standard C Runtime (CRT) is fully embedded directly inside the compiled DLLs. This completely eliminates external dependencies on Microsoft Visual C++ Redistributable packages as well as any third-party libraries.
* **Managed Memory & Isolated OS Footprint:** 
  * *Memory Management:* The C engine relies strictly on native **LabVIEW Memory Manager API functions** to allocate and resize data structures safely within LabVIEW's memory space.
  * *System API Interactivity:* From the Windows system APIs, the C engine calls only **basic operational and thread synchronization functions**. The C engine **does not invoke** APIs for networking, registry access, file system I/O, external process execution, or memory management.
* **Native LabVIEW Code & Script Extensibility:** All graphical source code consists exclusively of native LabVIEW primitives and standard functions.
  * *Execution Boundary:* The toolkit supports calling external LabVIEW VIs (`FuncVIs`) to extend script functionality. These calls are executed entirely within the native host LabVIEW environment via wrapper VIs, completely outside the C engine DLL boundary.
  * *Toolkit & User Functions:* The toolkit includes a set of predefined, visible `FuncVIs` (including standard file I/O and VI control utilities). Users can also implement custom `FuncVIs` with arbitrary capabilities (e.g., networking). The security vetting and use of these predefined and custom extensions are the responsibility of the end-user.
* **Audit-Ready:** Because the full C source code and LabVIEW blocks are completely visible, the toolkit is 100% auditable for corporate Software Bill of Materials (SBOM) generation, supply-chain validation, and static code analysis.

## Version 4.0 Milestone Release
Version 4.0 represents a significant evolutionary upgrade for the CalcExpress ecosystem. The complete, detailed list of new features is disclosed directly inside the accompanying help documentation and the [CHANGELOG.md](CHANGELOG.md) file.

With this version, the entire toolkit, including all LabVIEW VIs and the full C engine source code, **is now completely open and visible** to build 100% developer trust.

## Maintenance & Support Notice
CalcExpress is a mature product with a quarter-century operational history. With the launch of Version 4.0, the toolkit has transitioned to a fully open-source project and is provided **AS IS, WITHOUT WARRANTY OF ANY KIND**. 

The author is an individual developer and **can no longer guarantee technical support, email troubleshooting, or ongoing maintenance**. Because the full C engine source code and LabVIEW blocks are now unlocked, you have complete control. If you encounter an edge-case bug or wish to optimize the engine further for your specific needs, you are enthusiastically welcome to fork this repository, make modifications, and submit a Pull Request.

## Licensing (MIT)
This project is completely open-source under the terms of the **MIT License** (see the full text in the accompanying `LICENSE` file). 

You are entirely free to download, install, modify, and build CalcExpress into your proprietary, closed-source commercial applications or distributed automated test systems without any legal restrictions. The only requirement is that the original copyright notice remains intact within the source code.

* **Legacy Customer Notice:** If you or your company hold historical commercial development or runtime licenses for older versions (3.x and earlier), please see the dedicated [Legacy License Holders section in the CHANGELOG](CHANGELOG.md#important-notice-for-existing-license-holders-versions-3x-and-earlier) for terms regarding your upgrade path and discontinued tracking fees.

## Support the Developer & Project
CalcExpress Toolkit is distributed **100% free of charge** to maximize adoption and accessibility across the global LabVIEW community. However, maintaining high-performance engineering tools requires considerable efforts over decades. 

If CalcExpress saves your engineering team valuable hours or is actively used in your development or production, please consider supporting the author's efforts and ongoing work.

### Sponsorship & Invoicing
At the moment, sponsorship payments can be accepted exclusively via direct international bank wire transfer (SWIFT). If your company requires formal documentation for tax compliance, the author can provide a manual invoice and payment receipt. To request bank wire routing details or a formal corporate invoice, please contact the author via the verified contact details on the **[GitHub Profile Page](https://github.com/konstantin-shifershteyn)** with your billing details and desired sponsorship amount.

---

## Repository Structure

### Source files to build CalcExpress components
* `/BinRsrc/` — Original MSVC2022 C source code for the CalcExpress Engine DLLs.
* `/CE_Help/` — Source files and assets used to build the CalcExpress HTML Help (`.chm`).

### Source files targeted to their respective LabVIEW directory locations
* `/Examples/` — Example VIs targeted to `<labview>\examples\`.
* `/Help/` — Documentation files targeted to `<labview>\help\`.
* `/Project/` — Utility VIs targeted to `<labview>\project\`.
* `/Templates/` — Template VIs targeted to `<labview>\templates\`.
* `/vi.lib/` — Master toolkit VIs and palettes targeted to `<labview>\vi.lib\`.

### Building and Distribution
* `/__Build/` folder contains VIs and configuration files to build CalcExpress distribution packages for installation on end-user computers.
* `/__Deploy/` folder is the target for generating LabVIEW code that is used for building distribution packages. See the [/__Deploy/README.md](/__Deploy/README.md) file for details.
* `/__Packaged/` folder is the target for distribution packages you build. See the [/__Packaged/README.md](/__Packaged/README.md) file for details.
