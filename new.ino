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

    byte boot0 "ROBCO INDUSTRIES (TM) PIPBOY MODEL 3000B\nREV-V100\n\n",0
    byte boot1 "Initializing RobCo Industries (TM) PipOS v1.0.1\n\n\n",0
    byte boot2 "Mounting internal holodisk drive\nHolodisk drive mounted",0

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
func dispSTATS()

    txt_MoveCursor(0, 2);
    print(" STATS ");
    txt_MoveCursor(1, 13);
    print("HP 200/200   AP 0/0   XP 999/1000");

    txt_MoveCursor(19, 2);print(" Status ");
    txt_MoveCursor(19, 8);print(" S.P.E.C.I.A.L ");
    txt_MoveCursor(19, 19);print(" Skills ");
    txt_MoveCursor(19, 25);print(" Perks ");
    txt_MoveCursor(19, 31);print(" General ");

    img_SetPosition(imageList, (11 + imgOffset), 100, 5);//vert fade before HP
    img_Show(imageList, (11 + imgOffset));
    img_SetPosition(imageList, (11 + imgOffset), 179, 5);//vert fade before AP
    img_Show(imageList, (11 + imgOffset));
    img_SetPosition(imageList, (11 + imgOffset), 231, 5);//vert fade before XP
    img_Show(imageList, (11 + imgOffset));

endfunc
func dispStatus()

     maxSelection := 2;

    gfx_Rectangle(15,228,59,239, color);//subMode highlight rectangle

    txt_Ygap(11);
    txt_MoveCursor(2, 1);print("CND");
    txt_MoveCursor(3, 1);print("RAD");
    txt_MoveCursor(4, 1);print("EFF");

endfunc
func dispCND()

    gfx_Rectangle(4, 42, 35, 62, color);//highlight CND

    img_SetPosition(imageList, (193 + imgOffset),75, 45);//Vault Boy Outline
    img_Show(imageList, (193 + imgOffset));

    txt_MoveCursor(2, 33);print("Stimpak");
    txt_MoveCursor(4, 30);print("Doctor's Bag");
    txt_Ygap(0);
    //txt_MoveCursor(18, 24);putstr(name);print(" - Level ", level);

endfunc
func dispRAD()

    gfx_Rectangle(4, 64, 35, 84, color);//highlight RAD

    //drawLineDown(4, 176, 146);//line over RAD resist
    //gfx_Set(CLIPPING, 1);
    //img_SetPosition(imageList, (11 + imgOffset),318, 176);//vert fade after meter
    //gfx_Set(CLIPPING, 0);
    //img_Show(imageList, (11 + imgOffset));
    img_SetPosition(imageList, (149 + imgOffset),0, 153);//rad meter
    img_Show(imageList, (149 + imgOffset));

    txt_Set(TEXT_OPACITY, 0);//**********************NOTE: Go back and remove these "Text Opacity functions" the update functions will make them obsolete in the final program
    txt_MoveCursor(2, 33);print("RadAway");
    txt_MoveCursor(3, 35);print("Rad-X");
    txt_MoveCursor(6, 22);print("500");
    txt_MoveCursor(6, 36);print("1000");
    txt_Ygap(0);
    //txt_MoveCursor(15, 1);print("RAD RESIST          ", radResist, "%");
    txt_MoveCursor(13, 0);print("RADS");
    txt_Ygap(0);

endfunc
func dispEFF()

gfx_Rectangle(4, 88, 35, 108, color);//highlight EFF

endfunc

