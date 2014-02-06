; Script generated by the HM NIS Edit Script Wizard.

!include Library.nsh
!include x64.nsh

RequestExecutionLevel admin

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "GoZMetaseq"
!define PRODUCT_VERSION "1.0.2"
!define PRODUCT_PUBLISHER "devil.tamachan@gmail.com"
!define PRODUCT_WEB_SITE "http://code.google.com/p/goz-metaseq/"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\${PRODUCT_NAME}"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"


;--------------------------------
;Language Selection Dialog Settings

  ;Remember the installer language
  !define MUI_LANGDLL_REGISTRY_ROOT "HKCU" 
  !define MUI_LANGDLL_REGISTRY_KEY "Software\devil.tamachan\GoZMetaseq\MUI2" 
  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"


; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\classic-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\classic-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
;!insertmacro MUI_PAGE_LICENSE "${NSISDIR}\License.txt"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
  !insertmacro MUI_LANGUAGE "English" ;first language is the default language
  !insertmacro MUI_LANGUAGE "French"
  !insertmacro MUI_LANGUAGE "German"
  !insertmacro MUI_LANGUAGE "Spanish"
  !insertmacro MUI_LANGUAGE "SpanishInternational"
  !insertmacro MUI_LANGUAGE "SimpChinese"
  !insertmacro MUI_LANGUAGE "TradChinese"
  !insertmacro MUI_LANGUAGE "Japanese"
  !insertmacro MUI_LANGUAGE "Korean"
  !insertmacro MUI_LANGUAGE "Italian"
  !insertmacro MUI_LANGUAGE "Dutch"
  !insertmacro MUI_LANGUAGE "Danish"
  !insertmacro MUI_LANGUAGE "Swedish"
  !insertmacro MUI_LANGUAGE "Norwegian"
  !insertmacro MUI_LANGUAGE "NorwegianNynorsk"
  !insertmacro MUI_LANGUAGE "Finnish"
  !insertmacro MUI_LANGUAGE "Greek"
  !insertmacro MUI_LANGUAGE "Russian"
  !insertmacro MUI_LANGUAGE "Portuguese"
  !insertmacro MUI_LANGUAGE "PortugueseBR"
  !insertmacro MUI_LANGUAGE "Polish"
  !insertmacro MUI_LANGUAGE "Ukrainian"
  !insertmacro MUI_LANGUAGE "Czech"
  !insertmacro MUI_LANGUAGE "Slovak"
  !insertmacro MUI_LANGUAGE "Croatian"
  !insertmacro MUI_LANGUAGE "Bulgarian"
  !insertmacro MUI_LANGUAGE "Hungarian"
  !insertmacro MUI_LANGUAGE "Thai"
  !insertmacro MUI_LANGUAGE "Romanian"
  !insertmacro MUI_LANGUAGE "Latvian"
  !insertmacro MUI_LANGUAGE "Macedonian"
  !insertmacro MUI_LANGUAGE "Estonian"
  !insertmacro MUI_LANGUAGE "Turkish"
  !insertmacro MUI_LANGUAGE "Lithuanian"
  !insertmacro MUI_LANGUAGE "Slovenian"
  !insertmacro MUI_LANGUAGE "Serbian"
  !insertmacro MUI_LANGUAGE "SerbianLatin"
  !insertmacro MUI_LANGUAGE "Arabic"
  !insertmacro MUI_LANGUAGE "Farsi"
  !insertmacro MUI_LANGUAGE "Hebrew"
  !insertmacro MUI_LANGUAGE "Indonesian"
  !insertmacro MUI_LANGUAGE "Mongolian"
  !insertmacro MUI_LANGUAGE "Luxembourgish"
  !insertmacro MUI_LANGUAGE "Albanian"
  !insertmacro MUI_LANGUAGE "Breton"
  !insertmacro MUI_LANGUAGE "Belarusian"
  !insertmacro MUI_LANGUAGE "Icelandic"
  !insertmacro MUI_LANGUAGE "Malay"
  !insertmacro MUI_LANGUAGE "Bosnian"
  !insertmacro MUI_LANGUAGE "Kurdish"
  !insertmacro MUI_LANGUAGE "Irish"
  !insertmacro MUI_LANGUAGE "Uzbek"
  !insertmacro MUI_LANGUAGE "Galician"
  !insertmacro MUI_LANGUAGE "Afrikaans"
  !insertmacro MUI_LANGUAGE "Catalan"
  !insertmacro MUI_LANGUAGE "Esperanto"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "${PRODUCT_NAME}-${PRODUCT_VERSION}.exe"
