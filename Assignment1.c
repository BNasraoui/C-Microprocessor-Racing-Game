#include <stdlib.h>
#include <math.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>

#define MILLISECONDS 1000
#define car_width 8
#define car_height 4
#define road_width 24
#define road_height 4
#define tree_width 5
#define tree_height 4
#define blockadewidth 5
#define blockadeHeight 1
#define depot_width 13
#define depot_height 6


bool gameOver;
bool gameWon;
bool game;

int carX;
int carY;
double depotx;
double depoty;

int health;
int speed;
int score;
double fuel;
double distance;

double current_time;
double lastTime;
double start_time;

int treeCount;
int blockadeCount;
int roadCount;
double dy;

int dashY;
int dirtWidth;

timer_id depotTimer;

sprite_id road[100];
sprite_id tree[100];
sprite_id blockade[2];
sprite_id depot;
sprite_id car;

void SplashScreen(void) {
  /* code */

  char *splashCharImage = "  Race: To Zombie Mountain  "
                          "        Ben Nasraoui        "
                          "     Student No. 9437207    ";

  char *howToImage = "             HOW TO PLAY               "
                     "                                       "
                     "  Dodge Zombies To Protect Your Car.   "
                     "                                       "
                     "   Keep Fuel Topped up by driving      "
                     "    through stations when you can.     "
                     "                                       "
                     "   Don't let your health reach zero.   ";

 char *controlImage =   "        CONTROLS        "
                        "                        "
                        "W BUTTON:     Accelerate"
                        "S BUTTON:     Brake     "
                        "A BUTTON:     Move LEFT "
                        "D BUTTON:     MOVE RIGHT"
                        "                        "
                        "                        ";

  sprite_id splash;
  sprite_id howTo;
  sprite_id control;

  splash = sprite_create((screen_width()/ 2)-14, 2, 28, 3, splashCharImage);
  howTo = sprite_create((screen_width() / 4) - 19, (screen_height() /4), 39, 8, howToImage);
  control = sprite_create((screen_width() /2) + (screen_width() / 4) -12, screen_height()/4, 24, 8, controlImage);

  sprite_draw(splash);
  sprite_draw(howTo);
  sprite_draw(control);

  draw_string((screen_width() / 2)-14, screen_height() - 3, "Press any key to continue...");
}

void Border(void){
  int screenX1 = 0;                       // Far Left, Right, Top, and Bottom of Screen
  int screenX2 = screen_width() - 1;      //
  int screenY1 = 0;                       //
  int screenY2 = screen_height() - 1;     //

  draw_line(screenX1, screenY1, screenX2, screenY1, '*');  // Drawing Border
  draw_line(screenX2, screenY1, screenX2, screenY2, '*'); //
  draw_line(screenX2, screenY2, screenX1, screenY2, '*'); //
  draw_line(screenX1, screenY2, screenX1, screenY1, '*'); //

}

void Dashboard(void){
  /* code */
  int screenX1 = 1;                                           // Far Left, Right, Top, and Bottom of Screen
  int screenX2 = screen_width() - 1;                          //
  dashY =  2;      // Position at which the Dash starts

  draw_line(screenX1, dashY, screenX2, dashY, '*');           // Drawing Dashboard

  draw_string(3, dashY-1, "Health:   %  Distance:    km Time:      Speed:       Fuel:   %");
}

void initGameValues(void)
{
    start_time = get_current_time();
    health = 100;
    speed = 0.0;
    distance = 0;
    fuel = 100;
    score = 0;
    current_time = 0.0;

    dy = 0.0;
}

static void updateDashboard(void){
  current_time = get_current_time() - start_time;
  lastTime = current_time;

  draw_int(10, dashY-1, health);
  draw_double(37, dashY-1, ((int)(current_time*10 +0.5)/10.0));
  draw_int(27, dashY-1, distance);
  draw_int(49, dashY-1, speed);
  draw_int(61, dashY-1, fuel);
}

void initCar(void)
{
  char *carImage =  "()----()"
                    " |    | "
                    " |    | "
                    "()____()";

  carX = screen_width()/2 - (car_width/2);
  carY = (screen_height() - car_height - 1);

  car = sprite_create(carX, carY, car_width, car_height, carImage);
  sprite_draw(car);
}

void initRoad(void)
{
  char *roadImage =   "|          ||          |"
                      "|          ||          |"
                      "|                      |"
                      "|                      |";

  if(dashY % 4 != 0){roadCount = ((screen_height() - dashY)/4)+2;}
  else{roadCount = ((screen_height() - dashY)/4)+1;}


  for(int i = 0; i < roadCount; i++)
  {
    road[i] = sprite_create((screen_width()/2)-(road_width/2) + 1, ((i*4)+1)-4, road_width, road_height, roadImage);
    sprite_draw(road[i]);
  }
}

