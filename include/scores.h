#ifndef SCORES_H
#define SCORES_H

typedef struct {
  int points;
  int coins;
} Scores;

int track_score(int touched_portal, int points);

#endif 
