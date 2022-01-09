# pokerdeck

This project includes sevaral simple utilities related to poker hands.

* The `src` directory contains sources related to modeling a poker game and evaluating hands.
* The `prog` directory contains sources for specific console utilities (output to `bin`).
* The `include` directory contains headers needed for compilation.


## playround

Just a simple utility to deal random poker boards.

Sample output:

```
Hero:
	2s Qc
Villain:
	7s 2c
Flop:
	9d 6c 3h
Turn:
	9d 6c 3h 3s
River:
	9d 6c 3h 3s Qs
Result:
      Hero: Two Pair [QQ339]
   Villain: Pair [33Q97]
    Winner: Hero
```


## tophands

Runs heads-up simulations and computes the top starting hands based on win percentages.

Sample output:

```
Top starting hands for 1000000 simulations:
  1) [AA ]: 85.2%
  2) [KK ]: 81.5%
  3) [QQ ]: 79.6%
  4) [JJ ]: 76.8%
  5) [TT ]: 74.1%
  6) [99 ]: 71.0%
  7) [88 ]: 67.8%
  8) [AKs]: 66.9%
  9) [AQs]: 65.5%
 10) [AKo]: 65.4%
 11) [AJs]: 64.7%
 12) [AQo]: 64.5%
 13) [77 ]: 64.3%
 14) [AJo]: 63.5%
 15) [ATs]: 63.1%
 16) [KQs]: 62.9%
 17) [ATo]: 62.3%
 18) [A9s]: 61.9%
 19) [KJs]: 61.6%
 20) [KQo]: 61.4%
...
150) [53s]: 35.5%
151) [74o]: 35.3%
152) [54o]: 34.6%
153) [52s]: 34.2%
154) [83o]: 34.2%
155) [62s]: 34.0%
156) [72s]: 33.8%
157) [64o]: 33.8%
158) [82o]: 33.5%
159) [73o]: 32.9%
160) [32s]: 32.4%
161) [42s]: 32.3%
162) [53o]: 31.7%
163) [63o]: 31.6%
164) [72o]: 31.1%
165) [43o]: 31.0%
166) [62o]: 29.9%
167) [52o]: 29.9%
168) [42o]: 29.3%
169) [32o]: 27.5%
```


## showdown

Runs heads-up simulations and computes showdown statistics.

Sample output:

```
            hand ( freq.):   wins (  win%); losses ( loss%);  chops ( chop%);  total
     Royal Flush ( 0.01%):     64 ( 97.0%);      0 (  0.0%);      2 (  3.0%);     66
  Straight Flush ( 0.04%):    358 ( 93.5%);      3 (  0.8%);     22 (  5.7%);    383
  Four of a Kind ( 0.33%):   3064 ( 91.7%);    179 (  5.4%);     98 (  2.9%);   3341
      Full House ( 5.08%):  44587 ( 87.8%);   2797 (  5.5%);   3386 (  6.7%);  50770
           Flush ( 6.19%):  52433 ( 84.7%);   7301 ( 11.8%);   2200 (  3.6%);  61934
        Straight ( 9.18%):  74061 ( 80.7%);   6496 (  7.1%);  11220 ( 12.2%);  91777
 Three of a Kind ( 9.69%):  70502 ( 72.7%);  22995 ( 23.7%);   3444 (  3.6%);  96941
        Two Pair (46.82%): 301351 ( 64.4%); 143166 ( 30.6%);  23652 (  5.1%); 468169
            Pair (87.81%): 358091 ( 40.8%); 491042 ( 55.9%);  28964 (  3.3%); 878097
       High card (34.85%):  54564 ( 15.7%); 285096 ( 81.8%);   8862 (  2.5%); 348522
```
