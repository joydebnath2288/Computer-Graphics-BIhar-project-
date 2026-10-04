#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>


bool showIntro = true;

// ==== Animation state ====
float cloudX = 0.0f, RedBird  = 0, timeCount = 0.0f, sunY = 440, lightningAlpha = 0.0f;
bool isSunset = false, isNight = false, isRaining = false;

// ==== Stars and Rain ====
const int NUM_STARS = 100;
float stars[NUM_STARS][2];
const int NUM_DROPS = 200;
float drops[NUM_DROPS][2];

void playLoopSound(const char* filename) {
    PlaySound(TEXT(filename), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
}


void stopSound() {
    PlaySound(NULL, 0, 0);
}


void drawIntro() {
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1.0f, 1.0f, 1.0f);

    void *font = GLUT_BITMAP_HELVETICA_18;

    auto drawText = [&](float x, float y, const char* text) {
        glRasterPos2f(x, y);
        while (*text) glutBitmapCharacter(font, *text++);
    };

    drawText(300, 450, "BHIHAR PROJECT");
    drawText(280, 400, "'N' = NIGHT MODE");
    drawText(280, 370, "'S' = SUNSET");
    drawText(280, 340, "'R' = RAIN");
    drawText(280, 310, "'L' = LIGHTNING");
    drawText(280, 280, "Right Mouse = START RAIN");
    drawText(280, 250, "Left Mouse = STOP RAIN");
    drawText(280, 220, "NAME:JOY DEBNATH       ID:22-49209-3");
    drawText(280, 150, "[ PRESS ENTER TO START ]");

    glFlush();
}


// ==== Init ====
void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, 791, 0.0, 504.6);
    srand(time(0));
    for (int i = 0; i < NUM_STARS; ++i) {
        stars[i][0] = rand() % 1000;
        stars[i][1] = 400 + rand() % 200;
    }
    for (int i = 0; i < NUM_DROPS; ++i) {
        drops[i][0] = rand() % 1000;
        drops[i][1] = rand() % 600;
    }
}

// ==== Drawing Background Sky ====
void drawBackground() {
    if (isNight) glClearColor(0.05f, 0.05f, 0.2f, 1.0f);
    else if (isSunset) glClearColor(0.99f, 0.54f, 0.26f, 1.0f);
    else glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void drawSun() {
    glColor3f(1.0f, 0.843f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(650, sunY);
    for (int i = 0; i <= 100; ++i) {
        float angle = 2 * 3.1416f * i / 100;
        glVertex2f(650 + cos(angle) * 40, sunY + sin(angle) * 40);
    }
    glEnd();
}

void drawCloud(float x, float y, float scale = 1.0f) {
    glColor4f(1.0f, 1.0f, 1.0f, 0.75f);
    for (int i = 0; i < 4; ++i) {
        float cx = x + i * 20 * scale;
        float cy = y + (i % 2) * 5 * scale;
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (int j = 0; j <= 100; ++j) {
            float angle = 2 * 3.1416f * j / 100;
            glVertex2f(cx + cos(angle) * 15 * scale, cy + sin(angle) * 15 * scale);
        }
        glEnd();
    }
}

void drawRedBird(float x, float y, float wingAngle) {
    glPushMatrix();
    glTranslatef(x, y, 0);

    // Body
    glColor3f(0.8f, 0.0f, 0.0f); // Red
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 100; ++i) {
        float angle = 2.0f * 3.1416f * i / 100;
        glVertex2f(cos(angle) * 8, sin(angle) * 5);
    }
    glEnd();

    // Wing (flapping)
    glPushMatrix();
    glRotatef(sin(wingAngle) * 30, 0, 0, 1);
    glColor3f(1.0f, 0.4f, 0.4f);
    glBegin(GL_TRIANGLES);
        glVertex2f(0, 0);
        glVertex2f(-12, 8);
        glVertex2f(-6, 0);
    glEnd();
    glPopMatrix();

    // Beak
    glColor3f(1.0f, 0.65f, 0.0f); // Orange
    glBegin(GL_TRIANGLES);
        glVertex2f(8, 0);
        glVertex2f(13, 2);
        glVertex2f(13, -2);
    glEnd();

    glPopMatrix();
}



void drawStars() {
    glPointSize(2);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < NUM_STARS; ++i) {
        glVertex2f(stars[i][0], stars[i][1]);
    }
    glEnd();
}

