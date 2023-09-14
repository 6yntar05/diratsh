all: build run

build:
	@echo -n "Building: "
	g++ diratsh.cpp -o diratsh -Ofast > /dev/null

run:
	@echo "Starting..."
	@./diratsh

clean:
	@echo "Cleaning"
	@rm -rfv ./diratsh 2> /dev/null

install: diratsh
	@echo "Installing"
	@printf "\t"
	cp ./diratsh /usr/local/bin
	@printf "\techo "/usr/local/bin/diratsh" >> /etc/shells;\n"
	@if ! grep -Fxq "/usr/local/bin/diratsh" /etc/shells; then \
		echo "/usr/local/bin/diratsh" >> /etc/shells; \
	fi

