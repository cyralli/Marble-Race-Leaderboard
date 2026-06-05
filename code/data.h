#ifndef DATA_H
#define DATA_H

// Marble colors, this is not in order, only for matching
const unsigned long marbleCOLOR[44] = {
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

#endif