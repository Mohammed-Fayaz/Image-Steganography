#include<iostream>
#include<fstream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

string bin(int num){
    string s="";
    int i,flag=0;
    for(i = 31 ; i>=0 ; i--){
        int k = num >> i;
        if(k&1){
            s.append("1");
            flag=1;}
        else{
            if(flag == 1){
            s.append("0");
    }}}
    return s;
}


int main(int argc, char** argv){
  fstream MyFile(argv[2]);

    char c,d;
    string temp="";
    int flag;
    while(!MyFile.eof()){
         MyFile.get(c);
         cout<<c;
         temp.append(bin((int)c));
         cout<<temp<<endl;
         }

   MyFile.close();
 /* string s = "hai";
  for(std::string::size_type i =0; i<s.size(); i++){
      std::cout << bin((int)s[i])<<endl;
    }*/
    
    Mat image = imread(argv[1]);
    if(image.empty()){
        cout<< "There is no image\n";
        return -1;}

     int index=0,length_of_bits;
     bool last_null_char = false;
     bool encoded = false;
     length_of_bits = temp.length();


    for(int row =0 ; row < image.rows ; row++){
        for(int col =0 ; col < image.cols ; col++){
            for(int color = 0 ; color < 3 ; color++){

                
                Vec3b pixel = image.at<Vec3b>(Point(row,col));
                //cout<< (int)pixel.val[color]<<" ";
                if(index < length_of_bits){
                    if(temp[index] ==1){
                        pixel.val[color] |= 1;
                    }
                    else{
                        pixel.val[color] &= ~1;
                    }
                    index++;
                    image.at<Vec3b>(Point(row,col)) = pixel;
                    }
                else{
                    encoded = true;
                    goto exiting;
                    
                }
    
    
    }
   }
    }
exiting:;
if(!encoded){
    cout << "Image size is insufficient to encode Try again with a large Image"<<endl;
    return -1;
}

imwrite(argv[3],image);
return 0;
    }
