/*
 * refit/scan/loader.c
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

#include "loader.h"
#include "../cpp_foundation/XString.h"
#include "entry_scan.h"
#include "../Platform/Settings.h"
#include "../Platform/Hibernate.h"
#include "../refit/screen.h"
#include "../refit/menu.h"
#include "common.h"
#include "../Platform/Nvram.h"
#include "../Platform/APFS.h"
#include "../Platform/guid.h"
#include "../refit/lib.h"
#include "../gui/REFIT_MENU_SCREEN.h"

#ifndef DEBUG_ALL
#define DEBUG_SCAN_LOADER 1
#else
#define DEBUG_SCAN_LOADER DEBUG_ALL
#endif

#if DEBUG_SCAN_LOADER == 0
#define DBG(...)
#else
#define DBG(...) DebugLog(DEBUG_SCAN_LOADER, __VA_ARGS__)
#endif

const XStringW MACOSX_LOADER_PATH = L"\\System\\Library\\CoreServices\\boot.efi"_XSW;

const XStringW LINUX_ISSUE_PATH = L"\\etc\\issue"_XSW;
#define LINUX_BOOT_PATH L"\\boot"
#define LINUX_BOOT_ALT_PATH L"\\boot"
const XString8 LINUX_LOADER_PATH = "vmlinuz"_XS8;
const XStringW LINUX_FULL_LOADER_PATH = SWPrintf("%ls\\%s", LINUX_BOOT_PATH, LINUX_LOADER_PATH.c_str());
#define LINUX_LOADER_SEARCH_PATH L"vmlinuz*"
const XString8Array LINUX_DEFAULT_OPTIONS = Split<XString8Array>("ro add_efi_memmap quiet splash vt.handoff=7", " ");

#if defined(MDE_CPU_X64)
#define BOOT_LOADER_PATH L"\\EFI\\BOOT\\BOOTX64.efi"_XSW
#else
#define BOOT_LOADER_PATH L"\\EFI\\BOOT\\BOOTIA32.efi"_XSW
#endif


//extern LOADER_ENTRY *SubMenuKextInjectMgmt(LOADER_ENTRY *Entry);

// Linux loader path data
typedef struct LINUX_PATH_DATA
{
   CONST XStringW Path;
   CONST XStringW Title;
   CONST XStringW Icon;
   CONST XString8  Issue;
} LINUX_PATH_DATA;

typedef struct LINUX_ICON_DATA
{
   CONST CHAR16 *DirectoryName;
   CONST CHAR16 *IconName;
} LINUX_ICON_MAPPING;

STATIC LINUX_ICON_DATA LinuxIconMapping[] = {
    { L"LinuxMint", L"mint" },
    { L"arch_grub", L"arch" },
    { L"opensuse", L"suse" },
    { L"ORACLE", L"solaris" },
    { L"elementary", L"eos" },
    { L"pclinuxos", L"pclinux" },
    { L"mx18", L"mx" },
    { L"mx19", L"mx" }
};
STATIC CONST UINTN LinuxIconMappingCount = (sizeof(LinuxIconMapping) / sizeof(LinuxIconMapping[0]));

STATIC LINUX_PATH_DATA LinuxEntryData[] = {
#if defined(MDE_CPU_X64)
  //comment out all common names
//  { L"\\EFI\\grub\\grubx64.efi", L"Grub EFI boot menu", L"grub,linux" },
//  { L"\\EFI\\Gentoo\\grubx64.efi", L"Gentoo EFI boot menu", L"gentoo,linux", "Gentoo" },
  { L"\\EFI\\Gentoo\\kernelx64.efi"_XSW, L"Gentoo EFI kernel"_XSW, L"gentoo,linux"_XSW },
//  { L"\\EFI\\RedHat\\grubx64.efi", L"RedHat EFI boot menu", L"redhat,linux", "Redhat" },
//  { L"\\EFI\\debian\\grubx64.efi", L"Debian EFI boot menu", L"debian,linux", "Debian" },
//  { L"\\EFI\\kali\\grubx64.efi", L"Kali EFI boot menu", L"kali,linux", "Kali" },
//  { L"\\EFI\\ubuntu\\grubx64.efi", L"Ubuntu EFI boot menu", L"ubuntu,linux", "Ubuntu" },
//  { L"\\EFI\\kubuntu\\grubx64.efi", L"kubuntu EFI boot menu", L"kubuntu,linux", "kubuntu" },
//  { L"\\EFI\\LinuxMint\\grubx64.efi", L"Linux Mint EFI boot menu", L"mint,linux", "Linux Mint" },
//  { L"\\EFI\\Fedora\\grubx64.efi", L"Fedora EFI boot menu", L"fedora,linux", "Fedora" },
//  { L"\\EFI\\opensuse\\grubx64.efi", L"OpenSuse EFI boot menu", L"suse,linux", "openSUSE" },
//  { L"\\EFI\\arch\\grubx64.efi", L"ArchLinux EFI boot menu", L"arch,linux" },
//  { L"\\EFI\\arch_grub\\grubx64.efi", L"ArchLinux EFI boot menu", L"arch,linux" },
//  { L"\\EFI\\ORACLE\\grubx64.efi", L"Oracle Solaris EFI boot menu", L"solaris,linux", "Solaris" },
//  { L"\\EFI\\Endless\\grubx64.efi", L"EndlessOS EFI boot menu", L"endless,linux", "EndlessOS" },
//  { L"\\EFI\\antergos_grub\\grubx64.efi", L"Antergos EFI boot menu", L"antergos,linux", "Antergos" },
//  { L"\\EFI\\Deepin\\grubx64.efi", L"Deepin EFI boot menu", L"deepin,linux", "Deepin" },
//  { L"\\EFI\\elementary\\grubx64.efi", L"Elementary EFI boot menu", L"eos,linux", "Elementary" },
//  { L"\\EFI\\Manjaro\\grubx64.efi", L"Manjaro EFI boot menu", L"manjaro,linux", "Manjaro" },
//  { L"\\EFI\\xubuntu\\grubx64.efi", L"Xubuntu EFI boot menu", L"xubuntu,linux", "Xubuntu" },
//  { L"\\EFI\\zorin\\grubx64.efi", L"Zorin EFI boot menu", L"zorin,linux", "Zorin" },
  { L"\\EFI\\goofiboot\\goofibootx64.efi"_XSW, L"Solus EFI boot menu"_XSW, L"solus,linux"_XSW, "Solus"_XS8 },
//  { L"\\EFI\\centos\\grubx64.efi", L"CentOS EFI boot menu", L"centos,linux", "CentOS" },
//  { L"\\EFI\\pclinuxos\\grubx64.efi", L"PCLinuxOS EFI boot menu", L"pclinux,linux", "PCLinux" },
//  { L"\\EFI\\neon\\grubx64.efi", L"KDE Neon EFI boot menu", L"neon,linux", "KDE Neon" },
//  { L"\\EFI\\MX19\\grubx64.efi", L"MX Linux EFI boot menu", L"mx,linux", "MX Linux" },
//  { L"\\EFI\\parrot\\grubx64.efi", L"Parrot OS EFI boot menu", L"parrot,linux", "Parrot OS" },
#else
  //dont bother about 32bit compilation
  { L"\\EFI\\grub\\grub.efi", L"Grub EFI boot menu", L"grub,linux" },
  { L"\\EFI\\Gentoo\\grub.efi", L"Gentoo EFI boot menu", L"gentoo,linux", "Gentoo" },
  { L"\\EFI\\Gentoo\\kernel.efi", L"Gentoo EFI kernel", L"gentoo,linux" },
  { L"\\EFI\\RedHat\\grub.efi", L"RedHat EFI boot menu", L"redhat,linux", "Redhat" },
  { L"\\EFI\\debian\\grub.efi", L"Debian EFI boot menu", L"debian,linux", "Debian" },
  { L"\\EFI\\kali\\grub.efi", L"Kali EFI boot menu", L"kali,linux", "Kali" },
  { L"\\EFI\\ubuntu\\grub.efi", L"Ubuntu EFI boot menu", L"ubuntu,linux", "Ubuntu" },
  { L"\\EFI\\kubuntu\\grub.efi", L"kubuntu EFI boot menu", L"kubuntu,linux", "kubuntu" },
  { L"\\EFI\\LinuxMint\\grub.efi", L"Linux Mint EFI boot menu", L"mint,linux", "Linux Mint" },
  { L"\\EFI\\Fedora\\grub.efi", L"Fedora EFI boot menu", L"fedora,linux", "Fedora" },
  { L"\\EFI\\opensuse\\grub.efi", L"OpenSuse EFI boot menu", L"suse,linux", "openSUSE" },
  { L"\\EFI\\arch\\grub.efi", L"ArchLinux EFI boot menu", L"arch,linux" },
  { L"\\EFI\\arch_grub\\grub.efi", L"ArchLinux EFI boot menu", L"arch,linux" },
  { L"\\EFI\\ORACLE\\grub.efi", L"Oracle Solaris EFI boot menu", L"solaris,linux", "Solaris" },
  { L"\\EFI\\Endless\\grub.efi", L"EndlessOS EFI boot menu", L"endless,linux", "EndlessOS" },
  { L"\\EFI\\antergos_grub\\grub.efi", L"Antergos EFI boot menu", L"antergos,linux", "Antergos" },
  { L"\\EFI\\Deepin\\grub.efi", L"Deepin EFI boot menu", L"deepin,linux", "Deepin" },
  { L"\\EFI\\elementary\\grub.efi", L"Elementary EFI boot menu", L"eos,linux", "Elementary" },
  { L"\\EFI\\Manjaro\\grub.efi", L"Manjaro EFI boot menu", L"manjaro,linux", "Manjaro" },
  { L"\\EFI\\xubuntu\\grub.efi", L"Xubuntu EFI boot menu", L"xubuntu,linux", "Xubuntu" },
  { L"\\EFI\\zorin\\grub.efi", L"Zorin EFI boot menu", L"zorin,linux", "Zorin" },
  { L"\\EFI\\goofiboot\\goofiboot.efi", L"Solus EFI boot menu", L"solus,linux", "Solus" },
  { L"\\EFI\\centos\\grub.efi", L"CentOS EFI boot menu", L"centos,linux", "CentOS" },
  { L"\\EFI\\pclinuxos\\grub.efi", L"PCLinuxOS EFI boot menu", L"pclinux,linux", "PCLinux" },
  { L"\\EFI\\neon\\grub.efi", L"KDE Neon EFI boot menu", L"neon,linux", "KDE Neon" },
  { L"\\EFI\\MX19\\grub.efi", L"MX Linux EFI boot menu", L"mx,linux", "MX Linux" },
  { L"\\EFI\\parrot\\grub.efi", L"Parrot OS EFI boot menu", L"parrot,linux", "Parrot OS" },
#endif
  { L"\\EFI\\SuSe\\elilo.efi"_XSW, L"OpenSuse EFI boot menu"_XSW, L"suse,linux"_XSW },
};
STATIC CONST UINTN LinuxEntryDataCount = (sizeof(LinuxEntryData) / sizeof(LinuxEntryData[0]));

#if defined(ANDX86)
#if !defined(MDE_CPU_X64)
#undef ANDX86
#else
// ANDX86 loader path data
#define ANDX86_FINDLEN 3
typedef struct ANDX86_PATH_DATA
{
   CONST XStringW Path;
   CONST XStringW Title;
   CONST XStringW Icon;
   CONST XStringW Find[ANDX86_FINDLEN];
} ANDX86_PATH_DATA;

STATIC ANDX86_PATH_DATA AndroidEntryData[] = {
  //{ L"\\EFI\\boot\\grubx64.efi", L"Grub", L"grub,linux" },
  //{ L"\\EFI\\boot\\bootx64.efi", L"Grub", L"grub,linux" },
  { L"\\EFI\\remixos\\grubx64.efi"_XSW,         L"Remix"_XSW,     L"remix,grub,linux"_XSW,   { L"\\isolinux\\isolinux.bin"_XSW, L"\\initrd.img"_XSW, L"\\kernel"_XSW } },
  { L"\\EFI\\PhoenixOS\\boot\\grubx64.efi"_XSW, L"PhoenixOS"_XSW, L"phoenix,grub,linux"_XSW, { L"\\EFI\\PhoenixOS\\boot\\efi.img"_XSW, L"\\EFI\\PhoenixOS\\initrd.img"_XSW, L"\\EFI\\PhoenixOS\\kernel"_XSW } },
  { L"\\EFI\\boot\\grubx64.efi"_XSW,            L"Phoenix"_XSW,   L"phoenix,grub,linux"_XSW, { L"\\phoenix\\kernel"_XSW, L"\\phoenix\\initrd.img"_XSW, L"\\phoenix\\ramdisk.img"_XSW } },
  { L"\\EFI\\boot\\bootx64.efi"_XSW,            L"Chrome"_XSW,    L"chrome,grub,linux"_XSW,  { L"\\syslinux\\vmlinuz.A"_XSW, L"\\syslinux\\vmlinuz.B"_XSW, L"\\syslinux\\ldlinux.sys"_XSW} },

};
STATIC CONST UINTN AndroidEntryDataCount = (sizeof(AndroidEntryData) / sizeof(ANDX86_PATH_DATA));
#endif
#endif

CONST XStringW PaperBoot   = L"\\com.apple.boot.P\\boot.efi"_XSW;
CONST XStringW RockBoot    = L"\\com.apple.boot.R\\boot.efi"_XSW;
CONST XStringW ScissorBoot = L"\\com.apple.boot.S\\boot.efi"_XSW;

// OS X installer paths
CONST XStringW OSXInstallerPaths[] = {
  L"\\.IABootFiles\\boot.efi"_XSW, // 10.9 - 10.13.3
  L"\\Mac OS X Install Data\\boot.efi"_XSW, // 10.7
  L"\\OS X Install Data\\boot.efi"_XSW, // 10.8 - 10.11
  L"\\macOS Install Data\\boot.efi"_XSW, // 10.12 - 10.12.3
  L"\\macOS Install Data\\Locked Files\\Boot Files\\boot.efi"_XSW, // 10.12.4-10.15
  L"\\macOS Install Data\\Locked Files\\boot.efi"_XSW // 10.16+
};

STATIC CONST UINTN OSXInstallerPathsCount = (sizeof(OSXInstallerPaths) / sizeof(OSXInstallerPaths[0]));

STATIC INTN TimeCmp(IN EFI_TIME *Time1,
                    IN EFI_TIME *Time2)
{
   INTN Comparison;
   if (Time1 == NULL) {
     if (Time2 == NULL) {
       return 0;
     } else {
       return -1;
     }
   } else if (Time2 == NULL) {
     return 1;
   }
   Comparison = Time1->Year - Time2->Year;
   if (Comparison == 0) {
     Comparison = Time1->Month - Time2->Month;
     if (Comparison == 0) {
       Comparison = Time1->Day - Time2->Day;
       if (Comparison == 0) {
         Comparison = Time1->Hour - Time2->Hour;
         if (Comparison == 0) {
           Comparison = Time1->Minute - Time2->Minute;
           if (Comparison == 0) {
             Comparison = Time1->Second - Time2->Second;
             if (Comparison == 0) {
               Comparison = Time1->Nanosecond - Time2->Nanosecond;
             }
           }
         }
       }
     }
   }
   return Comparison;
}

UINT8 GetOSTypeFromPath(IN CONST XStringW& Path)
{
  if (Path.isEmpty()) {
    return OSTYPE_OTHER;
  }
  if ( Path.equalIC(MACOSX_LOADER_PATH)) {
    return OSTYPE_OSX;
  } else if ( Path.equalIC(OSXInstallerPaths[0]) ||
             ( Path.equalIC(OSXInstallerPaths[1])) ||
             ( Path.equalIC(OSXInstallerPaths[2])) ||
             ( Path.equalIC(OSXInstallerPaths[3])) ||
             ( Path.equalIC(OSXInstallerPaths[4])) ||
             ( Path.equalIC(RockBoot)) || ( Path.equalIC(PaperBoot)) || ( Path.equalIC(ScissorBoot)) ||
             (! Path.equalIC(L"\\.IABootFiles\\boot.efi") &&  Path.equalIC(L"\\.IAPhysicalMedia") &&  Path.equalIC(MACOSX_LOADER_PATH))
             ) {
    return OSTYPE_OSX_INSTALLER;
  } else if ( Path.equalIC(L"\\com.apple.recovery.boot\\boot.efi")) {
    return OSTYPE_RECOVERY;
  } else if (( Path.equalIC(L"\\EFI\\Microsoft\\Boot\\bootmgfw-orig.efi")) || //test first as orig
             ( Path.equalIC(L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi")) ||      //it can be Clover
    //         ( Path.equalIC(L"\\bootmgr.efi")) || //never worked, just extra icon in menu
             ( Path.equalIC(L"\\EFI\\MICROSOFT\\BOOT\\cdboot.efi"))) {
    return OSTYPE_WINEFI;
  } else if (LINUX_FULL_LOADER_PATH.equalIC(Path)) {
    return OSTYPE_LINEFI;
  } else if ( Path.containsIC("grubx64.efi") ) {
    return OSTYPE_LINEFI;
  } else {
    UINTN Index;
#if defined(ANDX86)
    Index = 0;
    while (Index < AndroidEntryDataCount) {
      if ( Path.equalIC(AndroidEntryData[Index].Path) ) {
        return OSTYPE_LIN;
      }
      ++Index;
    }
#endif
    Index = 0;
    while (Index < LinuxEntryDataCount) {
      if ( Path.equalIC(LinuxEntryData[Index].Path) ) {
        return OSTYPE_LIN;
      }
      ++Index;
    }
  }
  return OSTYPE_OTHER;
}

static const XStringW linux = L"linux"_XSW;

STATIC CONST XStringW& LinuxIconNameFromPath(IN CONST XStringW& Path,
                       IN EFI_FILE_PROTOCOL *RootDir)
{
  UINTN Index;
#if defined(ANDX86)
  Index = 0;
  while (Index < AndroidEntryDataCount) {
    if ( Path.equalIC(AndroidEntryData[Index].Path) ) {
      return AndroidEntryData[Index].Icon;
    }
    ++Index;
  }
#endif
  
  //check not common names
  Index = 0;
  while (Index < LinuxEntryDataCount) {
    if ( Path.equalIC(LinuxEntryData[Index].Path) ) {
      return LinuxEntryData[Index].Icon;
    }
    ++Index;
  }
  
  // Try to open the linux issue
  if ((RootDir != NULL) && LINUX_FULL_LOADER_PATH.equalIC(Path)) {
    CHAR8 *Issue = NULL;
    UINTN  IssueLen = 0;
    if (!EFI_ERROR(egLoadFile(RootDir, LINUX_ISSUE_PATH.wc_str(), (UINT8 **)&Issue, &IssueLen)) && (Issue != NULL)) {
      if (IssueLen > 0) {
        for (Index = 0; Index < LinuxEntryDataCount; ++Index) {
          if ( LinuxEntryData[Index].Issue.notEmpty()  &&  AsciiStrStr(Issue, LinuxEntryData[Index].Issue.c_str()) != NULL ) {
            FreePool(Issue);
            return LinuxEntryData[Index].Icon;
          }
        }
      }
      FreePool(Issue);
    }
  }
  return linux;
}

STATIC CONST XString8 LinuxInitImagePath[] = {
   "initrd%s"_XS8,
   "initrd.img%s"_XS8,
   "initrd%s.img"_XS8,
   "initramfs%s"_XS8,
   "initramfs.img%s"_XS8,
   "initramfs%s.img"_XS8,
};
STATIC CONST UINTN LinuxInitImagePathCount = (sizeof(LinuxInitImagePath) / sizeof(LinuxInitImagePath[0]));

STATIC XString8Array LinuxKernelOptions(IN EFI_FILE_PROTOCOL *Dir,
                                  IN CONST CHAR16            *Version,
                                  IN CONST CHAR16            *PartUUID,
                                  IN CONST XString8Array&           Options OPTIONAL)
{
  UINTN Index = 0;
  if ((Dir == NULL) || (PartUUID == NULL)) {
    return Options;
  }
  while (Index < LinuxInitImagePathCount) {
    XStringW InitRd = SWPrintf(LinuxInitImagePath[Index++].c_str(), (Version == NULL) ? L"" : Version);
    if (InitRd.notEmpty()) {
      if (FileExists(Dir, InitRd)) {
        XString8Array CustomOptions;
        CustomOptions.Add(S8Printf("root=/dev/disk/by-partuuid/%ls", PartUUID));
        CustomOptions.Add(S8Printf("initrd=%ls\\%ls", LINUX_BOOT_ALT_PATH, InitRd.wc_str()));
        CustomOptions.import(LINUX_DEFAULT_OPTIONS);
        CustomOptions.import(Options);
        return CustomOptions;
      }
    }
  }
  XString8Array CustomOptions;
  CustomOptions.Add(S8Printf("root=/dev/disk/by-partuuid/%ls", PartUUID));
  CustomOptions.import(LINUX_DEFAULT_OPTIONS);
  CustomOptions.import(Options);
  return CustomOptions;
}

STATIC BOOLEAN isFirstRootUUID(REFIT_VOLUME *Volume)
{
  UINTN         VolumeIndex;
  REFIT_VOLUME *scanedVolume;

  for (VolumeIndex = 0; VolumeIndex < Volumes.size(); VolumeIndex++) {
    scanedVolume = &Volumes[VolumeIndex];

    if (scanedVolume == Volume)
      return TRUE;

    if (CompareGuid(&scanedVolume->RootUUID, &Volume->RootUUID))
      return FALSE;

  }
  return TRUE;
}

//Set Entry->VolName to .disk_label.contentDetails if it exists
STATIC EFI_STATUS GetOSXVolumeName(LOADER_ENTRY *Entry)
{
  EFI_STATUS  Status = EFI_NOT_FOUND;
  CONST CHAR16* targetNameFile = L"\\System\\Library\\CoreServices\\.disk_label.contentDetails";
  CHAR8*  fileBuffer;
  UINTN   fileLen = 0;
  if(FileExists(Entry->Volume->RootDir, targetNameFile)) {
    Status = egLoadFile(Entry->Volume->RootDir, targetNameFile, (UINT8 **)&fileBuffer, &fileLen);
    if(!EFI_ERROR(Status)) {
//      CHAR16  *tmpName;
      //Create null terminated string
//      targetString = (CHAR8*) A_llocateZeroPool(fileLen+1);
//      CopyMem( (VOID*)targetString, (VOID*)fileBuffer, fileLen);
//      DBG("found disk_label with contents:%s\n", targetString);

      //      NOTE: Sothor - This was never run. If we need this correct it and uncomment it.
      //      if (Entry->LoaderType == OSTYPE_OSX) {
      //        INTN i;
      //        //remove occurence number. eg: "vol_name 2" --> "vol_name"
      //        i=fileLen-1;
      //        while ((i>0) && (targetString[i]>='0') && (targetString[i]<='9')) {
      //          i--;
      //        }
      //        if (targetString[i] == ' ') {
      //          targetString[i] = 0;
      //        }
      //      }

//      //Convert to Unicode
//      tmpName = (CHAR16*)A_llocateZeroPool((fileLen+1)*2);
//      AsciiStrToUnicodeStrS(targetString, tmpName, fileLen);

      Entry->VolName.strncpy(fileBuffer, fileLen);
      DBG("Created name:%ls\n", Entry->VolName.wc_str());

      FreePool(fileBuffer);
    }
  }
  return Status;
}
STATIC LOADER_ENTRY *CreateLoaderEntry(IN CONST XStringW& LoaderPath,
                                       IN CONST XString8Array& LoaderOptions,
                                       IN CONST XStringW& FullTitle,
                                       IN CONST XStringW& LoaderTitle,
                                       IN REFIT_VOLUME *Volume,
                                       IN const XStringW& APFSTargetUUID,
                                       IN XIcon *Image,
                                       IN XIcon *DriveImage,
                                       IN UINT8 OSType,
                                       IN UINT8 Flags,
                                       IN CHAR16 Hotkey,
                                       EFI_GRAPHICS_OUTPUT_BLT_PIXEL BootBgColor,
                                       IN UINT8 CustomBoot,
                                       IN XImage *CustomLogo,
                                       IN const KERNEL_AND_KEXT_PATCHES* Patches,
                                       IN BOOLEAN CustomEntry)
{
  EFI_DEVICE_PATH       *LoaderDevicePath;
  XStringW               LoaderDevicePathString;
  XStringW               FilePathAsString;
//  CONST CHAR16          *OSIconName = NULL;
  CHAR16                ShortcutLetter;
  LOADER_ENTRY          *Entry;
  CONST CHAR8           *indent = "    ";

  // Check parameters are valid
  if ((LoaderPath.isEmpty()) || (Volume == NULL)) {
    return NULL;
  }

  // Get the loader device path
  LoaderDevicePath = FileDevicePath(Volume->DeviceHandle, LoaderPath);
  if (LoaderDevicePath == NULL) {
    return NULL;
  }
  LoaderDevicePathString = FileDevicePathToXStringW(LoaderDevicePath);
  if (LoaderDevicePathString.isEmpty()) {
    return NULL;
  }

  // Ignore this loader if it's self path
  FilePathAsString = FileDevicePathToXStringW(SelfFullDevicePath);
  if (FilePathAsString.notEmpty()) {
    INTN Comparison = StriCmp(FilePathAsString.wc_str(), LoaderDevicePathString.wc_str());
    if (Comparison == 0) {
      DBG("%s skipped because path `%ls` is self path!\n", indent, LoaderDevicePathString.wc_str());
      return NULL;
    }
  }
// DBG("OSType =%d\n", OSType);
  if (!CustomEntry) {
    CUSTOM_LOADER_ENTRY *Custom;
    UINTN                CustomIndex = 0;

    // Ignore this loader if it's device path is already present in another loader
      for (UINTN i = 0; i < MainMenu.Entries.size(); ++i) {
        REFIT_ABSTRACT_MENU_ENTRY& MainEntry = MainMenu.Entries[i];
        // Only want loaders
        if (MainEntry.getLOADER_ENTRY()) {
          if (StriCmp(MainEntry.getLOADER_ENTRY()->DevicePathString.wc_str(), LoaderDevicePathString.wc_str()) == 0) {
            DBG("%s skipped because path `%ls` already exists for another entry!\n", indent, LoaderDevicePathString.wc_str());
            return NULL;
          }
        }
      }
    // If this isn't a custom entry make sure it's not hidden by a custom entry
    Custom = gSettings.CustomEntries;
    while (Custom) {
      // Check if the custom entry is hidden or disabled
      if ((OSFLAG_ISSET(Custom->Flags, OSFLAG_DISABLED) ||
           (OSFLAG_ISSET(Custom->Flags, OSFLAG_HIDDEN) && !gSettings.ShowHiddenEntries))) {

        INTN volume_match=0;
        INTN volume_type_match=0;
        INTN path_match=0;
        INTN type_match=0;

        // Check if volume match
        if (Custom->Volume.notEmpty()) {
          // Check if the string matches the volume
          volume_match =
            ((StrStr(Volume->DevicePathString.wc_str(), Custom->Volume.wc_str()) != NULL) ||
             ((Volume->VolName.notEmpty()) && (StrStr(Volume->VolName.wc_str(), Custom->Volume.wc_str()) != NULL))) ? 1 : -1;
        }

        // Check if the volume_type match
        if (Custom->VolumeType != 0) {
          volume_type_match = (((1ull<<Volume->DiskKind) & Custom->VolumeType) != 0) ? 1 : -1;
        }

        // Check if the path match
        if (Custom->Path.notEmpty()) {
          // Check if the loader path match
          path_match = (Custom->Path.equalIC(LoaderPath)) ? 1 : -1;
        }

        // Check if the type match
        if (Custom->Type != 0) {
          type_match = OSTYPE_COMPARE(Custom->Type, OSType) ? 1 : -1;
        }

        if (volume_match == -1 || volume_type_match == -1 || path_match == -1 || type_match == -1 ) {
          UINTN add_comma = 0;

          DBG("%sNot match custom entry %llu: ", indent, CustomIndex);
          if (volume_match != 0) {
            DBG("Volume: %ls", volume_match == 1 ? L"match" : L"not match");
            add_comma++;
          }
          if (path_match != 0) {
            DBG("%lsPath: %ls",
                (add_comma ? L", " : L""),
                path_match == 1 ? L"match" : L"not match");
            add_comma++;
          }
          if (volume_type_match != 0) {
            DBG("%lsVolumeType: %ls",
                (add_comma ? L", " : L""),
                volume_type_match == 1 ? L"match" : L"not match");
            add_comma++;
          }
          if (type_match != 0) {
            DBG("%lsType: %ls",
                (add_comma ? L", " : L""),
                type_match == 1 ? L"match" : L"not match");
          }
          DBG("\n");
        } else {
          // Custom entry match
          DBG("%sSkipped because matching custom entry %llu!\n", indent, CustomIndex);
          return NULL;
        }
      }
      Custom = Custom->Next;
      ++CustomIndex;
    }
  }
// DBG("prepare the menu entry\n");
  // prepare the menu entry
  Entry = new LOADER_ENTRY();
  Entry->Row = 0;
  Entry->Volume = Volume;
  Entry->APFSTargetUUID = APFSTargetUUID;

  Entry->LoaderPath       = LoaderPath;
  Entry->VolName          = Volume->VolName;
  Entry->DevicePath       = LoaderDevicePath;
  Entry->DevicePathString = LoaderDevicePathString;
  Entry->Flags            = OSFLAG_SET(Flags, OSFLAG_USEGRAPHICS);

  if (OSFLAG_ISSET(Flags, OSFLAG_NODEFAULTARGS)) {
    Entry->LoadOptions  = LoaderOptions;
  }else{
    Entry->LoadOptions = Split<XString8Array>(gSettings.BootArgs, " ");
    Entry->LoadOptions.import(LoaderOptions);
  }
  //actions
  Entry->AtClick = ActionSelect;
  Entry->AtDoubleClick = ActionEnter;
  Entry->AtRightClick = ActionDetails;
  Entry->CustomBoot = CustomBoot;
  if (CustomLogo != nullptr) {
    Entry->CustomLogo = *CustomLogo; //else empty ximage already constructed
  }

  Entry->LoaderType = OSType;
  Entry->BuildVersion.setEmpty();
  Entry->OSVersion = GetOSVersion(Entry);
//DBG("OSVersion=%s \n", Entry->OSVersion);
  // detect specific loaders
  XStringW OSIconName;
  ShortcutLetter = 0;

  switch (OSType) {
    case OSTYPE_OSX:
    case OSTYPE_RECOVERY:
    case OSTYPE_OSX_INSTALLER:
      OSIconName = GetOSIconName(Entry->OSVersion);// Sothor - Get OSIcon name using OSVersion
      // apianti - force custom logo even when verbose
/* this is not needed, as this flag is also being unset when booting if -v is present (LoadOptions may change until then)
      if ( Entry->LoadOptions.containsIC("-v")  ) {
        // OSX is not booting verbose, so we can set console to graphics mode
        Entry->Flags = OSFLAG_UNSET(Entry->Flags, OSFLAG_USEGRAPHICS);
      }
*/
      if (OSType == OSTYPE_OSX && IsOsxHibernated(Entry)) {
        Entry->Flags = OSFLAG_SET(Entry->Flags, OSFLAG_HIBERNATED);
        DBG("  =>set entry as hibernated\n");
      }
      //always unset checkFakeSmc for installer
      if (OSType == OSTYPE_OSX_INSTALLER){
        Entry->Flags = OSFLAG_UNSET(Entry->Flags, OSFLAG_CHECKFAKESMC);
        Entry->Flags = OSFLAG_SET(Entry->Flags, OSFLAG_WITHKEXTS);
      }
      ShortcutLetter = 'M';
      if ( Entry->VolName.isEmpty() ) {
        // else no sense to override it with dubious name
        GetOSXVolumeName(Entry);
      }
      break;
    case OSTYPE_WIN:
      OSIconName = L"win"_XSW;
      ShortcutLetter = 'W';
      break;
    case OSTYPE_WINEFI:
      OSIconName = L"vista,win"_XSW;
      //ShortcutLetter = 'V';
      ShortcutLetter = 'W';
      break;
    case OSTYPE_LIN:
    case OSTYPE_LINEFI:
    // we already detected linux and have Path and Image
      Entry->LoaderType = OSType;
      OSIconName = L"linux"_XSW;
      if (Image == nullptr) {
        DBG(" linux image not found\n");
        OSIconName = LinuxIconNameFromPath(LoaderPath, Volume->RootDir); //something named "issue"
      }
      ShortcutLetter = 'L';
      break;
    //case OSTYPE_OTHER: 
    case OSTYPE_EFI:
      OSIconName = L"clover"_XSW;
      ShortcutLetter = 'E';
      Entry->LoaderType = OSTYPE_OTHER;
      break;
    default:
      OSIconName = L"unknown"_XSW;
      Entry->LoaderType = OSTYPE_OTHER;
      break;
  }
