#include<iostream>
#include<fstream>
#include "opencv2/opencv.hpp" 

using namespace std;
using namespace cv;


int getintvalue(string s){
    int val =0;
    
    for(int i=0 ; i<s.size() ; i++){
    
        val = val*2 + (int)s[i]-48;
    
    }
    
    return val;
}


int main(int argc , char** argv){

    
    string temp = "";
    string message="";
    int bits =0;
    int flag;


    Mat image = imread(argv[1]);
    if(image.empty()){
        cout << "An error occured due to image"<<endl;
        return -1;
    }

    for(int row= 0 ; row <= image.rows ; row++){
        for(int col = 0; col <= image.cols ; col++){
            for(int color = 0; color < 3 ; color++){

                Vec3b pixel = image.at<Vec3b>(Point(row,col));
                if(pixel.val[color] &1){
                    temp.append("1");
                }
                else{
                    temp.append("0");
                }
                bits++;
                if(bits == 8){
                    flag = getintvalue(temp);
                    if(flag == 0){
                    
                        goto exiting;
                    
                    }
                    else{
                        char x = (char)flag;
                        message.append(&x,1);
                        temp ="";
                        bits=0;
                    }
                }
            }
        }
    }
    exiting:;
    cout << message <<endl;
}



