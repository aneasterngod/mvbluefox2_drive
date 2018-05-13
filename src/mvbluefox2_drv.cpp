#include "mvbluefox2_common.h"
#include "mvbluefox2_io.h"

int main(int argc, char **argv)
{

    mvbluefox2_io mv;
    bool chk = false;
    if (argc == 2)
        chk = mv.init(argv[1]);
    else
        //chk = mv.init("26803931");
        chk = mv.init("26802554");
    if (!chk)
        return 0;
    // default 20000
    //mv.setExposure(7000);

    mv.startCapture();

    while (1)
    {
        if (mv.m_data.size() != 0)
        {
            imshow("test", mv.m_data.front());
            cv::waitKey(10);            
            mv.m_data.pop_front();
        }
    }
    return 0;
}