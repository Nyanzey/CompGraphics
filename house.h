#pragma once

#include <vector>
#include "transform.h"

class house
{
public:

    vec4 center;
    vec4 last_scale;

    unsigned int VBO, VAO, EBO;

    std::vector<vec4> vtx = {
        //wall
         vec4(-0.6f,  0.0f, 0.0f, 1.0f),  // C 0
         vec4(0.6f, 0.0f, 0.0f, 1.0f),  // D 1
        vec4(-0.6f,  -0.8f, 0.0f, 1.0f),  // A 2
        vec4(0.6f, -0.8f, 0.0f, 1.0f),   // B 3
        //roof
         vec4(-1.0f,  0.0f, 0.0f, 1.0f),  // E 4
         vec4(0.0f, 0.8f, 0.0f, 1.0f),  // F 5
        vec4(1.0f,  0.0f, 0.0f, 1.0f)  // G 6
    };

    std::vector<unsigned int> idx = {
        //wall
        0, 1, 3,
        0, 2, 3,
        //roof
        4, 5, 6
    };

    house(float scale_value, float Xoffset, float Yoffset, float Zoffset)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        last_scale = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        scale(scale_value, scale_value, scale_value);
        center = vec4(Xoffset, Yoffset, Zoffset, 1.0f);
    }

    void draw(int vertexColorLocation, int polymode, vec4 RGB)
    {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * vtx.size(), &vtx[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * idx.size(), &idx[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glUniform4f(vertexColorLocation, RGB.x, RGB.y, RGB.z, RGB.w);
        glDrawElements(polymode, idx.size(), GL_UNSIGNED_INT, 0);
    }

    void translate(vec4 vec)
    {
        mat4 matrix;
        matrix.set_T(vec);

        apply_mat(center, matrix);
        for (int i = 0; i < vtx.size(); i++)
        {
            apply_mat(vtx[i], matrix);
        }
    }

    vec4 get_middle()
    {
        vec4 res;
        for (int i = 0; i < vtx.size(); i++)
        {
            res = res + vtx[i];
        }

        return res / vtx.size();
    }

    void scale(float sx, float sy, float sz)
    {
        mat4 matrixS;
        matrixS.set_S(sx / last_scale.x, sy / last_scale.y, sz / last_scale.z);

        last_scale.update(sx, sy, sz, 1.0f);

        mat4 matrixT;
        matrixT.set_T(vec4(center.x, center.y, 0.0f, 1.0f));

        mat4 matrixt;
        matrixt.set_T(vec4(-center.x, -center.y, 0.0f, 1.0f));

        //apply_mat(center, matrix);

        for (int i = 0; i < vtx.size(); i++)
        {
            apply_mat(vtx[i], matrixt);
            apply_mat(vtx[i], matrixS);
            apply_mat(vtx[i], matrixT);
        }
    }

    void rotate(float angle, char axis = 'Z')
    {
        mat4 matrixR;
        
        switch (axis)
        {
        case 'X':
            matrixR.set_RX(angle);
            break;
        case 'Y':
            matrixR.set_RY(angle);
            break;
        default:
            matrixR.set_RZ(angle);
            break;
        }

        mat4 matrixT;
        matrixT.set_T(vec4(center.x, center.y, 0.0f, 1.0f));

        mat4 matrixt;
        matrixt.set_T(vec4(-center.x, -center.y, 0.0f, 1.0f));

        for (int i = 0; i < vtx.size(); i++)
        {
            apply_mat(vtx[i], matrixt);
            apply_mat(vtx[i], matrixR);
            apply_mat(vtx[i], matrixT);
        }
    }

    void redo_indices(std::string option)
    {
        if (option == "outline")
        {
            idx.clear();
            idx = { 4, 5, 6, 1, 3, 2, 0, 4 };
        }
        else if (option == "triangle")
        {
            idx.clear();
            idx =
            {//wall
             0, 1, 3,
             0, 2, 3,
             //roof
             4, 5, 6 };
        }
    }

    void translate_inv(vec4 vec)
    {
        mat4 matrix;
        matrix.set_T(vec);

        mat4 matrixT;
        inverse(matrix, matrixT);

        apply_mat(center, matrixT);
        for (int i = 0; i < vtx.size(); i++)
        {
            apply_mat(vtx[i], matrixT);
        }
    }

    void scale_inv(float sx, float sy, float sz)
    {
        mat4 matrix;
        matrix.set_S(sx / last_scale.x, sy / last_scale.y, sz / last_scale.z);

        mat4 matrixS;
        inverse(matrix, matrixS);

        last_scale.update(sx, sy, sz, 1.0f);

        mat4 matrixT;
        matrixT.set_T(vec4(center.x, center.y, 0.0f, 1.0f));

        mat4 matrixt;
        matrixt.set_T(vec4(-center.x, -center.y, 0.0f, 1.0f));

        for (int i = 0; i < vtx.size(); i++)
        {
            apply_mat(vtx[i], matrixt);
            apply_mat(vtx[i], matrixS);
            apply_mat(vtx[i], matrixT);
        }
    }

    void rotate_inv(float angle, char axis = 'Z')
    {
        mat4 matrix;

        switch (axis)
        {
        case 'X':
            matrix.set_RX(angle);
            break;
        case 'Y':
            matrix.set_RY(angle);
            break;
        default:
            matrix.set_RZ(angle);
            break;
        }

        mat4 matrixR;
        inverse(matrix, matrixR);

        mat4 matrixT;
        matrixT.set_T(vec4(center.x, center.y, 0.0f, 1.0f));

        mat4 matrixt;
        matrixt.set_T(vec4(-center.x, -center.y, 0.0f, 1.0f));

        for (int i = 0; i < vtx.size(); i++)
        {
            apply_mat(vtx[i], matrixt);
            apply_mat(vtx[i], matrixR);
            apply_mat(vtx[i], matrixT);
        }
    }

    void apply_transform(mat4 matrix)
    {
        //Not applying to center (IDK what kind of transform is)
        for (int i = 0; i < vtx.size(); i++)
        {
            apply_mat(vtx[i], matrix);
        }
    }

    float get_limit(char op)
    {
        float minx = vtx[0].x;
        float maxx = vtx[0].x;
        float miny = vtx[0].y;
        float maxy = vtx[0].y;
        for (int i = 1; i < vtx.size(); i++)
        {
            minx = std::min(minx, vtx[i].x);
            maxx = std::max(maxx, vtx[i].x);

            miny = std::min(miny, vtx[i].y);
            maxy = std::max(maxy, vtx[i].y);
        }

        switch (op)
        {
        case 'r':
            return maxx;
        case 'l':
            return minx;
        case 'd':
            return miny;
        case 'u':
            return maxy;
        default:
            break;
        }
    }
};
