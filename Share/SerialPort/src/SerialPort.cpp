#include "SerialPort.hpp"

SerialPort::SerialPort(string port, int n_speed, int n_bits, char n_event, int n_stop)
{
    open_port(port);
    set_opt(n_speed, n_bits, n_event, n_stop);
}

//@brief:linux下的串口通信类的成员函数。
//       open_port()成员函数可以打开一个串口，set_opt()更改参数。
//@example:open_port("/dev/ttyUSB0");
//         set_opt(115200, 8, 'N', 1);
SerialPort::SerialPort(string port)
{
    open_port(port);
    set_opt();
}

SerialPort::SerialPort()
{
}
SerialPort::~SerialPort()
{
    close(fd);
}

//Serialport下的成员函数open_port()的实现；
int SerialPort::open_port(string port)
{
    // char *dev[]={"/dev/ttyS0","/dev/ttyS1","/dev/ttyS2"};
    // fd = open( "/dev/ttyS0", O_RDWR|O_NOCTTY|O_NDELAY);
    fd = open( "/dev/ttyUSB0", O_RDWR|O_NOCTTY|O_NDELAY);
    // fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (-1 == fd)
    {
        printf("\n\e[31m\e[1m ERROR:打开串口%s失败 \e[0m\n\n", port.c_str());
        return -1;
    }
    else
    {
        fcntl(fd, F_SETFL, 0);
    }

    if (fcntl(fd, F_SETFL, 0) < 0)
        printf("fcntl failed!\n");
    else
        printf("fcntl=%d\n", fcntl(fd, F_SETFL, 0));

    if (isatty(STDIN_FILENO) == 0)
        printf("standard input is not a terminal device\n");
    else
        printf("isatty success!\n");

    //printf("fd-open=%d\n",fd);

    return fd;
}

/*设置串口属性：
       fd: 文件描述符
       nSpeed: 波特率
       nBits: 数据位
       nEvent: 奇偶校验
       nStop: 停止位*/
int SerialPort::set_opt(int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio, oldtio;
    if (tcgetattr(fd, &oldtio) != 0)
    {
        perror("SetupSerial error");
        return -1;
    }
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag |= CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;
    switch (nBits)
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
        //case 9:
        //newtio.c_cflag |= CS9;
        //break;
    }
    switch (nEvent)
    {
    case 'O':
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'E':
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'N':
        newtio.c_cflag &= ~PARENB;
        break;
    }
    switch (nSpeed)
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    default:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    }

    if (nStop == 1)
        newtio.c_cflag &= ~CSTOPB;
    else if (nStop == 2)
        newtio.c_cflag |= CSTOPB;

    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;
    tcflush(fd, TCIFLUSH);

    if ((tcsetattr(fd, TCSANOW, &newtio)) != 0)
    {
        perror("com set error");
        return -1;
    }
    printf("Serial port set done!\n");
    return 0;
}

bool SerialPort::send(TargetData& td)
{
    char *p = new char[2 + 2]; //两个起始位，两个结束位
    //设置开始结束标志
    p[0] = 0xA5;
    send(p);
    p[1] = 0x5A;
    send(p + 1);
    char *data = (char *)(&td);
    int len = sizeof(TargetData);
    for (int i = 0; i < len; i++)
    {
        send(data + i);
    }

    p[2] = 0xAA;
    send(p + 2);
    p[3] = 0xAA;
    send(p + 3);
}

bool SerialPort::send(char *str)
{
    if (fd < 0 || write(fd, str, 1) < 0)
    {
        perror("\e[31m\e[1m ERROR:串口通信失败 \e[0m");
        return false;
    }

    return true;
}

// 0xA5 | 0x5A | yaw_angle | pitch_angle | 0xAA | 0xAA
bool SerialPort::sendAngle(float _angle1, float _angle2)
{
    char send_num = 0;
    unsigned char *p;
    memset(tmpchar, 0x00, sizeof(tmpchar)); //对tempchar清零
    tmpchar[0] = 0xA5;                      //起始标志
    tmpchar[1] = 0x5A;
    //tmpchar[2] = 0x06;                                //the number of data bytes
    p = (unsigned char *)&_angle1;
    tmpchar[2] = *p;       //第一个角度的低8位
    tmpchar[3] = *(p + 1); //第一个角度
    tmpchar[4] = *(p + 2); //
    tmpchar[5] = *(p + 3); //
    p = (unsigned char *)&_angle2;
    tmpchar[6] = *p;       //第二个角度的低8位
    tmpchar[7] = *(p + 1); //第二个角度
    tmpchar[8] = *(p + 2); //
    tmpchar[9] = *(p + 3); //
    tmpchar[10] = 0xAA;    //Check
    tmpchar[11] = 0xAA;    //End
                           //tmpchar[7] = 0xFE;                  //结束标志

    //cout << "Send_Angle:"<<dec<<_angle1<<","<<dec<<_angle2<<endl;
    //cout << "Send_data1:" <<hex<<(int)tmpchar[2]<<","<<hex<<(int)tmpchar[3]<<","<<hex<<(int)tmpchar[4]<<","<<hex<<(int)tmpchar[5]<<endl;
    //cout << "Send_data2:" <<hex<<(int)tmpchar[6]<<","<<hex<<(int)tmpchar[7]<<","<<hex<<(int)tmpchar[8]<<","<<hex<<(int)tmpchar[9]<<endl;
    //cout << "Check:" << hex << (int)tmpchar[10] << endl;

    for (send_num = 0; send_num < 12; send_num++)
    {
        if (!send(tmpchar + send_num))
            return false;
    }
    cout << "Send successfully!" << endl;
    return true;
}

