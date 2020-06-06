#pragma once

#include "renderer.hpp"
#include "museumWallsCoordinates.hpp"

class Museum {

    unsigned int VBO, VAO,VAOrectangle, VBOrectangle;
    static const int numberOfPaintings=17,numberOfPainters=3;
    unsigned int textureOfWall,textureOfFloor,textureOfDoor,paintings[numberOfPaintings],posters[numberOfPainters];
    public:
    Museum() {
        init();
    }

    void init() {
        float x=20.0f;
        float y=3.0f;
        //scaling cube to museum cuboid. glm:scale doesnt work well on textures...
        for(int i=0;i<36;i++){
            vertices[0+5*i]*=x;
            vertices[2+5*i]*=y;
            vertices[3+5*i]*=x;
            vertices[4+5*i]*=y;
        }


        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glGenVertexArrays(1, &VAOrectangle);
        glGenBuffers(1, &VBOrectangle);
        glBindVertexArray(VAOrectangle);
        glBindBuffer(GL_ARRAY_BUFFER, VBOrectangle);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
    // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);


        
        for(int i=0;i<numberOfPaintings;i++){
            std::string str;
            str="graphics/obraz";
            str=str+to_string(i+1);
            str = str+".jpg";
            cout<<str<<endl;
            const char * c = str.c_str();
            paintings[i]=loadTexture(c);
        }

        for(int i=0;i<numberOfPainters;i++){
            std::string str;
            str="graphics/plakat";
            str=str+to_string(i+1);
            str = str+".jpg";
            cout<<str<<endl;
            const char * c = str.c_str();
            posters[i]=loadTexture(c);
        }

        textureOfWall=loadTexture("graphics/whiteHighRes.jpg");
        textureOfFloor=loadTexture("graphics/floor.jpg");
        textureOfDoor=loadTexture("graphics/door.jpg");
    }

    void draw(Renderer &r) {
        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, textureOfWall);
        r.passMatrices();
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindBuffer(GL_ARRAY_BUFFER, VBOrectangle);
        glBindVertexArray(VAOrectangle);
        glBindTexture(GL_TEXTURE_2D, textureOfFloor);

        for(unsigned int i = 0; i < 66; i++)
        {
            r.i();
            r.tr(floorPositions[i]+vec3(0.0f,0.001f,0.0f));
            r.renderTriangles(6);
        }

        

        glBindTexture(GL_TEXTURE_2D, textureOfDoor);
        r.i();
        r.tr(doorPosition);
        r.ro(90, X);
        r.ro(90, Z);
        r.renderTriangles(6);

        glBindTexture(GL_TEXTURE_2D, textureOfWall);
        for(unsigned int i = 0; i < 6; i++)
        {
            r.i();
            r.tr(roomWallPositions[i]);
            r.ro(90, X);
            r.ro(90, Z);
            r.renderTriangles(6);
        }
        for(unsigned int i = 0; i < numberOfPainters; i++){
            glBindTexture(GL_TEXTURE_2D, posters[i]);
            r.i();
            r.tr(posterPositions[i]);
            r.ro(180, Z);
            r.ro(90, X);
            r.ro(90, Z);
            r.sc(0.6f);
            r.renderTriangles(6);
        }


        for(unsigned int i = 0; i < numberOfPaintings; i++)
        {
            glBindTexture(GL_TEXTURE_2D, paintings[i]);

            r.i();

            r.tr(paintingsPosition[i]);
            r.ro(90, X);
            r.ro(180, Y);
            r.sc(paintingsScaling[i]);
            r.renderTriangles(6);
        }
    }   
};