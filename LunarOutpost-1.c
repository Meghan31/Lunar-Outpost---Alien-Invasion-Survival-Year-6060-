// // /*
// //  *  Lunar Outpost - Alien Invasion Survival (Year 6060)
// //  *  
// //  *  A first-person survival game set on the Moon
// //  *  
// //  *  Key bindings:
// //  *  WASD       Move forward/left/backward/right
// //  *  Space      Jump
// //  *  Mouse      Look around (FPP)
// //  *  v/V        Toggle FPP/Third-person debug view
// //  *  l/L        Toggle lighting
// //  *  m/M        Toggle mouse look
// //  *  ESC        Exit
// //  *  
// //  *  Mouse controls (when enabled):
// //  *  Move       Look around
// //  *  Click      Re-center mouse (if it gets stuck)
// //  */

// // #include "CSCIx229.h"
// // #include <stdbool.h>

// // // Window dimensions
// // int windowWidth = 800;
// // int windowHeight = 600;

// // // Camera and view state
// // int axes = 0;           // Display axes for debugging
// // int light = 1;          // Lighting enabled
// // int viewMode = 1;       // 1=FPP, 0=Third-person debug
// // int mouseLook = 1;      // Mouse look enabled
// // double asp = 1.0;       // Aspect ratio
// // double dim = 50.0;      // View distance
// // double fov = 60;        // Field of view

// // // Player position and movement
// // double playerX = 0.0;
// // double playerY = 2.0;   // Height above ground (standing on moon surface)
// // double playerZ = 0.0;
// // double playerVelY = 0.0; // Vertical velocity for jumping
// // double playerHeight = 1.8; // Player eye height

// // // Player rotation (angles in degrees)
// // double playerYaw = 0.0;   // Horizontal rotation (left/right)
// // double playerPitch = 0.0; // Vertical rotation (up/down)

// // // Movement state
// // bool moveForward = false;
// // bool moveBackward = false;
// // bool moveLeft = false;
// // bool moveRight = false;
// // bool isJumping = false;

// // // Mouse control
// // int lastMouseX = 0;
// // int lastMouseY = 0;
// // bool firstMouse = true;
// // double mouseSensitivity = 0.1;

// // // Physics constants
// // const double MOON_GRAVITY = 1.62;  // Moon gravity in m/s²
// // const double MOVE_SPEED = 0.15;    // Movement speed
// // const double JUMP_VELOCITY = 5.0;  // Initial jump velocity

// // // Lighting
// // int zh = 90;           // Light azimuth
// // float ylight = 50.0;   // Light elevation

// // // Terrain data (based on ex18.c approach)
// // #define TERRAIN_SIZE 65
// // #define TERRAIN_SCALE 2.0
// // float terrainZ[TERRAIN_SIZE][TERRAIN_SIZE];
// // float terrainNormals[TERRAIN_SIZE][TERRAIN_SIZE][3];
// // float terrainMin = 0.0;
// // float terrainMax = 0.0;

// // /*
// //  * Initialize terrain with procedurally generated lunar surface
// //  * Using simple noise for now, will enhance later
// //  */
// // void InitTerrain()
// // {
// //    // Generate simple random terrain (we'll make this more sophisticated later)
// //    srand(12345);  // Fixed seed for consistent terrain
   
// //    for (int i = 0; i < TERRAIN_SIZE; i++)
// //    {
// //       for (int j = 0; j < TERRAIN_SIZE; j++)
// //       {
// //          // Simple height function - we'll improve this
// //          double x = (i - TERRAIN_SIZE/2) * TERRAIN_SCALE;
// //          double z = (j - TERRAIN_SIZE/2) * TERRAIN_SCALE;
         
// //          // Create some varied terrain
// //          terrainZ[i][j] = 0.5 * sin(x * 0.05) * cos(z * 0.05) + 
// //                           0.3 * sin(x * 0.1) * sin(z * 0.1) +
// //                           ((rand() % 100) / 100.0 - 0.5) * 0.2;
         
// //          if (terrainZ[i][j] < terrainMin) terrainMin = terrainZ[i][j];
// //          if (terrainZ[i][j] > terrainMax) terrainMax = terrainZ[i][j];
// //       }
// //    }
   
// //    // Compute normals for terrain (based on ex18.c approach)
// //    for (int i = 0; i <= TERRAIN_SIZE-1; i++)
// //    {
// //       for (int j = 0; j <= TERRAIN_SIZE-1; j++)
// //       {
// //          // X component
// //          if (i == 0)
// //             terrainNormals[i][j][0] = terrainZ[i][j] - terrainZ[i+1][j];
// //          else if (i == TERRAIN_SIZE-1)
// //             terrainNormals[i][j][0] = terrainZ[i-1][j] - terrainZ[i][j];
// //          else
// //             terrainNormals[i][j][0] = (terrainZ[i-1][j] - terrainZ[i+1][j]) / 2.0;
         
// //          // Y component
// //          if (j == 0)
// //             terrainNormals[i][j][1] = terrainZ[i][j] - terrainZ[i][j+1];
// //          else if (j == TERRAIN_SIZE-1)
// //             terrainNormals[i][j][1] = terrainZ[i][j-1] - terrainZ[i][j];
// //          else
// //             terrainNormals[i][j][1] = (terrainZ[i][j-1] - terrainZ[i][j+1]) / 2.0;
         
// //          // Z component is the grid spacing
// //          terrainNormals[i][j][2] = TERRAIN_SCALE;
// //       }
// //    }
   
// //    Print("Terrain initialized: height range [%.2f, %.2f]\n", terrainMin, terrainMax);
// // }

// // /*
// //  * Get terrain height at given world coordinates
// //  * Returns the interpolated height for collision detection
// //  */
// // double GetTerrainHeight(double worldX, double worldZ)
// // {
// //    // Convert world coordinates to terrain grid coordinates
// //    double gridX = (worldX / TERRAIN_SCALE) + TERRAIN_SIZE / 2.0;
// //    double gridZ = (worldZ / TERRAIN_SCALE) + TERRAIN_SIZE / 2.0;
   
// //    // Clamp to terrain bounds
// //    if (gridX < 0 || gridX >= TERRAIN_SIZE-1 || gridZ < 0 || gridZ >= TERRAIN_SIZE-1)
// //       return 0.0;
   
// //    // Get integer and fractional parts for interpolation
// //    int x0 = (int)gridX;
// //    int z0 = (int)gridZ;
// //    double fx = gridX - x0;
// //    double fz = gridZ - z0;
   
// //    // Bilinear interpolation
// //    double h00 = terrainZ[x0][z0];
// //    double h10 = terrainZ[x0+1][z0];
// //    double h01 = terrainZ[x0][z0+1];
// //    double h11 = terrainZ[x0+1][z0+1];
   
// //    double h0 = h00 * (1-fx) + h10 * fx;
// //    double h1 = h01 * (1-fx) + h11 * fx;
   
// //    return h0 * (1-fz) + h1 * fz;
// // }

// // /*
// //  * Draw the lunar terrain
// //  * Based on ex18.c draped texture approach
// //  */
// // void DrawTerrain()
// // {
// //    glColor3f(0.7, 0.7, 0.7);  // Moon gray color
   
// //    // Draw terrain as grid of quads
// //    for (int i = 0; i < TERRAIN_SIZE-1; i++)
// //    {
// //       for (int j = 0; j < TERRAIN_SIZE-1; j++)
// //       {
// //          double x = (i - TERRAIN_SIZE/2) * TERRAIN_SCALE;
// //          double z = (j - TERRAIN_SIZE/2) * TERRAIN_SCALE;
         
// //          glBegin(GL_QUADS);
         
// //          glNormal3fv(terrainNormals[i][j]);
// //          glVertex3d(x, terrainZ[i][j], z);
         
// //          glNormal3fv(terrainNormals[i+1][j]);
// //          glVertex3d(x + TERRAIN_SCALE, terrainZ[i+1][j], z);
         
// //          glNormal3fv(terrainNormals[i+1][j+1]);
// //          glVertex3d(x + TERRAIN_SCALE, terrainZ[i+1][j+1], z + TERRAIN_SCALE);
         
// //          glNormal3fv(terrainNormals[i][j+1]);
// //          glVertex3d(x, terrainZ[i][j+1], z + TERRAIN_SCALE);
         
// //          glEnd();
// //       }
// //    }
// // }

// // /*
// //  * Draw a simple player indicator (for third-person debug view)
// //  */
// // void DrawPlayer()
// // {
// //    glPushMatrix();
// //    glTranslated(playerX, playerY, playerZ);
   
// //    // Simple capsule to represent player
// //    glColor3f(0.2, 0.8, 0.2);
   
