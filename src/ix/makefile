
include ../makefile.inc

all: libix.a ixtest1 ixtest2 ixtest3 ixtest4a ixtest4b ixtest5a ixtest5b ixtest6a ixtest6b ixtest7 ixtest8 ixtest9 ixtest_extra_1 ixtest_extra_2 ixtest_extra_3

# lib file dependencies
libix.a: libix.a(ix.o)  # and possibly other .o files

# c file dependencies
ix.o: ix.h

ixtest_util.o: ixtest_util.h
ixtest1.o: ixtest_util.h
ixtest2.o: ixtest_util.h
ixtest3.o: ixtest_util.h
ixtest4a.o: ixtest_util.h
ixtest4b.o: ixtest_util.h
ixtest5a.o: ixtest_util.h
ixtest5b.o: ixtest_util.h
ixtest6a.o: ixtest_util.h
ixtest6b.o: ixtest_util.h
ixtest7.o: ixtest_util.h
ixtest8.o: ixtest_util.h
ixtest9.o: ixtest_util.h
ixtest_extra_1.o: ixtest_util.h
ixtest_extra_2.o: ixtest_util.h
ixtest_extra_3.o: ixtest_util.h

# binary dependencies
ixtest1: ixtest1.o ixtest_util.o libix.a $(CODEROOT)/rbf/librbf.a 
ixtest2: ixtest2.o ixtest_util.o libix.a $(CODEROOT)/rbf/librbf.a 
ixtest3: ixtest3.o ixtest_util.o libix.a $(CODEROOT)/rbf/librbf.a 
ixtest4a: ixtest4a.o ixtest_util.o libix.a $(CODEROOT)/rbf/librbf.a 
ixtest4b: ixtest4b.o ixtest_util.o libix.a $(CODEROOT)/rbf/librbf.a 
ixtest5a: ixtest5a.o ixtest_util.o libix.a $(CODEROOT)/rbf/librbf.a 
ixtest5b: ixtest5b.o ixtest_util.o libix.a $(CODEROOT)/rbf/librbf.a 
ixtest6a: ixtest6a.o ixtest_util.o libix.a $(CODEROOT)/rbf/librbf.a 
ixtest6b: ixtest6b.o ixtest_util.o libix.a $(CODEROOT)/rbf/librbf.a 
ixtest7: ixtest7.o ixtest_util.o libix.a $(CODEROOT)/rbf/librbf.a 
ixtest8: ixtest8.o ixtest_util.o libix.a $(CODEROOT)/rbf/librbf.a 
ixtest9: ixtest9.o ixtest_util.o libix.a $(CODEROOT)/rbf/librbf.a 
ixtest_extra_1: ixtest_extra_1.o ixtest_util.o libix.a $(CODEROOT)/rbf/librbf.a 
ixtest_extra_2: ixtest_extra_2.o ixtest_util.o libix.a $(CODEROOT)/rbf/librbf.a 
ixtest_extra_3: ixtest_extra_3.o ixtest_util.o libix.a $(CODEROOT)/rbf/librbf.a 

# dependencies to compile used libraries
.PHONY: $(CODEROOT)/rbf/librbf.a
$(CODEROOT)/rbf/librbf.a:
	$(MAKE) -C $(CODEROOT)/rbf librbf.a

.PHONY: clean
clean:
	-rm *.o *.a ixtest1 ixtest2 ixtest3 ixtest4a ixtest4b ixtest5a ixtest5b ixtest6a ixtest6b ixtest7 ixtest8 ixtest9 ixtest_extra_1 ixtest_extra_2 ixtest_extra_3 
	$(MAKE) -C $(CODEROOT)/rbf clean
