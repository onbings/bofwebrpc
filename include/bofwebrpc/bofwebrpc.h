/*!
  Copyright (c) 2008, EVS. All rights reserved.

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  History:
    V 1.00  August 24 2013  BHA : Original version
*/
#pragma once
/*
cmake -DCMAKE_TOOLCHAIN_FILE=C:/pro/vcpkg/scripts/buildsystems/vcpkg.cmake -DBUILD_SHARED_LIBS=OFF -DVCPKG_TARGET_TRIPLET=evs-x64-windows-static -DBOFSTD_GENERATE_HELP=ONN C:/pro/evs-gbio/webrpc
*/

#define BOFWEBRPC onbings::bofwebrpc
#define BEGIN_WEBRPC_NAMESPACE() \
  namespace onbings              \
  {                              \
  namespace bofwebrpc               \
  {
#define END_WEBRPC_NAMESPACE()   \
  }                              \
  }
#define USE_WEBRPC_NAMESPACE() using namespace BOFWEBRPC;

BEGIN_WEBRPC_NAMESPACE()

END_WEBRPC_NAMESPACE()