void initTrees(void){
  char *treeImage = "  *  "
                    " *** "
                    "*****"
                    "  |  ";

  int treeRows = ((screen_height() - dashY)/tree_height);
  treeCount = treeRows*2;

  dirtWidth = (screen_width()/2) - (road_width/2) - depot_width;
  int treeColumns = (dirtWidth/tree_width);

  for(int i = 0; i < treeRows; i++)
  {
      int xPos = (rand() % dirtWidth) - tree_width;

      tree[i] = sprite_create(xPos, (tree_height*i)+dashY, tree_width, tree_height, treeImage);
      sprite_draw(tree[i]);
  }

  for(int j = 0; j < treeRows; j++){
      int x2Pos = ((rand() % dirtWidth)) + (screen_width()/2) + (road_width/2);
      tree[j+(treeCount/2)] = sprite_create(x2Pos, (tree_height*j)+dashY, tree_width, tree_height, treeImage);
      sprite_draw(tree[j+(treeCount/2)]);
  }
}

void initBlockade(void){
  char *blockadeImage = "/***\\";
  blockadeCount = 2;
  for(int i = 0; i < blockadeCount; i++){
    int xPos = ((screen_width()/2) - (dirtWidth/2)) + (rand() % dirtWidth);
    int yPos = (rand() % (screen_height()/2)) + dashY;

     blockade[i] = sprite_create(xPos,yPos,blockadewidth, blockadeHeight, blockadeImage);
     sprite_draw(blockade[i]);
  }

}

void initFuelDepot(void)
{
  char *depotImage = "_____________"
                     "| F |        "
                     "| U |  XXXX  "
                     "| E |  XXXX  "
                     "| L |  XXXX  "
                     "|___|________";

   depotx = ((screen_width()/2)-(road_width/2)) - depot_width;
   depoty = (screen_height()/2) - (depot_height/2);
   int randtime = (rand() % 15000) + 5000;
   depotTimer = create_timer(randtime);
   depot = sprite_create(depotx, depoty, depot_width, depot_height, depotImage);
   sprite_draw(depot);
}

void speedChange(int keyPressed){

  if(keyPressed == 'w'){          // Accelerating
    if(speed < 10){ speed++; }
  }

  else if(keyPressed == 's'){     // Deccelerating
    if(speed > 0){ speed--; }
  }

  else if(keyPressed == 1000){    // Car in dirt;
    if(speed > 3){speed = 3;}
  }

  else if(keyPressed == 2000){
    speed = 0;                    // Car Collision
  }

  dy = (speed / 100000.0);        // Update Displacement vectors
}

void carPosChange(int keyPressed){

  if(keyPressed == 'a' && sprite_x(car) != 1 && speed > 0){
    carX--;
    sprite_move_to(car, carX, carY);
  }
  else if(keyPressed == 'd'&& sprite_x(car) != screen_width() - 1 && speed > 0){
    carX++;
    sprite_move_to(car, carX, carY);
  }
}

void checkInput(void)
{
  int keyPressed = get_char();

  if (keyPressed == 'w' || keyPressed == 's'){speedChange(keyPressed);}
  else if (keyPressed == 'a' || keyPressed == 'd'){carPosChange(keyPressed);}
  else if(keyPressed == '0'){wait_char();}
}

void updateBackground(void){
  for(int i = 0; i < treeCount; i++)
  {
    sprite_turn_to(tree[i], 0, dy);
    sprite_step(tree[i]);
    double temp = (5);
    if(round(sprite_y(tree[i])) == screen_height() - 1)
    {
      sprite_move_to(tree[i], sprite_x(tree[i]), 3); // Wrap screen
    }
    sprite_draw(tree[i]);
  }

  for(int j = 0; j < roadCount; j++)
  {
    sprite_turn_to(road[j], 0.0, dy);
    sprite_step(road[j]);
    if(round(sprite_y(road[j])) == screen_height() - 1){
      sprite_move_to(road[j], sprite_x(road[j]), 3);
    } // Wrap screen
    sprite_draw(road[j]);
  }

  for(int k = 0; k < blockadeCount; k++)
  {
    sprite_turn_to(blockade[k], 0.0, dy);
    sprite_step(blockade[k]);
    if(round(sprite_y(blockade[k])) == screen_height() - 1){
      int xPos = ((screen_width()/2) - (dirtWidth/2)) + (rand() % dirtWidth);
      sprite_move_to(blockade[k], xPos, 3);
    }
    sprite_draw(blockade[k]);
  }


  if(sprite_visible(depot)){
    sprite_turn_to(depot, 0.0, dy);
    sprite_step(depot);
    sprite_draw(depot);
    if(round(sprite_y(depot)) == screen_height() - 1){
      sprite_hide(depot);
      timer_reset(depotTimer);
    }
  }

  if(timer_expired(depotTimer) && (!sprite_visible(depot))){
    sprite_move_to(depot, depotx, depoty);
    sprite_show(depot);
    sprite_draw(depot);
  }
}

