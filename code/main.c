#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "raylib.h"

#include "ffmpeg.h"
#include "useful.h"

#define WIDTH 500
#define HEIGHT 1500
#define FPS 60
const double delTime = (1.0f/FPS);

// --------------- FOREGROUND SETTINGS ------------------------------------
const int margin = 0;
const int padding = 5;
const int BOARDMAX = 15; // how many boards can we have
const int margindivide = (HEIGHT-(margin))/BOARDMAX-margin; // boxes length

// --------------- FONT SETTINGS ------------------------------------------
const int fontSize = HEIGHT/BOARDMAX-60;
const int timingSize = fontSize+10;

// --------------- MARBLE SETTINGS ----------------------------------------
const int eliminatedMarbles = 4;

// Marble colors, this is not in order, only for matching
const unsigned long marbleColors[44] = {
    0xFFFFFFFF,0x999999FF,0x666666FF,0x333333FF,0x000000FF, // 5 CORRECT
    0xFF9C9CFF,0xFF5959FF,0xFF0000FF,0x800000FF, // 9 colors, Maroon CORRECT
    0xFFB366FF,0xFF8000FF,0xC46200FF,0x783C00FF, // 13 Brown
    0xFFFF80FF,0xFFEA00FF,0xF2B600FF,0x808000FF, // 17 Olive
    0x54AD28FF, // 18 Fern
    0xAAFF00FF,0x00FF00FF,0x00A100FF, // 21 Green
    0x00FF80FF, // 22 Turquoise 
    0x80FFFFFF,0x00FFFFFF,0x00AAAAFF, // 25 Teal CORRECT
    0x0080FFFF,0x0000FFFF,0x0000AAFF,0x000055FF, // 29 Midnight
    0x968CFFFF, // 30 Periwinkle
    0xB56BFFFF,0x8000FFFF,0x5500AAFF,0x2B0055FF, // 34 Dark Purple
    0xFF70FFFF,0xFF00FFFF,0xAA00AAFF, // 37 Violet
    0xFF6BB5FF,0xFF0080FF,0xAA0055FF,0x55002BFF, // 41 Crimson
    0xB83D7AFF, // 42 Mauve
    0x000000FF,0x000000FF}; // 44 colors (SMILE AND RAINBOW)

const char *marbleNoorder[] = {
    "White","Silver","Gray","Shadow","Black",
    "Salmon","Coral","Red","Maroon",
    "Tan","Orange","Copper","Brown",
    "Ivory","Yellow","Gold","Olive",
    "Fern",
    "Neon Green","Lime","Green",
    "Turquoise",
    "Crystal","Cyan","Teal",
    "Sky","Blue","Navy","Midnight",
    "Periwinkle",
    "Lilac","Lavender","Purple","Dark Purple",
    "Pink","Magenta","Violet",
    "Rose","Hot Pink","Hot Magenta","Crimson",
    "Mauve",
    "Smile","Rainbow"
    
    };

// Here comes the marble names with their times in another list
const char *marbleNames[] = {"Coral", "Hot Pink", "Fern", "Periwinkle", "Gray", "Lilac", "Violet", "Green", "Blue", "Red", "Pink", "Olive", "Teal", "Midnight", "Orange", "Copper", "Rose", "Silver", "Turquoise", "Salmon", "Ivory", "Hot Magenta", "Maroon", "Crystal", "Rainbow", "Yellow", "Neon Green", "White", "Dark Purple", "Tan", "Gold", "Cyan", "Black", "Magenta", "Sky", "Purple", "Brown", "Smile", "Shadow", "Navy", "Lavender", "Lime", "Crimson", "Mauve"};

const float marbleTimes[44] = {61.266663, 62.900009, 74.100006, 75.333328, 92.25, 108.183334, 110.45, 131.3, 137.00002, 153.23335, 154.53334, 155.7, 158.2, 159.65001, 163.93333, 169.2, 181.51668, 194.05, 195.83333, 196.88335, 205.46669, 228.65001, 233.15001, 250.33336, 251.63335, 252.90001, 260.90002, 265.96667, 275.73334, 289.65002, 303.15002, 304.98334, 346.88336, 369.90002, 378.3, 381, 404.68335, 471.8, 491.90002, 612.83337, 678.58337, 769.71667, 883.31665, 1112.2833};

// ---------------------- ANIMATION SETTINGS ------------------------
const int transitionDuration = 50; // frames     why frames? this transition is not supposed to take long, so that why its used frames
const float timerevealDuration = 3; // sec       long transition

