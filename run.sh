#!/bin/bash
# File              : run.sh
# Author            : Yanqing Wu <meet.yanqing.wu@gmail.com>
# Date              : 17.10.2019
# Last Modified Date: 17.10.2019
# Last Modified By  : Yanqing Wu <meet.yanqing.wu@gmail.com>

rm -rf build
mkdir build
cd build
cmake ..
make -j8
cd ..
#rm *.ppm
MESA_GL_VERSION_OVERRIDE=4.3 ./build/raytracing
