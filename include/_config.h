#pragma once

#define BEATTOGETHER_EXPORT __attribute__((visibility("default")))

#ifdef __cplusplus
#define BEATTOGETHER_EXPORT_FUNC extern "C" BEATTOGETHER_EXPORT
#else
#define BEATTOGETHER_EXPORT_FUNC BEATTOGETHER_EXPORT
#endif
