#pragma once

#include "pch.h"

IFNITY_NAMESPACE

using namespace rhi;



/// Bitmap formats based in 3D Rendering Cookbook by Packt Publishing Chapter 3. 
/// R/RG/RGB/RGBA bitmaps
struct IFNITY_API Bitmap
{
   /// Default constructor
   Bitmap() = default;

   /**
    * @brief Constructor for 2D bitmap
    * @param w Width of the bitmap
    * @param h Height of the bitmap
    * @param comp Number of components per pixel
    * @param fmt Format of the bitmap
    */
   Bitmap(int w, int h, int comp, eBitmapFormat fmt)
       :w_(w), h_(h), comp_(comp), fmt_(fmt), data_(w* h* comp* getBytesPerComponent(fmt))
   {
       initGetSetFuncs();
   }

   /**
    * @brief Constructor for 3D bitmap
    * @param w Width of the bitmap
    * @param h Height of the bitmap
    * @param d Depth of the bitmap
    * @param comp Number of components per pixel
    * @param fmt Format of the bitmap
    */
   Bitmap(int w, int h, int d, int comp, eBitmapFormat fmt)
       :w_(w), h_(h), d_(d), comp_(comp), fmt_(fmt), data_(w* h* d* comp* getBytesPerComponent(fmt))
   {
       initGetSetFuncs();
   }

   /**
    * @brief Constructor for 2D bitmap with data
    * @param w Width of the bitmap
    * @param h Height of the bitmap
    * @param comp Number of components per pixel
    * @param fmt Format of the bitmap
    * @param ptr Pointer to the data
    */
   Bitmap(int w, int h, int comp, eBitmapFormat fmt, const void* ptr)
       :w_(w), h_(h), comp_(comp), fmt_(fmt), data_(w* h* comp* getBytesPerComponent(fmt))
   {
       initGetSetFuncs();
       memcpy(data_.data(), ptr, data_.size());
   }

   int w_ = 0; ///< Width of the bitmap
   int h_ = 0; ///< Height of the bitmap
   int d_ = 1; ///< Depth of the bitmap (default is 1 for 2D bitmaps)
   int comp_ = 3; ///< Number of components per pixel
   eBitmapFormat fmt_ = eBitmapFormat_UnsignedByte; ///< Format of the bitmap
   eBitmapType type_ = eBitmapType_2D; ///< Type of the bitmap (default is 2D)
   std::vector<uint8_t> data_; ///< Data of the bitmap

   /**
    * @brief Get the number of bytes per component based on the format
    * @param fmt Format of the bitmap
    * @return Number of bytes per component
    */
   static int Bitmap::getBytesPerComponent(eBitmapFormat fmt)
   {
       if(fmt == eBitmapFormat_UnsignedByte) return 1;
       if(fmt == eBitmapFormat_Float) return 4;
       return 0;
   }

   /**
    * @brief Set the pixel color at the specified position
    * @param x X coordinate of the pixel
    * @param y Y coordinate of the pixel
    * @param c Color to set
    */
   void setPixel(int x, int y, const glm::vec4& c)
   {
       (*this.*setPixelFunc)(x, y, c);
   }

   /**
    * @brief Get the pixel color at the specified position
    * @param x X coordinate of the pixel
    * @param y Y coordinate of the pixel
    * @return Color of the pixel
    */
   glm::vec4 getPixel(int x, int y) const
   {
       return ((*this.*getPixelFunc)(x, y));
   }

private:
   using setPixel_t = void(Bitmap::*)(int, int, const glm::vec4&);
   using getPixel_t = glm::vec4(Bitmap::*)(int, int) const;
   setPixel_t setPixelFunc = &Bitmap::setPixelUnsignedByte;
   getPixel_t getPixelFunc = &Bitmap::getPixelUnsignedByte;

   /**
    * @brief Initialize the function pointers for setting and getting pixels
    */
   void initGetSetFuncs()
   {
       switch(fmt_)
       {
       case eBitmapFormat_UnsignedByte:
           setPixelFunc = &Bitmap::setPixelUnsignedByte;
           getPixelFunc = &Bitmap::getPixelUnsignedByte;
           break;
       case eBitmapFormat_Float:
           setPixelFunc = &Bitmap::setPixelFloat;
           getPixelFunc = &Bitmap::getPixelFloat;
           break;
       }
   }

   /**
    * @brief Set the pixel color at the specified position for float format
    * @param x X coordinate of the pixel
    * @param y Y coordinate of the pixel
    * @param c Color to set
    */
   void setPixelFloat(int x, int y, const glm::vec4& c)
   {
       const int ofs = comp_ * (y * w_ + x);
       float* data = reinterpret_cast<float*>(data_.data());
       if(comp_ > 0) data[ ofs + 0 ] = c.x;
       if(comp_ > 1) data[ ofs + 1 ] = c.y;
       if(comp_ > 2) data[ ofs + 2 ] = c.z;
       if(comp_ > 3) data[ ofs + 3 ] = c.w;
   }

   /**
    * @brief Get the pixel color at the specified position for float format
    * @param x X coordinate of the pixel
    * @param y Y coordinate of the pixel
    * @return Color of the pixel
    */
   glm::vec4 getPixelFloat(int x, int y) const
   {
       const int ofs = comp_ * (y * w_ + x);
       const float* data = reinterpret_cast<const float*>(data_.data());
       return glm::vec4(
           comp_ > 0 ? data[ ofs + 0 ] : 0.0f,
           comp_ > 1 ? data[ ofs + 1 ] : 0.0f,
           comp_ > 2 ? data[ ofs + 2 ] : 0.0f,
           comp_ > 3 ? data[ ofs + 3 ] : 0.0f);
   }

   /**
    * @brief Set the pixel color at the specified position for unsigned byte format
    * @param x X coordinate of the pixel
    * @param y Y coordinate of the pixel
    * @param c Color to set
    */
   void setPixelUnsignedByte(int x, int y, const glm::vec4& c)
   {
       const int ofs = comp_ * (y * w_ + x);
       if(comp_ > 0) data_[ ofs + 0 ] = uint8_t(c.x * 255.0f);
       if(comp_ > 1) data_[ ofs + 1 ] = uint8_t(c.y * 255.0f);
       if(comp_ > 2) data_[ ofs + 2 ] = uint8_t(c.z * 255.0f);
       if(comp_ > 3) data_[ ofs + 3 ] = uint8_t(c.w * 255.0f);
   }

   /**
    * @brief Get the pixel color at the specified position for unsigned byte format
    * @param x X coordinate of the pixel
    * @param y Y coordinate of the pixel
    * @return Color of the pixel
    */
   glm::vec4 getPixelUnsignedByte(int x, int y) const
   {
       const int ofs = comp_ * (y * w_ + x);
       return glm::vec4(
           comp_ > 0 ? float(data_[ ofs + 0 ]) / 255.0f : 0.0f,
           comp_ > 1 ? float(data_[ ofs + 1 ]) / 255.0f : 0.0f,
           comp_ > 2 ? float(data_[ ofs + 2 ]) / 255.0f : 0.0f,
           comp_ > 3 ? float(data_[ ofs + 3 ]) / 255.0f : 0.0f);
   }
};

IFNITY_END_NAMESPACE