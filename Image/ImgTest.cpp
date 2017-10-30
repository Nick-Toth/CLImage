/* ***************************************************************
\\ File Name:  ImgTest.cpp
// Created By: Nick G. Toth
\\ E-Mail:     ntoth@pdx.edu
// Date:       Oct 30th, 2017
\\
// Overview: This file contains a a basic demonstration of the use
\\ of the Image class (Image.cpp) for use in CLImage.
//
\\ ***************************************************************/

#include "Image.h"
#include <iostream>

using namespace std;
using namespace cv;


// Default image file for testing the Image class.
const string DEFAULT_IMG_FILENAME = "mario.png";

// Allocate, initialize a new Image.
Img * setupImage(int argc, char * argv[]);

// Takes the name of an image file for testing. Uses mario.png as default.
int main(int argc, char * argv[])
{
  // ---------------------------------------------------------
  // Setup a new image.

  // The image object to be tested.
  Img * test_img = setupImage(argc, argv);

  // If setup failed..
  if(test_img == nullptr)
  {
    // Display failure alert
    cout << "\n  No image could be opened. Invalid file names!"
         << endl << endl;
    // Exit with error code 1.
    return 1;
  }

  // Display success alert.
  cout << "\n  Successfully opened:  " << test_img->getFilename() << endl;


  // ---------------------------------------------------------
  // Test #1 - Display the image.

  cout << "\n  Displaying image in a new window.." << endl;
  // Try to display the new image. Store return value.
  if(!test_img->displayImage())
  {
    // Display failure alert.
    cout << "\n  The image could not be displayed! Exiting program.." << endl;
    // Exit with error code 2.
    return 2;
  }

  // ---------------------------------------------------------
  // Test #2 - Retrieve image attributes.

  cout << "\n  Displaying image attributes." << endl;

  unique_ptr<uint[]> channels = test_img->getArrColors_int(200, 150);

  short idx = 0,
        chs = test_img->getChannels();

  cout << boolalpha
       << "\n    Height(rows) => " << test_img->getHeight()
       << "\n    Width(cols)  => " << test_img->getWidth()
       << "\n    Channel #    => " << test_img->getChannels()
       << "\n    Channels:";

  if(channels)
    for(; idx < chs; ++idx)
      cout << "\n      [" << idx << "] => " << static_cast<uint>(channels[idx]);

  cout << "\n\n" << endl;

  // ---------------------------------------------------------
  // Test #3 - Save image.
  
  std::cout << "\n  COPYING IMAGE!" << std::endl;

  // Make a copy of the test image.
  Image copy_img(*test_img);

  std::cout << "\n    SUCCESSFULLY COPIED IMAGE!\n\n  SAVING IMAGE!" << std::endl;

  // 
  if(copy_img.saveImage())
    std::cout << "\n    SUCCESSFULLY SAVED IMAGE WITH FILENAME : " << copy_img.getFilename() << std::endl;
  else std::cout << "\n    FAILED TO SAVE IMAGE!" << std::endl;

  delete test_img;

  return 0;
}


/* *************************************************
\\
//
\\
// *************************************************/
Img* setupImage(int argc, char * argv[])
{
  // Create a new Image pointer.
  Img * new_img = new Img;

  // Location for the return value of the
  // Image class's openImage function. True
  // if the Image was opened successfully.
  bool img_opened = false;

  // If at least one argument was given to main..
  if(argc > 1)
    // Try to open an image using the (presumed)
    // image filename arg. Store the result.
    img_opened = new_img->openImage(argv[1]);

  // If an Image was opened successfully using
  // a filename in argv, return the new Image.
  if(img_opened || !new_img) return new_img;

  new_img->openImage(DEFAULT_IMG_FILENAME);

  // Return the Image (or nullptr if the default failed).
  return new_img;
}