// //    // Body
// //    glPushMatrix();
// //    glScaled(0.3, playerHeight/2, 0.3);
// //    glTranslated(0, 1, 0);
// //    glutSolidCube(1.0);
// //    glPopMatrix();
   
// //    // Head
// //    glPushMatrix();
// //    glTranslated(0, playerHeight, 0);
// //    glutSolidSphere(0.2, 8, 8);
// //    glPopMatrix();
   
// //    // Direction indicator
// //    glColor3f(1, 0, 0);
// //    glBegin(GL_LINES);
// //    glVertex3d(0, playerHeight/2, 0);
// //    glVertex3d(sin(playerYaw * M_PI/180) * 0.5, playerHeight/2, cos(playerYaw * M_PI/180) * 0.5);
// //    glEnd();
   
// //    glPopMatrix();
// // }

// // /*
// //  * Update player physics and position
// //  */
// // void UpdatePlayer(double dt)
// // {
// //    // Apply moon gravity
// //    if (isJumping || playerY > GetTerrainHeight(playerX, playerZ) + playerHeight + 0.01)
// //    {
// //       playerVelY -= MOON_GRAVITY * dt;
// //       playerY += playerVelY * dt;
      
// //       // Check ground collision
// //       double groundHeight = GetTerrainHeight(playerX, playerZ) + playerHeight;
// //       if (playerY <= groundHeight)
// //       {
// //          playerY = groundHeight;
// //          playerVelY = 0.0;
// //          isJumping = false;
// //       }
// //    }
   
// //    // Movement (based on player facing direction)
// //    double moveX = 0.0;
// //    double moveZ = 0.0;
   
// //    if (moveForward)
// //    {
// //       moveX += sin(playerYaw * M_PI / 180.0) * MOVE_SPEED;
// //       moveZ += cos(playerYaw * M_PI / 180.0) * MOVE_SPEED;
// //    }
// //    if (moveBackward)
// //    {
// //       moveX -= sin(playerYaw * M_PI / 180.0) * MOVE_SPEED;
// //       moveZ -= cos(playerYaw * M_PI / 180.0) * MOVE_SPEED;
// //    }
// //    if (moveLeft)
// //    {
// //       moveX -= cos(playerYaw * M_PI / 180.0) * MOVE_SPEED;
// //       moveZ += sin(playerYaw * M_PI / 180.0) * MOVE_SPEED;
// //    }
// //    if (moveRight)
// //    {
// //       moveX += cos(playerYaw * M_PI / 180.0) * MOVE_SPEED;
// //       moveZ -= sin(playerYaw * M_PI / 180.0) * MOVE_SPEED;
// //    }
   
// //    // Apply movement
// //    playerX += moveX;
// //    playerZ += moveZ;
   
// //    // Keep player above terrain when walking
// //    if (!isJumping)
// //    {
// //       double groundHeight = GetTerrainHeight(playerX, playerZ) + playerHeight;
// //       if (playerY < groundHeight)
// //          playerY = groundHeight;
// //    }
// // }

// // /*
// //  * Set up the camera based on view mode
// //  */
// // void SetupCamera()
// // {
// //    glLoadIdentity();
   
// //    if (viewMode)  // First-person view
// //    {
// //       // Calculate look direction based on yaw and pitch
// //       double lookX = sin(playerYaw * M_PI / 180.0) * cos(playerPitch * M_PI / 180.0);
// //       double lookY = sin(playerPitch * M_PI / 180.0);
// //       double lookZ = cos(playerYaw * M_PI / 180.0) * cos(playerPitch * M_PI / 180.0);
      
// //       // Position camera at player eye position
// //       gluLookAt(playerX, playerY, playerZ,
// //                 playerX + lookX, playerY + lookY, playerZ + lookZ,
// //                 0, 1, 0);
// //    }
// //    else  // Third-person debug view
// //    {
// //       // Camera positioned behind and above player
// //       double camDist = 5.0;
// //       double camHeight = 3.0;
      
// //       double camX = playerX - sin(playerYaw * M_PI / 180.0) * camDist;
// //       double camY = playerY + camHeight;
// //       double camZ = playerZ - cos(playerYaw * M_PI / 180.0) * camDist;
      
// //       gluLookAt(camX, camY, camZ,
// //                 playerX, playerY, playerZ,
// //                 0, 1, 0);
// //    }
// // }

// // /*
// //  * Setup lighting
// //  * Based on ex22.c and my_objects_for_project.c lighting setup
// //  */
// // void SetupLighting()
// // {
// //    if (light)
// //    {
// //       float Ambient[]  = {0.3, 0.3, 0.3, 1.0};
// //       float Diffuse[]  = {0.8, 0.8, 0.8, 1.0};
// //       float Specular[] = {0.5, 0.5, 0.5, 1.0};
// //       float Position[] = {5 * Cos(zh), ylight, 5 * Sin(zh), 0.0};  // Directional light (sun)
      
// //       glEnable(GL_NORMALIZE);
// //       glEnable(GL_LIGHTING);
// //       glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
// //       glEnable(GL_COLOR_MATERIAL);
// //       glEnable(GL_LIGHT0);
      
// //       glLightfv(GL_LIGHT0, GL_AMBIENT,  Ambient);
// //       glLightfv(GL_LIGHT0, GL_DIFFUSE,  Diffuse);
// //       glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
// //       glLightfv(GL_LIGHT0, GL_POSITION, Position);
// //    }
// //    else
// //    {
// //       glDisable(GL_LIGHTING);
// //    }
// // }

// // /*
// //  * OpenGL display function
// //  */
// // void display()
// // {
// //    // Clear the window
// //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// //    glEnable(GL_DEPTH_TEST);
   
// //    // Set up camera
// //    SetupCamera();
   
// //    // Set up lighting
// //    SetupLighting();
   
// //    // Draw terrain
// //    DrawTerrain();
   
// //    // Draw player model in third-person view
// //    if (!viewMode)
// //    {
// //       DrawPlayer();
// //    }
   
// //    // Draw axes for debugging
// //    if (axes)
// //    {
// //       glDisable(GL_LIGHTING);
// //       glColor3f(1, 1, 1);
// //       glBegin(GL_LINES);
// //       glVertex3d(0, 0, 0);
// //       glVertex3d(5, 0, 0);
// //       glVertex3d(0, 0, 0);
// //       glVertex3d(0, 5, 0);
// //       glVertex3d(0, 0, 0);
// //       glVertex3d(0, 0, 5);
// //       glEnd();
      
// //       glRasterPos3d(5, 0, 0);
// //       Print("X");
// //       glRasterPos3d(0, 5, 0);
// //       Print("Y");
// //       glRasterPos3d(0, 0, 5);
// //       Print("Z");
// //    }
   
// //    // Display HUD info
// //    glDisable(GL_LIGHTING);
// //    glColor3f(1, 1, 1);
// //    glWindowPos2i(5, 5);
// //    Print("Position: (%.1f, %.1f, %.1f) Yaw: %.1f Pitch: %.1f", 
// //          playerX, playerY, playerZ, playerYaw, playerPitch);
// //    glWindowPos2i(5, 25);
// //    Print("View: %s | Mouse Look: %s | Light: %s",
// //          viewMode ? "FPP" : "Third-Person", mouseLook ? "ON" : "OFF", light ? "ON" : "OFF");
// //    glWindowPos2i(5, 45);
// //    Print("WASD: Move | Space: Jump | Mouse: Look | V: Toggle View | M: Toggle Mouse | L: Light");
   
// //    // Check for errors and swap buffers
// //    ErrCheck("display");
// //    glFlush();
// //    glutSwapBuffers();
// // }

// // /*
// //  * Idle function for continuous updates
// //  */
// // void idle()
// // {
// //    static int lastTime = 0;
// //    int currentTime = glutGet(GLUT_ELAPSED_TIME);
// //    double dt = (currentTime - lastTime) / 1000.0;
// //    lastTime = currentTime;
   
// //    if (dt > 0.1) dt = 0.1;  // Cap dt to prevent huge jumps
   
// //    UpdatePlayer(dt);
   
// //    // Update light position (orbiting sun)
// //    double t = currentTime / 1000.0;
// //    zh = fmod(10 * t, 360.0);  // Slower sun movement
   
// //    glutPostRedisplay();
// // }

