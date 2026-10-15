# CalcExpress Help Sources (`/CE_Help/`)

This folder contains the source files, project assets, and raw HTML pages used to compile the official **CalcExpress HTML Help manual (`CEToolkit.chm`)**.

---

## Files & Sub-Folders

* `CE_Toolkit.hgprj` — A **VI Helpeks** project configuration file. VI Helpeks is a LabVIEW add-on used to dynamically generate HTML help documentation for VIs and function palettes according to pre-configured rules. Using VI Helpeks is optional, but it significantly automates and simplifies the help-generation process. 
  * *System Requirement:* The minimum VI Helpeks version required is **3.0.4**.

* `/html_source/` — Contains static source HTML files and layout assets. These files contain general toolkit information, script syntax guidelines, and detailed script function descriptions. **These files are edited manually** and do not contain individual VI descriptions.

* `/html/` — Contains the finalized, fully compiled HTML tree and deployment assets for the CalcExpress help manual. This folder represents the complete help system as a collection of standalone HTML files, images, CSS stylesheets, and JavaScript files. 
  * *Automation Note:* These files are automatically updated/overwritten whenever you regenerate help using VI Helpeks.
  * *CHM Compiler Project:* This folder also contains the master **Microsoft HTML Help Workshop** project files (`.hhp`, `.hhc`, `.hhk`) required to compile the final runtime Compiled HTML Help file (`CEToolkit.chm`).
  * *.CHM destination:* The compiled `CEToolkit.chm` file is output directly into the `/help/Calcexpress/` folder.
  