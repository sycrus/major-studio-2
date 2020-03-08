//
//  CVShape.h
//  from https://gist.github.com/kashimAstro
//
#include "ofMain.h"
#include "ofxCv.h"

using namespace ofxCv;
using namespace cv;

class CVShape{
public:
    Mat gray;
    Mat bw;
    Mat dst;
    ofPixels pixelsRep;
    //added to get more than ofLog notices on shapes
    cv::Point pt;
    int type; // 0 = circle, 3 = triangle, 4 = rect
    
    vector<vector<cv::Point> > contours;
    vector<cv::Point> approx;
    
    static double angle(cv::Point pt1,cv::Point pt2, cv::Point pt0)
    {
        double dx1 = pt1.x - pt0.x;
        double dy1 = pt1.y - pt0.y;
        double dx2 = pt2.x - pt0.x;
        double dy2 = pt2.y - pt0.y;
        return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
    }
    
    void update(ofPixels pix){
        pixelsRep = pix;
        Mat src = toCv(pixelsRep);
        if (src.empty())
            ofLog()<<"Error image";
        cvtColor(src, gray, CV_BGR2GRAY);
        Canny(gray, bw, 0, 50, 5);
        findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
        dst = src.clone();
        
        for (int i = 0; i < contours.size(); i++)
        {
            approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);
            if (std::fabs(contourArea(contours[i])) < 100 || !isContourConvex(approx))
                continue;
            if (approx.size() == 3)
            {
                Moments m = moments(Mat(contours[i]),true);
                cv::Point p(m.m10/m.m00, m.m01/m.m00);
                pt = p;
                type = 3;
                //ofLog()<<"TRIANGLES";
            }
            else if (approx.size() >= 4 && approx.size() <= 6)
            {
                int vtc = approx.size();
                std::vector<double> cos;
                for (int j = 2; j < vtc+1; j++)
                    cos.push_back(angle(approx[j%vtc], approx[j-2], approx[j-1]));
                std::sort(cos.begin(), cos.end());
                double mincos = cos.front();
                double maxcos = cos.back();
                if (vtc == 4 && mincos >= -0.1 && maxcos <= 0.3) {
                    
                    Moments m = moments(Mat(contours[i]),true);
                    cv::Point p(m.m10/m.m00, m.m01/m.m00);
                    pt = p;
                    type = 4;
                    //ofLog()<<"RECTANGLE" << p;
                }
                else if (vtc == 5 && mincos >= -0.34 && maxcos <= -0.27) {
                    //ofLog()<<"PENTA";
                }
                else if (vtc == 6 && mincos >= -0.55 && maxcos <= -0.45){
                    //ofLog()<<"HEXA";
                }
            }
            else
            {
                double area = contourArea(contours[i]);
                cv::Rect r = boundingRect(contours[i]);
                int radius = r.width / 2;
                if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 &&
                    std::abs(1 - (area / (CV_PI * std::pow(radius, 2)))) <= 0.2) {
                    Moments m = moments(Mat(contours[i]),true);
                    cv::Point p(m.m10/m.m00, m.m01/m.m00);
                    pt = p;
                    type = 0;
                    //ofLog()<<"CIRCLE";
                }
                
            }
        }
    }
};