// // /*
// //  * Handle keyboard input
// //  */
// // void keyboard(unsigned char key, int x, int y)
// // {
// //    if (key == 27)  // ESC
// //       exit(0);
// //    else if (key == 'w' || key == 'W')
// //       moveForward = true;
// //    else if (key == 's' || key == 'S')
// //       moveBackward = true;
// //    else if (key == 'a' || key == 'A')
// //       moveLeft = true;
// //    else if (key == 'd' || key == 'D')
// //       moveRight = true;
// //    else if (key == ' ' && !isJumping)  // Space bar - jump
// //    {
// //       isJumping = true;
// //       playerVelY = JUMP_VELOCITY;
// //    }
// //    else if (key == 'v' || key == 'V')
// //       viewMode = 1 - viewMode;
// //    else if (key == 'm' || key == 'M')
// //    {
// //       mouseLook = 1 - mouseLook;
// //       if (mouseLook)
// //          glutSetCursor(GLUT_CURSOR_NONE);
// //       else
// //          glutSetCursor(GLUT_CURSOR_INHERIT);
// //    }
// //    else if (key == 'l' || key == 'L')
// //       light = 1 - light;
// //    else if (key == 'x' || key == 'X')
// //       axes = 1 - axes;
   
// //    glutPostRedisplay();
// // }

// // /*
// //  * Handle keyboard release
// //  */
// // void keyboardUp(unsigned char key, int x, int y)
// // {
// //    if (key == 'w' || key == 'W')
// //       moveForward = false;
// //    else if (key == 's' || key == 'S')
// //       moveBackward = false;
// //    else if (key == 'a' || key == 'A')
// //       moveLeft = false;
// //    else if (key == 'd' || key == 'D')
// //       moveRight = false;
// // }

// // /*
// //  * Handle mouse motion for looking around
// //  * Smooth mouse look implementation
// //  */
// // void mouseMotion(int x, int y)
// // {
// //    if (!mouseLook) return;
   
// //    if (firstMouse)
// //    {
// //       lastMouseX = x;
// //       lastMouseY = y;
// //       firstMouse = false;
// //       return;
// //    }
   
// //    // Calculate mouse movement delta
// //    int deltaX = x - lastMouseX;
// //    int deltaY = y - lastMouseY;
   
// //    lastMouseX = x;
// //    lastMouseY = y;
   
// //    // Update player rotation
// //    playerYaw += deltaX * mouseSensitivity;
// //    playerPitch -= deltaY * mouseSensitivity;
   
// //    // Clamp pitch to prevent camera flipping
// //    if (playerPitch > 89.0) playerPitch = 89.0;
// //    if (playerPitch < -89.0) playerPitch = -89.0;
   
// //    // Wrap yaw
// //    while (playerYaw > 360.0) playerYaw -= 360.0;
// //    while (playerYaw < 0.0) playerYaw += 360.0;
   
// //    // Re-center mouse to prevent hitting screen edges
// //    if (abs(deltaX) > 100 || abs(deltaY) > 100 ||
// //        x < 50 || x > windowWidth - 50 || y < 50 || y > windowHeight - 50)
// //    {
// //       glutWarpPointer(windowWidth / 2, windowHeight / 2);
// //       lastMouseX = windowWidth / 2;
// //       lastMouseY = windowHeight / 2;
// //    }
   
// //    glutPostRedisplay();
// // }

// // /*
// //  * Handle passive mouse motion (when no button is pressed)
// //  */
// // void passiveMouseMotion(int x, int y)
// // {
// //    mouseMotion(x, y);
// // }

// // /*
// //  * Handle window reshape
// //  */
// // void reshape(int width, int height)
// // {
// //    windowWidth = width;
// //    windowHeight = height;
// //    asp = (height > 0) ? (double)width / height : 1;
// //    glViewport(0, 0, width, height);
// //    Project(fov, asp, dim);
// // }

// // /*
// //  * Main program
// //  */
// // int main(int argc, char* argv[])
// // {
// //    // Initialize GLUT
// //    glutInit(&argc, argv);
// //    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
// //    glutInitWindowSize(windowWidth, windowHeight);
// //    glutCreateWindow("Lunar Outpost - Alien Invasion Survival");
   
// // #ifdef USEGLEW
// //    if (glewInit() != GLEW_OK) Fatal("Error initializing GLEW\n");
// // #endif
   
// //    // Initialize terrain
// //    InitTerrain();
   
// //    // Set player starting position (slightly above terrain)
// //    playerY = GetTerrainHeight(playerX, playerZ) + playerHeight;
   
// //    // Set callbacks
// //    glutDisplayFunc(display);
// //    glutReshapeFunc(reshape);
// //    glutKeyboardFunc(keyboard);
// //    glutKeyboardUpFunc(keyboardUp);
// //    glutMotionFunc(mouseMotion);
// //    glutPassiveMotionFunc(passiveMouseMotion);
// //    glutIdleFunc(idle);
   
// //    // Hide cursor for FPS mode
// //    glutSetCursor(GLUT_CURSOR_NONE);
   
// //    // Set projection
// //    Project(fov, asp, dim);
   
// //    Print("Lunar Outpost initialized\n");
// //    Print("Controls: WASD=Move, Space=Jump, Mouse=Look, V=View, M=Mouse, L=Light\n");
   
// //    // Start main loop
// //    ErrCheck("init");
// //    glutMainLoop();
   
// //    return 0;
// // }



// /*
//  *  Lunar Outpost - Alien Invasion Survival (Year 6060)
//  *  
//  *  A first-person survival game set on the Moon
//  *  
//  *  Key bindings:
//  *  WASD       Move forward/left/backward/right
//  *  Space      Jump
//  *  Mouse      Look around (FPP)
//  *  v/V        Toggle FPP/Third-person debug view
//  *  l/L        Toggle lighting
//  *  m/M        Toggle mouse look
//  *  ESC        Exit
//  *  
//  *  Mouse controls (when enabled):
//  *  Move       Look around
//  *  Click      Re-center mouse (if it gets stuck)
//  */

// #include "CSCIx229.h"
// #include <stdbool.h>

// // Window dimensions
// int windowWidth = 800;
// int windowHeight = 600;

// // Camera and view state
// int axes = 0;           // Display axes for debugging
// int light = 1;          // Lighting enabled
// int viewMode = 1;       // 1=FPP, 0=Third-person debug
// int mouseLook = 1;      // Mouse look enabled
// double asp = 1.0;       // Aspect ratio
// double dim = 200.0;     // View distance - increased for larger terrain
// double fov = 60;        // Field of view

// // Player position and movement
// double playerX = 0.0;
// double playerY = 2.0;   // Height above ground (standing on moon surface)
// double playerZ = 0.0;
// double playerVelY = 0.0; // Vertical velocity for jumping
// double playerHeight = 1.8; // Player eye height

// // Player rotation (angles in degrees)
// double playerYaw = 0.0;   // Horizontal rotation (left/right)
// double playerPitch = 0.0; // Vertical rotation (up/down)

// // Movement state
// bool moveForward = false;
// bool moveBackward = false;
// bool moveLeft = false;
// bool moveRight = false;
// bool isJumping = false;

// // Mouse control
// int lastMouseX = 0;
// int lastMouseY = 0;
// bool firstMouse = true;
// double mouseSensitivity = 0.05;  // Reduced for smoother control

// // Physics constants
// const double MOON_GRAVITY = 1.62;  // Moon gravity in m/s²
// const double MOVE_SPEED = 0.15;    // Movement speed
// const double JUMP_VELOCITY = 5.0;  // Initial jump velocity

// // Lighting
// int zh = 90;           // Light azimuth
// float ylight = 50.0;   // Light elevation

// // Terrain data (based on ex18.c approach)
// #define TERRAIN_SIZE 65
// #define TERRAIN_SCALE 5.0  // Increased for larger terrain
// float terrainZ[TERRAIN_SIZE][TERRAIN_SIZE];
// float terrainNormals[TERRAIN_SIZE][TERRAIN_SIZE][3];
// float terrainMin = 0.0;
// float terrainMax = 0.0;

// /*
//  * Initialize terrain with procedurally generated lunar surface
//  * Using simple noise for now, will enhance later
//  */
// void InitTerrain()
// {
//    // Generate simple random terrain (we'll make this more sophisticated later)
//    srand(12345);  // Fixed seed for consistent terrain
   
//    for (int i = 0; i < TERRAIN_SIZE; i++)
//    {
//       for (int j = 0; j < TERRAIN_SIZE; j++)
//       {
//          // Simple height function - we'll improve this
//          double x = (i - TERRAIN_SIZE/2) * TERRAIN_SCALE;
//          double z = (j - TERRAIN_SIZE/2) * TERRAIN_SCALE;
         
//          // Create some varied terrain
//          terrainZ[i][j] = 0.5 * sin(x * 0.05) * cos(z * 0.05) + 
//                           0.3 * sin(x * 0.1) * sin(z * 0.1) +
//                           ((rand() % 100) / 100.0 - 0.5) * 0.2;
         
