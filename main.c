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
/*
 * @brief Affiche le tableau vu par le joueur durant le niveau à l'écran
 * @param aucun
 * @return aucun
 */
void afficheTabVue(void){
    lcd_effaceAffichage();//efface toute donnée à l'écran
    for(int i = 0; i < NB_LIGNE; i++){//affichage de chaque ligne
        lcd_gotoXY(1, i+1);//place le curseur à l'emplacement prévu pour la ligne à afficher
        lcd_putMessage(m_tabVue[i]);//affiche la ligne
    }
}

/*
 * @brief Affiche le tableau contenant la solution du niveau à l'écran
 * @param aucun
 * @return aucun
 */
void afficheTabMine(void){
    lcd_effaceAffichage();//efface toute donnée à l'écran
    for(int i = 0; i < NB_LIGNE; i++){//affichage de chaque ligne
        lcd_gotoXY(1, i+1);//place le curseur à l'emplacement prévu pour la ligne à afficher
        lcd_putMessage(m_tabMines[i]);//affiche la ligne
    }
}

/*
 * @brief Rempli le tableau m_tabVue avec le caractère spécial (définie en CGRAM
 *  du LCD) TUILE. Met un '\0' à la fin de chaque ligne pour faciliter affichage
 *  avec lcd_putMessage().
 * @param rien
 * @return rien
 */
void initTabVue(void){
    for(int i = 0; i<NB_LIGNE; i++){//boucle pour les lignes
        for(int j = 0; j<NB_COL; j++){//boucle pour les colonnes
            m_tabVue[i][j] = TUILE;//remplace le caractere dans la ligne et la colonne spécifiéepar un espace
        }
        m_tabVue[i][NB_COL] = '\0';//met une fin de ligne à la fin de la ligne
    }
}
 
/*
 * @brief Rempli le tableau m_tabMines d'un nombre (nb) de mines au hasard.
 *  Les cases vides contiendront le code ascii d'un espace et les cases avec
 *  mine contiendront le caractère MINE défini en CGRAM.
 * @param int nb, le nombre de mines à mettre dans le tableau 
 * @return rien
 */
void rempliMines(int nb){
    char col = 0;//variable contenant temporairement les coordonés de la mine générée
    char ligne = 0;
    
    for(int i = 0; i < NB_COL; i++){//remplis le tableau d'espaces
        for(int j = 0; j < NB_LIGNE; j++){
            m_tabMines[j][i] = 32;
        }
    }
    
    for(int i = 0; i < nb; i++){//pour le nombre de mines à générer
        do{
           col = rand()%NB_COL;//génere les coordonés de la nouvelle mine
           ligne = rand()%NB_LIGNE;
        }while(m_tabMines[ligne][col] != 32);//recommence la génération si une mine se trouvait déjà à cet emplacement
        m_tabMines[ligne][col] = MINE;//met la mine dans la matrice de mines à ses coordonés
    }
}
 
/*
 * @brief Rempli le tableau m_tabMines avec le nombre de mines que touche la case.
 * Si une case touche à 3 mines, alors la méthode place le code ascii de 3 dans
 * le tableau. Si la case ne touche à aucune mine, la méthode met le code
 * ascii d'un espace.
 * Cette méthode utilise calculToucheCombien(). 
 * @param rien
 * @return rien
 */
void metToucheCombien(void){
    for(int i = 0; i<NB_COL; i++){//boucle pour les lignes
        for(int j = 0; j<NB_LIGNE; j++){//boucle pour les colonnes
            if(m_tabMines[j][i] != MINE){//passe les cases qui contiennent une mine pour ne pas l'effacer
                m_tabMines[j][i] = calculToucheCombien(j, i);//calcule combien de mines se trouvent dans un rayon de 1 autour de la case
            }
        }
    }
}
 
/*
 * @brief Calcul à combien de mines touche la case. Cette méthode est appelée par metToucheCombien()
 * @param int ligne, int colonne La position dans le tableau m_tabMines a vérifier
 * @return char nombre. Le nombre de mines touchées par la case
 */
