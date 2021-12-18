#include "imagetotext.hpp"

using namespace cv;
using namespace std;

QImageToText::QImageToText(String filename)
:
  m_image(imread(filename.c_str()))
{
  if (!m_image.empty())
    cvtColor(m_image, m_gray, COLOR_RGB2GRAY);
}

QImageToText::QImageToText(const Mat &_image)
:
  m_image(_image)
{
  if (! m_image.empty())
    cvtColor(m_image, m_gray, COLOR_RGB2GRAY);
}

// Detecting borders of words of text
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

// Text recognition: runs Tesseract OCR all over the image and selects Tesseracts's words next
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

// Text recognition: runs Tesseract OCR on just one word from wordCandidates, pointed by its index in m_words vector
vector<QWord>::iterator QImageToText::candidateToWord(int i)
{
  vector<Rect>::iterator it = m_wordCandidates.begin() + i;
  return candidateToWord(it);
}

// Text recognition: runs Tesseract OCR on just one word from wordCandidates, pointed by m_word iterator
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
