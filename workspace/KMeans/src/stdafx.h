#pragma once

#include <limits.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <math.h>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/ml/ml.hpp"
#include <pthread.h>
#include <semaphore.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <time.h>
#include <vector>

#include "ioOperations.h"

using namespace cv;
using namespace std;

static int numFeatures = 0;
