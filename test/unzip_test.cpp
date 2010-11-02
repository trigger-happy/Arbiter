#include "../src/common/EasyUnzip.cpp"
#include <apr.h>
#include <iostream>
#include <fstream>
#include <iterator>

using namespace std;

int main(int argc, char** argv) {
  apr_initialize();
  easy_unzip("test.zip", "testdir2");
  /*/
  ManifestData manifest = get_manifest("test.zip");
  cout << manifest.type << endl;
  
  ofstream myfile("test2.zip");
  ostream_iterator<uint8_t> iter(myfile, "");
  copy(manifest.data.begin(), manifest.data.end(), iter);
  myfile.close();/**/
  return 0;
}