/* ***************************************************************
\\ File Name:  Image.cpp
// Created By: Nick G. Toth
\\ E-Mail:     ntoth@pdx.edu
// Date:       July 2nd, 2017
\\
// Overview: Implementation for a lightweight wrapper for the
\\ OpenCV Mat class. This class is made specifically to be used
// in the CLImage tool.
\\
// ***************************************************************/

#include "Image.h"


// *************************** |
// Global Constant Definitions |
// *************************** V

// The number of valid file extensions.
// Size of the VALID_EXTENSIONS array.
const static uint EXTENSION_COUNT = 6;

// An array containing the valid formats and their attributes.
const static FormatTrip VALID_EXTENSIONS[EXTENSION_COUNT] = {
  // Portable Network Graphics.
  FormatTrip(".png",  CV_IMWRITE_PNG_COMPRESSION, 9),
  // Joint Photographic Experts Group.
  FormatTrip(".jpg",  CV_IMWRITE_JPEG_QUALITY, 100),
  FormatTrip(".jpeg", CV_IMWRITE_JPEG_QUALITY, 100),
  // Pixelmator.
  FormatTrip(".pbm",  CV_IMWRITE_PXM_BINARY, 1),
  FormatTrip(".pgm",  CV_IMWRITE_PXM_BINARY, 1),
  FormatTrip(".ppm",  CV_IMWRITE_PXM_BINARY, 1)
};


// ********************* |
// Image Implementation  |
// ********************* V

/* ****************************************************
\\ Initializes a new Image.
//
\\ @param filename: The name of the image's file.
//
\\ ****************************************************/
Image :: Image(const std::string & filename) : super(nullptr)
{
  // Initialize filename with the filename param.
  this->filename = filename;

  // Try to open the image. If that fails, set super to null.
  if(!openImage()) super = nullptr;

  return;
}


/* ****************************************************
\\ Copies an existing Image.
//
\\ @param img_src: The Image to be copied.
//
\\ ****************************************************/
Image :: Image(const Image & img_src) : filename(generateFilename(img_src.filename))
{
  // If the source Image's super pointer isn't null,
  // initialize super with its copy constructor.
  if(img_src.super) super = std::unique_ptr<cv::Mat>(new cv::Mat(*img_src.super));
  // Initialize super to null.
  else super = nullptr;

  return;
}


/* ****************************************************
\\ Deallocates all dynamic Image memory (i.e., Calls
// the cv::Mat (base class) destructor).
\\
// ****************************************************/
Image :: ~Image(void)
{
  // If super is not null, deallocate its memory.
  //if(super) delete super;
  super = nullptr;

  return;
}


/* ****************************************************
\\ Returns the width in pixels of the image.
//
\\ ****************************************************/
uint Image :: getWidth(void) const
{
  // If the image is not initialized,
  // there is no height. i.e., 0.
  if(!initialized()) return 0;

  // Return the width.
  return super->cols;
}


/* ****************************************************
\\ Returns the height in pixels of the image.
//
\\ ****************************************************/
uint Image :: getHeight(void) const
{
  // If the image is not initialized,
  // there is no height. i.e., 0.
  if(!initialized()) return 0;

  // Return the height.
  return super->rows;
}


/* ****************************************************
\\ Returns a unique pointer to an array of unsigned
// characters, containing the channel values for the
\\ pixel at the specified row and column.
//
\\ @param row: The row number for the pixel to be
// retrieved.
\\
// @param col: The column number for the pixel to be
\\ retrieved.
//
\\ ****************************************************/
std::unique_ptr<uint[]> Image :: getArrColors_int( uint row,
                                                   uint col ) const
{
  // Try to get the channel values as an array of characters.
  std::unique_ptr<uchar[]> arr_clrs = getArrColors(row, col);

  // If getArrColors failed (image is
  // uninitialized), return nullptr.
  if(!arr_clrs) return std::unique_ptr<uint[]>();

      // Number of channels for loop control
  int channels = super->channels(),
      // Index counter for moving channel
      // values from char array to int array.
      clr = 0;

  // Create an int array for storing the colors.
  std::unique_ptr<uint[]> arr_clrs_int(new uint[channels]);

  // Fill the chnnel int array with the
  // contents of the channels char array.
  for(; clr < channels; ++clr)
    arr_clrs_int[clr] = static_cast<uint>(arr_clrs[clr]);

  // Return the integer array of channel values.
  return std::move(arr_clrs_int);
}


