#include <unordered_map>
#include "../include/Model.h"

bool Model::LoadOBJ(const std::string& path) {
    std::ifstream obj_file(path);
    if (!obj_file.is_open()) {
        std::cout << "[INFO] Model Load Failed, File is Invalid." << std::endl;
        return false;
    }

    std::string line;

    std::vector<Vec3> positions;
    std::vector<Vec2> uvs;
    std::vector<Vec3> normals;

    std::vector<uint32_t> posi_idxes;
    std::vector<uint32_t> uv_idxes;
    std::vector<uint32_t> normal_idxes;

    while (std::getline(obj_file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::unordered_map<VertexKey, uint32_t, VertexKeyHash> vertexMap;
        Mesh mesh;
        std::stringstream ss(line);
        std::string type;
        ss >> type;
        if (type == "o" || type == "g") {
            if (!mesh.getIndexBuffer().empty()) {
                m_meshes.emplace_back(mesh);
                mesh = Mesh();
                vertexMap.clear();
            }
            ss >> mesh.name;
        } else if (type == "v") {
            Vec3 posi;
            ss >> posi.x >> posi.y >> posi.z;
            positions.emplace_back(posi);
        } else if (type == "vt") {
            Vec2 uv;
            ss >> uv.x >> uv.y;
            uv.y = 1.0f - uv.y;
            uvs.emplace_back(uv);
        } else if (type == "vn") {
            Vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            normals.emplace_back(normal);
        } else if (type == "f") {
            std::vector<VertexKey> face;

            std::string vertStr;
            // 支持多顶点面片
            while (ss >> vertStr) {
                std::stringstream vs(vertStr);
                std::string idxp, idxu, idxn;

                std::getline(vs, idxp, '/');
                std::getline(vs, idxu, '/');
                std::getline(vs, idxn, '/');

                int vi = idxp.empty() ? 0 : std::stoi(idxp);
                int ti = idxu.empty() ? 0 : std::stoi(idxu);
                int ni = idxn.empty() ? 0 : std::stoi(idxn);

                VertexKey key;
                // 默认先加载v, vt, vn，最后加载f，因此可认为顶点数据已经全了
                key.vIdx = fixIndex(vi, positions.size());
                key.vtIdx = idxu.empty() ? -1 : fixIndex(ti, uvs.size());
                key.vnIdx = idxn.empty() ? -1 : fixIndex(ni, normals.size());
                face.push_back(key);
            }

            // 将该面上所有顶点与面上首个顶点组成三角形
            for (size_t i=1; i+1<face.size(); i++) {
                VertexKey tri[3] = {face[0], face[i], face[i+1]};

                for (int k; k<3; k++) {
                    VertexKey key = tri[k];

                    if (vertexMap.find(key) != vertexMap.end()) {
                        mesh.addIndex(vertexMap[key]);
                    } else {
                        Vertex v;
                        v.position = positions[key.vIdx];
                        v.uv = (key.vtIdx > 0) ? uvs[key.vtIdx];
                        v.normal = (key.vnIdx > 0) ? normals[key.vnIdx];
                        mesh.addVertex(v);
                        vertexMap[key] = mesh.getVerticies().size() - 1;
                        mesh.addIndex(mesh.getVerticies.size() - 1);
                    }
                }
            }
        } 
    }

    if (!mesh.getVertices().empty())
        m_meshes.emplace_back(mesh);
    return true;
}