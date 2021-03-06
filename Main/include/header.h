/*
 * @Copyright: CS of BIT
 * @Author: 王占坤
 * @File name: 
 * @Version: 
 * @Date: 2019-08-31 10:33:58 +0800
 * @LastEditTime : 2019-12-23 21:48:11
 * @LastEditors  : zzdr
 * @Description: 
 */
#ifndef __HEADER_H__
#define __HEADER_H__

#include "config.h"

//////////////C/C++//////////////
#include <stdio.h>
#include <algorithm>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <thread>
#include <linux/videodev2.h>
#include <errno.h>

#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>

//////////////OpenCV///////////////
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

//////////////其他//////////////
#include "GxIAPI.h"
// #include "Serialport.h"
#include "DxImageProc.h"

#include <cjson/cJSON.h>

//////////namespace/////////////
using namespace cv;
using namespace std;

////////////////////define/////////////////////


////////////////////typedef////////////////////

///////////////////自定义数据类型//////////////////
// typedef enum CORPS
// {
//     SENTRY = 1,   //哨兵
//     INFANTRY = 2, //步兵
//     HERO = 3,     //英雄
//     UAV = 4       //无人机
// } CORPS;

typedef struct TargetData
{
    bool is_get; //main<-ArmorDetector，ArmorData是否清零
    bool shoot;
    //以度为单位
    float yaw_angle;   //AngleCalculate->ArmorDetector->main->Serial
    //以度为单位
    float pitch_angle; //AngleCalculate->ArmorDetector->main->Serial
    float x;
    float y;
    float z;
    //以毫米为单位
    float atocDistance; //ArmorDetector<-AngleCalculate，装甲板决策，main<-ArmorDetector
    bool is_big;        //ArmorDetector->AngleCalculate，计算pnp
    TargetData()
    {
        is_get = false;
        shoot = false;
        yaw_angle = 0;
        pitch_angle = 0;
        x = 0;
        y = 0;
        z = 0;
        atocDistance = 0;
        is_big = false;
    }
    struct TargetData operator=(const struct TargetData &a)
    {
        is_get = a.is_get;
        shoot = a.is_get;
        yaw_angle = a.yaw_angle;
        pitch_angle = a.pitch_angle;
        x = a.x;
        y = a.y;
        z = a.z;
        atocDistance = a.atocDistance;
        is_big = a.is_big;
    }
    void print()
    {
        cout << "\t\tx=" << x << "\n"
            << "\t\t\ty=" << y << "\n"
            << "\t\t\tz=" << z << "\n"
            << "\t\tatocDistance=" << atocDistance << "\n"
            << "\t\t\tis_get:" << is_get << "\n"
            << "\t\tyaw_angle=" << yaw_angle << "\n"
            << "\t\tpitch_angle=" << pitch_angle << "\n";
    }

    void toJson(cJSON* j)
    {
        cJSON_AddNumberToObject(j, "yaw_angle", yaw_angle);
        cJSON_AddNumberToObject(j, "pitch_angle", pitch_angle);
        cJSON_AddNumberToObject(j, "distance", atocDistance / 1000);//转为以米为单位
        chrono::system_clock::time_point now_time = chrono::system_clock::now();
        int64_t send_time =  chrono::duration_cast<chrono::milliseconds>(now_time.time_since_epoch()).count();
        cJSON_AddNumberToObject(j, "timestamp", send_time);
    }

} TargetData; //armorData(mian)<->armordeta(ArmorDetector)<->point4Data(AngleCalculate)

///////////////////全局变量//////////////////////

#endif
