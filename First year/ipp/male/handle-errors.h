#ifndef IPPZAD1_HANDLE_ERRORS_H
#define IPPZAD1_HANDLE_ERRORS_H

#include "errors.h"

extern void handleAllErrors(Error errorNumber, int lineNumber);
extern void handleMemoryError(Error errorNumber);
extern void handleInputError(Error errorNumber, int lineNumber);
extern void handleOtherError(Error errorNumber);

#endif //IPPZAD1_HANDLE_ERRORS_H
