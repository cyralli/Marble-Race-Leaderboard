
#include "raylib.h"
#include "rlgl.h"

#if defined(_WIN32) // theres some stupid structs in windows.h that interfere with raylib
    #define NOGDI
    #define NOUSER // so we remove most of them by here
    
    typedef struct tagMSG { // but now theres another struct missing for some of the functions being used
        void*    hwnd;
        unsigned int message;
        unsigned int* wParam;
        long*        lParam;
        unsigned long time;
        struct { long x; long y; } pt;
    } MSG, *PMSG, *LPMSG;
#endif

#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BUFFERED_FRAMES 512

FILE* ffmpeg;
void ExporterThread(void *arg);
unsigned char* ringBuffer = NULL;

void* frameBuffer[MAX_BUFFERED_FRAMES];
int head = 0; // Where Raylib puts frames
int tail = 0; // Where the Exporter takes frames
int count = 0; // Current number of frames in buffer
HANDLE mutex;  // To prevent thread clashing
bool isFinished = false;

int WIDTH, HEIGHT, FPS;
int frameSize;

char cmd[512];

void StartRecording(int w, int h, int fps) {
    
    WIDTH = w;
    HEIGHT = h;
    FPS = fps;
    frameSize = w * h * 4;
    
    ringBuffer = (unsigned char*)malloc(frameSize * MAX_BUFFERED_FRAMES);
    
    // create string to command
    
    sprintf(cmd, 
    "ffmpeg -y "
    "-thread_queue_size 2048 "
    "-f rawvideo -pix_fmt rgba -s %dx%d -r %d -i - "
    "-i audioresult.wav "
    "-vf vflip "
    "-c:v png "
    "-c:a pcm_s16le "
    "-map 0:v:0 -map 1:a:0 "
    "-shortest "
    "-max_interleave_delta 0 "
    "-pix_fmt rgba output.mov", 
    w, h, fps);
    
    // create pipe
    FILE* ffmpeg = _popen(cmd, "wb");
    
    // create different thread for ffmpeg
    mutex = CreateMutex(NULL, FALSE, NULL);
    _beginthread(ExporterThread, 0, (void*)ffmpeg);
    
    return;
}


void StoreFrame(RenderTexture2D frame, bool repeated) {
    static void *internalPixels = NULL;
    
    if (!repeated) {
        if (internalPixels != NULL) {
            RL_FREE(internalPixels);
        }
        
        rlEnableFramebuffer(frame.id); 
        internalPixels = rlReadTexturePixels(frame.texture.id, WIDTH, HEIGHT, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
        rlDisableFramebuffer();
    }
    
    while (true) {
        WaitForSingleObject(mutex, INFINITE);
        if (count < MAX_BUFFERED_FRAMES) { // if theres space in the buffer...
            void* currentSlot = ringBuffer + (head * frameSize);
                    
            if (internalPixels != NULL) {
                memcpy(currentSlot, internalPixels, frameSize);
            }
                    
            frameBuffer[head] = currentSlot; // transfer to framebuffer
            head = (head + 1) % MAX_BUFFERED_FRAMES;
            count++;
            ReleaseMutex(mutex);
            break;
        }
        ReleaseMutex(mutex);
        Sleep(1);
    }
    
    return;
}

void EndRecording() {
    isFinished = true;
    
    while (true) { // wait for buffer to be empty
        int remaining;
        WaitForSingleObject(mutex, INFINITE);
        remaining = count;
        ReleaseMutex(mutex);
        if (remaining == 0) break;
        Sleep(10);
    }
        
    _pclose(ffmpeg);
}

// ---------------------------------------------------------------------------------------------------

void ExporterThread(void *arg) { 
    FILE* pipe = (FILE*)arg; // create pipe and convert arg to FILE*
    
    while (true) {
        void* datatoExport = NULL; // array for data to export
        
        WaitForSingleObject(mutex, INFINITE);
        if (count > 0) { // if there are frames in buffer
            datatoExport = frameBuffer[tail]; // then pass to datatoExport
        }
        bool shouldExit = (isFinished && count == 0); //
        ReleaseMutex(mutex);
        
        if (datatoExport) { // if theres data that ffmpeg already can export...
            fwrite(datatoExport, frameSize, 1, pipe); // then pass them to ffmpeg using the pipe
            
            WaitForSingleObject(mutex, INFINITE);
            tail = (tail + 1) % MAX_BUFFERED_FRAMES; // where the exporter takes the frames
            count--;
            ReleaseMutex(mutex);
        } else {
            if (shouldExit) break;
            Sleep(1); // wait for raylib to create more
        }
    }
}
// --------------------------------------------------------------------------------------------------

