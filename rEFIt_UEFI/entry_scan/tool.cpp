/*
 * refit/scan/tool.c
 *
 * Copyright (c) 2006-2010 Christoph Pfisterer
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *  * Neither the name of Christoph Pfisterer nor the names of the
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "entry_scan.h"
#include "../refit/menu.h"
#include "../refit/screen.h"
#include "../libeg/XImage.h"
#include "../refit/lib.h"
#include "../gui/REFIT_MENU_SCREEN.h"

//
// Clover File location to boot from on removable media devices
//
#define CLOVER_MEDIA_FILE_NAME_IA32    L"\\EFI\\CLOVER\\CLOVERIA32.EFI"_XSW
#define CLOVER_MEDIA_FILE_NAME_IA64    L"\\EFI\\CLOVER\\CLOVERIA64.EFI"_XSW
#define CLOVER_MEDIA_FILE_NAME_X64     L"\\EFI\\CLOVER\\CLOVERX64.EFI"_XSW
#define CLOVER_MEDIA_FILE_NAME_ARM     L"\\EFI\\CLOVER\\CLOVERARM.EFI"_XSW

#if   defined (MDE_CPU_IA32)
#define CLOVER_MEDIA_FILE_NAME   CLOVER_MEDIA_FILE_NAME_IA32
#elif defined (MDE_CPU_IPF)
#define CLOVER_MEDIA_FILE_NAME   CLOVER_MEDIA_FILE_NAME_IA64
#elif defined (MDE_CPU_X64)
#define CLOVER_MEDIA_FILE_NAME   CLOVER_MEDIA_FILE_NAME_X64
#elif defined (MDE_CPU_EBC)
#elif defined (MDE_CPU_ARM)
#define CLOVER_MEDIA_FILE_NAME   CLOVER_MEDIA_FILE_NAME_ARM
//#else
//#error Unknown Processor Type
#endif

#ifndef DEBUG_ALL
#define DEBUG_SCAN_TOOL 0
#else
#define DEBUG_SCAN_TOOL DEBUG_ALL
#endif

#if DEBUG_SCAN_TOOL == 0
#define DBG(...)
#else
#define DBG(...) DebugLog(DEBUG_SCAN_TOOL, __VA_ARGS__)
#endif

STATIC BOOLEAN AddToolEntry(IN CONST XStringW& LoaderPath, IN CONST CHAR16 *FullTitle, IN CONST CHAR16 *LoaderTitle,
                            IN REFIT_VOLUME *Volume, const XIcon& Image,
                            IN CHAR16 ShortcutLetter, IN CONST XString8Array& Options)
{
  REFIT_MENU_ENTRY_LOADER_TOOL *Entry;
  // Check the loader exists
  if ((LoaderPath.isEmpty()) || (Volume == NULL) || (Volume->RootDir == NULL) ||
      !FileExists(Volume->RootDir, LoaderPath)) {
    return FALSE;
  }
  // Allocate the entry
  Entry = new REFIT_MENU_ENTRY_LOADER_TOOL();
  if (Entry == NULL) {
    return FALSE;
  }

  if (FullTitle) {
    Entry->Title.takeValueFrom(FullTitle);
  } else {
    Entry->Title.SWPrintf("Start %ls", LoaderTitle);
  }
//  Entry->Tag = TAG_TOOL;
  Entry->Row = 1;
  Entry->ShortcutLetter = ShortcutLetter;
  Entry->Image = Image;
//  Entry->ImageHover = ImageHover;
  Entry->LoaderPath = LoaderPath;
  Entry->DevicePath = FileDevicePath(Volume->DeviceHandle, Entry->LoaderPath);
  Entry->DevicePathString = FileDevicePathToXStringW(Entry->DevicePath);
  Entry->LoadOptions = Options;
  //actions
  Entry->AtClick = ActionSelect;
  Entry->AtDoubleClick = ActionEnter;
  Entry->AtRightClick = ActionHelp;

  DBG("found tool %ls\n", LoaderPath.s());
  MainMenu.AddMenuEntry(Entry, true);
  return TRUE;
}

STATIC VOID AddCloverEntry(IN CONST XStringW& LoaderPath, IN CONST CHAR16 *LoaderTitle, IN REFIT_VOLUME *Volume)
{
  REFIT_MENU_ENTRY_CLOVER      *Entry;
  REFIT_MENU_ENTRY_CLOVER      *SubEntry;
  REFIT_MENU_SCREEN *SubScreen;
//  EFI_STATUS        Status;

  // prepare the menu entry
  Entry = new REFIT_MENU_ENTRY_CLOVER();
  Entry->Title.takeValueFrom(LoaderTitle);
//  Entry->Tag            = TAG_CLOVER;
  Entry->Row            = 1;
  Entry->ShortcutLetter = 'C';
  Entry->Image          = ThemeX.GetIcon(BUILTIN_ICON_FUNC_CLOVER);
  Entry->Volume = Volume;
  Entry->LoaderPath      = LoaderPath;
  Entry->VolName         = Volume->VolName;
  Entry->DevicePath      = FileDevicePath(Volume->DeviceHandle, Entry->LoaderPath);
  Entry->DevicePathString = FileDevicePathToXStringW(Entry->DevicePath);
  Entry->Flags           = 0;
  Entry->LoadOptions.setEmpty();
//  Entry->LoaderType      = OSTYPE_OTHER;

  //actions
  Entry->AtClick = ActionEnter;
  Entry->AtDoubleClick = ActionDetails;
  Entry->AtRightClick = ActionDetails;

  // create the submenu
  SubScreen = new REFIT_MENU_SCREEN;

  SubScreen->Title.takeValueFrom(LoaderTitle);

  SubScreen->TitleImage = Entry->Image;
  SubScreen->ID = SCREEN_BOOT;
  SubScreen->GetAnime();
  SubScreen->AddMenuInfoLine_f("%ls", FileDevicePathToXStringW(Volume->DevicePath).wc_str());

  if (gEmuVariableControl != NULL) {
    gEmuVariableControl->UninstallEmulation(gEmuVariableControl);
  }

//always add and always remove menu entries
  SubEntry = Entry->getPartiallyDuplicatedEntry();
  if (SubEntry) {
    SubEntry->Title.SWPrintf("Add Clover boot options for all entries");
    SubEntry->LoadOptions.setEmpty();
    SubEntry->LoadOptions.Add("BO-ADD");
    SubScreen->AddMenuEntry(SubEntry, true);
  }

  SubEntry = Entry->getPartiallyDuplicatedEntry();
  if (SubEntry) {
    SubEntry->Title.SWPrintf("Remove all Clover boot options");
    SubEntry->LoadOptions.setEmpty();
    SubEntry->LoadOptions.Add("BO-REMOVE");
    SubScreen->AddMenuEntry(SubEntry, true);
  }

  SubEntry = Entry->getPartiallyDuplicatedEntry();
  if (SubEntry) {
    SubEntry->Title.SWPrintf("Print all UEFI boot options to log");
    SubEntry->LoadOptions.setEmpty();
    SubEntry->LoadOptions.Add("BO-PRINT");
    SubScreen->AddMenuEntry(SubEntry, true);
  }

  SubScreen->AddMenuEntry(&MenuEntryReturn, false);
  Entry->SubScreen = SubScreen;
  MainMenu.AddMenuEntry(Entry, true);
}

VOID ScanTool(VOID)
{
  EFI_STATUS              Status;
  UINTN                   VolumeIndex;
  REFIT_VOLUME            *Volume;
  VOID                    *Interface;
  if (ThemeX.HideUIFlags & HIDEUI_FLAG_TOOLS)
    return;

  //    DBG("Scanning for tools...\n");
  if (!(ThemeX.HideUIFlags & HIDEUI_FLAG_SHELL)) {
    if (!AddToolEntry(L"\\EFI\\CLOVER\\tools\\Shell64U.efi"_XSW, NULL, L"UEFI Shell 64", SelfVolume, ThemeX.GetIcon(BUILTIN_ICON_TOOL_SHELL), 'S', NullXString8Array)) {
      AddToolEntry(L"\\EFI\\CLOVER\\tools\\Shell64.efi"_XSW, NULL, L"EFI Shell 64", SelfVolume, ThemeX.GetIcon(BUILTIN_ICON_TOOL_SHELL), 'S', NullXString8Array);
    }
  }

//  if (!gFirmwareClover) { //Slice: I wish to extend functionality on emulated nvram
    for (VolumeIndex = 0; VolumeIndex < Volumes.size(); VolumeIndex++) {
      Volume = &Volumes[VolumeIndex];
      if (!Volume->RootDir || !Volume->DeviceHandle) {
        continue;
      }

      Status = gBS->HandleProtocol (Volume->DeviceHandle, &gEfiPartTypeSystemPartGuid, &Interface);
      if (Status == EFI_SUCCESS) {
		  DBG("Checking EFI partition Volume %llu for Clover\n", VolumeIndex);

        // OSX adds label "EFI" to EFI volumes and some UEFIs see that
        // as a file. This file then blocks access to the /EFI directory.
        // We will delete /EFI file here and leave only /EFI directory.
        if (DeleteFile(Volume->RootDir, L"EFI")) {
          DBG(" Deleted /EFI label\n");
        }

        if (FileExists(Volume->RootDir, CLOVER_MEDIA_FILE_NAME)) {
          DBG(" Found Clover\n");
          // Volume->BootType = BOOTING_BY_EFI;
          AddCloverEntry(CLOVER_MEDIA_FILE_NAME, L"Clover Boot Options", Volume);
          break;
        }
      }
    }
//  }
}

// Add custom tool entries
VOID AddCustomTool(VOID)
{
  UINTN             VolumeIndex;
  REFIT_VOLUME      *Volume;
  CUSTOM_TOOL_ENTRY *Custom;
  XIcon             Image;
  UINTN              i = 0;

//  DBG("Custom tool start\n");
  DbgHeader("AddCustomTool");
  // Traverse the custom entries
  for (Custom = gSettings.CustomTool; Custom; ++i, Custom = Custom->Next) {
    if (OSFLAG_ISSET(Custom->Flags, OSFLAG_DISABLED)) {
		DBG("Custom tool %llu skipped because it is disabled.\n", i);
      continue;
    }
    if (!gSettings.ShowHiddenEntries && OSFLAG_ISSET(Custom->Flags, OSFLAG_HIDDEN)) {
		DBG("Custom tool %llu skipped because it is hidden.\n", i);
      continue;
    }

    if (Custom->Volume.notEmpty()) {
		DBG("Custom tool %llu matching \"%ls\" ...\n", i, Custom->Volume);
    }
    for (VolumeIndex = 0; VolumeIndex < Volumes.size(); ++VolumeIndex) {
      Volume = &Volumes[VolumeIndex];

      DBG("   Checking volume \"%ls\" (%ls) ... ", Volume->VolName, Volume->DevicePathString);

      // Skip Whole Disc Boot
      if (Volume->RootDir == NULL) {
        DBG("skipped because volume is not readable\n");
        continue;
      }

      // skip volume if its kind is configured as disabled
      if (((1ull<<Volume->DiskKind) & GlobalConfig.DisableFlags) != 0)
      {
        DBG("skipped because media is disabled\n");
        continue;
      }

      if (Custom->VolumeType != 0) {
        if (((1ull<<Volume->DiskKind) & Custom->VolumeType) == 0) {
          DBG("skipped because media is ignored\n");
          continue;
        }
      }

      if (Volume->Hidden) {
        DBG("skipped because volume is hidden\n");
        continue;
      }

      // Check for exact volume matches
      if (Custom->Volume.notEmpty()) {
        if ((StrStr(Volume->DevicePathString.wc_str(), Custom->Volume.wc_str()) == NULL) &&
            ((Volume->VolName.isEmpty()) || (StrStr(Volume->VolName.wc_str(), Custom->Volume.wc_str()) == NULL))) {
          DBG("skipped\n");
          continue;
        }
      }
      // Check the tool exists on the volume
      if (!FileExists(Volume->RootDir, Custom->Path)) {
        DBG("skipped because path does not exist\n");
        continue;
      }
      // Change to custom image if needed
      Image = Custom->Image;
      if (Image.isEmpty() && Custom->ImagePath.notEmpty()) {
        Image.LoadXImage(ThemeX.ThemeDir, Custom->ImagePath);
      }
      if (Image.isEmpty()) {
        AddToolEntry(Custom->Path, Custom->FullTitle.wc_str(), Custom->Title.wc_str(), Volume, ThemeX.GetIcon(BUILTIN_ICON_TOOL_SHELL), Custom->Hotkey, Custom->LoadOptions);
      } else {
      // Create a legacy entry for this volume
        AddToolEntry(Custom->Path, Custom->FullTitle.wc_str(), Custom->Title.wc_str(), Volume, Image, Custom->Hotkey, Custom->LoadOptions);
      }
      DBG("match!\n");
//      break; // break scan volumes, continue scan entries -- why?
    }
  }
//  DBG("Custom tool end\n");
}
