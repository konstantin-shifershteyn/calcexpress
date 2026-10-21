# Built Deployments folder  (`/__Deploy/`)

This folder contains LabVIEW code generated during the preparation of distribution packages.

* *(Note: Files in this folder are automatically generated when you run `/__Build/Build CalcExpress Deployment.vi`)*

* *(Note: Content of this folder is pre-configured to be ignored by the `.gitignore` file.)*

The `/LV 14.0/` sub-folder will contain CalcExpress Toolkit VIs saved for LabVIEW 2014, which are used as the source to build the final `.vip` package.

The optional `Update CLFN in LV2014 Deploy.vi` can be used to set proper path to the CalcExpress Engine DLL in the Call Library Function Nodes (CLFN) within the deployment for LabVIEW 2014. If used, it **must be saved and run in LabVIEW 2014**.
