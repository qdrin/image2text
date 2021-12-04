#include "text_detection.hpp"

using namespace cv;
using namespace std;

// Сортировка контуров слева направо
bool leftOrder(const QLetter a, const QLetter b)
{
  return a.rect.x < b.rect.x;
}

void textCandidates(
    Mat image,
    vector<Rect> &res,
    double min_text_fill, double max_text_fill,
    int min_text_height, int min_text_width)
{
  Mat gray, grad, imgTh, connected;
  Mat kernel_3x3 = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
  Mat kernel_9x1 = getStructuringElement(MORPH_RECT, Size(9, 1));
  cvtColor(image, gray, COLOR_BGR2GRAY);

  morphologyEx(gray, grad, MORPH_GRADIENT, kernel_3x3);
  threshold(grad, imgTh, 0, 255, THRESH_BINARY | THRESH_OTSU);
  morphologyEx(imgTh, connected, MORPH_CLOSE, kernel_9x1);

  vector<vector<Point>> contours;
  vector<Vec4i> hierarchy;
  findContours(connected, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
  Mat drawing = Mat::zeros(imgTh.size(), CV_8UC1);
  Scalar color = Scalar(255, 255, 255);
  for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
  {
    Rect rect = boundingRect(contours[idx]);
    Mat maskROI(drawing, rect);
    maskROI = Scalar(0, 0, 0);
    drawContours(drawing, contours, idx, color, FILLED);
    double r = static_cast<double>(countNonZero(maskROI)) / (rect.width * rect.height);
    if (
        r > min_text_fill && r < max_text_fill &&
        rect.height > min_text_height && rect.width > min_text_width)
    {
      res.insert(res.end(), rect);
    }
  }
}

void textContours(const Mat &image, vector<Mat *> &res, double thresh, double m_thresh)
{
  int hole; // Номер элемента промежуточного слоя (между родителем и реальными потомками)
  Mat gray, canny_out, imgTmp;
  Mat kernel_3x3 = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
  Scalar color = Scalar(255, 255, 255), back = Scalar(0, 0, 0);
  cvtColor(image, gray, COLOR_BGR2GRAY);
  // GaussianBlur(gray, canny_out, Size(3, 3), 3);
  blur(gray, canny_out, Size(3, 3));
  Canny(canny_out, canny_out, thresh, m_thresh, 3, true);
  vector<vector<Point>> contours;
  vector<Vec4i> hierarchy;
  // morphologyEx(canny_out, imgTmp, MORPH_CLOSE, kernel_3x3);
  findContours(canny_out, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);
  // res.resize(contours.size());
  vector<Rect> rects;
  vector<QLetter> unsorted;
  Mat drawing = Mat::zeros(canny_out.size(), CV_8UC1);
  Mat d, *cropped;
  for (int i = 0; i < contours.size(); i++)
  {
    if (hierarchy[i][3] == -1)
    {
      Rect rect = boundingRect(contours[i]);
      d = Mat::zeros(gray.size(), CV_8UC1);
      drawContours(d, contours, i, color, FILLED);
      if ((hole = hierarchy[i][2]) != -1)
      {
        for (int j = hierarchy[hole][2]; j != -1; j = hierarchy[j][0])
        {
          drawContours(d, contours, j, back, FILLED);
        }
      }
      QLetter cropped;
      cropped.letter = new Mat;
      cropped.rect = rect;
      d(rect).copyTo(*(cropped.letter));
      unsorted.insert(unsorted.end(), cropped);
    }
  }
  std::sort(unsorted.begin(), unsorted.end(), &leftOrder);
  for(vector<QLetter>::iterator i=unsorted.begin(); i != unsorted.end(); i++)
    res.insert(res.end(), i->letter); 
}