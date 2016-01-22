#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif

#include <SDL.h>
#include <stdio.h>
#include <SDL_image.h>
#include "constantes.h"
#include <string.h>


// Jeu : PONG . Code cr�� par GENDARME Thibaut et REUTER faustine le 08/2015 //
                           // Etat : Incomplet //

typedef struct  //Structure gestion des inputs du clavier et de la souris
{
    char key[SDLK_LAST];
    int mousex,mousey;
    int mousexrel,mouseyrel;
    char mousebuttons[8];
        char quit;
}Input;

typedef struct      // Structure des coordonn�es et du vecteur de d�placement de la balle
{
    int x,y;
    double x_vitesse,y_vitesse;
    double coeff;
}Ball;

//PROTOTYPES
void UpdateEvent(Input* in);
int Blit(SDL_Surface* src,SDL_Surface* dst,int x ,int y);
int Jeu_1v1(SDL_Surface* screen);
void Mvt_ball(Ball *ball,SDL_Surface *balle,SDL_Surface *border_up,SDL_Surface *border_down,SDL_Surface *j1,SDL_Surface *j2,int x_j1,int y_j1,int x_j2,int y_j2);
void SpeedBall(Ball *ball,int pos,int j);
//

int main (int argc, char *argv[])
{
    //Variables du main
    SDL_Surface *screen=NULL, *border_menu_up=NULL, *border_menu_down=NULL,*title=NULL;
    SDL_Event event;
    Input in;
    int menu_refresh=1;
    //

    SDL_Init(SDL_INIT_VIDEO); //Initialiation de la SDL
    SDL_WM_SetCaption("Pong v1.0.0",NULL); //Nom de la fenetre
    screen = SDL_SetVideoMode(LARGEUR_FENETRE, HAUTEUR_FENETRE, 32, SDL_HWSURFACE|SDL_DOUBLEBUF); //Parametre de la fenetre (sortie principale)

    border_menu_up = SDL_CreateRGBSurface(SDL_HWSURFACE,LARGEUR_FENETRE,HAUTEUR_FENETRE/128, 32,0,0,0,0);
    border_menu_down = SDL_CreateRGBSurface(SDL_HWSURFACE,LARGEUR_FENETRE,HAUTEUR_FENETRE/128, 32,0,0,0,0);

    title=IMG_Load("pong.png");

    SDL_FillRect(border_menu_up,NULL,SDL_MapRGB(border_menu_up->format,255,255,255));
    SDL_FillRect(border_menu_down,NULL,SDL_MapRGB(border_menu_down->format,255,255,255));

    //Affichage du menu
    Blit(title,screen,(screen->w /2) - (title->w /2),20);
    Blit(border_menu_up,screen,1,3);
    Blit(border_menu_down,screen,1,HAUTEUR_FENETRE - 9);
    //

    memset(&in,0,sizeof(in)); // Reset du tableau des input du clavier

    while(!in.quit) //Boucle du menu , sortie losque la croix de fermeture d'une fenetre est pr�ss�
    {
        if(menu_refresh==0)
        {
            SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,0,0,0));
            Blit(title,screen,(screen->w /2) - (title->w /2),20);
            Blit(border_menu_up,screen,1,3);
            Blit(border_menu_down,screen,1,HAUTEUR_FENETRE - 9);
        }
        UpdateEvent(&in); //Fonction de mise a jour du tableau qui detecte les touche enfonc� ou non
        if (in.key[SDLK_KP1])
        {
            menu_refresh=0;
            in.key[SDLK_KP1]=0;
            if(Jeu_1v1(screen)==557) // La ft Jeu_1v1() retourne 557 dans un cas pr�cis pour quitter et revenir au menu
            {
                in.quit=1;
            }

        }


        SDL_Flip(screen); //Mise a jour des nouveau blit sur l'ecran
    }

    //Liberation de la m�moire utilis� par les variables graphique
    SDL_FreeSurface(border_menu_up);
    SDL_FreeSurface(border_menu_down);
    SDL_FreeSurface(title);
    SDL_Quit();
    //

    return 0;
}