char calculToucheCombien(int ligne, int colonne){
    int x = 0;//variable contenant temporairement les coordonés de la case à vérifier
    int y = 0;
    char total = 0;//total de mines dans le rayon

    for(int i = -1; i < 2; i++){//rayon en x autour de la case
        for(int j = -1; j < 2; j++){//rayon en y autour de la case
            if(j != 0 || i != 0){//évite de vérifier la case centrale
                x = colonne + i;//mise à jour des coordonés à vérifier pour créer la zone
                y = ligne + j;
                if(x >= 0 && x < NB_COL && y >= 0 && y < NB_LIGNE){//si la case se trouve à l'intérieur de la zone de jeu
                    if(m_tabMines[y][x] == MINE){//si la case contient une mine, incrémente le total de 1
                        total++;
                    }
                }
            }
        }
    }
    if(total == 0){//retourne la valeur ASCII d'espace si aucune mine est à proximité
        return 32;
    }
    return total+48;//retourne la valeur ASCII correspondant au nombre de mines à proximité
}
 
/**
 * @brief Si la manette est vers la droite ou la gauche, on déplace le curseur 
 * d'une position (gauche, droite, bas et haut)
 * @param char* x, char* y Les positions X et y  sur l'afficheur
 * @return rien
 */
void deplace(char* px, char* py){
    int aX = getAnalog(AXE_X);//lecture des ports analogiques reliés aux joystics
    int aY = getAnalog(AXE_Y);
    
    if(aX < 100){//joystick vers la gauche
        (*px)--;//décrémente les coordonés du curseur en x
        if(*px <= 0){//vérifie si il est sorti de la zone de jeu si oui, retourne le curseur à l'opposé de l'écran
            *px = NB_COL;
        }
        lcd_gotoXY((*px), (*py));//mise à jour de la position du curseur
    }else if(aX > 150){//joystick vers la droite
        (*px)++;//incrémente les coordonés du curseur en x
        if(*px > NB_COL){//vérifie si il est sorti de la zone de jeu si oui, retourne le curseur à l'opposé de l'écran
            *px = 1;
        }
        lcd_gotoXY((*px), (*py));//mise à jour de la position du curseur
    }else if(aY < 100){//joystick vers le haut
        (*py)++;//incrémente les coordonés du curseur en y
        if(*py > NB_LIGNE){//vérifie si il est sorti de la zone de jeu si oui, retourne le curseur à l'opposé de l'écran
            *py = 1;
        }
        lcd_gotoXY((*px), (*py));//mise à jour de la position du curseur
    }else if(aY > 150){//joystick vers le bas
        (*py)--;//décrémente les coordonés du curseur en y
        if(*py <= 0){//vérifie si il est sorti de la zone de jeu si oui, retourne le curseur à l'opposé de l'écran
            *py = NB_LIGNE;
        }
        lcd_gotoXY((*px), (*py));//mise à jour de la position du curseur
    }
    
    
}
 
/*
 * @brief Dévoile une tuile (case) de m_tabVue. 
 * S'il y a une mine, retourne Faux. Sinon remplace la case et les cases autour
 * par ce qu'il y a derrière les tuiles (m_tabMines).
 * Utilise enleveTuileAutour().
 * @param char x, char y Les positions X et y sur l'afficheur LCD
 * @return faux s'il y avait une mine, vrai sinon
 */
bool demine(char x, char y){
    
    while(PORTBbits.RB1 == 0);//debounce du bouton
    
    x--;//translation des coordonés pour etre compatibles avec la matrice de mine et non ave l'écran
    y--;
    
    if(m_tabMines[y][x] == MINE){//si le joueur démine sur une mine
        lcd_gotoXY(x+1, y+1);// met le curseur à l'endroir correspondant à la mine
        return false;//indique au programme que le joueur a perdu
    }
    if(m_tabMines[y][x] == 32){//si joueur démine sur une case vide sans mine autour
        enleveTuilesAutour(x, y);//dégage les 8 cases autour de la case du curseur
    }else if(m_tabMines[y][x] >= 48){//si la case est à proximité d'une mine
        m_tabVue[y][x] = m_tabMines[y][x];//affiche seulement la case jouée
    }
    afficheTabVue();//mise à jour de l'écran
    lcd_gotoXY(x+1, y+1);//mise à jour des coordonés du curseur
    return true;//indique au programme que le déminage est réussi
}
 
