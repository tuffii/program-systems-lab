OUTDIR = build

all: $(OUTDIR)/komppl.exe $(OUTDIR)/kompassr.exe $(OUTDIR)/absloadm.exe

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(OUTDIR)/komppl.exe: komppl.c | $(OUTDIR)
	gcc -m32 -o $(OUTDIR)/komppl.exe komppl.c

$(OUTDIR)/kompassr.exe: kompassr.c | $(OUTDIR)
	gcc -m32 -o $(OUTDIR)/kompassr.exe kompassr.c

$(OUTDIR)/absloadm.exe: absloadm.c | $(OUTDIR)
	gcc -m32 -o $(OUTDIR)/absloadm.exe absloadm.c -lncurses

clean:
	rm -rf $(OUTDIR)