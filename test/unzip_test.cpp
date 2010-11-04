#include "../src/common/EasyUnzip.cpp"
#include <apr.h>
#include <iostream>
#include <fstream>
#include <iterator>

using namespace std;

int main(int argc, char** argv) {
  apr_initialize();
  easy_unzip("test/test.zip", "test/unzip_test");
  
  ManifestData manifest = get_manifest("test/test.zip");
  cout << manifest.type << endl;
  ostream_iterator<uint8_t> iter(cout, "");
  copy(manifest.data.begin(), manifest.data.end(), iter);
  return 0;
}