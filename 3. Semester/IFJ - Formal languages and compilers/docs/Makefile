# Documentation makefile for IFJ17 project
# Author: Patrik Goldschmidt <xgolds00@stud.fit.vutbr.cz>
# USAGE:
# make 		 - Makes .pdf and keeps all auxilliary files created in the process
# make pdf   - Makes .pdf and removes all other auxilliary files
# make clean - Cleans .pdf and all auxilliary files

NAME=dokumentace

all:
	@pdflatex (NAME).tex > /dev/null 2>&1
	@echo "pdflatex $(NAME).tex"

clean:
	rm -rf $(NAME).aux $(NAME).log $(NAME).pdflatex

pdf:
	@pdflatex (NAME).tex > /dev/null 2>&1
	@rm -rf $(NAME).aux $(NAME).log
	@echo "pdflatex $(NAME).tex"
	
.PHONY: all clean pdf