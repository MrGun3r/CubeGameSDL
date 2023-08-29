#include <stdio.h>   
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include<time.h>


#define PI (3.141592653589793)
// CODE RULES :
// ALL OBJECTS MUST USE THE SAME VARIABLE NAME THAT MEAN THE SAME THING 
// FOR POLYMORPHISM REASONS.
float last_frame = 0;
float deltaTime = 0;
int running = 0;
const int maxFPS = 0;
int fps = 0;
int main_menu = 1;
int paused = 0;
int pauseBool = 0;
const int windowHeight = 800;
const int windowWidth = 1000;
const int strokeWidth = 2; 
int timeShot = 0;
float triangleAngle = 0;
int StartTicks;
int PauseTicks;
float gameClock= 0;
int gameClock_Seconds = 0;
int gameClock_Minutes = 0;
//MAIN MENU
float ButtonSelectorY;
float ButtonSelectorX;
int ButtonSelectorSizeX = 0;
int ButtonSelectorSizeY = 0;
int ButtonSelectorNUM;
float ButtonSelectorOpacity;
// HUD 
float HUDOpacity;
float AlertDown = 0;
int AlertIndexOldest = 0;
int upgradeTab = 0;
int upgradeTabBool = 0;
float previewPlayerAngle = 0;
// enemy spawn rate
int enemy_spawnRate = 500; // ms
int enemy_timeSpawn = 0;
int enemy_increaseRate = 30000; // ms
int enemy_increaseTime = 0;
int enemy_max = 10;
int enemy_present = 0;
int enemy_baseEnragement = 0;
SDL_Renderer* renderer = NULL;
SDL_Window* window = NULL;
TTF_Font* font = NULL;
TTF_Font* font_outline = NULL;
// INIT SURFACE AND TEXTURE ATLASES
SDL_Surface * TEXT_Surface = NULL;
SDL_Texture * TEXT_Texture = NULL;
SDL_Surface * POWERUPS_Surface = NULL;
SDL_Texture * POWERUPS_Texture = NULL;

// COLORS 
int colorWHITE[3] = { 255, 255, 255 };
int colorBLACK[3] = { 0, 0, 0 };
int colorLIGHTGRAY[3] = {210,210,210};
int colorGREEN[3] = {0,200,0};
int colorRED[3] = {200,0,0};
int colorBLUE[3] = {3, 165, 252}; // 3, 165, 252
int colorYELLOW[3] = {212, 245, 2};
// TEXT DATA 
char *TEXT_reloading = "reloading";
// OTHER DATA 
int colorRectOutline[4];
int colorRect[4];
float deathscreenTime;
int gameOver;


// STORE KEY BOOLEANS (ON PRESS)
struct keys {
  // KEYBOARD
  int w;
  int s;
  int d;
  int a;
  int r;
  int escape;
  int tab;
  // MOUSE
  int left;
  int right;
} key;
// INFO
struct mousePosition {
  int x;
  int y;
} mouse;

// CAMERA

struct cameraStruct{
  float x;
  float y;
} camera;

// WORLD BORDER
struct worldBorderStruct{
  float x;
  float y;
} worldBorder;

// OBJECTS 

struct rectangle {
    float x;
    float y;
    float width;
    float height;
    float veloX;
    float veloY;
    float speed;
    float angle;
    float knockbackX;
    float knockbackY;
    float knockbackMulti;
    float health;
    float healthMax;
    float healthRegenTime;
    float healingTime;
    float CountMag;
    float mag;
    float magMax;
    float bulletDmg;
    float fireRate;
    float reloadTime;
    int reloading;
    float shield;
    float shieldMax;
    float shieldPower;
    float credit;
    float creditOpacity;
    float credittimeShown;
    float luck;
    int dead;
} rect;

struct bullet {
  int reserve;
  float x;
  float y;
  float width;
  float height;
  float veloX;
  float veloY;
  int emitter;
  int index;
  short r;
  short g;
  short b;
};
struct enemy{
  int reserve;
  int type;
  float x;
  float y;
  float width;
  float height;
  float veloX;
  float veloY;
  float knockbackX;
  float knockbackY;
  float knockbackMulti;
  float health;
  float healthMax;
  int healthbar;
  float angle;
  float hitDamage;
  float speed;// TRACK PLAYER
  float enragement; 
  float fireRate;
  float fireRateTime;
};

struct particle{
  int reserve;
  float x;
  float y;
  float width;
  float height;
  float veloX;
  float veloY;
  float angle;
  float angleVelo;
  float opacity;
};
struct obstacle{
  int reserve;
  float x;
  float y;
  float width;
  float height;
};
struct bush{
  int reserve;
  float x;
  float y;
  float width;
  float height;
  float angle;
};
struct item{
  int reserve;
  float x;
  float y;
  float width;
  float height;
  float veloX;
  float veloY;
  float angle;
  float angleVelo;
  int type;
};
struct alert{
  int reserve;
  float x;
  float y;
  float opacity;
  float timeShown;
  float maxtime;
  int* color;
  char *text;
  int size;
};
struct credit{
  int reserve;
  float x;
  float y;
  float veloX;
  float veloY;
  float rectPullX;
  float rectPullY;
  float width;
  float height;
  float angle;
  float angleVelo;
  float distanceTorect;
  float angleToRect;
};

struct upgradeTile{
  int type;
  int index;
  float price;
  float buttonHover;
  float value;
  char* UpgradeText;
  char* Description;
  char* CurrentValue;
  int UpgradeTextSize;
  int CurrentValueSize;
  int DescriptionSize;
  float valueIncrement;
  char* alertMessage;
  int alertMessageSize;
  SDL_Rect RectSrc;
};
struct particle particles[100];
struct bullet bullets[50];
struct enemy enemies[200];
struct obstacle obstacles[10];
struct bush bushes[100];
struct item items[300];
struct alert alerts[4];
struct credit credits[150];
struct upgradeTile upgrades[5];
// MATH 
int* rotatePoint(float angle , int x , int y, int ogX, int ogY ){
  int* arr = malloc(8);
  arr[0] = (x-ogX)*SDL_cos(angle)-(y-ogY)*SDL_sin(angle) + ogX; // x componant
  arr[1] = (x-ogX)*SDL_sin(angle)+(y-ogY)*SDL_cos(angle) + ogY; // y componant 
  return arr;
}
int intersectingLines(int x1, int y1,int x2,int y2,int x3,int y3,int x4,int y4){
  // 1 and 2 line and 3 and 4 line
   
   float uA1 = ((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3));
   float uA2 = ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1)); 
   float uA = uA1/uA2;
   
   //double uB = ((x2-x1)*(y1-y3) - (y2-y1)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));
   
   if (uA > 0 && uA <= 1 ){
    return (int)((double)x1 +((double)uA*(double)(x2-x1)));
  }
  return -99;
}
int collisionCheck(float ax,float ay, float awidth, float aheight,float bx,float by,float bwidth,float bheight){
  return ax+awidth > bx && bx+bwidth > ax && ay+aheight > by && by+bheight > ay;
}
void collisionCorrection(float *ax,float *ay, float *awidth, float *aheight,float *bx,float *by,float *bwidth,float *bheight){
  // ACTION TAKEN BY "a"
  
  if (SDL_min(SDL_abs(*ax+*awidth-*bx),SDL_abs(*ax-*bx-*bwidth))>SDL_min(SDL_abs(*ay+*aheight-*by),SDL_abs(*ay-*by-*bheight)))   
    {
      if (SDL_abs(*ay + *aheight - *by)>SDL_abs(*ay - *by - *bheight)){
        *ay = *by + *bheight;
       }
       else {*ay =* by - *aheight;
        }
    }
    else {
      if (SDL_abs(*ax + *awidth -*bx)>SDL_abs(*ax - *bx - *bwidth)){
      *ax = *bx + *bwidth;   
     }
     else {*ax = *bx - *awidth;
      }
    }
}
void fillPOLYGON(int* point1,int* point2,int* point3,int* point4 ,int y,int* color,int staticCamera){
  // FUNCTION USED FOR RECT ONLY
  int height = SDL_max(SDL_max(point1[1],point2[1]),SDL_max(point3[1],point4[1]));


  SDL_SetRenderDrawColor(renderer,color[0],color[1],color[2],color[3]);
  for(int i = -height/PI;i<height*PI;i++){
      int inter1 = -99;
      int inter2 = -99;
      int inter3 = -99;
      int inter4 = -99;
      int count = 0;
      if(SDL_min(point2[1]-camera.y*staticCamera,point1[1]-camera.y*staticCamera)<=y+i && SDL_max(point2[1]-camera.y*staticCamera,point1[1]-camera.y*staticCamera)>=y+i){
        inter1 = intersectingLines(-99,y+i,windowWidth+99,y+i,point1[0]-camera.x*staticCamera,point1[1]-camera.y*staticCamera,point2[0]-camera.x*staticCamera,point2[1]-camera.y*staticCamera);
        count++;
      }  
      if(SDL_min(point2[1]-camera.y*staticCamera,point3[1]-camera.y*staticCamera)<=y+i && SDL_max(point2[1]-camera.y*staticCamera,point3[1]-camera.y*staticCamera)>=y+i){
        inter2 = intersectingLines(-99,y+i,windowWidth+99,y+i,point2[0]-camera.x*staticCamera,point2[1]-camera.y*staticCamera,point3[0]-camera.x*staticCamera,point3[1]-camera.y*staticCamera);
        count++;
      }
      if(SDL_min(point4[1]-camera.y*staticCamera,point3[1]-camera.y*staticCamera)<=y+i && SDL_max(point4[1]-camera.y*staticCamera,point3[1]-camera.y*staticCamera)>=y+i){
        inter3 = intersectingLines(-99,y+i,windowWidth+99,y+i,point3[0]-camera.x*staticCamera,point3[1]-camera.y*staticCamera,point4[0]-camera.x*staticCamera,point4[1]-camera.y*staticCamera);
        count++;
      }
      if(SDL_min(point4[1]-camera.y*staticCamera,point1[1]-camera.y*staticCamera)<=y+i && SDL_max(point4[1]-camera.y*staticCamera,point1[1]-camera.y*staticCamera)>=y+i){
        inter4 = intersectingLines(-99,y+i,windowWidth+99,y+i,point4[0]-camera.x*staticCamera,point4[1]-camera.y*staticCamera,point1[0]-camera.x*staticCamera,point1[1]-camera.y*staticCamera);
        count++;
      }
      int intersections[] = {inter1,inter2,inter3,inter4};
      
      if(count < 2){
        continue;
      }
      
      int pt1 = -99;
      int pt2 = -99;
      for(int j = 0;j<4;j++){
        if(intersections[j] == -99){
          continue;
        }

        if(pt1 == -99){
          pt1 = intersections[j];
        }
        else if(pt2 == -99 && intersections[j] != pt1){
          pt2 = intersections[j];
        } 
      }
      if(pt1 == -99){
        pt1 = pt2;
      }
      else if (pt2 == -99){
        pt2 = pt1;
      }
      SDL_RenderDrawLine(renderer,pt1,y+i,pt2,y+i);    
    }
}




