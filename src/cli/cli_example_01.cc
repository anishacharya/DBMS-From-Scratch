#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cassert>

#include "cli.h"

using namespace std;

#define SUCCESS 0
#define MODE 0  // 0 = TEST MODE
                // 1 = INTERACTIVE MODE
                // 3 = TEST + INTERACTIVE MODE

CLI *cli;

void exec(string command, bool equal = true){
  cout << ">>> " << command << endl;

  if( equal )
    assert (cli->process(command) == SUCCESS);
  else
    assert (cli->process(command) != SUCCESS);
}

// test create table
// test drop table
void Test01()
{

  cout << "*********** CLI Test01 begins ******************" << endl;

  string command;

  exec("create catalog");

  exec("create table ekin name = varchar(40), age = int");

  exec("print cli_columns");

  exec("print cli_tables");

  exec("drop table ekin");

  exec("print cli_tables");

  exec("print cli_columns");

  cout << "We should not see anything related to ekin table" << endl;
}



int main()
{

  cli = CLI::Instance();

  if (MODE == 0 || MODE == 3) {
    Test01();
  } if (MODE == 1 || MODE == 3) {
    cli->start();
  }

  return 0;
}
