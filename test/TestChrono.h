#ifndef TESTCHRONO
#define TESTCHRONO

class TestChrono
{
public:
  void Start()    { cstart = std::chrono::system_clock::now(); }
  void Stop()     { cstop  = std::chrono::system_clock::now(); }
  int ElapsedMs() { return std::chrono::duration_cast<std::chrono::milliseconds>
                             (cstop-cstart).count();   }
private:
  std::chrono::time_point<std::chrono::system_clock> cstart;
  std::chrono::time_point<std::chrono::system_clock> cstop;

};
#endif
