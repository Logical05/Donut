#include <cmath>
#include <cstring>
#include <iostream>
#include <unistd.h>
using namespace std;

int main() 
{
    int   width = 80, height = 22;
    float A = M_PI_2, B = 0;
    int   aspectRatio[2] = {16, 9};
    char  display[width * height];
    /*
        Stores the closeness between the xy coordinates and the viewer; large values indicate closeness
    */
    float zBuffer[width * height];
    for (;;)
    {
        memset(display, 32, (width * height));
        memset(zBuffer, 0,  (width * height * 4));
        for (float theta = 0; theta < (2 * M_PI); theta += 7e-2F)
        for (float phi   = 0; phi   < (2 * M_PI); phi   += 2e-2F)
        {
            float sinTheta = sin(theta), cosTheta = cos(theta);
            float sinPhi   = sin(phi),   cosPhi   = cos(phi);
            float sinX     = sin(A),     cosX     = cos(A);
            float sinZ     = sin(B),     cosZ     = cos(B);
            /*
                R1 is the radius of the circle = 1
                R2 is the distance between the axis and the center of the circle = 2
                                                       ⎛cosϕ  0  −sinϕ⎞ ⎛ 1   0     0  ⎞ ⎛cosB −sinB  0 ⎞
                (x, y, z) = (R2 + R1cosθ, R1sinθ, 0) ⋅ ⎜ 0    1    0  ⎟⋅⎜ 0  cosA −sinA⎟⋅⎜sinB  cosB  0 ⎟
                                                       ⎝sinϕ  0   cosϕ⎠ ⎝ 0  sinA  cosA⎠ ⎝ 0     0    1 ⎠
                            ⎿_____________________⏌   ⎿_______________________________________________⏌
                                      Circle                             Rotation Matrix
            */
            #define Xcal(x) (x * cosPhi * cosZ - sinTheta * cosX * sinZ + x * sinPhi * sinX * sinZ)
            #define Ycal(x) (x * cosPhi * sinZ + sinTheta * cosX * cosZ - x * sinPhi * sinX * cosZ)
            #define Zcal(x) (x * sinPhi * cosX + sinTheta * sinX)
            float circleX = 2 + cosTheta;   // R2 + R1cosθ
            float finalX  = Xcal(circleX);  // x
            float finalY  = Ycal(circleX);  // y
            float finalZ  = Zcal(circleX);  // z
            /*
                Similar Triangles
                - y  is the height of the object point
                - y' is the height of the object point on the screen (terminal)
                - z  is the distance from the screen to the object
                - z' is the distance from the screen to the viewer = 1

                    y'    y  ⎟        y                     ⎟  ∴  y' = yZ
                    ─  =  ─  ⎟  y' =  ─  =  yz⁻¹  ∵ z' = 1  ⎟     x' = xZ
                    z'    z  ⎟        z                     ⎟ 
                             ⎟           =  yZ    ∵ Z = z⁻¹ ⎟
                  ───────────┴──────────────────────────────┴──────────────
                           1        - FAR is a value that can be adjusted according to the desired distance
                    Z = ───────
                        z + FAR
            */
            float Z = 1 / (finalZ + 4);
            /*
                Multiply the value by the aspect ratio value. Then increase the value
                so that the center of the donut is in the middle of the terminal
            */
            int displayX = (width  / 2) + aspectRatio[0] * finalX * Z;  // Column index of a 2D display
            /*                                            ⎿_________⏌
                                                            x' = xZ
            */
            int displayY = (height / 2) + aspectRatio[1] * finalY * Z;  // Row index of a 2D display
            /*                                            ⎿_________⏌
                                                            y' = yZ
            */
            int Index    = displayX + width * displayY;  // Convert 2D index to 1D index
            /*
                                                 ⎛cosϕ  0  −sinϕ⎞ ⎛ 1   0     0  ⎞ ⎛cosB −sinB  0 ⎞
                (Nx, Ny, Nz) = (cosθ, sinθ, 0) ⋅ ⎜ 0    1    0  ⎟⋅⎜ 0  cosA −sinA⎟⋅⎜sinB  cosB  0 ⎟
                                                 ⎝sinϕ  0   cosϕ⎠ ⎝ 0  sinA  cosA⎠ ⎝ 0     0    1 ⎠

                         Lum = (Nx, Ny, Nz)  ⋅  (0, -1, −1)  // Dot Product 
                              ⎿__________⏌    ⎿_________⏌ 
                              Surface Normal   Light Direction (Light up surfaces facing behind and above the viewer)
            */
            int L = 8 * (- Ycal(cosTheta) - Zcal(cosTheta));  // Multiply by 8 to have a range of 0-11
            if (0 < displayX && displayX < width && 0 < displayY && displayY < height && Z > zBuffer[Index])
            {   
                zBuffer[Index] = Z;
                display[Index] = ".,-~:;=!*#$@"[L > 0 ? L : 0];
            }
            #undef Xcal
            #undef Ycal
            #undef Zcal
        }
        cout << "\x1b[2J\x1b[H";  // Clear entire screen and moves cursor to upper left
        for (int i = 0; i <= (width * height); i++)
        {
            // Display CHAR one at a time until it equals the width of the display and then a new line
            putchar(i % width ? display[i] : 10);
        }
        A += 4e-2F;
        B += 2e-2f;
        usleep(30000);
    }
    return 0;
}