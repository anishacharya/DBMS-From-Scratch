#include "test_util.h"

int main()
{

  // By executing this script, the following tables including the system tables will be removed.
  cout << endl << "***** RM TEST - Deleting the Catalog and User tables *****" << endl;

  RC rc = rm->deleteTable("tbl_employee");

  rc = rm->deleteTable("tbl_employee2");

  rc = rm->deleteTable("tbl_employee3");

  rc = rm->deleteTable("tbl_employee4");

  rc = rm->deleteCatalog();

  return success;
}
