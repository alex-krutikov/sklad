Name       "Sklad"
OutFile    "sklad-setup.exe"
InstallDir $PROGRAMFILES\Sklad

XpStyle on
SetCompressor lzma

LoadLanguageFile "${NSISDIR}\Contrib\Language files\Russian.nlf"

InstallDirRegKey HKLM "Software\Sklad" "Install_Dir"

;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "Склад"
  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File ..\src\build\sklad.exe

  File ..\..\qt\bin\qtcore4.dll
  File ..\..\qt\bin\qtgui4.dll
  File ..\..\qt\bin\qtsql4.dll
  File ..\..\qt\bin\libmysql.dll
  File ..\..\qt\bin\mingwm10.dll

  SetOutPath $INSTDIR\doc

  File ..\doc\admin.html
  File ..\doc\base.pdf
  File ..\doc\sklad4_init.sql
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\Sklad "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Sklad" "DisplayName" "Sklad"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Sklad" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Sklad" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Sklad" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

  CreateDirectory "$SMPROGRAMS\Склад"
  CreateShortCut  "$SMPROGRAMS\Склад\Удаление.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut  "$SMPROGRAMS\Склад\Sklad.lnk" "$INSTDIR\Sklad.exe" "" "$INSTDIR\Sklad.exe" 0
  CreateShortCut  "$SMPROGRAMS\Склад\Руководство администратора.lnk" "$INSTDIR\doc\admin.html"
SectionEnd

; Optional section (can be disabled by the user)
Section "Иконка на рабочий стол"
  CreateShortCut "$DESKTOP\Склад.lnk" "$INSTDIR\Sklad.exe" "" "$INSTDIR\Sklad.exe" 0
SectionEnd

;----------------------------------------------------------------------------------------------------------------

; Uninstaller

Section "Uninstall"
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Sklad"
  DeleteRegKey HKLM SOFTWARE\Sklad

  RMDir /r "$INSTDIR"

  ; Remove shortcuts, if any
  Delete "$DESKTOP\Склад.lnk"

  ; Remove directories used
  RMDir /r "$SMPROGRAMS\Склад"

SectionEnd
