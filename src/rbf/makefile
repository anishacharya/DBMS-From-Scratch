include ../makefile.inc

all: librbf.a rbftest1 rbftest2 rbftest3 rbftest4 rbftest5 rbftest6 rbftest7 rbftest8 rbftest8b rbftest9 rbftest10 rbftest11 rbftest12 rbftest_extra_1 rbftest_private_1 rbftest_private_1b rbftest_private_2 rbftest_private_3 rbftest_private_4

# c file dependencies
pfm.o: pfm.h
rbfm.o: rbfm.h

# lib file dependencies
librbf.a: librbf.a(pfm.o)  # and possibly other .o files
librbf.a: librbf.a(rbfm.o)

rbftest1.o: pfm.h rbfm.h
rbftest2.o: pfm.h rbfm.h
rbftest3.o: pfm.h rbfm.h
rbftest4.o: pfm.h rbfm.h
rbftest5.o: pfm.h rbfm.h
rbftest6.o: pfm.h rbfm.h
rbftest7.o: pfm.h rbfm.h
rbftest8.o: pfm.h rbfm.h
rbftest8b.o: pfm.h rbfm.h
rbftest9.o: pfm.h rbfm.h
rbftest10.o: pfm.h rbfm.h
rbftest11.o: pfm.h rbfm.h
rbftest12.o: pfm.h rbfm.h
rbftest_extra_1.o: pfm.h rbfm.h
rbftest_private_1.o: pfm.h rbfm.h
rbftest_private_1b.o: pfm.h rbfm.h
rbftest_private_2.o: pfm.h rbfm.h
rbftest_private_3.o: pfm.h rbfm.h
rbftest_private_4.o: pfm.h rbfm.h

# binary dependencies
rbftest1: rbftest1.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest2: rbftest2.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest3: rbftest3.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest4: rbftest4.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest5: rbftest5.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest6: rbftest6.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest7: rbftest7.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest8: rbftest8.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest8b: rbftest8b.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest9: rbftest9.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest10: rbftest10.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest11: rbftest11.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest12: rbftest12.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest_extra_1: rbftest_extra_1.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest_private_1: rbftest_private_1.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest_private_1b: rbftest_private_1b.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest_private_2: rbftest_private_2.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest_private_3: rbftest_private_3.o librbf.a $(CODEROOT)/rbf/librbf.a
rbftest_private_4: rbftest_private_4.o librbf.a $(CODEROOT)/rbf/librbf.a

# dependencies to compile used libraries
.PHONY: $(CODEROOT)/rbf/librbf.a
$(CODEROOT)/rbf/librbf.a:
	$(MAKE) -C $(CODEROOT)/rbf librbf.a

.PHONY: clean
clean:
	-rm rbftest1 rbftest2 rbftest3 rbftest4 rbftest5 rbftest6 rbftest7 rbftest8 rbftest8b rbftest9 rbftest10 rbftest11 rbftest12 rbftest_extra_1 rbftest_private_1 rbftest_private_1b rbftest_private_2 rbftest_private_3 rbftest_private_4 test1 test3 test7 test8 test8b test9 test9rids test9sizes test11 test11rids test_extra_1a test_extra_1b test_private_1 test_private_1b test_private_2 test_private_3a test_private_3b test_private_3a_rids test_private_3b_rids   *.a *.o *~
