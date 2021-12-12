#include <opencv2/opencv.hpp>
#include "imagetotext.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
  String filename = "./erundulki.jpg";
  cv::String wName;
  QImageToText work;
  if(! work.loadImage(filename)) {
    cout << "could not load file " << filename;
    exit(1);
  }

  vector<Rect> wordRects = work.detectWords();
  for(int i = 0; i < wordRects.size(); i++) {
    QWord &w = *(work.candidateToWord(i));
  }

  for(vector<QWord>::const_iterator i=work.words().begin(); i != work.words().end(); i++) {
    if(i->confidence > 80.0) {
      cout << "cvword: " << i->word << ", confidence: " << i->confidence << ", rect: " << i->rect << endl;
    }
  }
  
  if(work.tessToText()) {
    vector<QWord> twords = work.words();
    cout << "Tesseract text:\n";
    for(vector<QWord>::iterator i=twords.begin(); i != twords.end(); i++) {
      if(i->confidence > 80.0) {
        rectangle(work.image(), i->rect, Scalar(0, 0, 255), 1);
        cout << "tessword: " << i->word << ", confidence: " << i->confidence << ", rect: " << i->rect << endl;
      }
    }
  }

  for(vector<Rect>::iterator i=wordRects.begin(); i != wordRects.end(); i++) {
    rectangle(work.image(), *i, Scalar(0, 255, 0), 1);
  }

  wName = "Ерундульки";
  namedWindow(wName);
  imshow(wName, work.image());
  waitKey(0);
  destroyAllWindows();

  return 0;
}