//          if (terrainZ[i][j] < terrainMin) terrainMin = terrainZ[i][j];
//          if (terrainZ[i][j] > terrainMax) terrainMax = terrainZ[i][j];
//       }
//    }
   
//    // Compute normals for terrain (based on ex18.c approach)
//    for (int i = 0; i <= TERRAIN_SIZE-1; i++)
//    {
//       for (int j = 0; j <= TERRAIN_SIZE-1; j++)
//       {
//          // X component
//          if (i == 0)
//             terrainNormals[i][j][0] = terrainZ[i][j] - terrainZ[i+1][j];
//          else if (i == TERRAIN_SIZE-1)
//             terrainNormals[i][j][0] = terrainZ[i-1][j] - terrainZ[i][j];
//          else
//             terrainNormals[i][j][0] = (terrainZ[i-1][j] - terrainZ[i+1][j]) / 2.0;
         
//          // Y component
//          if (j == 0)
//             terrainNormals[i][j][1] = terrainZ[i][j] - terrainZ[i][j+1];
//          else if (j == TERRAIN_SIZE-1)
//             terrainNormals[i][j][1] = terrainZ[i][j-1] - terrainZ[i][j];
//          else
//             terrainNormals[i][j][1] = (terrainZ[i][j-1] - terrainZ[i][j+1]) / 2.0;
         
//          // Z component is the grid spacing
//          terrainNormals[i][j][2] = TERRAIN_SCALE;
//       }
//    }
   
//    Print("Terrain initialized: height range [%.2f, %.2f]\n", terrainMin, terrainMax);
// }

// /*
//  * Get terrain height at given world coordinates
//  * Returns the interpolated height for collision detection
//  */
// double GetTerrainHeight(double worldX, double worldZ)
// {
//    // Convert world coordinates to terrain grid coordinates
//    double gridX = (worldX / TERRAIN_SCALE) + TERRAIN_SIZE / 2.0;
//    double gridZ = (worldZ / TERRAIN_SCALE) + TERRAIN_SIZE / 2.0;
   
//    // Clamp to terrain bounds
//    if (gridX < 0 || gridX >= TERRAIN_SIZE-1 || gridZ < 0 || gridZ >= TERRAIN_SIZE-1)
//       return 0.0;
   
//    // Get integer and fractional parts for interpolation
//    int x0 = (int)gridX;
//    int z0 = (int)gridZ;
//    double fx = gridX - x0;
//    double fz = gridZ - z0;
   
//    // Bilinear interpolation
//    double h00 = terrainZ[x0][z0];
//    double h10 = terrainZ[x0+1][z0];
//    double h01 = terrainZ[x0][z0+1];
//    double h11 = terrainZ[x0+1][z0+1];
   
//    double h0 = h00 * (1-fx) + h10 * fx;
//    double h1 = h01 * (1-fx) + h11 * fx;
   
//    return h0 * (1-fz) + h1 * fz;
// }

// /*
//  * Draw the lunar terrain
//  * Based on ex18.c draped texture approach
//  */
// void DrawTerrain()
// {
//    glColor3f(0.7, 0.7, 0.7);  // Moon gray color
   
//    // Draw terrain as grid of quads
//    for (int i = 0; i < TERRAIN_SIZE-1; i++)
//    {
//       for (int j = 0; j < TERRAIN_SIZE-1; j++)
//       {
//          double x = (i - TERRAIN_SIZE/2) * TERRAIN_SCALE;
//          double z = (j - TERRAIN_SIZE/2) * TERRAIN_SCALE;
         
//          glBegin(GL_QUADS);
         
//          glNormal3fv(terrainNormals[i][j]);
//          glVertex3d(x, terrainZ[i][j], z);
         
//          glNormal3fv(terrainNormals[i+1][j]);
//          glVertex3d(x + TERRAIN_SCALE, terrainZ[i+1][j], z);
         
//          glNormal3fv(terrainNormals[i+1][j+1]);
//          glVertex3d(x + TERRAIN_SCALE, terrainZ[i+1][j+1], z + TERRAIN_SCALE);
         
//          glNormal3fv(terrainNormals[i][j+1]);
//          glVertex3d(x, terrainZ[i][j+1], z + TERRAIN_SCALE);
         
//          glEnd();
//       }
//    }
// }

// /*
//  * Draw a simple player indicator (for third-person debug view)
//  */
// void DrawPlayer()
// {
//    glPushMatrix();
//    glTranslated(playerX, playerY, playerZ);
   
//    // Simple capsule to represent player
//    glColor3f(0.2, 0.8, 0.2);
   
//    // Body
//    glPushMatrix();
//    glScaled(0.3, playerHeight/2, 0.3);
//    glTranslated(0, 1, 0);
//    glutSolidCube(1.0);
//    glPopMatrix();
   
//    // Head
//    glPushMatrix();
//    glTranslated(0, playerHeight, 0);
//    glutSolidSphere(0.2, 8, 8);
//    glPopMatrix();
   
//    // Direction indicator
//    glColor3f(1, 0, 0);
//    glBegin(GL_LINES);
//    glVertex3d(0, playerHeight/2, 0);
//    glVertex3d(sin(playerYaw * M_PI/180) * 0.5, playerHeight/2, cos(playerYaw * M_PI/180) * 0.5);
//    glEnd();
   
//    glPopMatrix();
// }

// /*
//  * Update player physics and position
//  */
// void UpdatePlayer(double dt)
// {
//    // Apply moon gravity
//    if (isJumping || playerY > GetTerrainHeight(playerX, playerZ) + playerHeight + 0.01)
//    {
//       playerVelY -= MOON_GRAVITY * dt;
//       playerY += playerVelY * dt;
      
//       // Check ground collision
//       double groundHeight = GetTerrainHeight(playerX, playerZ) + playerHeight;
//       if (playerY <= groundHeight)
//       {
//          playerY = groundHeight;
//          playerVelY = 0.0;
//          isJumping = false;
//       }
//    }
   
//    // Movement (based on player facing direction)
//    double moveX = 0.0;
//    double moveZ = 0.0;
   
//    if (moveForward)
//    {
//       moveX += sin(playerYaw * M_PI / 180.0) * MOVE_SPEED;
//       moveZ += cos(playerYaw * M_PI / 180.0) * MOVE_SPEED;
//    }
//    if (moveBackward)
//    {
//       moveX -= sin(playerYaw * M_PI / 180.0) * MOVE_SPEED;
//       moveZ -= cos(playerYaw * M_PI / 180.0) * MOVE_SPEED;
//    }
//    if (moveLeft)
//    {
//       moveX -= cos(playerYaw * M_PI / 180.0) * MOVE_SPEED;
//       moveZ += sin(playerYaw * M_PI / 180.0) * MOVE_SPEED;
//    }
//    if (moveRight)
//    {
//       moveX += cos(playerYaw * M_PI / 180.0) * MOVE_SPEED;
//       moveZ -= sin(playerYaw * M_PI / 180.0) * MOVE_SPEED;
//    }
   
//    // Apply movement
//    playerX += moveX;
//    playerZ += moveZ;
   
//    // Keep player within terrain boundaries
//    double maxTerrainX = (TERRAIN_SIZE / 2.0 - 2) * TERRAIN_SCALE;
//    double maxTerrainZ = (TERRAIN_SIZE / 2.0 - 2) * TERRAIN_SCALE;
   
//    if (playerX > maxTerrainX) playerX = maxTerrainX;
//    if (playerX < -maxTerrainX) playerX = -maxTerrainX;
//    if (playerZ > maxTerrainZ) playerZ = maxTerrainZ;
//    if (playerZ < -maxTerrainZ) playerZ = -maxTerrainZ;
   
//    // Keep player above terrain when walking
//    if (!isJumping)
//    {
//       double groundHeight = GetTerrainHeight(playerX, playerZ) + playerHeight;
//       if (playerY < groundHeight)
//          playerY = groundHeight;
//    }
// }

// /*
//  * Set up projection with proper near/far clipping planes
//  * Fixes black screen when looking down
//  */
// void SetupProjection()
// {
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
   
//    // Use perspective with very close near plane to prevent clipping
//    double nearPlane = 0.1;   // Very close to camera
//    double farPlane = 500.0;  // Far enough to see distant terrain
   
//    gluPerspective(fov, asp, nearPlane, farPlane);
   
//    glMatrixMode(GL_MODELVIEW);
// }

// /*
//  * Set up the camera based on view mode
//  */
// void SetupCamera()
// {
//    glLoadIdentity();
   
