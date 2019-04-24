#pragma once

#ifndef ARITHMETICCODING_ERRORS_H
#define ARITHMETICCODING_ERRORS_H

#define ALLOCATION_ERROR                (-1)
#define DATA_CORRUPTION_ERROR           (-2)
#define COMMAMD_LINE_ARGUMENTS_ERROR    (-4)
#define IN_FILE_ERROR                   (-5)
#define OUT_FILE_ERROR                  (-6)
#define FILE_SIZE_ERROR                 (-7)

#define SUCCESS                         0


#define CODE_TO_MESSAGE(code)   ((code) == -1 ? "ALLOCATION_ERROR" : \
                                ((code) == -2 ? "DATA_CORRUPTION_ERROR" : \
                                ((code) == -5 ? "IN_FILE_ERROR" : \
                                ((code) == -6 ? "OUT_FILE_NAME" : \
                                ((code) == -7 ? "FILE_SIZE_ERROR" : \
                                ((code) ==  0 ? "SUCCESS" : "UNDEFINED"))))))


#endif
