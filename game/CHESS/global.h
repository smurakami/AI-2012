/*#include "typedef.h"*/

int     board[12][12];		/* work board */
int     board0[12][12];		/* last state */
int     board00[12][12] = {	/* initial state */
    ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB,
    ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB,
    ID_OB, ID_OB, IDR_B, IDN_B, IDB_B, IDQ_B, IDK_B, IDB_B, IDN_B, IDR_B, ID_OB, ID_OB,
    ID_OB, ID_OB, IDP_B, IDP_B, IDP_B, IDP_B, IDP_B, IDP_B, IDP_B, IDP_B, ID_OB, ID_OB,
    ID_OB, ID_OB, ID_SP, ID_SP, ID_SP, ID_SP, ID_SP, ID_SP, ID_SP, ID_SP, ID_OB, ID_OB,
    ID_OB, ID_OB, ID_SP, ID_SP, ID_SP, ID_SP, ID_SP, ID_SP, ID_SP, ID_SP, ID_OB, ID_OB,
    ID_OB, ID_OB, ID_SP, ID_SP, ID_SP, ID_SP, ID_SP, ID_SP, ID_SP, ID_SP, ID_OB, ID_OB,
    ID_OB, ID_OB, ID_SP, ID_SP, ID_SP, ID_SP, ID_SP, ID_SP, ID_SP, ID_SP, ID_OB, ID_OB,
    ID_OB, ID_OB, IDP_W, IDP_W, IDP_W, IDP_W, IDP_W, IDP_W, IDP_W, IDP_W, ID_OB, ID_OB,
    ID_OB, ID_OB, IDR_W, IDN_W, IDB_W, IDQ_W, IDK_W, IDB_W, IDN_W, IDR_W, ID_OB, ID_OB,
    ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB,
    ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB, ID_OB,
};
int     num_move;		/* no. of move */
int     man_white;		/* side of man */
int     level;			/* level */
int     max_depth;		/* max depth */
int     think_depth;		/* think depth */
int     level_depth;		/* level depth */
int     mask;			/* display thinking */
int     g_end;			/* end mark */
long    man_time,
        com_time,
        com0time;		/* comsumption time */
PMOVE pmv[MAX_DEPTH], pmv0[MAX_DEPTH];/* try move */
MOVE rmv[256];			/* recorded move */