/*
 * @brief Dévoile les cases non minées autour de la tuile reçue en paramètre.
 * Cette méthode est appelée par demine().
 * @param char x, char y Les positions X et y sur l'afficheur LCD.
 * @return rien
 */
void enleveTuilesAutour(char x, char y){
    
    char colonne = 0;//variable contenant temporairement les coordonés de la case à dégager
    char ligne = 0;
    
    for(int i = -1; i < 2; i++){//boucle en X
		for(int j = -1; j < 2; j++){//boucle en Y
			colonne = x + i;//mise à jour des coordonés à vérifier pour créer la zone
			ligne = y + j;
			m_tabVue[ligne][colonne] = m_tabMines[ligne][colonne];//met à jour les cases à afficher pour montrer les cases du tableau de mines
		}
    }
}
 
/*
 * @brief Vérifie si gagné. On a gagné quand le nombre de tuiles non dévoilées
 * est égal au nombre de mines. On augmente de 1 le nombre de mines si on a 
 * gagné.
 * @param int* pMines. Le nombre de mine.
 * @return vrai si gagné, faux sinon
 */
bool gagne(int* pMines){
    char ttl = 0;//total des cases non révélés
    for(int i = 0; i<NB_LIGNE; i++){//boucle pour les lignes
        for(int j = 0; j<NB_COL; j++){//boucle pour les colonnes
            if(m_tabVue[i][j] == TUILE){//incrémente le total si la sace est une tuile
                ttl++;
            }
        }
    }
    if(ttl == (*pMines)){//si le nombre de tuiles restantes correspond au nombre de mines cachés, je joueur a gagné et le nombre de mines pour la prochaine partie augmente de 1
        (*pMines)++;
        return true;
    }
    return false;
}

/*
 * @brief Lit le port analogique. 
 * @param Le no du port à lire
 * @return La valeur des 8 bits de poids forts du port analogique
 */
char getAnalog(char canal)
{ 
    ADCON0bits.CHS = canal;
    __delay_us(1);  
    ADCON0bits.GO_DONE = 1;  //lance une conversion
    while (ADCON0bits.GO_DONE == 1) //attend fin de la conversion
        ;
    return  ADRESH; //retourne seulement les 8 MSB. On laisse tomber les 2 LSB de ADRESL
}

/**
 * @brief Fait l'initialisation des différents regesitres du PIC
 * @param Aucun
 * @return Aucun
 */
void initialisation(void)
{  
    
    TRISD = 0; //Tout le port D en sortie
 
    ANSELH = 0;  // RB0 à RB4 en mode digital. Sur 18F45K20 AN et PortB sont sur les memes broches
    TRISB = 0xFF; //tout le port B en entree
 
    ANSEL = 0;  // PORTA en mode digital. Sur 18F45K20 AN et PortA sont sur les memes broches
    TRISA = 0; //tout le port A en sortie
 
    //Pour du vrai hasard, on doit rajouter ces lignes. 
    //Ne fonctionne pas en mode simulateur.
    T1CONbits.TMR1ON = 1;
    srand(TMR1);
 
   //Configuration du port analogique
    ANSELbits.ANS7 = 1;  //A7 en mode analogique
 
    ADCON0bits.ADON = 1; //Convertisseur AN à on
	ADCON1 = 0; //Vref+ = VDD et Vref- = VSS
 
    ADCON2bits.ADFM = 0; //Alignement à gauche des 10bits de la conversion (8 MSB dans ADRESH, 2 LSB à gauche dans ADRESL)
    ADCON2bits.ACQT = 0;//7; //20 TAD (on laisse le max de temps au Chold du convertisseur AN pour se charger)
    ADCON2bits.ADCS = 0;//6; //Fosc/64 (Fréquence pour la conversion la plus longue possible)   
}