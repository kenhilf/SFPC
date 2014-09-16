#ifndef _bmp_h_
#define _bmp_h_

//@TODO: still need to look into better bitmap loading code, or use some other file format

class AUX_RGBImageRec {
   void convertBGRtoRGB();
 public:
   byte* data;
   DWORD sizeX;
   DWORD sizeY;
   bool NoErrors;
   AUX_RGBImageRec(): NoErrors(false), data(NULL) {};
   AUX_RGBImageRec(const char *FileName);
   ~AUX_RGBImageRec();
   bool loadFile(const char *FileName);
   friend AUX_RGBImageRec *auxDIBImageLoad(const char *FileName);

   // added 11/4/09
   // Imported from SFPC DX version's bitmap loader
   bool Flip();
};

#endif //_bmp_h_