#include <bits/stdc++.h>
using namespace std;

int main(){        // Receive up to the telnet eol and possibly remove the telnet eol
  int i;
  int iState = 1;
  int cnt=0;
  while (iState != 0)
    {
      cout << "loop" << endl;
      switch (iState)
        {
          case 1:  // Figure out where to start.
            {
              cout << 1 << endl;
            }
          case 2:  // Fill the buffers with data.
            {
              cout << 2 << endl;
            }
          case 3:  // Look for the EOL sequence.
            {
              if (cnt < 5){
                iState = 1;
                ++cnt;
                break;
              }
              else iState = 4;
              cout << iState << " cnt:" << cnt << endl;
            }
          case 4:  // Cleanup and exit.
            {
              cout << 4 << endl;
              iState = 0;
              break;
            }
        } // End of switch statement.
    } // End of while loop.
    cout << "fin" << endl;
    return 0;
}
