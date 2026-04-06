OUTDIR = build
SRCDIR = src

all: $(OUTDIR)/komppl.exe

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(OUTDIR)/komppl.exe: $(SRCDIR)/komppl.c | $(OUTDIR)
	gcc -m32 -w -o $(OUTDIR)/komppl.exe $(SRCDIR)/komppl.c

clean:
	rm -rf $(OUTDIR)
