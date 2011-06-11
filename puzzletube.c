#include "game.h"

int main(int argc, char **argv)
{
  srand(time(NULL));
  initEngine();
  prepare_game_objects(1,6);
  run_game(1,timeMode,0);
  delete_game_objects();
  quitEngine();
  return 0;
}
