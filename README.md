# Marble Race Leaderboard
This is a leaderboard video renderer i created for my own marble race videos, i thought about sharing my source files since this could be very useful to a lot of people. So here it is! Already comes with video renderer built in, no need to record.

# How to install
1. Install raylib: https://www.raylib.com/
2. After installing raylib, open the leaderboard.c file with Notepad++ (it comes with the raylib installer itself)
3. Press F6 and click OK, make sure the script is in raylib_compile_execute
And now you got my own code running!!

To change the marbles data, just change these values in here, marbleColors and marbleNoorder both don't have the time order, they're only for matching colors!!
You can change the marbleNames and marbleTimes tho.
<img width="1919" height="911" alt="image" src="https://github.com/user-attachments/assets/7207aa9d-eeb2-4c88-a2a6-ac247467aa92" />
To get them from Algodoo follow this guide: <img width="1524" height="963" alt="image" src="https://github.com/user-attachments/assets/0e8aa8cd-7704-4de3-b7b5-acb4c76f179d" />

You can also change the video settings by changing the "cmd" string, which contains the ffmpeg script.    
ALSO!!!!!!!!!!!!! VERY IMPORTANT! make sure you have ffmpeg AS YOUR ENVIROMENT VARIABLE. If you run cmd and type ffmpeg in there, and it recognizes that code. It means you already have ffmpeg.  
IF you dont have it, install it here: https://ffmpeg.org/
