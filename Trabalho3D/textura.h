#ifndef TEXTURA_H
#define TEXTURA_H
#include <GL/gl.h>
#include "config.h"

struct Texturas { GLuint chao, paredes, barril, tampaBarril, corpo, pernas, arma, tiro; };
extern Texturas tex;

void CarregaTexturas(const Config& cfg);
inline void LigaTextura(GLuint t){ glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, t); }
inline void DesligaTextura(){ glDisable(GL_TEXTURE_2D); }

#endif
