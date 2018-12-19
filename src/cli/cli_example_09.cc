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

// Basic aggregation
void Test09()
{
  cout << "*********** CLI Test09 begins ******************" << endl;

  string command;

  exec("create table employee EmpName = varchar(30), Age = int, Height = real, Salary = int");

  exec("load employee employee_5");

  exec("print employee");

  exec("SELECT AGG employee GET MAX(Height)");

  exec("SELECT AGG employee GET MIN(Salary)");

  exec("SELECT AGG (PROJECT employee GET [ * ]) GET MAX(Salary)");

  exec("SELECT AGG (PROJECT employee GET [ Salary ]) GET SUM(Salary)");

  exec("SELECT AGG (PROJECT employee GET [ Salary ]) GET COUNT(Salary)");

  exec("SELECT AGG (PROJECT employee GET [ Salary ]) GET AVG(Salary)");

  exec("SELECT AGG (PROJECT employee GET [ * ]) GET COUNT(Height)");

  exec(("drop table employee"));
}

int main()
{

  cli = CLI::Instance();

  if (MODE == 0 || MODE == 3) {
    Test09(); // Basic aggregation
  } if (MODE == 1 || MODE == 3) {
    cli->start();
  }

  return 0;
}
