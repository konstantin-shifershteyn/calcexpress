@echo off

set	NameDLL32=celveng32.dll
set	NameDLL64=celveng64.dll
set	myDir=%~dp0

	copy "%myDir%\_Release\32bit\%NameDLL32%" "%myDir%\..\vi.lib\addons\CalcExpress\cersrc\"
rem	IF ERRORLEVEL	 1	goto err_copy

	copy "%myDir%\_Release\64bit\%NameDLL64%" "%myDir%\..\vi.lib\addons\CalcExpress\cersrc\"
rem	IF ERRORLEVEL	 1	goto err_copy

	@echo --
	@echo -- SUCCESS --
	@echo --
	goto end

:err_copy
	@echo --
	@echo -- ERROR: %ERRORLEVEL% --
	@echo --
	goto end
:end
