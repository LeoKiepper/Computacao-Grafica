#include "textura.h"
#include "imageloader.h"
#include <GL/glu.h>

Texturas tex;

static GLuint CarregaTextura(const std::string& arquivo){
    if (FILE* f = fopen(arquivo.c_str(), "rb")) fclose(f);
    else { fprintf(stderr, "textura: nao encontrei %s\n", arquivo.c_str()); exit(1); }
    Image* img = loadBMP(arquivo.c_str());
    GLuint t;
    glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, img->width, img->height,
                      GL_RGB, GL_UNSIGNED_BYTE, img->pixels);
    delete img;
    return t;
}
void CarregaTexturas(const Config& cfg){
    const auto& a = cfg.textura.arquivo;
    tex.chao        = CarregaTextura(a.chao);
    tex.paredes     = CarregaTextura(a.paredes);
    tex.barril      = CarregaTextura(a.barril);
    tex.tampaBarril = CarregaTextura(a.tampaBarril);
    tex.corpo       = CarregaTextura(a.corpo);
    tex.pernas      = CarregaTextura(a.pernas);
    tex.arma        = CarregaTextura(a.arma);
    tex.tiro        = CarregaTextura(a.tiro);
}
