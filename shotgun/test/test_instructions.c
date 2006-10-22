#include "rubinius.h"
#include <assert.h>
#include "cpu.h"

#define rassert(e) ({ assert(e); printf("%s:%d: <%s> => success\n", __FUNCTION__, __LINE__, #e); })

int main(int argc, char **argv) {
  STATE;
  
  state = rubinius_state_new();
  
  return 0;
}