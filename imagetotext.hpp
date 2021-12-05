#include <opencv2/opencv.hpp>

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
  QWord(Rect _rect): rect(_rect) {}
  Rect rect;
  vector<QLetter> letters;
};

class QImageToText {
private:
  ConfigParams m_config;
  Mat m_image;
  Mat m_gray;
  vector<QWord> m_words;

public:
  bool loadImage(String filename);
  const Mat &image() { return m_image;}
  const vector<QWord> &words() {return m_words;}
  const vector<QWord> &wordCandidates();
  const vector<QLetter> &letters(QWord &);
};
