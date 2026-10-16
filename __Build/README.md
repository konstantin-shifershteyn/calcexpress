# Build Distribution Tools (`/__Build/`)

This folder contains VIs and configuration files to build CalcExpress distribution packages for installation on end-user computers.


## Preamble

1. A **standard** distribution package must comply with these requirements:
   - 1.1. It is a `.vip` package.
   - 1.2. It is built for (and contains all VIs saved in) **LabVIEW 2014**.
   - 1.3. All Pre-/Post- Build/Install/Uninstall VIs used by the **VIPM Package Builder** must be saved in LabVIEW 2014.

2. LabVIEW versions for the VIs:
   - 2.1. All source CalcExpress VIs in this repository are saved in **LabVIEW 2021**.
   - 2.2. In this folder (`/__Build/`), VIs that operate on the source VIs (building or editing) are saved in **LabVIEW 2021**.
   - 2.3. All VIs that either are targeted to or operate on LabVIEW 2014 are saved in **LabVIEW 2014**. These include the Pre-/Post- Build/Install/Uninstall VIs in this folder (`/__Build/`) and the helper VIs in the `/__Deploy/` folder.

*(Note: Unless explicitly stated otherwise, LabVIEW 2021 or later can be used to open and run VIs from this repository. Any VI that contains `LV2014` in its name MUST be saved and run in LabVIEW 2014.)*

*(Note: To create a **standard** distribution using the VIPM Package Builder, you MUST have LabVIEW 2014 installed alongside LabVIEW 2021 or later. If you do not have it installed, you can create a **non-standard** distribution (see below). **IMPORTANT:** Non-standard distribution packages CANNOT be distributed over the official channels, including but not limited to `VIPM`.)*


## Creating **standard** CalcExpress distribution

1. (Optional) Run `/__Build/Edit CalcExpress Info.vi` to preset the toolkit version information. Note that this modifies the toolkit VIs version, not the core DLL version.

2. Run `/__Build/Build CalcExpress Deployment.vi` to build the deployment version of the toolkit VIs. This VI creates two deployments in the `/__Deploy/` folder: an intermediate deployment for the current active LabVIEW version, and a target deployment for **LabVIEW 2014** (`/__Deploy/LV 14.0/` folder), which serves as the source to build the final distribution package.
 
3. Optional but Recommended) In **LabVIEW 2014**, open and run `/__Deploy/Update CLFN in LV2014 Deploy.vi`. This VI updates the `Call Library Function Nodes` (CLFN) inside VIs within the `/__Deploy/LV 14.0/` folder to ensure they are properly linked using the correct relative paths to the core 32-bit and 64-bit DLLs. *(Note: If this step is skipped, LabVIEW will still automatically relink the CLFNs on target computers after installation, but it will trigger extra search prompts.)*

4. Open the `/__Build/CalcExpress (LV2014).vipb` file in the **VIPM Package Builder** and build the package.


## Creating **non-standard** CalcExpress Distribution

### Using the **VIPM Package Builder**

1. Create your `.vipb` file. You can use `/__Build/CalcExpress (LV2014).vipb` as a template and reconfigure it for your needs (e.g., changing the target LabVIEW version).

2. Follow the same build steps as described for creating a standard distribution, except that you will use your custom `.vipb` file.

### Using the **OpenG Package Builder (OGPB)**

1. Follow steps 1 to 3 as described for creating a standard distribution.

2. Open the `/__Build/CalcExpress (LV2014).ogpb` file in the `OpenG Package Builder` and build the package.

*(Note: You do not need LabVIEW 2014 installed to create a package for LabVIEW 2014 using OGPB.)*

### Using a **ZIP** or alternative archive utility

1. Follow steps 1 to 3 as described for creating a standard distribution.

2. Pack the folders from `/__Deploy/LV 14.0/` to your archive file (excluding project files in the root). This archive file can then be simply extracted into the LabVIEW root directory on target computers.
