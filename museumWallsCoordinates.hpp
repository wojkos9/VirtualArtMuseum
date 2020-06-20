
//cube vertices
   float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, 1.0f,  0.0f,  0.0f, //boczne
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, 1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, 1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, 1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, 1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, 1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  -1.0f,  0.0f,  0.0f, //boczne
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  -1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, 1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f,  1.0f, //przód
         0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  1.0f, //góra
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f, //dół
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

float rectangleVertices[] = {
 
   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f
};

glm::vec3 floorPositions[] = {
    glm::vec3( 1.0f,  0.0f,  0.0f), 
    glm::vec3( 2.0f,  0.0f,  0.0f), 
    glm::vec3( 3.0f,  0.0f,  0.0f), 
    glm::vec3( 4.0f,  0.0f,  0.0f), 
    glm::vec3( 5.0f,  0.0f,  0.0f), 
    glm::vec3( 6.0f,  0.0f,  0.0f), 
    glm::vec3( 7.0f,  0.0f,  0.0f), 
    glm::vec3(8.0f,  0.0f,  0.0f), 
    glm::vec3( 9.0f,  0.0f,  0.0f), 
    glm::vec3( 10.0f,  0.0f,  0.0f), 
    glm::vec3( -1.0f,  0.0f,  0.0f), 
    glm::vec3( -2.0f,  0.0f,  0.0f),
    glm::vec3( -3.0f,  0.0f,  0.0f), 
    glm::vec3( -4.0f,  0.0f,  0.0f),
    glm::vec3( -5.0f,  0.0f,  0.0f), 
    glm::vec3( -6.0f,  0.0f,  0.0f), 
    glm::vec3( -7.0f,  0.0f,  0.0f),
    glm::vec3( -8.0f,  0.0f,  0.0f), 
    glm::vec3( -9.0f,  0.0f,  0.0f),
    glm::vec3( -10.0f,  0.0f,  0.0f), 
    glm::vec3( 0.0f,  0.0f,  0.0f),

    glm::vec3( 1.0f,  0.0f,  1.0f), 
    glm::vec3( 2.0f,  0.0f,  1.0f), 
    glm::vec3( 3.0f,  0.0f,  1.0f), 
    glm::vec3( 4.0f,  0.0f,  1.0f), 
    glm::vec3( 5.0f,  0.0f,  1.0f), 
    glm::vec3( -1.0f,  0.0f, 1.0f), 
    glm::vec3( -2.0f,  0.0f, 1.0f),
    glm::vec3( -3.0f,  0.0f,  1.0f), 
    glm::vec3( -4.0f,  0.0f,  1.0f),
    glm::vec3( -5.0f,  0.0f,  1.0f), 
    glm::vec3( 0.0f,  0.0f,  1.0f),
    glm::vec3( 6.0f,  0.0f,  1.0f), 
    glm::vec3( 7.0f,  0.0f,  1.0f), 
    glm::vec3( 8.0f,  0.0f,  1.0f), 
    glm::vec3( 9.0f,  0.0f,  1.0f), 
    glm::vec3( 10.0f,  0.0f,  1.0f), 
    glm::vec3( -6.0f,  0.0f, 1.0f), 
    glm::vec3( -7.0f,  0.0f, 1.0f),
    glm::vec3( -8.0f,  0.0f,  1.0f), 
    glm::vec3( -9.0f,  0.0f,  1.0f),
    glm::vec3( -10.0f,  0.0f,  1.0f), 


    glm::vec3( 1.0f,  0.0f,  -1.0f), 
    glm::vec3( 2.0f,  0.0f,  -1.0f), 
    glm::vec3( 3.0f,  0.0f,  -1.0f), 
    glm::vec3( 4.0f,  0.0f,   -1.0f), 
    glm::vec3( 5.0f,  0.0f,   -1.0f), 
    glm::vec3( -1.0f,  0.0f,  -1.0f), 
    glm::vec3( -2.0f,  0.0f, -1.0f),
    glm::vec3( -3.0f,  0.0f,  -1.0f), 
    glm::vec3( -4.0f,  0.0f,  -1.0f),
    glm::vec3( -5.0f,  0.0f,  -1.0f), 
    glm::vec3( 0.0f,  0.0f,  -1.0f),

    glm::vec3( 6.0f,  0.0f,  -1.0f), 
    glm::vec3( 7.0f,  0.0f,  -1.0f), 
    glm::vec3( 8.0f,  0.0f,  -1.0f), 
    glm::vec3( 9.0f,  0.0f,   -1.0f), 
    glm::vec3( 10.0f,  0.0f,   -1.0f), 
    glm::vec3( -6.0f,  0.0f,  -1.0f), 
    glm::vec3( -7.0f,  0.0f, -1.0f),
    glm::vec3( -8.0f,  0.0f,  -1.0f), 
    glm::vec3( -9.0f,  0.0f,  -1.0f),
    glm::vec3( -10.0f,  0.0f,  -1.0f)

};

