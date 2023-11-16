PREFIX ?= /usr/
DESTDIR=
BIN=$(DESTDIR)$(PREFIX)/bin/
LIB=$(DESTDIR)$(PREFIX)/lib/

all: build run

build:
	@echo -n "Building: "
	g++ ./src/diratsh.cpp -I ./src -o diratsh -Ofast > /dev/null

run:
	@echo "Starting..."
	@./diratsh

clean:
	@echo "Cleaning"
	@rm -rfv ./diratsh 2> /dev/null

install: diratsh
	@echo "Installing"
	@printf "\t"
	cp ./diratsh $(BIN)
	@printf "\techo "$(BIN)diratsh" >> /etc/shells;\n"
	#@if ! grep -Fxq "$(BIN)diratsh" /etc/shells; then \
	#	echo "$(BIN)diratsh" >> /etc/shells; \
	#fi
	@grep -qe '^$(BIN)diratsh$' etc/shells || echo '$(BIN)diratsh' >> etc/shells

package: diratsh
	mkdir -p $(BIN)
	cp ./diratsh $(BIN)
