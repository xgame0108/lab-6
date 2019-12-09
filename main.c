/**
 * @file main.c  
 * @author Élian Dubuc
 * @date 5 Dec 2019
 * @brief  
 *Programme de fonctionnement du jeu "Démineur". Le joueur déplace le curseur sur une matrice de dalles
  * lorsque le joueur appuie sur le bouton, le programme révele au joueur ce qui se trouve sous la dalle que pointe le curseur.
  * Trois choses sont possibles: rien, un nombre ou une mine. Rien signifie que rien ne se trouve sous la dalle et qu'aucune mine n'est à proximité.
  * Quand une case vide est trouvée, toutes les cases à proximité se découvrent.
  * Un nombre signifie le nombre de mines qui sont à proximité immédiate de la case. Finalement une mine signifie qu'une mine se trouvait
  * sous la dalle et que la partie est maintenant terminée. Les mines sont placés au hasard au début de la partie. Le joueur a la possibilité de déplacer le curseur dans toutes les diretions et
  * lorsque le curseur atteint une limite, il est déplacé à l'autre extrémité de l'écran pour continuer son mouvement. Pour gagner, le joueur doit
  * découvrir toutes les cases qui ne sont pas minés. Une fois tout découvert, le jeu est mis à zéro et une mine additionelle est ajoutée et la partie recommence.
 * @version 1.0
 * Environnement:
 *     Développement: MPLAB X IDE (version 5.05)
 *     Compilateur: XC8 (version 2.00)
 *     Matériel: Carte démo du Pickit3. PIC 18F45K20
  */

/****************** Liste des INCLUDES ****************************************/
#include <xc.h>
#include <stdbool.h>  // pour l'utilisation du type bool
#include <conio.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include "Lcd4Lignes.h"


/********************** CONSTANTES *******************************************/
#define _XTAL_FREQ 1000000 //Constante utilisée par __delay_ms(x). Doit = fréq interne du uC

#define NB_LIGNE 4  //afficheur LCD 4x20
#define NB_COL 20
#define AXE_X 7  //canal analogique de l'axe x
#define AXE_Y 6
#define PORT_SW PORTBbits.RB1 //sw de la manette
#define TUILE 2 //caractère cgram d'une tuile
#define MINE 3 //caractère cgram d'une mine

/********************** PROTOTYPES *******************************************/
void initialisation(void);
void initTabVue(void);
void rempliMines(int nb);
void metToucheCombien(void);
char calculToucheCombien(int ligne, int colonne);
void deplace(char* x, char* y);
bool demine(char x, char y);
void enleveTuilesAutour(char x, char y);
bool gagne(int* pMines);
void afficheTabVue(void);
void afficheTabMine(void);
char getAnalog(char canal);


/****************** VARIABLES GLOBALES ****************************************/
 char m_tabVue[NB_LIGNE][NB_COL+1]; //Tableau des caractères affichés au LCD
 char m_tabMines[NB_LIGNE][NB_COL+1];//Tableau contenant les mines, les espaces et les chiffres

/*               ***** PROGRAMME PRINCPAL *****                             */
void main(void)
{
    char x = 1;//coordonés cartésiennes du curseur du joueur sur l'écran
    char y = 1;
    int nbMines = 4;//nombre de mines du premier niveau
    
    initialisation();//initialise les registres internes du PIC
    lcd_init();//initialisation de l'affichage LCD
    //lcd_cacheCurseur();//cache le curseur pour une meilleure expérience visuelle
    lcd_effaceAffichage();//efface l'affichage pour faire place au nouveau message
    lcd_curseurHome();//met le curseur à zéro
    
    initTabVue();//initialise le tableau vu par le joueur
    rempliMines(nbMines);//initialise le tableau caché contenant les mines
    metToucheCombien();//calcule pour chaque cases combien de mines sont à proximité et met ce nombre dans la case
    afficheTabVue();//affiche au joueur le tableau contenant les dalles
    
    while(1) //boucle principale
    {
        deplace(&x, &y);//déplace le curseur selon l'action du joueur
        if(PORTBbits.RB1 == 0){//si le boutton est appuyé
            if(!demine(x, y) || gagne(&nbMines)){//affiche ce qui se trouve sous la case et si des vases non minés sont restantes.
                //dans le cas ou une mine se trouvait sous la case ou si il ne reste plus de cases minés:
                afficheTabMine();//affiche la solution au niveau
                while(!PORTBbits.RB1 == 0);//attends une entrée utilisateur
                initTabVue();//réinitialise le tableau vu par le joueur
                rempliMines(nbMines);// réinitialise le tableau caché contenant les mines
                metToucheCombien();//recalcule pour chaque cases combien de mines sont à proximité et met ce nombre dans la case
                afficheTabVue();//affiche au joueur le tableau contenant les dalles
                lcd_gotoXY(x, y);//remet le curseur à la dernière position connue par le joueur
            }
        }
        __delay_ms(100);
    }
}