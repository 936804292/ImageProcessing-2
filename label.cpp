#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include "bmplib.h" 
#include "queue.h"
using namespace std;

void usage() { 
    cerr << "usage: ./label <options>" << endl;
    cerr <<"Examples" << endl;
    cerr << "./label test_queue" << endl;
    cerr << "./label gray <input file> <outputfile>" << endl;
    cerr << "./label binary <inputfile> <outputfile>" << endl;
    cerr << "./label segment <inputfile> <outputfile>" << endl;
}

//function prototypes
void test_queue();
void rgb2gray(unsigned char ***in,unsigned char **out,int height,int width);
void gray2binary(unsigned char **in,unsigned char **out,int height,int width);
int component_labeling(unsigned char **binary_image,int **labeled_image,int height,int width);
void label2RGB(int  **labeled_image, unsigned char ***rgb_image,int num_segment,int height,int width);
void clean(unsigned char ***input,unsigned char **gray,unsigned char **binary, int **labeled_image,int height , int width);
void bsearch(unsigned char** binary_image, int** label, int height, int width, Location start, int current_label);


// main function, you do not need to make any changes to this function 
int main(int argc,char **argv) {

    srand( (unsigned char)time(0) );
    if(argc < 2 )  {
        usage();
        return -1;
    }        
    unsigned char ***input=0;
    unsigned char **gray=0;
    unsigned char **binary=0;
    int **labeled_image=0;
    if( strcmp("test_queue",argv[1]) == 0 ) { 
        test_queue();
    } 
    else if(strcmp("gray",argv[1]) == 0 ) {
        if(argc <4 ) {
            cerr << "not enough arguemnt for gray" << endl;
            return -1;
        }
        int height,width;
        input = readRGBBMP(argv[2],&height,&width);
        if(input == 0)
        {
            cerr << "unable to open " << argv[2] << " for input." << endl;
            return -1;
        }            
        gray = new unsigned char*[height];
        for(int i=0;i<height;i++) 
            gray[i] = new unsigned char[width];
        rgb2gray(input,gray,height,width);
        if(writeGSBMP(argv[3],gray,height,width) != 0) { 
            cerr << "error writing file " << argv[3] << endl;
            clean(input,gray,binary,labeled_image,height,width);
            return -1;
        }
        clean(input,gray,binary,labeled_image,height,width);

    }
    else if(strcmp("binary",argv[1]) == 0 ) {
        if(argc <4 ) {
            cerr << "not enough arguemnt for binary" << endl;
            return -1;
        }            
        int height,width;
        input = readRGBBMP(argv[2],&height,&width);
        if(input == 0)
        {
            cerr << "unable to open " << argv[2] << " for input." << endl;
            clean(input,gray,binary,labeled_image,height,width);
            return -1;
        }            

        gray = new unsigned char*[height];
        for(int i=0;i<height;i++) 
            gray[i] = new unsigned char[width];
        rgb2gray(input,gray,height,width);

        binary = new unsigned char*[height];
        for(int i=0;i<height;i++)
            binary[i] = new unsigned char[width];

        gray2binary(gray,binary,height,width);
        if(writeBinary(argv[3],binary,height,width) != 0) { 
            cerr << "error writing file " << argv[3] << endl;
            clean(input,gray,binary,labeled_image,height,width);
            return -1;
        }
        clean(input,gray,binary,labeled_image,height,width);
     
    }
    else if(strcmp("segment",argv[1]) == 0 ) {
        if(argc <4 ) {
            cerr << "not enough arguemnt for segment" << endl;
            return -1;
        } 
        int height,width;
        input = readRGBBMP(argv[2],&height,&width);
        if(input == 0)
        {
            cerr << "unable to open " << argv[2] << " for input." << endl;
            clean(input,gray,binary,labeled_image,height,width);
            return -1;
        }            

        gray = new unsigned char*[height];
        for(int i=0;i<height;i++) 
            gray[i] = new unsigned char[width];
        rgb2gray(input,gray,height,width);

        binary = new unsigned char*[height];
        for(int i=0;i<height;i++)
            binary[i] = new unsigned char[height];

        gray2binary(gray,binary,height,width);


        labeled_image = new int*[height];
        for(int i=0;i<height;i++) labeled_image[i] = new int[width];
        int segments= component_labeling(binary, labeled_image,height,width); 

        for(int i=0;i<height;i++) 
            for(int j=0;j<width;j++) 
                for(int k=0;k<RGB;k++) 
                    input[i][j][k] = 0;
        label2RGB(labeled_image, input ,segments, height,width);
        if(writeRGBBMP(argv[3],input,height,width) != 0) {
            cerr << "error writing file " << argv[3] << endl;
            clean(input,gray,binary,labeled_image,height,width);
            return -1;
        }
        clean(input,gray,binary,labeled_image,height,width);

    }

   return 0;
}

//This function is used to test your queue implementation. You do not need to change it,
//though doing so might be useful/helpful

void test_queue() { 
    // create some locations;
    Location three_one, two_two;
    three_one.row = 3; three_one.col = 1;
    two_two.row = 2; two_two.col = 2;

    //create an Queue with max capacity 5
    Queue q(5);

    cout << boolalpha;
    cout << q.is_empty() << endl;           // true
    q.push(three_one);
    cout << q.is_empty() << endl;           // false
    q.push(two_two);

    Location loc = q.pop();
    cout << loc.row << " " << loc.col << endl; // 3 1
    loc = q.pop();
    cout << loc.row << " " << loc.col << endl; // 2 2
    cout << q.is_empty() << endl;           // true
}

