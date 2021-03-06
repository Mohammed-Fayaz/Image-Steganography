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
    
    string temp="";
    for(int i =0 ; i< 7-s.length() ; i++){
        temp.append("0");
}
    return temp.append(s);
}



int main(int argc, char** argv){
  
  
  fstream MyFile(argv[2]);
  
  if(!MyFile.is_open()){
      cout <<"error in file handling"<<endl;
      return -1;}

    string buffer,d;
    string temp="";
    int flag;
    while(getline(MyFile,buffer)){
         for(char c : buffer){
         cout<<c;
         //cout<<c;
         temp.append(bin((int)c));
         //cout<<temp<<endl;
         //free the variable c
         
         }
         temp.append("0001010");
     }
        //add 7 0's to temp in the end to indicate the end of file
        temp.append("0000000");

   MyFile.close();
    int count =0;

    Mat image = imread(argv[1]);
    if(image.empty()){
        cout<< "There is no image\n";
        return -1;}

     int index=0,length_of_bits;
     
     //bool encoded = false;
     length_of_bits = temp.length();
     if(length_of_bits >= image.rows * image.cols){
      cout<<"Image size is insufficent" <<endl;
      return -1;
     }

    // cout << "Rows in image : " << image.rows <<endl;
     //cout << "Columns in image : " << image.cols << endl;

     int limit = image.rows * image.cols /7 ;

    // cout << "The max number of characters that can be encoded in the image are : " << limit << endl;
     //cout << "The number of bits are : " << length_of_bits<<endl;

     //cout << "By assuming the average length of a word to be 4 characters : " << limit/4 <<endl;


    for(int row =0 ; row < image.rows ; row++){
        for(int col =0 ; col < image.cols ; col++){
            for(int color = 0 ; color < 3 ; color++){


                Vec3b pixel = image.at<Vec3b>(Point(row,col));
                if(index < length_of_bits){
               if((int)temp[index] ==49){

                        //cout<<"intial1"<<(int)pixel.val[color]<<"  ";
                        pixel.val[color] |= 1;
                        
                        //cout<<"final1"<<(int)pixel.val[color]<<endl;
                        //cout<<endl;

                    }
                    else{
                        //cout<<"intial0"<<(int)pixel.val[color]<<"  ";
                        pixel.val[color] &= ~1;
                        
                        //cout<<"final0"<<(int)pixel.val[color]<<endl;
                        //cout<<endl;
                    }
                    index++;
                    image.at<Vec3b>(Point(row,col)) = pixel;
                    }
                else{
                    //encoded = true;
                    goto exiting;
                    
                }
    
    
    }
   }
    }


exiting:;


//checks if the entire message is encoded or not
/*if(!encoded){
    cout << "Image size is insufficient to encode Try again with a large Image"<<endl;
    return -1;
}*/

imwrite(argv[3],image);
return 0;
    }