int initGame(void){
  
   if(SDL_Init(SDL_INIT_VIDEO) != 0){
    printf("SDL unsupported !");
    return 0;
   }
   window = SDL_CreateWindow("Game",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
   windowWidth,windowHeight,0);

   if(!window){
    printf("Window Closed");
    return 0;
   }
   renderer = SDL_CreateRenderer(window,-1,0);
   if(!renderer){
    printf("Renderer Closed");
    return 0;
   }
   TTF_Init();
   IMG_Init(IMG_INIT_PNG);
   // INIT PNG FONT ATLAS
   TEXT_Surface = IMG_Load("assets/font-atlas.png");
   POWERUPS_Surface = IMG_Load("assets/powerups.png");
   TEXT_Texture = SDL_CreateTextureFromSurface(renderer,TEXT_Surface);
   POWERUPS_Texture = SDL_CreateTextureFromSurface(renderer,POWERUPS_Surface);
   SDL_FreeSurface(TEXT_Surface);
   SDL_FreeSurface(POWERUPS_Surface);
    
   // RANDOM NUMBER GENERATION
   
   srand(time(NULL));

   return 1;



}
void kill(void){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_CloseFont(font_outline);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
void shoot(void){
  int i;
  for(i = 0;i<sizeof(bullets)/sizeof(bullets[0]);i++){
      if(!bullets[i].reserve){
        break;
      }
    }
    
  bullets[i].r = 242;
  bullets[i].g = 200;
  bullets[i].b = 145+(int)rect.shield*1.1f;
  bullets[i].x = rect.x + rect.width/2;
  bullets[i].y = rect.y + rect.height/2;
  bullets[i].veloX = -500*SDL_cos(rect.angle); // PER SECOND
  bullets[i].veloY = -500*SDL_sin(rect.angle); // PER SECOND
  bullets[i].reserve = 1;
  bullets[i].width = 3.0f;
  bullets[i].height = 3.0f;
  bullets[i].emitter = 0;
}
void input(void){
    int releasedLeft = 0;
    int releasedRight = 0;
    SDL_Event event;
    SDL_PollEvent(&event);
    

    if(event.type == SDL_QUIT){
      running = 0;
    }
    if(event.type == SDL_KEYDOWN){
      switch(event.key.keysym.sym){
        case SDLK_w:{
          key.w = 1;
          break;
        }    
        case SDLK_s:{
          key.s = 1;
          break;
        }
        case SDLK_d:{
          key.d = 1;
          break;
        }        
        case SDLK_a:{
          key.a = 1;
          break;
        }    
        case SDLK_r:{
          key.r = 1;
          break;
        }
        case SDLK_ESCAPE:{
          key.escape = 1;
          break;
        }   
        case SDLK_TAB:{
          key.tab = 1;
          break;
        }   
      }
    }
    if(event.type == SDL_KEYUP){
       switch(event.key.keysym.sym){
        case SDLK_w:{
          key.w = 0;
          break;
        }       
        case SDLK_s:{
          key.s = 0;
          break;
        }      
        case SDLK_d:{
          key.d = 0;
          break;
        }
        case SDLK_a:{
           key.a = 0;
           break;
        }
        case SDLK_r:{
          key.r = 0;
          break;
        }
        case SDLK_ESCAPE:{
          key.escape = 0;
          break;
        }
        case SDLK_TAB:{
          key.tab = 0;
          break;
        }    
        }
   

    }
    if(event.type == SDL_MOUSEMOTION){
       SDL_GetMouseState(&mouse.x,&mouse.y);
       if(!paused){
            rect.angle = SDL_atan((rect.y-camera.y+rect.height/2 - mouse.y)/(rect.x-camera.x+rect.width/2 - mouse.x));
        if((float)mouse.x > rect.x-camera.x+rect.width/2){
          rect.angle += PI;
    }   
       }
    
   }
    if(event.type == SDL_MOUSEBUTTONDOWN){
      if(event.button.button == SDL_BUTTON_LEFT){
        key.left = 1;
      }
      if(event.button.button == SDL_BUTTON_RIGHT){
        key.right = 1;
      }
    
    }
    if(event.type == SDL_MOUSEBUTTONUP){
      if(event.button.button == SDL_BUTTON_LEFT){
        key.left = 2;
        releasedLeft = 1;
      }
      if(event.button.button == SDL_BUTTON_RIGHT){
        key.right = 2;
        releasedRight = 1;
      }
    }
    if(key.left == 2 && !releasedLeft){
      key.left = 0;
    }
    if(key.right == 2 && !releasedRight){
      key.right = 0;
    }
    
}
void renderNum(int stringCount,int Value,int x ,int y, int width,int height,int opacity,int* color){
  // DEAL WITH RENDERING NUMBERS ONLY
   
    char str[stringCount]; 
    int digits;
    if(Value > 0){
       digits = (int)SDL_floor(SDL_log10(Value)+1);
    }
    else {
       digits = 1;
    }
    int posX;
    sprintf(str,"%d",Value);
    for (int i = 0;i<digits;i++){
      int charValue = str[i] - '0';
      posX = charValue*40;
    SDL_Rect srcRect = {
      posX,
      150,
      40,
      70
    };
    SDL_Rect dstRect = {
      x+i*(width/digits)-2,
      y-2,
      width/stringCount+1,
      height+1
    };
    SDL_SetTextureAlphaMod( TEXT_Texture, opacity);
    SDL_SetTextureColorMod( TEXT_Texture, 0, 0, 0 );
    SDL_RenderCopy(renderer,TEXT_Texture,&srcRect,&dstRect);
    } 
    // OUTLINE
    for (int i = 0;i<digits;i++){
      int charValue = str[i] - '0';
      posX = charValue*40;
    SDL_Rect srcRect = {
      posX,
      150,
      40,
      70
    };
    SDL_Rect dstRect = {
      x+i*(width/digits),
      y,
      width/stringCount,
      height
    };
    SDL_SetTextureAlphaMod( TEXT_Texture, opacity);
    SDL_SetTextureColorMod( TEXT_Texture, color[0], color[1],color[2]);
    SDL_RenderCopy(renderer,TEXT_Texture,&srcRect,&dstRect);
    }  
    SDL_SetTextureAlphaMod( TEXT_Texture, 255);
}
void renderText(int stringCount,char *Text,int x ,int y, int width,int height,int opacity,int* color){
  // DEALS WITH TEXT AS WELL AS NUMBERS
  
  char str[stringCount];
  for(int i = 0;i<stringCount;i++){
    str[i] = *(Text+i);
  }
  int posX;
  int cap;
  int num;
  SDL_SetTextureAlphaMod(TEXT_Texture,opacity); 
  // OUTLINE --> TEXT (order of rendering) 
   // {outline}
  for(int i = 0;i<stringCount;i++){
      cap = 0;
      num = 0;
      int charValue = str[i] - 'a';
      if (str[i]>=65 && str[i]<=90){
        cap = 1;
        num = 0;
        charValue += 32;
      }
      else if (str[i]>= 48 && str[i]<= 57){
        num = 1;
        cap = 0;
        charValue += 49;
      }
      posX = charValue*40;
      SDL_Rect srcRect = {
      posX,
      80*cap+150*num,
      40,
      70-(6*cap)
    };
    int deviation = SDL_floor((width/(stringCount*5))+1);
     SDL_Rect OUTLINEdstRect = {
      x+i*(width/stringCount)-deviation,
      y-deviation,
      width/stringCount+deviation,
      height+deviation
    };
    SDL_SetTextureColorMod( TEXT_Texture, 0,0,0);
    SDL_RenderCopy(renderer,TEXT_Texture,&srcRect,&OUTLINEdstRect);
    }
    // {text}
  for (int i = 0;i<stringCount;i++){
      cap = 0;
      num = 0;
      int charValue = str[i] - 'a';
      if (str[i]>=65 && str[i]<=90){
        cap = 1;
        num = 0;
        charValue += 32;
      }
      else if (str[i]>= 48 && str[i]<= 57){
        num = 1;
        cap = 0;
        charValue += 49;
      }
      posX = charValue*40;
    
    SDL_Rect srcRect = {
      posX,
      80*cap+150*num,
      40,
      70 -(6*cap)
    };
    SDL_Rect dstRect = {
      x+i*(width/stringCount)
      ,y
      ,width/stringCount
      ,height
    };
    
    
    SDL_SetTextureColorMod( TEXT_Texture, color[0],color[1],color[2]);
    SDL_RenderCopy(renderer,TEXT_Texture,&srcRect,&dstRect);
    }  
    SDL_SetTextureAlphaMod(TEXT_Texture,255);
}
void alertCall(int stringCount,char* Text,int* color){
  int alertBool = 0;
  for(int i = 0;i<sizeof(alerts)/sizeof(alerts[0]);i++){
    if(i >= sizeof(alerts)/sizeof(alerts[0])-1 && alerts[i].reserve){
      int oldestIndex = 0;
      int alertCount = 0;
     for(int j = 0;j<sizeof(alerts)/sizeof(alerts[0]);j++){
      if(alerts[j].reserve){
        alertCount++;
        if(alerts[oldestIndex].y < alerts[j].y){
          oldestIndex = j;
        }
      }
        
      }
      if(alerts[oldestIndex].y >= alertCount*20 - 21){
    alerts[oldestIndex].reserve = 0;
      i = oldestIndex;
      alertBool = 1;
      }
      else {
        break;
      }
    }
   
    if(!alerts[i].reserve){
      int negativeY = 0;
     for(int j = 0;j<sizeof(alerts)/sizeof(alerts[0]);j++){
       if(alerts[j].reserve && i != j){
         if(alerts[j].y<negativeY){
           negativeY = alerts[j].y;
         }
       }
     }
      alerts[i].reserve = 1;
      alerts[i].x = 0;
      alerts[i].y = -20+negativeY;
      alerts[i].opacity = 0.0f;
      alerts[i].size = stringCount;
      alerts[i].text = Text;
      alerts[i].timeShown = 0.0f;
      alerts[i].maxtime = 3000.0f;
      alerts[i].color = color;
      AlertDown += 20;
      break;
    }
  }
}
void draw(void){
  
 SDL_SetRenderDrawColor(renderer,27,148,6,255); // 27, 148, 6
 SDL_RenderClear(renderer);
  // SOME BACKGROUND 
 SDL_SetRenderDrawColor(renderer,0,70,0,255); 
 for(int i = 0;i<sizeof(bushes)/sizeof(bushes[0]);i++){
 if(bushes[i].reserve && (bushes[i].x+ bushes[i].width > camera.x && bushes[i].x<windowWidth+camera.x && bushes[i].y+ bushes[i].height > camera.y && bushes[i].y<windowHeight+camera.y) ){
   SDL_Rect BushThing = {
   (int)(bushes[i].x-camera.x),
   (int)(bushes[i].y-camera.y),
   (int)(bushes[i].width),
   (int)(bushes[i].height)
   };
 SDL_RenderDrawRect(renderer,&BushThing);  
 SDL_RenderFillRect(renderer,&BushThing);
  }
 }

 
 // DRAW ITEMS 
 for (int i = 0;i<sizeof(items)/sizeof(items[0]);i++){
  int SRCx,SRCy;
  if(items[i].reserve){
  
  SDL_Rect powerUpRect = {
     (int)items[i].x-camera.x,
     (int)items[i].y-camera.y,
     (int)items[i].width,
     (int)items[i].height

  };
  // HEALTH
  if(items[i].type == 0){
    SRCx = 0;
    SRCy = 0;
  }
  // BULLETS
  if(items[i].type == 1){
    SRCy = 0;
    SRCx = 200;
  }
  // SHIELD
  if(items[i].type == 2){
    SRCx = 0;
    SRCy = 200;
  }
  // PLAYER SPEED
  if(items[i].type == 3){
    SRCx = 200;
    SRCy = 200;
  }
  SDL_Rect powerUpRectSrc = {
   SRCx,SRCy,200,200
  };
 SDL_RenderCopyEx(renderer,POWERUPS_Texture,&powerUpRectSrc,&powerUpRect,items[i].angle,NULL,SDL_FLIP_NONE);
  }
 }
 // DRAW CREDITS 
  for(int i = 0 ;i<sizeof(credits)/sizeof(credits[0]);i++){
    if(credits[i].reserve){
    int *credits_point1 = rotatePoint(credits[i].angle,(int)credits[i].x,(int)credits[i].y,(int)credits[i].x+credits[i].width/2,(int)credits[i].y+credits[i].height/2);
    int *credits_point2 = rotatePoint(credits[i].angle,(int)credits[i].x+credits[i].width,(int) credits[i].y,(int)(credits[i].x+credits[i].width/2),(int)(credits[i].y+credits[i].height/2));
    int *credits_point3 = rotatePoint(credits[i].angle,(int)credits[i].x+credits[i].width,(int) credits[i].y+credits[i].height,(int)(credits[i].x+credits[i].width/2),(int)(credits[i].y+credits[i].height/2));
    int *credits_point4 = rotatePoint(credits[i].angle,(int)credits[i].x,(int)credits[i].y+credits[i].height,(int)(credits[i].x+credits[i].width/2),(int)(credits[i].y+credits[i].height/2));

    int colorYellow[4] = { 233, 240, 34,255};
    fillPOLYGON(credits_point1,credits_point2,credits_point3,credits_point4,credits[i].y-camera.y,colorYellow,1);
    
     free(credits_point1);
     free(credits_point2);
     free(credits_point3);
     free(credits_point4);
    }
  }
 //DRAW PARTICLES 
  for(int i = 0;i<sizeof(particles)/sizeof(particles[0]);i++){
   if(particles[i].reserve && (particles[i].x+ particles[i].width > camera.x && particles[i].x<windowWidth+camera.x && particles[i].y+ particles[i].height > camera.y && particles[i].y<windowHeight+camera.y)){
       
    int *particle_point1 = rotatePoint(particles[i].angle,(int)particles[i].x,(int)particles[i].y,(int)particles[i].x+particles[i].width/2,(int)particles[i].y+particles[i].height/2);
    int *particle_point2 = rotatePoint(particles[i].angle,(int)particles[i].x+particles[i].width,(int)particles[i].y,(int)(particles[i].x+particles[i].width/2),(int)(particles[i].y+particles[i].height/2));
    int *particle_point3 = rotatePoint(particles[i].angle,(int)particles[i].x+particles[i].width,(int)particles[i].y+particles[i].height,(int)(particles[i].x+particles[i].width/2),(int)(particles[i].y+particles[i].height/2));
    int *particle_point4 = rotatePoint(particles[i].angle,(int)particles[i].x,(int)particles[i].y+particles[i].height,(int)(particles[i].x+particles[i].width/2),(int)(particles[i].y+particles[i].height/2));

    int colorRed[4] = {200,0,0,(int)particles[i].opacity};
    fillPOLYGON(particle_point1,particle_point2,particle_point3,particle_point4,particles[i].y-camera.y,colorRed,1);
     free(particle_point1);
     free(particle_point2);
     free(particle_point3);
     free(particle_point4);
   
   }
    
  }
 {
  if(!rect.dead){
     int *EXpoint1 = rotatePoint(rect.angle,(int)rect.x-strokeWidth,(int)rect.y-strokeWidth,(int)rect.x+rect.width/2,(int)rect.y+rect.height/2);
  int *EXpoint2 = rotatePoint(rect.angle,(int)rect.x+rect.width+strokeWidth,(int)rect.y-strokeWidth,(int)rect.x+rect.width/2,(int)rect.y+rect.height/2);
  int *EXpoint3 = rotatePoint(rect.angle,(int)rect.x+rect.width+strokeWidth,(int)rect.y+rect.height+strokeWidth,(int)rect.x+rect.width/2,(int)rect.y+rect.height/2);
  int *EXpoint4 = rotatePoint(rect.angle,(int)rect.x-strokeWidth,(int)rect.y+rect.height+strokeWidth,(int)rect.x+rect.width/2,(int)rect.y+rect.height/2);
  int colorRectOutline[4] = {150,150,150+(int)rect.shield*1.05f,255};
  fillPOLYGON(EXpoint1,EXpoint2,EXpoint3,EXpoint4,(int)rect.y-camera.y,colorRectOutline,1);
  int *point1 = rotatePoint(rect.angle,(int)rect.x,(int)rect.y,(int)rect.x+rect.width/2,(int)rect.y+rect.height/2);
  int *point2 = rotatePoint(rect.angle,(int)rect.x+rect.width,(int)rect.y,(int)rect.x+rect.width/2,(int)rect.y+rect.height/2);
  int *point3 = rotatePoint(rect.angle,(int)rect.x+rect.width,(int)rect.y+rect.height,(int)rect.x+rect.width/2,(int)rect.y+rect.height/2);
  int *point4 = rotatePoint(rect.angle,(int)rect.x,(int)rect.y+rect.height,(int)rect.x+rect.width/2,(int)rect.y+rect.height/2);
  // FILL RECT WITH ROTATION (uncertain) // 
  int colorRect[4] = {242,200,145+(int)rect.shield*1.1f,255}; // 242, 200, 145
  fillPOLYGON(point1,point2,point3,point4,(int)rect.y-camera.y,colorRect,1); 
  
   free(point1);
   free(point2);
   free(point3);
   free(point4);
   free(EXpoint1);
   free(EXpoint2);
   free(EXpoint3);
   free(EXpoint4);
  }
   }
   // DRAW BULLETS IF AVAILABLE
   for(int i = 0;i<sizeof(bullets)/sizeof(bullets[0]);i++){
     if(bullets[i].reserve && (bullets[i].x+ 3.0f > camera.x && bullets[i].x<windowWidth+camera.x && bullets[i].y+ 3.0f > camera.y && bullets[i].y<windowHeight+camera.y)){
       SDL_Rect bulletRect = {
       (int)(bullets[i].x-camera.x-1),
       (int)(bullets[i].y-camera.y-1),
       (int)bullets[i].width,
       (int)bullets[i].height
     };
     SDL_SetRenderDrawColor(renderer,bullets[i].r,bullets[i].g,bullets[i].b,255);
     SDL_RenderDrawRect(renderer,&bulletRect);
     SDL_RenderFillRect(renderer,&bulletRect);
     }     
   }
   // DRAW ENEMIES
   for(int i = 0;i<sizeof(enemies)/sizeof(enemies[0]);i++){
     if(enemies[i].reserve && (enemies[i].x+ enemies[i].width > camera.x && enemies[i].x<windowWidth+camera.x && enemies[i].y+ enemies[i].height > camera.y && enemies[i].y<windowHeight+camera.y)){
        int *EX_Epoint1;
        int *EX_Epoint2;
        int *EX_Epoint3;
        int *EX_Epoint4;
        int    *Epoint1;
        int    *Epoint2;
        int    *Epoint3;
        int    *Epoint4;
        if(enemies[i].type == 0){
        EX_Epoint1 = (int*)rotatePoint(enemies[i].angle,(int)enemies[i].x-strokeWidth,(int)enemies[i].y-strokeWidth,(int)enemies[i].x+enemies[i].width/2,(int)enemies[i].y+enemies[i].height/2);
        EX_Epoint2 = (int*)rotatePoint(enemies[i].angle,(int)enemies[i].x+enemies[i].width+strokeWidth,(int)enemies[i].y-strokeWidth,(int)enemies[i].x+enemies[i].width/2,(int)enemies[i].y+enemies[i].height/2);
        EX_Epoint3 = (int*)rotatePoint(enemies[i].angle,(int)enemies[i].x+enemies[i].width+strokeWidth,(int)enemies[i].y+enemies[i].height+strokeWidth,(int)enemies[i].x+enemies[i].width/2,(int)enemies[i].y+enemies[i].height/2);
        EX_Epoint4 = (int*)rotatePoint(enemies[i].angle,(int)enemies[i].x-strokeWidth,(int)enemies[i].y+enemies[i].height+strokeWidth,(int)enemies[i].x+enemies[i].width/2,(int)enemies[i].y+enemies[i].height/2);
        Epoint1 = (int*)rotatePoint(enemies[i].angle,(int)enemies[i].x,(int)enemies[i].y,(int)enemies[i].x+enemies[i].width/2,(int)enemies[i].y+enemies[i].height/2);
        Epoint2 = (int*)rotatePoint(enemies[i].angle,(int)enemies[i].x+enemies[i].width,(int)enemies[i].y,(int)enemies[i].x+enemies[i].width/2,(int)enemies[i].y+enemies[i].height/2);
        Epoint3 = (int*)rotatePoint(enemies[i].angle,(int)enemies[i].x+enemies[i].width,(int)enemies[i].y+enemies[i].height,(int)enemies[i].x+enemies[i].width/2,(int)enemies[i].y+enemies[i].height/2);
        Epoint4 = (int*)rotatePoint(enemies[i].angle,(int)enemies[i].x,(int)enemies[i].y+enemies[i].height,(int)enemies[i].x+enemies[i].width/2,(int)enemies[i].y+enemies[i].height/2);
        }
        else if(enemies[i].type == 1){
        EX_Epoint1 = (int*)rotatePoint(enemies[i].angle-PI/2,(int)enemies[i].x+enemies[i].width/2,(int)enemies[i].y-strokeWidth,(int)enemies[i].x+enemies[i].width/2,(int)enemies[i].y+enemies[i].height/2);
        EX_Epoint2 = (int*)rotatePoint(enemies[i].angle-PI/2,(int)enemies[i].x+enemies[i].width/2,(int)enemies[i].y-strokeWidth,(int)enemies[i].x+enemies[i].width/2,(int)enemies[i].y+enemies[i].height/2);
        EX_Epoint3 = (int*)rotatePoint(enemies[i].angle-PI/2,(int)enemies[i].x+enemies[i].width+strokeWidth,(int)enemies[i].y+enemies[i].height+strokeWidth,(int)enemies[i].x+enemies[i].width/2,(int)enemies[i].y+enemies[i].height/2);
        EX_Epoint4 = (int*)rotatePoint(enemies[i].angle-PI/2,(int)enemies[i].x-strokeWidth,(int)enemies[i].y+enemies[i].height+strokeWidth,(int)enemies[i].x+enemies[i].width/2,(int)enemies[i].y+enemies[i].height/2);
           Epoint1 = (int*)rotatePoint(enemies[i].angle-PI/2,(int)enemies[i].x+enemies[i].width/2,(int)enemies[i].y,(int)enemies[i].x+enemies[i].width/2,(int)enemies[i].y+enemies[i].height/2);
           Epoint2 = (int*)rotatePoint(enemies[i].angle-PI/2,(int)enemies[i].x+enemies[i].width/2,(int)enemies[i].y,(int)enemies[i].x+enemies[i].width/2,(int)enemies[i].y+enemies[i].height/2);
           Epoint3 = (int*)rotatePoint(enemies[i].angle-PI/2,(int)enemies[i].x+enemies[i].width,(int)enemies[i].y+enemies[i].height,(int)enemies[i].x+enemies[i].width/2,(int)enemies[i].y+enemies[i].height/2);
           Epoint4 = (int*)rotatePoint(enemies[i].angle-PI/2,(int)enemies[i].x,(int)enemies[i].y+enemies[i].height,(int)enemies[i].x+enemies[i].width/2,(int)enemies[i].y+enemies[i].height/2);
        }
        
       if(enemies[i].knockbackMulti>0.05f){
       int colorWhite[4] = {255,255,255,255};
       fillPOLYGON(EX_Epoint1,EX_Epoint2,EX_Epoint3,EX_Epoint4,(int)enemies[i].y-camera.y,colorWhite,1);
       }
       else{
        int rednessOutline = 150-enemies[i].enragement*4;
        if(rednessOutline < 20){
          rednessOutline = 20;
        }
        int colorDarkRed[4] = {rednessOutline,0,0,255};
       fillPOLYGON(EX_Epoint1,EX_Epoint2,EX_Epoint3,EX_Epoint4,(int)enemies[i].y-camera.y,colorDarkRed,1);
       int redness = 200-enemies[i].enragement*4;
       if(redness < 0){
        redness = 0;
       }
       int colorRed[4] = {redness,0,0,255};
       fillPOLYGON(Epoint1,Epoint2,Epoint3,Epoint4,(int)enemies[i].y-camera.y,colorRed,1);
       
       }
       free(EX_Epoint1);
       free(EX_Epoint2);
       free(EX_Epoint3);
       free(EX_Epoint4);
       free(Epoint1);
       free(Epoint2);
       free(Epoint3);
       free(Epoint4);
       
       if(enemies[i].healthbar){
         SDL_Rect healthBar = {
           (int)enemies[i].x-camera.x,
           (int)(enemies[i].y - 2*enemies[i].height/3-camera.y),
           (int)enemies[i].width,
           5
         };
         SDL_SetRenderDrawColor(renderer,0,0,0,255);
         SDL_RenderDrawRect(renderer,&healthBar);
         SDL_Rect healthBarFiller = {
           (int)enemies[i].x-camera.x,
           (int)(enemies[i].y - 2*enemies[i].height/3-camera.y),
           (int)(enemies[i].width * enemies[i].health/enemies[i].healthMax),
           5
         };
         SDL_SetRenderDrawColor(renderer,200,0,0,255);
         SDL_RenderFillRect(renderer,&healthBarFiller);
       }
     }
   }
 
 // DRAW OBSTACLES
 // NOTE : OBSTACLES ARE NEVER ROTATED THUS NO NEED TO USE rotatePoint() 
 for(int i = 0;i<sizeof(obstacles)/sizeof(obstacles[0]);i++){
   if(obstacles[i].reserve && (obstacles[i].x+ obstacles[i].width > camera.x && obstacles[i].x<windowWidth+camera.x && obstacles[i].y+ obstacles[i].height > camera.y && obstacles[i].y<windowHeight+camera.y)){
     SDL_Rect obstacleRect = {
       (int)obstacles[i].x-camera.x,
       (int)obstacles[i].y-camera.y,
       (int)obstacles[i].width,
       (int)obstacles[i].height
     };
     SDL_Rect EXobstacleRect = {
       (int)obstacles[i].x+strokeWidth-camera.x,
       (int)obstacles[i].y+strokeWidth-camera.y,
       (int)obstacles[i].width-strokeWidth*2,
       (int)obstacles[i].height-strokeWidth*2
     };
     SDL_SetRenderDrawColor(renderer,120,20,0,255);
     SDL_RenderFillRect(renderer,&obstacleRect);
     SDL_SetRenderDrawColor(renderer,150,120,0,255);
     SDL_RenderFillRect(renderer,&EXobstacleRect);
   }
 }
    // HUD RENDERING
      // [HEALTH]
    SDL_Rect HealthBarStroke = {
       20,
       windowHeight-20,
       60,
       10
     };
     SDL_Rect HealthBarFILL = {
       20,
       windowHeight-20,
       (int)rect.health/rect.healthMax*60,
       10
     };
     SDL_SetRenderDrawColor(renderer,(int)(255 - (int)rect.health*255/(int)rect.healthMax),(int)((int)rect.health*255/(int)rect.healthMax),0,HUDOpacity);
     SDL_RenderFillRect(renderer,&HealthBarFILL);
     SDL_SetRenderDrawColor(renderer,0,0,0,HUDOpacity);
     SDL_RenderDrawRect(renderer,&HealthBarStroke);
     // [SHIELD]
     SDL_Rect ShieldBarStroke = {
       100,
       windowHeight-20,
       60,
       10
     };
     SDL_Rect ShieldBarFILL = {
       100,
       windowHeight-20,
       (int)(rect.shield/100*60), 
       10
     };
     SDL_SetRenderDrawColor(renderer,6,143,129,HUDOpacity); 
     SDL_RenderFillRect(renderer,&ShieldBarFILL);
     SDL_SetRenderDrawColor(renderer,0,0,0,HUDOpacity);
     SDL_RenderDrawRect(renderer,&ShieldBarStroke);
     // [BULLETS]
     SDL_Rect BulletStroke = {
       windowWidth-80,
       windowHeight-20,
       60,
       10
     };
     SDL_Rect BulletFILL = {
       windowWidth-80,
       windowHeight-20,
       (int)rect.mag/rect.magMax*60, 
       10
     };
     SDL_SetRenderDrawColor(renderer,100,100,100,HUDOpacity);
     SDL_RenderFillRect(renderer,&BulletFILL);
     SDL_SetRenderDrawColor(renderer,0,0,0,HUDOpacity);
     SDL_RenderDrawRect(renderer,&BulletStroke);


 ///////////////////////////////////////////////////////////
    // TEXT RENDERING 
  renderNum(2,rect.health,30,windowHeight-40,40,20,HUDOpacity,colorWHITE);
  if(rect.shield == 0){
  renderNum(2,(int)rect.shield,105,windowHeight-40,50,20,HUDOpacity,colorWHITE);
  }
  else {
  renderNum((int)(SDL_floor(SDL_log10(rect.shield)+1)),(int)rect.shield,110,windowHeight-40,40,20,HUDOpacity,colorWHITE);
  }
  renderNum(3,fps,windowWidth-40,0,40,20,HUDOpacity,colorWHITE);
  renderNum(2,rect.mag,windowWidth-70,windowHeight-40,40,20,HUDOpacity,colorWHITE);
  
  if(rect.reloading){
  renderText(10,TEXT_reloading,windowWidth-78,windowHeight-20,65,10,SDL_min(HUDOpacity,100),colorWHITE);
  }
  for(int i = 0;i<sizeof(alerts)/sizeof(alerts[0]);i++){
   if(alerts[i].reserve){
       renderText(alerts[i].size,alerts[i].text,alerts[i].x,alerts[i].y,alerts[i].size*10,15,(int)SDL_min(alerts[i].opacity,HUDOpacity),alerts[i].color);
   }
  }
  int credits_digits;
      if(rect.credit != 0){
        credits_digits = SDL_floor(SDL_log10(rect.credit)+1);
      }
      else {
        credits_digits = 1;
      }
  char TEXT_credit[8+credits_digits];
  sprintf(TEXT_credit,"Credits %d",(int)rect.credit);
  renderText(8+credits_digits,TEXT_credit,windowWidth-(8+credits_digits)*10,30,(8+credits_digits)*10,15,SDL_min(rect.creditOpacity,HUDOpacity),colorYELLOW);
  if(rect.creditOpacity > 0.01f && rect.credittimeShown < 5000.0f ){
      rect.credittimeShown += 1000*deltaTime;
  }
  if(rect.credittimeShown > 5000.0f && rect.creditOpacity > 0){
    rect.creditOpacity -= 200*deltaTime;
    if(rect.creditOpacity < 0){
      rect.creditOpacity = 0;
    }
  }
  char TEXT_seconds[2];
  char TEXT_minutes[2];
  if(gameClock_Seconds%60 < 10){
    sprintf(TEXT_seconds,"0%d",gameClock_Seconds%60);
  }
  else if (gameClock_Seconds%60 >= 10){
    sprintf(TEXT_seconds,"%d",gameClock_Seconds%60);
  }
  renderText(2,TEXT_seconds,windowWidth/2,0,20,15,HUDOpacity,colorWHITE);
  if(gameClock_Minutes < 10){
    sprintf(TEXT_minutes,"0%d",gameClock_Minutes);
  }
  else if(gameClock_Minutes >= 10){
    sprintf(TEXT_minutes,"%d",gameClock_Minutes);
  }
  
  renderText(2,TEXT_minutes,windowWidth/2-25,0,20,15,HUDOpacity,colorWHITE);
 
 ///////////////////////////////////////////////////////////   

 if(upgradeTab && HUDOpacity > 50){
   HUDOpacity -= 800*deltaTime;
 }
 if(HUDOpacity < 255 && !upgradeTab){
  HUDOpacity += 800*deltaTime;
  if(HUDOpacity > 255){
    HUDOpacity = 255;
  }
 }
}
void update(){  
 // GAME CLOCK
 if(!rect.dead){
  gameClock += 1000*deltaTime;
 gameClock_Seconds = gameClock/1000;
 if(gameClock_Seconds >= (gameClock_Minutes+1)*60){
    gameClock_Minutes++;
  }
  enemy_baseEnragement = gameClock_Minutes/2;
 {
 }
 
  // PAUSE GAME
  if(key.escape && !pauseBool){
    paused = 1;
  }
  if(!key.escape){
    pauseBool = 0;
  }
  // UPGRADE TAB
  if(key.tab && !upgradeTabBool){
    upgradeTab = 1;
  }
  if(!key.tab){
    upgradeTabBool = 0;
  }
  fps = (int)(1/deltaTime);
  if(key.w){
    rect.veloY = -rect.speed;
  }
  else if (key.s){
    rect.veloY = rect.speed;
  }
  else {
    rect.veloY = 0;
  }

  if(key.a){
    rect.veloX = -rect.speed;
  }
  else if (key.d){
    rect.veloX = rect.speed;
  }
  else {
    rect.veloX = 0;
  }
 }
  // HANDLE BULLET (FIRERATE)
  if(key.left == 1 && SDL_GetTicks() - timeShot > rect.fireRate && rect.mag > 0 && !rect.reloading && !upgradeTab){
    shoot();
    rect.mag--;
    timeShot = SDL_GetTicks();
  }

  // RELOADING
  if(key.r && rect.mag < rect.magMax && !rect.reloading){
    rect.reloading = 1;
    rect.CountMag = 0;
  } 
  if(rect.reloading){
    if(rect.CountMag < rect.mag){
      rect.CountMag += 15*deltaTime*1000/rect.reloadTime;
    }
    else{
      rect.CountMag = rect.magMax;
      rect.mag += 15*deltaTime*1000/rect.reloadTime;
    }
     // 2 SECOND RELOAD TIME
    if(rect.mag >= rect.magMax){
      rect.mag = rect.magMax;
      rect.reloading = 0;

    }
  }

   
  // PLAYER MOVEMENT AND ROTATION
  if(!rect.dead){
  rect.x += (rect.veloX*(1-rect.knockbackMulti)+rect.knockbackX*rect.knockbackMulti)*deltaTime;
  rect.y += (rect.veloY*(1-rect.knockbackMulti)+rect.knockbackY*rect.knockbackMulti)*deltaTime;
  if(rect.knockbackMulti>0.01f){
    rect.knockbackMulti *= (float)SDL_pow(0.9,60*deltaTime);
  }
  else {rect.knockbackMulti = 0;}
  if(rect.veloX || rect.veloY){
    SDL_GetMouseState(&mouse.x,&mouse.y);
    rect.angle = SDL_atan((rect.y-camera.y+rect.height/2 - mouse.y)/(rect.x-camera.x+rect.width/2 - mouse.x));
    if((float)mouse.x > rect.x-camera.x+rect.width/2){
      rect.angle += PI;
    }
  }
  }
  
  // CAMERA FOLLOWAGE
  if(rect.x+rect.width/2>=windowWidth/2 && rect.x+rect.width/2<=-windowWidth/2+worldBorder.x){
    camera.x = rect.x+rect.width/2-windowWidth/2;  
  }
  if(rect.y+rect.height/2>windowHeight/2 && rect.y+rect.height/2<=-windowHeight/2+worldBorder.y){
    camera.y = rect.y+rect.height/2-windowHeight/2; 
  }
  
  // BOUNDINGS
  if (rect.x < 0){
    rect.x = 0;
  }  
  if(rect.x + rect.width > worldBorder.x){
    rect.x = worldBorder.x - rect.width;
  }
  if(rect.y < 0){
    rect.y = 0;
  }
  if(rect.y + rect.height > worldBorder.y){
    rect.y = worldBorder.y - rect.height;
  }
  // BULLETS MOVEMENT
  for(int i = 0;i<sizeof(bullets)/sizeof(bullets[0]);i++){
    if(!bullets[i].reserve){
      continue;
    }
    // BOUNDARIES
    if(bullets[i].x-camera.x < 0 || bullets[i].x-camera.x > windowWidth || bullets[i].y-camera.y < 0 || bullets[i].y-camera.y > windowHeight){
     bullets[i].reserve = 0;
     continue;
    }
    // w/ ENEMY COLLISION
    if(bullets[i].emitter == 0){
    for(int j = 0;j<sizeof(enemies)/sizeof(enemies[0]);j++){
      if(collisionCheck(bullets[i].x,bullets[i].y,bullets[i].width,bullets[i].height,enemies[j].x,enemies[j].y,enemies[j].width,enemies[j].height)){
        bullets[i].reserve = 0;
        enemies[j].health -= rect.bulletDmg;
        enemies[j].knockbackX = bullets[i].veloX/2;
        enemies[j].knockbackY = bullets[i].veloY/2;
        if(enemies[j].knockbackY){
         enemies[j].knockbackMulti = 0.5;
        }
        else{
          enemies[j].knockbackMulti = 1;
        }
        break;
      }
    }
    }
    else if(bullets[i].emitter == 1){
      if(collisionCheck(bullets[i].x,bullets[i].y,bullets[i].width,bullets[i].height,rect.x,rect.y,rect.width,rect.height) && !rect.dead){
        bullets[i].reserve = 0;
        float hitTaken = 0;
        if(rect.shield > 0){
          rect.shield -= enemies[i].hitDamage*5;
          if(rect.shield < 0){
            rect.shield += enemies[i].hitDamage*5;
            hitTaken = -rect.shield;
            rect.shield = 0;
          }
          else {
            hitTaken = -enemies[i].hitDamage*5;
          }
        }
        rect.health -= enemies[i].hitDamage * (rect.shieldPower+ ((1-rect.shieldPower)*((hitTaken+enemies[i].hitDamage*5)/(enemies[i].hitDamage*5)))); 
        rect.knockbackX = bullets[i].veloX/2;
        rect.knockbackY = bullets[i].veloY/2;
        if(rect.knockbackY){
         rect.knockbackMulti = 0.5;
        }
        else{
          rect.knockbackMulti = 1;
        }
       
      }
      for(int j = 0;j<sizeof(enemies)/sizeof(enemies[0]);j++){
        if(collisionCheck(bullets[i].x,bullets[i].y,bullets[i].width,bullets[i].height,enemies[j].x,enemies[j].y,enemies[j].width,enemies[j].height) && j != bullets[i].index){
        bullets[i].reserve = 0;
        break;    
          }
      }
      
    }
    for(int j = 0;j<sizeof(obstacles)/sizeof(obstacles[0]);j++){
      if(collisionCheck(bullets[i].x,bullets[i].y,bullets[i].width,bullets[i].height,obstacles[j].x,obstacles[j].y,obstacles[j].width,obstacles[j].height)){
        bullets[i].reserve = 0; 
        break;
      }
    }
    bullets[i].x += bullets[i].veloX*deltaTime;
    bullets[i].y += bullets[i].veloY*deltaTime;
    
    }
  // HEALING
  if(rect.health < rect.healthMax && !rect.dead){
    rect.healthRegenTime -= 1000.0f*deltaTime;
    if(rect.healthRegenTime<0){
      rect.healthRegenTime = rect.healingTime;
      rect.health++;
    }
  }
  
  // ENEMY BEHAVIOR 
  for(int i = 0;i<sizeof(enemies)/sizeof(enemies[0]);i++){
    if(enemies[i].reserve){
      // FOLLOW PLAYER
      if((rect.x-enemies[i].x) != 0){
        enemies[i].angle = (float)SDL_atan((rect.y+rect.height/2-enemies[i].y-enemies[i].height/2)/(rect.x+rect.width/2-enemies[i].x-enemies[i].width/2));
       if(rect.x+rect.width/2 > enemies[i].x+enemies[i].width/2){
         enemies[i].angle += PI;
       }  
      }
      if(enemies[i].type == 0 || (enemies[i].type == 1 && SDL_sqrt(SDL_pow((enemies[i].x-rect.x),2)+SDL_pow((enemies[i].y-rect.y),2)) > 100)){
      enemies[i].veloX = -enemies[i].speed*SDL_cos(enemies[i].angle);
      enemies[i].veloY = -enemies[i].speed*SDL_sin(enemies[i].angle);
      enemies[i].x += (enemies[i].veloX*(1-enemies[i].knockbackMulti) + enemies[i].knockbackX*enemies[i].knockbackMulti)*deltaTime;
      enemies[i].y += (enemies[i].veloY*(1-enemies[i].knockbackMulti) + enemies[i].knockbackY*enemies[i].knockbackMulti)*deltaTime;
      }
      if(enemies[i].type == 1 && enemies[i].fireRateTime > enemies[i].fireRate){
        enemies[i].fireRateTime = 0;
        for(int j = 0;j<sizeof(bullets)/sizeof(bullets[0]);j++){
          if(!bullets[j].reserve){
            bullets[j].r = 200-enemies[i].enragement*4;
            bullets[j].g = 0;
            bullets[j].b = 0;
            bullets[j].emitter = 1;
            bullets[j].reserve = 1;
            bullets[j].x = enemies[i].x+enemies[i].width/2;
            bullets[j].y = enemies[i].y+enemies[i].height/2;
            bullets[j].veloX = -500*SDL_cos(enemies[i].angle+(2*rand()%2 - 1)*(float)(rand()%1000)/(5000+enemies[i].enragement*500));
            bullets[j].veloY = -500*SDL_sin(enemies[i].angle+(2*rand()%2 - 1)*(float)(rand()%1000)/(5000+enemies[i].enragement*500));
            bullets[j].index = i;
            bullets[j].width = 4.0f;
            bullets[j].height = 4.0f;
            break;
          }
        }
      }
      enemies[i].fireRateTime += 1000*deltaTime;
      if(enemies[i].knockbackMulti>0.01f){
        enemies[i].knockbackMulti *= (float)SDL_pow(0.9,30*deltaTime);
      }
      // CHECK DEATH
      if(enemies[i].health <= 0.0f){
        int j = 0;
        for(int k = 0;k<sizeof(particles)/sizeof(particles[0]);k++){
          if(!particles[k].reserve){
           int dirVelo = 1;
           j++;
           particles[k].reserve = 1;
           particles[k].x = enemies[i].x;
           particles[k].y = enemies[i].y;
           if(rand() % 2 == 1){
            dirVelo *= -1;
           }
           particles[k].veloX = (float)(dirVelo*(rand() % 500));
           particles[k].veloY = (float)(dirVelo*(rand() % 500));
           particles[k].angle = (float)(dirVelo*(rand() % 500));
           particles[k].angleVelo = (float)(rand() % 5);
           int randomSize = rand() % 15;
           particles[k].width = randomSize;
           particles[k].height = randomSize;
           particles[k].opacity = 255;
           if(j > 5){
            break;
           }
           } 
        }
        int randomValue = rand()%(int)rect.luck;
        if(randomValue == 0){
          for(int l = 0;i<sizeof(items)/sizeof(items[0]);l++){
          if(!items[l].reserve){
            items[l].reserve = 1;
            items[l].x = (float)enemies[i].x;
            items[l].y = (float)enemies[i].y;
            items[l].veloX = (rand()%200);
            items[l].veloY = (rand()%200);
            items[l].angle = (float)(rand()%360);
            items[l].angleVelo = (float)(rand()%720);
            items[l].type = rand()%4;
            items[l].width = 20.0f;
            items[l].height = 20.0f;
            break;
          }
        }
        }
        int j2 = 0;
        int j2_max = rand()%4 + enemies[i].enragement;
        for(int l = 0;l<sizeof(credits)/sizeof(credits[0]);l++){
          if(!credits[l].reserve){
             credits[l].reserve = 1;
             credits[l].x = enemies[i].x;
             credits[l].y = enemies[i].y;
             credits[l].width = 10.0f;
             credits[l].height = 10.0f;
             credits[l].angle = rand() % 3;
             credits[l].angleVelo = rand() % 10;
             credits[l].veloX = rand()%200;
             credits[l].veloY = rand()%200;
             credits[l].rectPullX = 0;
             credits[l].rectPullY = 0;
             j2++;
             if(j2 >= j2_max){
              break;
             }
             
          }   
        }

        enemies[i].reserve = 0;
        enemies[i].x = -99.0f;
        enemies[i].y = -99.0f;
        enemy_present--;
      }     
      // HEALTHBAR 
      if(enemies[i].health < enemies[i].healthMax){
        enemies[i].healthbar = 1;
      }
    }
    // COLLISION DETECTION;
       // {ENEMY}
    for(int j = 0;j<sizeof(enemies)/sizeof(enemies[0]);j++){
     if(collisionCheck(enemies[i].x-camera.x,enemies[i].y-camera.y,enemies[i].width,enemies[i].height,enemies[j].x-camera.x,enemies[j].y-camera.y,enemies[j].width,enemies[j].height) && i != j){
        collisionCorrection(&enemies[i].x,&enemies[i].y,&enemies[i].width,&enemies[i].height,&enemies[j].x,&enemies[j].y,&enemies[j].width,&enemies[j].height);
     }
    }
    // {PLAYER}
     if(collisionCheck(enemies[i].x-camera.x,enemies[i].y-camera.y,enemies[i].width,enemies[i].height,rect.x-camera.x,rect.y-camera.y,rect.width,rect.height) && !rect.dead){
        collisionCorrection(&enemies[i].x,&enemies[i].y,&enemies[i].width,&enemies[i].height,&rect.x,&rect.y,&rect.width,&rect.height);
        float hitTaken = 0;
        if(rect.shield > 0){
          rect.shield -= enemies[i].hitDamage*5;
          if(rect.shield < 0){
            rect.shield += enemies[i].hitDamage*5;
            hitTaken = -rect.shield;
            rect.shield = 0;
          }
          else {
            hitTaken = -enemies[i].hitDamage*5;
          }
        }
        rect.health -= enemies[i].hitDamage * (rect.shieldPower+ ((1-rect.shieldPower)*((hitTaken+enemies[i].hitDamage*5)/(enemies[i].hitDamage*5)))); 
        rect.healthRegenTime = rect.healingTime;
        rect.knockbackX = 5*enemies[i].veloX;
        rect.knockbackY = 5*enemies[i].veloY;
        rect.knockbackMulti = 1;
     }
  }

  // SPAWN ENEMIES
 if(SDL_GetTicks() - enemy_timeSpawn > enemy_spawnRate && enemy_present < enemy_max){
   enemy_timeSpawn = SDL_GetTicks();
   enemy_present++;
   for(int i = 0 ;i<sizeof(enemies)/sizeof(enemies[0]);i++){
     if(!enemies[i].reserve){
      enemies[i].reserve = 1;
      int randomValue = rand() % 4; 
      if(randomValue == 0){
        enemies[i].x = (float)(rand() % (int)worldBorder.x);
        enemies[i].y = -30;
      }
      else if (randomValue == 1){
        enemies[i].x = (float)(rand() % (int)worldBorder.x); 
        enemies[i].y = worldBorder.y + 30;
      }
      else if(randomValue == 2){
        enemies[i].x = -20;
        enemies[i].y = (float)(rand() % (int)worldBorder.y);
      }
      else {
        enemies[i].x = worldBorder.x+30;
        enemies[i].y = (float)(rand() % (int)worldBorder.y);
      }     
       int randomSize = rand()%(gameClock_Seconds/60 + 1);
       int randomSign = 2*(rand()%2)-1;
       float size = 20.0f+randomSign* (5*randomSize);
       enemies[i].enragement = rand()%(gameClock_Seconds/60 + enemy_baseEnragement + 1);
       if(gameClock_Minutes > 0){
        if(rand()%2 == 0){
          enemies[i].type = 0;
          enemies[i].fireRate = 0;
        }
        else if(rand()%2==1){
          enemies[i].type = 1;
          enemies[i].fireRate = 2000 - enemies[i].enragement*50;
          if(enemies[i].fireRate <100){
            enemies[i].fireRate = 100;
          }
          
        }
       }
       else {
        enemies[i].fireRate = 0;
        enemies[i].type = 0;
       }
       int sizeMin;
       int sizeMax;
       if(enemies[i].type == 0){
        sizeMin = 15;
        sizeMax = 40;
       }
       else if (enemies[i].type == 1){
        sizeMin = 20;
        sizeMax = 30;
       }
       if(size < sizeMin){
        size = sizeMin;
       }
       else if (size > sizeMax){
        size = sizeMax;
       }
       enemies[i].fireRateTime = 0;
       enemies[i].width  = size;
       enemies[i].height = size;
       enemies[i].knockbackX = 0.0f;
       enemies[i].knockbackY = 0.0f;
       enemies[i].knockbackMulti = 0.0f;
       enemies[i].healthbar = 0;
       enemies[i].angle = 0;
       enemies[i].health = enemies[i].width+enemies[i].enragement*2; 
       enemies[i].healthMax = enemies[i].health;
       enemies[i].hitDamage = enemies[i].width/4 +enemies[i].enragement;
       enemies[i].speed = 50.0f + (rand()%5) - (enemies[i].width-20) + enemies[i].enragement*2;
       
       break;
     }
   }
 }
 // INCREASE MAX ENEMY 
 if(SDL_GetTicks() - enemy_increaseTime > enemy_increaseRate){
  enemy_increaseTime = SDL_GetTicks();
  enemy_max++;
 }

 // PARTICLES 
 for(int i = 0;i<sizeof(particles)/sizeof(particles[0]);i++){
  if(particles[i].reserve){
    particles[i].x += particles[i].veloX*deltaTime;
    particles[i].y += particles[i].veloY*deltaTime;
    particles[i].angle += particles[i].angleVelo*deltaTime;
    particles[i].veloX *=  (float)SDL_pow(0.9,60*deltaTime);
    particles[i].veloY *=  (float)SDL_pow(0.9,60*deltaTime);
    particles[i].angleVelo *= (float)SDL_pow(0.98,60*deltaTime);
    particles[i].opacity -= 100*deltaTime;
    if(particles[i].opacity < 0.01f){
      particles[i].reserve = 0;
    }
  }
 }
 
 // OBSTACLES
 for(int i = 0;i<sizeof(obstacles)/sizeof(obstacles[0]);i++) {
  if(obstacles[i].reserve){
    // COLLISION DETECTION OBSTACLE
      // {ENEMY}
    for(int j = 0;j<sizeof(enemies)/sizeof(enemies[0]);j++){
      if(collisionCheck(enemies[j].x,enemies[j].y,enemies[j].width,enemies[j].height,obstacles[i].x,obstacles[i].y,obstacles[i].width,obstacles[i].height)){
        collisionCorrection(&enemies[j].x,&enemies[j].y,&enemies[j].width,&enemies[j].height,&obstacles[i].x,&obstacles[i].y,&obstacles[i].width,&obstacles[i].height);
      }
    }
    // {PLAYER}
    if(collisionCheck(rect.x,rect.y,rect.width,rect.height,obstacles[i].x,obstacles[i].y,obstacles[i].width,obstacles[i].height)){
      collisionCorrection(&rect.x,&rect.y,&rect.width,&rect.height,&obstacles[i].x,&obstacles[i].y,&obstacles[i].width,&obstacles[i].height);
    }
  }
 }
 // ITEMS 
 for (int i = 0;i<sizeof(items)/sizeof(items[0]);i++){
  if(items[i].reserve){
    items[i].x += items[i].veloX*deltaTime;
    items[i].y += items[i].veloY*deltaTime;
    items[i].veloX *= SDL_pow(0.9,30*deltaTime);
    items[i].veloY *= SDL_pow(0.9,30*deltaTime);
    items[i].angle += items[i].angleVelo*deltaTime;
    items[i].angleVelo *= SDL_pow(0.9,30*deltaTime);

    // CHECK COLLISION W PLAYER
    if(collisionCheck(items[i].x,items[i].y,items[i].width,items[i].height,rect.x,rect.y,rect.width,rect.height)){
      if(items[i].type == 0){
        // health upgrade
        rect.healthMax += 5;
        rect.health += 5;
        char *TEXT_alert = "Max Health Increased By 5";
        alertCall(26,TEXT_alert,colorGREEN);
      }
      else if(items[i].type == 1){
        // weapon upgrade
        
        int randomValue = rand()%3;
        if(randomValue == 0){
          rect.fireRate *= 0.8; 
          char *TEXT_alert = "Fire Rate Increased By 20";
          alertCall(26,TEXT_alert,colorGREEN);
        }
        else if (randomValue == 1)
        {
          rect.reloadTime *= 0.8;
          char *TEXT_alert = "Reload Time Decreased By 20";
          alertCall(28,TEXT_alert,colorGREEN);
        }
        else if(randomValue == 2){
          rect.magMax += 5;
          rect.mag += 5;
          char *TEXT_alert = "Bullets Capacity Increased By 5";
          alertCall(32,TEXT_alert,colorGREEN);
        }
      }
      else if(items[i].type == 2){
        if(rect.shield < rect.shieldMax){
          rect.shield += 10.0f;
        char *TEXT_alert = "Shield Recharge By 10";
        alertCall(22,TEXT_alert,colorBLUE);
        }
        else {
         rect.credit += 5;
         rect.creditOpacity = 255;
         rect.credittimeShown = 0.0f;
         char *TEXT_alert = "Max Shield Reached";
        alertCall(19,TEXT_alert,colorBLUE);
        }
        
      }
      else if(items[i].type == 3){
        upgrades[3].value += 5;
        char *TEXT_alert = "Player Speed Increased By 5";
        alertCall(28,TEXT_alert,colorGREEN);
      }
      items[i].reserve = 0;
   } 
  }
 }
 // ALERTS
  for (int i = 0;i<sizeof(alerts)/sizeof(alerts[0]);i++){
    if(alerts[i].reserve){
      
      if(alerts[i].opacity < 255 && alerts[i].timeShown < alerts[i].maxtime){
        alerts[i].opacity += 255*5*deltaTime;  
        if(alerts[i].opacity > 255){
          alerts[i].opacity = 255;
        }
      }
      alerts[i].timeShown += 1000*deltaTime;
      if(alerts[i].timeShown > alerts[i].maxtime){
        alerts[i].opacity -= 300*deltaTime;
        if(alerts[i].opacity < 0){
          alerts[i].reserve = 0;         
        }
      } 
    }
    if(AlertDown){
        for(int j = 0;j<sizeof(alerts)/sizeof(alerts[0]);j++){
          if(alerts[j].reserve){
            alerts[j].y += 40*deltaTime;
          }
        }
        AlertDown -= 40*deltaTime;
        if(AlertDown < 0.01f){
          AlertDown = 0;
        }
    }
  }
 // CREDITS 
 for (int i = 0;i<sizeof(credits)/sizeof(credits[0]);i++){
  if(credits[i].reserve){
    credits[i].distanceTorect = SDL_sqrt(SDL_pow(rect.y-credits[i].y,2)+SDL_pow(rect.x-credits[i].x,2));
    
    if(credits[i].distanceTorect < 100){
      if((rect.x-enemies[i].x) != 0){
        credits[i].angleToRect = (float)SDL_atan((rect.y-credits[i].y)/(rect.x-credits[i].x));
       if(rect.x > credits[i].x){
         credits[i].angleToRect += PI;
       }  
      }
      credits[i].rectPullX = -5000*SDL_cos(credits[i].angleToRect)/credits[i].distanceTorect;
      credits[i].rectPullY = -5000*SDL_sin(credits[i].angleToRect)/credits[i].distanceTorect;
    }
    else {
      credits[i].rectPullX = 0;
     credits[i].rectPullY = 0;
    }
    

    credits[i].x += (credits[i].veloX+credits[i].rectPullX)*deltaTime;
    credits[i].y += (credits[i].veloY+credits[i].rectPullY)*deltaTime;
    credits[i].angle += credits[i].angleVelo*deltaTime;
    credits[i].veloX *= SDL_pow(0.9,30*deltaTime);
    credits[i].veloY *= SDL_pow(0.9,30*deltaTime);
    credits[i].angleVelo = (credits[i].angleVelo-3)*SDL_pow(0.9,30*deltaTime)+3;

    if(collisionCheck(credits[i].x,credits[i].y,credits[i].width,credits[i].height,rect.x,rect.y,rect.width,rect.height)){
      credits[i].reserve = 0;
      rect.credit += 1;
      rect.creditOpacity = 255;
      rect.credittimeShown = 0.0f;
    }
  }
 }
 // PLAYER DEATH 
 if(rect.health <= 0.0f && !rect.dead){
  upgradeTab = 0;
  rect.health = 0;
  rect.dead = 1;
   int j = 0;
        for(int k = 0;k<sizeof(particles)/sizeof(particles[0]);k++){
          if(!particles[k].reserve){
           int dirVelo = 1;
           j++;
           particles[k].reserve = 1;
           particles[k].x = rect.x;
           particles[k].y = rect.y;
           if(rand() % 2 == 1){
            dirVelo *= -1;
           }
           particles[k].veloX = (float)(dirVelo*(rand() % 500));
           particles[k].veloY = (float)(dirVelo*(rand() % 500));
           particles[k].angle = (float)(dirVelo*(rand() % 500));
           particles[k].angleVelo = (float)(rand() % 5);
           int randomSize = rand() % 15;
           particles[k].width = randomSize;
           particles[k].height = randomSize;
           particles[k].opacity = 255;
           if(j > 20){
            break;
           }
           } 
        }
 }
 if(rect.dead){
  deathscreenTime += 1000*deltaTime;
  if(deathscreenTime >= 3000){
    gameOver = 1;
  }
 }
}
void setup(void){
 // EH?
 ButtonSelectorY = windowHeight/12 + 150;
 // ENABLE TRANSPARENCY
 SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
 //
 enemy_present = 0;
 enemy_max = 10;
 gameOver = 0;
 deathscreenTime = 0;
 gameClock = 0;
 gameClock_Seconds = 0;
 gameClock_Minutes = 0;
 enemy_baseEnragement = 0;
 // CONTROL INIT
 key.w = 0;
 key.s = 0;
 key.d = 0;
 key.a = 0;
 key.left = 0;
 key.right = 0;
 // CAMERA INIT
 camera.x = 0;
 camera.y = 0;
 // WORLD BORDER
 worldBorder.x = windowWidth + 1000;
 worldBorder.y = windowHeight + 1000;
 // INITIAL PLAYER VARIABLES 
 rect.x = 200.0f;
 rect.y = 200.0f;
 rect.width =  20.0f;
 rect.height = 20.0f;
 rect.veloX = 0.0f;
 rect.veloY = 0.0f;
 rect.speed = 100.0f;
 rect.angle = 0.0f;
 rect.knockbackMulti = 0;
 rect.knockbackX = 0;
 rect.knockbackY = 0;
 rect.health = 20.0f;
 rect.healthMax = 20.0f;
 rect.healthRegenTime = 10000.0f;
 rect.healingTime = 5000.0f;
 rect.mag = 10.0f;
 rect.magMax = 10.0f;
 rect.bulletDmg = 5.0f;
 rect.fireRate = 400; // ms (will corrupt other objects if the value is too low !)
 rect.reloadTime = 2000; // seconds per 15 bullets recharged
 rect.reloading = 0;
 rect.shield = 100;
 rect.shieldMax = 100;
 rect.shieldPower = 0.9f;
 rect.CountMag = 0;
 rect.credit = 0;
 rect.creditOpacity = 0;
 rect.credittimeShown = 0;
 rect.luck = 15;
 rect.dead = 0;
 HUDOpacity = 255;
 
 // RESERVE SPACE FOR BULLETS 
 for(int i = 0;i<sizeof(bullets)/sizeof(bullets[0]);i++){
  bullets[i].reserve = 0;
 }
 // RESERVE SPACE FOR ENEMIES
 for(int i = 0;i<sizeof(enemies)/sizeof(enemies[0]);i++){
  enemies[i].reserve = 0;
  enemies[i].x = -99.0f;
  enemies[i].y = -99.0f;
 }
 // RESERVE SPACE FOR PARTICLES
 for(int i = 0;i<sizeof(particles)/sizeof(particles[0]);i++){
  particles[i].reserve = 0;
 }
 // RESERVE SPACE FOR OBSTACLES
 for(int i = 0;i<sizeof(obstacles)/sizeof(obstacles[0]);i++){
  obstacles[i].reserve = 0;
 }
 // RESERVE SPACE FOR ITEMS
 for(int i = 0;i<sizeof(items)/sizeof(items[0]);i++){
  items[i].reserve = 0;
 }
 // RESERVE SPACE FOR ALERTS
 for(int i = 0;i<sizeof(alerts)/sizeof(alerts[0]);i++){
  alerts[i].reserve = 0;
 }
 // RESERVE SPACE FOR CREDITS
 for(int i = 0;i<sizeof(credits)/sizeof(credits[0]);i++){
  credits[i].reserve = 0;
 }
 //
 // SETUP BACKGROUND BUSHES
 for(int i = 0;i<sizeof(bushes)/sizeof(bushes[0]);i++){
   bushes[i].reserve = 1;
   bushes[i].x = (float)(rand() % (int)worldBorder.x);
   bushes[i].y = (float)(rand() % (int)worldBorder.y);
   bushes[i].width = 20;
   bushes[i].height = 20;
   bushes[i].angle = rand() % 5;
 }
 // SETUP UPGRADES 
 for (int i = 0;i<sizeof(upgrades)/sizeof(upgrades[0]);i++){
  upgrades[i].type = i;
  upgrades[i].index = i;
  upgrades[i].price = 10;
  upgrades[i].buttonHover = 0;
  if(i == 0){
   upgrades[i].value =  rect.healingTime;
   upgrades[i].alertMessage = "Decreased Healing Time";
   upgrades[i].UpgradeTextSize = sizeof("Healing Time");
   upgrades[i].DescriptionSize = sizeof("Heal faster");
   upgrades[i].UpgradeText = "Healing Time";
   upgrades[i].Description = "Heal faster";
   upgrades[i].RectSrc = (SDL_Rect){
    0,0,200,200
   };
  }
  if(i == 1){
    upgrades[i].value = rect.bulletDmg;
    upgrades[i].UpgradeTextSize = sizeof("Bullet Damage");
   upgrades[i].alertMessage = "Increased Bullet Damage";
   upgrades[i].alertMessageSize = 24;
   upgrades[i].DescriptionSize = sizeof("Deal more damage");
   upgrades[i].UpgradeText = "Bullet Damage";
   upgrades[i].Description = "Deal more damage";
   upgrades[i].RectSrc = (SDL_Rect){
    200,0,200,200
   };
  }
  if(i == 2){
    upgrades[i].value = rect.shieldPower;
    upgrades[i].RectSrc = (SDL_Rect){
    0,200,200,200
   };
    upgrades[i].UpgradeTextSize = sizeof("Shield Power");
   upgrades[i].DescriptionSize = sizeof("Take less damage");
   upgrades[i].UpgradeText = "Shield Power";
   upgrades[i].Description = "Take less damage";
    upgrades[i].alertMessage = "Increased Shield Power";
   upgrades[i].alertMessageSize = 23;
  }
  if(i == 3){
    upgrades[i].value = rect.speed;
    upgrades[i].RectSrc = (SDL_Rect){
    200,200,200,200
   };
   upgrades[i].alertMessage = "Increased Player Speed";
    upgrades[i].alertMessageSize = 23;
    upgrades[i].UpgradeTextSize = sizeof("Speed");
   upgrades[i].DescriptionSize = sizeof("Move Faster");
   upgrades[i].UpgradeText = "Speed";
   upgrades[i].Description = "Move Faster";
  }
  if(i == 4){
    upgrades[i].value = rect.luck;
    upgrades[i].RectSrc = (SDL_Rect){
    400,0,200,200
   };
   upgrades[i].alertMessage = "Increased Player Luck";
   upgrades[i].alertMessageSize = 22;
   upgrades[i].UpgradeTextSize = sizeof("Luck");
   upgrades[i].DescriptionSize = sizeof("Higher chance of powerups");
   upgrades[i].UpgradeText = "Luck";
   upgrades[i].Description = "Higher chance of powerups";
  }
 }
 // DEFINE OBSTACLES IN MAP
 obstacles[0].x = 750.0f;
 obstacles[0].y = 500.0f;
 obstacles[0].width = 100.0f;
 obstacles[0].height = 40.0f;
 obstacles[0].reserve = 1;
 
}
void drawMainMenu(void){
   SDL_SetRenderDrawColor(renderer,0,20,0,255);
   SDL_RenderClear(renderer);
   // SELECTION
    ButtonSelectorX = 10;
    ButtonSelectorSizeX = 200;
    ButtonSelectorSizeY = 40;
   if(mouse.x < 210){
     if(mouse.y > windowHeight/12 + 150 && mouse.y < windowHeight/12 + 190){
     ButtonSelectorNUM = 0;
    }
    else if(mouse.y > windowHeight/12 + 200 && mouse.y < windowHeight/12 + 240){
      ButtonSelectorNUM = 1;
    }
    else if(mouse.y > windowHeight/12 + 250 && mouse.y < windowHeight/12 + 290){
      ButtonSelectorNUM = 2;
    }
    else if(mouse.y > windowHeight/12 + 300 && mouse.y < windowHeight/12 + 340){
      ButtonSelectorNUM = 3;
    }
    else {
      ButtonSelectorNUM = -1;
    }
   }
   else {
      ButtonSelectorNUM = -1;
    }
  if(ButtonSelectorNUM == -1){
    ButtonSelectorOpacity -= 1000*deltaTime;
    if(ButtonSelectorOpacity<0){
     ButtonSelectorOpacity = 0;
    }
    
  }
  else {
    if(ButtonSelectorNUM == 0){
     ButtonSelectorY = (ButtonSelectorY-windowHeight/12 - 150)*SDL_pow(0.6,deltaTime*30) + windowHeight/12+150;
    }
  if(ButtonSelectorNUM == 1){
      ButtonSelectorY = (ButtonSelectorY-windowHeight/12 - 200)*SDL_pow(0.6,deltaTime*30)+ windowHeight/12+200;
  }
  if(ButtonSelectorNUM == 2){
      ButtonSelectorY = (ButtonSelectorY-windowHeight/12 - 250)*SDL_pow(0.6,deltaTime*30)+ windowHeight/12+250;
  }
  if(ButtonSelectorNUM == 3){
      ButtonSelectorY = (ButtonSelectorY-windowHeight/12 - 300)*SDL_pow(0.6,deltaTime*30)+ windowHeight/12+300;
  }
  ButtonSelectorOpacity += 1000*deltaTime;
  if(ButtonSelectorOpacity>200){
   ButtonSelectorOpacity = 200;
  }
  }
  
   SDL_Rect buttonRect = {
        (int)ButtonSelectorX,
        (int)ButtonSelectorY,
        ButtonSelectorSizeX,
        ButtonSelectorSizeY
      };
      SDL_SetRenderDrawColor(renderer,100,100,100,(int)ButtonSelectorOpacity);
      SDL_RenderFillRect(renderer,&buttonRect);
   // TITLE SCREEN
   char TEXT_titlescreen[] = "Cube Game";
   char TEXT_play[] = "Play";
   char TEXT_settings[] = "Settings";
   char TEXT_credits[] = "Credits";
   char TEXT_exit[] = "Exit";

   renderText(sizeof(TEXT_titlescreen),TEXT_titlescreen,20,windowHeight/12,400,80,255,colorWHITE);

   // BUTTONS
   renderText(sizeof(TEXT_play),TEXT_play,20,windowHeight/12 + 150,100,40,255,colorWHITE); // play
   renderText(sizeof(TEXT_settings),TEXT_settings,20,windowHeight/12 + 200,sizeof(TEXT_settings)*20,40,255,colorWHITE); // settings
   renderText(sizeof(TEXT_credits),TEXT_credits,20,windowHeight/12 + 250,sizeof(TEXT_credits)*20,40,255,colorWHITE); // credits
   renderText(sizeof(TEXT_exit),TEXT_exit,20,windowHeight/12 + 300,sizeof(TEXT_exit)*20,40,255,colorWHITE); // exit
   

   // CLICK
   if(key.left == 2 && ButtonSelectorNUM != 1){
     if(ButtonSelectorNUM == 0){
      main_menu = 0;
      ButtonSelectorX = 0;
      ButtonSelectorY = 0;
      ButtonSelectorSizeX = 0;
      ButtonSelectorSizeY = 0;
     }
     if(ButtonSelectorNUM == 3){
      running = 0;
     }
   }
}
void drawPauseMenu(void){
  SDL_SetRenderDrawColor(renderer,100,100,100,150);
  SDL_Rect PauseBox = {
    windowWidth/4,
    windowHeight/4,
    windowWidth/2,
    windowHeight/2
  };
  SDL_RenderFillRect(renderer,&PauseBox);
    
    char TEXT_pause[] = "Paused";
    char TEXT_resume[] = "Resume";
    char TEXT_restart[] = "Restart";
    char TEXT_exit[] = "Exit";
    char TEXT_settings[] = "Settings";
  
    if(mouse.y > windowHeight/4+windowHeight/8 && mouse.y < windowHeight/4+windowHeight/8 + 30 && mouse.x > windowWidth/2-sizeof(TEXT_resume)*5 && mouse.x < windowWidth/2+sizeof(TEXT_resume)*5){
     ButtonSelectorNUM = 0;
    }
    else if(mouse.y > windowHeight/4+windowHeight/8 + 30 && mouse.y < windowHeight/4+windowHeight/8 + 60  && mouse.x > windowWidth/2-sizeof(TEXT_restart)*5 && mouse.x < windowWidth/2+sizeof(TEXT_restart)*5){
      ButtonSelectorNUM = 1;
      
    }
    else if(mouse.y > windowHeight/4+windowHeight/8 + 60 && mouse.y < windowHeight/4+windowHeight/8 + 90  && mouse.x > windowWidth/2-sizeof(TEXT_settings)*5 && mouse.x < windowWidth/2+sizeof(TEXT_settings)*5){
      ButtonSelectorNUM = 2;
    }
    else if(mouse.y > windowHeight/4+windowHeight/8 + 90 && mouse.y < windowHeight/4+windowHeight/8 + 120 && mouse.x > windowWidth/2-sizeof(TEXT_exit)*5 && mouse.x < windowWidth/2+sizeof(TEXT_exit)*5){
      ButtonSelectorNUM = 3;
    }
    else {
      ButtonSelectorNUM = -1;
    }
  
  if(ButtonSelectorNUM == -1){
    ButtonSelectorOpacity -= 1000*deltaTime;
    if(ButtonSelectorOpacity<0){
     ButtonSelectorOpacity = 0;
    }
   
  }
 else 
 {    
  if(ButtonSelectorNUM == 0){
     ButtonSelectorY = (ButtonSelectorY-(windowHeight/4+windowHeight/8))*SDL_pow(0.6,deltaTime*30) + windowHeight/4+windowHeight/8;
     ButtonSelectorX = (ButtonSelectorX-(windowWidth/2-sizeof(TEXT_resume)*5))*SDL_pow(0.6,deltaTime*30) + windowWidth/2-sizeof(TEXT_resume)*5;
     ButtonSelectorSizeX = sizeof(TEXT_resume)*10;
    }
  if(ButtonSelectorNUM == 1){
      ButtonSelectorY = (ButtonSelectorY-(windowHeight/4+windowHeight/8 + 30))*SDL_pow(0.6,deltaTime*30)+ windowHeight/4+windowHeight/8 + 30;
      ButtonSelectorX = (ButtonSelectorX-(windowWidth/2-sizeof(TEXT_restart)*5))*SDL_pow(0.6,deltaTime*30) + windowWidth/2-sizeof(TEXT_restart)*5;
      ButtonSelectorSizeX = sizeof(TEXT_restart)*10;
  }
  if(ButtonSelectorNUM == 2){
      ButtonSelectorY = (ButtonSelectorY-(windowHeight/4+windowHeight/8 + 60))*SDL_pow(0.6,deltaTime*30)+ windowHeight/4+windowHeight/8 + 60;
      ButtonSelectorX = (ButtonSelectorX-(windowWidth/2-sizeof(TEXT_settings)*5))*SDL_pow(0.6,deltaTime*30) + windowWidth/2-sizeof(TEXT_settings)*5;
       ButtonSelectorSizeX = sizeof(TEXT_settings)*10;
  }
  if(ButtonSelectorNUM == 3){
      ButtonSelectorY = (ButtonSelectorY-(windowHeight/4+windowHeight/8 + 90))*SDL_pow(0.6,deltaTime*30)+ windowHeight/4+windowHeight/8 + 90;
      ButtonSelectorX = (ButtonSelectorX-(windowWidth/2-sizeof(TEXT_exit)*5))*SDL_pow(0.6,deltaTime*30) + windowWidth/2-sizeof(TEXT_exit)*5;
      ButtonSelectorSizeX = sizeof(TEXT_exit)*10;
  }
  ButtonSelectorSizeY = 20;
  ButtonSelectorOpacity += 1000*deltaTime;
  if(ButtonSelectorOpacity>200){
   ButtonSelectorOpacity = 200;
  }
 }
   SDL_Rect buttonRect = {
        (int)ButtonSelectorX-4,
        (int)ButtonSelectorY-4,
        (int)ButtonSelectorSizeX+8,
        (int)ButtonSelectorSizeY+8
      };
      SDL_SetRenderDrawColor(renderer,100,100,100,(int)ButtonSelectorOpacity);
      SDL_RenderFillRect(renderer,&buttonRect);
  renderText(sizeof(TEXT_pause),TEXT_pause,windowWidth/2-80,windowHeight/4,200,50,255,colorWHITE);
  renderText(sizeof(TEXT_resume),TEXT_resume,windowWidth/2-sizeof(TEXT_resume)*5,windowHeight/4+windowHeight/8,sizeof(TEXT_resume)*10,20,255,colorWHITE);
  renderText(sizeof(TEXT_restart),TEXT_restart,windowWidth/2-sizeof(TEXT_restart)*5,windowHeight/4+windowHeight/8+30,sizeof(TEXT_restart)*10,20,255,colorWHITE);
  renderText(sizeof(TEXT_settings),TEXT_settings,windowWidth/2-sizeof(TEXT_settings)*5,windowHeight/4+windowHeight/8+60,sizeof(TEXT_settings)*10,20,255,colorWHITE);
  renderText(sizeof(TEXT_exit),TEXT_exit,windowWidth/2-sizeof(TEXT_exit)*5,windowHeight/4+windowHeight/8+90,sizeof(TEXT_exit)*10,20,255,colorWHITE);

  
  // CLICK
   if(key.left == 2 && ButtonSelectorNUM != -1){
     if(ButtonSelectorNUM == 0){
      paused = 0;
     }
     
     if(ButtonSelectorNUM == 1){
      paused = 0;
      
      setup();
     }
     if(ButtonSelectorNUM == 3){
      paused = 0;
      main_menu = 1;
      setup();
     }
   }
  // PAUSE KEY HANDLING 
  if(key.escape && pauseBool){
    paused = 0;
  }
  if(!key.escape){
    pauseBool = 1;
  }
}
void drawUpgradeTab(void){
  // RENDER PLAYER STATUS
  
  SDL_SetRenderDrawColor(renderer,100,100,100,150);
  SDL_Rect UpgradeBox = {
    windowWidth/8,
    windowHeight/8,
    3*windowWidth/4,
    3*windowHeight/4
  };
  SDL_RenderFillRect(renderer,&UpgradeBox);
  SDL_SetRenderDrawColor(renderer,100,100,100,100);
  SDL_Rect PlayerStatusBox = {
    windowWidth/8+3*windowWidth/8+10,
    windowHeight/8+10,
    3*windowWidth/8-20,
    3*windowHeight/4-20
  };
  SDL_RenderFillRect(renderer,&PlayerStatusBox);
  
  for(int i = 0 ;i < 4;i++){
    SDL_Rect InfoBox = {
    PlayerStatusBox.x+5,
    PlayerStatusBox.y+i*40+175,
    3*windowWidth/8-30,
    30
  };
  SDL_SetRenderDrawColor(renderer,50,50,50,150);
  SDL_RenderFillRect(renderer,&InfoBox);
  }
  
  // PLAYER PREVIEW RENDERING
  { 
  PlayerStatusBox.x += windowWidth/8;
  PlayerStatusBox.y += windowHeight/16;
  int* Pre_point1 = rotatePoint(previewPlayerAngle,(int)PlayerStatusBox.x,   (int)PlayerStatusBox.y,     (int)PlayerStatusBox.x+40,(int)PlayerStatusBox.y+40);
  int* Pre_point2 = rotatePoint(previewPlayerAngle,(int)PlayerStatusBox.x+80,(int)PlayerStatusBox.y,    (int)PlayerStatusBox.x+ 40,(int)PlayerStatusBox.y+40);
  int* Pre_point3 = rotatePoint(previewPlayerAngle,(int)PlayerStatusBox.x+80,(int)PlayerStatusBox.y+ 80,(int)PlayerStatusBox.x+ 40,(int)PlayerStatusBox.y+40);
  int* Pre_point4 = rotatePoint(previewPlayerAngle,(int)PlayerStatusBox.x,   (int)PlayerStatusBox.y+ 80,(int)PlayerStatusBox.x+ 40,(int)PlayerStatusBox.y+40);
  
  int* PreEX_point1 = rotatePoint(previewPlayerAngle,(int)PlayerStatusBox.x-5,(int)PlayerStatusBox.y   -5,(int)PlayerStatusBox.x+ 40,(int)PlayerStatusBox.y+40);
  int* PreEX_point2 = rotatePoint(previewPlayerAngle,(int)PlayerStatusBox.x+85,(int)PlayerStatusBox.y  -5,(int)PlayerStatusBox.x+ 40,(int)PlayerStatusBox.y+40);
  int* PreEX_point3 = rotatePoint(previewPlayerAngle,(int)PlayerStatusBox.x+85,(int)PlayerStatusBox.y+ 85,(int)PlayerStatusBox.x+ 40,(int)PlayerStatusBox.y+40);
  int* PreEX_point4 = rotatePoint(previewPlayerAngle,(int)PlayerStatusBox.x-5,(int)PlayerStatusBox.y + 85,(int)PlayerStatusBox.x+ 40,(int)PlayerStatusBox.y+40);
  if(!paused){
    previewPlayerAngle += 1*deltaTime;
  }
  
  
  int colorRect[4] = {242,200,145+(int)rect.shield*1.1f,255};
  int colorRectOutline[4] = {150,150,150+(int)rect.shield*1.05f,255};
  fillPOLYGON(PreEX_point1,PreEX_point2,PreEX_point3,PreEX_point4,PlayerStatusBox.y,colorRectOutline,0);
  fillPOLYGON(Pre_point1,Pre_point2,Pre_point3,Pre_point4,PlayerStatusBox.y,colorRect,0);
  free(Pre_point1);
  free(Pre_point2);
  free(Pre_point3);
  free(Pre_point4);
  free(PreEX_point1);
  free(PreEX_point2);
  free(PreEX_point3);
  free(PreEX_point4);}
  int digits;

  digits= SDL_floor(SDL_log10(rect.health + (rect.health <= 0))+1);
  char TEXT_health[7+digits];
  digits = SDL_floor(SDL_log10(rect.healthMax + (rect.healthMax <= 0))+1);
  char TEXT_healthMax[4+digits];
  digits = SDL_floor(SDL_log10(rect.shield + (rect.shield <= 0))+1);
  char TEXT_shield[7+digits];
  digits = SDL_floor(SDL_log10(rect.shieldMax + (rect.shieldMax <= 0))+1);
  char TEXT_shieldMax[4+digits];
  digits = SDL_floor(SDL_log10(rect.mag + (rect.mag <= 0))+1);
  char TEXT_bullets[8+digits];
  digits = SDL_floor(SDL_log10(rect.magMax + (rect.magMax <= 0))+1);
  char TEXT_bulletsMax[4+digits];
  digits = SDL_floor(SDL_log10(rect.speed + (rect.speed <= 0))+1);
  char TEXT_speed[6+digits];
  PlayerStatusBox.x -= windowWidth/8;
  PlayerStatusBox.y -= windowHeight/16;

  sprintf(TEXT_health,"Health %d",(int)rect.health);
  sprintf(TEXT_healthMax,"Max %d",(int)rect.healthMax);
  sprintf(TEXT_shield,"Shield %d",(int)rect.shield);
  sprintf(TEXT_shieldMax,"Max %d",(int)rect.shieldMax);
  sprintf(TEXT_bullets,"Bullets %d",(int)rect.mag);
  sprintf(TEXT_bulletsMax,"Max %d",(int)rect.magMax);
  sprintf(TEXT_speed,"Speed %d",(int)rect.speed);
  SDL_Rect HealthRectSrc = {
    0,0,200,200
  };
  SDL_Rect HealthRectDst = {
    PlayerStatusBox.x+10,
    PlayerStatusBox.y+180,20,20
  };
  SDL_Rect ShieldRectSrc = {
    0,200,200,200
  };
  SDL_Rect ShieldRectDst = {
    PlayerStatusBox.x+10,
    PlayerStatusBox.y+220,20,20
  };
  SDL_Rect BulletRectSrc = {
    200,0,200,200
  };
  SDL_Rect BulletRectDst = {
    PlayerStatusBox.x+10,
    PlayerStatusBox.y+260,20,20
  };
  SDL_Rect SpeedRectSrc = {
   200,200,200,200
  };
  SDL_Rect SpeedRectDst = {
   PlayerStatusBox.x+10,
    PlayerStatusBox.y+300,20,20
  };
  SDL_RenderCopy(renderer,POWERUPS_Texture,&HealthRectSrc,&HealthRectDst);
  SDL_RenderCopy(renderer,POWERUPS_Texture,&ShieldRectSrc,&ShieldRectDst);
  SDL_RenderCopy(renderer,POWERUPS_Texture,&BulletRectSrc,&BulletRectDst);
  SDL_RenderCopy(renderer,POWERUPS_Texture,&SpeedRectSrc,&SpeedRectDst);
  // HEALTH
  renderText(sizeof(TEXT_health),TEXT_health,PlayerStatusBox.x+40,PlayerStatusBox.y+185,sizeof(TEXT_health)*10,15,255,colorWHITE);
  renderText(sizeof(TEXT_healthMax),TEXT_healthMax,PlayerStatusBox.x+3*windowWidth/8-25-sizeof(TEXT_healthMax)*10,PlayerStatusBox.y+185,sizeof(TEXT_healthMax)*10,15,100,colorWHITE);
  // SHIELD
  renderText(sizeof(TEXT_shield),TEXT_shield,PlayerStatusBox.x+40,PlayerStatusBox.y+225,sizeof(TEXT_shield)*10,15,255,colorWHITE);
  renderText(sizeof(TEXT_shieldMax),TEXT_shieldMax,PlayerStatusBox.x+3*windowWidth/8-25-sizeof(TEXT_shieldMax)*10,PlayerStatusBox.y+225,sizeof(TEXT_shieldMax)*10,15,100,colorWHITE);
  // BULLETS 
  renderText(sizeof(TEXT_bullets),TEXT_bullets,PlayerStatusBox.x+40,PlayerStatusBox.y+265,sizeof(TEXT_bullets)*10,15,255,colorWHITE);
  renderText(sizeof(TEXT_bulletsMax),TEXT_bulletsMax,PlayerStatusBox.x+3*windowWidth/8-25-sizeof(TEXT_bulletsMax)*10,PlayerStatusBox.y+265,sizeof(TEXT_bulletsMax)*10,15,100,colorWHITE);
  // SPEED
  renderText(sizeof(TEXT_speed),TEXT_speed,PlayerStatusBox.x+40,PlayerStatusBox.y+305,sizeof(TEXT_speed)*10,15,255,colorWHITE);
  // STATUS
  char *TEXT_status = "Status";
  renderText(7,TEXT_status,UpgradeBox.x+UpgradeBox.w-sizeof(TEXT_status)*40+40,UpgradeBox.y-15,sizeof(TEXT_status)*40,40,255,colorWHITE);
  
  
  
  // RENDER UPGRADE MENU
  
  digits = SDL_floor(SDL_log10(rect.credit+(rect.credit <= 0))+1);
  char TEXT_credit[8+digits];
  sprintf(TEXT_credit,"Credits %d",(int)rect.credit);
  renderText(9,"Upgrades",UpgradeBox.x-17,UpgradeBox.y-15,10*20,40,255,colorWHITE);
  SDL_SetRenderDrawColor(renderer,210,210,210,100);
  SDL_Rect CreditBox = {
    PlayerStatusBox.x-150-(windowWidth/2-3*windowWidth/8-UpgradeBox.x),
    UpgradeBox.y+5,
    120,
    30
  };
  SDL_RenderFillRect(renderer,&CreditBox);
  renderText(8+digits,TEXT_credit,PlayerStatusBox.x-150-(windowWidth/2-3*windowWidth/8-UpgradeBox.x)+10,UpgradeBox.y+13,sizeof(TEXT_credit)*10,15,255,colorYELLOW);
  for(int i =0 ;i<sizeof(upgrades)/sizeof(upgrades[0]);i++){
    SDL_Rect InfoBox = {
    UpgradeBox.x+5,
    UpgradeBox.y+i*70+50,
    3*windowWidth/8-30,
    60
  };
  SDL_SetRenderDrawColor(renderer,210,210,210,100);
  SDL_RenderFillRect(renderer,&InfoBox);
  }
  SDL_Rect RectDst = {
    UpgradeBox.x+10,
    UpgradeBox.y-10,20,20
  };

   
  
     for(int i = 0;i<sizeof(upgrades)/sizeof(upgrades[0]);i++){
      RectDst.y += 70;
      SDL_RenderCopy(renderer,POWERUPS_Texture,&upgrades[i].RectSrc,&RectDst);
      renderText(upgrades[i].UpgradeTextSize,upgrades[i].UpgradeText,UpgradeBox.x+40,UpgradeBox.y+60+i*70,upgrades[i].UpgradeTextSize*12,20,255,colorWHITE);
      renderText(upgrades[i].DescriptionSize,upgrades[i].Description,UpgradeBox.x+15,UpgradeBox.y+90+i*70,upgrades[i].DescriptionSize*9,15,255,colorLIGHTGRAY);    
      renderText(upgrades[i].CurrentValueSize,upgrades[i].CurrentValue,PlayerStatusBox.x-upgrades[i].CurrentValueSize*10-25,UpgradeBox.y+93+i*70,(upgrades[i].CurrentValueSize)*8,13,255,colorLIGHTGRAY);    
      free(upgrades[i].CurrentValue);

      SDL_Rect ButtonRect = {
        PlayerStatusBox.x-100,
        UpgradeBox.y+60+i*70,
       50,25
      };
      SDL_SetRenderDrawColor(renderer,170+50*upgrades[i].buttonHover,170+50*upgrades[i].buttonHover,13+(157+50*upgrades[i].buttonHover)*(upgrades[i].price > rect.credit),255);
      SDL_RenderFillRect(renderer,&ButtonRect);
      int* colorButton = (int*)colorYELLOW;
      if(upgrades[i].price > rect.credit){
        colorButton = (int*)colorLIGHTGRAY;
      }
      
      renderNum((int)SDL_floor(SDL_log10(upgrades[i].price+1)+1),upgrades[i].price,PlayerStatusBox.x-90,
      UpgradeBox.y+65+i*70,(int)SDL_floor(SDL_log10(upgrades[i].price+1)+1)*13,17,255,colorButton);

      if(mouse.x > ButtonRect.x && mouse.x <ButtonRect.x+ButtonRect.w  && mouse.y> ButtonRect.y && mouse.y<ButtonRect.y + ButtonRect.h){
            upgrades[i].buttonHover += 4*deltaTime;
            if (upgrades[i].buttonHover >= 1){
              upgrades[i].buttonHover = 1;
            }
            if(key.left == 2){
              if(upgrades[i].price <= rect.credit){
              upgrades[i].value += upgrades[i].valueIncrement;
              rect.credit -= upgrades[i].price;
              upgrades[i].price *= 2;
              alertCall(upgrades[i].alertMessageSize,upgrades[i].alertMessage,colorWHITE);
              }
              else {
                alertCall(23,"Cant Afford To Upgrade",colorRED);
              }
            }
      }
      else {
        upgrades[i].buttonHover -= 4*deltaTime;
        if(upgrades[i].buttonHover <= 0){
          upgrades[i].buttonHover = 0;
        }
      }
      free(upgrades[i].alertMessage);
  }  


 // UPDATE UPGRADES
 for(int i = 0;i<sizeof(upgrades)/sizeof(upgrades[0]);i++){
  if(i == 0){ // HEALING TIME
   upgrades[i].valueIncrement = -upgrades[i].value/2;
   rect.healingTime = (int)upgrades[i].value;
   int digits = (int)SDL_floor(SDL_log10(upgrades[i].value+1)+1);
   char TEXTCURRENT[digits+3];
   upgrades[i].alertMessageSize = 23;
   if(upgrades[i].value > 1000){
     sprintf(TEXTCURRENT,"%d s",(int)upgrades[i].value/1000);
     digits = (int)SDL_floor(SDL_log10(upgrades[i].value/1000 +1));
   }
   else {
    sprintf(TEXTCURRENT,"%d ms",(int)upgrades[i].value);
   } 
   upgrades[i].CurrentValueSize = digits+4;
   upgrades[i].CurrentValue = (char*)malloc(upgrades[i].CurrentValueSize);
   SDL_strlcpy(upgrades[i].CurrentValue,TEXTCURRENT,upgrades[i].CurrentValueSize);
  }
  if(i == 1){ // INCREASE DAMAGE
   upgrades[i].valueIncrement = 1;
   rect.bulletDmg = (int)upgrades[i].value;
   int digits = (int)SDL_floor(SDL_log10(upgrades[i].value+1)+1);
   char TEXTCURRENT[digits+5];
   sprintf(TEXTCURRENT,"%d dmg",(int)upgrades[i].value);
   upgrades[i].CurrentValueSize = digits+5;
   upgrades[i].CurrentValue = (char*)malloc(upgrades[i].CurrentValueSize);
   SDL_strlcpy(upgrades[i].CurrentValue,TEXTCURRENT,upgrades[i].CurrentValueSize);  
  }
  if(i == 2){ // SHIELD STRENGTH
   upgrades[i].valueIncrement = -0.05f;
   rect.shieldPower = upgrades[i].value;
   int digits = (int)SDL_floor(SDL_log10(100/upgrades[i].value+1)+1);
   char TEXTCURRENT[digits+5];
   sprintf(TEXTCURRENT,"%d pwr",(int)SDL_floor((100/upgrades[i].value)));
   upgrades[i].CurrentValueSize = digits+5;
   upgrades[i].CurrentValue = (char*)malloc(upgrades[i].CurrentValueSize);
   SDL_strlcpy(upgrades[i].CurrentValue,TEXTCURRENT,upgrades[i].CurrentValueSize);  
  }
  if(i == 3){ // PLAYER SPEED
   upgrades[i].valueIncrement = 5;
   rect.speed = upgrades[i].value;
   int digits = (int)SDL_floor(SDL_log10(upgrades[i].value+1)+1);
   char TEXTCURRENT[digits+5];
   sprintf(TEXTCURRENT,"%d UPS",(int)upgrades[i].value);
   upgrades[i].CurrentValueSize = digits+5;
   upgrades[i].CurrentValue = (char*)malloc(upgrades[i].CurrentValueSize);
   SDL_strlcpy(upgrades[i].CurrentValue,TEXTCURRENT,upgrades[i].CurrentValueSize); 
  }
  if(i == 4){ // LUCK
   upgrades[i].valueIncrement = -1;
   rect.luck = (int)upgrades[i].value;
   int digits = (int)SDL_floor(SDL_log10(upgrades[i].value+1)+1);
   char TEXTCURRENT[digits+6];
   sprintf(TEXTCURRENT,"1 in %d",(int)upgrades[i].value);
   upgrades[i].CurrentValueSize = digits+6;
   upgrades[i].CurrentValue = (char*)malloc(upgrades[i].CurrentValueSize);
   SDL_strlcpy(upgrades[i].CurrentValue,TEXTCURRENT,upgrades[i].CurrentValueSize);
  }
  }	
 
  if(!paused){
  if(key.tab && upgradeTabBool){
    upgradeTab = 0;
  }
  if(!key.tab){
    upgradeTabBool = 1;
  }
  }
}
void drawGameOver(void){
  SDL_SetRenderDrawColor(renderer,100,100,100,150);
  SDL_Rect gameOverBox = {
    windowWidth/4,
    windowHeight/4,
    windowWidth/2,
    windowHeight/2
  };
  SDL_RenderFillRect(renderer,&gameOverBox);
    
    char TEXT_died[] = "You Died";
    char TEXT_restart[] = "Restart";
    char TEXT_exit[] = "Exit";
    
  
    if(mouse.y > windowHeight/4+windowHeight/8 && mouse.y < windowHeight/4+windowHeight/8 + 30 && mouse.x > windowWidth/2-sizeof(TEXT_restart)*5 && mouse.x < windowWidth/2+sizeof(TEXT_restart)*5){
     ButtonSelectorNUM = 0;
    }
    else if(mouse.y > windowHeight/4+windowHeight/8 + 30 && mouse.y < windowHeight/4+windowHeight/8 + 60  && mouse.x > windowWidth/2-sizeof(TEXT_exit)*5 && mouse.x < windowWidth/2+sizeof(TEXT_exit)*5){
      ButtonSelectorNUM = 1;
      
    }
    else {
      ButtonSelectorNUM = -1;
    }
  
  if(ButtonSelectorNUM == -1){
    ButtonSelectorOpacity -= 1000*deltaTime;
    if(ButtonSelectorOpacity<0){
     ButtonSelectorOpacity = 0;
    }
   
  }
 else 
 {    
  if(ButtonSelectorNUM == 0){
     ButtonSelectorY = (ButtonSelectorY-(windowHeight/4+windowHeight/8))*SDL_pow(0.6,deltaTime*30) + windowHeight/4+windowHeight/8;
     ButtonSelectorX = (ButtonSelectorX-(windowWidth/2-sizeof(TEXT_restart)*5))*SDL_pow(0.6,deltaTime*30) + windowWidth/2-sizeof(TEXT_restart)*5;
     ButtonSelectorSizeX = sizeof(TEXT_restart)*10;
    }
  if(ButtonSelectorNUM == 1){
      ButtonSelectorY = (ButtonSelectorY-(windowHeight/4+windowHeight/8 + 30))*SDL_pow(0.6,deltaTime*30)+ windowHeight/4+windowHeight/8 + 30;
      ButtonSelectorX = (ButtonSelectorX-(windowWidth/2-sizeof(TEXT_exit)*5))*SDL_pow(0.6,deltaTime*30) + windowWidth/2-sizeof(TEXT_exit)*5;
      ButtonSelectorSizeX = sizeof(TEXT_exit)*10;
  }
  ButtonSelectorSizeY = 20;
  ButtonSelectorOpacity += 1000*deltaTime;
  if(ButtonSelectorOpacity>200){
   ButtonSelectorOpacity = 200;
  }
 }
   SDL_Rect buttonRect = {
        (int)ButtonSelectorX-4,
        (int)ButtonSelectorY-4,
        (int)ButtonSelectorSizeX+8,
        (int)ButtonSelectorSizeY+8
      };
  SDL_SetRenderDrawColor(renderer,100,100,100,(int)ButtonSelectorOpacity);
  SDL_RenderFillRect(renderer,&buttonRect);
  renderText(sizeof(TEXT_died),TEXT_died,windowWidth/2-80,windowHeight/4,200,50,255,colorWHITE);
  renderText(sizeof(TEXT_restart),TEXT_restart,windowWidth/2-sizeof(TEXT_restart)*5,windowHeight/4+windowHeight/8,sizeof(TEXT_restart)*10,20,255,colorWHITE);
  renderText(sizeof(TEXT_exit),TEXT_exit,windowWidth/2-sizeof(TEXT_exit)*5,windowHeight/4+windowHeight/8+30,sizeof(TEXT_exit)*10,20,255,colorWHITE);
   

   // CLICK
   if(key.left == 2 && ButtonSelectorNUM != -1){
     if(ButtonSelectorNUM == 0){
      paused = 0;
      setup();
     }
     if(ButtonSelectorNUM == 1){
      paused = 0;
      main_menu = 1;
      setup();
     }
   }
}
void delay(void){
  if(maxFPS > 0){
      int delay = (int)(last_frame +(float)(1000/maxFPS) - (float)SDL_GetTicks());
      if(delay>0){
        SDL_Delay(last_frame +(float)(1000/maxFPS) - SDL_GetTicks());
      }
     }
    deltaTime = ((float)SDL_GetTicks() - last_frame)/1000.0f;
    last_frame = SDL_GetTicks();
}

int main(int argc, char *argv[]){
 running = initGame();
 // START PROGRAM // 
 setup();
 while(running){
     // DELAY
     delay();
     input();
     if(!main_menu){
      if(!paused && !gameOver){
       update();
      }
      draw();
      if(upgradeTab && !gameOver){
        drawUpgradeTab();
      }
      if(paused && !gameOver){
        drawPauseMenu();
      }
      if(gameOver){       
        drawGameOver();
      }
     }
     else if(main_menu){
      drawMainMenu();
     }
     
     
    // RENDER SCREEN (PER FRAME) 
    SDL_RenderPresent(renderer);
 }

 // END PROGRAM // 
 
 kill();
 return 0;
}

