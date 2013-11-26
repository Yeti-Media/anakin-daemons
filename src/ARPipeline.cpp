/*****************************************************************************
*   Markerless AR desktop application.
******************************************************************************
*   by Khvedchenia Ievgen, 5th Dec 2012
*   http://computer-vision-talks.com
******************************************************************************
*   Ch3 of the book "Mastering OpenCV with Practical Computer Vision Projects"
*   Copyright Packt Publishing 2012.
*   http://www.packtpub.com/cool-projects-with-opencv/book
*****************************************************************************/

////////////////////////////////////////////////////////////////////
// File includes:
#include "ARPipeline.hpp"

ARPipeline::ARPipeline(const std::vector<cv::Mat>& patternImages, const CameraCalibration& calibration)
  : m_calibration(calibration)
{

  for(unsigned int x=0; x < patternImages.size(); x++)
  {
    Pattern pattern;
    m_patternDetector.buildPatternFromImage(patternImages[x], pattern);
    m_patterns.push_back(pattern);
  }
  m_patternDetector.train(m_patterns);

}

bool ARPipeline::processFrame(const cv::Mat& inputFrame)
{
  bool patternFound = m_patternDetector.findPattern(inputFrame);
  if (patternFound)
  {
    std::cout << "===============>Pattern found!<===============\n";
    m_patternDetector.patternMatched.computePose(m_calibration);
  }

  return patternFound;
}

const Transformation& ARPipeline::getPatternLocation() const
{
  return m_patternDetector.patternMatched.patternInfo.pose3d;
}
