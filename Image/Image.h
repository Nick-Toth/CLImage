/* ***************************************************************
\\ File Name:  Image.h
// Created By: Nick G. Toth
\\ E-Mail:     ntoth@pdx.edu
// Date:       July 2nd, 2017
\\
// Overview: Declaration for a lightweight OpenCV Mat wrapper.
\\ This class is made specifically to be used in the CLImage tool.
// See Image.cpp for more information.
\\
// ***************************************************************/

#ifndef IMAGE_WRAP_H
#define IMAGE_WRAP_H

// For cv::Mat params.
#include <vector>
// For verifying the existence of files.
#include <fstream>
#include <memory>
#include <cstring>
#include <string>
#include <cmath>

// Import the OpenCV library.
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"


/* *************************************************
\\ A wrapper for the OpenCV (Computer Vision) Mat
// class.
\\
// *************************************************/
typedef class Image
{
  public:

    // Initializes the Image.
    Image(const std::string & filename = "");
    // Copies the contents of another Image.
    Image(const Image & img_src);
    // Deallocates all dynamic Image memory.
    ~Image(void);

    // ********************* |
    // Retrieval Operations  |
    // ********************* V

    // Returns the width in pixels of the image.
    uint getWidth(void) const;
    // Returns the height in pixels of the image.
    uint getHeight(void) const;

    // Returns a unique pointer to an array of unsigned
    // characters, containing the channel values for
    // the pixel at the specified row and column.
    std::unique_ptr<uint[]> getArrColors_int( uint row,
                                              uint col ) const;

    // Returns one of the getArrColors_X functions, depending
    // on the number of channels used in the image. The delegate
    // function returns a unique pointer to an array of unsigned
    // integers, which contain the channel values.
    std::unique_ptr<uchar[]> getArrColors( uint row,
                                           uint col ) const;

    // Returns the sum of the channel values for
    // a pixel, divided by the number of channels.
    template <uint C>
    double getPixelIntensity( uint row,
                              uint col ) const;

    // If the Image was loaded from an external image file, or
    // created and saved during the life of the program in which
    // this function is called, that file's name (with extension) is
    // returned. Otherwise, the default generated name is returned
    // with an asterisk and no extension. e.g. *new_img_file.
    std::string getFilename(void) const { return std::string(filename); }

    // ***************************************** |
    // Initialization / Modification Operations  |
    // ***************************************** V

    // Open the image with the name specified
    // by filename True if opened successfully..
    bool openImage(std::string filename = "");

    // Save the image to a new/existing file.
    bool saveImage(void);

    // Sets the color values for
    // a pixel, using a cv::Vec.
    template<int C>
    bool setPixel( uint row,
                   uint col,
                   const cv::Vec<uchar, C> & c_vec );
    // Sets the color values for a pixel,
    // using an unsigned character array.
    bool setPixel( uint row,
                   uint col,
                   const uchar c_arr[] );
    // Sets the color values for a pixel,
    // using an unsigned integer array.
    bool setPixel( uint row,
                   uint col,
                   const uint c_arr[] )
    { return setPixel(row, col, reinterpret_cast<const uchar *>(c_arr)); }

    // ************************ |
    // Miscelaneous Operations  |
    // ************************ V

    // Display the image in a new window.
    bool displayImage(void) const;

    // Verifies whether a filename contains
    // an extension for a valid file type.
    int hasValidExtension(const std::string & FILENAME) const;

    // Confirms whether or not an
    // Image has been initialized.
    bool initialized(void) const
    { return (super && super->data); }

    // Returns the number of
    // channels in the image.
    uint getChannels(void) const
    { if(super) return super->channels(); return 0; }

  private:

    // Removes the extension from a filename.
    bool removeExtension(std::string & incomplete_filename) const;

    // Adds a file type extension based on image data.
    bool addExtension(std::string & incomplete_filename) const;

    // Returns true if the specified row and column are
    // within the range of the Image's width and height.
    inline bool dimInRange( int row,
                            int col ) const;

    // Generates a modified filename,
    // for the copy constructor.
    std::string generateFilename(std::string seed);

    // Delegate of getArrColors. (1 Channel).
    std::unique_ptr<uchar[]> getArrColors_one( uint row,
                                               uint col,
                                               std::unique_ptr<uchar[]> channel_arr ) const;
    // Delegate of getArrColors. (2 Channels).
    std::unique_ptr<uchar[]> getArrColors_two( uint row,
                                               uint col,
                                               std::unique_ptr<uchar[]> channel_arr ) const;
    // Delegate of getArrColors. (3 Channels).
    std::unique_ptr<uchar[]> getArrColors_three( uint row,
                                                 uint col,
                                                 std::unique_ptr<uchar[]> channel_arr ) const;
    // Delegate of getArrColors. (4 Channels).
    std::unique_ptr<uchar[]> getArrColors_four( uint row,
                                                uint col,
                                                std::unique_ptr<uchar[]> channel_arr ) const;
    // Delegate of getArrColors (5 Channels).
    std::unique_ptr<uchar[]> getArrColors_five( uint row,
                                                uint col,
                                                std::unique_ptr<uchar[]> channel_arr ) const;

    // The name of the image file.
    std::string filename;

    // Pointer to the Image's parent class.
    //cv::Mat * super;
    std::unique_ptr<cv::Mat> super;

} Img;

struct FormatTrip
{
  // Initialize all fields with the specified format into.
  FormatTrip(std::string e, uint f, uint q) :
  ext(e),  format(f),  quality(q)  { return; }

  // File extension for the compression
  // format specified by this FormatTrip.
  const std::string ext;

             // OpenCV compression format code.
  const uint format,
             // Maximum quality for
             // the compression format
             quality;
};

#endif // IMAGE_WRAP_H
