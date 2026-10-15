# CalcExpress Toolkit for LabVIEW

Copyright (c) 2000-2026 Konstantin Shifershteyn. All rights reserved.

## Introduction
CalcExpress Toolkit is an add-on for LabVIEW on Windows that introduces text-based scripting, formula/expression evaluation, and advanced data management tools. It is engineered to be deeply trusted, highly reliable, and incredibly flexible for both scientific research and industrial automated test environments.

CalcExpress Toolkit consists of a set of LabVIEW VIs and engine DLLs (for 32-bit and 64-bit LabVIEW environments). This repository contains all source code along with the end-user documentation.

## Formats, Tools and Requirements
To open, modify, or rebuild the various components of this toolkit, the following environment tools are used:
* **LabVIEW Version:** All LabVIEW VIs in this repository are saved natively in **LabVIEW 2021** format.
* **C Code Compiler:** MS Visual Studio 2022 is used to compile the engine DLLs from the C source code.
* **Documentation Compiler:** Microsoft HTML Help Workshop is used to compile the final documentation CHM file.
* **Help Generation Tool:** VI Helpeks 3.0.4 is used to generate documentation HTML files for LabVIEW VIs.

## Version 4.0 Milestone Release
Version 4.0 represents a significant evolutionary upgrade for the CalcExpress ecosystem. The complete, detailed list of new features is disclosed directly inside the accompanying help documentation.

With this version, the entire toolkit, including all LabVIEW VIs and the full C engine source code, is now completely open and visible to build 100% developer trust.

## Maintenance & Support Notice
CalcExpress is a mature product with a quarter-century operational history. With the launch of Version 4.0, the toolkit has transitioned to a fully open-source project and is provided **AS IS, WITHOUT WARRANTY OF ANY KIND**. 

The author is an individual developer and **can no longer guarantee technical support, email troubleshooting, or ongoing maintenance**. Because the full C engine source code and LabVIEW blocks are now unlocked, you have complete control. If you encounter an edge-case bug or wish to optimize the engine further for your specific needs, you are enthusiastically welcome to fork this repository, make modifications, and submit a Pull Request.

## Licensing (MIT)
This project is completely open-source under the terms of the **MIT License** (see the full text in the accompanying `LICENSE` file). 

You are entirely free to download, install, modify, and build CalcExpress into your proprietary, closed-source commercial applications or distributed automated test systems without any legal restrictions. The only requirement is that the original copyright notice remains intact within the source code.

* **Legacy Customer Notice:** If you or your company hold historical commercial development or runtime licenses for older versions (3.x and earlier), please see the dedicated [Legacy License Holders section in the CHANGELOG](CHANGELOG.md#important-notice-for-existing-license-holders-versions-3x-and-earlier) for terms regarding your upgrade path and discontinued tracking fees.

## Support the Developer & Project
CalcExpress Toolkit is distributed **100% free of charge** to maximize adoption and accessibility across the global LabVIEW community. However, maintaining high-performance engineering tools requires considerable effort over decades. 

If CalcExpress saves your engineering team hours of development or is deployed on your production lines, please consider supporting the author's ongoing work.

### Sponsorship & Invoicing
At the moment, sponsorship payments can be accepted exclusively via direct international bank wire transfer (SWIFT). If your company requires formal documentation for tax compliance, the author can provide a manual invoice and payment receipt. To request bank wire routing details or a formal corporate invoice, please contact the author via the verified contact details on the **[GitHub Profile Page](https://github.com/konstantin-shifershteyn)** with your billing details and desired sponsorship amount.

---

## Repository Structure

### Source files to build CalcExpress components
* `/BitRsrc/` — Original MSVC2022 C source code for the CalcExpress Engine DLLs.
* `/CE_Help/` — Source files and assets used to build the CalcExpress HTML Help (`.chm`).

### Source files mapped to their respective LabVIEW directory locations
* `/Examples/` — Example VIs. Mapped directly to `<labview>\examples\`.
* `/Help/` — Compiled documentation files. Mapped directly to `<labview>\help\`.
* `/Project/` — Utility and configuration VIs. Mapped directly to `<labview>\project\`.
* `/Templates/` — Template VIs. Mapped directly to `<labview>\templates\`.
* `/vi.lib/` — Master toolkit VIs and palettes. Mapped directly to `<labview>\vi.lib\`.

