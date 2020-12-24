/***********************************
* File:     main.cpp.c
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2020/12/24
***********************************/

#include <stdio.h>
#include <ctime>
#include <string>
#include "speex_resampler.h"

using namespace std;

/**
 resampler api的总体使用流程是
 >  1, 初始化对象
 >  2, 进行每帧处理
 >  3, 所有处理完后释放对象
 */

void resampler(string inputFilePath, string outputFilePath, int inputSamplerate,
               int outputSamplerate, int channels, int ms, int quality) {
    
        printf("start...\n");
        /**
         * channels: 支持单通道和多通道. eg: 1 or 2 or 3...
         * inputSamplerate: 原始采样率, 比如8k/16k/32k/44.1k/48k等常见采样率
         * outputSamplerate: 转换后采样率, 比如8k/16k/32k/44.1k/48k等常见采样率
         * quality: 转换质量，取值为1~10，值越高，效果越好，but 计算量越高，需要权衡.
         * err: 返回初始化的错误信息（正确初始化之后就不会返回啥错误信息）
         */
        int err = 0;
        SpeexResamplerState *resampler = elevoc_resampler_init(channels, inputSamplerate, outputSamplerate, quality, &err);
    
        if (resampler == NULL) {
            printf("resampler create error!!!\n");
            return;
        }
    
        FILE *inputp = fopen(inputFilePath.c_str(), "rb");
        if (inputp == NULL) {
            printf("%s open failed\n", inputFilePath.c_str());
            return;
        }
    
        FILE *outputp = fopen(outputFilePath.c_str(), "wb");
        if (outputp == NULL) {
            printf("%s open failed\n", outputFilePath.c_str());
            return;
        }
    
        size_t readLen = 0;
        int cnt = 0;
        float inbuf[4098];
        float outbuf[4098];
        int inputFrameSizePerChannel = inputSamplerate/1000 * ms;
        int outputFrameSizePerChannel = outputSamplerate/1000 * ms;
        int inputframeSize = inputFrameSizePerChannel * channels;
        do {
            readLen = fread(inbuf, sizeof(float), inputframeSize, inputp);
            if (readLen < inputframeSize) {
                break;
            }
            
            unsigned int inlen = inputFrameSizePerChannel;
            unsigned int outlen = outputFrameSizePerChannel;

            /**
             * 处理多通道交织数据
             *  inbuf: 读取的一帧多路交织数据，这里是10ms的长度
             *  inlen: 注意此处输入的是每个通道一帧的长度：eg 输入采样率为48000，这里即输入要480
             *  outbuf: 获取到的一帧多路输出数据
             *  outlen: 注意此处输入的是输出的每个通道一帧长度: eg 转换后采样率16000，这里即输入要160
             *  如若输入的时间长度不match，比如14ms之类，此api会修改inlen和outlen
             *  此api内部会调用elevoc_resampler_process_float，此api更通用
             */
            clock_t start = clock();
            int ret = speex_resampler_process_interleaved_float(resampler, inbuf, &inlen, outbuf, &outlen);
            clock_t end = clock();
            printf("time: %f\n", (double)(end-start)/1000);
            
            if (ret == RESAMPLER_ERR_SUCCESS) {
                printf("cnt:%d, intlen: %d, outlen: %d\n", cnt++, inlen * channels, outlen * channels);
                fwrite(outbuf, sizeof(float), outlen * channels, outputp);
            }else {
                printf("error\n");
            }
        } while (true);
        
        /**
         * 释放对象
         */
        elevoc_resampler_destroy(resampler);
    
        fclose(inputp);
        fclose(outputp);
        
        printf("end...\n");
}

int main(int argc, const char * argv[]) {
    
    string inputPath = "/Users/caipeng/Desktop/resamplertest/test48k4chs.raw";
    string outputPath = "/Users/caipeng/Desktop/resamplertest/48to16k4chs.pcm";
    int inputSamplerate = 48000;
    int outputSamplerate = 16000;
    int channel = 4; // 4 channels
    int frameTime = 10; //10ms
    int quality = 7;
    
    resampler(inputPath, outputPath, inputSamplerate, outputSamplerate, /*chanel*/channel, /*time*/frameTime, quality);
    
    return 0;
}
