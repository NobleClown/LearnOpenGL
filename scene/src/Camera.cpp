#include "../include/Camera.h"

// Ray Camera::generateRay(const Vec2& cur_p, const Vec2& resolution) const {
//     float ndc_x = (cur_p.x + 0.5) / resolution.x;
//     float ndc_y = (cur_p.y + 0.5) / resolution.y;

//     float screen_x = 2.0f * ndc_x - 1.0f;
//     float screen_y = 1.0f - ndc_y * 2.0f;

//     float scale = tanf(fov * 0.5f * PI / 180.f);
    
// }

Mat4 Camera::getViewMat() const {
    return Mat4::getViewMat(position, position + forward, up);
}

Mat4 Camera::getProjectionMat() const {
    return Mat4::getPerspectiveMat(fov, aspect, nearPlane, farPlane);
}