//FONCTIONS
void UpdateEvent(Input* in)
{
    SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			in->key[event.key.keysym.sym]=1;
			break;
		case SDL_KEYUP:
			in->key[event.key.keysym.sym]=0;
			break;
        case SDL_MOUSEMOTION:
			in->mousex=event.motion.x;
			in->mousey=event.motion.y;
			in->mousexrel=event.motion.xrel;
			in->mouseyrel=event.motion.yrel;
			break;
		case SDL_MOUSEBUTTONDOWN:
			in->mousebuttons[event.button.button]=1;
			break;
		case SDL_MOUSEBUTTONUP:
            if (event.button.button!=SDL_BUTTON_WHEELUP &&
event.button.button!=SDL_BUTTON_WHEELDOWN)
            {
			in->mousebuttons[event.button.button]=0;
            }
			break;
		case SDL_QUIT:
			in->quit = 1;
			break;
		default:
			break;
		}
	}
}

int Blit(SDL_Surface* src,SDL_Surface* dst,int x,int y) //Fonction qui simplifie le blit
{
    SDL_Rect R;
    R.x = x;
    R.y = y;
    R.w = R.h = 0;
    return SDL_BlitSurface(src,NULL,dst,&R);
}
int Jeu_1v1(SDL_Surface* screen) // Fonction du jeu
{
    SDL_Surface *j1=NULL,*j2=NULL,*border_up=NULL,*border_down=NULL,*ball=NULL;
    Input in;
    Ball mvt_ball;





    border_up = SDL_CreateRGBSurface(SDL_HWSURFACE,LARGEUR_FENETRE,HAUTEUR_FENETRE/128, 32,0,0,0,0);
    border_down = SDL_CreateRGBSurface(SDL_HWSURFACE,LARGEUR_FENETRE,HAUTEUR_FENETRE/128, 32,0,0,0,0);
    j1=SDL_CreateRGBSurface(SDL_HWSURFACE,9,100,32,0,0,0,0);
    j2=SDL_CreateRGBSurface(SDL_HWSURFACE,9,100,32,0,0,0,0);

    ball=IMG_Load("ball.bmp");
    SDL_SetColorKey(ball,SDL_SRCCOLORKEY,SDL_MapRGB(ball->format,255,255,255));


    int x_j1=12,y_j1=screen->h /2 - j1->h /2;
    int x_j2=screen->w -(x_j1+j1->w),y_j2=screen->h/2 - j2->h /2;
    mvt_ball.x=screen->w /2 - ball->w /2;
    mvt_ball.y=screen->h /2 - ball->h /2;
    mvt_ball.x_vitesse=-3;
    mvt_ball.y_vitesse=0;
    mvt_ball.coeff=2.00000000/57.000000000;

    SDL_FillRect(border_up,NULL,SDL_MapRGB(border_up->format,255,255,255));
    SDL_FillRect(border_down,NULL,SDL_MapRGB(border_down->format,255,255,255));
    SDL_FillRect(j1,NULL,SDL_MapRGB(j1->format,255,0,0));
    SDL_FillRect(j2,NULL,SDL_MapRGB(j2->format,0,0,255));

    SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,0,0,0));

    Blit(j1,screen,x_j1,y_j1);
    Blit(j2,screen,x_j2,y_j2);
    Blit(border_up,screen,1,6);
    Blit(border_down,screen,1,HAUTEUR_FENETRE - 12);
    Blit(ball,screen,mvt_ball.x,mvt_ball.y);

    memset(&in,0,sizeof(in));

    while(!in.quit)
    {
        UpdateEvent(&in);
        if(in.key[SDLK_a])
        {
            y_j1 = y_j1 + 2;
            if (y_j1+j1->h > HAUTEUR_FENETRE - 12)
            {
                y_j1 = y_j1 - 2;
            }
        }

        if(in.key[SDLK_d])
        {
            y_j1 = y_j1 - 2;
            if (y_j1 < 6+border_up->h)
            {
                y_j1 = y_j1 + 2;
            }
        }

        if(in.key[SDLK_LEFT])
        {
            y_j2 = y_j2 + 2;
            if (y_j2+j2->h > HAUTEUR_FENETRE - 12)
            {
                y_j2 = y_j2 - 2;
            }
        }

        if(in.key[SDLK_RIGHT])
        {
            y_j2 = y_j2 - 2;
            if (y_j2 < 6+border_up->h)
            {
                y_j2 = y_j2 + 2;
            }
        }

        Mvt_ball(&mvt_ball,ball,border_up,border_down,j1,j2,x_j1,y_j1,x_j2,y_j2);

        if(in.quit)
        {
            return 557;

        }

        if(in.key[SDLK_ESCAPE])
        {
            in.quit=1;
        }
        // Reset de lecran (remplissage de noir)
        SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,0,0,0));
        // Affichage  de tous les objets en jeu ( balle , joueurs ...)
        Blit(j1,screen,x_j1,y_j1);
        Blit(j2,screen,x_j2,y_j2);
        Blit(border_up,screen,1,6);
        Blit(border_down,screen,1,HAUTEUR_FENETRE - 12);
        Blit(ball,screen,mvt_ball.x,mvt_ball.y);
        //
        // Mise a jour de l'ecran
        SDL_Flip(screen);
    }
    // Liberation de la memoire
    SDL_FreeSurface(j1);
    SDL_FreeSurface(j2);
    SDL_FreeSurface(border_up);
    SDL_FreeSurface(border_down);
    SDL_FreeSurface(ball);
    //



}
//Fonction de gestion des hitbox (bordure + joueurs)
void Mvt_ball(Ball *ball,SDL_Surface *balle,SDL_Surface *border_up,SDL_Surface *border_down,SDL_Surface *j1,SDL_Surface *j2,int x_j1,int y_j1,int x_j2,int y_j2)
{
    int check_position=0,j=0;

    ball->x = ball->x+ball->x_vitesse;
    ball->y = ball->y+ball->y_vitesse;
    SDL_Delay(3);

    if(ball->x < 21 && ball->y < y_j1+j1->h && ball->y+balle->h > y_j1) // SI LA BALLE TAPE LE JOUEUR 1
    {
        ball->x = ball->x-ball->x_vitesse;
        ball->x_vitesse = ball->x_vitesse*-1;

        if(ball->y+(balle->h/2) < y_j1+(j1->h/2)) // SI LA BALLE TAPE LA PARTIE SUPP DU J1
        {
            check_position = (y_j1+(j1->h/2)) - (ball->y+balle->h/2);
            SpeedBall(ball,check_position,j);
        }

        if(ball->y+(balle->h/2) > y_j1+(j1->h/2)) // SI LA BALLE TAPE LA PARTIE INF DU J1
        {
            check_position = (y_j1+(j1->h/2)) - (ball->y+balle->h/2);
            SpeedBall(ball,check_position,j);
        }

    }

    if(ball->x > LARGEUR_FENETRE-39 && ball->y < y_j2+j2->h && ball->y+balle->h > y_j2) // SI LA BALLE TAPE LE JOUEUR 2
    {
        j=1;
        ball->x = ball->x-ball->x_vitesse;
        ball->x_vitesse = ball->x_vitesse*-1;

        if(ball->y+(balle->h/2) < y_j2+(j2->h/2)) // SI LA BALLE TAPE LA PARTIE SUPP DU J2
        {
            check_position = y_j2+(j2->h/2) - ball->y+balle->h/2;
            SpeedBall(ball,check_position,j);
        }

        if(ball->y+(balle->h/2) > y_j2+(j2->h/2))
        {
            check_position = y_j2+(j2->h/2) - ball->y+balle->h/2;
            SpeedBall(ball,check_position,j);
        }
    }

    if(ball->y < border_up->h+6 || ball->y+balle->h > HAUTEUR_FENETRE - 12)
    {
        ball->y_vitesse = ball->y_vitesse*-1;
    }
}