void drawRain() {
    glColor3f(0.5f, 0.5f, 1.0f);
    glBegin(GL_LINES);
    for (int i = 0; i < NUM_DROPS; ++i) {
        glVertex2f(drops[i][0], drops[i][1]);
        glVertex2f(drops[i][0], drops[i][1] - 10);
    }
    glEnd();
}

void drawLightning() {
    if (lightningAlpha > 0.0f) {
        glColor4f(1.0f, 1.0f, 1.0f, lightningAlpha);
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(1000, 0);
        glVertex2f(1000, 600);
        glVertex2f(0, 600);
        glEnd();
    }
}

// ==== Draw Static House ====
void drawHouse(); // Forward declaration

// ==== Full Scene ====
void drawScene() {
    if (showIntro) {
        drawIntro();
        return;
    }

    drawBackground();
    if (!isNight) drawSun();

    drawCloud(100 + cloudX, 420);
    drawCloud(300 + cloudX * 0.8f, 440, 0.8f);
    drawCloud(600 + cloudX * 0.6f, 400, 1.2f);


    if (isNight) drawStars();
    drawLightning();
    drawHouse();
    if (isRaining) drawRain();
    drawRedBird(RedBird, 390, timeCount * 0.2f);
    drawRedBird(RedBird + 100, 370, timeCount * 0.25f);
    drawRedBird(RedBird + 200, 410, timeCount * 0.3f);


    glFlush();
}


// ==== Timer Function ====
void timer(int) {
    cloudX += 0.3f;
    if (cloudX > 1100) cloudX = -200;
    RedBird += 1.0f;
    if (RedBird > 1200) RedBird = -100;
   // RedBird=0;
    timeCount += 1.0f;

    if (isSunset && sunY > -40) sunY -= 0.2f;

    if (isRaining) {
        for (int i = 0; i < NUM_DROPS; ++i) {
            drops[i][1] -= 4;
            if (drops[i][1] < 0) {
                drops[i][1] = 600;
                drops[i][0] = rand() % 1000;
            }
        }
    }

    if (lightningAlpha > 0.0f) {
        lightningAlpha -= 0.03f;
        if (lightningAlpha < 0) lightningAlpha = 0;
    }

 static bool rainSoundPlaying = false;
static bool nightSoundPlaying = false;
static bool birdSoundPlaying = false;  // ✅ add this

// 🌧 Rain sound logic
if (isRaining && !rainSoundPlaying) {
    playLoopSound("rain.wav");
    rainSoundPlaying = true;
}
if (!isRaining && rainSoundPlaying) {
    stopSound();  // this will stop rain
    rainSoundPlaying = false;
}

// 🌙 Night sound logic
if (isNight && !nightSoundPlaying && !isRaining) {
    playLoopSound("night.wav");
    nightSoundPlaying = true;
    birdSoundPlaying = false; // stop bird if night
}
if (!isNight && nightSoundPlaying && !isRaining) {
    stopSound();  // stop night sound
    nightSoundPlaying = false;
}

// ☀️ Daytime birds logic
if (!isNight && !isRaining && !birdSoundPlaying) {
    playLoopSound("birds.wav");
    birdSoundPlaying = true;
}
if ((isNight || isRaining) && birdSoundPlaying) {
    stopSound();  // stop birds if night or raining
    birdSoundPlaying = false;
}



    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);

}

// ==== Keyboard Input ====
void keyboard(unsigned char key, int x, int y) {
    if (showIntro && key == 13) { // ENTER key
        showIntro = false;
        return;
    }

    switch (key) {
    case 's': case 'S': isSunset = !isSunset; break;
    case 'n': case 'N': isNight = !isNight; break;
    case 'r': case 'R': isRaining = !isRaining; break;
    case 'l': case 'L':
        lightningAlpha = 0.9f;
        PlaySound(TEXT("thunder.wav"), NULL, SND_ASYNC | SND_FILENAME);
        break;
    }

    glutPostRedisplay();
}


