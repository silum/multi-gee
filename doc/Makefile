# $Id: Makefile,v 1.2 2002/11/15 17:45:27 dsm Exp $

## don't use builtin implicit rules

MAKEFLAGS = -r


## define custom suffixes

.SUFFIXES:
.SUFFIXES: .tex .dvi .ps


## LaTeX

LATEX = latex


## default target

DOC = \
    api.ps


.PHONY: all
all: $(DOC)


## dependencies

#file.ps: figure-1.eps


## miscellaneous targets

.PHONY: spell
spell:
	@aspell -c -t --lang=en_UK *.tex

.PHONY: clean
clean:
	@rm -f *~ *.log

.PHONY: squeaky
squeaky: clean
	@rm -f *.aui *.aux

.PHONY: clobber
clobber: squeaky
	@rm -f *.dvi *.bak *.toc

.PHONY: distclean
distclean: clobber
	@rm -f $(DOC)

.PHONY: realclean
realclean: distclean
	@


## implicit rules

%.ps: %.dvi
	dvips $< -o

%.dvi: %.tex
	$(LATEX) $< && $(LATEX) $<


## don't try to make the makefile..

Makefile: ;


## -fin-
# vim:set syntax=make:
