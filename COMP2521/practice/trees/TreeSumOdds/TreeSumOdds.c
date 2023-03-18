
#include <stdlib.h>

#include "tree.h"

int TreeSumOdds(Tree t) {
	if(t == NULL) return 0;
	return TreeSumOdds(t -> left) + TreeSumOdds(t -> right)
+ ((t -> value & 1) == 1 ? t -> value : 0);
}

