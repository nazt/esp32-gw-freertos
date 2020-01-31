#include "CMMC_Bucket.h"

CMMC_Bucket::CMMC_Bucket() {
}
CMMC_Bucket::~CMMC_Bucket() {
}

float CMMC_Bucket::add(float val) {
  v.push_back(val);
  std::sort(v.begin(), v.end());
  float r = get();

  if (v.size() == 20) {
    v.clear();
  }

  return r;
}

float CMMC_Bucket::get() {
  int val;
  if (v.size() == 0) {
    return 0;
  }
  return v[v.size()/2];
};