/* ****************************************************
\\ Returns a unique pointer to an array of unsigned
// integers, containing the channel values for the
\\ pixel at the specified row and column. 
//
\\ @param row: The row number for the pixel to be
// retrieved.
\\
// @param col: The column number for the pixel to be
\\ retrieved.
//
\\ ****************************************************/
std::unique_ptr<uchar[]> Image :: getArrColors( uint row,
                                                uint col ) const
{
  // If no image is loaded, return an empty unique pointer.
  if(!initialized()) return std::unique_ptr<uchar[]>();

  // Depending on the number of channels used by the image,
  // return one of the getArrColors delegate functions.
  switch(super->channels())
  {
    // Return the 1-channel getArrColors function.
    case 1: return getArrColors_one(row, col, std::unique_ptr<uchar[]>(new uchar[1]));
    // Return the 2-channel getArrColors function.
    case 2: return getArrColors_two(row, col, std::unique_ptr<uchar[]>(new uchar[2]));
    // Return the 3-channel getArrColors function.
    case 3: return getArrColors_three(row, col, std::unique_ptr<uchar[]>(new uchar[3]));
    // Return the 4-channel getArrColors function.
    case 4: return getArrColors_four(row, col, std::unique_ptr<uchar[]>(new uchar[4]));
    // Return the 5-channel getArrColors function.
    case 5: return getArrColors_five(row, col, std::unique_ptr<uchar[]>(new uchar[5]));
    // Return an empty unique pointer.
    default: return std::unique_ptr<uchar[]>();
  }
}



/* ****************************************************
\\ Returns the sum of the rgb integer values for a
// pixel, divided by 3.
\\
// @param row: The row number for the pixel to be
\\ retrieved.
//
\\ @param col: The column number for the pixel to be
// retrieved.
\\
// @return: If the specified pixel exists, returns the
\\ sum of the three (red, blue, green) pixel values,
// divided by 3. Otherwise, return -1.
\\
// ****************************************************/
template <uint C>
double Image :: getPixelIntensity( uint row,
                                   uint col ) const
{
  // If the specified row and column
  // are out of range, report error code -2.
  if(!dimInRange(row, col)) return -2.0;

  // If the specified number of C
  // is incorrect, return error code -1.
  if(C != super->channels()) return -1.0;

       // Sum of the pixel's channel values
       // at the specified row & column.
  uint px_sum = 0,
       // Index counter for traversing
       // the pixel channel values.
       px_clr = 0;

  // Get the pixel at the specified row & column.
  cv::Vec<uchar, C> pixel = super->at< cv::Vec<uchar, C> >(row, col);

  // For each channel..
  for(; px_clr < C; ++px_clr)
    // Get the current pixel color, convert to int and add to sum.
    px_sum += static_cast<uint>(pixel[px_clr]);

  // Return the average value.
  return px_sum / C;
}


/* ****************************************************
\\ Sets the red value for a pixel.
//
\\ @param col: The column of the pixel to be modified.
//
\\ @param col: The row of the pixel to be modified.
//
\\ @param c_arr: The array of new channel values for
// the specified pixel.
\\
// @return: True if pixel was modified successfully.
\\
// ****************************************************/
template<int C>
bool Image :: setPixel( uint row,
                        uint col,
                        const cv::Vec<uchar, C> & c_vec )
{
  // If the image is uninitialized, or the specified
  // row and column are out of range, report failure.
  if(!initialized() || !dimInRange(row, col)) return false;

  // Index counter for traversing c_vec.
  int clr = 0;

  // Verify that the colors specified by
  // c_vec are within the valid range.
  while(clr < C && c_vec[clr] < 256) ++clr;

  // If a value greater than 255
  // was detected, report failure.
  if(clr != C) return false;

  // Reset c_vec index counter.
  clr = 0;

  // Add all of the values in c_vec to the
  // corresponding channel of the specified pixel.
  while(clr < C)
  {
    // Set the current value at the
    // specified pixel to the new color.
    super->at< cv::Vec<uchar, C> >(row, col)[clr] = c_vec[clr];

    // Increment the color index counter.
    ++clr;
  }

  // Report success.
  return true;
}


