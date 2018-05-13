#include <iostream>
#include <deque>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <thread>
#include <opencv2/opencv.hpp>
#include <apps/Common/exampleHelper.h>
#include <common/minmax.h>
#include <mvIMPACT_CPP/mvIMPACT_acquire.h>
#include <mvIMPACT_CPP/mvIMPACT_acquire_GenICam.h>

#ifdef MALLOC_TRACE
#   include <mcheck.h>
#endif  // MALLOC_TRACE

using namespace std;