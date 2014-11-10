// Infrared.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
// This source code is licensed under the MIT license. Please see the License in License.txt.
// "This is preliminary software and/or hardware and APIs are preliminary and subject to change."
//

#include "stdafx.h"
#include <Windows.h>
#include <Kinect.h>
#include <opencv2/opencv.hpp>
#include<stdlib.h>
#include<string.h>

template<class Interface>
inline void SafeRelease( Interface *& pInterfaceToRelease )
{
	if( pInterfaceToRelease != NULL ){
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}
void getImageName(char* imagename,int counter);


int _tmain( int argc, _TCHAR* argv[] )
{
	cv::setUseOptimized( true );

	// Sensor
	IKinectSensor* pSensor;
	HRESULT hResult = S_OK;
	hResult = GetDefaultKinectSensor( &pSensor );
	if( FAILED( hResult ) ){
		std::cerr << "Error : GetDefaultKinectSensor" << std::endl;
		return -1;
	}

	hResult = pSensor->Open();
	if( FAILED( hResult ) ){
		std::cerr << "Error : IKinectSensor::Open()" << std::endl;
		return -1;
	}

	// Source
	IInfraredFrameSource* pInfraredSource;
	hResult = pSensor->get_InfraredFrameSource( &pInfraredSource );
	if( FAILED( hResult ) ){
		std::cerr << "Error : IKinectSensor::get_InfraredFrameSource()" << std::endl;
		return -1;
	}

	// Reader
	IInfraredFrameReader* pInfraredReader;
	hResult = pInfraredSource->OpenReader( &pInfraredReader );
	if( FAILED( hResult ) ){
		std::cerr << "Error : IInfraredFrameSource::OpenReader()" << std::endl;
		return -1;
	}


	// Description
	IFrameDescription* pDescription;
	hResult = pInfraredSource->get_FrameDescription( &pDescription );
	if( FAILED( hResult ) ){
		std::cerr << "Error : IInfraredFrameSource::get_FrameDescription()" << std::endl;
		return -1;
	}

	int width = 0;
	int height = 0;
	pDescription->get_Width( &width ); // 512
	pDescription->get_Height( &height ); // 424

	cv::Mat infraredMat( height, width, CV_8UC1 );
	cv::namedWindow( "Infrared" );

	int counter=0;
	while( 1 ){
		// Frame
		IInfraredFrame* pInfraredFrame = nullptr;
		hResult = pInfraredReader->AcquireLatestFrame( &pInfraredFrame );
		if( SUCCEEDED( hResult ) ){
			unsigned int bufferSize = 0;
			unsigned short* buffer = nullptr;
			hResult = pInfraredFrame->AccessUnderlyingBuffer( &bufferSize, &buffer );
			if( SUCCEEDED( hResult ) ){
				for( int y = 0; y < height; y++ ){
					for( int x = 0; x < width; x++ ){
						unsigned int index = y * width + x;
						infraredMat.at<unsigned char>( y, x ) = buffer[ index ] >> 8;
					}
				}
			}
				char imageName[1100];
				getImageName(imageName,counter++);
				cv::imwrite(imageName, infraredMat);
		}
	
		SafeRelease( pInfraredFrame );

		cv::imshow( "Infrared", infraredMat );

		if( cv::waitKey( 30 ) == VK_ESCAPE ){
			break;
		}
	}

	SafeRelease( pInfraredSource );
	SafeRelease( pInfraredReader );
	SafeRelease( pDescription );
	if( pSensor ){
		pSensor->Close();
	}
	SafeRelease( pSensor );
	cv::destroyAllWindows();

	return 0;
}

void getImageName(char* imagename,int counter)
{

		char* str1="H:/kinect2_infrared_Image/1/";
		char* str2=".jpg";
		char str3[1000]={0};
		_itoa_s(counter,str3,10);
		strcpy(imagename,str1);
		strcat(imagename,str3);
		strcat(imagename,str2);
		

}