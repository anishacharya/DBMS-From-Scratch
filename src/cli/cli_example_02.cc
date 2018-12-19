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
// test load table
void Test02()
{

  cout << "*********** CLI Test02 begins ******************" << endl;

  string command;

  exec("create table tbl_employee EmpName = varchar(30), Age = int, Height = real, Salary = int");

  cout << "Before loading file: " << endl;
  exec("print tbl_employee");

  exec("load tbl_employee employee_50");

  cout << "After loading file: " << endl;
  exec("print tbl_employee");

  exec(("drop table tbl_employee"));
}

int main()
{

  cli = CLI::Instance();

  if (MODE == 0 || MODE == 3) {
    Test02();
  } if (MODE == 1 || MODE == 3) {
    cli->start();
  }

  return 0;
}
