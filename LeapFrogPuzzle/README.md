# Leap Frog Puzzle

## Task

The game has `2N + 1` fields of play. In the beginning of the game there are `N` frogs which are looking to the left and placed on the rightmost `N` fields,
and `N` frogs which are looking to the right and placed on the leftmost fields. The aim of the game is to swap the places of the frogs and reach the opposite configuration.

The rules of play are as follows:
* Each frog can only move in the direction it looks at.
* Every frog can jump in a free field in front of it or jump over a frog to go to an empty field in front of it.

## Input:

`N` - number of frogs looking in one direction

## Output:

All the configurations that needed to go through to get from the start to the final state.

## Sample Input:

`2`

## Sample Output:

```
LL_RR

L_LRR

LRL_R

LRLR_

LR_RL

_RLRL

R_LRL

RRL_L

RR_LL
```
