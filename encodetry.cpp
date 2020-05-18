#include<iostream>
#include<fstream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

//ascii ranges from 0 to 127 we can have a max of 7 bits in binary
// So a char will be converted into a 7 bit binary code.

string bin(int num){
    string s="";
    int i,flag=0;
    for(i = 31 ; i>=0 ; i--){  
        int k = num >> i;
        if(k&1){
            
            s.append("1");
            flag=1;
            
            }
        
        else{
            if(flag == 1)
                s.append("0");
    
    }
    }
//making sure the legth of the binary is 7
    string temp="";
    for(int i =0 ; i< 7-s.length() ; i++){
        temp.append("0");
}
    return temp.append(s);
}



int main(int argc, char** argv){
  
  //argv[1] contains the image that is used.
  //argv[2] contains the textfile.
  //argv[3] contains the output image
  
  fstream MyFile(argv[2]);
  
  if(!MyFile.is_open()){
      cout <<"error in file handling"<<endl;
      return -1;}

    char c,d;
    string temp="";
    int flag;
    while(!MyFile.eof()){
         MyFile.get(c);
         cout<<c;
         temp.append(bin((int)c));
         cout<<temp<<endl;
         }
        //add 7 0's to temp in the end to indicate the end of file
        temp.append("0000000");

   MyFile.close();
    

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

               //getting the pixel values

                Vec3b pixel = image.at<Vec3b>(Point(row,col));
                
                if(index < length_of_bits){
               //if the bit value is 1 then LSB is modified as 1
               //if the bit value is 0 then LSB is modified as 0
               
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

//checks if the entire message is encoded or not
if(!encoded){
    cout << "Image size is insufficient to encode Try again with a large Image"<<endl;
    return -1;
}

imwrite(argv[3],image);
return 0;
    }
