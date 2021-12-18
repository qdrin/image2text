#include <opencv2/opencv.hpp>
//#include <opencv2/text/ocr.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>


using namespace cv;
using namespace std;

struct ConfigParams {
  double minTextFillRate = 0.4;
  double maxTextFillRate = 0.9;
  int minLetterHeight = 8;
  int minLetterWidth = 8;
  double minThresh = 100.0;
  double maxThresh = 200.0;
  int apertureSize = 3;
  bool L2Gradient = true;
};

struct QLetter {
  Mat letter;
  Rect rect;
};

struct QWord {
  QWord(Rect _rect): rect(_rect), confidence(0.0) {}
  QWord(Rect _rect, String _word, float _conf = 0.0): rect(_rect), word(_word), confidence(_conf) {}
  QWord(): rect(Rect(0,0,0,0)), confidence(0.0) {}
  Rect rect;
  String word;
  float confidence;
};

class QImageToText {
private:
  ConfigParams m_config;
  const Mat m_image;
  Mat m_gray;
  vector<Rect> m_wordCandidates;
  vector<QWord> m_words;
  // static Ptr<text::OCRTesseract> ocr;

public:
  QImageToText(String filename);
  QImageToText(const Mat &_image);
  const Mat &image() { return m_image;}
  const vector<QWord> &words() {return m_words;}
  const vector<Rect> &wordCandidates() {return m_wordCandidates;}
  const vector<Rect> &detectWords();
  void clearWords() { m_words.clear(); }
  vector<QWord>::iterator candidateToWord(vector<Rect>::iterator it);
  vector<QWord>::iterator candidateToWord(int i);
  bool tessToText();
  // const vector<QLetter> &letters(QWord &);
};
