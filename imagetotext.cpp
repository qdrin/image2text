#include "imagetotext.hpp"

using namespace cv;
using namespace std;

// Сортировка контуров слева направо
bool leftOrder(const QLetter a, const QLetter b)
{
  return a.rect.x < b.rect.x;
}

bool QImageToText::loadImage(String filename)
{
  m_image = imread(filename.c_str());
  if(m_image.empty()) return false;
  cvtColor(m_image, m_gray, COLOR_RGB2GRAY);
  return ! m_gray.empty();
}

const vector<QWord> &QImageToText::wordCandidates()
{
  Mat grad, imgTh, connected;
  Mat kernel_3x3 = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
  Mat kernel_9x1 = getStructuringElement(MORPH_RECT, Size(9, 1));
  cvtColor(m_image, m_gray, COLOR_BGR2GRAY);

  morphologyEx(m_gray, grad, MORPH_GRADIENT, kernel_3x3);
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
        r > m_config.minTextFillRate && r < m_config.maxTextFillRate &&
        rect.height > m_config.minLetterHeight && rect.width > m_config.minLetterWidth)
    {
      m_words.insert(m_words.end(), QWord(rect));
    }
  }
  return m_words;
}

string QImageToText::rectToText(Rect r)
{
  char *outText;
  tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
  if(api->Init(NULL, "rus")) {
    cout << "could not initialize tesseract with language 'rus'\n";
    return NULL;
  }
  PIX *image = pixRead("erundulki.jpg");
  api->SetImage(image);
  outText = api->GetUTF8Text();
  string res = string(outText);
  api->End();
  delete api;
  delete []outText;
  pixDestroy(&image);
  return res;
}

const vector<QLetter> &QImageToText::letters(QWord &word)
{
  int hole; // Номер элемента промежуточного слоя (между родителем и реальными потомками)
  Mat canny_out, imgTmp;
  Mat kernel_3x3 = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
  Scalar color = Scalar(255, 255, 255), backColor = Scalar(0, 0, 0);
  Mat grayWord = m_gray(word.rect);
  // GaussianBlur(gray, canny_out, Size(3, 3), 3);
  blur(grayWord, canny_out, Size(3, 3));
  Canny(canny_out, canny_out, m_config.minThresh, m_config.maxThresh, m_config.apertureSize, m_config.L2Gradient);
  vector<vector<Point>> contours;
  vector<Vec4i> hierarchy;
  findContours(canny_out, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);
  vector<Rect> rects;
  vector<QLetter> unsorted;
  Mat drawing = Mat::zeros(canny_out.size(), CV_8UC1);
  Mat d, *cropped;
  for (int i = 0; i < contours.size(); i++)
  {
    if (hierarchy[i][3] == -1)
    {
      Rect rect = boundingRect(contours[i]);
      d = Mat::zeros(m_gray.size(), CV_8UC1);
      drawContours(d, contours, i, color, FILLED);
      if ((hole = hierarchy[i][2]) != -1)
      {
        for (int j = hierarchy[hole][2]; j != -1; j = hierarchy[j][0])
        {
          drawContours(d, contours, j, backColor, FILLED);
        }
      }
      QLetter cropped;
      cropped.letter = *(new Mat);
      cropped.rect = rect;
      d(rect).copyTo(cropped.letter);
      unsorted.insert(unsorted.end(), cropped);
    }
  }
  std::sort(unsorted.begin(), unsorted.end(), &leftOrder);
  for(vector<QLetter>::iterator i=unsorted.begin(); i != unsorted.end(); i++)
    word.letters.insert(word.letters.end(), *i);
  return word.letters;
}