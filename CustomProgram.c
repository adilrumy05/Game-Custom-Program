//Adil Rumy (104388564)
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <time.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#define WINDOW_WIDTH 1500
#define WINDOW_HEIGHT 800
#define M_PI 3.14159265358979323846

// Player struct type
typedef struct {
    char name[60];
    char position[10];
    char country[20];
    char club[10];
    int rating;
} Player;

//function to draw the football field
void drawField(SDL_Renderer *renderer) {
    //draw green backgroud
    SDL_SetRenderDrawColor(renderer, 5, 114, 5, 255);
    //clear the renderer with the draw color
    SDL_RenderClear(renderer);
    
    //drawing white lines inside the box
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); //to draw the white circle in the centre
		int centerX = 750; //centre of the circle in X
		int centerY = 400; //centre of the circle in Y
		int radius = 90; //radius of the circle
		for (int angle = 0; angle <= 360; angle += 1) {
            double x = centerX + radius * cos(angle * M_PI / 180); //to find the X of a certain point in the circle
            double y = centerY + radius * sin(angle * M_PI / 180); //to find the Y of a certain point in the circle
            SDL_RenderDrawPoint(renderer, (int)x, (int)y); //draws a pixel in the calculated X and Y, making it a circle after the loop
        }
    
    //drawing the top and bottom smaller rectangle inside the outerline
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_Rect rect1 = {600, 50, 300, 125};
    SDL_RenderDrawRect(renderer, &rect1);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_Rect rect2 = {600, 625, 300, 125};
    SDL_RenderDrawRect(renderer, &rect2);

    //draws the Y center line that passes through the circle
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, 250, 400, 1249, 400);

    //draw the outerline
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect rec3 = {250, 50, 1000, 700};
    SDL_RenderDrawRect(renderer, &rec3);   
}

//function to read players' details from file
int readPlayers(Player* players, int max_players) {

    FILE* file = fopen("Players.txt", "r"); //opens file and check if it was successfully opened
    if (file == NULL) {
        printf("Error opening file.\n");
        return 0;
    }

    int numofplayers = 0;
    // we do fscanf(....)!=EOF to make sure that it reads all lines untill the end of the file
    while (fscanf(file, " %59[^\n] %9[^\n] %19[^\n] %9[^\n] %d", players[numofplayers].name, players[numofplayers].position, players[numofplayers].country, players[numofplayers].club, &players[numofplayers].rating) != EOF){
        /* in the above line. We use %59[^\n] %9[^\n] %19[^\n] %9[^\n] to limit the amount of characters read, 
           so that if the string is too long it doesnt lead to buffer overflow*/

        numofplayers++; //increments numofplayers
        if (numofplayers >= max_players) { //once numofplayers exceeds the max players available, it leaves the loop
            printf("Maximum number of players reached.\n");
            break;
        }
    }
        
    fclose(file); //closes file
    return numofplayers; //returns the number of players
}

//Function to calculate the teams overal rating 
int total_rating(Player* options_array, int selected, int team_rating){
    if (selected == -1){ //incase if the user clicks outside the wanted region 
        team_rating+=0;
    }else{
        team_rating+=options_array[selected].rating; //increments the selected players rating into the total team's rating
    }

    return team_rating;
}

//Function to calculate the teams overal chemistry 
int total_chemisty(Player* team){
    int chem_position[11];
    int final_chem=0;

    //for loop is used to iterate through all 11 players in the team, and add up the similarities
    for (int i = 0; i < 11; i++){
        int player_chem = 0;
        for(int x = 0; x < 11; x++){
            if (i != x) { 
                if (strcasecmp(team[i].club, team[x].club) == 0){ 
                    //if the player shares the same club it increments the chemisty of the player
                    player_chem++;
                }

                if(strcasecmp(team[i].country, team[x].country) == 0){
                    //if the player shares the same country it increments the chemisty of the player
                    player_chem++;
                }
            }
        }

        chem_position[i] = player_chem;
        final_chem += player_chem; //adds the players chemistry into the teams overall chemisty
    }

    return final_chem;//returns the teams chemistry
}

//function to draw the score on the screen
void load_score(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color, int score){
    char text[50];
    int texW=0; //text's width
    int texH=0; //text's height
    snprintf(text, sizeof(text),"%d %%", score); //stores the score into text

    //creating the surface and textures for the text
    SDL_Surface *TextScore_sur= TTF_RenderText_Solid(font,text,color);  
    SDL_Texture *TextScore_tex=SDL_CreateTextureFromSurface(renderer,TextScore_sur);

    SDL_QueryTexture(TextScore_tex,NULL,NULL,&texW,&texH); //queries the the texts infromation
    SDL_Rect ScoreRect={1330, 100, texW+30, texH+30}; //position and size of the text
    SDL_RenderCopy(renderer,TextScore_tex,NULL,&ScoreRect); //renders the text into the screen
    
    //destory and free the texture and surface
    SDL_DestroyTexture(TextScore_tex);
    SDL_FreeSurface(TextScore_sur);   
}

//function to draw the normal texts 
void load_text(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color, char text[], int X, int Y, int addW, int addH){
    int texW=0;
    int texH=0;

    //creating the surface and textures for the text
    SDL_Surface *TextScore_sur= TTF_RenderText_Solid(font,text,color);
    SDL_Texture *TextScore_tex=SDL_CreateTextureFromSurface(renderer,TextScore_sur);
    
    SDL_QueryTexture(TextScore_tex,NULL,NULL,&texW,&texH); //queries the the texts infromation
    SDL_Rect ScoreRect={X, Y, texW+addW, texH+addH}; //position and size of the text
    SDL_RenderCopy(renderer,TextScore_tex,NULL,&ScoreRect); //renders the text into the screen

    //destory and free the texture and surface
    SDL_DestroyTexture(TextScore_tex);
    SDL_FreeSurface(TextScore_sur);
}