/* ****************************************************
\\ Sets the color values for a pixel, using an unsigned
// character array.
\\
// @param col: The column of the pixel to be modified.
\\
// @param col: The row of the pixel to be modified.
\\
// @param c_arr: The array of new channel values for
\\ the specified pixel.
//
\\ @return: True if pixel was modified successfully.
//
\\ ****************************************************/
bool Image :: setPixel( uint row,
                        uint col,
                        const uchar c_arr[] )
{
  // If the image is uninitialized, or the specified
  // row and column are out of range, report failure.
  if(!initialized() || !dimInRange(row, col)) return false;

      // Index counter for traversing c_vec.
  int clr = 0,
      // The number of channels used by
      // the image. For loop control.
      chans = super->channels();

  // Verify that the colors specified by
  // c_vec are within the valid range.
  while(clr < chans && static_cast<uint>(c_arr[clr]) < 256) ++clr;

  // If a value greater than 255
  // was detected, report failure.
  if(clr != chans) return false;

  // Reset c_vec index counter.
  clr = 0;

  // Depending on the number of channels used by the
  // image, add all of the values in c_vec to the
  // corresponding channel of the specified pixel.
  switch(super->channels())
  {
    case 1: while(clr < chans) { super->at< cv::Vec<uchar, 1> >(row, col)[clr] = c_arr[clr]; ++clr; } break;
    case 2: while(clr < chans) { super->at< cv::Vec<uchar, 2> >(row, col)[clr] = c_arr[clr]; ++clr; } break;
    case 3: while(clr < chans) { super->at< cv::Vec<uchar, 3> >(row, col)[clr] = c_arr[clr]; ++clr; } break;
    case 4: while(clr < chans) { super->at< cv::Vec<uchar, 4> >(row, col)[clr] = c_arr[clr]; ++clr; } break;
    case 5: while(clr < chans) { super->at< cv::Vec<uchar, 5> >(row, col)[clr] = c_arr[clr]; ++clr; } break;
    default: return false;
  }

  // Report success.
  return true;
}


/* ****************************************************
\\ Display the image in a new window.
//
\\ @return: True unless display failed.
//
\\ ****************************************************/
bool Image :: displayImage(void) const
{

  // If the image is uninitialized, report failure.
  if(!initialized()) return false;

  // Open the image in a new window,
  // using the filename as a title.
  cv::imshow(filename, *super);

  // Pause until the window is closed.
  cv::waitKey();

  // Report success.
  return true;
}


/* ****************************************************
\\ Open the image with the name specified by filename.
// 
\\ @param filename: The name of the image file to be
// opened.
\\
// @return: True unless open failed.
\\
// ****************************************************/
bool Image :: openImage(std::string filename_param)
{
  // If the image is already defined, or both filenames are empty
  if(initialized() || (filename_param.empty() && this->filename.empty()))
    // Report failure.
    return false;

  // If the filename param is not empty, and
  // the current filename member is empty..
  if(!filename_param.empty() && this->filename.empty())
  {
    // If a file with the name specified by filename param exists,
    // set the filename member to the value of the filename param.
    if(std::ifstream(filename_param))
      this->filename = filename_param;
    else return false;
  }
  // If the filename param is not empty, and
  // the current filename member is not empty..
  else if(!filename_param.empty() && !this->filename.empty())
  {
    // If the current filename member does not contain the
    // name of an existing file, and the filename param does,
    // set the filename member to the value of the filename param.
    if(!std::ifstream(this->filename) && std::ifstream(filename_param))
      this->filename = filename_param;
    else return false;
  }
  // If the filename param is empty, and the current
  // filename member is not empty, and no file with the
  // name in current filename member exists, report failure.
  else if(filename_param.empty() && !this->filename.empty() && !std::ifstream(this->filename))
    return false;

  // Initialize super by opening the image specified by the class filename.
  super = std::unique_ptr<cv::Mat>(new cv::Mat(imread(this->filename, cv::IMREAD_UNCHANGED)));

  // Report success.
  return true;
}