//DBG("OSIconName=%ls \n", OSIconName);
  Entry->Title = FullTitle;
  if (Entry->Title.isEmpty()  &&  Volume->VolLabel.notEmpty()) {
    if (Volume->VolLabel[0] == L'#') {
      Entry->Title.SWPrintf("Boot %ls from %ls", (!LoaderTitle.isEmpty()) ? LoaderTitle.wc_str() : LoaderPath.basename().wc_str(), Volume->VolLabel.data(1));
    }else{
      Entry->Title.SWPrintf("Boot %ls from %ls", (!LoaderTitle.isEmpty()) ? LoaderTitle.wc_str() : LoaderPath.basename().wc_str(), Volume->VolLabel.wc_str());
    }
  }

  BOOLEAN BootCampStyle = ThemeX.BootCampStyle;

  if ( Entry->Title.isEmpty()  &&  Entry->VolName.isEmpty() ) {
    XStringW BasenameXW = XStringW(Basename(Volume->DevicePathString.wc_str()));
 //   DBG("encounter Entry->VolName ==%ls and StrLen(Entry->VolName) ==%llu\n",Entry->VolName, StrLen(Entry->VolName));
    if (BootCampStyle) {
      if (!LoaderTitle.isEmpty()) {
        Entry->Title = LoaderTitle;
      } else {
        Entry->Title = (BasenameXW.contains(L"-")) ? BasenameXW.subString(0,BasenameXW.indexOf(L"-") + 1) + L"..)" : BasenameXW;
      }
    } else {
      Entry->Title.SWPrintf("Boot %ls from %ls", (!LoaderTitle.isEmpty()) ? LoaderTitle.wc_str() : LoaderPath.basename().wc_str(),
                            (BasenameXW.contains(L"-")) ? (BasenameXW.subString(0,BasenameXW.indexOf(L"-") + 1) + L"..)").wc_str() : BasenameXW.wc_str());
    }
  }
