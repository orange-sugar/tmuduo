#include <string_view>
#include <string>
#include <iostream>
#include <map>
#include <assert.h>

using namespace std;

void byString(const string& str)
{
  cout << str;
}

int main()
{
  string s{"foooooooooooooooooooooooooooooooooooooooo"};
  // for (int i = 0; i < 10000000; ++i)
  // {
  //   byString(s);
  // }
  map<int, int> m{{1,1}};
  const auto it = m.find(0);
  assert(it == m.end());
}