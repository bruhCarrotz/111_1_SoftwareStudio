#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "FIRFilter.h"

#define SIZE 16 
#define FORMAT 1 
#define SAMPLE_RATE 44100
#define CHANNELS 1 
#define SIZE_COEF 65
#define LENGTH 1000 

//filter coef generated from the website
float GlobalCoeff[SIZE_COEF] = {
    0.009840869910806505,
    0.001872206941095882,
    -0.009933880929786575,
    -0.004992160295397135,
    0.009138643696330237,
    0.008120153742215953,
    -0.007387444227510984,
    -0.011005797420753928,
    0.004666491656801765,
    0.013380000751638645,
    -0.001019619462549121,
    -0.014965165863916228,
    -0.003451628893299144,
    0.015483018423007353,
    0.008591638448066282,
    -0.014656964977183827,
    -0.014197942089763502,
    0.012203603742142612,
    0.020031631328717086,
    -0.007802647121739918,
    -0.025830345901959761,
    0.001020266605663760,
    0.031323045552947230,
    0.008882129716964721,
    -0.036245635713823873,
    -0.023475735836296568,
    0.040356449424484964,
    0.046960363404796653,
    -0.043450586069254830,
    -0.095128326164481775,
    0.045372174045468297,
    0.311795071124927881,
    0.449012244903282953,
    0.311795071124927881,
    0.045372174045468297,
    -0.095128326164481775,
    -0.043450586069254830,
    0.046960363404796653,
    0.040356449424484964,
    -0.023475735836296568,
    -0.036245635713823873,
    0.008882129716964721,
    0.031323045552947230,
    0.001020266605663760,
    -0.025830345901959761,
    -0.007802647121739918,
    0.020031631328717086,
    0.012203603742142612,
    -0.014197942089763502,
    -0.014656964977183827,
    0.008591638448066282,
    0.015483018423007353,
    -0.003451628893299144,
    -0.014965165863916228,
    -0.001019619462549121,
    0.013380000751638645,
    0.004666491656801765,
    -0.011005797420753928,
    -0.007387444227510984,
    0.008120153742215953,
    0.009138643696330237,
    -0.004992160295397135,
    -0.009933880929786575,
    0.001872206941095882,
    0.009840869910806505,
};

//converted coef from float to int16
int16_t convertedCoef[SIZE_COEF] = {   
    322,
    61, -325, -163, 299, 266, -242, -360, 152, 438, -33,
    -490, -113, 507, 281, -480, -465, 399, 656, -255, -846,
    33, 1026, 291, -1187, -769, 1322, 1538, -1423, -3117, 1486,
    10216, 14713, 10216, 1486, -3117, -1423, 1538, 1322, -769, -1187,
    291, 1026, 33, -846, -255, 656, 399, -465, -480, 281,
    507, -113, -490, -33, 438, 152, -360, -242, 266, 299,
    -163, -325, 61, 322
};
int16_t coef_int16[SIZE_COEF] = {0};

//converting float to int16
void float_to_int16 (float *arr1, int16_t *arr2){
    for(int i=0; i<SIZE_COEF; i++){
        int value = arr1[i]*32768;
        if(value>=32768) arr2[i]=32768;
        else if(value<=-32768) arr2[i]=-32768;
        else arr2[i]=(int16_t)value;
    }
}

FILE *inputWAV; 
FILE *outputWAV;
char fileInput[2000];
char fileOutput[2000]; 

typedef struct WAVE_HEADER{
    char RIFF_ID[4];
    unsigned int RIFF_Size;
    char RIFF_TYPE[4];
} WAV_HEADER_DECLARE;

typedef struct FMT_CHUNK{
    char FMT_ID[4];
    unsigned int FMT_Size;
    short int Format_Compression_Type;
    short int Channels;
    unsigned int Sample_Rate;
    unsigned int Byte_Rate;
    short int Block_align;
    short int Bit_per_sample;
} WAV_FMT_CHUNK_DECLARE;

typedef struct Data_Chunk{
    char Data_ID[4];
    unsigned int Data_Size;
} WAV_DATA_CHUNK_DECLARE;

typedef struct WAV_Format
{
    WAV_HEADER_DECLARE Wav_Header_Data;
    WAV_FMT_CHUNK_DECLARE Wav_FMT_Chunk_Data;
    WAV_DATA_CHUNK_DECLARE Wav_Data_Chunk_Data;
} WAV_FORMAT_DECLARE;

typedef struct WAV_Additional_Information{
    long noSamples; //(8*Data_Size)/(Channels*Bit_per_sample);
    long sampleSize; //(Channels *Bit_per_sample)/8
    float duration;  //(float) RIFF_Size/Byte_Rate;
    long bytesChannels; // (sampleSize/Channels);

} WAV_AddInf_DECLARE;

void WAV_Write_AddInf (WAV_FORMAT_DECLARE *WavTargetStruct, WAV_AddInf_DECLARE *WavAddInfTargetStruct){
    WavAddInfTargetStruct->noSamples = (8*WavTargetStruct->Wav_Data_Chunk_Data.Data_Size)/WavTargetStruct->Wav_FMT_Chunk_Data.Bit_per_sample;
    WavAddInfTargetStruct->sampleSize = WavTargetStruct->Wav_FMT_Chunk_Data.Bit_per_sample/8;
    WavAddInfTargetStruct->duration = (float)WavTargetStruct->Wav_Header_Data.RIFF_Size/WavTargetStruct->Wav_FMT_Chunk_Data.Byte_Rate;
    WavAddInfTargetStruct->bytesChannels = WavAddInfTargetStruct->sampleSize;
}

