#include "plane.h"

void Plane::GenerateVertices() {
    if (tiling_.length == 0)
        return;
    vertices.clear();
    indices.clear();
    glm::vec2 tileSize = 1.0f / (glm::vec2) tiling_;

    heightGrid_ = std::vector<std::vector<float>>(tiling_.y + 1);
    for (int y = 0; y <= tiling_.y; y++) {
        heightGrid_[y] = std::vector<float>(tiling_.x + 1);
        for (int x = 0; x <= tiling_.x; x++) {
            glm::vec3 pos;
            glm::vec2 v = glm::vec2(0);

            if (variation > 0) {
                if (y > 0 && y < tiling_.y) {
                    v.x = ((float) rand() / (RAND_MAX)) - .5f;
                    v.x *= variation;
                }
                if (x > 0 && x < tiling_.x) {
                    v.y = ((float) rand() / (RAND_MAX)) - .5f;
                    v.y *= variation;
                }
            }
            
            pos.x = static_cast<float>(y) * tileSize.x - .5f + v.x * tileSize.x;
            pos.y = static_cast<float>(x) * tileSize.y - .5f + v.y * tileSize.y;
            vertices.push_back(pos.x);
            float height = (((float) rand() / (RAND_MAX)) - .5f) * heightVariation;
            heightGrid_[y][x] = height;
            vertices.push_back(height);
            vertices.push_back(pos.y);

            texCoords.push_back((x + v.x * tileSize.x) / textureSize.x);
            texCoords.push_back((y + v.y * tileSize.y) / textureSize.y);
        }
    }
    for (int y = 0; y < tiling_.y; y++) {
        for (int x = 0; x < tiling_.x; x++) {
            int tl = x + y * (tiling_.x + 1); // bottom left
            int tr = tl + 1; // top right
            int bl = x + (y + 1) * (tiling_.x + 1); // top left
            int br = bl + 1; // bottom right

            // top-left triangle
            indices.push_back(tl);
            indices.push_back(tr);
            indices.push_back(bl);

            // bottom-right triangle
            indices.push_back(tr); 
            indices.push_back(br);
            indices.push_back(bl);
        }
    }

    // all my homies hate calculating normals
    // ripped from https://stackoverflow.com/a/44250149
    // don't know if this is realistic at all but looks alright
    for (int y = 0; y < heightGrid_.size(); y++) {
        for (int x = 0; x < heightGrid_[y].size(); x++) {
            float hn, hnw, he, hse, hs, hsw, hw, hne;

            if (x <= 0) hw = 0;
            else hw = heightGrid_[y][x - 1];

            if (x >= heightGrid_[y].size() - 1) he = 0;
            else he = heightGrid_[y][x + 1];

            if (y <= 0) hn = 0;
            else hn = heightGrid_[y - 1][x];

            if (y >= heightGrid_.size() - 1) hs = 0;
            else hs = heightGrid_[y + 1][x];

            if (y <= 0 || x <= 0) hnw = 0;
            else hnw = heightGrid_[y - 1][x - 1];

            if (y >= heightGrid_.size() - 1 || x <= 0) hsw = 0;
            else hsw = heightGrid_[y + 1][x - 1];

            if (y <= 0) hne = 0;
            else if (x >= heightGrid_[y].size() - 1) hne = 0;
            else hne = heightGrid_[y - 1][x + 1];

            if (y >= heightGrid_.size() - 1) hse = 0;
            else if (x >= heightGrid_[y].size() - 1) hse = 0;
            else hse = heightGrid_[y + 1][x + 1];

            // deduce terrain normal
            float dydx = (hne + 2 * he + hse) - (hnw + 2 * hw + hsw);
            float dydz = (hsw + 2 * hs + hse) - (hnw + 2 * hn + hne);

            glm::vec3 normal = glm::normalize(glm::vec3(-dydx, 1.0f, -dydz));

            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
        }
    }
}