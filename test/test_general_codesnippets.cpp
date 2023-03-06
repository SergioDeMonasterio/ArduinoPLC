#include <stdio.h>
#include <string.h>

struct actuatorPinsAndTimes
{
  bool actActive;
  short boardPin;
  unsigned int startActTime;
  unsigned int stopActTime;
};

// main function
int main()
{
  const short qty = 4;
  actuatorPinsAndTimes var1[qty] = {{true, 12, 100, 200},
                                    {true, 13, 200, 300},
                                    {true, 14, 200, 300},
                                    {true, 15, 200, 300}};
  actuatorPinsAndTimes var2[qty];
  // --> 1st Option <--
  // for (short i = 0; i < qty; ++i)
  //   var2[i] = var1[i];

  // --> 2nd Option <--
  memcpy(&var2, &var1, sizeof(var1));

  for (short i = 0; i < qty; ++i)
  {
    printf("Cylinder status: %d \n", var2[i].actActive);
    printf("Cylinder status: %d \n", var2[i].boardPin);
    printf("Cylinder status: %d \n", var2[i].startActTime);
    printf("Cylinder status: %d \n", var2[i].stopActTime);
  }

  return 0;
}