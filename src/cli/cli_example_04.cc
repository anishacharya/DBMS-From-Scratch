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

// Projection Test
void Test04()
{
  cout << "*********** CLI Test04 begins ******************" << endl;

  string command;

  exec("create table tbl_employee EmpName = varchar(30), Age = int, Height = real, Salary = int");

  exec("load tbl_employee employee_5");

  exec("print tbl_employee");

  exec("SELECT PROJECT tbl_employee GET [ * ]");

  exec("SELECT PROJECT (PROJECT tbl_employee GET [ * ]) GET [ EmpName ]");

  exec("SELECT PROJECT (PROJECT tbl_employee GET [ EmpName, Age ]) GET [ Age ]");

  exec("SELECT PROJECT (PROJECT (PROJECT tbl_employee GET [ * ]) GET [ EmpName, Age ]) GET [ Age ]");

  exec("SELECT PROJECT tbl_employee GET [ EmpName, Age ]");

  exec(("drop table tbl_employee"));

}

int main()
{

  cli = CLI::Instance();

  if (MODE == 0 || MODE == 3) {
    Test04(); // Projection
  } if (MODE == 1 || MODE == 3) {
    cli->start();
  }

  return 0;
}
