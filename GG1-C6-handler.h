/**
 * @file GG1-C6-handler.h
 * @author Eron Ristich (eron@ristich.com)
 * @brief Handler class for GG1-C6, 
 * @version 0.1
 * @date 2022-08-06
 */

#ifndef GG1_C6_HANDLER_H
#define GG1_C6_HANDLER_H

#include <chrono>

#include "util/handler.h"
#include "objects/helper.h"
#include "objects/camera.h""

class GG1_C6_Handler : public Handler {
    public:
        GG1_C6_Handler();
        ~GG1_C6_Handler();

        void objEventHandler() override;
        void objRendererHandler() override;
        void objUpdateHandler() override;
        void objPreLoopStep() override;

    private:
        int frame = 0;
        float dt = 0.0f;
        int curFPS = 0;
        std::chrono::_V2::steady_clock::time_point lastT;

        int relX, relY;
        bool wDown, aDown, sDown, dDown, spDown, shDown, enDown;
        Camera* camera;

        // scene objects
        
};

#endif


