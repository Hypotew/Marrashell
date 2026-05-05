FROM epitechcontent/epitest-docker:latest AS builder
WORKDIR /app/make
COPY . .

CMD make CC=gcc && ls -l 42sh && make clean && make fclean && make CC=gcc re && ls -l 42sh
