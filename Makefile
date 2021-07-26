.SUFFIXES:
.SUFFIXES: .o .ec .cpp

#pathprj=$(shell dirname $(shell dirname `pwd`))
pathprj=${PWD}
dirAplicacao=$(pathprj)
dirBiblioteca=$(pathprj)/lib

nomeAplicacao=$(dirAplicacao)/utfToIso


GCC_VERSION_C11 := $(shell expr `g++ -dumpversion | cut -f1-2  -d.` \>= 4.8 )

ifeq ($(GCC_VERSION_C11),1)
   CC = g++ -std=c++11 -Wall -Werror -rdynamic
else
	CC = g++ -std=c++98 ${GCC_VERSION} -Wall -Werror -rdynamic
endif

SO_KERNEL_NAME	= $(shell uname)
SO_PLATAFORM	= $(shell uname -i)

AR = ar

DIR_SRC= ./src/

#-----------------------------------------------------
#Diretorios dos arquivos header (.h) do ESPECIFICO
#-----------------------------------------------------
INCLUDE_DIR	  =  -I$(DIR_SRC)


#-----------------------------------------------------
#Especifico
#-----------------------------------------------------
SRCS_CPP    = $(shell ls $(DIR_SRC)/*.cpp 2> /dev/null)
OBJS 		= $(SRCS_CPP:.cpp=.o)

ifeq ($(SO_KERNEL_NAME),Linux)
	LIB_SYS 		= -lpthread -ldl -lcrypt -lc
else
	LIB_SYS			= <<especificar para o SO>>
endif

#Link das Bibliotecas
LINK		= $(LIB_SYS)

.cpp.o:
	@echo "Compilando >>> $<"
	$(CC) -Wno-deprecated  $(INCLUDE_DIR) -c $< -o $@

all: $(OBJS)
	@echo "------------------------------------------------------------------------------"
	@[ -d $(dirAplicacao) ] || mkdir $(dirAplicacao)

	@echo "Criando autorizador <$(nomeAplicacao)>"
	@rm -f $(nomeAplicacao)
	$(CC)  $(LINK) -o $(nomeAplicacao) $?

	@chmod 770 $(nomeAplicacao)
	@echo "<<<<<< Feito >>>>>>"
	@echo "------------------------------------------------------------------------------"

local: $(OBJS)
	@echo "----------------------------------------"
	@echo "Criando autorizador <$(nomeAplicacao)>"
	@rm -f $(nomeAplicacao)
	@$(CC) $?  $(LINK) -o $(nomeAplicacao)
	@chmod 770 $(nomeAplicacao)
	@echo "<<<<<< Feito >>>>>>"
	@echo "----------------------------------------"

clean:
	@echo "-------------------------------------------------"
	@echo "Apagando Objs/dependencias de <$(nomeAplicacao)>"
	rm -f $(OBJS:.o=.d) $(OBJS) core* $(nomeAplicacao)
	@echo "<<<<<< Feito >>>>>>"
	@echo "-------------------------------------------------"

-include $(OBJS:.o=.d)
