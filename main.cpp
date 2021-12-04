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
  String wName;
  QImageToText work;

  if(! work.loadImage(filename)) {
    cout << "could not load file " << filename;
    exit(1);
  }

  work.wordCandidates();

  vector<QWord> words = work.words();
  for(vector<QWord>::iterator i=words.begin(); i != words.end(); i++) {
    rectangle(work.image(), i->rect, Scalar(0, 255, 0), 1);
  }
  wName = "Ерундульки";
  namedWindow(wName);
  imshow(wName, work.image());
  waitKey(0);
  destroyAllWindows();

  return 0;
}