//  DBG("check Entry->Title \n");
  if ( Entry->Title.isEmpty() ) {
 //   DBG("encounter LoaderTitle ==%ls and Entry->VolName ==%ls\n", LoaderTitle.wc_str(), Entry->VolName);
    if (BootCampStyle) {
      if ((StriCmp(LoaderTitle.wc_str(), L"macOS") == 0) || (StriCmp(LoaderTitle.wc_str(), L"Recovery") == 0)) {
        Entry->Title.takeValueFrom(Entry->VolName);
      } else {
        if (!LoaderTitle.isEmpty()) {
          Entry->Title = LoaderTitle;
        } else {
          Entry->Title = LoaderPath.basename();
        }
      }
    } else {
      Entry->Title.SWPrintf("Boot %ls from %ls", (!LoaderTitle.isEmpty()) ? LoaderTitle.wc_str() : LoaderPath.basename().wc_str(),
                            Entry->VolName.wc_str());
    }
  }
//  DBG("Entry->Title =%ls\n", Entry->Title.wc_str());
  // just an example that UI can show hibernated volume to the user
  // should be better to show it on entry image
  if (OSFLAG_ISSET(Entry->Flags, OSFLAG_HIBERNATED)) {
    Entry->Title.SWPrintf("%ls (hibernated)", Entry->Title.s());
  }

  Entry->ShortcutLetter = (Hotkey == 0) ? ShortcutLetter : Hotkey;

  // get custom volume icon if present
  if (GlobalConfig.CustomIcons && FileExists(Volume->RootDir, L"\\.VolumeIcon.icns")){
    Entry->Image.Image.LoadIcns(Volume->RootDir, L"\\.VolumeIcon.icns", 128);
    if (!Entry->Image.Image.isEmpty()) {
      Entry->Image.setFilled();
      DBG("using VolumeIcon.icns image from Volume\n");
    }    
  } else if (Image) {
    Entry->Image = *Image; //copy image from temporary storage
  } else {
    Entry->Image = ThemeX.LoadOSIcon(OSIconName);
  }
//  DBG("Load DriveImage\n");
  // Load DriveImage
  if (DriveImage) {
//    DBG("DriveImage presents\n");
    Entry->DriveImage = *DriveImage;
  } else {
    Entry->DriveImage = ScanVolumeDefaultIcon(Volume, Entry->LoaderType, Volume->DevicePath);
  }
//   DBG("HideBadges=%llu Volume=%ls ", ThemeX.HideBadges, Volume->VolName);
  if (ThemeX.HideBadges & HDBADGES_SHOW) {
    if (ThemeX.HideBadges & HDBADGES_SWAP) {
      Entry->BadgeImage.Image = XImage(Entry->DriveImage.Image, 0);
       DBG(" Show badge as Drive.");
    } else {
      Entry->BadgeImage.Image = XImage(Entry->Image.Image, 0);
       DBG(" Show badge as OSImage.");
    }
    if (!Entry->BadgeImage.Image.isEmpty()) {
      Entry->BadgeImage.setFilled();
    }
  }
  Entry->BootBgColor = BootBgColor;

//  Entry->KernelAndKextPatches = ((Patches == NULL) ? (KERNEL_AND_KEXT_PATCHES *)(((UINTN)&gSettings) + OFFSET_OF(SETTINGS_DATA, KernelAndKextPatches)) : Patches);
//  CopyKernelAndKextPatches(&Entry->KernelAndKextPatches, Patches == NULL ? &gSettings.KernelAndKextPatches : Patches);
  Entry->KernelAndKextPatches = Patches == NULL ? gSettings.KernelAndKextPatches : *Patches;
  
#ifdef DUMP_KERNEL_KEXT_PATCHES
  DumpKernelAndKextPatches(Entry->KernelAndKextPatches);
#endif
  DBG("%sLoader entry created for '%ls'\n", indent, Entry->DevicePathString.wc_str());
  return Entry;
}

