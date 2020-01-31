
#ifndef CMMC_BUCKET_H
#define CMMC_BUCKET_H
#include <Arduino.h>

class CMMC_Bucket {
  public:
    CMMC_Bucket();
    ~CMMC_Bucket();
    float add(float val);
    float get();
  protected:
  private:
    std::vector<float> v;
};

#endif
