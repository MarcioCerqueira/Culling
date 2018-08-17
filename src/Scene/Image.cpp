#include "Scene\Image.h"

Image::Image(int width, int height, int channels) {

	data = (unsigned char*)malloc(width * height * channels * sizeof(unsigned char));
	this->width = width;
	this->height = height;
	
}

Image::Image(char *filename) {
	
	cv::Mat img = cv::imread(filename);
	if(img.channels() > 1) cv::cvtColor(img, img, CV_BGR2RGB);
	data = (unsigned char*)malloc(img.cols * img.rows * img.channels() * sizeof(unsigned char));
	this->width = img.cols;
	this->height = img.rows;
	this->numberOfChannels = img.channels();
	
	for(int pixel = 0; pixel < this->width * this->height * this->numberOfChannels; pixel++)
		data[pixel] = img.ptr<unsigned char>()[pixel];

}

Image::~Image() {

	delete [] data;

}
