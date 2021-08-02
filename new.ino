#platform "uLCD-28PT"
#inherit "4DGL_16bitColours.fnc"
#inherit "MS_SanSerif8x12.fnt"
//#inherit "FONT4.fnt"

#MODE RUNFLASH
//


//Pip-Boy 3000B Program Version 1.0  by DJ Harrigan
//This is in the Public Domain

//This is actually a .4DG file and should be renamed as such for 4D systems (320x240) displays

#DATA

    word bootText
         boot0, boot1, boot2

    byte boot0 "SQUIRREL COMPUTERS (TM) ChipPad 1.0\nREV-V100\n\n",0
    byte boot1 "Initializing SQUIRREL COMPUTERS (TM) PCI/OS 1.0\n\n\n",0
    byte boot2 "BOOTING ....\nPCI/OS BOOTED",0

#END

var D;// disk
var *imageList;// pointer to our list of images stored on the uSD card
var combuf[50];
//************************************************************************************************************************************************MODE (state vars)
var mode := 0;//stats, items, data
var lastMode := 0;

var subMode := 0;//status, SPECIAL, skills etc.
var lastSubMode := 0;

var statusMode := 0;//CND, RAD, EFF
var lastStatusMode := 0;

var selection := 0;
var lastSelection := 0;

var imgOffset := 1;//If offset is 1, then images are green. If it is 0, then images are amber. (we can do this since the files are right next to each other in the image list)
var drawn := 0;
var config := 0;
//************************************************************************************************************************************************Screen Vars
//screen display variables
var bckgrndClr;
var color;
var dimColor;
var darkColor;

var rectX;
var rectY;
var lastRectX;
var lastRectY;
var lineX;
var lineY;
var index;
var n;
var char;
var maxSelection := 1;
//************************************************************************************************************************************************Game Mechanic Vars

//character variables
/* No need for these to be dynamic, just taking up space
var name := "User Name";
var level := 1;
var lvlUp := 0;
var exp := 0;
var expReq := 0;
var curHP := 0;
var maxHP := 0;
var curAP := 0;
var maxAP := 0;
var radResist := 0;
var DT := 0;
var caps := 0;
var curWg;
var maxWg;

var Strength := 5;
var Perception := 7;
var Endurance := 5;
var Charisma := 8;
var Intelligence := 7;
var Agility := 3;
var Luck := 7;

var barter := 0;
var bigGuns := 0;
var energyWeapons := 0;
var explosives := 0;
var lockpick := 0;
var medicine := 0;
var meleeWeapons := 0;
var repair := 0;
var science := 0;
var smallGuns := 0;
var sneak := 0;
var speech := 0;
var unarmed := 0;
*/

var karma := 0;

func main()


init(1);//initialization

repeat//Begin main loop

    com_Init(combuf, 50, '$');//read data into the serial buffer

    updateScreen();

    if(mode == 0)//only check when in the proper mode (which is STATS or mode 0)
        //updateStatusMode();
    endif

    if (drawn == 0)

        if (subMode == 5)//Configuration screen exists independently of modes
            dispConfig();
        endif

        drawBorders();

        //*********************************************MODE
        if (mode == 0)//STATS

            //if (drawn == 0)

            //drawBorders();
            dispSTATS();

            if (subMode == 0)//Status

                dispStatus();

                //*********************************************SUBMODE
                if (statusMode == 0)//CND
                    dispCND();
                endif
                //*********************************************
                if (statusMode == 1)//RAD
                    dispRAD();
                endif
                //*********************************************
                if (statusMode == 2)//EFF
                    dispEFF();
                endif

            endif
            //*********************************************
            if (subMode == 1)//SPECIAL
                dispSpecial();
            endif
            //*********************************************
            if (subMode == 2)//Skills
                dispSkills();
            endif
            //*********************************************
            if (subMode == 3)//Perks
                dispPerks();
            endif
            //*********************************************
            if (subMode == 4)//General
                dispGeneral();
            endif
            //endif//end of drawn

        endif
        //*********************************************MODE
        if (mode == 1)//ITEMS
            //if (drawn == 0)

            //drawBorders();
            dispITEMS();

            //*********************************************SUBMODE
            if (subMode == 0)//Weapons
                dispWeapons();
            endif
            //*********************************************
            if (subMode == 1)//Apparel
                dispApparel();
            endif
            //*********************************************
            if (subMode == 2)//Aid
                dispAid();
            endif
            //*********************************************
            if (subMode == 3)//Misc
                dispMisc();
            endif
            //*********************************************
            if (subMode == 4)//Ammo
                dispAmmo();
            endif
            //endif//end of drawn

        endif
        //*********************************************MODE
        if (mode == 2)//DATA
            //if (drawn == 0)

            //drawBorders();
            dispDATA();

            //*********************************************SUBMODE
            if (subMode == 0)//Local Map
                dispLocalMap();
            endif
            //*********************************************
            if (subMode == 1)//World Map
                dispWorldMap();
            endif
            //*********************************************
            if (subMode == 2)//Quests
                dispQuests();
            endif
            //*********************************************
            if (subMode == 3)//Notes
                dispNotes();
            endif
            //*********************************************
            if (subMode == 4)//Radio
                dispRadio();
            endif

        endif

    endif//end of "drawn"

