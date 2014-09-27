/* 
 * File:   paras.h
 * Students : 
 * Fengmin Deng     (dengf, 659332)
 * Jiajie Li        (jiajiel, 631482)
 * Shuangchao Yin   (shuangchaoy, 612511) 
 */

#ifndef PARAS_H
#define	PARAS_H

#define DELTA_T         "day"
#define STEPS           3650
#define SIZE            500
#define POP_DENSITY     0.2
#define PROB_M          0.4978 // probability that a human is a male
#define MOVE            0.1
#define DEATH_M         0.0000343 // probability that a male dies per time step
#define DEATH_F         0.0000325 // probability that a female dies per time step
#define BIRTH           0.00042 // probability that a pair reproduce per time step
#define BORN_M          0.5136 // probability that a baby is born a male
#define INIT_Z_QTY      2 // initial number of zombies
#define DEATH_Z         0.0714 // probability that a zombie dies at a time step
#define INFECT_EARLY    0.7// probability of zombiefication in the first year
#define INFECT_LATER    0.0001// probability of zombiefication afterwards

#define DMGP_CURVES     3 // number of curves of demographics

#endif	/* PARAS_H */