void resetScreen(void){
  clear_screen();
  carX = (screen_width()/2) - (car_width/2);
  sprite_move_to(car, carX, carY);
  sprite_move_to(blockade[0], (screen_width()/2)-(dirtWidth/2), sprite_y(blockade[0]));
  sprite_move_to(blockade[1], (screen_width()/2)-(dirtWidth/2), sprite_y(blockade[1]));
  sprite_draw(car);
  sprite_draw(blockade[0]);
  sprite_draw(blockade[1]);
  speed = 0;
}

void checkCollision(void)
{
  for(int i = 0; i < treeCount; i++) // Hit tree?
  {
    if(round(sprite_y(tree[i])) + tree_height >= screen_height() - 1 - car_height){
      if(round(sprite_x(tree[i])) >= sprite_x(car) && round(sprite_x(tree[i])) < sprite_x(car) + car_width)
      {
        resetScreen();
        speedChange(2000);
        health = health - 20;
        fuel = 100;
      }
    }
  }

  for(int i = 0; i < blockadeCount; i++) // Hit blockade?
  {
    if(round(sprite_y(blockade[i])) >= screen_height() - 1 - car_height){
      if(round(sprite_x(blockade[i])) >= sprite_x(car) && round(sprite_x(blockade[i])) < sprite_x(car) + car_width)
      {
        resetScreen();
        speedChange(2000);
        health = health - 25;
        fuel = 100;
      }
    }
  }

  if((round(sprite_y(depot)) >= screen_height() - 1 - car_height) && (sprite_visible(depot))){ // in Depot?
    if(round(sprite_x(depot)) + depot_width >= sprite_x(car) && round(sprite_x(depot)) < sprite_x(car) + car_width)
    {
      fuel = 100;
    }
  }

  if((sprite_x(car) < ((screen_width()/2)- (road_width/2))) || sprite_x(car) + car_width -1 > ((screen_width()/2) + (road_width/2))) // car in dirt?
  {
    speedChange(1000);
  }

}

void updateGame(void){
  distance += dy;
  if(distance >= 250){
    gameOver = true;
    gameWon = true;
  }

  fuel = fuel - (speed/100000.0);
  if(fuel <= 0 || health <= 0){
    gameOver = true;
    gameWon = false;
  }
}

bool endScreen(void)
{
  bool continueGame;
  clear_screen();
  while (get_char() >= 0){}
  Border();

  if(gameWon){draw_string((screen_width()/2) - 12, screen_height()/3, "CONGRATULATIONS YOU WIN!");}
  else{draw_string((screen_width()/2) - 6, screen_height()/3, "YOU LOST ;_;");}

  draw_string((screen_width()/2)-23, (screen_height()/3)+1, "Total Time:      seconds Total Distance: ");
  draw_double((screen_width()/2)-7, (screen_height()/3)+1, current_time);
  draw_double((screen_width()/2)+18, (screen_height()/3)+1, distance);

  draw_string((screen_width()/2)-8, (screen_height()/3)+2, "PLAY AGAIN? (Y/N)");
  show_screen();
  wait_char();
  int keyPressed = get_char();

  if(keyPressed == 'y' || keyPressed == 'Y'){continueGame = true;}
  else if(keyPressed == 'n' || keyPressed == 'N'){continueGame = false;}

  return continueGame;
}

void intro(void){
  setup_screen();
  Border();
  SplashScreen();             // Show Splash Screen
  show_screen();
  wait_char();
}

void setupGame(void){
  gameOver = false;
  clear_screen();
  Border();
  Dashboard();
  initGameValues();
  updateDashboard();
  initRoad();
  initCar();
  initTrees();
  initFuelDepot();
  initBlockade();
  show_screen();
}


int main(void) {
  game = true;

  while(game != false){
    clear_screen();
    gameOver = false;
    intro();

    setupGame();
    while(gameOver != true){
      clear_screen();
      checkInput();
      checkCollision();
      updateBackground();
      updateGame();
      Dashboard();
      updateDashboard();
      Border();
      sprite_draw(car);
      show_screen();
      if(gameOver){timer_pause(1500);}
    }
    game = endScreen();
  }

}
