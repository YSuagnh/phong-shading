#include "loadobj.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>

#include <glm/glm.hpp>

std::vector<float> loadOBJ(const std::string& filename, int& vertexCount)
{
    // 输出交错数据: pos(3) + normal(3)
    std::vector<float> vertices;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    struct Idx { int v; int n; };
    std::vector<std::vector<Idx>> faces;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "ERROR: Cannot open OBJ file: " << filename << std::endl;
        return vertices;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            // 读取顶点坐标
            float x, y, z;
            iss >> x >> y >> z;
            positions.emplace_back(x, y, z);
        }
        else if (prefix == "vn") {
            float x, y, z;
            iss >> x >> y >> z;
            normals.emplace_back(x, y, z);
        }
        else if (prefix == "f") {
            // 读取面（支持三角/四边形）
            std::string token;
            std::vector<Idx> face;
            while (iss >> token) {
                int v = 0, t = 0, n = 0;
                // 解析 v//n 或 v/t/n 或 v
                if (sscanf(token.c_str(), "%d/%d/%d", &v, &t, &n) == 3) {
                    // v/t/n
                } else if (sscanf(token.c_str(), "%d//%d", &v, &n) == 2) {
                    // v//n
                } else if (sscanf(token.c_str(), "%d/%d", &v, &t) == 2) {
                    // v/t 但无法线
                } else if (sscanf(token.c_str(), "%d", &v) == 1) {
                    // 仅 v
                }
                face.push_back({v - 1, n - 1}); // 转为 0 基
            }
            // 三角或四边形
            if (face.size() == 3) {
                faces.push_back(face);
            } else if (face.size() == 4) {
                // 拆分为两个三角形: 0-1-2, 0-2-3
                faces.push_back({face[0], face[1], face[2]});
                faces.push_back({face[0], face[2], face[3]});
            }
        }
    }

    file.close();
    // 根据索引构建最终的顶点数组（交错: pos + normal）
    // 若 OBJ 未提供法线，则使用“角度加权”平均的平滑顶点法线（推荐用于 Phong Shading）
    bool hasProvidedNormals = !normals.empty();
    std::vector<glm::vec3> smoothNormals;
    if (!hasProvidedNormals) {
        smoothNormals.assign(positions.size(), glm::vec3(0.0f));
        auto safeNormalize = [](const glm::vec3 &v) -> glm::vec3 {
            float len = glm::length(v);
            return (len > 1e-8f) ? (v / len) : glm::vec3(0.0f);
        };
        auto clampDot = [](float d) { return glm::clamp(d, -1.0f, 1.0f); };

        for (const auto &tri : faces) {
            if (tri.size() != 3) continue;
            int i0 = tri[0].v, i1 = tri[1].v, i2 = tri[2].v;
            if (i0 < 0 || i1 < 0 || i2 < 0) continue;
            glm::vec3 p0 = positions[i0];
            glm::vec3 p1 = positions[i1];
            glm::vec3 p2 = positions[i2];
            glm::vec3 e0 = p1 - p0;
            glm::vec3 e1 = p2 - p0;
            glm::vec3 e2 = p2 - p1;

            glm::vec3 fn = glm::cross(e0, e1);
            float area2 = glm::length(fn);
            if (area2 < 1e-12f) continue; // 退化三角形
            fn /= area2; // 先单位化（方向）

            // 角度加权（可选与面积加权结合，这里角度权重已隐含形状特征）
            glm::vec3 u0 = glm::normalize(e0);
            glm::vec3 v0 = glm::normalize(e1);
            glm::vec3 u1 = glm::normalize(p0 - p1);
            glm::vec3 v1 = glm::normalize(p2 - p1);
            glm::vec3 u2 = glm::normalize(p0 - p2);
            glm::vec3 v2 = glm::normalize(p1 - p2);

            float a0 = acosf(glm::clamp(glm::dot(u0, v0), -1.0f, 1.0f));
            float a1 = acosf(glm::clamp(glm::dot(u1, v1), -1.0f, 1.0f));
            float a2 = acosf(glm::clamp(glm::dot(u2, v2), -1.0f, 1.0f));

            // 累加角度加权的面法线
            smoothNormals[i0] += fn * a0;
            smoothNormals[i1] += fn * a1;
            smoothNormals[i2] += fn * a2;
        }
        // 归一化
        for (auto &n : smoothNormals) {
            float len = glm::length(n);
            if (len > 1e-8f) n /= len;
        }
    }

    for (const auto &tri : faces) {
        // 输出三角形顶点
        if (tri.size() != 3) continue;
        for (int i = 0; i < 3; ++i) {
            const auto &idx = tri[i];
            glm::vec3 pos = positions[idx.v];
            glm::vec3 nrm;
            if (hasProvidedNormals && idx.n >= 0 && idx.n < (int)normals.size()) {
                nrm = normals[idx.n];
            } else if (!hasProvidedNormals && idx.v >= 0 && idx.v < (int)smoothNormals.size()) {
                nrm = smoothNormals[idx.v];
            } else {
                // 极端退化回退：使用(0,0,1)
                nrm = glm::vec3(0.0f, 0.0f, 1.0f);
            }
            vertices.push_back(pos.x);
            vertices.push_back(pos.y);
            vertices.push_back(pos.z);
            vertices.push_back(nrm.x);
            vertices.push_back(nrm.y);
            vertices.push_back(nrm.z);
        }
    }

    vertexCount = vertices.size() / 6; // 每个顶点 6 个 float
    std::cout << "Loaded OBJ file: " << filename << std::endl;
    std::cout << "Positions: " << positions.size() << ", Normals: " << normals.size() << std::endl;
    std::cout << "Triangles: " << (vertexCount / 3) << std::endl;

    return vertices;
}
