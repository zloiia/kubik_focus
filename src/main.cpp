#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
// #include <opencv2/imgproc.hpp>
#include <string>
#include <stdint.h>
#include <cstdio>
#include <string.h>
#include <sstream> 


using namespace cv;

char m_devPath[250];
const uint32_t width = 1600;
const uint32_t height = 1200;

const char* windowName = "KUBIK";

uint16_t pRGB565buffer[width*height] = {0};


const int EXIT_KEY = 1048603;
const int NORMAL_MODE_KEY = 1048686;
const int LAPLACIAN_MODE_KEY = 1048684;
const int RAINBOW_MODE_KEY = 1048690;

enum class SHOW_MODE_T {
    NORMAL,
    LAPLACIAN,
    RAINBOW
} showMode = SHOW_MODE_T::NORMAL;




bool readFrame(uint16_t* dest) {
    const uint32_t len = 512*7500;
    const uint32_t sk = 5000*512;

    const uint32_t bytesToRead = width*height*sizeof(uint16_t);


    FILE* f = fopen(m_devPath, "r");
    if (f == NULL) {
        std::cout << "Can not open file to read" << std::endl;
        return false;
    }
    if (fseek(f,sk,  SEEK_SET)!=0) {
        std::cout << "Can not seek" << std::endl;
        fclose(f);
        return false;
    }
    if (fread(dest, bytesToRead, 1, f)!=1) {
        std::cout << "Can not read" << std::endl;
        fclose(f);
        return false;
    }
    fclose(f);

    f = fopen(m_devPath, "w");
    if (f == NULL) {
        std::cout << "Can not open file to write" << std::endl;
        return false;
    }
    if (fseek(f,sk,  SEEK_SET) != 0 ) {
        fclose(f);
        return false;
    }
    char bf[] = {0x00};
    fwrite(bf, 1, sizeof(bf), f);
    fclose(f);

    return true;
}



int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Please specify path to device" << std::endl;
        return 1;
    }
    mempcpy(m_devPath, argv[1], strlen(argv[1]));

    // std::cout << "Hello Easy C++ project!" << std::endl;
    
    Mat raw_image(height, width, CV_8UC2, pRGB565buffer);
    Mat colorImage;
    Mat gray, abs_dst, dst;
    double variance = 0.0;

    namedWindow( windowName, CV_WINDOW_FULLSCREEN);// Create a window for display.

    // namedWindow( "COLOR", CV_WINDOW_AUTOSIZE );// Create a window for display.
    // namedWindow( "Lap", CV_WINDOW_AUTOSIZE );// Create a window for display.

    while(1) {
        if (!readFrame(pRGB565buffer)) {
            std::cout << "Read error" << std::endl;
            continue;
        }
        cvtColor(raw_image, colorImage, CV_BGR5652BGR);
        variance = -1.0;
        // applyColorMap(colorImage, color1Image, COLORMAP_RAINBOW);
        // cvtColor(raw_image, colorImage, CV_YUV2BGR);


        // cvtColor(colorImage, colorImage, CV_BGR2HSV);

        {
            int kernel_size = 3;
            int scale = 1;
            int delta = 0;
            int ddepth = CV_8U;
            
            cvtColor( colorImage, gray, CV_BGR2GRAY );
            Laplacian( gray, dst, ddepth, kernel_size, scale, delta, BORDER_DEFAULT );
            Scalar mean, stddev;
            meanStdDev(dst, mean, stddev, Mat());
            variance = stddev.val[0] * stddev.val[0];
            // std::cout << variance << std::endl;
            convertScaleAbs( dst, abs_dst );
            // imshow( "Lap", abs_dst );
        }

        std::stringstream ss;
        ss << variance;
        
        switch(showMode) {
            case SHOW_MODE_T::NORMAL:
                putText(colorImage, ss.str(), Point(5,100),  FONT_HERSHEY_DUPLEX, 1, Scalar(0,0,255), 3);
                imshow( windowName, colorImage ); 
            break;

            case SHOW_MODE_T::LAPLACIAN:
                putText(abs_dst, ss.str(), Point(5,100),  FONT_HERSHEY_DUPLEX, 1, Scalar(255,255,255), 3);
                imshow( windowName, abs_dst ); 
            break;

            case SHOW_MODE_T::RAINBOW:
                applyColorMap(gray, colorImage, COLORMAP_RAINBOW);
                imshow( windowName, colorImage ); 
            break;


        }
        

        int key = waitKey(1);
        // std::cout << key << std::endl;
        if (key == EXIT_KEY) {
            break;
        }
        else if (key == NORMAL_MODE_KEY) {
            showMode = SHOW_MODE_T::NORMAL;
        }
        else if (key == LAPLACIAN_MODE_KEY) {
            showMode = SHOW_MODE_T::LAPLACIAN;
        }
        else if (key == RAINBOW_MODE_KEY) {
            showMode = SHOW_MODE_T::RAINBOW;
        }
    }
    destroyAllWindows();
    
    return 0;
}