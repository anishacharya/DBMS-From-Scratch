
- Modify the "CODEROOT" variable in makefile.inc to point to the root
  of your code base

- Implement the Record-based Files (RBF) Component:

   Go to folder "rbf" and type in:

    make clean
    make
    ./rbftest1

   The program should work.  But it does nothing.  You are supposed to
   implement the API of the paged file manager defined in pfm.h and some
   of the methods in rbfm.h as explained in the project description.

- By default you should not change those functions of the PagedFileManager,
  FileHandle, and RecordBasedFileManager classes defined in rbf/pfm.h and rbf/rbfm.h.
  If you think some changes are really necessary, please contact us first.



Please Note- 
You need to run 
chmod u+x test.sh

before ./test.sh project1-StudentID

