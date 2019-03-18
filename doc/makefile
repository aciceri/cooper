THESIS=elaborato
FLAGS = -shell-escape

.SUFFIXES:
.SUFFIXES: .bib .pdf .tex
.PHONY: clean

run: $(THESIS).pdf

$(THESIS).pdf: $(THESIS).bbl $(THESIS).tex
	pdflatex $(FLAGS) $(THESIS).tex -draftmode
	pythontex $(THESIS)
	pdflatex $(FLAGS) $(THESIS).tex

$(THESIS).bbl: $(THESIS).aux
	bibtex $(THESIS).aux

$(THESIS).aux: $(THESIS).bib
	pdflatex $(FLAGS) $(THESIS).tex -draftmode
	pdflatex $(FLAGS) $(THESIS).tex -draftmode

clean:
	rm -rf *.xml *.aux *.lof *.log *.lot *.toc *.bbl *.blg $(THESIS)-blx.bib $(THESIS).pytxcode latex.py pythontex-files-elaborato
