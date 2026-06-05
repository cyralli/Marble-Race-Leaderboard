#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "raylib.h"

#include "ffmpeg.h"
#include "useful.h"
#include "data.h"

#define WIDTH 500
#define HEIGHT 1500
#define FPS 60
const double delTime = (1.0f/FPS);

// --------------- FOREGROUND SETTINGS ------------------------------------
const int padding = 5;
const int BOARDMAX = 15; // how many boards can we have
const int margindivide = HEIGHT/BOARDMAX; // boxes length

// --------------- FONT SETTINGS ------------------------------------------
const int fontSize = HEIGHT/BOARDMAX-60;
const int timingSize = fontSize+10;

// --------------- MARBLE SETTINGS ----------------------------------------
const int eliminatedMarbles = 4;

// ---------------------- ANIMATION SETTINGS ------------------------
const int transitionDuration = 50; // frames     why frames? this transition is not supposed to take long, so that why its used frames
const float timerevealDuration = 3; // sec       long transition

int main(void) {
    //before we start lets just organize "data.h" marbleCOLOR into our order
    Color marbleColors[44];
    for (int i = 0; i < 44; ++i) {
        int cl = findElement(marbleNames, 44, marbleNoorder[i]);
        *(marbleColors + cl) = GetColor(marbleCOLOR[i]);
    }
    
    SetTraceLogLevel(LOG_ERROR);
    
    StartRecording(WIDTH, HEIGHT, FPS);
    
    SetConfigFlags(0x00000104);
    InitWindow(900, 900, "Board Rendering");
    
    Font pFont = LoadFontEx("../font/BaseNeue-WideOblique.ttf", 100, 0, 0); // Placement font
    SetTextureFilter(pFont.texture, TEXTURE_FILTER_BILINEAR);
    Font nFont = LoadFontEx("../font/BaseNeue-SemiBold.ttf", 100, 0, 0); // Namings
    SetTextureFilter(nFont.texture, TEXTURE_FILTER_BILINEAR);
    Font tFont = LoadFontEx("../font/BaseNeue-WideMediumOblique.ttf", 100, 0, 0); // Time
    SetTextureFilter(tFont.texture, TEXTURE_FILTER_BILINEAR);
    Font defaultf = LoadFontEx("C:/Windows/Fonts/verdana.ttf", 100, 0, 0); // default font
    
    // -------------------------------- CREATE SOUND ---------------------------------
    
    int sampRate = 44100, sampSize = 32, sampCh = 1;
    
    Wave sound = LoadWave("../s.wav");
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
    int circoffset = (margindivide/2)+maxlengthpFont*1.5;
    
    //create images to be used in the namings
    Texture2D namingsTxtures[44]; // pass onto the gpu (textures)
    for (int i = 0; i < 44; i++) {
        Image namingsImg = GenImageColor( MeasureTextEx(nFont,marbleNoorder[i],fontSize,1.0f).x, fontSize, BLANK); // create image (in cpu mem)
        ImageDrawTextEx(&namingsImg, nFont, marbleNoorder[i], (Vector2){0,0}, fontSize, 1.0f, GetColor(grayscaleRGBA(ColorToInt(*(marbleColors + i))^0xFFFFFF00)));
        
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
    
    char rendtx[strlen(cmd)+16]; // for later, look into the end
                char newcmd[strlen(cmd)];
                sprintf(newcmd, "%s", cmd);
                
                for (int i = 30; i < 201; i += 30) {
                    while (cmd[i] != 32) { // space character
                        i++;
                    }
                    newcmd[i] = 10; // newline character
                }
                sprintf(rendtx, "FFMPEG Command:\n%s", newcmd); 
    
    // ------------------------------------------------------------------- MAIN LOOP ------------------------------------------------------------
    
        int frame = 0;
    
        for (; frame < (int)(marbleTimes[44-1]*FPS + timerevealDuration*FPS + transitionDuration) && !WindowShouldClose(); frame++) {
        
            float videotime = frame * delTime;
            
            if (floor(marbleTimes[marbleNumber] * FPS) == frame) { // comparing if this frame has a marble being safe
                    marbleNumber++; TleaderboardScroll = 0;
            }
            TleaderboardScroll += TleaderboardScroll < 1 ? 1.0/transitionDuration : 0;
            leaderboardScroll = marbleNumber + (1 - pow(1 - TleaderboardScroll, 3)); // big thanks to easings.net for easeoutcubic
            
            if (videotime > marbleTimes[marbleNumber-1] + timerevealDuration || marbleNumber == 0) {
                BeginDrawing();
                
                ClearBackground(DARKGRAY);
                
                int scW = GetScreenWidth();
                int scH = GetScreenHeight();
                
                DrawRectangle(scW/2, 5, scW/2-5, scH-10, BLACK);
                //how to find letterbox
                //find the max out of these WIDTH HEIGHT
                //so example WIDTH is bigger
                //then we divide our boxWidth by WIDTH
                //and then with the value we get ^here we can multiply it on HEIGHT too
                
                double getLtbx = 1;
                int centerW, centerH;
                if (WIDTH>HEIGHT) {
                    getLtbx = (scW/2-5)/(float)WIDTH;
                    centerW = scW/2;
                    centerH = (scH/2)-(HEIGHT*getLtbx/2);
                } else {
                    getLtbx = (scH-10)/(float)HEIGHT;
                    centerW = ((scW/2)+(scW/2-5)/2)-(WIDTH*getLtbx/2);
                    centerH = 5;
                }
                
                DrawTexturePro(target.texture, (Rectangle){0,0,WIDTH,HEIGHT},(Rectangle){centerW, centerH, WIDTH*getLtbx, HEIGHT*getLtbx}, (Vector2){0,0} ,0.0f, WHITE);
                
                DrawFPS(0,0);
                
                char mytext1[200];
                sprintf(mytext1, "\n\nRaylib Status:\nFPS: %i\nSpeed: %.2f\nFrame: %i (%.2fs)\nBuffer Line: %i/%i%s\n",
                GetFPS(), GetFPS()/(float)FPS, frame, videotime, count, MAX_BUFFERED_FRAMES, count==MAX_BUFFERED_FRAMES?" (Raylib is currently being held by FFMPEG.)":"");
                
                char mytext2[1024];
                snprintf(mytext2, 1024, "%s%s\nTime spent: %f", rendtx, mytext1, GetTime());
                
                DrawTextEx(defaultf, mytext2, (Vector2){0, 20}, 16, 1.0f, WHITE);
                
                EndDrawing();
                
                StoreFrame(target, true);
                continue;
            }
            
            BeginTextureMode(target); // begin drawing on texture
            
                ClearBackground(BLANK);
                
                for (int ind = marbleNumber; ind > 0 && ind > marbleNumber - BOARDMAX - 1; ind--) { // Drawing Code
                    //int boardy = HEIGHT+(margindivide+margin)*ind-(leaderboardScroll*margin)-(margindivide*leaderboardScroll);
                    int boardy = (margindivide*leaderboardScroll-margindivide) - margindivide*ind;
                    
                    char timetx[9]; // 8 digits plus this WEIRD null terminator
                    char plactx[4]; // 3 digits + null again
                    //                   minutes    seconds + millisec
                    
                    sprintf(timetx, "%d:%02d.%02d", (int)marbleTimes[ind-1]/60, (int)marbleTimes[ind-1]%60, (int)(marbleTimes[ind-1]*100)%100);
                    sprintf(plactx, "%d", ind);
                    
                    Color marblecolor = marbleColors[ind-1];
                    
                    Vector2 textPos = (Vector2){(maxlengthpFont+30)/2 - MeasureTextEx(pFont,plactx,fontSize,1.0f).x/2, boardy+(margindivide/2)-(fontSize/2)};

                    DrawRectangleGradientH(0, boardy, WIDTH, margindivide, GetColor(0x171717FA), GetColor(0x171717EE)); // board
                    
                    if (!strcmp(marbleNames[ind-1], "Smile")) {
                        marblecolor = GetColor(0xFFFF00FF);
                        
                        DrawTexturePro( 
                        smileTexture, 
                        (Rectangle){0,0,smileTexture.width,smileTexture.height}, 
                        (Rectangle){circoffset - (margindivide-padding*2)/2, padding+boardy, (margindivide-padding*2),(margindivide-padding*2)},
                        (Vector2){0,0},
                        0.0f,
                        WHITE);
                    }
                    else if (!strcmp(marbleNames[ind-1], "Rainbow")) {
                        marblecolor = ColorFromHSV(frame%360,1,1);
                        
                        DrawTexturePro( 
                        rainbowTexture, 
                        (Rectangle){0,0,rainbowTexture.width,rainbowTexture.height}, 
                        (Rectangle){circoffset - (margindivide-padding*2)/2, padding+boardy, (margindivide-padding*2),(margindivide-padding*2)},
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
                ClearBackground(DARKGRAY);
                
                int scW = GetScreenWidth();
                int scH = GetScreenHeight();
                
                DrawRectangle(scW/2, 5, scW/2-5, scH-10, BLACK);
                //how to find letterbox
                //find the max out of these WIDTH HEIGHT
                //so example WIDTH is bigger
                //then we divide our boxWidth by WIDTH
                //and then with the value we get ^here we can multiply it on HEIGHT too
                
                double getLtbx = 1;
                int centerW, centerH;
                if (WIDTH>HEIGHT) {
                    getLtbx = (scW/2-5)/(float)WIDTH;
                    centerW = scW/2;
                    centerH = (scH/2)-(HEIGHT*getLtbx/2);
                } else {
                    getLtbx = (scH-10)/(float)HEIGHT;
                    centerW = ((scW/2)+(scW/2-5)/2)-(WIDTH*getLtbx/2);
                    centerH = 5;
                }
                
                DrawTexturePro(target.texture, (Rectangle){0,0,WIDTH,HEIGHT},(Rectangle){centerW, centerH, WIDTH*getLtbx, HEIGHT*getLtbx}, (Vector2){0,0} ,0.0f, WHITE);
                
                DrawFPS(0,0);
                
                char mytext1[200];
                sprintf(mytext1, "\n\nRaylib Status:\nFPS: %i\nSpeed: %.2f\nFrame: %i (%.2fs)\nBuffer Line: %i/%i%s\n",
                GetFPS(), GetFPS()/(float)FPS, frame, videotime, count, MAX_BUFFERED_FRAMES, count==MAX_BUFFERED_FRAMES?" (Raylib is currently being held by FFMPEG.)":"");
                
                char mytext2[1024];
                snprintf(mytext2, 1024, "%s%s\nTime spent: %f", rendtx, mytext1, GetTime());
                
                DrawTextEx(defaultf, mytext2, (Vector2){0, 20}, 16, 1.0f, WHITE);
                
            EndDrawing();
            
            StoreFrame(target, false);
            
        }
    EndRecording();
    
    for (int i = 0; i < 43; i++) { UnloadTexture(namingsTxtures[i]); }
    CloseWindow();
    
    return 0;
}