void LOADER_ENTRY::AddDefaultMenu()
{
  XStringW     FileName;
//  CHAR16* TempOptions;
//  CHAR16            DiagsFileName[256];
  LOADER_ENTRY      *SubEntry;
//  REFIT_MENU_SCREEN *SubScreen;
//  REFIT_VOLUME      *Volume;
  UINT64            VolumeSize;
  EFI_GUID          *Guid = NULL;
  BOOLEAN           KernelIs64BitOnly;
  UINT64            os_version = AsciiOSVersionToUint64(OSVersion);

  constexpr LString8 quietLitteral = "quiet";
  constexpr LString8 splashLitteral = "splash";

  // Only kernels up to 10.7 have 32-bit mode
  KernelIs64BitOnly = (OSVersion.isEmpty() ||
                       os_version >= AsciiOSVersionToUint64("10.8"_XS8));
  
  const char* macOS = (os_version < AsciiOSVersionToUint64("10.8"_XS8))? "Mac OS X" :
                      (os_version < AsciiOSVersionToUint64("10.12"_XS8))? "OS X" : "macOS";

  FileName = LoaderPath.basename();

  // create the submenu
  SubScreen = new REFIT_MENU_SCREEN;
  SubScreen->Title.SWPrintf("Options for %ls on %ls", Title.wc_str(), VolName.wc_str());

  SubScreen->TitleImage = Image;
  SubScreen->ID = LoaderType + 20; //wow
//    DBG("get anime for os=%lld\n", SubScreen->ID);
  SubScreen->GetAnime();
  VolumeSize = RShiftU64(MultU64x32(Volume->BlockIO->Media->LastBlock, Volume->BlockIO->Media->BlockSize), 20);
  SubScreen->AddMenuInfoLine_f("Volume size: %lluMb", VolumeSize);
  SubScreen->AddMenuInfoLine_f("%ls", FileDevicePathToXStringW(DevicePath).wc_str());
  Guid = FindGPTPartitionGuidInDevicePath(Volume->DevicePath);
  if (Guid) {
    SubScreen->AddMenuInfoLine_f("UUID: %s", strguid(Guid));
  }
  if ( Volume->ApfsFileSystemUUID.notEmpty() ||  APFSTargetUUID.notEmpty() ) {
    SubScreen->AddMenuInfoLine_f("APFS volume name: %ls", VolName.wc_str());
  }
  if ( Volume->ApfsFileSystemUUID.notEmpty() ) {
    SubScreen->AddMenuInfoLine_f("APFS File System UUID: %s", Volume->ApfsFileSystemUUID.c_str());
  }
  if ( APFSTargetUUID.notEmpty() ) {
    SubScreen->AddMenuInfoLine_f("APFS Target UUID: %ls", APFSTargetUUID.wc_str());
  }
  SubScreen->AddMenuInfoLine_f("Options: %s", LoadOptions.ConcatAll(" "_XS8).c_str());
  // loader-specific submenu entries
  if (LoaderType == OSTYPE_OSX ||
      LoaderType == OSTYPE_OSX_INSTALLER ||
      LoaderType == OSTYPE_RECOVERY) { // entries for Mac OS X
    SubScreen->AddMenuInfoLine_f("%s: %s", macOS, OSVersion.c_str());

    if (OSFLAG_ISSET(Flags, OSFLAG_HIBERNATED)) {
      SubEntry = getPartiallyDuplicatedEntry();
      if (SubEntry) {
        SubEntry->Title.takeValueFrom("Cancel hibernate wake");
        SubEntry->Flags     = OSFLAG_UNSET(SubEntry->Flags, OSFLAG_HIBERNATED);
        SubScreen->AddMenuEntry(SubEntry, true);
      }
    }

    SubEntry = getPartiallyDuplicatedEntry();
    if (SubEntry) {
      SubEntry->Title.SWPrintf("Boot %s with selected options", macOS);
      SubScreen->AddMenuEntry(SubEntry, true);
    }
    
    SubEntry = getPartiallyDuplicatedEntry();
    if (SubEntry) {
      SubEntry->Title.SWPrintf("Boot %s with injected kexts", macOS);
      SubEntry->Flags       = OSFLAG_UNSET(SubEntry->Flags, OSFLAG_CHECKFAKESMC);
      SubEntry->Flags       = OSFLAG_SET(SubEntry->Flags, OSFLAG_WITHKEXTS);
      SubScreen->AddMenuEntry(SubEntry, true);
    }

    SubEntry = getPartiallyDuplicatedEntry();
    if (SubEntry) {
      SubEntry->Title.SWPrintf("Boot %s without injected kexts", macOS);
      SubEntry->Flags       = OSFLAG_UNSET(SubEntry->Flags, OSFLAG_CHECKFAKESMC);
      SubEntry->Flags       = OSFLAG_UNSET(SubEntry->Flags, OSFLAG_WITHKEXTS);
      SubScreen->AddMenuEntry(SubEntry, true);
    }

    SubScreen->AddMenuEntry(SubMenuKextInjectMgmt(), true);
    SubScreen->AddMenuInfo_f("=== boot-args ===");
    if (!KernelIs64BitOnly) {
      if (os_version < AsciiOSVersionToUint64("10.8"_XS8)) {
        SubScreen->AddMenuCheck("Mac OS X 32bit",   OPT_I386, 68);
      }
//      SubScreen->AddMenuCheck(XString8().SPrintf("%s 64bit", macOS).c_str(), OPT_X64,  68);
      SubScreen->AddMenuCheck((macOS + " 64bit"_XS8).c_str(), OPT_X64,  68);
    }
    SubScreen->AddMenuCheck("Verbose (-v)",                               OPT_VERBOSE, 68);
    // No Caches option works on 10.6 - 10.9
    if (os_version < AsciiOSVersionToUint64("10.10"_XS8)) {
      SubScreen->AddMenuCheck("Without caches (-f)",                        OPT_NOCACHES, 68);
    }
    SubScreen->AddMenuCheck("Single User (-s)",                           OPT_SINGLE_USER, 68);
    SubScreen->AddMenuCheck("Safe Mode (-x)",                             OPT_SAFE, 68);
    SubScreen->AddMenuCheck("Disable KASLR (slide=0)",                    OPT_SLIDE, 68);
    SubScreen->AddMenuCheck("Set Nvidia to VESA (nv_disable=1)",          OPT_NVDISABLE, 68);
    SubScreen->AddMenuCheck("Use Nvidia WEB drivers (nvda_drv=1)",        OPT_NVWEBON, 68);
    SubScreen->AddMenuCheck("Disable PowerNap (darkwake=0)",              OPT_POWERNAPOFF, 68);
    SubScreen->AddMenuCheck("Use XNU CPUPM (-xcpm)",                      OPT_XCPM, 68);
//    SubScreen->AddMenuCheck("Disable Intel Idle Mode (-gux_no_idle)",     OPT_GNOIDLE, 68);
//    SubScreen->AddMenuCheck("Sleep Uses Shutdown (-gux_nosleep)",         OPT_GNOSLEEP, 68);
//    SubScreen->AddMenuCheck("Force No Msi Int (-gux_nomsi)",              OPT_GNOMSI, 68);
//    SubScreen->AddMenuCheck("EHC manage USB2 ports (-gux_defer_usb2)",    OPT_EHCUSB, 68);
    SubScreen->AddMenuCheck("Keep symbols on panic (keepsyms=1)",         OPT_KEEPSYMS, 68);
    SubScreen->AddMenuCheck("Don't reboot on panic (debug=0x100)",        OPT_DEBUG, 68);
    SubScreen->AddMenuCheck("Debug kexts (kextlog=0xffff)",               OPT_KEXTLOG, 68);
//    SubScreen->AddMenuCheck("Disable AppleALC (-alcoff)",                 OPT_APPLEALC, 68);
//    SubScreen->AddMenuCheck("Disable Shiki (-shikioff)",                  OPT_SHIKI, 68);

    if (gSettings.CsrActiveConfig == 0) {
      SubScreen->AddMenuCheck("No SIP", OSFLAG_NOSIP, 69);
    }
    
  } else if (LoaderType == OSTYPE_LINEFI) {
    BOOLEAN Quiet = LoadOptions.contains(quietLitteral);
    BOOLEAN WithSplash = LoadOptions.contains(splashLitteral);
    
    // default entry
    SubEntry = getPartiallyDuplicatedEntry();
    if (SubEntry) {
      SubEntry->Title.SWPrintf("Run %ls", FileName.wc_str());
      SubScreen->AddMenuEntry(SubEntry, true);
    }

    SubEntry = getPartiallyDuplicatedEntry();
    if (SubEntry) {
      if (Quiet) {
        SubEntry->Title.SWPrintf("%ls verbose", Title.s());
        SubEntry->LoadOptions.removeIC(quietLitteral);
      } else {
        SubEntry->Title.SWPrintf("%ls quiet", Title.s());
        SubEntry->LoadOptions.AddID(quietLitteral);
      }
      SubScreen->AddMenuEntry(SubEntry, true);
    }

    SubEntry = getPartiallyDuplicatedEntry();
    if (SubEntry) {
      if (WithSplash) {
        SubEntry->Title.SWPrintf("%ls without splash", Title.s());
        SubEntry->LoadOptions.removeIC(splashLitteral);
      } else {
        SubEntry->Title.SWPrintf("%ls with splash", Title.s());
        SubEntry->LoadOptions.AddID(splashLitteral);
      }
      SubScreen->AddMenuEntry(SubEntry, true);
    }

    SubEntry = getPartiallyDuplicatedEntry();
    if (SubEntry) {
      if (WithSplash) {
        if (Quiet) {
          SubEntry->Title.SWPrintf("%ls verbose without splash", Title.s());
          SubEntry->LoadOptions.removeIC(splashLitteral);
          SubEntry->LoadOptions.removeIC(quietLitteral);
        } else {
          SubEntry->Title.SWPrintf("%ls quiet without splash",Title.s());
          SubEntry->LoadOptions.removeIC(splashLitteral);
          SubEntry->LoadOptions.Add(quietLitteral);
        }
      } else if (Quiet) {
        SubEntry->Title.SWPrintf("%ls verbose with splash",Title.s());
        SubEntry->LoadOptions.removeIC(quietLitteral); //
        SubEntry->LoadOptions.AddID(splashLitteral);
      } else {
        SubEntry->Title.SWPrintf("%ls quiet with splash",Title.s());
        SubEntry->LoadOptions.AddID(quietLitteral);
        SubEntry->LoadOptions.AddID(splashLitteral);
      }
      SubScreen->AddMenuEntry(SubEntry, true);
    }

  } else if ((LoaderType == OSTYPE_WIN) || (LoaderType == OSTYPE_WINEFI)) {
    // by default, skip the built-in selection and boot from hard disk only
    LoadOptions.setEmpty();
    LoadOptions.Add("-s"_XS8);
    LoadOptions.Add("-h"_XS8);
    
    // default entry
    SubEntry = getPartiallyDuplicatedEntry();
    if (SubEntry) {
      SubEntry->Title.SWPrintf("Run %ls", FileName.wc_str());
      SubScreen->AddMenuEntry(SubEntry, true);
    }

    SubEntry = getPartiallyDuplicatedEntry();
    if (SubEntry) {
      SubEntry->Title.takeValueFrom("Boot Windows from Hard Disk");
      SubScreen->AddMenuEntry(SubEntry, true);
    }

    SubEntry = getPartiallyDuplicatedEntry();
    if (SubEntry) {
      SubEntry->Title.takeValueFrom("Boot Windows from CD-ROM");
      LoadOptions.setEmpty();
      LoadOptions.Add("-s"_XS8);
      LoadOptions.Add("-c"_XS8);
      SubScreen->AddMenuEntry(SubEntry, true);
    }

    SubEntry = getPartiallyDuplicatedEntry();
    if (SubEntry) {
      SubEntry->Title.SWPrintf("Run %ls in text mode", FileName.wc_str());
      SubEntry->Flags           = OSFLAG_UNSET(SubEntry->Flags, OSFLAG_USEGRAPHICS);
      LoadOptions.setEmpty();
      LoadOptions.Add("-v"_XS8);
      SubEntry->LoaderType      = OSTYPE_OTHER; // Sothor - Why are we using OSTYPE_OTHER here?
      SubScreen->AddMenuEntry(SubEntry, true);
    }

  }

  SubScreen->AddMenuEntry(&MenuEntryReturn, false);
  // DBG("    Added '%ls': OSType='%d', OSVersion='%s'\n",Title,LoaderType,OSVersion);
}

LOADER_ENTRY* AddLoaderEntry(IN CONST XStringW& LoaderPath, IN CONST XString8Array& LoaderOptions,
                       IN CONST XStringW& FullTitle, IN CONST XStringW& LoaderTitle,
                       IN REFIT_VOLUME *Volume, IN const XStringW& APFSTargetUUID, IN XIcon *Image,
                       IN UINT8 OSType, IN UINT8 Flags)
{
  LOADER_ENTRY *Entry;
  INTN          HVi;

  if ((LoaderPath.isEmpty()) || (Volume == NULL) || (Volume->RootDir == NULL) || !FileExists(Volume->RootDir, LoaderPath)) {
    return NULL;
  }

  DBG("        AddLoaderEntry for Volume Name=%ls\n", Volume->VolName.wc_str());
  if (OSFLAG_ISSET(Flags, OSFLAG_DISABLED)) {
    DBG("        skipped because entry is disabled\n");
    return NULL;
  }
  if (!gSettings.ShowHiddenEntries && OSFLAG_ISSET(Flags, OSFLAG_HIDDEN)) {
    DBG("        skipped because entry is hidden\n");
    return NULL;
  }
  //don't add hided entries
  if (!gSettings.ShowHiddenEntries) {
    for (HVi = 0; HVi < gSettings.HVCount; HVi++) {
      if ( LoaderPath.containsIC(gSettings.HVHideStrings[HVi]) ) {
        DBG("        hiding entry: %ls\n", LoaderPath.s());
        return NULL;
      }
    }
  }
  Entry = CreateLoaderEntry(LoaderPath, LoaderOptions, FullTitle, LoaderTitle, Volume, APFSTargetUUID, Image, NULL, OSType, Flags, 0, MenuBackgroundPixel, CUSTOM_BOOT_DISABLED, NULL, NULL, FALSE);
  if (Entry != NULL) {
    if ((Entry->LoaderType == OSTYPE_OSX) ||
        (Entry->LoaderType == OSTYPE_OSX_INSTALLER ) ||
        (Entry->LoaderType == OSTYPE_RECOVERY)) {
      if (gSettings.WithKexts) {
        Entry->Flags = OSFLAG_SET(Entry->Flags, OSFLAG_WITHKEXTS);
      }
      if (gSettings.WithKextsIfNoFakeSMC) {
        Entry->Flags = OSFLAG_SET(Entry->Flags, OSFLAG_CHECKFAKESMC);
        Entry->Flags = OSFLAG_SET(Entry->Flags, OSFLAG_WITHKEXTS);
      }
      if (gSettings.NoCaches) {
        Entry->Flags = OSFLAG_SET(Entry->Flags, OSFLAG_NOCACHES);
      }
    }
    //TODO there is a problem that Entry->Flags is unique while InputItems are global ;(
//    InputItems[69].IValue = Entry->Flags;
    Entry->AddDefaultMenu();
    MainMenu.AddMenuEntry(Entry, true);
    return Entry;
  }
  return NULL;
}