//    if (viewMode)  // First-person view
//    {
//       // Calculate look direction based on yaw and pitch
//       double lookX = sin(playerYaw * M_PI / 180.0) * cos(playerPitch * M_PI / 180.0);
//       double lookY = sin(playerPitch * M_PI / 180.0);
//       double lookZ = cos(playerYaw * M_PI / 180.0) * cos(playerPitch * M_PI / 180.0);
      
//       // Position camera at player eye position
//       gluLookAt(playerX, playerY, playerZ,
//                 playerX + lookX, playerY + lookY, playerZ + lookZ,
//                 0, 1, 0);
//    }
//    else  // Third-person debug view
//    {
//       // Camera positioned behind and above player
//       double camDist = 5.0;
//       double camHeight = 3.0;
      
//       double camX = playerX - sin(playerYaw * M_PI / 180.0) * camDist;
//       double camY = playerY + camHeight;
//       double camZ = playerZ - cos(playerYaw * M_PI / 180.0) * camDist;
      
//       gluLookAt(camX, camY, camZ,
//                 playerX, playerY, playerZ,
//                 0, 1, 0);
//    }
// }

// /*
//  * Setup lighting
//  * Based on ex22.c and my_objects_for_project.c lighting setup
//  */
// void SetupLighting()
// {
//    if (light)
//    {
//       float Ambient[]  = {0.3, 0.3, 0.3, 1.0};
//       float Diffuse[]  = {0.8, 0.8, 0.8, 1.0};
//       float Specular[] = {0.5, 0.5, 0.5, 1.0};
//       float Position[] = {5 * Cos(zh), ylight, 5 * Sin(zh), 0.0};  // Directional light (sun)
      
//       glEnable(GL_NORMALIZE);
//       glEnable(GL_LIGHTING);
//       glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
//       glEnable(GL_COLOR_MATERIAL);
//       glEnable(GL_LIGHT0);
      
//       glLightfv(GL_LIGHT0, GL_AMBIENT,  Ambient);
//       glLightfv(GL_LIGHT0, GL_DIFFUSE,  Diffuse);
//       glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
//       glLightfv(GL_LIGHT0, GL_POSITION, Position);
//    }
//    else
//    {
//       glDisable(GL_LIGHTING);
//    }
// }

// /*
//  * OpenGL display function
//  */
// void display()
// {
//    // Clear the window
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glEnable(GL_DEPTH_TEST);
   
//    // Set up projection with proper clipping planes
//    SetupProjection();
   
//    // Set up camera
//    SetupCamera();
   
//    // Set up lighting
//    SetupLighting();
   
//    // Draw terrain
//    DrawTerrain();
   
//    // Draw player model in third-person view
//    if (!viewMode)
//    {
//       DrawPlayer();
//    }
   
//    // Draw axes for debugging
//    if (axes)
//    {
//       glDisable(GL_LIGHTING);
//       glColor3f(1, 1, 1);
//       glBegin(GL_LINES);
//       glVertex3d(0, 0, 0);
//       glVertex3d(5, 0, 0);
//       glVertex3d(0, 0, 0);
//       glVertex3d(0, 5, 0);
//       glVertex3d(0, 0, 0);
//       glVertex3d(0, 0, 5);
//       glEnd();
      
//       glRasterPos3d(5, 0, 0);
//       Print("X");
//       glRasterPos3d(0, 5, 0);
//       Print("Y");
//       glRasterPos3d(0, 0, 5);
//       Print("Z");
//    }
   
//    // Display HUD info
//    glDisable(GL_LIGHTING);
//    glColor3f(1, 1, 1);
//    glWindowPos2i(5, 5);
//    Print("Position: (%.1f, %.1f, %.1f) Yaw: %.1f Pitch: %.1f", 
//          playerX, playerY, playerZ, playerYaw, playerPitch);
//    glWindowPos2i(5, 25);
//    Print("View: %s | Mouse Look: %s | Light: %s",
//          viewMode ? "FPP" : "Third-Person", mouseLook ? "ON" : "OFF", light ? "ON" : "OFF");
//    glWindowPos2i(5, 45);
//    Print("WASD: Move | Space: Jump | Mouse: Look | V: Toggle View | M: Toggle Mouse | L: Light");
   
//    // Check for errors and swap buffers
//    ErrCheck("display");
//    glFlush();
//    glutSwapBuffers();
// }

// /*
//  * Idle function for continuous updates
//  */
// void idle()
// {
//    static int lastTime = 0;
//    int currentTime = glutGet(GLUT_ELAPSED_TIME);
//    double dt = (currentTime - lastTime) / 1000.0;
//    lastTime = currentTime;
   
//    if (dt > 0.1) dt = 0.1;  // Cap dt to prevent huge jumps
   
//    UpdatePlayer(dt);
   
//    // Update light position (orbiting sun)
//    double t = currentTime / 1000.0;
//    zh = fmod(10 * t, 360.0);  // Slower sun movement
   
//    glutPostRedisplay();
// }

// /*
//  * Handle keyboard input
//  */
// void keyboard(unsigned char key, int x, int y)
// {
//    if (key == 27)  // ESC
//       exit(0);
//    else if (key == 'w' || key == 'W')
//       moveForward = true;
//    else if (key == 's' || key == 'S')
//       moveBackward = true;
//    else if (key == 'a' || key == 'A')
//       moveLeft = true;
//    else if (key == 'd' || key == 'D')
//       moveRight = true;
//    else if (key == ' ' && !isJumping)  // Space bar - jump
//    {
//       isJumping = true;
//       playerVelY = JUMP_VELOCITY;
//    }
//    else if (key == 'v' || key == 'V')
//       viewMode = 1 - viewMode;
//    else if (key == 'm' || key == 'M')
//    {
//       mouseLook = 1 - mouseLook;
//       if (mouseLook)
//          glutSetCursor(GLUT_CURSOR_NONE);
//       else
//          glutSetCursor(GLUT_CURSOR_INHERIT);
//    }
//    else if (key == 'l' || key == 'L')
//       light = 1 - light;
//    else if (key == 'x' || key == 'X')
//       axes = 1 - axes;
   
//    glutPostRedisplay();
// }

// /*
//  * Handle keyboard release
//  */
// void keyboardUp(unsigned char key, int x, int y)
// {
//    if (key == 'w' || key == 'W')
//       moveForward = false;
//    else if (key == 's' || key == 'S')
//       moveBackward = false;
//    else if (key == 'a' || key == 'A')
//       moveLeft = false;
//    else if (key == 'd' || key == 'D')
//       moveRight = false;
// }

// /*
//  * Handle mouse motion for looking around
//  * Smooth mouse look implementation
//  */
// void mouseMotion(int x, int y)
// {
//    if (!mouseLook) return;
   
//    if (firstMouse)
//    {
//       lastMouseX = x;
//       lastMouseY = y;
//       firstMouse = false;
//       return;
//    }
   
//    // Calculate mouse movement delta
//    int deltaX = x - lastMouseX;
//    int deltaY = y - lastMouseY;
   
//    // Ignore huge jumps (from warping)
//    if (abs(deltaX) > 200 || abs(deltaY) > 200)
//    {
//       lastMouseX = x;
//       lastMouseY = y;
//       return;
//    }
   
//    lastMouseX = x;
//    lastMouseY = y;
   
//    // Update player rotation with smoother sensitivity
//    playerYaw += deltaX * mouseSensitivity;
//    playerPitch -= deltaY * mouseSensitivity;
   
//    // Clamp pitch to prevent camera flipping
//    if (playerPitch > 89.0) playerPitch = 89.0;
//    if (playerPitch < -89.0) playerPitch = -89.0;
   
//    // Wrap yaw
//    while (playerYaw > 360.0) playerYaw -= 360.0;
//    while (playerYaw < 0.0) playerYaw += 360.0;
   
//    // Re-center mouse to prevent hitting screen edges (smoother threshold)
//    if (x < 100 || x > windowWidth - 100 || y < 100 || y > windowHeight - 100)
//    {
//       int centerX = windowWidth / 2;
//       int centerY = windowHeight / 2;
//       glutWarpPointer(centerX, centerY);
//       lastMouseX = centerX;
//       lastMouseY = centerY;
//    }
   
//    glutPostRedisplay();
// }

// /*
//  * Handle passive mouse motion (when no button is pressed)
//  */
// void passiveMouseMotion(int x, int y)
// {
//    mouseMotion(x, y);
// }

// /*
//  * Handle window reshape
//  */
// void reshape(int width, int height)
// {
//    windowWidth = width;
//    windowHeight = height;
//    asp = (height > 0) ? (double)width / height : 1;
//    glViewport(0, 0, width, height);
//    SetupProjection();
// }

// /*
//  * Main program
//  */
// int main(int argc, char* argv[])
// {
//    // Initialize GLUT
//    glutInit(&argc, argv);
//    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
//    glutInitWindowSize(windowWidth, windowHeight);
//    glutCreateWindow("Lunar Outpost - Alien Invasion Survival");
   
