/**
 * @file GG1-C6-handler.cpp
 * @author Eron Ristich (eron@ristich.com)
 * @brief Handler class for GG1-C6, 
 * @version 0.1
 * @date 2022-08-06
 */

#include "GG1-C6-handler.h"

GG1_C6_Handler::GG1_C6_Handler() {
    wDown = false; aDown = false; sDown = false; dDown = false; spDown = false; shDown = false; enDown = false;
    camera = new Camera(glm::vec3(2.8963, 0.35203, -1.65028), glm::vec3(0, 1, 0), -209.7, -6.2);
    camera->movementSpeed = 20.0f;
}

GG1_C6_Handler::~GG1_C6_Handler() {
    delete camera;
}