/* ****************************************************
\\ (Private) - Confirms whether or not row and col are
// in range of the Image's height and width. This
\\ function is called by all functions that take row
// and column parameters.
\\
// @param row: A specific row in the Image. Must be
\\ less than the height in pixels of the image.
//
\\ @param col: A specific column in the Image. Must be
// less than the width in pixels of the image.
\\
// @return: True if the specified row and column are
\\ within range of the Image's dimensions.
//
\\ ****************************************************/
inline bool Image :: dimInRange( int row,
                                 int col) const
{
  // If super contains no image, report failure.
  if(!initialized()) return false;

  // If the specified row is out of the Image's height range
  // or the specified column is out of the Image's width range..
  if( row < 0 || row >= super->rows ||
      col < 0 || col >= super->cols )
        // Report dims out of bounds.
        return false;

  // Report dims within bounds.
  return true;
}


/* *************************************************
\\ Confirms whether or not a filename contains a
// valid extension (valid extensions listed in the
\\ VALID_EXTENSIONS array).
//
\\ @param FILENAME: The FILENAME whose extension
// will be checked for validity.
\\
// @return -3: The FILENAME param param is empty.
\\
//         -2: The FILENAME contains no extension,
\\             or an extension and nothing else.
//
\\         -1: The FILENAME contains an invalid
//             extension.
\\
//          ∞: Otherwise returns the index of the
\\             extension in VALID_EXTENSIONS.
//
\\ *************************************************/
int Image :: hasValidExtension(const std::string & FILENAME) const
{
    // If the filename is empty, report error code -3.
    if(FILENAME.empty()) return -3;

        // Index counter for traversing the FILENAME.
    int ch = FILENAME.length() - 1,
        // Index counter for traversing the VALID_EXTENSIONS array.
        idx = 0;

    // Locate the dot in FILENAME.
    while( FILENAME.at(ch) != '.') --ch;

    // If FILENAME did not contain
    // a dot, report error code -2.
    if(ch == 0) return -2;

    // Get the extension from FILENAME.
    std::string ext = FILENAME.substr(ch, FILENAME.length() - 1);

    // Check for ext in VALID_EXTENSIONS.
    while(idx < EXTENSION_COUNT)
    {
        // If a match is found, return the index
        // of the matching file extension.
        if(!ext.compare(VALID_EXTENSIONS[idx].ext)) return idx;

        // Increment the extension index counter.
        ++idx;
    }

    // Report error code -1.
    return -1;
}


/* *************************************************
// Saves the image in a new file, with the name
\\ specified by this->filename.
//
\\ @param filename: The name of the file being saved.
//
\\ @return: True if save is successful.
//
\\ *************************************************/
bool Image :: saveImage(void)
{
  // If super does not contain a complete image,
  // or the filename is empty, report failure.
  if(!initialized() || this->filename.empty()) return false;

  // Check whether or not filename has a valid extension.
  int ext_idx = hasValidExtension(filename);

  // If filename does not have a
  // valid extension, report failure.
  if(ext_idx < 0 || ext_idx >= EXTENSION_COUNT)
  {
    // Remove the erroneous extension.
    removeExtension(filename);

    // Try to add a valid extension.
    // If that fails, report failure.
    if(!addExtension(filename)) return false;
  }

              // Get the compression format from
              // the matching file FormatTrip.
  const int COMPRESSION_FORMAT = VALID_EXTENSIONS[ext_idx].format,
              // Get the maximum quality from
              // the matching FormatTrip
              QUALITY = VALID_EXTENSIONS[ext_idx].quality;

  // Vector of params for image to be saved.
  std::vector<int> params;

  // Initialize params with the compression format and quality.
  params.push_back(COMPRESSION_FORMAT); params.push_back(QUALITY);

  // Try to save the image.
  try { cv::imwrite(filename, *super, params); }
  // If writing failed, catch the error.
  catch (std::runtime_error & ex)
  {
      // Display error message.
      fprintf(stderr, "Exception converting image format: %s\n", ex.what());
      // Report failure.
      return false;
  }

  // Report success.
  return true;
}


