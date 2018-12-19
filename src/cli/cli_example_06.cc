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

// Projection + Filter Test
void Test06()
{
  cout << "*********** CLI Test06 begins ******************" << endl;

  string command;

  exec("create table tbl_employee EmpName = varchar(30), Age = int, Height = real, Salary = int");

  exec("load tbl_employee employee_5");

  exec("print tbl_employee");

  exec("SELECT FILTER tbl_employee WHERE Age != 45");

  exec("SELECT PROJECT (FILTER tbl_employee WHERE Age != 45) GET [ Age ]");

  exec("SELECT PROJECT (FILTER tbl_employee WHERE Age != 45) GET [ EmpName, Age ]");

  exec("SELECT PROJECT (FILTER tbl_employee WHERE Age != 45) GET [ EmpName, Height ]");

  exec("SELECT PROJECT (FILTER tbl_employee WHERE Age != 45) GET [ * ]");

  exec("SELECT FILTER (PROJECT tbl_employee GET [ EmpName, Age ]) WHERE Age != 45");

  exec("SELECT FILTER (PROJECT tbl_employee GET [ EmpName, Age ]) WHERE Age >= 45");

  exec("SELECT PROJECT (FILTER (PROJECT tbl_employee GET [ EmpName, Age ]) WHERE Age >= 45) GET [ EmpName ]");

  exec(("drop table tbl_employee"));
}

int main()
{

  cli = CLI::Instance();

  if (MODE == 0 || MODE == 3) {
    Test06(); // Projection + Filter
  } if (MODE == 1 || MODE == 3) {
    cli->start();
  }

  return 0;
}
