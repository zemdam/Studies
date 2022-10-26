#include <stdio.h>
#include <stdlib.h>
#include "handle-errors.h"

void sendErrorToStdErr(Error errorNumber) {
    fprintf(stderr, "ERROR %d\n", errorNumber);
}

void sendLineNumberToStdErr(int lineNumber) {
    fprintf(stderr, "ERROR %d\n", lineNumber);
}

void handleMemoryError(Error errorNumber) {
    if(errorNumber == MEMORY_ERROR) {
        sendErrorToStdErr(OTHER_ERROR);
        exit(1);
    }
}

void handleInputError(Error errorNumber, int lineNumber) {
    if(errorNumber == INPUT_ERROR) {
        sendLineNumberToStdErr(lineNumber);
    }
}

void handleOtherError(Error errorNumber) {
    if(errorNumber == OTHER_ERROR) {
        sendErrorToStdErr(OTHER_ERROR);
    }
}

void handleAllErrors(Error errorNumber, int lineNumber) {
    handleMemoryError(errorNumber);
    handleOtherError(errorNumber);
    handleInputError(errorNumber, lineNumber);
}
