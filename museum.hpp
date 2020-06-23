#pragma once

#include "renderer.hpp"
#include "museumWallsCoordinates.hpp"

class Museum {

    unsigned int VBO, VAO,VAOrectangle, VBOrectangle, lightVAO;
    static const int numberOfPaintings=17,numberOfPainters=3, numberOfLights=4;
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
            vertices[0+8*i]*=x;
            vertices[2+8*i]*=y;
            vertices[6+8*i]*=x;
            vertices[7+8*i]*=y;
        }


        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
         // normal coord attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // texture coord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glGenVertexArrays(1, &VAOrectangle);
        glGenBuffers(1, &VBOrectangle);
        glBindVertexArray(VAOrectangle);
        glBindBuffer(GL_ARRAY_BUFFER, VBOrectangle);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
        
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
         // normal coord attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // texture coord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);


        glGenVertexArrays(1, &lightVAO);
       glBindBuffer(GL_ARRAY_BUFFER, VBOrectangle);

        glBindVertexArray(lightVAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
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
    void drawLight(Renderer &r){
          glBindBuffer(GL_ARRAY_BUFFER, VBOrectangle);
          glBindVertexArray(lightVAO);
          r.passMatrices();
          glEnableVertexAttribArray(0);
          for(unsigned int i=0;i<4;i++){
          r.i();
          r.tr(lightTranslate[i]);
          r.sc(vec3(0.1,1.0,0.1));
          r.passMatrices();

          r.renderTriangles(6);
          }


    }
    void draw(Renderer &r) {
        r.setLightPositions(lightPositions);
        r.setCameraPosition();
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, textureOfWall);
         r.passMatrices();
       // glEnableVertexAttribArray(0);
       // glEnableVertexAttribArray(1);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindBuffer(GL_ARRAY_BUFFER, VBOrectangle);
        glBindVertexArray(VAOrectangle);
        glBindTexture(GL_TEXTURE_2D, textureOfFloor);

        for(unsigned int i = 0; i < 66; i++)
        {
            vec3 fixedLightPositions[4];
            for(int i=0;i<4;i++){
                fixedLightPositions[i].x=lightPositions[i].x;
                fixedLightPositions[i].y= -lightPositions[i].y - 1.0f;
                fixedLightPositions[i].z=lightPositions[i].z;
            }

            r.setLightPositions(fixedLightPositions);

            r.i();
            r.tr(floorPositions[i]+vec3(0.0f,0.001f,0.0f));
            r.passMatrices();

            r.renderTriangles(6);
        }

        

        glBindTexture(GL_TEXTURE_2D, textureOfDoor);
        r.i();
        r.setLightPositions(lightPositions);

        r.tr(doorPosition);
        r.ro(90, X);
        r.ro(90, Z);
       // r.ro(180,Z);
        r.passMatrices();
        r.renderTriangles(6);

        glBindTexture(GL_TEXTURE_2D, textureOfWall);
        for(unsigned int i = 0; i < 6; i++)
        {
            r.i();
            vec3 fixedLightPositions[4];
            for(int i=0;i<4;i++){
                fixedLightPositions[i].x= 5.0f + lightPositions[i].x;
                fixedLightPositions[i].y= lightPositions[i].y;
                fixedLightPositions[i].z=lightPositions[i].z;
            }
            r.setLightPositions(fixedLightPositions);
            r.tr(roomWallPositions[i]);
            r.ro(90, X);
            r.ro(90, Z);
            r.passMatrices();
            r.renderTriangles(6);
        
            r.i();
            r.setLightPositions(lightPositions);
            r.tr(roomWallPositions[i]+vec3(0.2f,0.0f,0.0f));
            r.ro(90, X);
            r.ro(90, Z);
            r.passMatrices();
            r.renderTriangles(6);
       
            r.i();
            r.setLightPositions(lightPositions);
            r.tr(roomWallPositions[i+6]);
            if(i%2==0){
                r.sc(vec3(0.2,1.0,0.2));
                r.ro(90, X);
            }else{

                r.sc(vec3(0.2,1.0,0.2));
                r.ro(90, X);
                r.ro(180, Z);
            }
            r.passMatrices();
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
            r.passMatrices();
            r.setLightPositions(lightPositions);
            r.renderTriangles(6);
        }


        for(unsigned int i = 0; i < numberOfPaintings; i++)
        {
            glBindTexture(GL_TEXTURE_2D, paintings[i]);
           
            for(int a = 0; a < sizeof(wrongNormalsPaintingsIndexes)/sizeof(int); a++){
              if(wrongNormalsPaintingsIndexes[a]==i){

                vec3 fixedLightPositions[4];
                for(int i=0;i<4;i++){
                    fixedLightPositions[i].x=lightPositions[i].x;
                    fixedLightPositions[i].y= lightPositions[i].y;
                    fixedLightPositions[i].z= -3.0f + lightPositions[i].z;
                 }
                r.setLightPositions(fixedLightPositions);
                break;        
              }else{
                   r.setLightPositions(lightPositions);
             }
            }


       
            r.i();
            r.tr(paintingsPosition[i]);
            r.ro(90, X);
            r.ro(180, Y);
            
            r.sc(paintingsScaling[i]);
            r.passMatrices();

            r.renderTriangles(6);
        }
    }   
};