#include "imagetotext.hpp"

using namespace cv;
using namespace std;

// Ptr<text::OCRTesseract> QImageToText::ocr = text::OCRTesseract::create(
//   NULL, "rus"
// );
// Сортировка контуров слева направо
bool leftOrder(const QLetter a, const QLetter b)
{
  return a.rect.x < b.rect.x;
}

bool QImageToText::loadImage(String filename)
{
  m_image = imread(filename.c_str());
  if (m_image.empty())
    return false;
  cvtColor(m_image, m_gray, COLOR_RGB2GRAY);
  return !m_gray.empty();
}

const vector<Rect> &QImageToText::detectWords()
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
  m_wordCandidates.clear();
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
      m_wordCandidates.insert(m_wordCandidates.end(), rect);
    }
  }
  return m_wordCandidates;
}

bool QImageToText::tessToText()
{
  m_words.clear();
  Mat *img = &m_gray;
  tesseract::TessBaseAPI api;
  if (api.Init(NULL, "rus"))
  {
    cout << "could not initialize tesseract with language 'rus'\n";
    return false;
  }
  api.SetImage(img->data, img->size().width, img->size().height, img->channels(), img->step1());
  api.Recognize(0);
  tesseract::PageIteratorLevel level = tesseract::RIL_WORD;
  tesseract::ResultIterator* ri = api.GetIterator();
  for(int i = (ri != 0); i != 0 ; i = ri->Next(level)) {
    const char* w = ri->GetUTF8Text(level);
    float confidence = ri->Confidence(level);
    int x1=0, y1=0, x2=0, y2=0;
    ri->BoundingBox(level, &x1, &y1, &x2, &y2);
    Rect r(x1, y1, x2, y2);
    QWord qw(r, String(w), confidence);
    m_words.insert(m_words.end(), qw);
    delete[] w;
  }
  api.End();
  return true;
}

vector<QWord>::iterator QImageToText::candidateToWord(int i)
{
  vector<Rect>::iterator it = m_wordCandidates.begin() + i;
  return candidateToWord(it);
}

vector<QWord>::iterator QImageToText::candidateToWord(vector<Rect>::iterator it)
{
  Rect r = *it;
  Mat wimg(m_gray, r);
  tesseract::TessBaseAPI api;
  if (api.Init(NULL, "rus"))
  {
    cout << "could not initialize tesseract with language 'rus'\n";
    return vector<QWord>::iterator(NULL);
  }
  api.SetImage(wimg.data, wimg.size().width, wimg.size().height, wimg.channels(), wimg.step1());
  const char* w = api.GetUTF8Text();
  String res(w);
  if(res[res.length()-1] == '\n')
    res.erase(res.length()-1);
  float conf = api.MeanTextConf();
  delete[] w;
  QWord qw(r, res, conf);
  return m_words.insert(m_words.end(), qw);
}


/*
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
  for (vector<QLetter>::iterator i = unsorted.begin(); i != unsorted.end(); i++)
    word.letters.insert(word.letters.end(), *i);
  return word.letters;
}
*/