//
// Created by bhiaibogf on 2022/1/27.
//

#ifndef SKETCHFAB_CONFIG_H
#define SKETCHFAB_CONFIG_H

enum class Type {
    kRenderer, kPrt, kLBuffer, kGBuffer
};

enum class Prt {
    kSkybox, kIrradiance, kPrefilter, kLut
};

enum class LBuf {
    kDir, kPoint, kDirPos, kPointPos
};

enum class GBuf {
    kPos, kNorm, kAlb, kId, kAo, kDepth
};

struct Config {
    Type type = Type::kRenderer;

    // renderer
    bool ambient = true;
    bool lo = true;
    bool ao = true;
    bool ao_map = true;
    bool ibl = true;

    Prt prt = Prt::kSkybox;

    LBuf l_buf = LBuf::kDir;
    int light = 0;

    GBuf g_buf = GBuf::kPos;

};


#endif //SKETCHFAB_CONFIG_H
