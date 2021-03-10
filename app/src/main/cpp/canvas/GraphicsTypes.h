//
// Created by zhanfang on 2021/3/9.
//

#ifndef ANDROID_V8_DEMO_MASTER_GRAPHICSTYPES_H
#define ANDROID_V8_DEMO_MASTER_GRAPHICSTYPES_H

#include <SkColor.h>
#include <string>

namespace Engine {
    union Color {
        struct {
            unsigned r: 8;
            unsigned g: 8;
            unsigned b: 8;
            unsigned a: 8;
        } rgba;
        unsigned char components[4];
        unsigned int hex;
    };

    SkColor to_color(const std::string& colorStr);
}


#endif //ANDROID_V8_DEMO_MASTER_GRAPHICSTYPES_H
