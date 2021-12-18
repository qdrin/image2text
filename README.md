# QImage2Text class. Text words detect and representing them as cv:String's
We are using [OpenCV](https://opencv.org/) library to work with images
We are using [Tesseract](https://github.com/tesseract-ocr/tesseract) provided with opencv-contrib part of OpenCV 
## Necessary dependencies
### opencv: library and opencv-contrib (for Tesseract)
* Install on Ubuntu:
  -  apt install libopencv-contrib4.5
  -  apt install libopencv-core-dev
  -  apt install libopencv-core4.5

* Other Linux distrib's:
  see https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html

* Install on Windows:
  see https://docs.opencv.org/4.x/d3/d52/tutorial_windows_install.html

## Example using this repo

### creating QImageToText object from file
```c++
  #include "imagetotext.hpp"
  using namespace std;
  using namespace cv;

  String filename = "./erundulki.jpg";
  QImageToText workTess(filename);
```
### creating QImageToText object from existing cv::Mat
  ```c++
  #include "imagetotext.hpp"
  using namespace std;
  using namespace cv;

  Mat img;
  // Here we make the image not empty

  QImageToText work(img);
  ```
### Recognise image's words with Tesseract algorithm upon the whole image
  ```c++
  #include "imagetotext.hpp"
  using namespace std;
  using namespace cv;

  Mat someImage;
  // Fill someImage with sence
  QImageToText workTess(someImage);

  if(workTess.tessToText()) {
    vector<QWord> twords = workTess.words();
    cout << "Tesseract text:\n";
    for(vector<QWord>::iterator i=twords.begin(); i != twords.end(); i++) {
      // QWord struct consists of confidence, cv::Rect, and cv::String 
      if(i->confidence > minConfidence) {
        rectangle(work.image(), i->rect, Scalar(0, 255, 0), 1);
        cout << "tessword: " << i->word << ", confidence: " << i->confidence << ", rect: " << i->rect << endl;
      }
    }
  }
  ```
### Detecting and recognizing particular words
  ```c++
  #include "imagetotext.hpp"
  using namespace std;
  using namespace cv;

  float minConfidence = 80.0;  // Used to filter noise
  cv::Mat someImage;
  // Fill someImage with sence
  QImageToText work(someImage);

  // Detecting candidates-to-be-words in the image
  vector<Rect> wordRects = work.detectWords();

  // Apply Tesseract OCR to every rectangle in wordRects
  // result words can also be found in work.words()
  for(int i = 0; i < wordRects.size(); i++) {
    QWord &w = *(work.candidateToWord(i));
  }

  for(vector<QWord>::const_iterator i=work.words().begin(); i != work.words().end(); i++) {
    if(i->confidence > minConfidence) {
      cout << "cvword: " << i->word << ", confidence: " << i->confidence << ", rect: " << i->rect << endl;
    }
  }

  // Another form for working with words
  const vector<QWord> &words = work.words();
  for(int i=0; i < words.size(); i++) {
    if(words[i].confidence > minConfidence) {
      cout << "cvword2: " << words[i].word << ", confidence: " << words[i].confidence << ", rect: " << words[i].rect << endl;
    }
  }
  ```
