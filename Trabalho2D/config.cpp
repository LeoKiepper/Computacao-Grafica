#include "config.h"
#include "config_campos.h"
#include "tinyxml.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>

struct Leitor {
    TiXmlNode* no;
    std::string caminho;  
    
    const char* atributo(const char* nome)
    {
        TiXmlElement* e = no->ToElement();
        const char* s = e ? e->Attribute(nome) : NULL;
        if (!s) fprintf(stderr, "config: %s.%s ausente\n", caminho.c_str(), nome);
        return s;
    }

    void operator()(const char* nome, double& x)
    {
        const char* s = atributo(nome);
        if (!s) return;
        char* fim;
        double d = strtod(s, &fim);
        if (*s && !*fim) x = d;
        else fprintf(stderr, "config: %s.%s = \"%s\" nao e numero\n", caminho.c_str(), nome, s);
    }

    void operator()(const char* nome, bool& x)
    {
        if (const char* s = atributo(nome)) x = std::string(s) == "true";
    }

    void operator()(const char* nome, std::string& x)
    {
        if (const char* s = atributo(nome)) x = s;
    }

    template <class T>
    void operator()(const char* nome, T& filho)
    {
        Leitor sub = { no->FirstChildElement(nome), caminho.empty() ? nome : caminho + "." + nome };
        if (sub.no) campos(filho, sub);
        else fprintf(stderr, "config: %s ausente\n", sub.caminho.c_str());
    }
};

Config lerConfiguracoes(const char* caminho)
{
    TiXmlDocument doc(caminho);
    if (!doc.LoadFile()) {
        fprintf(stderr, "Erro ao ler %s:%d:%d: %s\n", caminho, doc.ErrorRow(), doc.ErrorCol(), doc.ErrorDesc());
        exit(1);
    }
    Config cfg;
    Leitor leitor = { &doc, "" };
    campos(cfg, leitor);
    return cfg;
}
