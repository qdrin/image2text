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
  float minConfidence = 80.0;
  String filename = "./erundulki.jpg";
  Mat img;
  img = imread(filename.c_str());
  cv::String wName;
  QImageToText work(img);

  vector<Rect> wordRects = work.detectWords();
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

  // For tessToText method we load image from file (just to test this loading method)
  QImageToText workTess(filename);
  if(workTess.tessToText()) {
    vector<QWord> twords = workTess.words();
    cout << "Tesseract text:\n";
    for(vector<QWord>::iterator i=twords.begin(); i != twords.end(); i++) {
      if(i->confidence > minConfidence) {
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
