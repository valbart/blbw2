#include "strategy.h"
// Setter et getter
void Strategy::setScoreActuel() {
  scoreActuel = estimateCurrentScore();
}

Sint32 Strategy::getScoreActuel() {
  return scoreActuel;
}

// On joue le movement mv en fonction des règles du jeu,
// cette fonction devrait être bonne.
// juste un point : on joue comme si on était l'autre joueur ici :
// en effet, cette fonction est appellée par mon constructeur pour branchement :
// c'est donc le fils du noeud actuel qui simulera le coup à jouer
// il doit donc se comporter comme s'il était son père donc comme s'il était
// 1 - _current_player.
void Strategy::applyMove (const movement& mv) {
  if (((mv.ox - mv.nx)*(mv.ox - mv.nx) <= 1) && ((mv.oy - mv.ny)*(mv.oy - mv.ny) <= 1)) {
    // Mouvement d'une case on crée un blob
    _blobs.set(mv.nx, mv.ny, 1 - _current_player);
  } else {
    // Sinon on saute : déplacement du blob
    _blobs.set(mv.ox, mv.oy, -1);
    _blobs.set(mv.nx, mv.ny, 1 -_current_player);
  }
  // On change les blobs de camps
  for(Sint8 i = -1 ; i < 2 ; i++) {
    for(Sint8 j = -1 ; j < 2 ; j++) {
      if (mv.nx+i < 0) continue;
      if (mv.nx+i > 7) continue;
      if (mv.ny+j < 0) continue;
      if (mv.ny+j > 7) continue;
      if ((_blobs.get(mv.nx+i, mv.ny+j) != -1) && (_blobs.get(mv.nx+i, mv.ny+j) != 1 - _current_player)) {
        _blobs.set(mv.nx+i, mv.ny+j, 1 - _current_player);
      }
    }
  }
}

Sint32 Strategy::estimateCurrentScore () const {
    Sint32 currentScore = 0;
    for(int x = 0 ; x < 8 ; ++x) {
        for(int y = 0; y < 8 ; ++y) {
            if (_blobs.get(x, y) == (int) _current_player) {
              currentScore += 10000; // Score pour le fait de posséder un blob
              currentScore += score.get(x,y); // Score pour la position du blob
            }
        }
    }
    vector<movement> v;
    computeValidMoves(v);
    currentScore += v.size(); // Score pour la mobilité
    return currentScore;
}


// On calcul simplement tous les mouvements possibles pour le jouer :
// On parcourt tout le plateau, dés qu'on trouve un blob de current_player
// on calcul tout ses coups possibles et on les ajoute au vecteur
vector<movement>& Strategy::computeValidMoves (vector<movement>& valid_moves) const {
  for(int oldx = 0 ; oldx < 8 ; ++oldx) {
      for(int oldy = 0; oldy < 8 ; ++oldy) {
          //cout << _blobs.get(oldx, oldy) << "\n" << endl;
          if (_blobs.get(oldx, oldy) == (int) _current_player) {
            //  cout << "Blob en : " << oldx << "," << oldy << " au joueur " << _current_player << "\n" << endl;              //iterate on possible destinations
              for(int newx = std::max(0,oldx-2) ; newx <= std::min(7,oldx+2) ; newx++) {
                  for(int newy = std::max(0,oldy-2) ; newy <= std::min(7, oldy+2) ; newy++) {
                      if (_holes.get(newx, newy)) continue;
                      if (_blobs.get(newx, newy) == -1) {
                        valid_moves.push_back(movement(oldx,oldy,newx,newy));
                      };
                  }
              }
          }
      }
  }
    return valid_moves;
}

/*void Strategy::computeBestMove() {

}*/



// C'est la fonction de parcourt d'arbre :
void Strategy::computeBestMove () {
    //cout << "JOUEUR : " << _current_player << "\n" << endl;
    // To be improved...
    // On part avec le plus mauvais score possible.
    Sint32 scoreMax = 0x80000000;
    if (profondeurExplo <= 0) {
      // On est au fond de l'arbre : on calcul la valeur du noeud
      setScoreActuel();
    } else {
      // Sinon on calcul tous les coups possibles
      vector<movement> mvt_ok;
      movement best_move;
      computeValidMoves(mvt_ok);
      for (vector<movement>::iterator it = mvt_ok.begin(); it != mvt_ok.end(); ++it) {
        Strategy s(_blobs, _holes, 1 - _current_player, _saveBestMove, *it, score, scoreActuel, profondeurExplo - 1);
        s.computeBestMove();
        scoreActuel = - s.getScoreActuel();
        if (scoreActuel >= scoreMax) {
          scoreMax  = scoreActuel;
          //cout << "SCOREMAX : " << scoreActuel << "\n" << endl;
          best_move = *it;
        }
      }
      _saveBestMove(best_move);
    }





    /*
    //The following code find a valid move.
    move mv(0,0,0,0);
    //iterate on starting position
    for(mv.ox = 0 ; mv.ox < 8 ; mv.ox++) {
        for(mv.oy = 0 ; mv.oy < 8 ; mv.oy++) {
            if (_blobs.get(mv.ox, mv.oy) == (int) _current_player) {
                //iterate on possible destinations
                for(mv.nx = std::max(0,mv.ox-2) ; mv.nx <= std::min(7,mv.ox+2) ; mv.nx++) {
                    for(mv.ny = std::max(0,mv.oy-2) ; mv.ny <= std::min(7,mv.oy+2) ; mv.ny++) {
                        if (_holes.get(mv.nx, mv.ny)) continue;
                        if (_blobs.get(mv.nx, mv.ny) == -1) goto end_choice;
                    }
                }
            }
        }
    }

end_choice:
     _saveBestMove(mv);
     return;
*/
}