//function to draw the volume on the esc menu
void load_volume(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color, int volume){
    int output_volume=volume * 0.78; //to make the shown volume to not exceed 100%
    char text1[50];
    int texW=0;
    int texH=0;

    snprintf(text1, sizeof(text1),"Current volume:%d %%", output_volume); //stores the volume into text1

    //creating the surface and textures for the text
    SDL_Surface *TextScore_sur1= TTF_RenderText_Solid(font,text1,color);
    SDL_Texture *TextScore_tex1=SDL_CreateTextureFromSurface(renderer,TextScore_sur1);

    SDL_QueryTexture(TextScore_tex1,NULL,NULL,&texW,&texH); //queries the the texts infromation
    SDL_Rect ScoreRect={600, 200, texW+35, texH+30}; //position and size of the text
    SDL_RenderCopy(renderer,TextScore_tex1,NULL,&ScoreRect); //renders the text into the screen
    
    //destory and free the texture and surface
    SDL_DestroyTexture(TextScore_tex1);
    SDL_FreeSurface(TextScore_sur1);  
}

// Function to get 3 random options for the user to select
Player* options(Player* players, int numofplayers, Player* display, char* PosToFind) {
    Player PlayersInthePosition[15];
    // Allocate memory for three players
    display = malloc(3 * sizeof(Player));
    if (display == NULL) { //checks if the memory was successfully allocated
        printf("Memory allocation failed.\n");
        return NULL;
    }

    //store all the players found for the given position in an array
    int Index = 0;
    /*if the player in the main Player array shared the same desired position, 
      it stores that player into an array dedicated to only those players in that position*/
    for (int i = 0; i < numofplayers; i++) {
        if (strcmp(players[i].position, PosToFind) == 0) { 
            PlayersInthePosition[Index++] = players[i]; 
            /*once the player has been added into the new array, 
              the index increments for the next player*/
        }
    }
    
    //shuffle the array of players
    for (int i = Index - 1; i > 0; i--) {
        int j = rand() % (i + 1); //stores a random number between 0 and i
        Player temp = PlayersInthePosition[i]; 
        PlayersInthePosition[i] = PlayersInthePosition[j];
        PlayersInthePosition[j] = temp;
    } 

    //copy the first three elements to the display array
    for (int x = 0; x < 3 && x < Index; x++) {
        display[x] = PlayersInthePosition[x];
    }

    return display;
}