// 0xA5 | 0x5A | yaw_angle | pitch_angle | 0xAA | 0xAA
bool SerialPort::sendAngle(float _angle1, float _angle2, float Dis, bool big, bool insight, bool get)
{
    char send_num = 0;
    unsigned char *p;
    memset(tmpchar, 0x00, sizeof(tmpchar)); //对tempchar清零
    tmpchar[0] = 0xA5;                      //起始标志
    tmpchar[1] = 0x5A;
    p = (unsigned char *)&_angle1;
    tmpchar[2] = *p;
    tmpchar[3] = *(p + 1);
    tmpchar[4] = *(p + 2);
    tmpchar[5] = *(p + 3);
    p = (unsigned char *)&_angle2;
    tmpchar[6] = *p;
    tmpchar[7] = *(p + 1);
    tmpchar[8] = *(p + 2);
    tmpchar[9] = *(p + 3);
    p = (unsigned char *)&Dis;
    tmpchar[10] = *p;
    tmpchar[11] = *(p + 1);
    tmpchar[12] = *(p + 2);
    tmpchar[13] = *(p + 3);
    tmpchar[14] = big;
    tmpchar[15] = insight;
    tmpchar[16] = get;
    tmpchar[17] = 0xAA; //Check
    tmpchar[18] = 0xAA; //End

    // cout << "Send_Angle:"<<dec<<_angle1<<","<<dec<<_angle2<<endl;
    // cout << "isget = " << get << endl;
    // cout << "is_big = " << big << endl;
    // cout << "Send_data:" <<hex<<(int)tmpchar[2]<<","<<hex<<(int)tmpchar[3]<<","<<hex<<(int)tmpchar[4]<<","<<hex<<(int)tmpchar[5]<<endl;
    // cout << "Send_data:" <<hex<<(int)tmpchar[6]<<","<<hex<<(int)tmpchar[7]<<","<<hex<<(int)tmpchar[8]<<","<<hex<<(int)tmpchar[9]<<endl;
    // cout << "Check:" << hex << (int)tmpchar[10] << endl;

    for (send_num = 0; send_num < 19; send_num++)
    {
        if (!send(tmpchar + send_num))
            return false;
    }
    // cout<<"Send successfully!"<<endl;
    return true;
}

// 0xA5 | 0x5A | hitmode | 0xff
void SerialPort::readMode(int &carMode)
{
    int bytes;
    ioctl(fd, FIONREAD, &bytes);
    if (bytes == 0)
        return;
    bytes = read(fd, rData, 6);
    if (rData[0] == 0xA5)
    {
        carMode = rData[2];
        printf("receive mode:%d\r\n", carMode);
    }
    ioctl(fd, FIONREAD, &bytes);
    if (bytes > 0)
    {
        read(fd, rData, bytes);
    }
    // cout << "guapi is here !!! \n" << endl;
}

// wantLists:   1.自检是否开了串口
//              2.自检是否发送成功
//              3.自检是否接收成功
//              4.自检数据解算是否正确
//              5.验证发送数据是否正确

bool SerialPort::sendAngleDist(float _angle1, float _angle2, float _angle3, float _angle4)
{
    char tmpchar[20];
    char send_num = 0;
    unsigned char *p;
    memset(tmpchar, 0x00, sizeof(tmpchar)); //对tempchar清零
    tmpchar[0] = 0xA5;                      //起始标志
    tmpchar[1] = 0x5A;
    //tmpchar[2] = 0x06;                                //the number of data bytes
    p = (unsigned char *)&_angle1;
    tmpchar[2] = *p;       //第一个角度的低8位
    tmpchar[3] = *(p + 1); //第一个角度
    tmpchar[4] = *(p + 2); //
    tmpchar[5] = *(p + 3); //
    p = (unsigned char *)&_angle2;
    tmpchar[6] = *p;       //第二个角度的低8位
    tmpchar[7] = *(p + 1); //第二个角度
    tmpchar[8] = *(p + 2); //
    tmpchar[9] = *(p + 3); //
    p = (unsigned char *)&_angle3;
    tmpchar[10] = *p;       //第三个角度的低8位
    tmpchar[11] = *(p + 1); //第三个角度
    tmpchar[12] = *(p + 2); //
    tmpchar[13] = *(p + 3); //、
    p = (unsigned char *)&_angle4;
    tmpchar[14] = *p;       //第三个角度的低8位
    tmpchar[15] = *(p + 1); //第三个角度
    tmpchar[16] = *(p + 2); //
    tmpchar[17] = *(p + 3); //
    tmpchar[18] = 0xAA;     //Check
    tmpchar[19] = 0xAA;     //End
                            //tmpchar[7] = 0xFE;                  //结束标志
                            //cout << "Send_Angle:"<<dec<<_angle1<<","<<dec<<_angle2<<endl;
                            //cout << "Send_data1:" <<hex<<(int)tmpchar[2]<<","<<hex<<(int)tmpchar[3]<<","<<hex<<(int)tmpchar[4]<<","<<hex<<(int)tmpchar[5]<<endl;
                            //cout << "Send_data2:" <<hex<<(int)tmpchar[6]<<","<<hex<<(int)tmpchar[7]<<","<<hex<<(int)tmpchar[8]<<","<<hex<<(int)tmpchar[9]<<endl;
                            //cout << "Send_data3:" <<hex<<(int)tmpchar[10]<<","<<hex<<(int)tmpchar[11]<<","<<hex<<(int)tmpchar[12]<<","<<hex<<(int)tmpchar[13]<<endl;
    //cout << "Check:" << hex << (int)tmpchar[10] << endl;
    for (send_num = 0; send_num < 20; send_num++)
    {
        if (!send(tmpchar + send_num))
            return false;
    }
    //cout<<"Send successfully!"<<endl;
    return true;
}
