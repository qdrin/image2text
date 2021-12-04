#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

struct QLetter {
  Mat letter;
  Rect rect;
};


class QImageToText {
private:
  Mat image;
  Mat gray;
  vector<QLetter> words;

public:
  void wordCandidates(
    Mat image,
    vector<Rect> &res,
    double min_text_fill = 0.4,
    double max_text_fill = 0.9,
    int min_text_height = 8,
    int min_text_width = 8
  );
  void letters(const Mat& wordImage, vector<Mat*> &res, double thresh = 100.0, double m_thresh = 200.0);
};

void textCandidates(
  Mat image,
  vector<Rect> &res,
  double min_text_fill = 0.4,
  double max_text_fill = 0.9,
  int min_text_height = 8,
  int min_text_width = 8
);

void textContours(const Mat& image, vector<Mat*> &res, double thresh = 100.0, double m_thresh = 200.0);
