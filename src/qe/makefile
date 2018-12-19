
include ../makefile.inc

all: libqe.a qetest_01 qetest_02 qetest_03 qetest_03b qetest_03c qetest_04 qetest_05 qetest_05b qetest_06 qetest_06b qetest_07 qetest_07b qetest_08 qetest_08b qetest_09 qetest_09b

# lib file dependencies
libqe.a: libqe.a(qe.o)  # and possibly other .o files

# c file dependencies
qe.o: qe.h

qetest.o: qe.h

# binary dependencies
qetest_01: qetest_01.o libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
qetest_02: qetest_02.o libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
qetest_03: qetest_03.o libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
qetest_03b: qetest_03b.o libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
qetest_03c: qetest_03c.o libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
qetest_04: qetest_04.o libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
qetest_05: qetest_05.o libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
qetest_05b: qetest_05b.o libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
qetest_06: qetest_06.o libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
qetest_06b: qetest_06b.o libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
qetest_07: qetest_07.o libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
qetest_07b: qetest_07b.o libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
qetest_08: qetest_08.o libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
qetest_08b: qetest_08b.o libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
qetest_09: qetest_09.o libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
qetest_09b: qetest_09b.o libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a

# dependencies to compile used libraries
.PHONY: $(CODEROOT)/rbf/librbf.a
$(CODEROOT)/rbf/librbf.a:
	$(MAKE) -C $(CODEROOT)/rbf librbf.a

.PHONY: $(CODEROOT)/rm/librm.a
$(CODEROOT)/rm/librm.a:
	$(MAKE) -C $(CODEROOT)/rm librm.a

.PHONY: $(CODEROOT)/ix/libix.a
$(CODEROOT)/ix/libix.a:
	$(MAKE) -C $(CODEROOT)/ix libix.a

.PHONY: clean
clean:
	-rm qetest_01 qetest_02 qetest_03 qetest_03b qetest_03c qetest_04 qetest_05 qetest_05b qetest_06 qetest_06b qetest_07 qetest_07b qetest_08 qetest_08b qetest_09 qetest_09b *.a *.o *~
	$(MAKE) -C $(CODEROOT)/rm clean
	$(MAKE) -C $(CODEROOT)/ix clean 
