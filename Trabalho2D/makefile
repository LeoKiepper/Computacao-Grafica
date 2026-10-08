CC = gcc
CXX = g++
CFLAGS  = -g -Wall -MMD -MP
LINKING = -lglut -lGL -lGLU
TARGET = trabalhocg
SOURCES = main.cpp tinyxml.cpp tinyxmlparser.cpp tinyxmlerror.cpp tinystr.cpp barril.cpp atirador.cpp config.cpp funcoesaux.cpp tiro.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LINKING)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	$(RM) $(TARGET) $(OBJECTS) $(OBJECTS:.o=.d)

.PHONY: all run clean



TINYXML = tinyxml.cpp tinyxmlparser.cpp tinyxmlerror.cpp tinystr.cpp
gerador_config: gerador_config.cpp $(TINYXML)
	$(CXX) $(CFLAGS) -o $@ $^
config_gerado.h: configuracoes.xml gerador_config
	./gerador_config configuracoes.xml config_gerado.h config_campos.h
config_campos.h: config_gerado.h ;
$(OBJECTS): config_gerado.h
config.o: config_campos.h
clean: clean_config
clean_config:
	$(RM) gerador_config config_gerado.h config_campos.h