STATIC VOID LinuxScan(REFIT_VOLUME *Volume, UINT8 KernelScan, UINT8 Type, XStringW *CustomPath, XIcon *CustomImage)
{
  // When used for Regular Entries, all found entries will be added by AddLoaderEntry()
  // When used for Custom Entries (detected by CustomPath!=NULL), CustomPath+CustomImage will be set to the first entry found and execution will stop
  // Scanning is adjusted according to Type: OSTYPE_LIN will scan for linux loaders, OSTYPE_LINEFI will scan for linux kernels, unspecified will scan for both
  UINTN        Index;
  EFI_GUID     *PartGUID;

  // check for linux loaders
  if (Type != OSTYPE_LINEFI) { // OSTYPE_LIN or unspecified
    //
    //----- Test common linux name and path like /EFI/ubuntu/grubx64.efi
    REFIT_DIR_ITER  DirIter;
    EFI_FILE_INFO  *DirEntry = NULL;
    DirIterOpen(Volume->RootDir, L"\\EFI", &DirIter);
    while (DirIterNext(&DirIter, 1, L"*", &DirEntry)) {
      if (DirEntry->FileName[0] == '.') {
        //DBG("Skip dot entries: %ls\n", DirEntry->FileName);
        continue;
      }
      XStringW File = SWPrintf("EFI\\%ls\\grubx64.efi", DirEntry->FileName);
      XStringW OSName = XStringW().takeValueFrom(DirEntry->FileName); // this is folder name, for example "ubuntu"
      OSName.lowerAscii(); // lowercase for icon name and title (first letter in title will be capitalized later)
      if (FileExists(Volume->RootDir, File)) {
        // check if nonstandard icon mapping is needed
        for (Index = 0; Index < LinuxIconMappingCount; ++Index) {
          if (StrCmp(OSName.wc_str(),LinuxIconMapping[Index].DirectoryName) == 0) {
            OSName = XStringW().takeValueFrom(LinuxIconMapping[Index].IconName);
            break;
          }
        }
        if (!CustomPath) {
          DBG("  found entry %ls,linux\n", OSName.wc_str());
        }
        XStringW LoaderTitle = OSName.subString(0,1); // capitalize first letter for title
        LoaderTitle.upperAscii();
        LoaderTitle += OSName.subString(1, OSName.length()) + L" Linux"_XSW;
        // Very few linux icons exist in IconNames, but these few may be preloaded, so check that first
        XIcon ImageX = ThemeX.GetIcon(L"os_"_XSW + OSName); //will the image be destroyed or rewritten by next image after the cycle end?
        if (ImageX.isEmpty()) {
          // no preloaded icon, try to load from dir
          ImageX.LoadXImage(ThemeX.ThemeDir, L"os_"_XSW + OSName);
        }
        if (CustomPath) { 
          *CustomPath = File;
          if (CustomImage) {
            *CustomImage = ImageX;
          }
          DirIterClose(&DirIter);
          return;
        } 
        AddLoaderEntry(File, NullXString8Array, L""_XSW, LoaderTitle, Volume, L""_XSW,
                      (ImageX.isEmpty() ? NULL : &ImageX), OSTYPE_LIN, OSFLAG_NODEFAULTARGS);
      } //anyway continue search other entries
    }
    DirIterClose(&DirIter);

    // check for non-standard grub path
    for (Index = 0; Index < LinuxEntryDataCount; ++Index) {
      if (FileExists(Volume->RootDir, LinuxEntryData[Index].Path)) {
        XStringW OSIconName = XStringW().takeValueFrom(LinuxEntryData[Index].Icon);
        OSIconName = OSIconName.subString(0, OSIconName.indexOf(','));
        XIcon ImageX = ThemeX.GetIcon(L"os_"_XSW + OSIconName);
        if (ImageX.isEmpty()) {
          ImageX.LoadXImage(ThemeX.ThemeDir, L"os_"_XSW + OSIconName);
        }
        if (CustomPath) {
          *CustomPath = LinuxEntryData[Index].Path;
          if (CustomImage) {
            *CustomImage = ImageX;
          }
          return;
        }
        AddLoaderEntry(LinuxEntryData[Index].Path, NullXString8Array, L""_XSW, XStringW().takeValueFrom(LinuxEntryData[Index].Title), Volume, L""_XSW,
                       (ImageX.isEmpty() ? NULL : &ImageX), OSTYPE_LIN, OSFLAG_NODEFAULTARGS);
      }
    }

  }

  if (Type != OSTYPE_LIN) { //OSTYPE_LINEFI or unspecified
    // check for linux kernels
    PartGUID = FindGPTPartitionGuidInDevicePath(Volume->DevicePath);
    if ((PartGUID != NULL) && (Volume->RootDir != NULL)) {
      REFIT_DIR_ITER  Iter;
      EFI_FILE_INFO  *FileInfo = NULL;
      EFI_TIME        PreviousTime;
      XStringW        Path;
      // CHAR16         *Options;
      // Get the partition UUID and make sure it's lower case
      CHAR16          PartUUID[40];
      ZeroMem(&PreviousTime, sizeof(EFI_TIME));
      snwprintf(PartUUID, sizeof(PartUUID), "%s", strguid(PartGUID));
      StrToLower(PartUUID);
      // open the /boot directory (or whatever directory path)
      DirIterOpen(Volume->RootDir, LINUX_BOOT_PATH, &Iter);
  
      // Check which kernel scan to use
  
      // the following options can produce only a single option
      switch (KernelScan) {
        case KERNEL_SCAN_FIRST:
          // First kernel found only
          while (DirIterNext(&Iter, 2, LINUX_LOADER_SEARCH_PATH, &FileInfo)) {
            if (FileInfo != NULL) {
              if (FileInfo->FileSize == 0) {
                continue;
              }
              // get the kernel file path
              Path.SWPrintf("%ls\\%ls", LINUX_BOOT_PATH, FileInfo->FileName);
              // free the file info
              break;
            }
          }
          break;
        case KERNEL_SCAN_LAST:
          // Last kernel found only
          while (DirIterNext(&Iter, 2, LINUX_LOADER_SEARCH_PATH, &FileInfo)) {
            if (FileInfo != NULL) {
              if (FileInfo->FileSize > 0) {
                // get the kernel file path
                Path.SWPrintf("%ls\\%ls", LINUX_BOOT_PATH, FileInfo->FileName);
              }
            }
          }
          break;
        case KERNEL_SCAN_NEWEST:
          // Newest dated kernel only
          while (DirIterNext(&Iter, 2, LINUX_LOADER_SEARCH_PATH, &FileInfo)) {
            if (FileInfo != NULL) {
              if (FileInfo->FileSize > 0) {
                // get the kernel file path
                if ((PreviousTime.Year == 0) || (TimeCmp(&PreviousTime, &(FileInfo->ModificationTime)) < 0)) {
                  Path.SWPrintf("%ls\\%ls", LINUX_BOOT_PATH, FileInfo->FileName);
                  PreviousTime = FileInfo->ModificationTime;
                }
              }
            }
          }
          break;
        case KERNEL_SCAN_OLDEST:
          // Oldest dated kernel only
          while (DirIterNext(&Iter, 2, LINUX_LOADER_SEARCH_PATH, &FileInfo)) {
            if (FileInfo != NULL) {
              if (FileInfo->FileSize > 0) {
                // get the kernel file path
                if ((PreviousTime.Year == 0) || (TimeCmp(&PreviousTime, &(FileInfo->ModificationTime)) > 0)) {
                  Path.SWPrintf("%ls\\%ls", LINUX_BOOT_PATH, FileInfo->FileName);
                  PreviousTime = FileInfo->ModificationTime;
                }
              }
            }
          }
          break;
        case KERNEL_SCAN_MOSTRECENT:
          // most recent kernel version only
          while (DirIterNext(&Iter, 2, LINUX_LOADER_SEARCH_PATH, &FileInfo)) {
            if (FileInfo != NULL) {
              if (FileInfo->FileSize > 0) {
                // get the kernel file path
                XStringW NewPath = SWPrintf("%ls\\%ls", LINUX_BOOT_PATH, FileInfo->FileName);
                if ( Path < NewPath ) {
                   Path = NewPath;
                } else {
                    Path.setEmpty();
                }
              }
            }
          }
          break;
        case KERNEL_SCAN_EARLIEST:
          // earliest kernel version only
          while (DirIterNext(&Iter, 2, LINUX_LOADER_SEARCH_PATH, &FileInfo)) {
            if (FileInfo != NULL) {
              if (FileInfo->FileSize > 0) {
                // get the kernel file path
                XStringW NewPath = SWPrintf("%ls\\%ls", LINUX_BOOT_PATH, FileInfo->FileName);
                if ( Path > NewPath ) {
                  Path = NewPath;
                } else {
                  Path.setEmpty();
                }
              }
            }
          }
          break;
        case KERNEL_SCAN_NONE:
        default:
          // No kernel scan
          break;
      }

      // add the produced entry
      if (Path.notEmpty()) {
        if (CustomPath) {
          *CustomPath = Path;
          DirIterClose(&Iter);
          return;
        }
        XString8Array Options = LinuxKernelOptions(Iter.DirHandle, Basename(Path.wc_str()) + LINUX_LOADER_PATH.length(), PartUUID, NullXString8Array);
        // Add the entry
        AddLoaderEntry(Path, (Options.isEmpty()) ? LINUX_DEFAULT_OPTIONS : Options, L""_XSW, L""_XSW, Volume, L""_XSW, NULL, OSTYPE_LINEFI, OSFLAG_NODEFAULTARGS);
        Path.setEmpty();
      }

      // the following produces multiple entries
      // custom entries has a different implementation, and does not use this code
      if (!CustomPath && KernelScan == KERNEL_SCAN_ALL) {
        // get all the filename matches
        while (DirIterNext(&Iter, 2, LINUX_LOADER_SEARCH_PATH, &FileInfo)) {
          if (FileInfo != NULL) {
            if (FileInfo->FileSize > 0) {
              // get the kernel file path
              Path.SWPrintf("%ls\\%ls", LINUX_BOOT_PATH, FileInfo->FileName);
              XString8Array Options = LinuxKernelOptions(Iter.DirHandle, Basename(Path.wc_str()) + LINUX_LOADER_PATH.length(), PartUUID, NullXString8Array);
              // Add the entry
              AddLoaderEntry(Path, (Options.isEmpty()) ? LINUX_DEFAULT_OPTIONS : Options, L""_XSW, L""_XSW, Volume, L""_XSW, NULL, OSTYPE_LINEFI, OSFLAG_NODEFAULTARGS);
              Path.setEmpty();
            }
          }
        }
      }
      //close the directory
      DirIterClose(&Iter);
    }  
  }

}

//constants
//const XStringW APFSFVBootPath      = L"\\00000000-0000-0000-0000-000000000000\\System\\Library\\CoreServices\\boot.efi"_XSW;
//const XStringW APFSRecBootPath     = L"\\00000000-0000-0000-0000-000000000000\\boot.efi"_XSW;
//const XStringW APFSInstallBootPath = L"\\00000000-0000-0000-0000-000000000000\\com.apple.installer\\boot.efi"_XSW;

#define Paper 1
#define Rock  2
#define Scissor 4

VOID AddPRSEntry(REFIT_VOLUME *Volume)
{
  INTN WhatBoot = 0;
  //CONST INTN Paper = 1;
  //CONST INTN Rock = 2;
  //CONST INTN Scissor = 4;

  WhatBoot |= FileExists(Volume->RootDir, RockBoot)?Rock:0;
  WhatBoot |= FileExists(Volume->RootDir, PaperBoot)?Paper:0;
  WhatBoot |= FileExists(Volume->RootDir, ScissorBoot)?Scissor:0;
  switch (WhatBoot) {
    case Paper:
    case (Paper | Rock):
      AddLoaderEntry(PaperBoot, NullXString8Array, L""_XSW, L"macOS InstallP"_XSW, Volume, L""_XSW, NULL, OSTYPE_OSX_INSTALLER, 0);
      break;
    case Scissor:
    case (Paper | Scissor):
      AddLoaderEntry(ScissorBoot, NullXString8Array, L""_XSW, L"macOS InstallS"_XSW, Volume, L""_XSW, NULL, OSTYPE_OSX_INSTALLER, 0);
      break;
    case Rock:
    case (Rock | Scissor):
    case (Rock | Scissor | Paper):
      AddLoaderEntry(RockBoot, NullXString8Array, L""_XSW, L"macOS InstallR"_XSW, Volume, L""_XSW, NULL, OSTYPE_OSX_INSTALLER, 0);
      break;

    default:
      break;
  }
}
#undef Paper
#undef Rock
#undef Scissor

