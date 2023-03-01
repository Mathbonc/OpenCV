#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

Mat preProcessing(Mat image){
    Mat kernel = getStructuringElement(MORPH_RECT, Size(5,5));

    cvtColor(image, image, COLOR_BGR2GRAY); 
    GaussianBlur(image, image, Size(3, 3), 3, 0);
	Canny(image, image, 150, 75);
	dilate(image, image, kernel);

    return image;
}

vector<Point> sortingPoints(vector<Point> points){
    vector<Point> newPoints;
    vector<int> sumPoints, subPoints;
    int index;
    
    for(int i=0; i<4; i++){
        sumPoints.push_back(points[i].x + points[i].y);
        subPoints.push_back(points[i].x - points[i].y);
    }
    
    //Devolve o index min_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()
    newPoints.push_back(points[min_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]); 
    newPoints.push_back(points[max_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]);
    newPoints.push_back(points[min_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]); 
    newPoints.push_back(points[max_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]);

    return newPoints;
}

vector<Point> getContours(Mat imgDil, Mat imgOriginal) { 
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//drawContours(imgOriginal, contours, -1, Scalar(0, 255, 0), 3);

	vector<vector<Point>> conPoly(contours.size()); 
	vector<Point> biggest;

    int maxArea = 0;

	//Filtros
	for (int i = 0; i < contours.size(); i++) {
		int area = contourArea(contours[i]);
        
		if (area > 1000) {
			float peri = arcLength(contours[i], true);//pegando o perímetro  do contorno [i], e se ele é fechado ou năo
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
	
            if(area > maxArea && conPoly[i].size() == 4){
                biggest = {conPoly[i][0],conPoly[i][1],conPoly[i][2],conPoly[i][3]};
                maxArea = area; 
            }

			//drawContours(imgOriginal, conPoly, i, Scalar(255, 30, 255), 3);
			// rectangle(imgOriginal, boundRect[i].tl(), boundRect[i].br(), Scalar(255, 0, 0), 5);
		}
	}

    if(biggest.size() == 4){
        biggest = sortingPoints(biggest);
    }
    
    
	return biggest;
}

void drawPoints(Mat image, vector<Point> points){
    for(int i = 0; i<points.size(); i++){
        circle(image, points[i], 10, Scalar(0,0,255), FILLED);
        putText(image, to_string(i), points[i], FONT_HERSHEY_COMPLEX, 3, Scalar(0,255,0), 10);
    }
}

Mat getWarp(Mat img, vector<Point> points, float w, float h){
    Mat matrix,warpedImg;
    Point2f src[4] = { points[0], points[1], points[2], points[3] };
	Point2f Dest[4] = { {0.0f,0.0f},{w,0.0f},{0.0f,h},{w,h} };

	matrix = getPerspectiveTransform(src, Dest);
	warpPerspective(img, warpedImg, matrix, Point(w, h));

    return warpedImg;
}

void videoWarp(string path){
    Mat img;
    Mat imgTH, imgWarp;
    VideoCapture cam(path);
    vector<Point> contourPoints;

    while(1)
    {
        cam.read(img);
        imgTH = preProcessing(img);
        contourPoints = getContours(imgTH,img);
        //drawPoints(img, contourPoints);  

        if(contourPoints.size() == 4){
            imgWarp = getWarp(img, contourPoints, 2*596.0f, 2*420.0f);
            
        }else{
            imgWarp = imread("Recursos/Stand_by.jpg");
            resize(imgWarp,imgWarp,Size(2*596.0,2*420.0f));
        }

        imshow("Img", img);
        imshow("Img TH", imgTH);
        imshow("Img Warp", imgWarp);

        waitKey(16);
    }
}

void imageWarp(string path){
    Mat img = imread(path);
    Mat imgTH, imgWarp;
    vector<Point> contourPoints;

    imgTH = preProcessing(img);
    contourPoints = getContours(imgTH,img);
    //drawPoints(img, contourPoints);  

    if(contourPoints.size() == 4){
        imgWarp = getWarp(img, contourPoints, 2*596.0f, 2*420.0f);
        
    }else{
        imgWarp = imread("Recursos/Stand_by.jpg");
        resize(imgWarp,imgWarp,Size(2*596.0,2*420.0f));
    }

    imshow("Img", img);
    imshow("Img TH", imgTH);
    imshow("Img Warp", imgWarp);

    waitKey(0);
}

int main(){
    string path = "Recursos/Warp_test3.mp4";
    string path2 = "Recursos/Yae.png";
    string path3 = "Recursos/Documento.jpeg";
    
    videoWarp(path);
    //imageWarp(path2);

    return 0;
}

