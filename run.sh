#!/bin/bash
# File              : run.sh
# Author            : Yanqing Wu <meet.yanqing.wu@gmail.com>
# Date              : 17.10.2019
# Last Modified Date: 25.10.2019
# Last Modified By  : Yanqing Wu <meet.yanqing.wu@gmail.com>

if [ -z "$1" ]; then
    cd build
    make -j8
    cd ..
    #rm *.ppm
    MESA_GL_VERSION_OVERRIDE=4.3 ./build/raytracing
    exit 0
fi

if [ "$1" -eq 0 ]; then
    # Run without a fully rebuild
    MESA_GL_VERSION_OVERRIDE=4.3 ./build/raytracing
elif [ "$1" -eq 1 ]; then
    # Fresh rebuild with GDB debugging
    rm -rf build
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Debug ..
    make -j8
    cd ..
    MESA_GL_VERSION_OVERRIDE=4.3 ./build/raytracing
fi

exit 0
