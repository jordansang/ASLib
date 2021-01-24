#ifndef ASL_BITOPE_H
#define ASL_BITOPE_H

#define BIT_IS_SET(V,F)     ((V) & (F))
#define SET_BIT(V,F)        (V) = (V) | (F)
#define UNSET_BIT(V,F)      (V) = (V) & ~(F)

#endif /* ASL_BITOPE_H */