/* ****************************************************
\\ (Private) - Removes the extension from a filename.
//
\\ @param bad_filename: The filename which currently
// contains an invalid extension. Old extension will
\\ be removed.
//
\\ @return: True if the extension is removed successfully.
//
\\ ****************************************************/
bool Image :: removeExtension(std::string & bad_filename) const
{
  // If the filename is empty, report failure.
  if(bad_filename.empty()) return false;

      // Get the length of the filename for loop control.
  int s_len = bad_filename.length(),
      // Index counter for traversing the filename.
      ch = s_len - 1;

  // Locate the file type extension (if there is one).
  while(ch > 0 && bad_filename.at(ch) != '.') --ch;

  // If no extension was detected, report failure.
  if(ch == 0) return false;

  // Remove the extension.
  bad_filename.erase(ch, (s_len - ch));

  // Report successful removal.
  return true;
}


/* ****************************************************
\\ (Private) - Adds a format extension to a filename.
// Currently just adds PNG. This function is mostly
\\ here for future use. 
//
\\ @param incomplete_filename: The filename which is
// either missing an extension, or contains an invalid
\\ extension. Old extension will be removed (where
// applicable), and ".png" will be appended.
\\
// @return: True if the extension is added successfully.
\\
// ****************************************************/
bool Image :: addExtension(std::string & incomplete_filename) const
{
  // If the image is incomplete, report failure.
  if(!initialized()) return false;

  // If the filename already has
  // an extension, remove it.
  removeExtension(incomplete_filename);

  // Replace bad extension with png.
  incomplete_filename.append(".png");

  // Report success.
  return true;
}


/* ****************************************************
\\ (Private) - Generates a modified filename for the
// copy constructor to avoid duplicate filenames. For
\\ example, if an Image with the filename example.png
// is given to the Image copy constructor, this function
\\ will return example_1.png. Later, if example_1.png
// is given, this function will return example_2.png.
\\
// @param seed: The filename to be modified.
\\
// @return: A modified version of seed.
\\
// ****************************************************/
std::string Image :: generateFilename(std::string seed)
{
  // If the filename is empty, or its extension
  // is invalid, return an empty string.
  if(seed.empty() || !(hasValidExtension(seed) >= 0) ) return "";

      // Get the length of the filename for loop control.
  int s_len = seed.length(),
      // Index counter for locating the filename extension.
      ext_ch = s_len - 1,
      // Index counter for locating any preexisting
      // filename modification. e.g. If seed is a filename
      // that was previously generated by this function.
      mod_ch = 0;

  // Used to track whether or not seed
  // has been previously modified.
  bool first_mod = true;

  // Locate the file type extension (if there is one).
  while(ext_ch > 0 && seed.at(ext_ch) != '.') --ext_ch;

  // Get the index where seed's preexisting
  // modification (if it exists) ends.
  mod_ch = ext_ch - 1;
  
  // Modified filename extension.
  std::string mod = "";

  // If seed at mod_ch is a number (true,
  // if another modification exists)..
  if(isdigit(seed.at(mod_ch)))
  {
    // Check whether or not seed actually contains a
    // modification, and locate the index of its beginning.
    while(mod_ch >= 0 && isdigit(seed.at(mod_ch)))
    {
      // Insert the current digit at the beginning of mod.
      mod.insert(0,1, (seed.at(mod_ch)));

      // Go to the previous character seed character.
      --mod_ch;
    }

    // If a complete preexisting modification is found..
    if(mod_ch > 0 && seed.at(mod_ch) == '_')
    {
      // Add 1 to the previous modification number.
      mod = std::to_string(std::stoi(mod) + 1 );
      // Insert the modification delimiter.
      mod.insert(0, "_");

      // Keep track of the fact that seed
      // contains a preexisting modification. 
      first_mod = false;
    }
    // If the filename is ONLY a modification, and
    // a format extension, return the same string.
    else return seed;
  }

  // If seed did not already contain a
  // modification, add the first extension.
  if(first_mod) seed.insert(ext_ch, "_1");
  // If seed did already contain a modification,
  // replace it with the incremented modification.
  else seed.replace(mod_ch, (ext_ch - mod_ch), mod);

  // Return the modified filename.
  return seed;
}