func dispSpecial()

        maxSelection := 7;

        drawLineDown(130, 151, 185);

        gfx_Rectangle(65, 228, 146, 239, color);//highlight special
        rectY := 39 + (20 * selection);
        gfx_Rectangle(9, rectY, 129, (rectY + 15), color);//highlights the current selected SPECIAL stat (needs to adjust according to Xpos of rotary encoder
        //gfx_RectangleFilled(10, (rectY + 1), 115, (rectY + 14), darkColor);
        //NOTE: Optimize by renaming the SPECIAL images to be in alphabetical order like the skill list
             if (selection == 0)

                img_SetPosition(imageList, (191 + imgOffset), 160, 50);//Strong Boy Icon
                img_Show(imageList, (191 + imgOffset));

             endif
             if (selection == 1)

                img_SetPosition(imageList, (189 + imgOffset), 160, 50);//Perceptive Vault Boy Icon
                img_Show(imageList, (189 + imgOffset));

             endif
             if (selection == 2)

                img_SetPosition(imageList, (183 + imgOffset), 160, 50);//Enduring Vault Boy Icon
                img_Show(imageList, (183 + imgOffset));

             endif
             if (selection == 3)

                img_SetPosition(imageList, (181 + imgOffset), 160, 50);//Charismatic Vault Boy Icon
                img_Show(imageList, (181 + imgOffset));

             endif
             if (selection == 4)

                img_SetPosition(imageList, (185 + imgOffset), 160, 50);//Intelligent Vault Boy Icon
                img_Show(imageList, (185 + imgOffset));

             endif
             if (selection == 5)

                img_SetPosition(imageList, (179 + imgOffset), 160, 50);//Agile Vault Boy Icon
                img_Show(imageList, (179 + imgOffset));

             endif
             if (selection == 6)

                img_SetPosition(imageList, (187 + imgOffset), 160, 50);//Lucky Vault Boy Icon
                img_Show(imageList, (187 + imgOffset));

             endif

        txt_Ygap(8);
        txt_Set(TEXT_OPACITY, 0);
        txt_MoveCursor(2, 2);print("Strength");
        //txt_MoveCursor(2, 13);print(Strength);

        txt_MoveCursor(3, 2);print("Perception");
        //txt_MoveCursor(3, 13);print(Perception);

        txt_MoveCursor(4, 2);print("Endurance");
        //txt_MoveCursor(4, 13);print(Endurance);

        txt_MoveCursor(5, 2);print("Charisma");
        //txt_MoveCursor(5, 13);print(Charisma);

        txt_MoveCursor(6, 2);print("Intelligence");
        //txt_MoveCursor(6, 13);print(Intelligence);

        txt_MoveCursor(7, 2);print("Agility");
        //txt_MoveCursor(7, 13);print(Agility);

        txt_MoveCursor(8, 2);print("Luck");
        //txt_MoveCursor(8, 13);print(Luck);

endfunc
func dispSkills()

        maxSelection := 8;

        drawLineDown(130, 151, 185);

        gfx_Rectangle(152,228,189,239, color);//highlight skills tab
        rectY := 39 + (20 * selection);
        gfx_Rectangle(9, rectY, 129, (rectY + 15), color);//highlights the current selected skill (needs to adjust according to Xpos of rotary encoder

        img_SetPosition(imageList, (153 +(2 * selection) + imgOffset), 175, 50);//since the skills are alphabetical, we can just display the current one
        img_Show(imageList, (153 +(2 * selection) + imgOffset));                // by multiplying by the current skill index (the "skill" variable)
//**NOTE**Make above code into a dynamic function so with vars of x/y position, start image index, (152 in above) and rectangle length
        txt_Ygap(8);
        txt_Set(TEXT_OPACITY, 0);
        txt_MoveCursor(2, 2);print("Barter");
        //txt_MoveCursor(2, 15);print(barter);

        txt_MoveCursor(3, 2);print("Big Guns");
        //txt_MoveCursor(3, 15);print(bigGuns);

        txt_MoveCursor(4, 2);print("Energy Weapons");
        //txt_MoveCursor(4, 15);print(energyWeapons);

        txt_MoveCursor(5, 2);print("Explosives");
        //txt_MoveCursor(5, 15);print(explosives);

        txt_MoveCursor(6, 2);print("Lockpick");
        //txt_MoveCursor(6, 15);print(lockpick);

        txt_MoveCursor(7, 2);print("Medicine");
        //txt_MoveCursor(7, 15);print(medicine);

        txt_MoveCursor(8, 2);print("Melee Weapons");
        //txt_MoveCursor(8, 15);print(meleeWeapons);

        txt_MoveCursor(9, 2);print("Repair");
        //txt_MoveCursor(9, 15);print(repair);

        txt_MoveCursor(10, 2);print("Science");
        //txt_MoveCursor(10, 15);print(science);
        /* Must add dynamic scrolling before these can be seen
        txt_MoveCursor(11, 2);print("Small Guns");
        txt_MoveCursor(11, 15);print(smallGuns);

        txt_MoveCursor(12, 2);print("Sneak");
        txt_MoveCursor(12, 15);print(sneak);

        txt_MoveCursor(13, 2);print("Speech");
        txt_MoveCursor(13, 15);print(speech);

        txt_MoveCursor(14, 2);print("Unarmed");
        txt_MoveCursor(14, 15);print(unarmed);*/