forever

endfunc
//************************************************************************************************************************************************
func updateScreen()

selection := LObyte(combuf[0]) - 48;// Ascii "0" is equal to decimal 48, so we subtract 48 from each value to make it a decimal
mode := HIbyte(combuf[0]) - 48;     // technically unnecessary but it make the program easier to follow
subMode := LObyte(combuf[1]) - 48;
statusMode := HIbyte(combuf[1]) - 48;

if (mode == lastMode || subMode == lastSubMode || statusMode == lastStatusMode || selection == lastSelection)//nothing has changed, so no need to redraw

    drawn := 1;

endif

if (mode != lastMode || subMode != lastSubMode || statusMode != lastStatusMode || selection != lastSelection)//a change has occured on the screen

//this might seem redundant to check these again, but each variable needs to be updated individually if triggered, but they all affect the 'drawn' variable as a whole
    if (mode != lastMode) lastMode := mode;
    if (subMode != lastSubMode) lastSubMode := subMode;
    if (statusMode != lastStatusMode) lastStatusMode := statusMode;
    if (selection != lastSelection) lastSelection := selection;

    gfx_Cls();
    drawn := 0;

endif

if (selection > maxSelection) selection := maxSelection;

endfunc

//************************************************************************************************************************************************
func drawLineDown(var x, var y, var length)//draw a horizontal line that ends with the top->down vertical fade at the end

    gfx_Rectangle(x, y, (x + length), (y + 1), color);

    img_SetPosition(imageList, (11 + imgOffset),(x + length + 1), y);//draw the vertical fade at the end of the line
    img_Show(imageList, (11 + imgOffset));

endfunc
func checkSelection()//this limits the scrolling on certain screens so we can't highlight items not on the list

    if (selection < 0)
        selection := 0;
    endif

    if (selection > maxSelection)
        selection := maxSelection;
    endif

endfunc
//Begin STATS functions
func dispDATA()

    txt_MoveCursor(0, 2);
    print(" DATA ");
    txt_MoveCursor(1, 20);
    print("Unknown   00.00.00, 00:00");

    txt_Set(TEXT_OPACITY, 1);
    txt_MoveCursor(19, 2);print(" Local Map ");
    txt_MoveCursor(19, 11);print(" World Map ");
    txt_MoveCursor(19, 20);print(" Quests  ");
    txt_MoveCursor(19, 27);print(" Notes ");
    txt_MoveCursor(19, 33);print(" Radio ");

    img_SetPosition(imageList, (11 + imgOffset), 214, 5);//vert fade after "unknown"
    img_Show(imageList, (11 + imgOffset));

endfunc
func dispLocalMap()

    drawLineDown(80, 116, 165);

    gfx_Rectangle(16,228,80,239, color);//highlight Local Map
    txt_MoveCursor(10, 10);
    print("INTERNAL ERROR 0xD3:");
    txt_MoveCursor(11, 10);
    print("LIDAR module not detected");
    txt_MoveCursor(12, 10);
    print("Local map data unavailable");
    img_SetPosition(imageList, (0 + imgOffset), 110,36);//Sad Vault Boy Icon
    img_Show(imageList, (0 + imgOffset));

endfunc
func dispWorldMap()

        gfx_Rectangle(87,228,150,239, color);//highlight Local Map

        txt_MoveCursor(4, 1);
        print("INTERNAL ERROR 0xE3: MAP DATA: UNAVAILABLE");

        rectX := 12;

        for (index := 0; index <= 8; index++)
             img_SetPosition(imageList,( 17 + (2 * index + imgOffset)), rectX, 72);
             img_Show(imageList,( 17 + (2 * index + imgOffset)));
             rectX += 30;
        next

        rectX := 12;

        for (index := 8; index <= 14; index++)
             img_SetPosition(imageList,( 17 + (2 * index + imgOffset)), rectX, 102);
             img_Show(imageList,( 17 + (2 * index + imgOffset)));
             rectX += 30;
        next

endfunc

func dispNotes()

        maxSelection := 2;

        gfx_Rectangle(216,228,255,239, color);//highlight Notes

        rectY := 39 + (20 * selection);
        gfx_Rectangle(0, rectY, 129, (rectY + 15), color);
        //gfx_RectangleFilled(1, 84, 149, 97, darkColor);
        gfx_RectangleFilled(6, 82, 12, 88, color);//little "active" rectangle next to the text

        txt_Ygap(8);
        txt_Set(TEXT_OPACITY, 0);
        txt_MoveCursor(2,2);print("Note #1");
        txt_MoveCursor(3,2);print("Note #2");
        txt_MoveCursor(4,2);print("Note To Self");

        gfx_Rectangle(158, 85, 164, 91, color);//little "active" rectangle next to the text
        txt_Ygap(9);
        if (selection == 0)
        txt_MoveCursor(4,21);print("See Note #2");
        endif
        if (selection == 1)
        txt_MoveCursor(4,21);print("See Note To Self");
        endif
        if (selection == 2)
        txt_MoveCursor(4,21);print("Install note system");
        endif

