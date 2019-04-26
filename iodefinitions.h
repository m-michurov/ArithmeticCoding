#pragma once

#ifndef ARITHMETICCODING_IODEFINITIONS_H
#define ARITHMETICCODING_IODEFINITIONS_H


#define FORMAT_SIZE_SIZE(size) ((size) > 1023 ? ((size) > 1048575 ? (size) / 1048576 : (size) / 1024) : (size))
#define FORMAT_SIZE_PREFIX(size) ((size) > 1023 ? ((size) > 1048575 ? "MiB" : "KiB") : "B")


#endif