//Gestion de la vitesse x et y en fonction du rebond sur le joueur
void SpeedBall(Ball *ball,int pos,int j)
{
    //PARTIE HAUTE DU JOUEUR 1
    if(pos>0)
    {
        ball->y_vitesse=0;
        ball->x_vitesse=3;
    }

    if(pos>10)
    {
        ball->y_vitesse=-1;
        ball->x_vitesse=3;
    }

        if(pos>33)
    {
        ball->y_vitesse=-2;
        ball->x_vitesse=2;
    }

        if(pos>46)
    {
        ball->y_vitesse=-3;
        ball->x_vitesse=1;
    }

    //PARTIE BASSE DU J1
    if(pos<0)
    {
        ball->y_vitesse=0;
        ball->x_vitesse=3;
    }

    if(pos<-10)
    {
        ball->y_vitesse=1;
        ball->x_vitesse=3;
    }

        if(pos<-33)
    {
        ball->y_vitesse=2;
        ball->x_vitesse=2;
    }

        if(pos<-46)
    {
        ball->y_vitesse=3;
        ball->x_vitesse=1;
    }

    if (j) // SI LA BALLE REBONDIE SUR LE JOUEUR 2
    {
        ball->x_vitesse =  ball->x_vitesse*-1;
    }

}

















//
