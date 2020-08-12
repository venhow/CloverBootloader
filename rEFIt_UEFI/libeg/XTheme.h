#if !defined(__XTHEME_H__)
#define __XTHEME_H__

#include "../cpp_foundation/XObjArray.h"
#include "../cpp_foundation/XString.h"
#include "libeg.h"
//#include "nanosvg.h"
#include "XImage.h"
#include "XIcon.h"
#include "XCinema.h"

#define INDICATOR_SIZE (52)

class XTheme
{
public:
  XObjArray<XIcon> Icons;
  EFI_FILE    *ThemeDir;

//  UINTN       DisableFlags;
  UINTN       HideBadges;
  UINTN       HideUIFlags;
//  BOOLEAN     TextOnly;
  FONT_TYPE   Font;
  INTN        CharWidth;
  UINTN       SelectionColor;
  XStringW    FontFileName;
  XStringW    Theme;
  XStringW    BannerFileName;
  XStringW    SelectionSmallFileName;
  XStringW    SelectionBigFileName;
  XStringW    SelectionIndicatorName;
  XStringW    DefaultSelection;
  XStringW    BackgroundName;
  SCALING     BackgroundScale;
  INTN        BackgroundSharp;
  BOOLEAN     BackgroundDark;
//  BOOLEAN     CustomIcons;
  BOOLEAN     SelectionOnTop;
  BOOLEAN     BootCampStyle;
  INTN        BadgeOffsetX;
  INTN        BadgeOffsetY;
  INTN        BadgeScale;
  INTN        ThemeDesignWidth;
  INTN        ThemeDesignHeight;
  INTN        BannerPosX;
  INTN        BannerPosY;
  INTN        BannerEdgeHorizontal;
  INTN        BannerEdgeVertical;
  INTN        BannerNudgeX;
  INTN        BannerNudgeY;
  BOOLEAN     VerticalLayout;
  BOOLEAN     NonSelectedGrey;
  INTN        MainEntriesSize;
  INTN        TileXSpace;
  INTN        TileYSpace;
//  INTN        IconFormat;
  BOOLEAN     Proportional;
//  BOOLEAN     ShowOptimus;
  BOOLEAN     embedded;
  BOOLEAN     DarkEmbedded;
  BOOLEAN     TypeSVG;
//  INTN        Codepage;  //no! it is global settings
//  INTN        CodepageSize;
  float       Scale;
  float       CentreShift;
  INTN        row0TileSize;
  INTN        row1TileSize;
  INTN        BanHeight;
  INTN        LayoutHeight; //it was 376 before
  INTN        LayoutBannerOffset;
  INTN        LayoutButtonOffset;
  INTN        LayoutTextOffset;
  INTN        LayoutAnimMoveForMenuX;
  INTN        ScrollWidth;
  INTN        ScrollButtonsHeight;
  INTN        ScrollBarDecorationsHeight;
  INTN        ScrollScrollDecorationsHeight;


  INTN  FontWidth;
  INTN  FontHeight;
  INTN  TextHeight;

  BOOLEAN     Daylight;

  XImage  Background; //Background and Banner will not be in array as they live own life
  XImage  BigBack; //it size is not equal to screen size will be scaled or cropped
  XImage  Banner; //same as logo in the array, make a link?
  XImage  SelectionImages[6];
  XImage  Buttons[4];
  XImage  ScrollbarBackgroundImage;
  XImage  BarStartImage;
  XImage  BarEndImage;
  XImage  ScrollbarImage;
  XImage  ScrollStartImage;
  XImage  ScrollEndImage;
  XImage  UpButtonImage;
  XImage  DownButtonImage;

  XImage  FontImage;

  EG_RECT  BannerPlace;

  XCinema Cinema;

  void *SVGParser;
  
  void Init();
  XTheme(); //default constructor
  XTheme(const XTheme&) = delete;
  XTheme& operator=(const XTheme&) = delete;

  ~XTheme();

  
  //fill the theme
//  const XImage& GetIcon(const char* Name);
//  const XImage& GetIcon(const CHAR16* Name);
  const XIcon& GetIcon(const XString8& Name);  //get by name
        XIcon* GetIconP(const XString8& Name);
  const XIcon& GetIcon(INTN Id); //get by id
        XIcon* GetIconP(INTN Id); //not const
        XIcon& GetIconAlt(INTN Id, INTN Alt); //if id not found
  const XIcon& LoadOSIcon(const CHAR16* OSIconName); //TODO make XString provider
  const XIcon& LoadOSIcon(const XString8& Full);
  bool CheckNative(INTN Id);
 
  //fonts
  void LoadFontImage(IN BOOLEAN UseEmbedded, IN INTN Rows, IN INTN Cols);
  void PrepareFont();
  INTN GetEmpty(const XImage& Buffer, const EFI_GRAPHICS_OUTPUT_BLT_PIXEL& FirstPixel, INTN MaxWidth, INTN Start, INTN Step);
  INTN RenderText(IN const XStringW& Text, OUT XImage* CompImage_ptr,
                    IN INTN PosX, IN INTN PosY, IN UINTN Cursor, INTN textType, float textScale = 0.f);
  //overload for UTF8 text
  INTN RenderText(IN const XString8& Text, OUT XImage* CompImage_ptr,
                          IN INTN PosX, IN INTN PosY, IN UINTN Cursor, INTN textType, float textScale = 0.f);
  VOID MeasureText(IN const XStringW& Text, OUT INTN *Width, OUT INTN *Height);


//  void AddIcon(XIcon& NewIcon);  //return EFI_STATUS?
  void FillByEmbedded();
  void FillByDir();
  EFI_STATUS GetThemeTagSettings(void* DictPointer);
  void parseTheme(void* p, const char** dict); //in nano project
  EFI_STATUS ParseSVGXTheme(const CHAR8* buffer); // in VectorTheme
  EFI_STATUS ParseSVGXIcon(INTN Id, const XString8& IconNameX, XImage* Image, void **SVGIcon);
  void* LoadTheme(const XStringW& TestTheme); //return TagPtr why?
  EFI_STATUS LoadSvgFrame(INTN i, OUT XImage* XFrame); // for animation

  //screen operations
  void ClearScreen();
  void FillRectAreaOfScreen(IN INTN XPos, IN INTN YPos, IN INTN Width, IN INTN Height);
//  void InitSelection();
  void InitBar();

protected:
  //internal layout variables instead of globals in menu.cpp

};
#endif
