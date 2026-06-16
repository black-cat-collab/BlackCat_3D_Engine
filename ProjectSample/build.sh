#! /bin/bash

BUILD_TYPE="Release"        #Debug,Release
ARCH_TYPE="x64"           #x64,aarch64
CURRENT_DIR=$(cd $(dirname $0); pwd)
CPU_NUM=`cat /proc/stat | grep cpu[0-9] -c`


do_make()
{
   make -j${CPU_NUM}
}

clear_makefile()
{
   rm -fr CMakeCache.txt  CMakeFiles  cmake_install.cmake
}

cmake_config()
{
   cmake . -DBUILD_TYPE=$1 -DARCH_TYPE=$2
}

cmake_dir()
{
	cd $1
	cmake_config $2 $3
    do_make
    cd $CURRENT_DIR
}

clean_dir()
{
   cd $1
   clear_makefile
   cd $CURRENT_DIR
}

cmake_all()
{
   dir=("Utility" "Core" "Engine" "Model" "ProjectBased" "Protocol" "Render")
	   
   for loop in 0 1 2 3 4 5 6 7 8
   do
      cmake_dir ${dir[loop]} $1 $2
   done
}

clean_all()
{
   dir=("Utility" "Core" "Engine" "Model" "ProjectBased" "Protocol" "Render")
   for loop in 0 1 2 3 4 5 6 7 8
   do
      echo "clean "${dir[loop]}
	  clean_dir ${dir[loop]}
   done
   
}


if [ $# -eq 0 ]; then
   do_make
elif [ $# -eq 1 ]; then
	params=$(echo $1 | tr '[A-Z]' '[a-z]') 
	
	if [ $params = "config" ]; then
	   clear_makefile
	   clear;cmake_config $BUILD_TYPE $ARCH_TYPE
	elif [ $params = "debug" ]; then
	   clear_makefile
	   clear;cmake_config "Debug" $ARCH_TYPE
	elif [ $params = "release" ]; then
	   clear_makefile
	   clear;cmake_config "Release" $ARCH_TYPE
	elif [ $params = "clean" ]; then
	   make clean;do_make
	elif [ $params = "clear" ]; then
	   clear;do_make
	elif [ $params = "all" ]; then
	   cmake_all $BUILD_TYPE
	elif [ $params = "help" ]; then
	   echo "config  : config makefile as default setting."
	   echo "debug   : compile debug version."
	   echo "release : compile release version."
	   echo "clean   : clean obj files before compilation."
	   echo "clear   : clear screen before compilation."
	   echo "all     : compile all project directory."
	   echo "all <debug/release>: compile all project directory with debug/release version."
	   echo "help    : help text."
	  
	else
	   STR=$1
	   lastChar=$(echo ${STR: -1})
	   theDir=$1
	   
	   if [ $lastChar = "/" ]; then
	      theDir=$(echo ${STR%?})
	   fi
	
	   cmake_dir $theDir $BUILD_TYPE $ARCH_TYPE
	fi

elif [ $# -eq 2 ]; then	
	param1="all"
	param2="debug"
	
	params1=$(echo $1 | tr 'A-Z' 'a-z')
	params2=$(echo $2 | tr 'A-Z' 'a-z')
	

	if [ $params1 = "all" ]; then
	   if [ $params2 = "debug" ]; then 
		 clear;cmake_all "Debug" $ARCH_TYPE
	   elif [ $params2 = "release" ]; then
	     clear;cmake_all "Release" $ARCH_TYPE
	   elif [ $params2 = "clean" ]; then
	     clean_all
	   else
	     clear;cmake_dir $1 $param2 $ARCH_TYPE
	   fi
	fi
fi
