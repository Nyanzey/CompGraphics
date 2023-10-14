#pragma once

#include <vector>
#include <iostream>
#include <string>
#include "transform.h"

class circle
{
public:
    std::vector<vec4> vtx;
    std::vector<unsigned int> idx;
    vec4 center;
    float rad;
    int points;
    vec4 last_scale;

    unsigned int VAO, VBO, EBO;

    circle(float radius, int point_num, float Xoffset, float Yoffset, float Zoffset)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        rad = radius;
        points = point_num;

        //360 angle to form a cirle
        float angle = 360.f / point_num;

        //Number of triangles to draw based on number of points
        int tri_count = point_num - 1;

        //Appending center poit for reference
        vtx.push_back(vec4(Xoffset, Yoffset, Zoffset, 1.0f));

        last_scale = vec4(1.0f, 1.0f, 1.0f, 1.0f);

        center = vec4(Xoffset, Yoffset, Zoffset, 1.0f);

        //Getting coordinates for every point: x = r.cos(angle), y = r.sin(angle)
        for (int i = 0; i < point_num; i++)
        {
            float cur_angle = angle * i;
            float x = radius * cos(cur_angle * (PI / 180.0));
            float y = radius * sin(cur_angle * (PI / 180.0));
            float w = 1.0f;

            //Inserting new vertices in vectors
            vtx.push_back(vec4(x + Xoffset, y + Yoffset, Zoffset, w));
        }

        //Getting indices for every triangle to draw for EBO
        for (int i = 0; i < tri_count; i++)
        {
            //Center point
            idx.push_back(0);

            //Every two points there is a triangle
            idx.push_back(i + 1);
            idx.push_back(i + 2);
        }

        //Last triangle
        idx.push_back(0);
        idx.push_back(1);
        idx.push_back(vtx.size() - 1);
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

    vec4 get_middle()
    {
        vec4 res;
        for (int i = 0; i < vtx.size(); i++)
        {
            res = res + vtx[i];
        }

        return res / vtx.size();
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
            for (int i = 1; i < vtx.size(); i++)
            {
                idx.push_back(i);
            }
            idx.push_back(1);
        }
        else if (option == "triangle")
        {
            idx.clear();
            int tri_count = points - 1;
            for (int i = 0; i < tri_count; i++)
            {
                //Center point
                idx.push_back(0);

                //Every two points there is a triangle
                idx.push_back(i + 1);
                idx.push_back(i + 2);
            }

            //Last triangle
            idx.push_back(0);
            idx.push_back(1);
            idx.push_back(vtx.size() - 1);
        }
    }

    void update(int num_points, float Xoffset, float Yoffset, float Zoffset)
    {
        if (num_points < 3) { num_points = 3; }

        vtx.clear();
        idx.clear();

        points = num_points;

        //360 angle to form a cirle
        float angle = 360.f / points;

        //Number of triangles to draw based on number of points
        int tri_count = points - 1;

        //Appending center poit for reference
        vtx.push_back(vec4(Xoffset, Yoffset, Zoffset, 1.0f));

        center = vec4(Xoffset, Yoffset, Zoffset, 1.0f);

        //Getting coordinates for every point: x = r.cos(angle), y = r.sin(angle)
        for (int i = 0; i < points; i++)
        {
            float cur_angle = angle * i;
            float x = rad * cos(cur_angle * (PI / 180.0));
            float y = rad * sin(cur_angle * (PI / 180.0));

            //Inserting new vertices in vectors
            vtx.push_back(vec4(x + Xoffset, y + Yoffset, Zoffset, 1.0f));
        }

        //Getting indices for every triangle to draw for EBO
        for (int i = 0; i < tri_count; i++)
        {
            //Center point
            idx.push_back(0);

            //Every two points there is a triangle
            idx.push_back(i + 1);
            idx.push_back(i + 2);
        }

        //Last triangle
        idx.push_back(0);
        idx.push_back(1);
        idx.push_back(vtx.size() - 1);
    }

    void translate_inv(vec4 vec)
    {
        mat4 matrix;
        matrix.set_T(vec);

        mat4 matrixT;
        INV(matrix, matrixT);

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
        INV(matrix, matrixS);

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
        INV(matrix, matrixR);

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
