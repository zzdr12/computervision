#include "OrdinaryCamera.h"

#ifdef ORDINARY_CAMERA

OrdinaryCamera::OrdinaryCamera(string camera_name):Camera(camera_name)
{
    this->camera_handle = nullptr;
    this->is_opened = false;
}

OrdinaryCamera::~OrdinaryCamera()
{
}


int OrdinaryCamera::open()
{
    ord_cam.open(cam_name);
    if(ord_cam.isOpened() == false)
    {
        this->is_opened = false;
        return -1;
    }
    this->is_opened = true;
    return 0;
}

Mat OrdinaryCamera::getFrame()
{
    ord_cam >> img;
    return img.clone();
}

#endif