#pragma once

#include <vector>
#include <string>

// 加载 OBJ 文件，返回交错数组: pos(3) + normal(3)
// - filename: OBJ 路径
// - vertexCount: 输出顶点数量（用于 glDrawArrays 的 count）
std::vector<float> loadOBJ(const std::string& filename, int& vertexCount);