//Loop over the 'in' image array and calculate the single 'out' pixel value using the formula
// GS = 0.2989 * R + 0.5870 * G + 0.1140 * B 
void rgb2gray(unsigned char ***in,unsigned char **out,int height,int width) 
{   
   for(int one = 0; one < height; one++)
   {
       for(int two = 0; two < width; two++)
       {
           //this is where the math stuff should go...
           out[one][two] = (unsigned char)(0.2989*in[one][two][0] + 0.5870*in[one][two][1] + 0.1140*in[one][two][2]);       
       }      
   } 
}

//Loop over the 'in' gray scale array and create a binary (0,1) valued image 'out'
//Set the 'out' pixel to 1 if 'in' is above the threshold, else 0
void gray2binary(unsigned char **in,unsigned char **out,int height,int width) 
{
    for(int a = 0; a < height; a++)
    {
        for(int b = 0; b < width; b++)
        {
            if(in[a][b] > THRESHOLD)
            {
                out[a][b] = 1;
            }
            else
            {
                out[a][b] = 0;
            }
       
        }
    }
}

//This is the function that does the work of looping over the binary image and doing the connected component labeling
//See the .pdf for more details
int component_labeling(unsigned char **binary_image,int **label,int height,int width) 
{
    //label[height][width] == binary_image[height][width]; ... copying input to output
    
    int c_label = 1;
    
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            label[i][j] = 0; //set the output array to zero
        }
    }
    
    for(int x = 0; x < height; x++)
    {
        for(int y = 0; y < width; y++)
        {
            if(label[x][y] == 0 && binary_image[x][y] == 1)
            //if output is black and input is white (hasn't been labeled), do the search
            {
                Location start;
                start.row = x;
                start.col = y;
                bsearch(binary_image, label, height, width, start, c_label); //make a BFS function
                c_label++;
            }
        }
    }
    
    cout << c_label << endl;
    return c_label;
   
}    


void bsearch(unsigned char** binary_image, int** label, int height, int width, //BFS function to make life easier
             Location start, int current_label)
{

    label[start.row][start.col] = current_label;
    
    Queue myq(height * width);
    myq.push(start);
    
    while(!myq.is_empty())
    {
        Location checker;
        checker = myq.pop();
        //check for right
        if(checker.col + 1 < width && 
           binary_image[checker.row][checker.col+1] == 1 
           && label[checker.row][checker.col+1] == 0)
        {
            Location right;
            label[checker.row][checker.col + 1] = current_label;
            right.row = checker.row;
            right.col = checker.col + 1;
            myq.push(right);
            
        }
        //check for below
        if(checker.row + 1 < height && 
           binary_image[checker.row+1][checker.col] == 1 
           && label[checker.row+1][checker.col] == 0)
        {
            Location below;
            label[checker.row+1][checker.col] = current_label;
            below.row = checker.row + 1 ;
            below.col = checker.col;
            myq.push(below);   
        }    
        //check for above
        if(checker.row - 1 >= 0 && 
           binary_image[checker.row-1][checker.col] == 1 
           && label[checker.row-1][checker.col] == 0)
        {
            Location above;
            label[checker.row-1][checker.col] = current_label;
            above.row = checker.row - 1;
            above.col = checker.col;
            myq.push(above);   
        }     
        //check for left 
        if(checker.col - 1 >= 0 && 
           binary_image[checker.row][checker.col-1] == 1 
           && label[checker.row][checker.col - 1] == 0)
        {
            Location left;
            label[checker.row][checker.col - 1] = current_label;
            left.row = checker.row;
            left.col = checker.col - 1;
            myq.push(left);   
        }
    
    }


}


//First make num_segments random colors to use for coloring the labeled parts of the image.
//Then loop over the labeled image using the label to index into your random colors array.
//Set the rgb_pixel to the corresponding color, or set to black if the pixel was unlabeled.
#ifndef AUTOTEST
void label2RGB(int  **labeled_image, unsigned char ***rgb_image,int num_segments,int height,int width)
{
    //create a color array... will need to dynamically allocate to account for size(num_segments)
    unsigned char* colors[3];
    
    for(int i = 0; i < 3; i++)
    {
        colors[i] = new unsigned char[num_segments];
    }
    
    //fill colors array with random colors using rand
    for(int x = 0; x < 3; x++)
    {
        for(int y = 0; y < num_segments; y++)
        {
            colors[x][y] = (unsigned char) rand() % 255;
        }
    }
    
    //fill the rgb final output image with the values of the color array
    for(int a = 0; a < height; a++)
    {
        for(int b = 0; b < width; b++)
        {
            for(int c = 0; c < RGB; c++)
            {
                if(labeled_image[a][b] == 0)
                {
                    rgb_image[a][b][c] = 0;
                }
                else
                    rgb_image[a][b][c] = colors[c][labeled_image[a][b]];
            }
        }
    }
    
    //delete the color array
    for(int i = 0; i < 3; i++)
    {
        delete[] colors[i];
    }
}
#endif
//fill out this function to delete all of the dynamic arrays created
void clean(unsigned char ***input,unsigned char **gray,unsigned char **binary, int **labeled_image,int height , int width) {
    if(input ) {
            //delete allocated input image array here
        for(int i = 0; i < height; i++)
        {
            for(int j = 0; j < width; j++)
            {
                delete[] input[i][j];
            }
            delete[] input[i];
        }
        delete[] input;
        
        } 
        if(gray) {
            //delete gray-scale image here
            for(int i = 0; i < height; i++)
            {
                delete[] gray[i];
            }
            delete[] gray;
        }
        if(binary) {
            //delete binary image array here
            for(int i = 0; i < height; i++)
            {
                delete[] binary[i];
            }
            delete[] binary;
        }
        if(labeled_image) {
            //delete labeled array here
            for(int i = 0; i < height; i++)
            {
                delete[] labeled_image[i];
            }
            delete[] labeled_image;
        }


}