// PRIVATE getArrColors DELEGATE FUNCTIONS!
// |                  |                   |
// V                  V                   V

// (Private) - Delegate of getArrColors. (1 Channel).
std::unique_ptr<uchar[]> Image :: getArrColors_one( uint row,
                                                    uint col,
                                                    std::unique_ptr<uchar[]> channel_arr ) const
{
  // Get the pixel at the specified row & column.
  cv::Vec<uchar, 1> pixel = super->at< cv::Vec<uchar, 1> >(row, col);

  // Add the only channel value
  // to the channel value array.
  channel_arr[0] = pixel[0];

  // Return the channel value.
  return std::move(channel_arr);
}
// (Private) - Delegate of getArrColors. (2 Channels).
std::unique_ptr<uchar[]> Image :: getArrColors_two( uint row,
                                                    uint col,
                                                    std::unique_ptr<uchar[]> channel_arr ) const
{
  // Get the pixel at the specified row & column.
  cv::Vec<uchar, 2> pixel = super->at< cv::Vec<uchar, 2> >(row, col);

  // Fill the chnnel array with the channel
  // values at the specified pixel.
  for(uint clr_idx = 0; clr_idx < 2; ++clr_idx)
    channel_arr[clr_idx] = pixel[clr_idx];

  // Return the character array of channel values.
  return std::move(channel_arr);
}
// (Private) - Delegate of getArrColors (3 Channels).
std::unique_ptr<uchar[]> Image :: getArrColors_three( uint row,
                                                      uint col,
                                                      std::unique_ptr<uchar[]> channel_arr ) const
{
  // Get the pixel at the specified row & column.
  cv::Vec<uchar, 3> pixel = super->at< cv::Vec<uchar, 3> >(row, col);

  // Fill the chnnel array with the channel
  // values at the specified pixel.
  for(uint clr_idx = 0; clr_idx < 3; ++clr_idx)
    channel_arr[clr_idx] = pixel[clr_idx];

  // Return the character array of channel values.
  return std::move(channel_arr);
}
// (Private) - Delegate of getArrColors (4 Channels).
std::unique_ptr<uchar[]> Image :: getArrColors_four( uint row,
                                                     uint col,
                                                     std::unique_ptr<uchar[]> channel_arr ) const
{
  // Get the pixel at the specified row & column.
  cv::Vec<uchar, 4> pixel = super->at< cv::Vec<uchar, 4> >(row, col);

  // Fill the chnnel array with the channel
  // values at the specified pixel.
  for(uint clr_idx = 0; clr_idx < 4; ++clr_idx)
    channel_arr[clr_idx] = pixel[clr_idx];

  // Return the character array of channel values.
  return std::move(channel_arr);
}
// (Private) - Delegate of getArrColors (5 Channels).
std::unique_ptr<uchar[]> Image :: getArrColors_five( uint row,
                                                     uint col,
                                                     std::unique_ptr<uchar[]> channel_arr ) const
{
  // Get the pixel at the specified row & column.
  cv::Vec<uchar, 5> pixel = super->at< cv::Vec<uchar, 5> >(row, col);

  // Fill the chnnel array with the channel
  // values at the specified pixel.
  for(uint clr_idx = 0; clr_idx < 5; ++clr_idx)
    channel_arr[clr_idx] = pixel[clr_idx];

  // Return the character array of channel values.
  return std::move(channel_arr);
}