glm::vec3 roomWallPositions[] = {
    glm::vec3( 5.0f,  0.0f,  1.0f), 
    glm::vec3( 0.0f,  0.0f,  1.0f), 
    glm::vec3( -5.0f,  0.0f,  1.0f), 
    glm::vec3( 5.0f,  0.0f,  -1.0f), 
    glm::vec3( 0.0f,  0.0f,  -1.0f), 
    glm::vec3( -5.0f,  0.0f,  -1.0f)

};

glm::vec3 lightPositions []={
  
    glm::vec3( 7.5f,  0.99f,  0.0f),
    glm::vec3( 2.5f,  0.99f,  0.0f),
    glm::vec3( -2.5f,  0.99f,  0.0f),
    glm::vec3( -7.5f,  0.99f,  0.0f)
};

glm::vec3 doorPosition=  glm::vec3( -10.49f,  0.0f,  0.0f);

glm::vec3 paintingsPosition[] = {
    glm::vec3( -9.0f,  0.0f, 1.74f), 
    glm::vec3( -8.0f,  0.0f,  -1.09f),
    glm::vec3( -7.0f,  0.0f,  -1.09f),
    glm::vec3( -6.0f,  0.0f, 1.74f), 
    glm::vec3( -5.5f,  0.05f,  -1.09f),
    glm::vec3( -3.5f,  0.05f,  -1.09f),
    glm::vec3( -1.5f,  0.05f,  -1.09f),
    glm::vec3( -3.5f,  0.05f,   1.74f),
    glm::vec3( -1.5f,  0.05f,   1.74f),
    glm::vec3( 3.5f,  0.05f,  -1.09f),
    glm::vec3( 1.5f,  0.05f,  -1.09f),
    glm::vec3( 3.5f,  0.05f,   1.74f),
    glm::vec3( 1.5f,  0.05f,   1.74f),
    glm::vec3( 8.0f,  0.05f,  -1.09f),
    glm::vec3( 6.5f,  0.05f,  -1.09f),
    glm::vec3( 6.5f,  0.05f,   1.74f),
    glm::vec3( 8.5f,  0.05f,   1.74f)

};
glm::vec3 paintingsScaling[] ={
    glm::vec3( 1.0f,  0.5f,  0.5f), 
    glm::vec3( 0.5f,  0.75f,  0.6f),
    glm::vec3( 1.0f,  0.75f,  0.5f),
    glm::vec3( 1.0f,  0.5f,  0.75f),
    glm::vec3( 1.0f,  0.75f,  0.45f), 
    glm::vec3( 0.5f,  0.75f,  0.6f),
    glm::vec3( 1.0f,  0.75f,  0.5f),
    glm::vec3( 1.0f,  0.5f,  0.75f),
    glm::vec3( 1.0f,  0.5f,  1.0f), 
    glm::vec3( 0.5f,  0.75f,  0.6f),
    glm::vec3( 1.0f,  0.75f,  0.5f),
    glm::vec3( 1.0f,  0.5f,  0.75f),
    glm::vec3( 1.0f,  0.5f,  1.0f), 
    glm::vec3( 0.5f,  0.75f,  0.6f),
    glm::vec3( 1.0f,  0.75f,  0.5f),
    glm::vec3( 1.0f,  0.5f,  0.75f),
    glm::vec3( 1.0f,  0.5f,  1.0f), 

};

glm::vec3 posterPositions[] ={
    glm::vec3( -4.21f,  0.0f,  -1.0f),
    glm::vec3( 0.77f,  0.0f,  -1.0f),
    glm::vec3( 5.79f,  0.0f,  -1.0f)

};

int wrongNormalsPaintingsIndexes[] = {
1,2,4,5,6,9,10,13,14
};