//fucntion for if there is an error in displaying the players
int display_error(Player* display,SDL_Renderer *renderer,SDL_Window *window){
    if (display == NULL) {
        printf("display is empty");
        free(display);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
}

//function to create the option card
int create_options(char **filename,Player* display,SDL_Renderer *renderer,SDL_Window *window,SDL_Texture *cardTexture[3]){
    SDL_Surface *cardsurface[3];
    for (int j = 0; j < 3; j++) {
        filename[j] = malloc(100 * sizeof(char)); //allocates 100 characters into filename 
        if (filename[j] == NULL) {
            printf("Memory allocation failed.\n");
            for (int k = 0; k < j; k++) {
                free(filename[k]);  //frees the memory
            }
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
    }

    for (int j=0;j<3;j++){
        sprintf(filename[j], "images/%s.png", display[j].name); //stores "images/ .png" with the display name before the .png
        cardsurface[j] = IMG_Load(filename[j]); //loads the image into the surface
        if (!cardsurface[j]) {
            printf("IMG_Load Error: %s\n", IMG_GetError());
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }

        cardTexture[j] = SDL_CreateTextureFromSurface(renderer, cardsurface[j]); //creates a texture with the surface
        SDL_FreeSurface(cardsurface[j]); //free the surface
        if (!cardTexture[j]) {
            printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
    }
}

//function to display the card options to the user 
void display_options(SDL_Renderer *renderer,SDL_Texture *cardTexture[3]){
    int X=50;
    int Y=100;
    int W=150;
    int H=200;

    SDL_Rect cardRect1 = {X, Y, W, H}; 
    SDL_RenderCopy(renderer, cardTexture[0], NULL, &cardRect1); //render the first option

    Y+=225;
    SDL_Rect cardRect2 = {X, Y, W, H};
    SDL_RenderCopy(renderer, cardTexture[1], NULL, &cardRect2); //render the second option

    Y+=225;
    SDL_Rect cardRect3 = {X, Y, W, H};
    SDL_RenderCopy(renderer, cardTexture[2], NULL, &cardRect3); //render the third option
}

//function to free the display array, filename array, and to destroy the cards textures
void free_and_destroy(Player* display,char **filename,SDL_Texture *cardTexture[3]){
    free(display); //free the display array
    for (int j = 0; j < 3; j++) {
        free(filename[j]); //freeing the filenames
    }
    for (int x=0;x<3;x++){
        SDL_DestroyTexture(cardTexture[x]); //destroying the textures
    }
}

//function that allows the user to select the card they want 
int select(int mouseX, int mouseY, char **filenames, SDL_Texture **textures, bool *complete,Mix_Chunk* SelectSound){
    int selected=-1; //declares the variable selected and asigns -1 to it

    if (mouseX > 50 && mouseX < 200 && mouseY > 100 && mouseY < 300) //the first card's borders
    {
        Mix_PlayChannel(-1, SelectSound, 0); //plays the select sound effect
        selected = 0; //if first card is selected, it asigns 0 to the variable selected
        *complete = true; //asigns true to complete to indicate the card has been selected
    }
    else if (mouseX > 50 && mouseX < 200 && mouseY > 325 && mouseY < 525) //the second card's borders
    {
        Mix_PlayChannel(-1, SelectSound, 0);
        selected = 1; //if second card is selected, it asigns 1 to the variable selected
        *complete = true;
    }
    else if (mouseX > 50 && mouseX < 200 && mouseY > 550 && mouseY < 750) //the third card's borders
    {
        Mix_PlayChannel(-1, SelectSound, 0);
        selected = 2; //if third card is selected, it asigns 2 to the variable selected
        *complete = true;
    }

    //when one of the card is selected, the rest of the card's textures will be destroyed
    if (selected==0){
        SDL_DestroyTexture(textures[1]);
        SDL_DestroyTexture(textures[2]);
    }else if (selected==1){
        SDL_DestroyTexture(textures[0]);
        SDL_DestroyTexture(textures[2]);
    }else if (selected==2){
        SDL_DestroyTexture(textures[0]);
        SDL_DestroyTexture(textures[1]);
    }

    return selected; //selected value is returned
}

//function to move the selected card to its new position
void selected_card(int newX,int newY,int selected,SDL_Texture **textures,SDL_Renderer *renderer){
    SDL_Rect selected_cardRect = {newX, newY, 100, 150}; // New position for the selected card
    SDL_RenderCopy(renderer, textures[selected], NULL, &selected_cardRect); //renders the card onto the screen
}

//main function
int main(int argc, char *argv[]) {
    bool completed[11] = {false}; //array of boolean values to see if a player is selected
    bool start=false;  //boolean value to inidicate the game started
    bool leave_MainMenu=false; //boolean value to indicate to leave the main menu
    bool open_EscMenu=false; //boolean value inidicating that the esc menu is open
    bool submit=false; //boolean value to inidicate the submit button has been pressed
    bool final_score=false; //boolean value to indicate that the final score should be shown for the first team
    bool final_score_2=false; //boolean value to indicate that the final score should be shown for the second team
    bool team2_ongoing=false; //boolean value to indicate the second part of the game is ongoing
    bool winning_screen=false; //boolean value to inidicate the winning screen is open
    bool leave_info=false; //boolean value to indiciate to leave the info screen 
    Player TeamOne[11]; //array of all the chosen players for the first team
    Player TeamTwo[11]; //array of all the chosen players for the second team
    int final_chem=0; //final team chemistry for the first team
    int final_chem_2=0; //final team chemistry for the second team
    int selected[11]; //array of the index of the selected player 
    int team_rating=0; //final team rating for the first team
    int team_rating_2=0; //final team rating for the second team 
    int volume = MIX_MAX_VOLUME / 2; //music volume
    int VolSliderAdd=100; //initial volume slider position
    int score=0; //displayed score in the playing screen for the first team
    int score_2=0; //displayed score in the playing screen for the second team
    int play_cheer=0; //variable to make sure the cheering sound effect plays only once
    bool isRunning = true; //needed for the main loops condition  
    SDL_Event event; 
    srand(time(NULL));

    //SDL initiation
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }
    
    //window creation
    SDL_Window *window = SDL_CreateWindow("Football Drafts simulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL) {
        printf("Failed to create window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    //rederer creation
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    //intiating the image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image initialization failed: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    //initilising text
    TTF_Init();
    //initiating music and sound. Also opening audio channel
    Mix_Init(MIX_INIT_MP3);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    //initialising font and color
    TTF_Font *font=TTF_OpenFont("Russia Five.ttf",25);
    SDL_Color color={255,255,255};

    //define maximum number of players
    int max_players = 500;
    Player* players = malloc(max_players * sizeof(Player));
    if (players == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    //read players details from file
    int numofplayers = readPlayers(players, max_players);
    if (numofplayers == 0) {
        printf("No players found in the file.\n");
        free(players);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    //load select sound affect
    Mix_Chunk* SelectSound = Mix_LoadWAV("audio/select.mp3");
    if (!SelectSound) {
        printf("Failed to load select sound: %s\n", Mix_GetError());
    }

    //load cheering sound affect
    Mix_Chunk* CheeringSound = Mix_LoadWAV("audio/cheering.mp3");
    if (!CheeringSound) {
        printf("Failed to load cheering sound: %s\n", Mix_GetError());
    }

    //load music 
    Mix_Music* main_music = Mix_LoadMUS("audio/waka_waka.mp3");
    if (!main_music) {
        printf("Failed to load music track: %s\n", Mix_GetError());
    }

    //loading the main menu
    SDL_Surface *main_menu_surface= IMG_Load("images/Main_Menu.png");
    SDL_Texture *main_menu_texture= SDL_CreateTextureFromSurface(renderer,main_menu_surface);
    SDL_FreeSurface(main_menu_surface);

    //loading the esc menu
    SDL_Surface *menu_surface= IMG_Load("images/Menu.png");
    SDL_Texture *menu_texture= SDL_CreateTextureFromSurface(renderer,menu_surface);
    SDL_FreeSurface(menu_surface);

    //loading the info screen
    SDL_Surface *info_surface= IMG_Load("images/info.png");
    SDL_Texture *info_texture= SDL_CreateTextureFromSurface(renderer,info_surface);
    SDL_FreeSurface(info_surface);

    //loading the submit button
    SDL_Surface *SubmitBtn_sur= IMG_Load("images/BtnSubmit.png");
    SDL_Texture *SubmitBtn_tex= SDL_CreateTextureFromSurface(renderer,SubmitBtn_sur);
    SDL_FreeSurface(SubmitBtn_sur);

    //loading the reference 
    SDL_Surface *ref_sur= IMG_Load("images/reference.png");
    SDL_Texture *ref_tex= SDL_CreateTextureFromSurface(renderer,ref_sur);
    SDL_FreeSurface(ref_sur);

    //loading the slider 
    SDL_Surface *slide_sur= IMG_Load("images/slider.png");
    SDL_Texture *slide_tex= SDL_CreateTextureFromSurface(renderer,slide_sur);
    SDL_FreeSurface(slide_sur);

    //loading the winning screen 
    SDL_Surface *win_sur= IMG_Load("images/winning_screen.png");
    SDL_Texture *win_tex= SDL_CreateTextureFromSurface(renderer,win_sur);
    SDL_FreeSurface(win_sur);

    //getting the options to display

    Player* displayGK = options(players, numofplayers,displayGK,"GK");
    display_error(displayGK,renderer,window);

    Player* displayLCB = options(players, numofplayers,displayLCB,"LCB");
    display_error(displayLCB,renderer,window);

    Player* displayRCB = options(players, numofplayers,displayRCB,"RCB");
    display_error(displayRCB,renderer,window);

    Player* displayLB = options(players, numofplayers,displayLB,"LB");
    display_error(displayLB,renderer,window);

    Player* displayRB = options(players, numofplayers,displayRB,"RB");
    display_error(displayRB,renderer,window);

    Player* displayCDM = options(players, numofplayers,displayCDM,"CDM");
    display_error(displayCDM,renderer,window);

    Player* displayLCM = options(players, numofplayers,displayLCM,"LCM");
    display_error(displayLCM,renderer,window);

    Player* displayRCM = options(players, numofplayers,displayRCM,"RCM");
    display_error(displayRCM,renderer,window);

    Player* displayST = options(players, numofplayers,displayST,"ST");
    display_error(displayST,renderer,window);

    Player* displayLW = options(players, numofplayers,displayLW,"LW");
    display_error(displayLW,renderer,window);

    Player* displayRW = options(players, numofplayers,displayRW,"RW");
    display_error(displayRW,renderer,window);
    
    //Load the image and create the texture from surface for the player options
    char *filename_gk[3]; 
    SDL_Texture *cardTexture_gk[3];
    create_options(filename_gk,displayGK,renderer,window,cardTexture_gk);

    char *filename_lcb[3]; 
    SDL_Texture *cardTexture_lcb[3];
    create_options(filename_lcb,displayLCB,renderer,window,cardTexture_lcb);
 
    char *filename_rcb[3]; 
    SDL_Texture *cardTexture_rcb[3];
    create_options(filename_rcb,displayRCB,renderer,window,cardTexture_rcb);

    char *filename_lb[3]; 
    SDL_Texture *cardTexture_lb[3];
    create_options(filename_lb,displayLB,renderer,window,cardTexture_lb);

    char *filename_rb[3]; 
    SDL_Texture *cardTexture_rb[3];
    create_options(filename_rb,displayRB,renderer,window,cardTexture_rb);

    char *filename_cdm[3]; 
    SDL_Texture *cardTexture_cdm[3];
    create_options(filename_cdm,displayCDM,renderer,window,cardTexture_cdm);

    char *filename_lcm[3]; 
    SDL_Texture *cardTexture_lcm[3];
    create_options(filename_lcm,displayLCM,renderer,window,cardTexture_lcm);

    char *filename_rcm[3]; 
    SDL_Texture *cardTexture_rcm[3];
    create_options(filename_rcm,displayRCM,renderer,window,cardTexture_rcm);

    char *filename_st[3]; 
    SDL_Texture *cardTexture_st[3];
    create_options(filename_st,displayST,renderer,window,cardTexture_st);

    char *filename_lw[3]; 
    SDL_Texture *cardTexture_lw[3];
    create_options(filename_lw,displayLW,renderer,window,cardTexture_lw);

    char *filename_rw[3]; 
    SDL_Texture *cardTexture_rw[3];
    create_options(filename_rw,displayRW,renderer,window,cardTexture_rw);

    //loading in the options to display team 2
    Player* displayGK2 = options(players, numofplayers,displayGK2,"GK");
    display_error(displayGK2,renderer,window);

    Player* displayLCB2 = options(players, numofplayers,displayLCB2,"LCB");
    display_error(displayLCB2,renderer,window);

    Player* displayRCB2 = options(players, numofplayers,displayRCB2,"RCB");
    display_error(displayRCB2,renderer,window);

    Player* displayLB2 = options(players, numofplayers,displayLB2,"LB");
    display_error(displayLB2,renderer,window);

    Player* displayRB2 = options(players, numofplayers,displayRB2,"RB");
    display_error(displayRB2,renderer,window);

    Player* displayCDM2 = options(players, numofplayers,displayCDM2,"CDM");
    display_error(displayCDM2,renderer,window);

    Player* displayLCM2 = options(players, numofplayers,displayLCM2,"LCM");
    display_error(displayLCM2,renderer,window);

    Player* displayRCM2 = options(players, numofplayers,displayRCM2,"RCM");
    display_error(displayRCM2,renderer,window);

    Player* displayST2 = options(players, numofplayers,displayST2,"ST");
    display_error(displayST2,renderer,window);

    Player* displayLW2 = options(players, numofplayers,displayLW2,"LW");
    display_error(displayLW2,renderer,window);

    Player* displayRW2 = options(players, numofplayers,displayRW2,"RW");
    display_error(displayRW2,renderer,window);
    
    //Load the image and create the texture from surface for the player options
    char *filename_gk2[3]; 
    SDL_Texture *cardTexture_gk2[3];
    create_options(filename_gk2,displayGK2,renderer,window,cardTexture_gk2);

    char *filename_lcb2[3]; 
    SDL_Texture *cardTexture_lcb2[3];
    create_options(filename_lcb2,displayLCB2,renderer,window,cardTexture_lcb2);
 
    char *filename_rcb2[3]; 
    SDL_Texture *cardTexture_rcb2[3];
    create_options(filename_rcb2,displayRCB2,renderer,window,cardTexture_rcb2);

    char *filename_lb2[3]; 
    SDL_Texture *cardTexture_lb2[3];
    create_options(filename_lb2,displayLB2,renderer,window,cardTexture_lb2);

    char *filename_rb2[3]; 
    SDL_Texture *cardTexture_rb2[3];
    create_options(filename_rb2,displayRB2,renderer,window,cardTexture_rb2);

    char *filename_cdm2[3]; 
    SDL_Texture *cardTexture_cdm2[3];
    create_options(filename_cdm2,displayCDM2,renderer,window,cardTexture_cdm2);

    char *filename_lcm2[3]; 
    SDL_Texture *cardTexture_lcm2[3];
    create_options(filename_lcm2,displayLCM2,renderer,window,cardTexture_lcm2);

    char *filename_rcm2[3]; 
    SDL_Texture *cardTexture_rcm2[3];
    create_options(filename_rcm2,displayRCM2,renderer,window,cardTexture_rcm2);

    char *filename_st2[3]; 
    SDL_Texture *cardTexture_st2[3];
    create_options(filename_st2,displayST2,renderer,window,cardTexture_st2);

    char *filename_lw2[3]; 
    SDL_Texture *cardTexture_lw2[3];
    create_options(filename_lw2,displayLW2,renderer,window,cardTexture_lw2);

    char *filename_rw2[3]; 
    SDL_Texture *cardTexture_rw2[3];
    create_options(filename_rw2,displayRW2,renderer,window,cardTexture_rw2);

    // Play music
    Mix_PlayMusic(main_music, -1);  

	while (isRunning)
	{    
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            // Handles OS "Exit" event
            case SDL_QUIT:
                isRunning = false;
                break;

            //accessing the esc menu
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) { //checks if the esc button on the keyboard is pressed
                    open_EscMenu = true; //opens the esc menu
                }
                break;

            //allowing the user to click buttons and cards 
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    //stores the coordinates of where the mouse is clicked
                    int mouseX = event.motion.x; 
                    int mouseY = event.motion.y;

                    //these buttons should be able to be pressed only in the main menu
                    if (leave_MainMenu==false  && start==false){ 
                        if (mouseX > 550 && mouseX < 950){
                            if (mouseY > 350 && mouseY < 500){ //next button
                                Mix_PlayChannel(-1, SelectSound, 0);
                                leave_MainMenu = true;
                                SDL_DestroyTexture(main_menu_texture);
                            }
                            else if (mouseY > 550 && mouseY < 700){ //quit button
                                Mix_PlayChannel(-1, SelectSound, 0);
                                printf("program was closed from main menu");
                                SDL_DestroyWindow(window);
                                SDL_Quit();
                                return 1;
                            }
                        }
                    }else if(leave_MainMenu=true && leave_info==false){
                        //buttons should be able to be pressed only in the info page
                        if (mouseX > 650 && mouseX < 850){
                            if (mouseY > 690 && mouseY < 770){ //start button
                                Mix_PlayChannel(-1, SelectSound, 0);
                                start = true;
                                SDL_DestroyTexture(info_texture);
                                leave_info=true;
                            }
                        }
                    }

                    //main playing screen, the  buttons and cards can only be clicked if this condiiton is true
                    if (start==true && open_EscMenu== false ){  
                        if (completed[0] == false){ //checks if the GK position has been selected or not 
                            if (team2_ongoing==false){ //checks if the selection for team 2 started
                                //assigns the selected card's index into its respected spot in the selected array
                                selected[0]=select(mouseX, mouseY, filename_gk, cardTexture_gk, &completed[0], SelectSound);
                                TeamOne[0]=displayGK[selected[0]]; //stores the selected player into the first team's final array
                                team_rating= total_rating(displayGK,selected[0],team_rating); //calculates and stores  the first teams current rating
                            }else if (team2_ongoing==true){ //checks if the selection for team 2 started
                                selected[0]=select(mouseX, mouseY, filename_gk2, cardTexture_gk2, &completed[0], SelectSound);
                                TeamTwo[0]=displayGK2[selected[0]]; //stores the selected player into the second team's final array
                                team_rating_2= total_rating(displayGK2,selected[0],team_rating_2);//calculates and stores the second teams current rating
                            } 
                        }
                        else if (completed[1] == false){//checks if the LCB position has been selected or not 
                            if (team2_ongoing==false){
                                selected[1]=select(mouseX, mouseY, filename_lcb, cardTexture_lcb, &completed[1], SelectSound);
                                TeamOne[1]=displayLCB[selected[1]];
                                team_rating= total_rating(displayLCB,selected[1],team_rating);
                            }else if (team2_ongoing==true){
                                selected[1]=select(mouseX, mouseY, filename_lcb2, cardTexture_lcb2, &completed[1], SelectSound);
                                TeamTwo[1]=displayLCB2[selected[1]];    
                                team_rating_2= total_rating(displayLCB2,selected[1],team_rating_2);
                            } 
                        }
                        else if (completed[2] == false){//checks if the RCB position has been selected or not 
                            if (team2_ongoing==false){
                                selected[2]=select(mouseX, mouseY, filename_rcb, cardTexture_rcb, &completed[2], SelectSound);
                                TeamOne[2]=displayRCB[selected[2]];
                                team_rating= total_rating(displayRCB,selected[2],team_rating);
                            }else if (team2_ongoing==true){
                                selected[2]=select(mouseX, mouseY, filename_rcb2, cardTexture_rcb2, &completed[2], SelectSound);
                                TeamTwo[2]=displayRCB2[selected[2]];    
                                team_rating_2= total_rating(displayRCB2,selected[2],team_rating_2);
                            } 
                        }
                        else if (completed[3] == false){//checks if the LB position has been selected or not 
                            if (team2_ongoing==false){
                                selected[3]=select(mouseX, mouseY, filename_lb, cardTexture_lb, &completed[3], SelectSound);
                                TeamOne[3]=displayLB[selected[3]];
                                team_rating= total_rating(displayLB,selected[3],team_rating);
                            }else if (team2_ongoing==true){
                                selected[3]=select(mouseX, mouseY, filename_lb2, cardTexture_lb2, &completed[3], SelectSound);
                                TeamTwo[3]=displayLB2[selected[3]];    
                                team_rating_2= total_rating(displayLB2,selected[3],team_rating_2);
                            } 
                        }
                        else if (completed[4] == false){//checks if the RB position has been selected or not 
                            if (team2_ongoing==false){
                                selected[4]=select(mouseX, mouseY, filename_rb, cardTexture_rb, &completed[4], SelectSound);
                                TeamOne[4]=displayRB[selected[4]];
                                team_rating= total_rating(displayRB,selected[4],team_rating);
                            }else if (team2_ongoing==true){
                                selected[4]=select(mouseX, mouseY, filename_rb2, cardTexture_rb2, &completed[4], SelectSound);
                                TeamTwo[4]=displayRB2[selected[4]];    
                                team_rating_2= total_rating(displayRB2,selected[4],team_rating_2);
                            } 
                        }
                        else if (completed[5] == false){//checks if the CDM position has been selected or not 
                            if (team2_ongoing==false){
                                selected[5]=select(mouseX, mouseY, filename_cdm, cardTexture_cdm, &completed[5], SelectSound);
                                TeamOne[5]=displayCDM[selected[5]];
                                team_rating= total_rating(displayCDM,selected[5],team_rating);
                            }else if (team2_ongoing==true){
                                selected[5]=select(mouseX, mouseY, filename_cdm2, cardTexture_cdm2, &completed[5], SelectSound);
                                TeamTwo[5]=displayCDM2[selected[5]];    
                                team_rating_2= total_rating(displayCDM2,selected[5],team_rating_2);
                            } 
                        }
                        else if (completed[6] == false){//checks if the LCM position has been selected or not 
                            if (team2_ongoing==false){
                                selected[6]=select(mouseX, mouseY, filename_lcm, cardTexture_lcm, &completed[6], SelectSound);
                                TeamOne[6]=displayLCM[selected[6]];
                                team_rating= total_rating(displayLCM,selected[6],team_rating);
                            }else if (team2_ongoing==true){
                                selected[6]=select(mouseX, mouseY, filename_lcm2, cardTexture_lcm2, &completed[6], SelectSound);
                                TeamTwo[6]=displayLCM2[selected[6]];    
                                team_rating_2= total_rating(displayLCM2,selected[6],team_rating_2);
                            } 
                        }
                        else if (completed[7] == false){//checks if the RCM position has been selected or not 
                            if (team2_ongoing==false){
                                selected[7]=select(mouseX, mouseY, filename_rcm, cardTexture_rcm, &completed[7], SelectSound);
                                TeamOne[7]=displayRCM[selected[7]];
                                team_rating= total_rating(displayRCM,selected[7],team_rating);
                            }else if (team2_ongoing==true){
                                selected[7]=select(mouseX, mouseY, filename_rcm2, cardTexture_rcm2, &completed[7], SelectSound);
                                TeamTwo[7]=displayRCM2[selected[7]];    
                                team_rating_2= total_rating(displayRCM2,selected[7],team_rating_2);
                            } 
                        }
                        else if (completed[8] == false){//checks if the ST position has been selected or not 
                            if (team2_ongoing==false){
                                selected[8]=select(mouseX, mouseY, filename_st, cardTexture_st, &completed[8], SelectSound);
                                TeamOne[8]=displayST[selected[8]];
                                team_rating= total_rating(displayST,selected[8],team_rating);
                            }else if (team2_ongoing==true){
                                selected[8]=select(mouseX, mouseY, filename_st2, cardTexture_st2, &completed[8], SelectSound);
                                TeamTwo[8]=displayST2[selected[8]];    
                                team_rating_2= total_rating(displayST2,selected[8],team_rating_2);
                            } 
                        }
                        else if (completed[9] == false){//checks if the LW position has been selected or not 
                            if (team2_ongoing==false){
                                selected[9]=select(mouseX, mouseY, filename_lw, cardTexture_lw, &completed[9], SelectSound);
                                TeamOne[9]=displayLW[selected[9]];
                                team_rating= total_rating(displayLW,selected[9],team_rating);
                            }else if (team2_ongoing==true){
                                selected[9]=select(mouseX, mouseY, filename_lw2, cardTexture_lw2, &completed[9], SelectSound);
                                TeamTwo[9]=displayLW2[selected[9]];    
                                team_rating_2= total_rating(displayLW2,selected[9],team_rating_2);
                            } 
                        }
                        else if (completed[10] == false){//checks if the RW position has been selected or not 
                            if (team2_ongoing==false){
                                selected[10]=select(mouseX, mouseY, filename_rw, cardTexture_rw, &completed[10], SelectSound);
                                TeamOne[10]=displayRW[selected[10]];
                                team_rating= total_rating(displayRW,selected[10],team_rating);
                                final_chem= total_chemisty(TeamOne); //calculates the first teams chemisty
                            }else if (team2_ongoing==true){
                                selected[10]=select(mouseX, mouseY, filename_rw2, cardTexture_rw2, &completed[10], SelectSound);
                                TeamTwo[10]=displayRW2[selected[10]];    
                                team_rating_2= total_rating(displayRW2,selected[10],team_rating_2);
                                final_chem_2= total_chemisty(TeamTwo); //calculates the second teams chemisty
                            } 
                        }
                    }
                    
                    if (open_EscMenu==true){ //checks if the user wants to open the esc menu
                        //enabling the user to change the value
                        if (mouseX >= 650 && mouseX <= 850 && mouseY >= 280 && mouseY <= 300) {
                            // Calculate new volume based on mouse position
                            volume = (mouseX - 650) * MIX_MAX_VOLUME / 200;
                            Mix_VolumeMusic(volume);
                            VolSliderAdd=(mouseX-650);
                            
                        }

                        if (mouseX > 550 && mouseX < 950){//enabling the user to click on the start or quit button
                            if (mouseY > 350 && mouseY < 500){ //start button
                                Mix_PlayChannel(-1, SelectSound, 0);
                                open_EscMenu = false;
                            }
                            else if (mouseY > 550 && mouseY < 700){ //quit button
                                Mix_PlayChannel(-1, SelectSound, 0);
                                printf("program was closed from the esc menu");
                                SDL_DestroyWindow(window);
                                SDL_Quit();
                                return 1;
                            }
                        }
                    }

                    if (winning_screen==true){ //checks if the winning screen should be opened
                        if (mouseX > 550 && mouseX < 950 && mouseY > 550 && mouseY < 700){ //quit button
                            Mix_PlayChannel(-1, SelectSound, 0);
                            printf("program was closed from the winning screen");
                            SDL_DestroyWindow(window);
                            SDL_Quit();
                            return 1;
                        }   
                    }
                    
                    if (submit==true){ //checks if the submit button is open
                        if (mouseX > 1275 && mouseX < 1475 && mouseY > 650 && mouseY < 750){ //submit button             
                            Mix_PlayChannel(-1, SelectSound, 0);
                            if (team2_ongoing==false){ //checks if team 2 has started
                                for (int x=0;x<11;x++){ //resets the completed and selected array for team 2
                                    completed[x] = false;
                                    selected[x]=-1;
                                }
                                team2_ongoing=true; //allows team 2 to start selection
                                submit=false; //turns the submit button off
                            }else{ //checks if team 2 has already been selected
                                winning_screen=true; //open the winning screen 
                                submit=false; //turns off the button
                            }
                        }
                    }
                }
                break;
            }
        }

       //make the background black and clears the render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        if (start==true && open_EscMenu== false){ //checks if the game started and if the esc menu is not open
            drawField(renderer); //draws the football field

            SDL_Rect refRect = {1275, 150, 200, 475}; 
            SDL_RenderCopy(renderer, ref_tex, NULL, &refRect); //renders the team positions reference

            load_text(renderer,font,color,"Current Score:",1255,50,35,30); //renders the text onto the screen
            if (team2_ongoing==false){ //checks if team 2 started
                load_text(renderer,font,color,"Team 1",700,5,15,15); //loads the team 1 text onto the screen
                if (final_score==false){ //checks if the final score is ready
                    score=team_rating * 0.1; //makes the score more presentable
                    load_score(renderer,font,color,score); //renders the score
                }else{
                    score=(team_rating+final_chem) * 0.1; //adds the chemistry if the final score is ready
                    load_score(renderer,font,color,score); //renders the score
                }
            }else if (team2_ongoing==true){
                load_text(renderer,font,color,"Team 2",700,5,15,15);//loads the team 2 text onto the screen
                if (final_score_2==false){
                    score_2=team_rating_2 * 0.1;
                    load_score(renderer,font,color,score_2);
                }else{
                    score_2=(team_rating_2+final_chem_2) * 0.1;
                    load_score(renderer,font,color,score_2);
                }
            }
            
            //load the selected card to its new position
            if (completed[0]==true){ //checks if the player was selected
                if (team2_ongoing==false){ //decides which team is ongoing
                    selected_card(705,625,selected[0],cardTexture_gk,renderer); //renders the selected card onto its new position
                }else if(team2_ongoing==true){
                    selected_card(705,625,selected[0],cardTexture_gk2,renderer);
                }
                
                if  (completed[1]==true){
                    if (team2_ongoing==false){
                        selected_card(550,500,selected[1],cardTexture_lcb,renderer);
                    }else if(team2_ongoing==true){
                        selected_card(550,500,selected[1],cardTexture_lcb2,renderer);
                    }

                    if  (completed[2]==true){
                        if (team2_ongoing==false){
                            selected_card(850,500,selected[2],cardTexture_rcb,renderer);
                        }else if(team2_ongoing==true){
                            selected_card(850,500,selected[2],cardTexture_rcb2,renderer);
                        }

                        if  (completed[3]==true){
                            if (team2_ongoing==false){
                                selected_card(300,450,selected[3],cardTexture_lb,renderer);
                            }else if(team2_ongoing==true){
                                selected_card(300,450,selected[3],cardTexture_lb2,renderer);
                            }

                            if  (completed[4]==true){
                                if (team2_ongoing==false){
                                    selected_card(1100,450,selected[4],cardTexture_rb,renderer);
                                }else if(team2_ongoing==true){
                                    selected_card(1100,450,selected[4],cardTexture_rb2,renderer);
                                }

                                if  (completed[5]==true){
                                    if (team2_ongoing==false){
                                        selected_card(705,350,selected[5],cardTexture_cdm,renderer);
                                    }else if(team2_ongoing==true){
                                        selected_card(705,350,selected[5],cardTexture_cdm2,renderer);
                                    }
                                        
                                    if  (completed[6]==true){
                                        if (team2_ongoing==false){
                                            selected_card(475,300,selected[6],cardTexture_lcm,renderer);
                                        }else if(team2_ongoing==true){
                                            selected_card(475,300,selected[6],cardTexture_lcm2,renderer);
                                        }   

                                        if  (completed[7]==true){
                                            if (team2_ongoing==false){
                                                selected_card(935,300,selected[7],cardTexture_rcm,renderer);
                                            }else if(team2_ongoing==true){
                                                selected_card(935,300,selected[7],cardTexture_rcm2,renderer);
                                            }   

                                            if  (completed[8]==true){
                                                if (team2_ongoing==false){
                                                    selected_card(705,50,selected[8],cardTexture_st,renderer);
                                                }else if(team2_ongoing==true){
                                                    selected_card(705,50,selected[8],cardTexture_st2,renderer);
                                                }   

                                                if  (completed[9]==true){
                                                    if (team2_ongoing==false){
                                                        selected_card(300,100,selected[9],cardTexture_lw,renderer);
                                                    }else if(team2_ongoing==true){
                                                        selected_card(300,100,selected[9],cardTexture_lw2,renderer);
                                                    }  

                                                    if  (completed[10]==true){
                                                        if (team2_ongoing==false){
                                                            selected_card(1100,100,selected[10],cardTexture_rw,renderer);
                                                        }else if(team2_ongoing==true){
                                                            selected_card(1100,100,selected[10],cardTexture_rw2,renderer);
                                                        } 
                                                        
                                                        final_score=true; //lets us know that the final score is ready
                                                        submit=true; //opens the submit button
                                                    } 
                                                } 
                                            } 
                                        } 
                                    } 
                                } 
                            } 
                        } 
                    } 
                } 
            }

            //once the previous card is stored in its new position, the next batch of cards will be displayed
            int textX=10; //sets the text's X coordinate
            int textY=50; //sets the text's Y coordinate
            int textW=10; //add to the text's width
            int textH=10; //add to the text's height

            if (completed[0]==false){ //checks if the GK card is not selected
                load_text(renderer,font,color,"Select your GK",textX,textY,textW,textH); //renders the text
                if (team2_ongoing==false){ //check which team is currently being selected
                    display_options(renderer,cardTexture_gk); //display the options for the user to select from
                }else if(team2_ongoing==true){
                    display_options(renderer,cardTexture_gk2);
                }
            }else if (completed[1]==false){ //checks if the LCB card is not selected
                load_text(renderer,font,color,"Select your LCB",textX,textY,textW,textH);
                if (team2_ongoing==false){
                    display_options(renderer,cardTexture_lcb);
                }else if(team2_ongoing==true){
                    display_options(renderer,cardTexture_lcb2);
                }

            }else if (completed[2]==false){ //checks if the RCB card is not selected
                load_text(renderer,font,color,"Select your RCB",textX,textY,textW,textH);
                if (team2_ongoing==false){
                    display_options(renderer,cardTexture_rcb);
                }else if(team2_ongoing==true){
                    display_options(renderer,cardTexture_rcb2);
                }
            }else if (completed[3]==false){ //checks if the LB card is not selected
                load_text(renderer,font,color,"Select your LB",textX,textY,textW,textH);
                if (team2_ongoing==false){
                    display_options(renderer,cardTexture_lb);
                }else if(team2_ongoing==true){
                    display_options(renderer,cardTexture_lb2);
                }

            }else if (completed[4]==false){ //checks if the RB card is not selected
                load_text(renderer,font,color,"Select your RB",textX,textY,textW,textH);
                if (team2_ongoing==false){
                    display_options(renderer,cardTexture_rb);
                }else if(team2_ongoing==true){
                    display_options(renderer,cardTexture_rb2);
                }

            }else if (completed[5]==false){ //checks if the CDM card is not selected
                load_text(renderer,font,color,"Select your CDM",textX,textY,textW,textH);
                if (team2_ongoing==false){   
                    display_options(renderer,cardTexture_cdm);
                }else if(team2_ongoing==true){
                    display_options(renderer,cardTexture_cdm2);
                }

            }else if (completed[6]==false){ //checks if the LCM card is not selected
                load_text(renderer,font,color,"Select your LCM",textX,textY,textW,textH);
                if (team2_ongoing==false){
                    display_options(renderer,cardTexture_lcm);
                }else if(team2_ongoing==true){
                    display_options(renderer,cardTexture_lcm2);
                }

            }else if (completed[7]==false){ //checks if the RCM card is not selected
                load_text(renderer,font,color,"Select your RCM",textX,textY,textW,textH);
                if (team2_ongoing==false){
                    display_options(renderer,cardTexture_rcm);
                }else if(team2_ongoing==true){
                    display_options(renderer,cardTexture_rcm2);
                }

            }else if (completed[8]==false){ //checks if the ST card is not selected
                load_text(renderer,font,color,"Select your ST",textX,textY,textW,textH);
                if (team2_ongoing==false){
                    display_options(renderer,cardTexture_st);
                }else if(team2_ongoing==true){
                    display_options(renderer,cardTexture_st2);
                }

            }else if (completed[9]==false){ //checks if the LW card is not selected
                load_text(renderer,font,color,"Select your LW",textX,textY,textW,textH);
                if (team2_ongoing==false){    
                    display_options(renderer,cardTexture_lw);
                }else if(team2_ongoing==true){
                    display_options(renderer,cardTexture_lw2);
                }

            }else if (completed[10]==false){ //checks if the RW card is not selected
                load_text(renderer,font,color,"Select your RW",textX,textY,textW,textH);
                if (team2_ongoing==false){
                    display_options(renderer,cardTexture_rw);
                }else if(team2_ongoing==true){
                    display_options(renderer,cardTexture_rw2);
                }

            }
        }
        
        //rendering the menu image
        if (open_EscMenu==true) {
            SDL_Rect menurect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}; 
            SDL_RenderCopy(renderer, menu_texture, NULL, &menurect);

            /*positions the slider image, with the width to depend on the users prefrence. 
              But initially set at half*/
            SDL_Rect SliderRect = {651, 282, (VolSliderAdd-2), 16}; 
            SDL_RenderCopy(renderer, slide_tex, NULL, &SliderRect);//renders the slider image
            load_volume(renderer, font, color, volume); //renders the volume percentage
        }

        //rendering the submit button
        if (submit==true && open_EscMenu== false){
            SDL_Rect SubmitBtnRect = {1275, 650, 200, 100}; 
            SDL_RenderCopy(renderer, SubmitBtn_tex, NULL, &SubmitBtnRect);
        }
        //rendering the main menu
        if (leave_MainMenu==false){ //checks if the main menu is open
        SDL_Rect mainrect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}; 
        SDL_RenderCopy(renderer, main_menu_texture, NULL, &mainrect);
        }else if(leave_MainMenu==true){ //checks if the main menu is closed
            SDL_Rect inforect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}; 
            SDL_RenderCopy(renderer, info_texture, NULL, &inforect); //renders the info page once 
        }

        if (winning_screen==true){ //checks if the winning screen should be opened
            SDL_Rect winrect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}; 
            SDL_RenderCopy(renderer, win_tex, NULL, &winrect); //renders the winning screen

            if (score>score_2){ //checks if team 1 score is bigger than team 2
                while (play_cheer<1){ //allows the sound to play once 
                    Mix_Volume(-1, MIX_MAX_VOLUME / 2); //sets the sound effects volume to half
                    Mix_PlayChannel(-1, CheeringSound, 0); //plays the cheering sound effect
                    play_cheer++; //increments play_cheer so that it exits the loop
                }
                //renders the winner in text
                load_text(renderer,font,color,"Team 1 won!!",575,100,200,100); 
            }else if (score<score_2){ //checks if team 2 score is bigger than team 1
                while (play_cheer<1){
                    Mix_Volume(-1, MIX_MAX_VOLUME / 2);
                    Mix_PlayChannel(-1, CheeringSound, 0);
                    play_cheer++;
                }
                load_text(renderer,font,color,"Team 2 won!!",575,100,200,100);
            }else{ //if its a tie
                // renders a text mentioning its a tie
                load_text(renderer,font,color,"It was a tie!!",575,100,200,100);
            }
        }
        SDL_RenderPresent(renderer);
        
    }
    
    //freeing, destroying and quiting arrays,textures, render, SDL, etc...
    Mix_FreeChunk(SelectSound);
    Mix_FreeMusic(main_music);
    Mix_CloseAudio();
    free(players);
    free_and_destroy(displayGK,filename_gk,cardTexture_gk);
    free_and_destroy(displayLCB,filename_lcb,cardTexture_lcb);
    free_and_destroy(displayRCB,filename_rcb,cardTexture_rcb);
    free_and_destroy(displayLB,filename_lb,cardTexture_lb);
    free_and_destroy(displayRB,filename_rb,cardTexture_rb);
    free_and_destroy(displayCDM,filename_cdm,cardTexture_cdm);
    free_and_destroy(displayLCM,filename_lcm,cardTexture_lcm);
    free_and_destroy(displayRCM,filename_rcm,cardTexture_rcm);
    free_and_destroy(displayST,filename_st,cardTexture_st);
    free_and_destroy(displayLW,filename_lw,cardTexture_lw);
    free_and_destroy(displayRW,filename_rw,cardTexture_rw);
    free_and_destroy(displayGK2,filename_gk2,cardTexture_gk2);
    free_and_destroy(displayLCB2,filename_lcb2,cardTexture_lcb2);
    free_and_destroy(displayRCB2,filename_rcb2,cardTexture_rcb2);
    free_and_destroy(displayLB2,filename_lb2,cardTexture_lb2);
    free_and_destroy(displayRB2,filename_rb2,cardTexture_rb2);
    free_and_destroy(displayCDM2,filename_cdm2,cardTexture_cdm2);
    free_and_destroy(displayLCM2,filename_lcm2,cardTexture_lcm2);
    free_and_destroy(displayRCM2,filename_rcm2,cardTexture_rcm2);
    free_and_destroy(displayST2,filename_st2,cardTexture_st2);
    free_and_destroy(displayLW2,filename_lw2,cardTexture_lw2);
    free_and_destroy(displayRW2,filename_rw2,cardTexture_rw2);
    SDL_DestroyTexture(main_menu_texture);
    SDL_DestroyTexture(menu_texture);
    SDL_DestroyTexture(info_texture);
    SDL_DestroyTexture(slide_tex);
    SDL_DestroyTexture(SubmitBtn_tex);
    SDL_DestroyTexture(ref_tex);
    SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
	SDL_Quit();

    return 0;
}