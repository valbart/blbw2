#ifndef __STRATEGY_H
#define __STRATEGY_H

#include "common.h"
#include "bidiarray.h"
#include "movement.h"

class Strategy {

private:
    //! array containing all blobs on the board
    bidiarray<Sint16> _blobs;
    //! an array of booleans indicating for each cell whether it is a hole or not.
    const bidiarray<bool>& _holes;
    //! Current player
    Uint16 _current_player;


    //! Call this function to save your best move.
    //! Multiple call can be done each turn,
    //! Only the last move saved will be used.
    void (*_saveBestMove)(movement&);


public:

    Sint32 scoreActuel;

    // Valuation des cases pour évaluer la force d'une position
    // C'est une matrice constante qui associe simplement une valeur à chaque position
    bidiarray<Sint32> score;

    //Variable pour fixer la profondeur d'arbre minmax qu'on veut explorer
    int profondeurExplo;

    // Variable pour stocker le numéro du joueur qui joue effectivement :
    // lors de la simulation des coups des différents joueurs, on sauve un nouveau
    // "meilleur coup" trouvé ssi le numéro du joueurs est égale à cette variable.
    Uint16 playerToMove;

        // Constructor from a current situation
        // Constructeur donné dans le squelette de code, modifié pour instancier
        // les variables scoreActuel, profondeurExplo et score
        // en particulier il prend une nouveau paramêtre : la matrice de score vide
        // a nécessité la modification de launchStrategy.cc pour initialiser cette variable
    Strategy (bidiarray<Sint16>& blobs,
              const bidiarray<bool>& holes,
              const Uint16 current_player,
              void (*saveBestMove)(movement&),
			        bidiarray <Sint32> score)
            : _blobs(blobs),_holes(holes), _current_player(current_player), _saveBestMove(saveBestMove),
			        scoreActuel(0x80000000), score(score), profondeurExplo(3), playerToMove(current_player)
            {
              //scoreActuel = 0x80000000;
              // Initialisation de la matrice score :
              // on initialise seulement un premier quart 4x4
              // le reste est copié par symétrie.
              score.set(0,0,500);
              score.set(1,0,-150);
              score.set(2,0,30);
              score.set(3,0,10);
              score.set(0,1,-150);
              score.set(1,1,-250);
              score.set(2,1,0);
              score.set(3,1,0);
              score.set(0,2,30);
              score.set(1,2,0);
              score.set(2,2,1);
              score.set(3,2,2);
              score.set(0,3,10);
              score.set(1,3,0);
              score.set(2,3,2);
              score.set(3,3,16);
              int i = 1;
              for (int y = 7; y > 3; --y) {
                for (int x = 0; x < 4; ++x) {
                  score.set(x,y,score.get(x, 4-i));
                }
                ++i;
              }
              i = 1;
              for (int y = 0; y < 8; ++y) {
                for (int x = 4; x < 8; ++x) {
                  score.set(x,y,score.get(y, 4-i));
                }
                ++i;
              }
            }


    //Copy constructor
    Strategy(Strategy & St)
          : _blobs(St._blobs), _holes(St._holes),_current_player(St._current_player)
          {}

    // Constructeur pour branchement :
    // Lorsqu'on veux simuler un nouveau coup, c'est ce constructeur qui est appelé
    // Il prend en paramêtre le score actuel et la profondeur d'exploration qui doit
    // diminuer après chaque nouveau coup.
    // De plus, il est toujours appelé avec 1 - _current_player : on change de joueur
    // à chaque nouveau coup.
    Strategy (bidiarray<Sint16>& blobs,
                const bidiarray<bool>& holes,
                const Uint16 current_player,
                void (*saveBestMove)(movement&),
                const movement & move_todo, // le movement correspondant à celui pour lequel on a créeer la strategie
                const bidiarray<Sint32> & scoreMat,
                Sint32 scoreActuel,
                Sint8 profondeurExplo,
                Uint16 playerToMove)
              : _blobs(blobs),_holes(holes), _current_player(current_player), _saveBestMove(saveBestMove),
                scoreActuel(scoreActuel), score(scoreMat), profondeurExplo(profondeurExplo), playerToMove(playerToMove)
              {
                applyMove(move_todo); // on joue tout de suite ce movement
                                      // pour bien avoir la strategy modifié aprés création
              }

        // Destructor
    ~Strategy() {}

        /**
         * Apply a move to the current state of blobs
         * Assumes that the move is valid
         */
    void applyMove (const movement& mv);

    void setScoreActuel();

    Sint32 getScoreActuel();

        /**
         * Compute the vector containing every possible moves
         */
    vector<movement>& computeValidMoves (vector<movement>& valid_moves) const;

        /**
         * Estimate the score of the current state of the game
         */
    Sint32 estimateCurrentScore () const;

        /**
         * Find the best move.
         */
    void computeBestMove ();


};

#endif
