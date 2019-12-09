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
