#ifndef YOCTO_API_BUILD_NO
#define YOCTO_API_BUILD_NO          "64286"
#endif
#define YBUILD_64286

#ifndef YOCTO_API_BUILD_DATE
#ifndef RC_INVOKED
#define YOCTO_API_BUILD_DATE          "" __DATE__ " " __TIME__ ""
#else
#define YOCTO_API_BUILD_DATE          "PATCH_WITH_DATE"
#endif
#endif
