/* 
 * File:   hashgen.h
 * Author: Matt
 *
 * Created on den 24 november 2013, 21:02
 */

#ifndef HASHGEN_H
#define	HASHGEN_H

#ifdef	__cplusplus
extern "C" {
#endif

// returns a hash identifier for the string passed to it
uint32_t hash32_gen(char *key, size_t len);


#ifdef	__cplusplus
}
#endif

#endif	/* HASHGEN_H */