VOID ScanLoader(VOID)
{
  //DBG("Scanning loaders...\n");
  DbgHeader("ScanLoader");
   
  for (UINTN VolumeIndex = 0; VolumeIndex < Volumes.size(); VolumeIndex++) {
    REFIT_VOLUME* Volume = &Volumes[VolumeIndex];
    if (Volume->RootDir == NULL) { // || Volume->VolName == NULL)
      //DBG(", no file system\n", VolumeIndex);
      continue;
    }
    DBG("- [%02llu]: '%ls'", VolumeIndex, Volume->VolName.wc_str());
    if (Volume->VolName.isEmpty()) {
      Volume->VolName = L"Unknown"_XSW;
    }

    // skip volume if its kind is configured as disabled
    if (((1ull<<Volume->DiskKind) & GlobalConfig.DisableFlags) != 0)
    {
      DBG(", flagged disable\n");
      continue;
    }

    if (Volume->Hidden) {
      DBG(", hidden\n");
      continue;
    }
    DBG("\n");

    // check for Mac OS X Install Data
    // 1st stage - createinstallmedia
    if (FileExists(Volume->RootDir, L"\\.IABootFiles\\boot.efi")) {
      if (FileExists(Volume->RootDir, L"\\Install OS X Mavericks.app") ||
          FileExists(Volume->RootDir, L"\\Install OS X Yosemite.app") ||
          FileExists(Volume->RootDir, L"\\Install OS X El Capitan.app")) {
        AddLoaderEntry(L"\\.IABootFiles\\boot.efi"_XSW, NullXString8Array, L""_XSW, L"OS X Install"_XSW, Volume, L""_XSW, NULL, OSTYPE_OSX_INSTALLER, 0); // 10.9 - 10.11
      } else {
        AddLoaderEntry(L"\\.IABootFiles\\boot.efi"_XSW, NullXString8Array, L""_XSW, L"macOS Install"_XSW, Volume, L""_XSW, NULL, OSTYPE_OSX_INSTALLER, 0); // 10.12 - 10.13.3
      }
    } else if (FileExists(Volume->RootDir, L"\\.IAPhysicalMedia") && FileExists(Volume->RootDir, MACOSX_LOADER_PATH)) {
      AddLoaderEntry(MACOSX_LOADER_PATH, NullXString8Array, L""_XSW, L"macOS Install"_XSW, Volume, L""_XSW, NULL, OSTYPE_OSX_INSTALLER, 0); // 10.13.4+
    }
    // 2nd stage - InstallESD/AppStore/startosinstall/Fusion Drive
    AddLoaderEntry(L"\\Mac OS X Install Data\\boot.efi"_XSW, NullXString8Array, L""_XSW, L"Mac OS X Install"_XSW, Volume, L""_XSW, NULL, OSTYPE_OSX_INSTALLER, 0); // 10.7
    AddLoaderEntry(L"\\OS X Install Data\\boot.efi"_XSW, NullXString8Array, L""_XSW, L"OS X Install"_XSW, Volume, L""_XSW, NULL, OSTYPE_OSX_INSTALLER, 0); // 10.8 - 10.11
    AddLoaderEntry(L"\\macOS Install Data\\boot.efi"_XSW, NullXString8Array, L""_XSW, L"macOS Install"_XSW, Volume, L""_XSW, NULL, OSTYPE_OSX_INSTALLER, 0); // 10.12 - 10.12.3
    AddLoaderEntry(L"\\macOS Install Data\\Locked Files\\Boot Files\\boot.efi"_XSW, NullXString8Array, L""_XSW, L"macOS Install"_XSW, Volume, L""_XSW, NULL, OSTYPE_OSX_INSTALLER, 0); // 10.12.4+
    AddLoaderEntry(L"\\macOS Install Data\\Locked Files\\boot.efi"_XSW, NullXString8Array, L""_XSW, L"macOS Install"_XSW, Volume, L""_XSW, NULL, OSTYPE_OSX_INSTALLER, 0); // 10.16+
    AddPRSEntry(Volume); // 10.12+

    // Netinstall
    AddLoaderEntry(L"\\NetInstall macOS High Sierra.nbi\\i386\\booter"_XSW, NullXString8Array, L""_XSW, L"macOS Install"_XSW, Volume, L""_XSW, NULL, OSTYPE_OSX_INSTALLER, 0);
    // Use standard location for boot.efi, according to the install files is present
    // That file indentifies a DVD/ESD/BaseSystem/Fusion Drive Install Media, so when present, check standard path to avoid entry duplication
    if (FileExists(Volume->RootDir, MACOSX_LOADER_PATH)) {
      if (FileExists(Volume->RootDir, L"\\System\\Installation\\CDIS\\Mac OS X Installer.app")) {
        // InstallDVD/BaseSystem
        AddLoaderEntry(MACOSX_LOADER_PATH, NullXString8Array, L""_XSW, L"Mac OS X Install"_XSW, Volume, L""_XSW, NULL, OSTYPE_OSX_INSTALLER, 0); // 10.6/10.7
      } else if (FileExists(Volume->RootDir, L"\\System\\Installation\\CDIS\\OS X Installer.app")) {
        // BaseSystem
        AddLoaderEntry(MACOSX_LOADER_PATH, NullXString8Array, L""_XSW, L"OS X Install"_XSW, Volume, L""_XSW, NULL, OSTYPE_OSX_INSTALLER, 0); // 10.8 - 10.11
      } else if (FileExists(Volume->RootDir, L"\\System\\Installation\\CDIS\\macOS Installer.app")) {
        // BaseSystem
        AddLoaderEntry(MACOSX_LOADER_PATH, NullXString8Array, L""_XSW, L"macOS Install"_XSW, Volume, L""_XSW, NULL, OSTYPE_OSX_INSTALLER, 0); // 10.12+
      } else if (FileExists(Volume->RootDir, L"\\BaseSystem.dmg") && FileExists(Volume->RootDir, L"\\mach_kernel")) {
        // InstallESD
        if (FileExists(Volume->RootDir, L"\\MacOSX_Media_Background.png")) {
          AddLoaderEntry(MACOSX_LOADER_PATH, NullXString8Array, L""_XSW, L"Mac OS X Install"_XSW, Volume, L""_XSW, NULL, OSTYPE_OSX_INSTALLER, 0); // 10.7
        } else {
          AddLoaderEntry(MACOSX_LOADER_PATH, NullXString8Array, L""_XSW, L"OS X Install"_XSW, Volume, L""_XSW, NULL, OSTYPE_OSX_INSTALLER, 0); // 10.8
        }
      } else if (FileExists(Volume->RootDir, L"\\com.apple.boot.R\\System\\Library\\PrelinkedKernels\\prelinkedkernel") ||
                 FileExists(Volume->RootDir, L"\\com.apple.boot.P\\System\\Library\\PrelinkedKernels\\prelinkedkernel") ||
                 FileExists(Volume->RootDir, L"\\com.apple.boot.S\\System\\Library\\PrelinkedKernels\\prelinkedkernel")) {
        if (StriStr(Volume->VolName.wc_str(), L"Recovery") != NULL) {
          // FileVault of HFS+
          // TODO: need info for 10.11 and lower
          AddLoaderEntry(MACOSX_LOADER_PATH, NullXString8Array, L""_XSW, L"macOS FileVault"_XSW, Volume, L""_XSW, NULL, OSTYPE_OSX, 0); // 10.12+
        } else {
          // Fusion Drive
          AddLoaderEntry(MACOSX_LOADER_PATH, NullXString8Array, L""_XSW, L"OS X Install"_XSW, Volume, L""_XSW, NULL, OSTYPE_OSX_INSTALLER, 0); // 10.11
        }
      } else if (!FileExists(Volume->RootDir, L"\\.IAPhysicalMedia")) {
        // Installed
        if (EFI_ERROR(GetRootUUID(Volume)) || isFirstRootUUID(Volume)) {
          if (!FileExists(Volume->RootDir, L"\\System\\Library\\CoreServices\\NotificationCenter.app") && !FileExists(Volume->RootDir, L"\\System\\Library\\CoreServices\\Siri.app")) {
            AddLoaderEntry(MACOSX_LOADER_PATH, NullXString8Array, L""_XSW, L"Mac OS X"_XSW, Volume, L""_XSW, NULL, OSTYPE_OSX, 0); // 10.6 - 10.7
          } else if (FileExists(Volume->RootDir, L"\\System\\Library\\CoreServices\\NotificationCenter.app") && !FileExists(Volume->RootDir, L"\\System\\Library\\CoreServices\\Siri.app")) {
            AddLoaderEntry(MACOSX_LOADER_PATH, NullXString8Array, L""_XSW, L"OS X"_XSW, Volume, L""_XSW, NULL, OSTYPE_OSX, 0); // 10.8 - 10.11
          } else {
            AddLoaderEntry(MACOSX_LOADER_PATH, NullXString8Array, L""_XSW, L"macOS"_XSW, Volume, L""_XSW, NULL, OSTYPE_OSX, 0); // 10.12+
          }
        }
      }
    }

    // check for Mac OS X Recovery Boot
    AddLoaderEntry(L"\\com.apple.recovery.boot\\boot.efi"_XSW, NullXString8Array, L""_XSW, L"Recovery"_XSW, Volume, L""_XSW, NULL, OSTYPE_RECOVERY, 0);

    // Sometimes, on some systems (HP UEFI, if Win is installed first)
    // it is needed to get rid of bootmgfw.efi to allow starting of
    // Clover as /efi/boot/bootx64.efi from HD. We can do that by renaming
    // bootmgfw.efi to bootmgfw-orig.efi
    AddLoaderEntry(L"\\EFI\\microsoft\\Boot\\bootmgfw-orig.efi"_XSW, NullXString8Array, L""_XSW, L"Microsoft EFI"_XSW, Volume, L""_XSW, NULL, OSTYPE_WINEFI, 0);
    // check for Microsoft boot loader/menu
    // If there is bootmgfw-orig.efi, then do not check for bootmgfw.efi
    // since on some systems this will actually be CloverX64.efi
    // renamed to bootmgfw.efi
    AddLoaderEntry(L"\\EFI\\microsoft\\Boot\\bootmgfw.efi"_XSW, NullXString8Array, L""_XSW, L"Microsoft EFI Boot"_XSW, Volume, L""_XSW, NULL, OSTYPE_WINEFI, 0);
    // check for Microsoft boot loader/menu. This entry is redundant so excluded
    // AddLoaderEntry(L"\\bootmgr.efi", L"", L"Microsoft EFI mgrboot", Volume, NULL, OSTYPE_WINEFI, 0);
    // check for Microsoft boot loader/menu on CDROM
    if (!AddLoaderEntry(L"\\EFI\\MICROSOFT\\BOOT\\cdboot.efi"_XSW, NullXString8Array, L""_XSW, L"Microsoft EFI cdboot"_XSW, Volume, L""_XSW, NULL, OSTYPE_WINEFI, 0)) {
      AddLoaderEntry(L"\\EFI\\MICROSOF\\BOOT\\CDBOOT.EFI"_XSW, NullXString8Array, L""_XSW, L"Microsoft EFI CDBOOT"_XSW, Volume, L""_XSW, NULL, OSTYPE_WINEFI, 0);
    }

#if defined(ANDX86)
    if (TRUE) { //gSettings.AndroidScan
      // check for Android loaders
      for (UINTN Index = 0; Index < AndroidEntryDataCount; ++Index) {
        UINTN aIndex, aFound;
      if (FileExists(Volume->RootDir, AndroidEntryData[Index].Path)) {
          aFound = 0;
          for (aIndex = 0; aIndex < ANDX86_FINDLEN; ++aIndex) {
            if ((AndroidEntryData[Index].Find[aIndex].isEmpty()) || FileExists(Volume->RootDir, AndroidEntryData[Index].Find[aIndex])) ++aFound;
          }
          if (aFound && (aFound == aIndex)) {
            XIcon ImageX;
            XStringW IconXSW = XStringW().takeValueFrom(AndroidEntryData[Index].Icon);
            ImageX.LoadXImage(ThemeX.ThemeDir, (L"os_"_XSW + IconXSW.subString(0, IconXSW.indexOf(','))).wc_str());
            AddLoaderEntry(AndroidEntryData[Index].Path, NullXString8Array, L""_XSW, XStringW().takeValueFrom(AndroidEntryData[Index].Title), Volume, L""_XSW,
                           (ImageX.isEmpty() ? NULL : &ImageX), OSTYPE_LIN, OSFLAG_NODEFAULTARGS);
          }
        }
      }
    }
#endif

    if (gSettings.LinuxScan) {
      LinuxScan(Volume, gSettings.KernelScan, 0, NULL, NULL);
    }

    //     DBG("search for  optical UEFI\n");
    if (Volume->DiskKind == DISK_KIND_OPTICAL) {
      AddLoaderEntry(BOOT_LOADER_PATH, NullXString8Array, L""_XSW, L"UEFI optical"_XSW, Volume, L""_XSW, NULL, OSTYPE_OTHER, 0);
    }
    //     DBG("search for internal UEFI\n");
    if (Volume->DiskKind == DISK_KIND_INTERNAL) {
      AddLoaderEntry(BOOT_LOADER_PATH, NullXString8Array, L""_XSW, L"UEFI internal"_XSW, Volume, L""_XSW, NULL, OSTYPE_OTHER, OSFLAG_HIDDEN);
    }
    //    DBG("search for external UEFI\n");
    if (Volume->DiskKind == DISK_KIND_EXTERNAL) {
      AddLoaderEntry(BOOT_LOADER_PATH, NullXString8Array, L""_XSW, L"UEFI external"_XSW, Volume, L""_XSW, NULL, OSTYPE_OTHER, OSFLAG_HIDDEN);
    }
  }


  /* Pass to add non redundant Preboot volume AND redundant as hidden */

  for (UINTN VolumeIndex = 0; VolumeIndex < Volumes.size(); VolumeIndex++) {
    REFIT_VOLUME* Volume = &Volumes[VolumeIndex];
    if (Volume->RootDir == NULL) { // || Volume->VolName == NULL)
      //DBG(", no file system\n", VolumeIndex);
      continue;
    }
    DBG("- [%02llu]: '%ls'", VolumeIndex, Volume->VolName.wc_str());
    if (Volume->VolName.isEmpty()) {
      Volume->VolName = L"Unknown"_XSW;
    }

    // skip volume if its kind is configured as disabled
    if (((1ull<<Volume->DiskKind) & GlobalConfig.DisableFlags) != 0)
    {
      DBG(", flagged disable\n");
      continue;
    }

    if (Volume->Hidden) {
      DBG(", hidden\n");
      continue;
    }
    DBG("\n");

    if ( Volume->ApfsTargetUUIDArray.size() > 0 ) {

      for (UINTN i = 0; i < Volume->ApfsTargetUUIDArray.size(); i++)
      {
        const XString8& ApfsTargetUUID = Volume->ApfsTargetUUIDArray[i];

        int flag = 0;
        const LOADER_ENTRY* targetLoaderEntry = NULL;
        for ( size_t entryIdx = 0 ; entryIdx < MainMenu.Entries.size() ; entryIdx ++ )
        {
          if ( MainMenu.Entries[entryIdx].getLOADER_ENTRY() ) {
            const LOADER_ENTRY* loaderEntry = MainMenu.Entries[entryIdx].getLOADER_ENTRY();
            if ( loaderEntry->Volume->ApfsFileSystemUUID == ApfsTargetUUID ) {
              targetLoaderEntry = loaderEntry;
              flag = OSFLAG_HIDDEN;
            }
          }
        }
        if ( targetLoaderEntry ) {
          AddLoaderEntry(SWPrintf("\\%s\\System\\Library\\CoreServices\\boot.efi", ApfsTargetUUID.c_str()), NullXString8Array, SWPrintf("Boot Mac OS X from %ls via %ls", targetLoaderEntry->VolName.wc_str(), Volume->VolName.wc_str()), L""_XSW, Volume, Volume->ApfsTargetUUIDArray[i], NULL, OSTYPE_OSX, OSFLAG_HIDDEN);
          //Try to add Recovery APFS entry
          AddLoaderEntry(SWPrintf("\\%s\\boot.efi", Volume->ApfsTargetUUIDArray[i].c_str()), NullXString8Array, SWPrintf("Boot Mac OS X Recovery for %ls via %ls", targetLoaderEntry->VolName.wc_str(), Volume->VolName.wc_str()), L""_XSW, Volume, Volume->ApfsTargetUUIDArray[i], NULL, OSTYPE_RECOVERY, 0);
          //Try to add macOS install entry
          AddLoaderEntry(SWPrintf("\\%s\\com.apple.installer\\boot.efi", Volume->ApfsTargetUUIDArray[i].c_str()), NullXString8Array, L""_XSW, L"macOS Install Prebooter"_XSW, Volume, Volume->ApfsTargetUUIDArray[i], NULL, OSTYPE_OSX_INSTALLER, 0);
        }
        else
        {
          REFIT_VOLUME* targetVolume = NULL;
          for (UINTN VolumeIndex2 = 0; VolumeIndex2 < Volumes.size(); VolumeIndex2++) {
            REFIT_VOLUME* Volume2 = &Volumes[VolumeIndex2];
            //DBG("name %ls  uuid=%ls \n", Volume2->VolName, Volume2->ApfsFileSystemUUID.wc_str());
            if ( Volume2->ApfsFileSystemUUID == ApfsTargetUUID ) {
              targetVolume = Volume2;
            }
          }
          if ( targetVolume ) {
            AddLoaderEntry(SWPrintf("\\%s\\System\\Library\\CoreServices\\boot.efi", ApfsTargetUUID.c_str()), NullXString8Array, SWPrintf("Boot Mac OS X from %ls via %ls", targetVolume->VolName.wc_str(), Volume->VolName.wc_str()), L""_XSW, Volume, Volume->ApfsTargetUUIDArray[i], NULL, OSTYPE_OSX, 0);
            //Try to add Recovery APFS entry
            AddLoaderEntry(SWPrintf("\\%s\\boot.efi", Volume->ApfsTargetUUIDArray[i].c_str()), NullXString8Array, SWPrintf("Boot Mac OS X Recovery for %ls via %ls", targetVolume->VolName.wc_str(), Volume->VolName.wc_str()), L""_XSW, Volume, Volume->ApfsTargetUUIDArray[i], NULL, OSTYPE_RECOVERY, 0);
            //Try to add macOS install entry
            AddLoaderEntry(SWPrintf("\\%s\\com.apple.installer\\boot.efi", Volume->ApfsTargetUUIDArray[i].c_str()), NullXString8Array, SWPrintf("Boot Mac OS X Install for %ls via %ls", targetVolume->VolName.wc_str(), Volume->VolName.wc_str()), L""_XSW, Volume, Volume->ApfsTargetUUIDArray[i], NULL, OSTYPE_OSX_INSTALLER, 0);
          } else {
            AddLoaderEntry(SWPrintf("\\%s\\System\\Library\\CoreServices\\boot.efi", ApfsTargetUUID.c_str()), NullXString8Array, L""_XSW, L"FileVault Prebooter"_XSW, Volume, Volume->ApfsTargetUUIDArray[i], NULL, OSTYPE_OSX, 0);
            //Try to add Recovery APFS entry
            AddLoaderEntry(SWPrintf("\\%s\\boot.efi", Volume->ApfsTargetUUIDArray[i].c_str()), NullXString8Array, SWPrintf("Boot Mac OS X Recovery for %ls via %ls", targetLoaderEntry->VolName.wc_str(), Volume->VolName.wc_str()), L""_XSW, Volume, Volume->ApfsTargetUUIDArray[i], NULL, OSTYPE_RECOVERY, 0);
            //Try to add macOS install entry
            AddLoaderEntry(SWPrintf("\\%s\\com.apple.installer\\boot.efi", Volume->ApfsTargetUUIDArray[i].c_str()), NullXString8Array, L""_XSW, L"macOS Install Prebooter"_XSW, Volume, Volume->ApfsTargetUUIDArray[i], NULL, OSTYPE_OSX_INSTALLER, 0);
          }
        }
      }
    }
  }
}

