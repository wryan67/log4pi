
#include "FileUtil.h"
#include "Logger.h"
#include "string.h"


namespace common { namespace utility {

    FileUtil::FileUtil(long maxBytes, long maxRecords, int maxLineSize) {
            if (maxBytes>0) {
                this->maxBytes=maxBytes;
            }
            if (maxRecords>0) {
                this->maxRecords=maxRecords;
            }
            if (maxLineSize>0) {
                this->maxLineSize=maxLineSize;
            }
        }


    long FileUtil::read2vector(vector<strLine*> &myVector, FILE *inputFile, bool closeFile) {
        if (feof(inputFile)) {
            fprintf(stderr,"pipe is closed before reading\n"); fflush(stderr);
        }

        long bytes=0;
        while (!feof(inputFile)) {
            char buf[maxLineSize];
            int bytesRead = fread(buf, 1, sizeof(buf), inputFile);

            if (bytesRead>0) {
                strLine *line = (strLine *) malloc(sizeof(strLine));
                line->len = bytesRead;
                line->data = (char*)malloc(bytesRead);
                memcpy(line->data,buf,bytesRead);
                myVector.push_back(line);
                bytes+=bytesRead;
            }
            if (bytes > maxBytes) {
                break;
            }
            

            if ((long)myVector.size() > maxRecords) {
                break;
            }
        }
        return bytes;
    }   

}}