int main(void) {
    SetTraceLogLevel(LOG_ERROR);
    
    StartRecording(WIDTH, HEIGHT, FPS);
    
    InitWindow(WIDTH, HEIGHT, "Board Rendering");
    
    Font pFont = LoadFontEx("../font/BaseNeue-WideOblique.ttf", 100, 0, 0); // Placement font
    SetTextureFilter(pFont.texture, TEXTURE_FILTER_BILINEAR);
    Font nFont = LoadFontEx("../font/BaseNeue-SemiBold.ttf", 100, 0, 0); // Namings
    SetTextureFilter(nFont.texture, TEXTURE_FILTER_BILINEAR);
    Font tFont = LoadFontEx("../font/BaseNeue-WideMediumOblique.ttf", 100, 0, 0); // Time
    SetTextureFilter(tFont.texture, TEXTURE_FILTER_BILINEAR);
    
    // -------------------------------- CREATE SOUND ---------------------------------
    
    int sampRate = 44100, sampSize = 32, sampCh = 1;
    
    Wave sound = LoadWave("s.wav");
    WaveFormat(&sound, sampRate, sampSize, sampCh);
    
    Wave mainS = { 0 };
    mainS.frameCount = (sampRate * marbleTimes[44-1]) + sound.frameCount;
    mainS.sampleRate = sampRate;
    mainS.sampleSize = sampSize;
    mainS.channels = sampCh;
    mainS.data = RL_CALLOC(mainS.frameCount, sizeof(float));
    
    for (int i = 0; i < 44; i++) { // get sound starting position
        int startFrame = (int)(marbleTimes[i] * sampRate);
    
        for (int j = 0; j < sound.frameCount; j++) { // insert sfx in wave
            if (startFrame + j < mainS.frameCount) {
                ((float *)mainS.data)[startFrame + j] += ((float *)sound.data)[j];
            }
        }
    }
    
    ExportWave(mainS, "audioresult.wav");
    
    int maxlengthpFont = MeasureTextEx(pFont, "88", fontSize, 1.0f).x;
    int circoffset = ((2*margin+margindivide)/2)+maxlengthpFont+30;
    
    //create images to be used in the namings
    Texture2D namingsTxtures[44]; // pass onto the gpu (textures)
    for (int i = 0; i < 44; i++) {
        Image namingsImg = GenImageColor( MeasureTextEx(nFont,marbleNoorder[i],fontSize,1.0f).x, fontSize, BLANK); // create image (in cpu mem)
        ImageDrawTextEx(&namingsImg, nFont, marbleNoorder[i], (Vector2){0,0}, fontSize, 1.0f, GetColor(grayscaleRGBA(marbleColors[i]^0xFFFFFF00)));
        
        // this is where we convert the image onto textures
        namingsTxtures[i] = LoadTextureFromImage(namingsImg);
        UnloadImage(namingsImg);
    }
    
    // custom marbles textures
    
    // Smile
    Image smileImg = LoadImage("../asset/smile.png");
    Texture2D smileTexture = LoadTextureFromImage(CircleMask(smileImg));
    UnloadImage(smileImg);
    // Rainbow
    Image rainbowImg = LoadImage("../asset/rainbow.jpg");
    Texture2D rainbowTexture = LoadTextureFromImage(CircleMask(rainbowImg));
    UnloadImage(rainbowImg);
    
    SetTargetFPS(0);
    
    RenderTexture2D target = LoadRenderTexture(WIDTH, HEIGHT);
    
    int marbleNumber = 0;
    float leaderboardScroll = 0.0;
    float TleaderboardScroll = 1.0;
    
    // ------------------------------------------------------------------- MAIN LOOP ------------------------------------------------------------
    
        int frame = 0;
    
        for (; frame < (int)(marbleTimes[44-1]*FPS + timerevealDuration*FPS) && !WindowShouldClose(); frame++) {
        
            float videotime = frame * delTime;
            
            if (floor(marbleTimes[marbleNumber] * FPS) == frame) { // comparing if this frame has a marble being safe
                    marbleNumber++; TleaderboardScroll = 0;
            }
            TleaderboardScroll += TleaderboardScroll < 1 ? 1.0/transitionDuration : 0;
            leaderboardScroll = marbleNumber + (1 - pow(1 - TleaderboardScroll, 3)); // big thanks to easings.net for easeoutcubic
            
            BeginTextureMode(target); // begin drawing on texture
            
                ClearBackground(BLANK);
                
                for (int ind = marbleNumber; ind > 0 && ind > marbleNumber - BOARDMAX - 1; ind--) { // Drawing Code
                    //int boardy = HEIGHT+(margindivide+margin)*ind-(leaderboardScroll*margin)-(margindivide*leaderboardScroll);
                    int boardy = 0 - ( (margindivide+margin)*ind-(leaderboardScroll*margin)-(margindivide*leaderboardScroll) );
                    
                    char timetx[9]; // 8 digits plus this WEIRD null terminator, whatever bro
                    char plactx[4]; // 3 digits + null term again
                    //                   minutes    seconds + millisec
                    
                    sprintf(timetx, "%d:%02d.%02d", (int)marbleTimes[ind-1]/60, (int)marbleTimes[ind-1]%60, (int)(marbleTimes[ind-1]*100)%100);
                    sprintf(plactx, "%d", ind);
                    
                    Color marblecolor = GetColor(marbleColors[findElement(marbleNoorder, 44, marbleNames[ind-1])]);
                    
                    Vector2 textPos = (Vector2){(maxlengthpFont+30)/2 - MeasureTextEx(pFont,plactx,fontSize,1.0f).x/2, boardy+(margindivide/2)-(fontSize/2)};

                    DrawRectangleGradientH(margin, boardy, WIDTH-(margin*2), margindivide, GetColor(0x171717F0), GetColor(0x171717CC)); // board
                    
                    if (!strcmp(marbleNames[ind-1], "Smile")) {
                        marblecolor = GetColor(0xFFFF00FF);
                        
                        DrawTexturePro( 
                        smileTexture, 
                        (Rectangle){0,0,smileTexture.width,smileTexture.height}, 
                        (Rectangle){circoffset - (margindivide-padding*2)/2, padding+boardy+margin*4, (margindivide-padding*2),(margindivide-padding*2)},
                        (Vector2){0,0},
                        0.0f,
                        WHITE);
                    }
                    else if (!strcmp(marbleNames[ind-1], "Rainbow")) {
                        marblecolor = ColorFromHSV(frame%360,1,1);
                        
                        DrawTexturePro( 
                        rainbowTexture, 
                        (Rectangle){0,0,rainbowTexture.width,rainbowTexture.height}, 
                        (Rectangle){circoffset - (margindivide-padding*2)/2, padding+boardy+margin*4, (margindivide-padding*2),(margindivide-padding*2)},
                        (Vector2){0,0},
                        0.0f,
                        WHITE);
                    }
                    else { DrawCircle(circoffset, boardy+(margindivide/2), (margindivide-padding*2)/2, marblecolor); } // color
                    
                    Color eliminatedColor = ind > 44 - eliminatedMarbles ? GetColor(0xFFA0A0FF) : WHITE;
                    
                    DrawTextEx(pFont, plactx, textPos, fontSize, 1.0f, eliminatedColor); // placement (#1..)
                    
                    DrawTextEx(tFont, timetx, (Vector2){(circoffset + 20 + (margindivide-padding*2)/2) + ( (WIDTH - (circoffset + 30 + (margindivide-padding*2)/2)) / 2) - (MeasureTextEx(tFont,timetx,timingSize,1.0f).x/2), (boardy+10) + ((margindivide-20)/2) - (timingSize/2)}, timingSize, 1.0f, eliminatedColor);
                    
                    if (videotime < marbleTimes[ind-1] + timerevealDuration) {
                        int namingBoxlength = (WIDTH - (circoffset + 30 + (margindivide-padding*2)/2)) * //multiply with transition (0 to 1)
                        easeInOutQuart(( marbleTimes[ind-1] + timerevealDuration - videotime) / timerevealDuration); // again HUGE thanks to easings.net
                        
                        Rectangle boxtx = (Rectangle){circoffset + 20 + (margindivide-padding*2)/2, boardy+10, namingBoxlength, margindivide-20};
                        Vector2 textpos = (Vector2){boxtx.x + ( (WIDTH - (circoffset + 30 + (margindivide-padding*2)/2)) / 2) - ((namingsTxtures[findElement(marbleNoorder, 44, marbleNames[ind-1])].width) / 2), (boxtx.y) + ((margindivide-20)/2) - (fontSize/2)}; 
                        float textreclength = namingBoxlength - (textpos.x - boxtx.x);
                        Rectangle textrec = (Rectangle){0,0,textreclength < 0 ? 0 : textreclength > (namingsTxtures[findElement(marbleNoorder, 44, marbleNames[ind-1])].width) ? (namingsTxtures[findElement(marbleNoorder, 44, marbleNames[ind-1])].width) : textreclength,fontSize};
                    
                        DrawRectangleRounded(boxtx, 0.1, 4, marblecolor);
                        DrawTextureRec(
                        namingsTxtures[findElement(marbleNoorder,
                        44, 
                        marbleNames[ind-1])], 
                        textrec, 
                        textpos, 
                        (!strcmp(marbleNames[ind-1], "Rainbow") || !strcmp(marbleNames[ind-1], "Smile")) ? BLACK : WHITE);
                    }
                }
                
            EndTextureMode(); // stop drawing on texture
            
            BeginDrawing();
                ClearBackground(BLANK);
                
                DrawFPS(0,0);
            EndDrawing();
            
            StoreFrame(target);
            
            Wait4BufferSpace();
        }
    EndRecording();
    
    for (int i = 0; i < 43; i++) { UnloadTexture(namingsTxtures[i]); }
    CloseWindow();
    
    return 0;
}