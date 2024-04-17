#ifndef SCORES_H
#define SCORES_H

typedef struct {
  int points;
  int coins;
} Scores;

// Assembly functions
int track_score(int touched_portal, int points);
int track_coins(int coin_collected, int coins);

#endif 