endfunc
func dispConfig()//change UI settings

    var countdown := 3;
    //config := 1;
    maxSelection := 1;

    gfx_Cls();
    drawBorders();
    txt_MoveCursor(0, 2);
    print(" CONFIG ");

    txt_Ygap(8);
    txt_Set(TEXT_OPACITY, 0);
    txt_MoveCursor(1, 1);print("SQUIRREL COMPUTERS (TM) MODEL: 1.0 PCI/OS: v1.0");
    //txt_MoveCursor(2, 2);print("Amber");
    //txt_MoveCursor(3, 2);print("Green");
    txt_MoveCursor(4, 2);print("Changing UI color in", countdown);
    pause(1000);
    countdown--;
    txt_MoveCursor(4, 2);print("Changing UI color in", countdown);
    pause(1000);
    countdown--;
    txt_MoveCursor(4, 2);print("Changing UI color in", countdown);
    pause(1000);
    countdown--;

    imgOffset := !imgOffset;

    img_SetPosition(imageList, (127 + imgOffset), 160, 50);//Math Wrath
    img_Show(imageList, (127 + imgOffset));
                                                                                                                           
    init(0);

endfunc
func drawBorders()

        if (imgOffset == 0)
        var xColor;
        xColor := gfx_RGBto565(8, 8, 0);//for some reason the amber icons I created dont have pure black backgrounds, we'll need to make that dark color invisible
        gfx_Transparency(1);//uncomment these lines  to enable transparent images (use if you want to have the "raster scan lines" as if this were a CRT display)
        gfx_TransparentColour(xColor);
        endif
        /*gfx_TransparentColour(BLACK);
        for (lineY := 0; lineY <= 240; lineY += 4)
             gfx_Hline(lineY, 0, 319, darkColor);//horizontal raster lines
        next
        */

             img_SetPosition(imageList, (11 + imgOffset), 2, 5);//top left border
             img_Show(imageList, (11 + imgOffset));
             gfx_Rectangle(4, 5, 319, 6, color);//top horizontal line
             img_SetPosition(imageList, (11 + imgOffset), 316, 5);//top right border
             img_Show(imageList, (11 + imgOffset));

             img_SetPosition(imageList, (13 + imgOffset), 314, 206);//bottom right border
             img_Show(imageList, (13 + imgOffset));
             gfx_Rectangle(6, 234, 315, 235, color);//bottom horizontal line
             img_SetPosition(imageList, (13 + imgOffset), 2, 206);//bottom left border
             img_Show(imageList, (13 + imgOffset));

             txt_Ygap(0);
             txt_Set(TEXT_OPACITY, 1);
             //txt_MoveCursor(21, 49);print(" Terminal ");

             //file_PlayWAV("tabSW.wav");
             //txt_MoveCursor(10, 1);print(mem_Heap()," bytes\navailable");

endfunc
//************************************************************************************************************************************************
func init(var reboot)

gfx_Cls();

if (imgOffset)
    color := gfx_RGBto565(25, 255, 132);//bright green
    dimColor := gfx_RGBto565(0, 140, 20);//dim green
    darkColor := gfx_RGBto565(0, 70, 10);//very dim green
    bckgrndClr := gfx_RGBto565(4, 26, 13);//dark green

else

    color := gfx_RGBto565(217, 155, 40);//bright amber
    dimColor := gfx_RGBto565(100, 59, 0);//dim amber
    darkColor := gfx_RGBto565(50, 29, 0);//very dim amber
    bckgrndClr := gfx_RGBto565(25, 14, 0);//dark amber

endif

 //gfx_BGcolour(bckgrndClr);//now any calls to clear the screen will do so with this color instead of black

 txt_Set(TEXT_COLOUR, color);
 txt_FontID(MS_SanSerif8x12);
 txt_Set(9, 1);//bold text
 gfx_Set(CLIPPING, 1);
 gfx_Set( SCREEN_MODE, LANDSCAPE);// landscape mode LANDSCAPE_R for upside down

var printDelay := 35;

    for(n:=0; n < sizeof(bootText); n++)

        for (index:=0; index<strlen(bootText[n]); index++)
            print([CHR]*(bootText[n] + index));
            pause(printDelay);
        next

    next

if (reboot)

    if (!(D:=file_Mount()))
        while(!(D:=file_Mount()))
            putstr("INTERNAL ERROR");//the "holodisk" is actually the microSD card ;)
            pause(1000);
            gfx_Cls();
            pause(1000);
        wend
    endif

imageList := file_LoadImageControl("graphics.dat", "graphics.gci",1);//create pointer to list of images
img_Enable(imageList, ALL);

//com_SetBaud(COM0, 960);// uncomment this line to set communication to 9600 BAUD, default is 115200 BAUD
com_Init(combuf, 50, '$');

endif

endfunc