endfunc
func dispPerks()

    maxSelection := 2;

    drawLineDown(130, 151, 185);
    gfx_Rectangle(200,228,239,239, color);//highlight perks tab
        rectY := 39 + (20 * selection);
        gfx_Rectangle(9, rectY, 129, (rectY + 15), color);//highlights the current selected perk (needs to adjust according to Xpos of rotary encoder

        img_SetPosition(imageList, (121 +(2 * selection) + imgOffset), 175, 50);//since the perks are alphabetical, we can just display the current one
        img_Show(imageList, (121 +(2 * selection) + imgOffset));                // by multiplying by the current perk index (the "perk" variable)

        txt_Ygap(8);
        txt_Set(TEXT_OPACITY, 0);
        txt_MoveCursor(2, 2);print("Comprehension");

        txt_MoveCursor(3, 2);print("Cyborg");

        txt_MoveCursor(4, 2);print("Explorer");

endfunc
func dispGeneral()

        maxSelection := 9;

        gfx_Rectangle(248,228,299,238, color);//highlight general tab
        rectY := 39 + (20 * selection);
        gfx_Rectangle(9, rectY, 175, (rectY + 15), color);//highlights the current selected perk (needs to adjust according to Xpos of rotary encoder

        if (karma == -2)
        img_SetPosition(imageList, (111 + imgOffset), 205, 50);//EVIL
        img_Show(imageList, (111 + imgOffset));
        txt_MoveCursor(11, 28);print("Evil");
        endif
        if (karma == -1)
        img_SetPosition(imageList, (109 + imgOffset), 205, 50);//BAD
        img_Show(imageList, (109 + imgOffset));
        txt_MoveCursor(11, 28);print("Bad");
        endif
        if (karma == 0)
        img_SetPosition(imageList, (115 + imgOffset), 205, 50);//NEUTRAL
        img_Show(imageList, (115 + imgOffset));
        txt_MoveCursor(11, 28);print("Neutral");
        endif
        if (karma == 1)
        img_SetPosition(imageList, (113 + imgOffset), 205, 50);//GOOD
        img_Show(imageList, (113 + imgOffset));
        txt_MoveCursor(11, 28);print("Good");
        endif
        if (karma == 2)
        img_SetPosition(imageList, (117 + imgOffset), 205, 50);//SAINTLY
        img_Show(imageList, (117 + imgOffset));
        txt_MoveCursor(11, 28);print("Saintly");
        endif

        txt_Ygap(8);
        txt_Set(TEXT_OPACITY, 0);
        txt_MoveCursor(2, 2);print("Quests Completed");
        txt_MoveCursor(3, 2);print("Locations Discovered");
        txt_MoveCursor(4, 2);print("People Killed");
        txt_MoveCursor(5, 2);print("Creatures Killed");
        txt_MoveCursor(6, 2);print("Locks Picked");
        txt_MoveCursor(7, 2);print("Computers Hacked");
        txt_MoveCursor(8, 2);print("Stimpaks Taken");
        txt_MoveCursor(9, 2);print("Rad-X Taken");
        txt_MoveCursor(10, 2);print("RadAway Taken");
        //txt_MoveCursor(11, 2);print("Chems Taken");

endfunc
//Begin ITEMS functions
func dispITEMS()

    drawLineDown(130, 151, 185);

    img_SetPosition(imageList, (11 + imgOffset), 135, 5);//vert fade before HP
    img_Show(imageList, (11 + imgOffset));
    img_SetPosition(imageList, (11 + imgOffset), 211, 5);//vert fade before DT
    img_Show(imageList, (11 + imgOffset));
    img_SetPosition(imageList, (11 + imgOffset), 246, 5);//vert fade before Caps
    img_Show(imageList, (11 + imgOffset));

    txt_MoveCursor(0, 2);
    print(" ITEMS ");
    txt_MoveCursor(1, 8);
    txt_Set(TEXT_OPACITY, 0);
    print("Wg 185/235  HP 200/200  DT 3  Caps 1000");
    txt_Set(TEXT_OPACITY, 1);
    txt_MoveCursor(19, 2);print(" Weapons ");
    txt_MoveCursor(19, 11);print(" Apparel ");
    txt_MoveCursor(19, 19);print(" Aid  ");
    txt_MoveCursor(19, 25);print(" Misc ");
    txt_MoveCursor(19, 31);print(" Ammo ");
