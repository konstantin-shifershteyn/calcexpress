@echo off

set	ceRsrcDir=%~dp0
set	VerBuildFile="%ceRsrcDir%\setCeBuildNo.bat"
set	VerHeaderFile="%ceRsrcDir%\ceDllBuildNo.h"

	if EXIST %VerBuildFile%		goto exec
	set	DLL_BUILD=1
	@echo	set DLL_BUILD=%DLL_BUILD% > %VerBuildFile%

:exec
	call	%VerBuildFile%

	set /A	DLL_BUILD="DLL_BUILD+1"
	@echo	set DLL_BUILD=%DLL_BUILD% > %VerBuildFile%
	@echo	#define	__DLL_BUILD %DLL_BUILD% > %VerHeaderFile%
	goto end

:error1
	@echo	=========================================================
	@echo	FATAL ERROR: ceBinDir environment variable is not defined
	@echo	=========================================================
	goto end

:end
 