STATIC VOID AddCustomEntry(IN UINTN                CustomIndex,
                           IN XStringW             CustomPath,
                           IN CUSTOM_LOADER_ENTRY *Custom,
                           IN REFIT_MENU_SCREEN   *SubMenu)
{
  UINTN           VolumeIndex;
  REFIT_VOLUME   *Volume;
  REFIT_DIR_ITER  SIter;
  REFIT_DIR_ITER *Iter = &SIter;
  CHAR16          PartUUID[40];
  BOOLEAN         IsSubEntry = (SubMenu != NULL);
  BOOLEAN         FindCustomPath = (CustomPath.isEmpty());

  if (Custom == NULL) {
    return;
  }

  if (FindCustomPath && (Custom->Type != OSTYPE_LINEFI) && (Custom->Type != OSTYPE_LIN)) {
//    DBG("Custom %lsentry %llu skipped because it didn't have a ", IsSubEntry ? L"sub " : L"", CustomIndex);
//    if (Custom->Type == 0) {
//      DBG("Type.\n");
//    } else {
//      DBG("Path.\n");
//    }
    return;
  }

  if (OSFLAG_ISSET(Custom->Flags, OSFLAG_DISABLED)) {
//    DBG("Custom %lsentry %llu skipped because it is disabled.\n", IsSubEntry ? L"sub " : L"", CustomIndex);
    return;
  }

  if (!gSettings.ShowHiddenEntries && OSFLAG_ISSET(Custom->Flags, OSFLAG_HIDDEN)) {
//    DBG("Custom %lsentry %llu skipped because it is hidden.\n", IsSubEntry ? L"sub " : L"", CustomIndex);
    return;
  }

#if 0  //if someone want to debug this
  DBG("Custom %lsentry %llu ", IsSubEntry ? L"sub " : L"", CustomIndex);
  //  if (Custom->Title) {
  DBG("Title:\"%ls\" ", Custom->Title.wc_str());
  //  }
  //  if (Custom->FullTitle) {
  DBG("FullTitle:\"%ls\" ", Custom->FullTitle.wc_str());
  //  }
  if (CustomPath) {
    DBG("Path:\"%ls\" ", CustomPath);
  }
  if (Custom->Options != NULL) {
    DBG("Options:\"%ls\" ", Custom->Options);
  }
  DBG("Type:%d Flags:0x%hhX matching ", Custom->Type, Custom->Flags);
  if (Custom->Volume) {
    DBG("Volume:\"%ls\"\n", Custom->Volume);
  } else {
    DBG("all volumes\n");
  }
#endif

  for (VolumeIndex = 0; VolumeIndex < Volumes.size(); ++VolumeIndex) {
    CUSTOM_LOADER_ENTRY *CustomSubEntry;
    LOADER_ENTRY        *Entry = NULL;
    XIcon Image = Custom->Image;
    XIcon DriveImage = Custom->DriveImage;

    EFI_GUID            *Guid = NULL;
    UINT64               VolumeSize;

    Volume = &Volumes[VolumeIndex];
    if ((Volume == NULL) || (Volume->RootDir == NULL)) {
      continue;
    }
    if (Volume->VolName.isEmpty()) {
      Volume->VolName = L"Unknown"_XSW;
    }

    DBG("    Checking volume \"%ls\" (%ls) ... ", Volume->VolName.wc_str(), Volume->DevicePathString.wc_str());

    // skip volume if its kind is configured as disabled
    if (((1ull<<Volume->DiskKind) & GlobalConfig.DisableFlags) != 0) {
      DBG("skipped because media is disabled\n");
      continue;
    }

    if (Custom->VolumeType != 0 && ((1<<Volume->DiskKind) & Custom->VolumeType) == 0) {
      DBG("skipped because media is ignored\n");
      continue;
    }

    if (Volume->Hidden) {
      DBG("skipped because volume is hidden\n");
      continue;
    }

    // Check for exact volume matches (devicepath / volumelabel)
    if (Custom->Volume.notEmpty()) {
      if ((StrStr(Volume->DevicePathString.wc_str(), Custom->Volume.wc_str()) == NULL) &&
          ((Volume->VolName.isEmpty()) || (StrStr(Volume->VolName.wc_str(), Custom->Volume.wc_str()) == NULL))) {
        DBG("skipped because volume does not match\n");
        continue;
      }
      // NOTE: Sothor - We dont care about legacy OS type // Check if the volume should be of certain os type
      //if ((Custom->Type != 0) && (Volume->OSType != 0) && !OSTYPE_COMPARE(OSType, Volume->OSType)) {
      //  DBG("skipped because wrong type (%d != %d)\n", OSType, Volume->OSType);
      //  continue;
      //}
      //} else if ((Custom->Type != 0) && (Volume->OSType != 0) && !OSTYPE_COMPARE(OSType, Volume->OSType)) {
      //DBG("skipped because wrong type (%d != %d)\n", OSType, Volume->OSType);
      //continue;
    }

    // Check the volume is readable and the entry exists on the volume
    if (Volume->RootDir == NULL) {
      DBG("skipped because filesystem is not readable\n");
      continue;
    }

/*
    if (StriCmp(CustomPath, MACOSX_LOADER_PATH) == 0 && FileExists(Volume->RootDir, L"\\.IAPhysicalMedia")) {
      DBG("skipped standard macOS path because volume is 2nd stage Install Media\n");
      continue;
    }
*/

    Guid = FindGPTPartitionGuidInDevicePath(Volume->DevicePath);
    if (FindCustomPath) {
      // Get the partition UUID and make sure it's lower case
      if (Guid == NULL) {
        DBG("skipped because volume does not have partition uuid\n");
        continue;
      }
      snwprintf(PartUUID, sizeof(PartUUID), "%s", strguid(Guid));
      StrToLower(PartUUID);

      // search for standard/nonstandard linux uefi paths, and all kernel scan options that != KERNEL_SCAN_ALL
      if (Custom->Type == OSTYPE_LIN || Custom->KernelScan != KERNEL_SCAN_ALL) {
        LinuxScan(Volume, Custom->KernelScan, Custom->Type, &CustomPath, &Image);
      }
      if (Custom->Type == OSTYPE_LINEFI) {
        // Open the boot directory to determine linux loadoptions when found item, or kernels when KERNEL_SCAN_ALL
        DirIterOpen(Volume->RootDir, LINUX_BOOT_PATH, Iter);
      }
    } else if (!FileExists(Volume->RootDir, CustomPath)) {
      DBG("skipped because path does not exist\n");
      continue;
    }

    // Change to custom image if needed
    if (Image.isEmpty() && Custom->ImagePath.notEmpty()) {
      Image.LoadXImage(ThemeX.ThemeDir, Custom->ImagePath);
      if (Image.isEmpty()) {
        Image.LoadXImage(ThemeX.ThemeDir, L"os_"_XSW + Custom->ImagePath);
        if (Image.isEmpty()) {
          Image.LoadXImage(SelfDir, Custom->ImagePath);
          if (Image.isEmpty()) {
            Image.LoadXImage(SelfRootDir, Custom->ImagePath);
            if (Image.isEmpty()) {
              Image.LoadXImage(Volume->RootDir, Custom->ImagePath);
            }
          }
        }
      }
    }

    // Change to custom drive image if needed
    if (DriveImage.isEmpty() && Custom->DriveImagePath.notEmpty()) {
      DriveImage.LoadXImage(ThemeX.ThemeDir, Custom->DriveImagePath);
      if (DriveImage.isEmpty()) {
        DriveImage.LoadXImage(SelfDir, Custom->ImagePath);
        if (DriveImage.isEmpty()) {
          DriveImage.LoadXImage(SelfRootDir, Custom->ImagePath);
          if (DriveImage.isEmpty()) {
            DriveImage.LoadXImage(Volume->RootDir, Custom->ImagePath);
          }
        }
      }
    }

    do { // when not scanning for kernels, this loop will execute only once
      XString8Array CustomOptions = Custom->LoadOptions;

      // for LINEFI with option KERNEL_SCAN_ALL, use this loop to search for kernels
      if (FindCustomPath && Custom->Type == OSTYPE_LINEFI && Custom->KernelScan == KERNEL_SCAN_ALL) {
        EFI_FILE_INFO *FileInfo = NULL;
        // Get the next kernel path or stop looking
        if (!DirIterNext(Iter, 2, LINUX_LOADER_SEARCH_PATH, &FileInfo) || (FileInfo == NULL)) {
          DBG("\n");
          break;
        }
        // who knows....
        if (FileInfo->FileSize == 0) {
          continue;
        }
        // get the kernel file path
        CustomPath.SWPrintf("%ls\\%ls", LINUX_BOOT_PATH, FileInfo->FileName);
      }
      if (CustomPath.isEmpty()) {
        DBG("skipped\n");
        break;
      }

      // Check to make sure if we should update linux custom options or not
      if (FindCustomPath && Custom->Type == OSTYPE_LINEFI && OSFLAG_ISUNSET(Custom->Flags, OSFLAG_NODEFAULTARGS)) {
        // Find the init ram image and select root
        CustomOptions = LinuxKernelOptions(Iter->DirHandle, Basename(CustomPath.wc_str()) + LINUX_LOADER_PATH.length(), PartUUID, Custom->LoadOptions);
        Custom->Flags = OSFLAG_SET(Custom->Flags, OSFLAG_NODEFAULTARGS);
      }

      // Check to make sure that this entry is not hidden or disabled by another custom entry
      if (!IsSubEntry) {
        CUSTOM_LOADER_ENTRY *Ptr;
        UINTN                i = 0;
        BOOLEAN              BetterMatch = FALSE;
        for (Ptr = gSettings.CustomEntries; Ptr != NULL; ++i, Ptr = Ptr->Next) {
          // Don't match against this custom
          if (Ptr == Custom) {
            continue;
          }
          // Can only match the same types
          if (Custom->Type != Ptr->Type) {
            continue;
          }
          // Check if the volume string matches
          if (Custom->Volume != Ptr->Volume) {
            if (Ptr->Volume.isEmpty()) {
              // Less precise volume match
              if (Custom->Path != Ptr->Path) {
                // Better path match
                BetterMatch = ((Ptr->Path.notEmpty()) && CustomPath.equal(Ptr->Path) &&
                               ((Custom->VolumeType == Ptr->VolumeType) ||
                                ((1ull<<Volume->DiskKind) & Custom->VolumeType) != 0));
              }
            } else if ((StrStr(Volume->DevicePathString.wc_str(), Custom->Volume.wc_str()) == NULL) &&
                       ((Volume->VolName.isEmpty()) || (StrStr(Volume->VolName.wc_str(), Custom->Volume.wc_str()) == NULL))) {
              if (Custom->Volume.isEmpty()) {
                // More precise volume match
                if (Custom->Path != Ptr->Path) {
                  // Better path match
                  BetterMatch = ((Ptr->Path.notEmpty()) && CustomPath.equal(Ptr->Path) &&
                                 ((Custom->VolumeType == Ptr->VolumeType) ||
                                  ((1ull<<Volume->DiskKind) & Custom->VolumeType) != 0));
                } else if (Custom->VolumeType != Ptr->VolumeType) {
                  // More precise volume type match
                  BetterMatch = ((Custom->VolumeType == 0) &&
                                 ((1ull<<Volume->DiskKind) & Custom->VolumeType) != 0);
                } else {
                  // Better match
                  BetterMatch = TRUE;
                }
              // Duplicate volume match
              } else if (Custom->Path != Ptr->Path) {
                // Better path match
                BetterMatch = ((Ptr->Path.notEmpty()) && CustomPath.equal(Ptr->Path) &&
                               ((Custom->VolumeType == Ptr->VolumeType) ||
                                ((1ull<<Volume->DiskKind) & Custom->VolumeType) != 0));
              // Duplicate path match
              } else if (Custom->VolumeType != Ptr->VolumeType) {
                // More precise volume type match
                BetterMatch = ((Custom->VolumeType == 0) &&
                               ((1ull<<Volume->DiskKind) & Custom->VolumeType) != 0);
              } else {
                // Duplicate entry
                BetterMatch = (i <= CustomIndex);
              }
            }
          // Duplicate volume match
          } else if (Custom->Path != Ptr->Path) {
            if (Ptr->Path.isEmpty()) {
              // Less precise path match
              BetterMatch = ((Custom->VolumeType != Ptr->VolumeType) &&
                             ((1ull<<Volume->DiskKind) & Custom->VolumeType) != 0);
            } else if (CustomPath.equal(Ptr->Path)) {
              if (Custom->Path.isEmpty()) {
                // More precise path and volume type match
                BetterMatch = ((Custom->VolumeType == Ptr->VolumeType) ||
                               ((1ull<<Volume->DiskKind) & Custom->VolumeType) != 0);
              } else if (Custom->VolumeType != Ptr->VolumeType) {
                // More precise volume type match
                BetterMatch = ((Custom->VolumeType == 0) &&
                               ((1ull<<Volume->DiskKind) & Custom->VolumeType) != 0);
              } else {
                // Duplicate entry
                BetterMatch = (i <= CustomIndex);
              }
            }
          // Duplicate path match
          } else if (Custom->VolumeType != Ptr->VolumeType) {
            // More precise volume type match
            BetterMatch = ((Custom->VolumeType == 0) &&
                           ((1ull<<Volume->DiskKind) & Custom->VolumeType) != 0);
          } else {
            // Duplicate entry
            BetterMatch = (i <= CustomIndex);
          }
          if (BetterMatch) {
            break;
          }
        }
        if (BetterMatch) {
          DBG("skipped because custom entry %llu is a better match and will produce a duplicate entry\n", i);
          continue;
        }
      }

      DBG("match!\n");
      // Create an entry for this volume
      Entry = CreateLoaderEntry(CustomPath, CustomOptions, Custom->FullTitle, Custom->Title, Volume, L""_XSW,
                                (Image.isEmpty() ? NULL : &Image), (DriveImage.isEmpty() ? NULL : &DriveImage),            
                                Custom->Type, Custom->Flags, Custom->Hotkey, Custom->BootBgColor, Custom->CustomBoot, &Custom->CustomLogo, 
                                /*(KERNEL_AND_KEXT_PATCHES *)(((UINTN)Custom) + OFFSET_OF(CUSTOM_LOADER_ENTRY, KernelAndKextPatches))*/ NULL, TRUE);
      if (Entry != NULL) {
        DBG("Custom settings: %ls.plist will %s be applied\n", Custom->Settings.wc_str(), Custom->CommonSettings?"not":"");
        if (!Custom->CommonSettings) {
          Entry->Settings = Custom->Settings;
        }
        if (OSFLAG_ISUNSET(Custom->Flags, OSFLAG_NODEFAULTMENU)) {
          Entry->AddDefaultMenu();
        } else if (Custom->SubEntries != NULL) {
          UINTN CustomSubIndex = 0;
          // Add subscreen
          REFIT_MENU_SCREEN *SubScreen = new REFIT_MENU_SCREEN;
          if (SubScreen) {
            SubScreen->Title.SWPrintf("Boot Options for %ls on %ls", (Custom->Title.notEmpty()) ? Custom->Title.wc_str() : CustomPath.wc_str(), Entry->VolName.wc_str());
            SubScreen->TitleImage = Entry->Image;
            SubScreen->ID = Custom->Type + 20;
            SubScreen->GetAnime();
            VolumeSize = RShiftU64(MultU64x32(Volume->BlockIO->Media->LastBlock, Volume->BlockIO->Media->BlockSize), 20);
            SubScreen->AddMenuInfoLine_f("Volume size: %lldMb", VolumeSize);
            SubScreen->AddMenuInfoLine_f("%ls", FileDevicePathToXStringW(Entry->DevicePath).wc_str());
            if (Guid) {
              SubScreen->AddMenuInfoLine_f("UUID: %s", strguid(Guid));
            }
            SubScreen->AddMenuInfoLine_f("Options: %s", Entry->LoadOptions.ConcatAll(" "_XS8).c_str());
            DBG("Create sub entries\n");
            for (CustomSubEntry = Custom->SubEntries; CustomSubEntry; CustomSubEntry = CustomSubEntry->Next) {
              if ( CustomSubEntry->Settings.isEmpty() ) {
                CustomSubEntry->Settings = Custom->Settings;
              }
              AddCustomEntry(CustomSubIndex++, (CustomSubEntry->Path.notEmpty()) ? CustomSubEntry->Path : CustomPath, CustomSubEntry, SubScreen);
            }
            SubScreen->AddMenuEntry(&MenuEntryReturn, true);
            Entry->SubScreen = SubScreen;
          }
        }
        if (IsSubEntry)
          SubMenu->AddMenuEntry(Entry, true);
        else
          MainMenu.AddMenuEntry(Entry, true);
      }
    } while (FindCustomPath && Custom->Type == OSTYPE_LINEFI && Custom->KernelScan == KERNEL_SCAN_ALL); // repeat loop only for kernel scanning

    // Close the kernel boot directory
    if (FindCustomPath && Custom->Type == OSTYPE_LINEFI) {
      DirIterClose(Iter);
    }
  }

}

