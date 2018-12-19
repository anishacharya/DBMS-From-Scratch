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

// Grace Hash Join
void Test07()
{
  cout << "*********** CLI Test07 begins ******************" << endl;

  string command;

  exec("create table employee EmpName = varchar(30), Age = int, Height = real, Salary = int");

  exec("create table ages Age = int, Explanation = varchar(50)");

  exec("create table salary Salary = int, Explanation = varchar(50)");

  exec("load employee employee_5");

  exec("load ages ages_90");

  exec("load salary salary_5");

  exec("SELECT GHJOIN employee, ages WHERE Age = Age PARTITIONS(10)");

  exec("SELECT GHJOIN (GHJOIN employee, salary WHERE Salary = Salary PARTITIONS(10)), ages WHERE Age = Age PARTITIONS(10)");

  exec("SELECT GHJOIN (GHJOIN (GHJOIN employee, employee WHERE EmpName = EmpName PARTITIONS(10)), salary) WHERE Salary = Salary PARTITIONS(10)), ages WHERE Age = Age PARTITIONS(10)");

  exec(("drop table employee"));

  exec(("drop table ages"));

  exec(("drop table salary"));
}

int main()
{

  cli = CLI::Instance();

  if (MODE == 0 || MODE == 3) {
    Test07(); // GHJoin
  } if (MODE == 1 || MODE == 3) {
    cli->start();
  }

  return 0;
}