void mouse(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        isRaining = true;
        playLoopSound("rain.wav");
    } else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        isRaining = false;
        stopSound();
    }
}



// ==== Main Function ====
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(791, 504.6);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("BHIHAR");
    init();
    glutDisplayFunc(drawScene);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}

// ==== Red House from main.cpp ====
void drawHouse(){
    glLineWidth(3);



    glColor3f(0.800, 0.522, 0.247);
    glBegin(GL_POLYGON);
    glVertex2i(0.0666627764122, 176.915037699036);
    glVertex2i(243.2486033632649, 117.5778369260456);
    glVertex2i(240.0702694050909, 3.0919551012972);
    glVertex2i(0.0649922257523, 98.6043376289714);
    glEnd();


    glBegin(GL_POLYGON);
    glVertex2i(243.2486033632649, 117.5778369260456);
    glVertex2i(695.5408168518136, 173.3467552244912);
    glVertex2i(698.6101031231661, 98.9699313354026);
    glVertex2i(240.0702694050909, 3.0919551012972);
    glEnd();

    glColor3f(0.6, 0.3, 0.1);
    glBegin(GL_POLYGON);
    glVertex2i(0.0721628571076, 253.6969125190391);
    glVertex2i(301.5566779098028, 240.6212047726911);
    glVertex2i(301.3970738888992, 169.6796858723581);
    glVertex2i(0.0092185206099, 212.9672530121446);
    glEnd();

    glColor3f(0.6, 0.3, 0.1);
    glBegin(GL_POLYGON);
    glVertex2i(301.5566779098028, 240.6212047726911);
    glVertex2i(301.3970738888992, 169.6796858723581);
    glVertex2i(795.0115586172365, 205.7292057232314);
    glVertex2i(794.0837216933438, 250.3030372509864);
    glEnd();

    glColor3f(0.6, 0.3, 0.1);
    glBegin(GL_POLYGON);
    glVertex2i(0.0092185206099, 212.9672530121446);
    glVertex2i(301.3970738888992, 169.6796858723581);
    glVertex2i(795.0115586172365, 205.7292057232314);
    glVertex2i(794.5827760396577, 184.300081278373);
    glVertex2i(243.2486033632649, 117.5778369260456);
    glVertex2i(0.0666627764122, 176.915037699036);

    glEnd();

    glColor3f(0.804, 0.522, 0.247);
    glBegin(GL_POLYGON);
    glVertex2i(695.5408168518136, 173.3467552244912);
    glVertex2i(794.5827760396577, 184.300081278373);
    glVertex2i(792.1720825450367, 121.2577039331763);
    glVertex2i(698.6101031231661, 98.9699313354026);

    glEnd();


    glColor3f(0.804, 0.522, 0.247);
    glBegin(GL_POLYGON);
    glVertex2i(695.5408168518136, 173.3467552244912);
    glVertex2i(794.5827760396577, 184.300081278373);
    glVertex2i(792.1720825450367, 121.2577039331763);
    glVertex2i(698.6101031231661, 98.9699313354026);

    glEnd();



    glColor3f(0.804, 0.48, 0.247);
    glBegin(GL_POLYGON);
    glVertex2i(301.5566779098028, 240.6212047726911);
    glVertex2i(0.0721628571076, 253.6969125190391);
    glVertex2i(0.3876223999244, 274.1505599455184);
    glVertex2i(24.0553707596413, 282.4176202837911);
    glVertex2i(24.0553707596413, 282.4176202837911);
    glVertex2i(73.6262675378297, 287.4652492684602);
    glVertex2i(73.6262675378297, 287.4652492684602);
    glVertex2i(83.8755677479946, 314.8570256248119);
    glVertex2i(83.8755677479946, 314.8570256248119);
    glVertex2i(125.6777323103224, 330.1680411802847);
    glVertex2i(125.6777323103224, 330.1680411802847);
    glVertex2i(143.7699175801913, 341.2879248937521);
    glVertex2i(143.7699175801913, 341.2879248937521);
    glVertex2i(171.8492606868996, 344.7821722958585);
    glVertex2i(171.8492606868996, 344.7821722958585);
    glVertex2i(199.945260263204, 356.1702895554217);
    glVertex2i(199.945260263204, 356.1702895554217);
    glVertex2i(221.6162023860378, 380.4775840994442);

    glVertex2i(221.6162023860378, 380.4775840994442);
    glVertex2i(247.315543825894, 412.0023694837946);

    glVertex2i(247.315543825894, 412.0023694837946);
    glVertex2i(279.0439701879349, 430.6799222192616);

    glVertex2i(279.0439701879349, 430.6799222192616);
    glVertex2i(302.1012738174926, 448.6002768708584);

    glVertex2i(302.1012738174926, 448.6002768708584);
    glVertex2i(397.2371035262056, 504.4729214762763);

    glVertex2i(397.2371035262056, 504.4729214762763);
    glVertex2i(486.6043708793637, 487.3007809159414);

    glVertex2i(486.6043708793637, 487.3007809159414);
    glVertex2i(495.6276973855742, 472.5068671471606);

    glVertex2i(495.6276973855742, 472.5068671471606);
    glVertex2i(504.3625805760025, 465.4748919082769);

    glVertex2i(504.3625805760025, 465.4748919082769);
    glVertex2i(514.8007601547579, 449.597710669813);

    glVertex2i(514.8007601547579, 449.597710669813);
    glVertex2i(558.2797086970871, 423.42734245968);

    glVertex2i(558.2797086970871, 423.42734245968);
    glVertex2i(604.9126416606699, 361.928927863539);

    glVertex2i(604.9126416606699, 361.928927863539);
    glVertex2i(644.8030442947965, 344.0782825772896);

    glVertex2i(644.8030442947965, 344.0782825772896);
    glVertex2i(660.4197177565928, 324.5225474237309);

    glVertex2i(660.4197177565928, 324.5225474237309);
    glVertex2i(690.4760334021076, 316.8673443808343);

    glVertex2i(690.4760334021076, 316.8673443808343);
    glVertex2i(716.60453507122, 298.0574169040024);

    glVertex2i(716.60453507122, 298.0574169040024);
    glVertex2i(733.113977035432, 295.1615407466484);
    glVertex2i(794.0837216933438, 250.3030372509864);
    glEnd();

    glColor3f(0.5, 0.32, 0.1);
    glBegin(GL_POLYGON);
    glVertex2i(480, 380);
    glVertex2i(540, 380);
    glVertex2i(543.4866972345103, 306.1670305975239);
    glVertex2i(479.0420599135231, 309.3752098676837);
    glVertex2i(480, 380);
    glVertex2i(479.0420599135231, 309.3752098676837);
    glVertex2i(540, 380);
    glVertex2i(543.4866972345103, 306.1670305975239);
    glEnd();

    //n
    glColor3f(0.545, 0.371, 0.075);
    glBegin(GL_POLYGON);
    glVertex2i(368.7331669670971, 432.5108072645035);
    glVertex2i(247.315543825894, 412.0023694837946);
    glVertex2i(302.1012738174926, 448.6002768708584);
    glVertex2i(378.7745881251958, 471.5360354967791);
    glVertex2i(514.8007601547579, 449.597710669813);
    glVertex2i(558.2797086970871, 423.42734245968);
    glVertex2i(570.9558066716595, 406.1229785552309);
    glEnd();

    //ground_green
    glColor3f(0.0, 0.3, 0.0);
    glBegin(GL_POLYGON);
    glVertex2i(240.0702694050909, 3.0919551012972);
    glVertex2i(792.1720825450367, 121.2577039331763);
    glVertex2i(801.0616171489811, 0);
    glVertex2i(240.0702694050909, 0);
    glEnd();


    glColor3f(0.0, 0.3, 0.0);
    glBegin(GL_POLYGON);
    glVertex2i(0.0649922257523, 98.6043376289714);
    glVertex2i(240.0702694050909, 3.0919551012972);
    glVertex2i(240.0702694050909, 0);
    glVertex2i(0, 0);

    glEnd();




    //line_stuffs
    glColor3f(0.0, 0.0, 0.1);
    glBegin(GL_LINES);
    glVertex2i(480, 380);
    glVertex2i(540, 380);
    glVertex2i(543.4866972345103, 306.1670305975239);
    glVertex2i(479.0420599135231, 309.3752098676837);
    glVertex2i(480, 380);
    glVertex2i(479.0420599135231, 309.3752098676837);
    glVertex2i(540, 380);
    glVertex2i(543.4866972345103, 306.1670305975239);
    glEnd();

    glVertex2i(368.7354529707912, 395.9195739870487);
    glVertex2i(368.7354529707912, 317.895814285932);

    glVertex2i(419.3753643819696, 393.9689799945208);
    glVertex2i(420.5176180228232, 317.4081657878);

    glVertex2i(332.4481100811757, 257.73387773159);
    glVertex2i(514.8544074449431, 257.786930481589);

    glVertex2i(332.4481100811757, 257.73387773159);
    glVertex2i(0.6356366718235, 266.1805908455514);

    glVertex2i(257.4639603460736, 391.9754836380131);
    glVertex2i(224.8457991010295, 364.9206614705003);
    glVertex2i(224.8457991010295, 364.9206614705003);
    glVertex2i(179.0781123132011, 342.8797907653605);
    glVertex2i(179.0781123132011, 342.8797907653605);
    glVertex2i(138.4215345117724, 313.5368435575948);


    glVertex2i(302.1012738174926, 448.6002768708584);
    glVertex2i(378.7745881251958, 471.5360354967791);
    glVertex2i(378.7745881251958, 471.5360354967791);
    glVertex2i(514.8007601547579, 449.597710669813);

    glVertex2i(247.315543825894, 412.0023694837946);
    glVertex2i(368.7331669670971, 432.5108072645035);
    glVertex2i(368.7331669670971, 432.5108072645035);
    glVertex2i(550.2888337446069, 407.9818716369132);

    glVertex2i(334.0351977497558, 394.5342896464377);
    glVertex2i(283.6620244581624, 348.4477778383038);
    glVertex2i(283.6620244581624, 348.4477778383038);
    glVertex2i(255.6969542976163, 342.6539540786108);
    glVertex2i(255.6969542976163, 342.6539540786108);
    glVertex2i(233.2334983391038, 317.60971548628);
    glEnd();


    glColor3f(0.0, 0.0, 0.1);
    glBegin(GL_LINES);
    glVertex2i(0.0666627764122, 176.915037699036);
    glVertex2i(243.2486033632649, 117.5778369260456);
    glVertex2i(240.0702694050909, 3.0919551012972);
    glVertex2i(0.0649922257523, 98.6043376289714);
    glEnd();

    //B
    glColor3f(0.0, 0.0, 0.1);

    glBegin(GL_LINES);
    glVertex2i(243.2486033632649, 117.5778369260456);
    glVertex2i(695.5408168518136, 173.3467552244912);
    glVertex2i(698.6101031231661, 98.9699313354026);
    glVertex2i(240.0702694050909, 3.0919551012972);
    glEnd();

    //C_STAIRS
    glColor3f(0.0, 0.0, 0.1);

    glBegin(GL_LINES);
    glVertex2i(695.5408168518136, 173.3467552244912);
    glVertex2i(794.5827760396577, 184.300081278373);
    glVertex2i(705.1026124683126, 161.3613288758556);
    glVertex2i(794.2204435870261, 173.6741595042159);
    glVertex2i(726.1612780408853, 145.9214301514037);
    glVertex2i(794.830839690579, 156.4752849757126);
    glVertex2i(744.4381008931247, 142.4114846377825);
    glVertex2i(794.6669862406899, 149.5367344780706);
    glVertex2i(739.0337271531968, 129.1788777915331);
    glVertex2i(794.6669862406899, 138.5434918673403);
    glVertex2i(758.7437960870515, 123.4786779193026);
    glVertex2i(794.5080340718685, 132.4361348613791);
    glVertex2i(759.0617004246943, 113.2997495760338);
    glVertex2i(792.1720825450367, 121.2577039331763);
    glVertex2i(698.6101031231661, 98.9699313354026);
    glVertex2i(759.0617004246943, 113.2997495760338);
    glEnd();



//F_OUTFIGURE
    glColor3f(0.0, 0.0, 0.1);

    glBegin(GL_LINES);
    glVertex2i(0.3876223999244, 274.1505599455184);
    glVertex2i(24.0553707596413, 282.4176202837911);

    glVertex2i(24.0553707596413, 282.4176202837911);
    glVertex2i(73.6262675378297, 287.4652492684602);

    glVertex2i(73.6262675378297, 287.4652492684602);
    glVertex2i(83.8755677479946, 314.8570256248119);

    glVertex2i(83.8755677479946, 314.8570256248119);
    glVertex2i(125.6777323103224, 330.1680411802847);

    glVertex2i(125.6777323103224, 330.1680411802847);
    glVertex2i(143.7699175801913, 341.2879248937521);

    glVertex2i(143.7699175801913, 341.2879248937521);
    glVertex2i(171.8492606868996, 344.7821722958585);

    glVertex2i(171.8492606868996, 344.7821722958585);
    glVertex2i(199.945260263204, 356.1702895554217);

    glVertex2i(199.945260263204, 356.1702895554217);
    glVertex2i(221.6162023860378, 380.4775840994442);

    glVertex2i(221.6162023860378, 380.4775840994442);
    glVertex2i(247.315543825894, 412.0023694837946);

    glVertex2i(247.315543825894, 412.0023694837946);
    glVertex2i(279.0439701879349, 430.6799222192616);

    glVertex2i(279.0439701879349, 430.6799222192616);
    glVertex2i(302.1012738174926, 448.6002768708584);

    glVertex2i(302.1012738174926, 448.6002768708584);
    glVertex2i(397.2371035262056, 504.4729214762763);

    glVertex2i(397.2371035262056, 504.4729214762763);
    glVertex2i(486.6043708793637, 487.3007809159414);

    glVertex2i(486.6043708793637, 487.3007809159414);
    glVertex2i(495.6276973855742, 472.5068671471606);

    glVertex2i(495.6276973855742, 472.5068671471606);
    glVertex2i(504.3625805760025, 465.4748919082769);

    glVertex2i(504.3625805760025, 465.4748919082769);
    glVertex2i(514.8007601547579, 449.597710669813);

    glVertex2i(514.8007601547579, 449.597710669813);
    glVertex2i(558.2797086970871, 423.42734245968);

    glVertex2i(558.2797086970871, 423.42734245968);
    glVertex2i(604.9126416606699, 361.928927863539);

    glVertex2i(604.9126416606699, 361.928927863539);
    glVertex2i(644.8030442947965, 344.0782825772896);

    glVertex2i(644.8030442947965, 344.0782825772896);
    glVertex2i(660.4197177565928, 324.5225474237309);

    glVertex2i(660.4197177565928, 324.5225474237309);
    glVertex2i(690.4760334021076, 316.8673443808343);

    glVertex2i(690.4760334021076, 316.8673443808343);
    glVertex2i(716.60453507122, 298.0574169040024);

    glVertex2i(716.60453507122, 298.0574169040024);
    glVertex2i(733.113977035432, 295.1615407466484);

    glVertex2i(733.113977035432, 295.1615407466484);
    glVertex2i(768.3912199078596, 264.7332127058891);

    glVertex2i(768.3912199078596, 264.7332127058891);
    glVertex2i(794.0837216933438, 250.3030372509864);

    glVertex2i(794.0837216933438, 250.3030372509864);
    glVertex2i(792.1720825450367, 121.2577039331763);
    glEnd();

//G_INNER_DESIGN

    glColor3f(0.0, 0.0, 0.1);

    glBegin(GL_LINES);
    glVertex2i(333.3647099310783, 312.481138783244);
    glVertex2i(138.4215345117724, 313.5368435575948);

    glVertex2i(333.3647099310783, 312.481138783244);
    glVertex2i(733.113977035432, 295.1615407466484);
    glEnd();

    glColor3f(0.0, 0.0, 0.1);
    glBegin(GL_LINES);
    glVertex2i(550.2888337446069, 407.9818716369132);
    glVertex2i(368.7331669670971, 432.5108072645035);
    glVertex2i(368.7331669670971, 432.5108072645035);
    glVertex2i(247.315543825894, 412.0023694837946);
    glEnd();



    glColor3f(0.0, 0.0, 0.1);
    glBegin(GL_LINES);
    glVertex2i(514.8007601547579, 449.597710669813);
    glVertex2i(378.7745881251958, 471.5360354967791);
    glVertex2i(378.7745881251958, 471.5360354967791);
    glVertex2i(302.1012738174926, 448.6002768708584);
    glEnd();


    glColor3f(0.0, 0.0, 0.1);
    glBegin(GL_LINES);
    glVertex2i(334.0351977497558, 394.5342896464377);
    glVertex2i(283.6620244581624, 348.4477778383038);
    glVertex2i(283.6620244581624, 348.4477778383038);
    glVertex2i(255.6969542976163, 342.6539540786108);
    glVertex2i(255.6969542976163, 342.6539540786108);
    glVertex2i(233.2334983391038, 317.60971548628);
    glEnd();


    glColor3f(0.0, 0.0, 0.1);
    glBegin(GL_LINES);
    glVertex2i(257.4639603460736, 391.9754836380131);
    glVertex2i(224.8457991010295, 364.9206614705003);
    glVertex2i(224.8457991010295, 364.9206614705003);
    glVertex2i(179.0781123132011, 342.8797907653605);
    glVertex2i(179.0781123132011, 342.8797907653605);
    glVertex2i(138.4215345117724, 313.5368435575948);
    glVertex2i(0.3876223999244, 274.1505599455184);
    glVertex2i(334.648519837944, 265.3143029812256);
    glVertex2i(334.648519837944, 265.3143029812256);
    glVertex2i(755.4441546765084, 278.656249185365);


    glEnd();

    //D_stairs
    glColor3f(0.0, 0.0, 0.1);
    glBegin(GL_LINES);
    glVertex2i(0.2821501400043, 243.2617175260415);
    glVertex2i(301.4571824426608, 215.9228627351919);
    glVertex2i(0.0092185206099, 212.9672530121446);
    glVertex2i(301.3970738888992, 169.6796858723581);
    glVertex2i(301.4571824426608, 215.9228627351919);
    glVertex2i(585.0268009807223, 229.319849815714);
    glVertex2i(301.3970738888992, 169.6796858723581);
    glVertex2i(585.7311242379861, 195.7984480440141);
    glVertex2i(0.0721628571076, 253.6969125190391);
    glVertex2i(301.5566779098028, 240.6212047726911);
    glVertex2i(301.5566779098028, 240.6212047726911);
    glVertex2i(794.0837216933438, 250.3030372509864);
    glVertex2i(609.4473802030293, 236.5881765150581);
    glVertex2i(690, 240);
    glVertex2i(629.0054584639315, 226.0217978551925);
    glVertex2i(702.2062193831956, 228.9015564249824);
    glVertex2i(643.8833990431197, 214.4028816431434);
    glVertex2i(714.3881936598688, 218.8820121730539);
    glVertex2i(661.3697072317975, 202.7571422653761);
    glVertex2i(730, 210);
    glVertex2i(673.7664997425932, 192.7052576111899);
    glVertex2i(745.208418483989, 198.1636263254803);
    glVertex2i(585.7311242379861, 195.7984480440141);
    glVertex2i(673.8751031632275, 185.2968354966565);
    glVertex2i(673.8751031632275, 185.2968354966565);
    glVertex2i(746.0572503057066, 190.7556430229012);
    glVertex2i(585.0268009807223, 229.319849815714);
    glVertex2i(609.4473802030293, 236.5881765150581);
    glVertex2i(615.3280119723826, 227.6328294214782);
    glVertex2i(629.0054584639315, 226.0217978551925);
    glEnd();



    glFlush();
}