InstallDir "$PROGRAMFILES\GoZMetaseq"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite on
;  CreateDirectory "$SMPROGRAMS\GoZMetaseq"
;  CreateDirectory "$INSTDIR\htm"
${If} ${RunningX64}
  CreateDirectory "$APPDATA\tetraface\Metasequoia4_x64\Plugins\Command"
  SetOutPath "$APPDATA\tetraface\Metasequoia4_x64\Plugins\Command"
  File "..\Release\x64\GoZMetaseq64.dll"
${Endif}
  CreateDirectory "$APPDATA\tetraface\Metasequoia4\Plugins\Command"
  SetOutPath "$APPDATA\tetraface\Metasequoia4\Plugins\Command"
  File "..\Release\Win32\GoZMetaseq.dll"
  CreateDirectory "$PROFILE\..\public\Pixologic\GoZApps\Metaseq"
  SetOutPath "$PROFILE\..\public\Pixologic\GoZApps\Metaseq"
  File "..\..\GoZApps\Metaseq\GoZ_Info.txt"
  SetOutPath "$INSTDIR"

SectionEnd

Section -AdditionalIcons
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$APPDATA\tetraface\Metasequoia4\Plugins\Command\GoZMetaseq.dll"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
;  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\Bz.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(MUI_UNTEXT_FINISH_SUBTITLE)"
FunctionEnd


Function .onInit
  SetRegView 32
  StrCpy $INSTDIR "$PROGRAMFILES\GoZMetaseq"

  !insertmacro MUI_LANGDLL_DISPLAY

;  ReadRegStr $R0 HKLM \
;  "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
;  "UninstallString"
;  StrCmp $R0 "" done
 
;  MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
;  "${PRODUCT_NAME}は既にインストールされています。$\n$\nOKボタンをクリックすると前のバージョンを先に削除します。Cancelボタンをクリックするとそのまま上書きします。" \
;  IDOK uninst
;  Abort
  
;Run the uninstaller
;uninst:
;  ClearErrors
;  ExecWait '$R0 _?=$INSTDIR' ;Do not copy the uninstaller to a temp file
 
;  IfErrors no_remove_uninstaller
    ;You can either use Delete /REBOOTOK in the uninstaller or add some code
    ;here to remove the uninstaller. Use a registry key to check
    ;whether the user has chosen to uninstall. If you are using an uninstaller
    ;components page, make sure all sections are uninstalled.
;  no_remove_uninstaller:
  
done:
FunctionEnd

Function un.onInit

  !insertmacro MUI_UNGETLANGUAGE

;  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "$(^Name)を完全に削除しますがよろしいですか？" IDYES +2
;  Abort

  SetRegView 32
  StrCpy $INSTDIR "$PROGRAMFILES\GoZMetaseq"
FunctionEnd

Section Uninstall
${If} ${RunningX64}
  Delete "$APPDATA\tetraface\Metasequoia4_x64\Plugins\Command\GoZMetaseq64.dll"
${Endif}
  Delete "$APPDATA\tetraface\Metasequoia4\Plugins\Command\GoZMetaseq.dll"
  Delete "$PROFILE\..\public\Pixologic\GoZApps\Metaseq\GoZ_Info.txt"
  RMDir "$PROFILE\..\public\Pixologic\GoZApps\Metaseq"

  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd
