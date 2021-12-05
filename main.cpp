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

  vector<QWord> words = work.wordCandidates();

  for(vector<QWord>::iterator i=words.begin(); i != words.end(); i++) {
    rectangle(work.image(), i->rect, Scalar(0, 255, 0), 1);
  }
  
  vector<Mat> wordWindows;
  int num = 0;
  for(vector<QWord>::iterator i=words.begin(); i != words.end(); i++, num++) {
    const vector<QLetter> &wl = work.letters(*i);
    wName = "word" + to_string(num);
    cout << num << ": " << wName << ", letters: " << wl.size() << endl;
    Mat w = Mat::zeros(Size(i->rect.width, i->rect.height), CV_8UC1);
    wordWindows.insert(wordWindows.end(), w);
    for(vector<QLetter>::const_iterator l=wl.begin(); l != wl.end(); l++) {
      l->letter.copyTo(w(l->rect));
    }
    namedWindow(wName);
    imshow(wName, w);
  }
  wName = "Ерундульки";
  namedWindow(wName);
  imshow(wName, work.image());
  waitKey(0);
  destroyAllWindows();

  return 0;
}
