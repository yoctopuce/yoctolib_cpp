#ifndef YOCTO_API_BUILD_NO
#define YOCTO_API_BUILD_NO          "35900"
#endif
#define YBUILD_35900

#ifndef YOCTO_API_BUILD_DATE
#ifndef RC_INVOKED
#define YOCTO_API_BUILD_DATE          "" __DATE__ " " __TIME__ ""
#else
#define YOCTO_API_BUILD_DATE          "PATCH_WITH_DATE"
#endif
#endif