/*
    txt_Ygap(0);
    txt_Set(TEXT_OPACITY, 0);
    txt_MoveCursor(1, 12);print("Wg ", Wg);
    txt_MoveCursor(1, 18);print("HP ", curHP, "/", HP);
    txt_MoveCursor(1, 30);print("DT ", DT);
    txt_MoveCursor(1, 43);print("Caps    ", caps);
    txt_Set(TEXT_OPACITY, 1);
    txt_MoveCursor(21, 3);print(" Weapons ");
    txt_MoveCursor(21, 14);print(" Apparel ");
    txt_MoveCursor(21, 24);print(" Aid  ");
    txt_MoveCursor(21, 32);print(" Misc ");
    txt_MoveCursor(21, 40);print(" Ammo ");
    txt_Set(TEXT_OPACITY, 0);
*/
endfunc
/*func dispItemsBracket()
gfx_Rectangle(180, 126, 319, 127, color);//line over DAM, WG, VAL

    img_SetPosition(imageList, (11 + imgOffset), 319, 126);//vert fade
    img_Show(imageList, (11 + imgOffset));
    img_SetPosition(imageList, (11 + imgOffset), 282, 126);//vert fade before WG
    img_Show(imageList, (11 + imgOffset));
    img_SetPosition(imageList, (11 + imgOffset), 319, 126);//vert fade before VAL
    img_Show(imageList, (11 + imgOffset));
    img_SetPosition(imageList, (11 + imgOffset), 282, 156);//vert fade after CND
    img_Show(imageList, (11 + imgOffset));


    txt_Ygap(6);
    txt_MoveCursor(9, 23);print("CND");//NOTE: make item's display variable based on the submode to only display certain brackets like VAL, CND etc.
    txt_Ygap(7);
    txt_MoveCursor(7, 36);print("WG");
    txt_MoveCursor(7, 46);print("VAL");

endfunc*/
func dispWeapons()

    maxSelection := 1;

    //dispItemsBracket();
    img_SetPosition(imageList, (105 +(2 * selection) + imgOffset), 170, 50);//display selected weapon
    img_Show(imageList, (105 +(2 * selection) + imgOffset));

    gfx_Rectangle(16,228,72,239, color);//highlight Weapons tab
    rectY := 39 + (20 * selection);
    gfx_Rectangle(4, rectY, 141, (rectY + 15), color);

    txt_Ygap(8);
    txt_Set(TEXT_OPACITY, 0);
    txt_MoveCursor(2, 2);print("Alien Blaster");//NOTE: add dynamic weapon variables
    txt_MoveCursor(3, 2);print("Fat Man");

endfunc
func dispApparel()

     maxSelection := 1;

    //drawLineDown(180, 156, 291);
    //dispItemsBracket();
    img_SetPosition(imageList, (77 +(2 * selection) + imgOffset), 170, 50);//display selected item
    img_Show(imageList, (77 +(2 * selection) + imgOffset));

    gfx_Rectangle(88,228,138,239, color);//highlight Apparel tab
    rectY := 39 + (20 * selection);
    gfx_Rectangle(4, rectY, 129, (rectY + 15), color);

    //txt_MoveCursor(7, 23);print("DR");
    txt_Ygap(8);
    txt_Set(TEXT_OPACITY, 0);
    txt_MoveCursor(2, 2);print("Lab Coat");
    txt_MoveCursor(3, 2);print("Sunglasses");//NOTE: add dynamic apparel variables

endfunc
func dispAid()

     maxSelection := 4;

    //drawLineDown(282, 126, 80);//line over WG
    //drawLineDown(367, 126, 103);//line over VAL
    img_SetPosition(imageList, (47 +(2 * selection) + imgOffset), 170, 50);//display selected item
    img_Show(imageList, (47 +(2 * selection) + imgOffset));

    gfx_Rectangle(152,228,181,239, color);//highlight Aid tab
    rectY := 39 + (20 * selection);
    gfx_Rectangle(4, rectY, 160, (rectY + 15), color);
/*
    txt_Ygap(7);
    txt_MoveCursor(7, 36);print("WG");
    txt_MoveCursor(7, 46);print("VAL");
*/
    txt_Ygap(8);
    txt_Set(TEXT_OPACITY, 0);
    txt_MoveCursor(2, 2);print("Big Book of Science");
    //txt_MoveCursor(3, 2);print("Chinese Army Spec. Ops Training Manual");
    txt_MoveCursor(3, 2);print("Chinese Army Spec. Ops");
    txt_MoveCursor(4, 2);print("Deans Electronics");
    txt_MoveCursor(5, 2);print("Duck and Cover");
    txt_MoveCursor(6, 2);print("Grognak the Barbarian");
    /*txt_MoveCursor(7, 2);print("Guns n Bullets");
    txt_MoveCursor(8, 2);print("D.C. Journal of Internal Medicine");
    txt_MoveCursor(9, 2);print("Lyning Congressional Style");
    txt_MoveCursor(10, 2);print("Nikola Tesla and You");
    txt_MoveCursor(11, 2);print("Pugilism Illustrated");*/

