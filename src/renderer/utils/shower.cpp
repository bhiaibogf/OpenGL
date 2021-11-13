//
// Created by bhiaibogf on 2021/11/8.
//

#include "shower.h"

void Shower::Show(unsigned int map) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, map);
    quad_.Draw();
}
