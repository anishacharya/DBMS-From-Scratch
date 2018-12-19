include ../makefile.inc

all: libcli.a cli_example_01 cli_example_02 cli_example_03 cli_example_04 cli_example_05 cli_example_06 cli_example_07 cli_example_08 cli_example_09 cli_example_10 cli_example_11 cli_example_12 start

# lib file dependencies
libcli.a: libcli.a(cli.o)  # and possibly other .o files

# c file dependencies
cli.o: cli.h
cli_example_01.o: cli.h
cli_example_02.o: cli.h
cli_example_03.o: cli.h
cli_example_04.o: cli.h
cli_example_05.o: cli.h
cli_example_06.o: cli.h
cli_example_07.o: cli.h
cli_example_08.o: cli.h
cli_example_09.o: cli.h
cli_example_10.o: cli.h
cli_example_11.o: cli.h
cli_example_12.o: cli.h
start.o: cli.h

# binary dependencies
cli_example_01: cli_example_01.o libcli.a $(CODEROOT)/qe/libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
cli_example_02: cli_example_02.o libcli.a $(CODEROOT)/qe/libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
cli_example_03: cli_example_03.o libcli.a $(CODEROOT)/qe/libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
cli_example_04: cli_example_04.o libcli.a $(CODEROOT)/qe/libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
cli_example_05: cli_example_05.o libcli.a $(CODEROOT)/qe/libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
cli_example_06: cli_example_06.o libcli.a $(CODEROOT)/qe/libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
cli_example_07: cli_example_07.o libcli.a $(CODEROOT)/qe/libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
cli_example_08: cli_example_08.o libcli.a $(CODEROOT)/qe/libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
cli_example_09: cli_example_09.o libcli.a $(CODEROOT)/qe/libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
cli_example_10: cli_example_10.o libcli.a $(CODEROOT)/qe/libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
cli_example_11: cli_example_11.o libcli.a $(CODEROOT)/qe/libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
cli_example_12: cli_example_12.o libcli.a $(CODEROOT)/qe/libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a
start: start.o libcli.a $(CODEROOT)/qe/libqe.a $(CODEROOT)/ix/libix.a $(CODEROOT)/rm/librm.a $(CODEROOT)/rbf/librbf.a

$(CODEROOT)/rm/librm.a:
	$(MAKE) -C $(CODEROOT)/rm librm.a

$(CODEROOT)/rbf/librbf.a:
	$(MAKE) -C $(CODEROOT)/rbf librbf.a

# dependencies to compile used libraries
.PHONY: $(CODEROOT)/rbf/librbf.a $(CODEROOT)/rm/librm.a
$(CODEROOT)/ix/libix.a:
	$(MAKE) -C $(CODEROOT)/ix libix.a

.PHONY: $(CODEROOT)/pf/libpf.a $(CODEROOT)/rm/librm.a $(CODEROOT)/ix/libix.a
$(CODEROOT)/qe/libqe.a:
	$(MAKE) -C $(CODEROOT)/qe libqe.a

.PHONY: clean
clean:
	-rm cli_example_01 cli_example_02 cli_example_03 cli_example_04 cli_example_05 cli_example_06 cli_example_07 cli_example_08 cli_example_09 cli_example_10 cli_example_11 cli_example_12 start *.a *.o *~
	$(MAKE) -C $(CODEROOT)/rbf clean
	$(MAKE) -C $(CODEROOT)/rm clean
	$(MAKE) -C $(CODEROOT)/ix clean
	$(MAKE) -C $(CODEROOT)/qe clean
