OUTDIR = build
SRCDIR = src

all: $(OUTDIR)/komppl.exe $(OUTDIR)/kompassr.exe $(OUTDIR)/absloadm.exe

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(OUTDIR)/komppl.exe: $(SRCDIR)/komppl.c | $(OUTDIR)
	gcc -m32 -o $(OUTDIR)/komppl.exe $(SRCDIR)/komppl.c

$(OUTDIR)/kompassr.exe: $(SRCDIR)/kompassr.c | $(OUTDIR)
	gcc -m32 -o $(OUTDIR)/kompassr.exe $(SRCDIR)/kompassr.c

$(OUTDIR)/absloadm.exe: $(SRCDIR)/absloadm.c | $(OUTDIR)
	gcc -m32 -o $(OUTDIR)/absloadm.exe $(SRCDIR)/absloadm.c -lncurses

clean:
	rm -rf $(OUTDIR)
