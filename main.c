#include <stdlib.h> // For exit()
#include <stdio.h> // For printf()
#include <math.h> // For sin() and cos()
#include "GfxLib.h" // To do simple graphics
#include "ESLib.h" // For valeurAleatoire()

#define LargeurFenetre 800
#define HauteurFenetre 600

#define NbrMaxTour 2000

#define PopulationSize 100

float min_mutation = 0.0001;
float max_mutation = 0.01;
int turn;
int generation = 1;
char generationString[20];
int key=0;

typedef struct
{
	int x,y;
	float angle;
	float genome[NbrMaxTour];
	int etat; //vivant = 0; mort !=0
	int but;
	float mutation;

}blatt;

blatt populationBlatt[100];


int main(int argc, char *argv[])
{
	initialiseGfx(argc, argv);

	prepareFenetreGraphique(argv[argc-1], LargeurFenetre, HauteurFenetre);
	lanceBoucleEvenements();

	return 0;
}

float initialiseMutation(){
	
	float mutation;
	do{
		mutation = (valeurIntervalleZeroUn()/10);
	}while((mutation<min_mutation)||(mutation>max_mutation));

	return mutation;
}

blatt initialisePop(blatt lablatte){
	lablatte.x=100;
	lablatte.y=550;
	lablatte.angle = 0;
	lablatte.etat=0;
	lablatte.but=2000;
	lablatte.mutation = initialiseMutation();

	for (int i = 0; i < NbrMaxTour; ++i)
	{
		lablatte.genome[i]=((valeurIntervalleZeroUn()*4)-2)/10;
		
	}

	return(lablatte);
}

void initializePopulation(blatt* lapopulace)
{
	int index;
	
	for (index = 0; index < PopulationSize; ++index)
	{
		
		lapopulace[index]=initialisePop(lapopulace[index]);


	}
	
}

blatt child()
{
	//blatt maman = PopulationBlatt[0]   blatt papa = PopulationBlatt[1]
	blatt enfant;
	enfant = initialisePop(enfant);
	float random1, random2;

	for (int i=0; i < abs(populationBlatt[1].etat) ; i++) //just pass to next generation the array until dead
	{
	  	random1=valeurIntervalleZeroUn();
	  	if (random1<0.5)
	  		enfant.genome[i]=populationBlatt[0].genome[i];
	  	else
	  		enfant.genome[i]=populationBlatt[1].genome[i];

	  	random2=valeurIntervalleZeroUn();
	    if (random2<enfant.mutation) //mutations
	    	enfant.genome[i]=((valeurIntervalleZeroUn()*4)-2)/10;
	}
	for (int i= abs(populationBlatt[1].etat)+1; i<NbrMaxTour; i++) //random after dead array positions (can reduce a lot of not evolution time)
	  	enfant.genome[i]=((valeurIntervalleZeroUn()*4)-2)/10;

	return enfant; 
}

blatt checkColision(blatt blatte){
	/*if((blatte.x >= 200  && blatte.y <= 600 && blatte.x <= 250 && blatte.y >= 230 )){
		blatte.etat=-turn;
	}p
	if((blatte.x >= 200  && blatte.y <= 70 && blatte.x <= 250 && blatte.y >= 0 )){
		blatte.etat=-turn;
	}
	if((blatte.x >= 550  && blatte.y <= 600 && blatte.x <= 600 && blatte.y >= 450 )){
		blatte.etat=-turn;
	}
	if((blatte.x >= 550  && blatte.y <= 250 && blatte.x <= 600 && blatte.y >= 0 )){
		blatte.etat=-turn;
	}*/
	if((blatte.x >= 150  && blatte.y <= 320 && blatte.x <= 650 && blatte.y >= 280 )){ //rectangle jaune centrale
		blatte.etat=-turn; //mort sur le rectangle
	}
	if((blatte.x <= 0  || blatte.y <= 0 || blatte.x >= 800 || blatte.y >= 600 )){ //bord de la fenetre
		blatte.etat=-turn; //mort sur le bord
	}
	if((blatte.x <= 720  && blatte.y <= 120 && blatte.x >= 680 && blatte.y >= 80 )){ //carré vert
		blatte.etat=turn; //mort sur l'objectif
	}
	return blatte;
	
}