// #ifdef USEGLEW
//    if (glewInit() != GLEW_OK) Fatal("Error initializing GLEW\n");
// #endif
   
//    // Initialize terrain
//    InitTerrain();
   
//    // Set player starting position (slightly above terrain)
//    playerY = GetTerrainHeight(playerX, playerZ) + playerHeight;
   
//    // Set callbacks
//    glutDisplayFunc(display);
//    glutReshapeFunc(reshape);
//    glutKeyboardFunc(keyboard);
//    glutKeyboardUpFunc(keyboardUp);
//    glutMotionFunc(mouseMotion);
//    glutPassiveMotionFunc(passiveMouseMotion);
//    glutIdleFunc(idle);
   
//    // Hide cursor for FPS mode
//    glutSetCursor(GLUT_CURSOR_NONE);
   
//    // Set projection
//    SetupProjection();
   
//    Print("Lunar Outpost initialized\n");
//    Print("Controls: WASD=Move, Space=Jump, Mouse=Look, V=View, M=Mouse, L=Light\n");
   
//    // Start main loop
//    ErrCheck("init");
//    glutMainLoop();
   
//    return 0;
// }



/*
 *  Lunar Outpost - Alien Invasion Survival (Year 6060)
 *  
 *  A first-person survival game set on the Moon with artistic environment
 *  Featuring: Giant orange sun, starfield, rolling lunar dunes, mysterious patches
 *  
 *  Key bindings:
 *  WASD       Move forward/left/backward/right
 *  Space      Jump
 *  Mouse      Look around (FPP)
 *  v/V        Toggle FPP/Third-person debug view
 *  l/L        Toggle lighting
 *  m/M        Toggle mouse look
 *  ESC        Exit
 */

#include "CSCIx229.h"
#include <stdbool.h>

// Window dimensions
int windowWidth = 1200;
int windowHeight = 800;

// Camera and view state
int axes = 0;           // Display axes for debugging
int light = 1;          // Lighting enabled
int viewMode = 1;       // 1=FPP, 0=Third-person debug
int mouseLook = 1;      // Mouse look enabled
double asp = 1.0;       // Aspect ratio
double fov = 70;        // Field of view (wider for dramatic effect)

// Player position and movement
double playerX = 0.0;
double playerY = 2.0;   // Height above ground
double playerZ = 30.0;  // Start further back to see scene
double playerVelY = 0.0;
double playerHeight = 1.8;

// Player rotation (angles in degrees)
double playerYaw = 0.0;
double playerPitch = 0.0;

// Movement state
bool moveForward = false;
bool moveBackward = false;
bool moveLeft = false;
bool moveRight = false;
bool isJumping = false;

// Mouse control
int lastMouseX = 0;
int lastMouseY = 0;
bool firstMouse = true;
double mouseSensitivity = 0.05;

// Physics constants
const double MOON_GRAVITY = 1.62;
const double MOVE_SPEED = 0.15;
const double JUMP_VELOCITY = 5.0;

// Sun/lighting animation
double sunTime = 0.0;
int animateSun = 0;  // Toggle sun animation

// Terrain data
#define TERRAIN_SIZE 65
#define TERRAIN_SCALE 8.0  // Larger scale for rolling dunes
float terrainZ[TERRAIN_SIZE][TERRAIN_SIZE];
float terrainNormals[TERRAIN_SIZE][TERRAIN_SIZE][3];

// Star positions (random, fixed)
#define NUM_STARS 20
typedef struct {
   float x, y, z;
   float size;
   float brightness;
} Star;
Star stars[NUM_STARS];

/*
 * Initialize starfield
 * Random positions in the sky, varying sizes
 */
void InitStarfield()
{
   srand(42);  // Fixed seed for consistent stars
   for (int i = 0; i < NUM_STARS; i++)
   {
      // Position stars in a hemisphere above
      float theta = (float)rand() / RAND_MAX * 360.0;
      float phi = (float)rand() / RAND_MAX * 60.0 + 20.0;  // Upper hemisphere
      float dist = 400.0;  // Far away
      
      stars[i].x = dist * Sin(theta) * Cos(phi);
      stars[i].y = dist * Sin(phi);
      stars[i].z = dist * Cos(theta) * Cos(phi);
      stars[i].size = 1.0 + (float)rand() / RAND_MAX * 3.0;
      stars[i].brightness = 0.6 + (float)rand() / RAND_MAX * 0.4;
   }
}

/*
 * Draw starfield
 * Reference: ex19.c starfield rendering
 */
void DrawStarfield()
{
   glDisable(GL_LIGHTING);
   glColor3f(1, 1, 1);
   
   for (int i = 0; i < NUM_STARS; i++)
   {
      glPointSize(stars[i].size);
      glColor3f(stars[i].brightness, stars[i].brightness, stars[i].brightness);
      
      glBegin(GL_POINTS);
      glVertex3f(stars[i].x, stars[i].y, stars[i].z);
      glEnd();
   }
}

/*
 * Draw vertex in polar coordinates (for sphere)
 * Reference: ex19.c planet rendering
 */
static void Vertex(int th, int ph)
{
   double x = Sin(th) * Cos(ph);
   double y = Cos(th) * Cos(ph);
   double z = Sin(ph);
   glNormal3d(x, y, z);
   glVertex3d(x, y, z);
}

/*
 * Draw the giant sun with surface texture variations
 * Reference: ex19.c planet sphere rendering
 */
void DrawSun(double x, double y, double z, double radius)
{
   // Sun emits its own light
   float emission[] = {1.0, 0.4, 0.1, 1.0};  // Orange glow
   float ambient[] = {1.0, 0.4, 0.1, 1.0};
   
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, ambient);
   
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(radius, radius, radius);
   
   // Draw sphere with varying colors for surface detail
   // Use latitude bands like ex19.c
   for (int ph = -90; ph < 90; ph += 10)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th = 0; th <= 360; th += 10)
      {
         // Vary color slightly for surface features
         float variation = 0.8 + 0.2 * sin(th * 0.1) * cos(ph * 0.1);
         glColor3f(1.0 * variation, 0.4 * variation, 0.1 * variation);
         
         Vertex(th, ph);
         Vertex(th, ph + 10);
      }
      glEnd();
   }
   
   // Reset emission
   float noEmission[] = {0, 0, 0, 1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
   
   glPopMatrix();
}

/*
 * Initialize lunar terrain with smooth rolling dunes
 * Reference: ex18.c terrain generation
 */
void InitTerrain()
{
   srand(12345);
   
   for (int i = 0; i < TERRAIN_SIZE; i++)
   {
      for (int j = 0; j < TERRAIN_SIZE; j++)
      {
         double x = (i - TERRAIN_SIZE/2.0) * TERRAIN_SCALE;
         double z = (j - TERRAIN_SIZE/2.0) * TERRAIN_SCALE;
         
         // Create smooth, rolling dunes (artistic, painterly look)
         terrainZ[i][j] = 
            3.0 * sin(x * 0.02) * cos(z * 0.03) +
            2.0 * sin(x * 0.04 + 1.5) * sin(z * 0.04) +
            1.5 * cos(x * 0.05) * cos(z * 0.05 + 2.0) +
            0.5 * sin(x * 0.1) * cos(z * 0.1);
      }
   }
   
   // Compute normals (reference: ex18.c normal calculation)
   for (int i = 0; i < TERRAIN_SIZE; i++)
   {
      for (int j = 0; j < TERRAIN_SIZE; j++)
      {
         // X component
         if (i == 0)
            terrainNormals[i][j][0] = terrainZ[i][j] - terrainZ[i+1][j];
         else if (i == TERRAIN_SIZE-1)
            terrainNormals[i][j][0] = terrainZ[i-1][j] - terrainZ[i][j];
         else
            terrainNormals[i][j][0] = (terrainZ[i-1][j] - terrainZ[i+1][j]) / 2.0;
         
         // Y component
         if (j == 0)
            terrainNormals[i][j][1] = terrainZ[i][j] - terrainZ[i][j+1];
         else if (j == TERRAIN_SIZE-1)
            terrainNormals[i][j][1] = terrainZ[i][j-1] - terrainZ[i][j];
         else
            terrainNormals[i][j][1] = (terrainZ[i][j-1] - terrainZ[i][j+1]) / 2.0;
         
         // Z component is grid spacing
         terrainNormals[i][j][2] = TERRAIN_SCALE;
      }
   }
}

/*
 * Get terrain height at world position
 */
