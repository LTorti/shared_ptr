#include <iostream>
#include <memory>
#include <string>
#include <cmath>

#include <agrum/core/timer.h>
#include <agrum/core/hashTable.h>
#include <agrum/variables/labelizedVariable.h>

#define RUN_COUNT 1000

double avg( const std::vector<double>& v ) {
  double sum = 0.0;
  for (auto d: v) {
    sum += d;
  }
  return sum / (double)v.size();
}

double cov( const std::vector<double>& v ) {
  double sum = 0.0;
  double mean = avg(v);
  for (auto d: v) {
    sum += std::pow(d - mean, 2);
  }
  return sum / (double)v.size();
}

gum::HashTable<long, gum::DiscreteVariable* > normal_ptr(long count) {
  gum::HashTable<long, gum::DiscreteVariable* > hash;
  for (long i = 0; i < count; ++i) {
    std::string name = "var_" + i;
    auto var = new gum::LabelizedVariable( name );
    hash.insert( i,  var );
  }
  return hash;
}

gum::HashTable<long, std::shared_ptr< gum::DiscreteVariable >> shared_ptr(long count) {
  gum::HashTable<long, std::shared_ptr< gum::DiscreteVariable >> hash;
  for (long i = 0; i < count; ++i) {
    std::string name = "var_" + i;
    auto var = std::make_shared< gum::LabelizedVariable >( name );
    hash.insert( i,  var );
  }
  return hash;
}

template <typename HASH>
long use(HASH hash) {
  long size = 1;
  for (auto iter: hash) {
    size *= iter.second->domainSize();
  }
  return size;
}

void run_normal(std::vector<double>& normal, std::vector<double>& normal_use) {
  gum::Timer timer;
  timer.reset();
  auto normal_hash = normal_ptr(RUN_COUNT);
  normal.push_back( timer.pause() );
  timer.reset();
  auto foo = use<gum::HashTable<long, gum::DiscreteVariable*>>(normal_hash);
  normal_use.push_back( timer.pause() );
  for (auto iter: normal_hash) {
    delete iter.second;
  }
}

void run_shared(std::vector<double>& shared, std::vector<double>& shared_use) {
  gum::Timer timer;
  timer.reset();
  auto shared_hash = shared_ptr(RUN_COUNT);
  shared.push_back( timer.pause() );
  timer.reset();
  auto bar = use< gum::HashTable<long, std::shared_ptr< gum::DiscreteVariable >> >(shared_hash);
  shared_use.push_back( timer.pause() );
}

int main(int argc, char* argv[])
  try {
    std::vector<double> normal, shared, normal_use, shared_use, normal_delete, shared_delete;
    gum::Timer timer;
    for (long i = 0 ; i < RUN_COUNT; ++i) {
      timer.reset();
      run_normal(normal, normal_use);
      normal_delete.push_back(timer.pause() - normal.back() - normal_use.back());

      timer.reset();
      run_shared(shared, shared_use);
      shared_delete.push_back(timer.pause() - shared.back() - shared_use.back());
    }

    std::cout << "Normal pointers - avg: " << avg(normal) << ", cov: " << cov(normal) << std::endl;
    std::cout << "Shared pointers - avg: " << avg(shared) << ", cov: " << cov(shared) << std::endl;
    std::cout << "Normal use - avg: " << avg(normal_use) << ", cov: " << cov(normal_use) << std::endl;
    std::cout << "Shared use - avg: " << avg(shared_use) << ", cov: " << cov(shared_use) << std::endl;
    std::cout << "Normal delete - avg: " << avg(normal_delete) << ", cov: " << cov(normal_delete) << std::endl;
    std::cout << "Shared delete - avg: " << avg(shared_delete) << ", cov: " << cov(shared_delete) << std::endl;
  } catch ( gum::Exception& e) {
    GUM_SHOWERROR( e );
  }

