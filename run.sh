#!/bin/bash
# File              : run.sh
# Author            : Yanqing Wu <meet.yanqing.wu@gmail.com>
# Date              : 17.10.2019
# Last Modified Date: 24.10.2019
# Last Modified By  : Yanqing Wu <meet.yanqing.wu@gmail.com>

# Run without a fully rebuild
if [ "$1" -eq 0 ]; then
    MESA_GL_VERSION_OVERRIDE=4.3 ./build/raytracing
    exit 0
fi

rm -rf build
mkdir build
cd build
cmake ..
make -j8
cd ..
#rm *.ppm
MESA_GL_VERSION_OVERRIDE=4.3 ./build/raytracing
