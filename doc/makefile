NAME=dokumentace

$(NAME).pdf: $(NAME).tex $(NAME).bib
	pdflatex -sPAPERSIZE=a4 $(NAME).tex
	bibtex $(NAME)
	pdflatex -sPAPERSIZE=a4 $(NAME).tex
	pdflatex -sPAPERSIZE=a4 $(NAME).tex