double GetTerrainHeight(double worldX, double worldZ)
{
   double gridX = (worldX / TERRAIN_SCALE) + TERRAIN_SIZE / 2.0;
   double gridZ = (worldZ / TERRAIN_SCALE) + TERRAIN_SIZE / 2.0;
   
   if (gridX < 0 || gridX >= TERRAIN_SIZE-1 || gridZ < 0 || gridZ >= TERRAIN_SIZE-1)
      return 0.0;
   
   int x0 = (int)gridX;
   int z0 = (int)gridZ;
   double fx = gridX - x0;
   double fz = gridZ - z0;
   
   // Bilinear interpolation
   double h00 = terrainZ[x0][z0];
   double h10 = terrainZ[x0+1][z0];
   double h01 = terrainZ[x0][z0+1];
   double h11 = terrainZ[x0+1][z0+1];
   
   double h0 = h00 * (1-fx) + h10 * fx;
   double h1 = h01 * (1-fx) + h11 * fx;
   
   return h0 * (1-fz) + h1 * fz;
}

/*
 * Draw the lunar terrain with beige/tan coloring
 * Reference: ex18.c terrain rendering
 */
void DrawTerrain()
{
   // Lunar dust color - beige/tan
   glColor3f(0.85, 0.80, 0.70);
   
   // Material properties for matte surface
   float diffuse[] = {0.85, 0.80, 0.70, 1.0};
   float specular[] = {0.1, 0.1, 0.1, 1.0};  // Very low specular (matte)
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffuse);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 2.0);
   
   // Draw terrain as quads
   for (int i = 0; i < TERRAIN_SIZE-1; i++)
   {
      for (int j = 0; j < TERRAIN_SIZE-1; j++)
      {
         double x = (i - TERRAIN_SIZE/2) * TERRAIN_SCALE;
         double z = (j - TERRAIN_SIZE/2) * TERRAIN_SCALE;
         
         glBegin(GL_QUADS);
         
         glNormal3fv(terrainNormals[i][j]);
         glVertex3d(x, terrainZ[i][j], z);
         
         glNormal3fv(terrainNormals[i+1][j]);
         glVertex3d(x + TERRAIN_SCALE, terrainZ[i+1][j], z);
         
         glNormal3fv(terrainNormals[i+1][j+1]);
         glVertex3d(x + TERRAIN_SCALE, terrainZ[i+1][j+1], z + TERRAIN_SCALE);
         
         glNormal3fv(terrainNormals[i][j+1]);
         glVertex3d(x, terrainZ[i][j+1], z + TERRAIN_SCALE);
         
         glEnd();
      }
   }
}

/*
 * Draw an irregular organic patch (alien vegetation or contamination)
 * Reference: my_objects_for_project.c triangle fan approach
 */
void DrawPatch(double x, double y, double z, double size, int seed)
{
   srand(seed);
   
   // Dark green-gray color
   glColor3f(0.2, 0.3, 0.25);
   float patchColor[] = {0.2, 0.3, 0.25, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, patchColor);
   
   glPushMatrix();
   glTranslated(x, y + 0.01, z);  // Slightly above terrain
   glRotated(-90, 1, 0, 0);  // Flat on ground
   
   // Draw irregular blob using triangle fan
   int numPoints = 16;
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0, 0, 1);
   glVertex3d(0, 0, 0);  // Center
   
   for (int i = 0; i <= numPoints; i++)
   {
      float angle = (float)i / numPoints * 360.0;
      // Irregular radius with random variation
      float radius = size * (0.7 + 0.3 * (float)rand() / RAND_MAX);
      float px = radius * Cos(angle);
      float py = radius * Sin(angle);
      glVertex3d(px, py, 0);
   }
   glEnd();
   
   // Add small highlight dots
   glColor3f(0.9, 0.9, 0.8);
   float highlightEmission[] = {0.3, 0.3, 0.3, 1.0};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, highlightEmission);
   
   for (int i = 0; i < 2; i++)
   {
      float dx = (float)rand() / RAND_MAX * size * 0.3 - size * 0.15;
      float dy = (float)rand() / RAND_MAX * size * 0.3 - size * 0.15;
      glPushMatrix();
      glTranslated(dx, dy, 0.02);
      glutSolidSphere(0.1, 6, 6);
      glPopMatrix();
   }
   
   // Reset emission
   float noEmission[] = {0, 0, 0, 1};
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
   
   glPopMatrix();
}

/*
 * Update player physics
 */
void UpdatePlayer(double dt)
{
   // Apply moon gravity
   if (isJumping || playerY > GetTerrainHeight(playerX, playerZ) + playerHeight + 0.01)
   {
      playerVelY -= MOON_GRAVITY * dt;
      playerY += playerVelY * dt;
      
      double groundHeight = GetTerrainHeight(playerX, playerZ) + playerHeight;
      if (playerY <= groundHeight)
      {
         playerY = groundHeight;
         playerVelY = 0.0;
         isJumping = false;
      }
   }
   
   // Movement
   double moveX = 0.0;
   double moveZ = 0.0;
   
   if (moveForward)
   {
      moveX += sin(playerYaw * M_PI / 180.0) * MOVE_SPEED;
      moveZ += cos(playerYaw * M_PI / 180.0) * MOVE_SPEED;
   }
   if (moveBackward)
   {
      moveX -= sin(playerYaw * M_PI / 180.0) * MOVE_SPEED;
      moveZ -= cos(playerYaw * M_PI / 180.0) * MOVE_SPEED;
   }
   if (moveLeft)
   {
      moveX -= cos(playerYaw * M_PI / 180.0) * MOVE_SPEED;
      moveZ += sin(playerYaw * M_PI / 180.0) * MOVE_SPEED;
   }
   if (moveRight)
   {
      moveX += cos(playerYaw * M_PI / 180.0) * MOVE_SPEED;
      moveZ -= sin(playerYaw * M_PI / 180.0) * MOVE_SPEED;
   }
   
   playerX += moveX;
   playerZ += moveZ;
   
   // Boundary checking
   double maxTerrainX = (TERRAIN_SIZE / 2.0 - 2) * TERRAIN_SCALE;
   double maxTerrainZ = (TERRAIN_SIZE / 2.0 - 2) * TERRAIN_SCALE;
   
   if (playerX > maxTerrainX) playerX = maxTerrainX;
   if (playerX < -maxTerrainX) playerX = -maxTerrainX;
   if (playerZ > maxTerrainZ) playerZ = maxTerrainZ;
   if (playerZ < -maxTerrainZ) playerZ = -maxTerrainZ;
   
   // Keep above terrain
   if (!isJumping)
   {
      double groundHeight = GetTerrainHeight(playerX, playerZ) + playerHeight;
      if (playerY < groundHeight)
         playerY = groundHeight;
   }
}

/*
 * Set up projection
 */
void SetupProjection()
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(fov, asp, 0.1, 600.0);
   glMatrixMode(GL_MODELVIEW);
}

/*
 * Set up camera
 */
void SetupCamera()
{
   glLoadIdentity();
   
   if (viewMode)  // FPP
   {
      double lookX = sin(playerYaw * M_PI / 180.0) * cos(playerPitch * M_PI / 180.0);
      double lookY = sin(playerPitch * M_PI / 180.0);
      double lookZ = cos(playerYaw * M_PI / 180.0) * cos(playerPitch * M_PI / 180.0);
      
      gluLookAt(playerX, playerY, playerZ,
                playerX + lookX, playerY + lookY, playerZ + lookZ,
                0, 1, 0);
   }
   else  // Third-person debug
   {
      double camDist = 8.0;
      double camHeight = 5.0;
      
      double camX = playerX - sin(playerYaw * M_PI / 180.0) * camDist;
      double camY = playerY + camHeight;
      double camZ = playerZ - cos(playerYaw * M_PI / 180.0) * camDist;
      
      gluLookAt(camX, camY, camZ,
                playerX, playerY, playerZ,
                0, 1, 0);
   }
}

/*
 * Setup scene lighting with warm orange sun tint
 * Reference: ex22.c advanced lighting
 */
void SetupLighting()
{
   if (light)
   {
      // Warm orange tint from the sun
      float Ambient[]  = {0.15, 0.12, 0.10, 1.0};
      float Diffuse[]  = {0.9, 0.6, 0.4, 1.0};
      float Specular[] = {0.5, 0.3, 0.2, 1.0};
      
      // Directional light from sun position (left side)
      float sunAngle = animateSun ? sunTime * 5.0 : -30.0;
      float Position[] = {-1.0 * Cos(sunAngle), 0.3, -0.5 * Sin(sunAngle), 0.0};
      
      glEnable(GL_NORMALIZE);
      glEnable(GL_LIGHTING);
      glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      glEnable(GL_LIGHT0);
      
      glLightfv(GL_LIGHT0, GL_AMBIENT,  Ambient);
      glLightfv(GL_LIGHT0, GL_DIFFUSE,  Diffuse);
      glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
      glLightfv(GL_LIGHT0, GL_POSITION, Position);
      
      // Enable smooth shading for painterly look
      glShadeModel(GL_SMOOTH);
   }
   else
   {
      glDisable(GL_LIGHTING);
   }
}