void WAV_FORMAT_OUTPUT (WAV_FORMAT_DECLARE *PrintTargetStruct, WAV_FORMAT_DECLARE *WavSource, WAV_AddInf_DECLARE *WavSourceAddInf)
{
    WAV_FORMAT_DECLARE *PTS;
    PTS = PrintTargetStruct;

    WAV_HEADER_DECLARE *PTS_Wav_Header = &PTS->Wav_Header_Data;
    WAV_FMT_CHUNK_DECLARE *PTS_Wav_FC = &PTS->Wav_FMT_Chunk_Data;
    WAV_DATA_CHUNK_DECLARE *PTS_Wav_DC = &PTS->Wav_Data_Chunk_Data;

    PTS->Wav_Header_Data.RIFF_ID[0] = 'R';
    PTS->Wav_Header_Data.RIFF_ID[1] = 'I';
    PTS->Wav_Header_Data.RIFF_ID[2] = 'F';
    PTS->Wav_Header_Data.RIFF_ID[3] = 'F';

    PTS->Wav_Header_Data.RIFF_Size = WavSource->Wav_Header_Data.RIFF_Size;

    PTS->Wav_Header_Data.RIFF_TYPE[0] = 'W';
    PTS->Wav_Header_Data.RIFF_TYPE[1] = 'A';
    PTS->Wav_Header_Data.RIFF_TYPE[2] = 'V';
    PTS->Wav_Header_Data.RIFF_TYPE[3] = 'E';

    PTS->Wav_FMT_Chunk_Data.FMT_ID[0] = 'f';
    PTS->Wav_FMT_Chunk_Data.FMT_ID[1] = 'm';
    PTS->Wav_FMT_Chunk_Data.FMT_ID[2] = 't';
    PTS->Wav_FMT_Chunk_Data.FMT_ID[3] = ' ';

    PTS->Wav_FMT_Chunk_Data.FMT_Size = SIZE;
    PTS->Wav_FMT_Chunk_Data.Format_Compression_Type = FORMAT;
    PTS->Wav_FMT_Chunk_Data.Channels = CHANNELS;
    PTS->Wav_FMT_Chunk_Data.Sample_Rate = WavSource->Wav_FMT_Chunk_Data.Sample_Rate;
    PTS->Wav_FMT_Chunk_Data.Byte_Rate = WavSource->Wav_FMT_Chunk_Data.Byte_Rate;
    PTS->Wav_FMT_Chunk_Data.Block_align = 2;
    PTS->Wav_FMT_Chunk_Data.Bit_per_sample = 16;

    PTS->Wav_Data_Chunk_Data.Data_ID[0] = 'd';
    PTS->Wav_Data_Chunk_Data.Data_ID[1] = 'a';
    PTS->Wav_Data_Chunk_Data.Data_ID[2] = 't';
    PTS->Wav_Data_Chunk_Data.Data_ID[3] = 'a';

    PTS->Wav_Data_Chunk_Data.Data_Size = WavSource->Wav_Data_Chunk_Data.Data_Size;
}

int main(int argc, char *argv[]){
    //take from command line
    strcat(fileInput, argv[1]);
    strcat(fileOutput, argv[2]);

    //declare all necessary info
    WAV_FORMAT_DECLARE inputStruct;
    WAV_FORMAT_DECLARE outputStruct;
    WAV_AddInf_DECLARE addInfo;
    FIRFilter filter;
    size_t error;

    //open input.wav
    inputWAV = fopen(fileInput, "rb");
    fread(&inputStruct, 1 ,sizeof(WAV_FORMAT_DECLARE), inputWAV);
    WAV_Write_AddInf(&inputStruct, &addInfo);

    //memory allocation
    int16_t *InputSamples = (int16_t*)malloc(addInfo.noSamples * sizeof(int16_t));
    int16_t *OutputSamples = (int16_t*)malloc(addInfo.noSamples * sizeof(int16_t));
    error = fread(InputSamples, inputStruct.Wav_FMT_Chunk_Data.Block_align, addInfo.noSamples, inputWAV);

    //converting float to int16
    float_to_int16(GlobalCoeff, coef_int16);

    //start the firfilter
    FIRFilter_Init(&filter);

    //updating each firfilter input to output
    for(int i=0; i<addInfo.noSamples; i++){
        OutputSamples[i] = FIRFilter_Update(&filter, InputSamples[i]);
    }

    //write out.wav
    WAV_FORMAT_OUTPUT(&outputStruct, &inputStruct, &addInfo);
    outputWAV = fopen(fileOutput, "wb");

    //check for error in header
    error = fwrite(&outputStruct, sizeof(WAV_FORMAT_DECLARE), 1, outputWAV);
    if(error != 1) printf("Error Write Header\n");

    //check for error in samples
    error = fwrite(OutputSamples, sizeof(int16_t), addInfo.noSamples, outputWAV);
    if(error != addInfo.noSamples) printf("Error Write Samples\n");

    //close files and free resources
    fclose(inputWAV);
    fclose(outputWAV);
    free(InputSamples);
    free(OutputSamples);

    printf("Accepted\n");

    return 0;
}
