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

// Group-based hash aggregation
void Test10()
{
  cout << "*********** CLI Test10 begins ******************" << endl;

  string command;

  exec("create table employee EmpName = varchar(30), Age = int, Height = real, Salary = int");

  exec("create table ages Age = int, Explanation = varchar(50)");

  exec("create table salary Salary = int, Explanation = varchar(50)");

  exec("load employee employee_5");

  exec("load ages ages_90");

  exec("load salary salary_5");

  exec("SELECT AGG ages GROUPBY(Explanation) GET AVG(Age) PARTITIONS(10)");

  exec("SELECT AGG ages GROUPBY(Explanation) GET MIN(Age) PARTITIONS(10)");

  exec("SELECT AGG (PROJECT ages GET [ Age, Explanation ]) GROUPBY(Explanation) GET MIN(Age) PARTITIONS(10)");

  exec("SELECT AGG (FILTER ages WHERE Age > 14) GROUPBY(Explanation) GET MIN(Age) PARTITIONS(10)");

  exec(("drop table employee"));

  exec(("drop table salary"));

  exec(("drop table ages"));
}

int main()
{

  cli = CLI::Instance();

  if (MODE == 0 || MODE == 3) {
    Test10(); // Group-based hash aggregation
  } if (MODE == 1 || MODE == 3) {
    cli->start();
  }

  return 0;
}