/*
 * Draw a simple player model (third-person view)
 */
void DrawPlayer()
{
   glPushMatrix();
   glTranslated(playerX, playerY, playerZ);
   
   glColor3f(0.2, 0.8, 0.2);
   
   // Body
   glPushMatrix();
   glScaled(0.3, playerHeight/2, 0.3);
   glTranslated(0, 1, 0);
   glutSolidCube(1.0);
   glPopMatrix();
   
   // Head
   glPushMatrix();
   glTranslated(0, playerHeight, 0);
   glutSolidSphere(0.2, 8, 8);
   glPopMatrix();
   
   // Direction indicator
   glDisable(GL_LIGHTING);
   glColor3f(1, 0, 0);
   glBegin(GL_LINES);
   glVertex3d(0, playerHeight/2, 0);
   glVertex3d(sin(playerYaw * M_PI/180) * 0.5, playerHeight/2, cos(playerYaw * M_PI/180) * 0.5);
   glEnd();
   
   glPopMatrix();
}

/*
 * Main display function
 */
void display()
{
   // Clear with black (space)
   glClearColor(0.0, 0.0, 0.0, 1.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);
   
   SetupProjection();
   SetupCamera();
   
   // Draw starfield (no lighting)
   DrawStarfield();
   
   // Draw the giant sun (with self-illumination)
   // Positioned to the left and behind, partially visible
   glDisable(GL_LIGHTING);
   DrawSun(-150, 60, -180, 80);
   
   // Enable lighting for terrain
   SetupLighting();
   
   // Draw terrain
   DrawTerrain();
   
   // Draw mysterious patches on terrain
   // Large patch on the left
   DrawPatch(-15, GetTerrainHeight(-15, 10), 10, 4.0, 100);
   // Smaller patch on the right
   DrawPatch(20, GetTerrainHeight(20, 15), 15, 2.5, 200);
   
   // Draw player in third-person mode
   if (!viewMode)
   {
      glEnable(GL_LIGHTING);
      DrawPlayer();
   }
   
   // Draw axes for debugging
   if (axes)
   {
      glDisable(GL_LIGHTING);
      glColor3f(1, 1, 1);
      glBegin(GL_LINES);
      glVertex3d(0, 0, 0); glVertex3d(5, 0, 0);
      glVertex3d(0, 0, 0); glVertex3d(0, 5, 0);
      glVertex3d(0, 0, 0); glVertex3d(0, 0, 5);
      glEnd();
      glRasterPos3d(5, 0, 0); Print("X");
      glRasterPos3d(0, 5, 0); Print("Y");
      glRasterPos3d(0, 0, 5); Print("Z");
   }
   
   // HUD
   glDisable(GL_LIGHTING);
   glColor3f(1, 1, 1);
   glWindowPos2i(5, 5);
   Print("Position: (%.1f, %.1f, %.1f) Yaw: %.1f Pitch: %.1f", 
         playerX, playerY, playerZ, playerYaw, playerPitch);
   glWindowPos2i(5, 25);
   Print("View: %s | Mouse: %s | Light: %s | Sun Anim: %s",
         viewMode ? "FPP" : "Third-Person", 
         mouseLook ? "ON" : "OFF", 
         light ? "ON" : "OFF",
         animateSun ? "ON" : "OFF");
   glWindowPos2i(5, 45);
   Print("WASD:Move Space:Jump Mouse:Look V:View M:Mouse L:Light X:Axes T:Sun");
   
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 * Idle function
 */
void idle()
{
   static int lastTime = 0;
   int currentTime = glutGet(GLUT_ELAPSED_TIME);
   double dt = (currentTime - lastTime) / 1000.0;
   lastTime = currentTime;
   
   if (dt > 0.1) dt = 0.1;
   
   UpdatePlayer(dt);
   
   if (animateSun)
      sunTime = currentTime / 1000.0;
   
   glutPostRedisplay();
}

/*
 * Keyboard input
 */
void keyboard(unsigned char key, int x, int y)
{
   if (key == 27)
      exit(0);
   else if (key == 'w' || key == 'W')
      moveForward = true;
   else if (key == 's' || key == 'S')
      moveBackward = true;
   else if (key == 'a' || key == 'A')
      moveLeft = true;
   else if (key == 'd' || key == 'D')
      moveRight = true;
   else if (key == ' ' && !isJumping)
   {
      isJumping = true;
      playerVelY = JUMP_VELOCITY;
   }
   else if (key == 'v' || key == 'V')
      viewMode = 1 - viewMode;
   else if (key == 'm' || key == 'M')
   {
      mouseLook = 1 - mouseLook;
      if (mouseLook)
         glutSetCursor(GLUT_CURSOR_NONE);
      else
         glutSetCursor(GLUT_CURSOR_INHERIT);
   }
   else if (key == 'l' || key == 'L')
      light = 1 - light;
   else if (key == 'x' || key == 'X')
      axes = 1 - axes;
   else if (key == 't' || key == 'T')
      animateSun = 1 - animateSun;
   
   glutPostRedisplay();
}

/*
 * Keyboard release
 */
void keyboardUp(unsigned char key, int x, int y)
{
   if (key == 'w' || key == 'W')
      moveForward = false;
   else if (key == 's' || key == 'S')
      moveBackward = false;
   else if (key == 'a' || key == 'A')
      moveLeft = false;
   else if (key == 'd' || key == 'D')
      moveRight = false;
}

/*
 * Mouse motion for looking
 */
void mouseMotion(int x, int y)
{
   if (!mouseLook) return;
   
   if (firstMouse)
   {
      lastMouseX = x;
      lastMouseY = y;
      firstMouse = false;
      return;
   }
   
   int deltaX = x - lastMouseX;
   int deltaY = y - lastMouseY;
   
   // Ignore huge jumps
   if (abs(deltaX) > 200 || abs(deltaY) > 200)
   {
      lastMouseX = x;
      lastMouseY = y;
      return;
   }
   
   lastMouseX = x;
   lastMouseY = y;
   
   playerYaw += deltaX * mouseSensitivity;
   playerPitch -= deltaY * mouseSensitivity;
   
   // Clamp pitch
   if (playerPitch > 89.0) playerPitch = 89.0;
   if (playerPitch < -89.0) playerPitch = -89.0;
   
   // Wrap yaw
   while (playerYaw > 360.0) playerYaw -= 360.0;
   while (playerYaw < 0.0) playerYaw += 360.0;
   
   // Re-center if near edges
   if (x < 100 || x > windowWidth - 100 || y < 100 || y > windowHeight - 100)
   {
      int centerX = windowWidth / 2;
      int centerY = windowHeight / 2;
      glutWarpPointer(centerX, centerY);
      lastMouseX = centerX;
      lastMouseY = centerY;
   }
   
   glutPostRedisplay();
}

void passiveMouseMotion(int x, int y)
{
   mouseMotion(x, y);
}

/*
 * Window reshape
 */
void reshape(int width, int height)
{
   windowWidth = width;
   windowHeight = height;
   asp = (height > 0) ? (double)width / height : 1;
   glViewport(0, 0, width, height);
   SetupProjection();
}

/*
 * Main
 */
int main(int argc, char* argv[])
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(windowWidth, windowHeight);
   glutCreateWindow("Lunar Outpost - Year 6060");
   
#ifdef USEGLEW
   if (glewInit() != GLEW_OK) Fatal("Error initializing GLEW\n");
#endif
   
   // Initialize scene
   InitTerrain();
   InitStarfield();
   
   // Set player starting position
   playerY = GetTerrainHeight(playerX, playerZ) + playerHeight;
   
   // Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutKeyboardUpFunc(keyboardUp);
   glutMotionFunc(mouseMotion);
   glutPassiveMotionFunc(passiveMouseMotion);
   glutIdleFunc(idle);
   
   glutSetCursor(GLUT_CURSOR_NONE);
   SetupProjection();
   
   Print("=== Lunar Outpost - Year 6060 ===\n");
   Print("Artistic lunar landscape with giant sun loaded\n");
   Print("Controls: WASD=Move, Space=Jump, Mouse=Look\n");
   Print("V=Toggle View, M=Mouse, L=Light, T=Animate Sun\n");
   
   ErrCheck("init");
   glutMainLoop();
   
   return 0;
}