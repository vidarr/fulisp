TEX_FILES=main.tex

MAIN_TEX_FILE=main.tex

PDFLATEX=pdflatex


pdf: ${TEX_FILES}
	${PDFLATEX} ${MAIN_TEX_FILE}

clean:
	@echo "REMOVING BUILT DOCUMENTATION..." && \
    rm -f *.aux && rm -f *.log && rm -f *.pdf