endfunc
func dispMisc()

    maxSelection := 4;

    img_SetPosition(imageList, (83 +(2 * selection) + imgOffset), 170, 50);//display selected item
    img_Show(imageList, (83 +(2 * selection) + imgOffset));

    gfx_Rectangle(201,228,231,239, color);//highlight Misc tab
    rectY := 39 + (20 * selection);
    gfx_Rectangle(4, rectY, 129, (rectY + 15), color);

    txt_Ygap(8);
    txt_Set(TEXT_OPACITY, 0);
    txt_MoveCursor(2, 2);print("Bobby Pin");
    txt_MoveCursor(3, 2);print("Gift");
    txt_MoveCursor(4, 2);print("Holotape");
    txt_MoveCursor(5, 2);print("Keyring");
    txt_MoveCursor(6, 2);print("Map");

endfunc
func dispAmmo()

    maxSelection := 1;

    img_SetPosition(imageList, (73 +(2 * selection) + imgOffset), 170, 50);//display selected item
    img_Show(imageList, (73 +(2 * selection) + imgOffset));

    gfx_Rectangle(248,228,288,239, color);//highlight Ammo tab
    rectY := 39 + (20 * selection);
    gfx_Rectangle(4, rectY, 129, (rectY + 15), color);

    txt_Ygap(8);
    txt_Set(TEXT_OPACITY, 0);
    txt_MoveCursor(2, 2);print("Aliean Power Cell");
    txt_MoveCursor(3, 2);print("Mini Nuke");

endfunc
//Begin DATA functions
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
func dispQuests()

        gfx_Rectangle(160,228,209,239, color);//highlight Quests
        //Placeholder "Quest" Graphics (soon to be dynamic! That is, read from a .txt file stored on the uSD card)

        txt_Ygap(9);
        txt_Set(TEXT_OPACITY, 0);
        txt_MoveCursor(2,2);
        print("That Lucky Old Sun");

        txt_MoveCursor(3,2);
        print("They Went That-a-Way");

        gfx_Rectangle(0, 83, 150, 98, color);//highlights the quest
        //gfx_RectangleFilled(1, 84, 149, 97, darkColor);
        gfx_RectangleFilled(6, 88, 12, 94, color);//little "active" rectangle next to the text
        txt_MoveCursor(4,2);
        print("My Kind of Town");

        gfx_Rectangle(158, 75, 164, 81, color);//little "active" rectangle next to the text
        txt_Ygap(6);
        txt_MoveCursor(4,21);print("Beagle wants a new");
        txt_MoveCursor(5,21);print("sheriff for Primm. He");
        txt_MoveCursor(6,21);print("suggested and ex-sheriff");
        txt_MoveCursor(7,21);print("at NCRCF and the NCR as");
        txt_MoveCursor(8,21);print("potential candidates.");

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
func dispRadio()

        maxSelection := 1;

        gfx_Rectangle(264,228,303,239, color);//highlight Radio
        rectY := 39 + (20 * selection);
        gfx_Rectangle(0, rectY, 129, (rectY + 15), color);

        //create the "audio waveform grid" This displays, but is not dynamic for the time being
        gfx_Rectangle(318, 50, 319, 200, color);//right side of radio waveform bracket
        gfx_Rectangle(168, 199, 317, 200, color);//bottom side

        for (rectY := 52; rectY <= 199; rectY += 4)
        gfx_Hline(rectY, 310, 317, dimColor);//horizontal notches rightmost
        next
        for (lineY := 72; lineY <= 200; lineY += 24)
        gfx_Hline(lineY, 302, 309, dimColor);//horizontal notches right
        next
        for (lineX := 170; lineX <= 316; lineX += 4)
        gfx_Vline(lineX, 191, 198, dimColor);//vertical notches bottom
        next
        for (lineX := 174; lineX <= 300; lineX += 24)
        gfx_Vline(lineX, 183, 190, dimColor);//vertical notches top
        next

        gfx_Rectangle(168, 120, 316, 121, color);//"no audio" waveform line

        txt_Ygap(8);
        txt_Set(TEXT_OPACITY, 0);
        txt_MoveCursor(2,2);print("Enclave Radio");
        txt_MoveCursor(3,2);print("Galaxy News Radio");

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
    txt_MoveCursor(1, 1);print("RobCo Industries (TM) MODEL: 3000B PipOS: v1.0.1");
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
            putstr("INTERNAL ERROR 0x00: Holodisk drive not mounted");//the "holodisk" is actually the microSD card ;)
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
