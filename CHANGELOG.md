# CalcExpress Toolkit Changelog


# VERSION 4.0.0

## Licensing

### Transition to MIT
Since this version, CalcExpress Toolkit has transitioned to the Open Source model under the terms of the **MIT License** (see the full text in the accompanying `LICENSE` file). All source code of the Toolkit is available at GitHub:
	https://github.com/konstantin-shifershteyn/calcexpress

You are entirely free to download, install, modify, and build CalcExpress into your proprietary, closed-source commercial applications or distributed automated test systems without any legal restrictions. The only requirement is that the original copyright notice remains intact within the source code.

### Legacy License Holders
* For owners of commercial licenses for the previous versions (3.x and earlier), please review the important notice below in this document.


## CalcExpress and CSE

### Scripts and expressions:
* Predefined Named Constants in scripts and expressions. 
* Parsing Directives in scripts and expressions.
* New documented ways of implicit conversion of DBL values to 32-bit integers.

### Parsing and Execution VIs
* Parsing VIs have got the Directive control for initial Parsing Directives state.
* New VIs to preset or read Writer ID for the running script or expression.

### Script Functions
* New functions to explicitly convert DBL values to 32-bit integers: `i32s`, `i32b`, `u32s`, `u32b`.
* New function to read/write default Writer ID for the script or expression: `vwrid`
* New functions to read variable statistics: `vtype`, `vstcid`, `vstcms`, `vstcts`, `vstwid`, `vstwms`, `vstwts`, `vstwcnt`
* New functions to handle special values: `nan`, `isnan`, `isfinite`, `inf`, `infn`, `isinf`.
* New function to return the list of Named Constants: `_nclist`.
* User-interaction functions updated. The windows they open are resizable: `Probe`, `vhShow`, `WFGraph`, `XYGraph`
* Attach history function has got new parameter that enables/disables collecting Writer IDs in the variable history: `vhAttach`

### CSE ISP utility (Interactive Script Processor)
* Within the opened script, it offers help on a right-clicked statement, function name, Parsing Directive, or Named Constant.
* Indicates its default Writer ID.
* Has got new example scripts for new features.


## Variable Manager

### Variable Properties
* Variables have got Writer ID property.
* Variables collect statistics about their creation and update.
* A Variable History can be configured to collect Writer IDs along with variable values and time stamps.

### VarMan VIs
* New VIs to manage variables. They allow writing and reading Writer ID and Writing Statistics together with variable value: `VM3 - <TYPE>.vi`
* New VI to retrieve information on the variable. It returns both Creating and Writing Statistics, along with other data: `VM3 - Variable Data Info.vi`
* New VIs to read and write multiple variables in a batch: `VM3 - Read Variables (<TYPE>).vi` and `VM3 - Write Variables (<TYPE>).vi`
* `VHistory - History Controller.vi` has got a new command to create a variable history that collects Writer IDs.
* Read variable history VIs have got new Writer IDs indicator. Note that these VIs do not have the `dup VC Handle` indicator anymore, its terminal is now connected to the Writer IDs indicator: `VHistory - Get <TYPE> History.vi`.

### VarMan Spy utility
* Has got variable name filter.
* Supports Writer ID, and indicates variable statistics and memory use.
* Opens resizable Variable Inspector windows with more data now.
* Saves and restores its window position and size, and values and essential properties of its controls.


## CalcExpress Engine DLLs
* Code has got drastic revamp to improve performance and extend functionality.


## Documentation (.chm)
* Content and style updated.
* Third-party dependencies (JavaScripts) are eliminated.


## Miscellaneous Changes
* Various minor enhancements, code cleanups, and performance tweaks were implemented throughout the VIs and C code, which are not explicitly listed in this file.

---

## Important Notice for Existing License Holders (Versions 3.x and earlier)

If you or your company has previously purchased commercial development and/or runtime licenses for older versions of CalcExpress, please review the following terms regarding the Version 4.0 transition:

1. **Validity of Prior Licenses:** 
All commercial licenses purchased over the past years remain completely valid, active, and legally binding for the specific versions they were issued for. Your deployment history remains fully compliant.

2. **Transition to Version 4.0:** 
With the launch of Version 4.0, the entire toolkit has transitioned to the permissive **MIT License**. As a legacy customer, you are highly encouraged to upgrade your applications to Version 4.0 completely free of charge. By upgrading, your project automatically inherits the unrestricted commercial rights granted under the MIT framework.

3. **Runtime Tracking Discontinuation:** 
Starting with Version 4.0, the manual tracking and reporting of the historical runtime fee is **completely discontinued**. All internal deployment and external distribution of compiled applications embedding CalcExpress 4.0 (and subsequent versions) is now 100% royalty-free.

4. **Support and Maintenance Transition:** 
All historical support arrangements, email troubleshooting channels, and direct engineering consultations associated with legacy purchases have officially concluded. Version 4.0 is a mature release provided strictly **AS IS**. Legacy users requiring ongoing updates or specific modifications are welcome to leverage the unlocked C engine source code and LabVIEW assets in this repository.

---

# VERSION 3.0.1

* Bug fixes and updates to the core code, user utilities, predefined functions, and documentation.

---

# VERSION 3.0.0

* CalcExpress Toolkit can be used with both **32-bit** and **64-bit** LabVIEW.
* Core DLL code has been revamped to add new features, improve performance, and fix known issues.
* Revamped CalcExpress documentation is provided in the Compiled HTML Help format (.CHM file).
* Scripts syntax is extended.
* VIs, utilities, and examples are updated.
* etc.