void TriBlatt(){ //calcul le score de chaque blatt, parametres importants
	//  la distance correspondant au momen le + proche de l'objectif dans sa vie
	//  la distance entre la ou la blatt est morte et l'objectif
	//  le nbr de tour (pas la génération, on parle ici de tour correspondant a chaque mouvement) jusqu'a sa mort
	//  l'endroit ou la course de la blatte se fini (on differencie bien une fin sur l'objectif qu'une mort sur le bord par exemple)
	int final_dist;
	for(int m=0;m<100;m++){
		final_dist=(int)(sqrt((populationBlatt[m].x-700)*(populationBlatt[m].x-700)+(populationBlatt[m].y-100)*(populationBlatt[m].y-100)));
		if(populationBlatt[m].etat>0){
			
			populationBlatt[m].but=(int)(1.5*(2000-final_dist)+1.3*(2000-populationBlatt[m].etat));
		}
		else{
			if(populationBlatt[m].etat!=0){
				populationBlatt[m].but=(int)((2000-final_dist)+0.5*(2000-populationBlatt[m].but)+0.3*(2000+populationBlatt[m].etat));
			}
			else{
				populationBlatt[m].but=(int)((2000-final_dist)+0.5*(2000-populationBlatt[m].but));
			}
		}
	}
	//tri des meilleurs blatts, on passe les 2 meilleurs au debut du tableau de la population
	//la generation suivante se basera sur les 2 meilleurs.
	if(populationBlatt[0].but<populationBlatt[1].but){
		blatt tmpBlatt = populationBlatt[0];
		populationBlatt[0]=populationBlatt[1];
		populationBlatt[1]=tmpBlatt;
	}
	for (int i = 2; i < 100; ++i)
	{
		if(populationBlatt[i].but>populationBlatt[0].but){
			populationBlatt[1]=populationBlatt[0];
			populationBlatt[0]=populationBlatt[i];
		}
		else{
			if(populationBlatt[i].but>populationBlatt[1].but){
				populationBlatt[1]=populationBlatt[i];
			}
		}
	}
}

void calculBut(blatt lablatte){
	//permet de calculer une distance
	int lemodulo;
	lemodulo =(int)(sqrt((lablatte.x-700)*(lablatte.x-700)+(lablatte.y-100)*(lablatte.y-100)));
	if(lemodulo<lablatte.but){
		lablatte.but=lemodulo;
	}

}

blatt calculMouvement(blatt blatte, int tour)
{
	if(blatte.etat == 0){
		blatte.angle = blatte.angle + blatte.genome[tour];
		blatte.x = blatte.x + (int)(2*cos(blatte.angle));
		blatte.y = blatte.y + (int)(2*sin(blatte.angle));
		blatte = checkColision(blatte);
		calculBut(blatte);
	}
	return blatte;
}

blatt* mouvementPopulation(blatt* blatte, int tour)
{
	int index;
  	for(index=0; index < PopulationSize; index++)
  	{
  		blatte[index]=calculMouvement(blatte[index], tour);
  	}
  	return blatte;
}

void drawBlatt(int x, int y){
	epaisseurDeTrait(50);
	couleurCourante(255,0,0);
	point(x,y);
}

void setTerrain(){
	epaisseurDeTrait(120);
	couleurCourante(255,210,0);
	//rectangle(200,600,250,230);
	//rectangle(200,0,250,70);
	//rectangle(550,600,600,450);
	//rectangle(550,0,600,250);
	//couleurCourante(0,0,255);
	rectangle(150,280,650,320);
	couleurCourante(124,252,0);
	point(700,100);
	rectangle(720,80,680,120); 
	epaisseurDeTrait(1);
	couleurCourante(255,255,255);
	afficheChaine(generationString,20,30,30);

}



void verifCondFinGeneration(){
	int compteurDeBlattMorte=0;
	if((turn>2000)){
		TriBlatt();
		//initializePopulation(populationBlatt);
		for(int j =2; j<100;j++){
			populationBlatt[j]=child();
		}
		populationBlatt[0]=populationBlatt[2];
		populationBlatt[1]=populationBlatt[3];
		turn=0;
		generation++;
		sprintf(generationString, "Generation : %d",generation);
		//printf("set key to 0\n");
		key=0;
	}
	else{
		for(int i=0;i<100;i++){
			if(populationBlatt[i].etat!=0){
				compteurDeBlattMorte++;
			}
		}
		if(compteurDeBlattMorte>=100){
			TriBlatt();
			//initializePopulation(populationBlatt);
			for(int l =2; l<100;l++){
				populationBlatt[l]=child();
			}
			populationBlatt[0]=populationBlatt[2];
			populationBlatt[1]=populationBlatt[3];
			turn=0;
			generation++;
			sprintf(generationString, "Generation : %d",generation);
			//printf("set key to 0\n");
			key=0;
		}
	}
}

void gestionEvenement(EvenementGfx event){

	

	switch (event)
	{
		case Initialisation:
			sprintf(generationString, "Generation : %d",generation);
			initializePopulation(populationBlatt);
			
			turn = 0;
			demandeTemporisation(1);
			break;

		case Temporisation:
			if(turn>0)
			{
				mouvementPopulation(populationBlatt, turn);
				turn=turn+1;
			}
			verifCondFinGeneration();

			rafraichisFenetre();
			break;

		case Affichage:
			effaceFenetre(0,0,0);
			setTerrain();
			for (int i = 0; i < PopulationSize; ++i)
			{
				drawBlatt(populationBlatt[i].x,populationBlatt[i].y);
			}
			break;
		case Clavier:
			switch(caractereClavier())
			{
				case 'p':
					//key permet d'eviter de remettre la valeur turn à 1 pendant les mouvements
					//Il faut appuyer sur p pour faire demarrer les generations
					if(key==0){
						turn = 1;
						//printf("set key to 1\n");
						key=1;
					}
					break;

				case 'R':
				case 'r':
					initializePopulation(populationBlatt);
					turn = 0;
					rafraichisFenetre();
					break;

			}
			break;
		case ClavierSpecial:
			break;
		case BoutonSouris:
			break;
		case Souris:
			break;
		case Inactivite:
			break;
		case Redimensionnement:
			break;
	}
}