// Add custom entries
VOID AddCustomEntries(VOID)
{
  CUSTOM_LOADER_ENTRY *Custom;
  UINTN                i = 0;
  
  if (!gSettings.CustomEntries) {
    return;
  }

  //DBG("Custom entries start\n");
  DbgHeader("AddCustomEntries");
  // Traverse the custom entries
  for (Custom = gSettings.CustomEntries; Custom; ++i, Custom = Custom->Next) {
    if ((Custom->Path.isEmpty()) && (Custom->Type != 0)) {
      if (OSTYPE_IS_OSX(Custom->Type)) {
        AddCustomEntry(i, MACOSX_LOADER_PATH, Custom, NULL);
      } else if (OSTYPE_IS_OSX_RECOVERY(Custom->Type)) {
        AddCustomEntry(i, L"\\com.apple.recovery.boot\\boot.efi"_XSW, Custom, NULL);
      } else if (OSTYPE_IS_OSX_INSTALLER(Custom->Type)) {
        UINTN Index = 0;
        while (Index < OSXInstallerPathsCount) {
          AddCustomEntry(i, OSXInstallerPaths[Index++], Custom, NULL);
        }
      } else if (OSTYPE_IS_WINDOWS(Custom->Type)) {
        AddCustomEntry(i, L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi"_XSW, Custom, NULL);
      } else if (OSTYPE_IS_LINUX(Custom->Type)) {
#if defined(ANDX86)
        for (UINTN Index = 0; Index < AndroidEntryDataCount; ++Index) {
          AddCustomEntry(i, AndroidEntryData[Index].Path, Custom, NULL);
        }
#endif
        AddCustomEntry(i, NullXStringW, Custom, NULL);
      } else if (Custom->Type == OSTYPE_LINEFI) {
        AddCustomEntry(i, NullXStringW, Custom, NULL);
      } else {
        AddCustomEntry(i, BOOT_LOADER_PATH, Custom, NULL);
      }
    } else {
      AddCustomEntry(i, Custom->Path, Custom, NULL);
    }
  }
  //DBG("Custom entries finish\n");
}
