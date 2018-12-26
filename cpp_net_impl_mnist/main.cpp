#include "globals.h"
#ifdef OPENCL
#include "cl_net.h"
#else
#include "neural_net.h"
#endif

#define img_size 784
#define data_size 60000
#define train_size 50000
#define test_size 10000
#define out_size 10
#define hidden_size 30
#define num_of_epocs 100
#define lambda 0.01f
#define regularsation_param 0.0f

string mnist = "C:/Users/weepi/Documents/PythonProjects/neural_nets/net_prac/mnist/";

std::mt19937_64 generator;

BYTE train_imgs[data_size][img_size];
BYTE train_labs[data_size];

void read_files();
void scramble();
void make_image(basic_network & net,int n);

struct data_generator{
    int start;
    int end;
    void operator () (function<void(learn_data&)> fn){
        learn_data data;
        data.input_data.resize(img_size);
        data.output_data.resize(out_size);
        for(int data_n : range(start,end)){
            for(int d : range(out_size))
                data.output_data[d] = (d == train_labs[data_n]);

            for(int j : range(img_size))
                 data.input_data[j] = train_imgs[data_n][j] / 256.0;

            fn(data);
        }
    }
};

int main()
{
    generator.seed(0);//clock());
    //srand(clock());
    read_files();
    basic_network net({img_size,hidden_size,out_size},lambda,regularsation_param);
    net.test(data_generator{train_size,data_size});
    for(int i = 0; i < num_of_epocs; i++){
        net.learn(data_generator{0,train_size});
        //make_image(net,i);//generates image representation
        net.test(data_generator{train_size,data_size});
        //scramble();
    }
    return 0;
}
void scramble(){
    BYTE temp[img_size];
    uniform_int_distribution<int> dist(0,train_size);
    for(int n = 0; n < train_size; n++){
        int newN = dist(generator);
        swap(train_labs[n],train_labs[newN]);
        memcpy(temp,train_imgs[n],img_size);
        memcpy(train_imgs[n],train_imgs[newN],img_size);
        memcpy(train_imgs[newN],temp,img_size);
    }
}

void read_files(){
    ifstream test_img_file(mnist +"train-images.idx3",ios::binary);
    if (!test_img_file){
        cout << "data file cannot be opened!\n";
        throw -1;
    }
    char empt_buff[16];
    test_img_file.read(empt_buff,16);

    test_img_file.read(reinterpret_cast<char *>(&train_imgs),data_size * img_size);
    test_img_file.close();

    ifstream test_lab_file(mnist + "train-labels.idx1",ios::binary);
    if (!test_lab_file){
        cout << "data file cannot be opened!\n";
        throw -1;
    }
    test_lab_file.read(empt_buff,8);

    test_lab_file.read(reinterpret_cast<char *>(&train_labs),data_size);
    test_lab_file.close();

    /*for(int g = 0; g < 20;g++)
       cout << "label #" << g << " = " << int(train_labs[g]) << "\n";

    cin.get();
    for(int g = 0; g < 20;g++){
        cout << "data #" << g << ":\n";
        for(int y = 0; y < 28; y++){
           for(int x = 0; x < 28; x++){
                cout << int(train_imgs[g][y*28 + x]) << " ";
           }
           cout << "\n";
        }
    }*/
}

bool SaveImage(char* szPathName, void* lpBits, int w, int h){

    //Create a new file for writing

    FILE *pFile;
    fopen_s(&pFile,szPathName, "wb");

    if(pFile == NULL)

    {

        return false;

    }

        BITMAPINFOHEADER BMIH;

        BMIH.biSize = sizeof(BITMAPINFOHEADER);

        BMIH.biSizeImage = w * h * 3;

        // Create the bitmap for this OpenGL context

        BMIH.biSize = sizeof(BITMAPINFOHEADER);

        BMIH.biWidth = w;

        BMIH.biHeight = h;

        BMIH.biPlanes = 1;

        BMIH.biBitCount = 24;

        BMIH.biCompression = BI_RGB;

        BMIH.biSizeImage = w * h* 3;

        BITMAPFILEHEADER bmfh;

        int nBitsOffset = sizeof(BITMAPFILEHEADER) + BMIH.biSize;

        LONG lImageSize = BMIH.biSizeImage;

        LONG lFileSize = nBitsOffset + lImageSize;

        bmfh.bfType = 'B'+('M'<<8);

        bmfh.bfOffBits = nBitsOffset;

        bmfh.bfSize = lFileSize;

        bmfh.bfReserved1 = bmfh.bfReserved2 = 0;

        //Write the bitmap file header

        UINT nWrittenFileHeaderSize = fwrite(&bmfh, 1,

        sizeof(BITMAPFILEHEADER), pFile);

        //And then the bitmap info header

        UINT nWrittenInfoHeaderSize = fwrite(&BMIH,

        1, sizeof(BITMAPINFOHEADER), pFile);

        //Finally, write the image data itself

        //-- the data represents our drawing

        UINT nWrittenDIBDataSize =

        fwrite(lpBits, 1, lImageSize, pFile);

        fclose(pFile);



    return true;

}
void make_image(basic_network & net,int n){
    vector<float> expected(out_size*BATCH_SIZE,0);
    expected[BATCH_SIZE*9] = 1.0;
    vector<float> err_data = net.get_idea(expected);
    vector<char> data(img_size*3);
    for(int i : range(img_size)){
        char color = err_data[i] * 256;
        data[3*i+0] = color;
        data[3*i+1] = color;
        data[3*i+2] = color;
    }
    SaveImage(("pic" + to_string(n) + ".bmp").c_str(),data.data(),28,28);
}
