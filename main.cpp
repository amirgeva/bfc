#include <conio.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <stack>

std::vector<uint8_t> ribbon;
size_t pos = 0;

struct Loop
{
  Loop(int64_t from=0, int64_t to=-1)
    : start(from), stop(to) {}
  int64_t start, stop;

  bool done() const { return ribbon[start] == 0; }
};

std::unordered_map<int64_t, Loop> loops_map;

void resize_ribbon()
{
  if (pos >= ribbon.size()) ribbon.resize(pos + 1);
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: bfc <program>\n";
        return 1;
    }
    std::vector<char> code;
    {
        std::ifstream f(argv[1], std::ios::in | std::ios::binary);
        f.seekg(0, std::ios::end);
        size_t n = f.tellg();
        code.resize(n+1);
        f.seekg(0);
        f.read(&code[0], n);
        code[n] = 0;
    }
  int64_t cur = 0;
  std::stack<Loop> loops;
  for (const char* ptr = &code[0]; *ptr; ++ptr, ++cur)
  {
    if (*ptr == '[') loops.push(Loop(cur + 1));
    if (*ptr == ']')
    {
      if (loops.empty()) throw std::runtime_error("No loop to close");
      if (loops.top().stop >= 0) throw std::runtime_error("Too many loop close commands");
      Loop cur_loop = loops.top();
      loops.pop();
      cur_loop.stop = cur;
      loops_map[cur_loop.start] = cur_loop;
    }
  }
  if (!loops.empty()) throw std::runtime_error("Missing loop close");
  int64_t n = cur;
  for(cur=0;cur<n;++cur)
  {
    resize_ribbon();
    switch (code[cur])
    {
    case '<': { if (pos > 0) --pos; break; }
    case '>': ++pos; break;
    case '[':
    {
      const Loop& l = loops_map.find(cur + 1)->second;
      if (!ribbon[pos]) cur = l.stop;
      else
        loops.push(l);
      break;
    }
    case ']':
    {
      auto& l = loops.top();
      if (!ribbon[pos])
        loops.pop();
      else
        cur = l.start - 1;
      break;
    }
    case '+': ribbon[pos]++; break;
    case '-': ribbon[pos]--; break;
    case ',': ribbon[pos] = _getch(); std::cout << ribbon[pos]; break;
    case '.': std::cout << ribbon[pos]; break;
    default: break;
    }
  }
  return 0;
}
