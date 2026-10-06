#include "tinyxml.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>

static const char* tipo(const char* valor)
{
    char* fim;
    strtod(valor, &fim);
    if (*valor && !*fim) return "double";
    std::string s = valor;
    if (s == "true" || s == "false") return "bool";
    return "std::string";
}

static void corpo(FILE* out, TiXmlNode* no, const std::string& ind)
{
    if (TiXmlElement* e = no->ToElement())
        for (TiXmlAttribute* a = e->FirstAttribute(); a; a = a->Next())
            fprintf(out, "%s%s %s{};\n", ind.c_str(), tipo(a->Value()), a->Name());
    for (TiXmlElement* c = no->FirstChildElement(); c; c = c->NextSiblingElement()) {
        fprintf(out, "%sstruct %s_t {\n", ind.c_str(), c->Value());
        corpo(out, c, ind + "    ");
        fprintf(out, "%s} %s;\n", ind.c_str(), c->Value());
    }
}

static void funcoes(FILE* out, TiXmlNode* no, const std::string& tipoStruct)
{
    std::string chamadas;
    if (TiXmlElement* e = no->ToElement())
        for (TiXmlAttribute* a = e->FirstAttribute(); a; a = a->Next())
            chamadas += std::string(" v(\"") + a->Name() + "\", s." + a->Name() + ");";
    for (TiXmlElement* c = no->FirstChildElement(); c; c = c->NextSiblingElement())
        chamadas += std::string(" v(\"") + c->Value() + "\", s." + c->Value() + ");";
    fprintf(out, "template <class V> void campos(%s& s, V& v) {%s }\n",
            tipoStruct.c_str(), chamadas.c_str());

    for (TiXmlElement* c = no->FirstChildElement(); c; c = c->NextSiblingElement())
        funcoes(out, c, tipoStruct + "::" + c->Value() + "_t");
}

int main(int argc, char** argv)
{
    if (argc != 4) {
        fprintf(stderr, "uso: %s config.xml config_gerado.h config_campos.h\n", argv[0]);
        return 2;
    }
    TiXmlDocument doc(argv[1]);
    if (!doc.LoadFile()) {
        fprintf(stderr, "gerador_config: %s:%d:%d: %s\n", argv[1], doc.ErrorRow(), doc.ErrorCol(), doc.ErrorDesc());
        return 1;
    }

    FILE* structs = fopen(argv[2], "w");
    fprintf(structs, "// GERADO a partir de %s por gerador_config. Nao edite.\n", argv[1]);
    fprintf(structs, "#ifndef CONFIG_GERADO_H\n#define CONFIG_GERADO_H\n#include <string>\n\n");
    fprintf(structs, "struct Config {\n");
    corpo(structs, &doc, "    ");   // the file's top-level elements are Config's fields
    fprintf(structs, "};\n\n#endif\n");
    fclose(structs);

    FILE* campos = fopen(argv[3], "w");
    fprintf(campos, "// GERADO a partir de %s por gerador_config. Nao edite.\n", argv[1]);
    fprintf(campos, "// Usado apenas por config.cpp para preencher Config.\n");
    fprintf(campos, "#ifndef CONFIG_CAMPOS_H\n#define CONFIG_CAMPOS_H\n#include \"%s\"\n\n", argv[2]);
    funcoes(campos, &doc, "Config");
    fprintf(campos, "\n#endif\n");
    fclose(campos);
    return 0;
}
