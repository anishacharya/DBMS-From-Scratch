#include "test_util.h"

int main()
{

  // By executing this script, the following tables including the system tables will be removed and constructed again.

  // Before executing rmtest_xx, you need to make sure that this script work properly.
  cout << endl << "***** RM TEST - Creating the Catalog and user tables *****" << endl;

  // Try to delete the System Catalog.
  // If this is the first time, it will generate an error. It's OK and we will ignore that.
  RC rc = rm->deleteCatalog();

  rc = rm->createCatalog();
  assert (rc == success && "Creating the Catalog should not fail.");

  // Delete the actual file and create Table tbl_employee
  remove("tbl_employee");

  rc = createTable("tbl_employee");
  assert (rc == success && "Creating a table should not fail.");

  // Delete the actual file and create Table tbl_employee
  remove("tbl_employee2");

  rc = createTable("tbl_employee2");
  assert (rc == success && "Creating a table should not fail.");

  // Delete the actual file and create Table tbl_employee
  remove("tbl_employee3");

  rc = createTable("tbl_employee3");
  assert (rc == success && "Creating a table should not fail.");

  // Delete the actual file and create Table tbl_employee
  remove("tbl_employee4");

  rc = createLargeTable("tbl_employee4");
  assert (rc == success && "Creating a table should not fail.");

  return success;
}
