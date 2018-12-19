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

// Filter Test
void Test05()
{
  cout << "*********** CLI Test05 begins ******************" << endl;

  string command;

  exec("create table tbl_employee EmpName = varchar(30), Age = int, Height = real, Salary = int");

  exec("load tbl_employee employee_5");

  exec("print tbl_employee");

  exec("SELECT FILTER tbl_employee WHERE Age = 45");

  exec("SELECT FILTER tbl_employee WHERE Age < 45");

  exec("SELECT FILTER tbl_employee WHERE Age > 45");

  exec("SELECT FILTER tbl_employee WHERE Age <= 45");

  exec("SELECT FILTER tbl_employee WHERE Age >= 45");

  exec("SELECT FILTER tbl_employee WHERE Age != 45");

  exec("SELECT FILTER tbl_employee WHERE Height < 6.3");

  exec("SELECT FILTER tbl_employee WHERE EmpName < L");

  exec("SELECT FILTER (FILTER tbl_employee WHERE  Age < 67) WHERE EmpName < L");

  exec("SELECT FILTER (FILTER tbl_employee WHERE  Age <= 67) WHERE Height >= 6.4");

  exec("SELECT FILTER (FILTER (FILTER tbl_employee WHERE EmpName > Ap) WHERE  Age <= 67) WHERE Height >= 6.4");

  exec(("drop table tbl_employee"));
}

int main()
{

  cli = CLI::Instance();

  if (MODE == 0 || MODE == 3) {
    Test05(); // Filter
  } if (MODE == 1 || MODE == 3) {
    cli->start();
  }

  return 0;
}
