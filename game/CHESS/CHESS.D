#include          <stdio.h>
#include          <time.h>
#include          <curses.h>
#include          <ctype.h>
#include          <signal.h> 
#include	  <sys/file.h>
#include          "constant.h"
#include          "typedef.h"
#include          "function.h"
#include          "global.h"

WINDOW * aho;

WINDOW * pass;

/* following is used in eval print by 'X' */

FILE *fpev;
int evalmask;

extern int  man_white,
            num_move,
            level,
            max_depth,
            level_depth;
extern int  board[12][12],
            board0[12][12];
extern long com_time,
            com0time;
extern  PMOVE pmv0[];

static int  move_n[] = {
    0, 0, 8, 14, 14, 28, 0, 0, 0, 0, 8, 14, 14, 28, 0, 0,
    0, 0, 8, 14, 14, 28, 0, 0, 0, 0, 8, 14, 14, 28, 0, 0,
};

int     com_move () {		/* computer move *//* 0: continue,  1: com
				   win, 2: stalemate, 3:com lose */
            MOVE mv;
    int    *p,
            i,
            move_bw,
            em;
    long    t1;

    time (&com0time);
    disp_prompt (!man_white, COMPUTER, num_move);


    reset2pawn (!man_white);

    b2board (&board[Y_8][X_a], &board0[Y_8][X_a]);

    for (move_bw = 0, p = &board[Y_8][X_a], i = 0; i < 92; i++, p++)
	move_bw += move_n[*p];

    level_depth = level;
    if (move_bw < 14)
	level_depth++;
    if (move_bw < 28)
	level_depth++;
    if (move_bw < 56)
	level_depth++;


    if (man_white) {
	if (max_black (0, -10000, 10000) == -10000)
	    return (3);
    }
    else {
	if (min_white (0, -10000, 10000) == 10000)
	    return (3);
    }
    mv.x_src = (pmv0[0].p_src - &board[0][0]) % 12;
    mv.y_src = (pmv0[0].p_src - &board[0][0]) / 12;
    mv.x_des = (pmv0[0].p_des - &board[0][0]) % 12;
    mv.y_des = (pmv0[0].p_des - &board[0][0]) / 12;
    mv.i_src = board0[mv.y_src][mv.x_src];
    mv.i_des = pmv0[0].i_des;
    if ((mv.i_des & ID_PART) != ID_PAWN)
	mv.i_des |= 8;

    time (&t1);
    com_time += (t1 - com0time);

    disp_time (com_time, !man_white);

    disp_move (&mv, !man_white);

    beep ();
    disp_piece (&mv);

    b2board (&board[Y_8][X_a], &board0[Y_8][X_a]);

    up_board (&mv);

    b2board (&board0[Y_8][X_a], &board[Y_8][X_a]);

    disp_check (!man_white);

    record_move (&mv, num_move);

    em = ((man_white) ? check_b () : check_w ());
    max_depth = 1;
    level_depth = level;
    if (((!man_white) && (max_black (0, -10000, 10000) == -10000))
	    || ((man_white) && (min_white (0, -10000, 10000) == 10000))) {
	max_depth = MAX_DEPTH;
	if (em)
	    return (1);
	else
	    return (2);
    }

    max_depth = MAX_DEPTH;

    /* next is modified for evalprint */
    evalmask=0;

    return (0);
}
extern int  think_depth,
            max_depth,
            level_depth;
extern int  mask,
            man_white,
            board[12][12];
extern long com_time,
            com0time;
extern  PMOVE pmv[],
        pmv0[];

static int  val[] = {
    0, 1, 2, 3, 4, 5, 6, 0, 0, 1, 2, 3, 4, 5, 6, 0,
    0, 1, 2, 3, 4, 5, 6, 0, 0, 1, 2, 3, 4, 5, 6, 0,
};

int     advb (ps, pd, depth, palpha, beta)/* try 1 move, & get aipha */
int    *ps,
       *pd,
        depth,
       *palpha,
        beta;			/* 0: next try,  1: end of try */
{
    int     i0,
            is,
            id,
            bt;

    evalprint_mess("advb",depth,*palpha,beta);
    disp_move_fp(depth,ps,pd);

    if (*pd < ID_SP)
	return (0);

    id = *pd;
    is = *ps;
    *ps = ID_SP;
    if ((is == IDP_B) || (is == IDK_B))
	*pd = i0 = is;
    else
	*pd = i0 = (is | 8);


    if (check_w ()) {
	*ps = is;
	*pd = id;
	return (0);
    }
    if (depth >= level_depth) {
	if ((id == ID_SP) || ((val[id] <= val[is]) && under_w (pd))
		|| (evaluate_advb (ps,pd,depth,palpha,beta) < *palpha)) {
	    *ps = is;
	    *pd = id;
	    return (0);
	}
    }
    if (mask) {
    /* locate(65,depth+2); */
	wmove (aho, 65, depth + 2);
	wprintw (aho, "%c", 'A' + (ps - &board[0][0]) % 12 - 2);
	wprintw (aho, "%c", '8' - (ps - &board[0][0]) / 12 + 2);
	wprintw (aho, "%c", 'A' + (pd - &board[0][0]) % 12 - 2);
	wprintw (aho, "%c", '8' - (pd - &board[0][0]) / 12 + 2);
    /* locate(65,depth+3); */
	wmove (aho, 65, depth + 3);
	wprintw (aho, "----");
	wrefresh (aho);
    }
    bt = min_white (depth + 1, *palpha, beta);

    *ps = is;
    *pd = id;

    if (*palpha < bt) {
	*palpha = bt;
	pmv[depth].p_src = ps;
	pmv[depth].p_des = pd;
	pmv[depth].i_des = i0;
	if (!depth) {
	    for (i0 = 0; i0 < think_depth; i0++) {
		pmv0[i0].p_src = pmv[i0].p_src;
		pmv0[i0].p_des = pmv[i0].p_des;
		pmv0[i0].i_des = pmv[i0].i_des;
		if (mask && (max_depth > 1)) {
		/* locate(71,i0+2); */
		    wmove (aho, 71, i0 + 2);
		    wprintw (aho, "%c", 'A' + (pmv[i0].p_src - &board[0][0]) % 12 - 2);
		    wprintw (aho, "%c", '8' - (pmv[i0].p_src - &board[0][0]) / 12 + 2);
		    wprintw (aho, "%c", 'A' + (pmv[i0].p_des - &board[0][0]) % 12 - 2);
		    wprintw (aho, "%c", '8' - (pmv[i0].p_des - &board[0][0]) / 12 + 2);
		/* locate(71,i0+3); */
		    wmove (aho, 71, i0 + 3);
		    wprintw (aho, "----");
		    wrefresh (aho);
		}
	    }
	}
	if (*palpha >= beta)
	    return (1);
    }
    return (0);
}

int     max_black (depth, alpha, beta)/* try black move */
int     depth,
        alpha,
        beta;			/* return (alpha) */
{
    long    t1;
    int     x,
            y,
           *ps,
           *p,
            al;


    evalprint_mess("max_black",depth,alpha,beta);

    think_depth = depth;
    if (depth >= max_depth)
	return (evaluate_max_black (1,depth,alpha,beta));

    al = alpha;

    for (x = X_a; x <= X_h; x++)
	if (board[Y_5][x] == IDP_B1)
	    board[Y_5][x] = IDP_B;

    if (depth == 1) {
	time (&t1);

	/* disp_time (com_time + (t1 - com_time), !man_white);*/
	disp_time (com_time + (t1 - com0time), !man_white);

    /* locate(X_MOVE,((man_white)? Y_B_MOVE : Y_W_MOVE)); */
    }
    for (x = X_e; x <= X_h; x = ((x > X_d) ? 11 : 12) - x) {
	for (y = Y_1; y >= Y_8; y--) {
	    ps = &board[y][x];
	    if (*ps < ID_SP) {
		switch (*ps) {
		    case IDP_B: 
			if (y == Y_7) {
			    if ((*(ps + 12) == ID_SP) && (*(ps + 24) == ID_SP)) {
				*ps = IDP_B1;
				if (advb (ps, ps + 24, depth, &al, beta)) {
				    *ps = IDP_B;
				    return (al);
				}
				/* *ps = IDP_W;*/
				*ps = IDP_B;
			    }
			}
			else
			    /* next is modified */
			    /* if (y == Y_5) {*/
			    if (y == Y_4) {
				/* if ((*(p = ps - 1) == IDP_B1)*/
					/* || (*(ps = ps + 1) == IDP_B1)) {*/
				if ((*(p = ps - 1) == IDP_W1)
					|| (*(ps = ps + 1) == IDP_W1)) {
				    *p = ID_SP;
				    /* if (advw (ps, p - 12, depth, alpha, &al)) {*/
				    if (advb (ps, p - 12, depth, &al, beta)) {
					*p = IDP_B1;
					return (al);
				    }
				    *p = IDP_B1;
				}
			    }
			    else
			    /* next is modified */
			    /* if (y == Y_7) {*/
				if (y == Y_2) {
				    /* *ps = IDQ_W1;*/
				    *ps = IDQ_B1;
				    /* if (((*(ps - 11) < ID_SP)*/
				    if (((*(ps + 11) < ID_SP)
						&& advb (ps, ps + 11, depth, &al, beta))
					    || ((*(ps + 13) > ID_SP)
						&& advb (ps, ps + 13, depth, &al, beta))
					    || ((*(ps + 12) == ID_SP)
						&& advb (ps, ps + 12, depth, &al, beta))) {
					*ps = IDP_B;
					return (al);
				    }
				    *ps = IDN_B1;
				    if (((*(ps + 11) > ID_SP)
						&& advb (ps, ps + 11, depth, &al, beta))
					    || ((*(ps + 13) > ID_SP)
						&& advb (ps, ps + 13, depth, &al, beta))
					    || ((*(ps + 12) == ID_SP)
						&& advb (ps, ps + 12, depth, &al, beta))) {
					*ps = IDP_B;
					return (al);
				    }
				    *ps = IDP_B;
				}

			if ((*(ps + 11) > ID_SP)
				&& advb (ps, ps + 11, depth, &al, beta))
			    return (al);
			if ((*(ps + 13) > ID_SP)
				&& advb (ps, ps + 13, depth, &al, beta))
			    return (al);
			if ((*(ps + 12) == ID_SP)
				&& advb (ps, ps + 12, depth, &al, beta))
			    return (al);
			break;

		    case IDN_B: 
		    case IDN_B1: 
			if (advb (ps, ps + 25, depth, &al, beta))
			    return (al);
			if (advb (ps, ps + 23, depth, &al, beta))
			    return (al);
			if (advb (ps, ps + 14, depth, &al, beta))
			    return (al);
			if (advb (ps, ps + 10, depth, &al, beta))
			    return (al);
			if (advb (ps, ps - 10, depth, &al, beta))
			    return (al);
			if (advb (ps, ps - 14, depth, &al, beta))
			    return (al);
			if (advb (ps, ps - 23, depth, &al, beta))
			    return (al);
			if (advb (ps, ps - 25, depth, &al, beta))
			    return (al);
			break;

		    case IDB_B: 
		    case IDB_B1: 
		    case IDQ_B: 
		    case IDQ_B1: 
			p = ps;
			do {
			    if (advb (ps, p += 13, depth, &al, beta))
				return (al);
			} while (*p == ID_SP);
			p = ps;
			do {
			    if (advb (ps, p += 11, depth, &al, beta))
				return (al);
			} while (*p == ID_SP);
			p = ps;
			do {
			    if (advb (ps, p -= 11, depth, &al, beta))
				return (al);
			} while (*p == ID_SP);
			p = ps;
			do {
			    if (advb (ps, p -= 13, depth, &al, beta))
				return (al);
			} while (*p == ID_SP);
			if ((*ps & ID_SIDE) == IDB_B)
			    break;
		    case IDR_B: 
		    case IDR_B1: 
			p = ps;
			do {
			    if (advb (ps, p += 12, depth, &al, beta))
				return (al);


			} while (*p == ID_SP);
			p = ps;
			do {
			    if (advb (ps, ++p, depth, &al, beta))
				return (al);
			} while (*p == ID_SP);
			p = ps;
			do {
			    if (advb (ps, --p, depth, &al, beta))
				return (al);
			} while (*p == ID_SP);
			p = ps;
			do {
			    if (advb (ps, p -= 12, depth, &al, beta))
				return (al);
			} while (*p == ID_SP);
			break;

		    case IDK_B: 
			if ((*(ps - 4) == IDR_B) && (*(ps - 3) == ID_SP)
				&& (*(ps - 2) == ID_SP) && (*(ps - 1) == ID_SP)
				&& (!under_w (ps - 2)) && (!under_w (ps - 1))
				&& (!under_w (ps))) {
			    *(ps - 1) = IDR_B1;
			    *(ps - 4) = ID_SP;
			    if (advb (ps, ps - 2, depth, &al, beta)) {
				*(ps - 1) = ID_SP;
				*(ps - 4) = IDR_B;
				return (al);
			    }
			    *(ps - 1) = ID_SP;
			    *(ps - 4) = IDR_B;
			}


			if ((*(ps + 3) == IDR_B) && (*(ps + 2) == ID_SP)
				&& (*(ps + 1) == ID_SP) && (!under_w (ps))
				&& (!under_w (ps + 1)) && (!under_w (ps + 2))) {
			    *(ps + 1) = IDR_B1;
			    *(ps + 3) = ID_SP;
			    if (advb (ps, ps + 2, depth, &al, beta)) {
				*(ps + 1) = ID_SP;
				*(ps + 3) = IDR_B;
				return (al);
			    }
			    *(ps + 1) = ID_SP;
			    *(ps + 3) = IDR_B;
			}
		    case IDK_B1: 
			if (advb (ps, ps + 11, depth, &al, beta))
			    return (al);
			if (advb (ps, ps + 12, depth, &al, beta))
			    return (al);
			if (advb (ps, ps + 13, depth, &al, beta))
			    return (al);
			if (advb (ps, ps + 1, depth, &al, beta))
			    return (al);
			if (advb (ps, ps - 1, depth, &al, beta))
			    return (al);
			if (advb (ps, ps - 11, depth, &al, beta))
			    return (al);
			if (advb (ps, ps - 12, depth, &al, beta))
			    return (al);
			if (advb (ps, ps - 13, depth, &al, beta))
			    return (al);
		}		/* switch */
	    }			/* if */
	}			/* for */
    }				/* for */
    if (al > alpha)
	return (al);
    if (depth >= level_depth)
	return (evaluate_max_black (2,depth,alpha,beta));
    return (alpha);
}


extern int  think_depth,
            max_depth,
            level_depth;
extern int  mask,
            man_white,
            board[12][12];
extern long com_time,
            com0time;
extern  PMOVE pmv[],
        pmv0[];

/* static int  val[] = {
    0, 1, 2, 3, 4, 5, 6, 0, 0, 1, 2, 3, 4, 5, 6, 0,
    0, 1, 2, 3, 4, 5, 6, 0, 0, 1, 2, 3, 4, 5, 6, 0
}; */

int     advw (ps, pd, depth, alpha, pbeta)/* try 1 move, & get deta */
int    *ps,
       *pd,
        depth,
        alpha,
       *pbeta;			/* 0: next try, 1: end of try */
{
    int     i0,
            is,
            id,
            al;

    evalprint_mess("advw",depth,alpha,*pbeta);
    disp_move_fp(depth,ps,pd);

    if ((*pd > ID_SP) || (*pd == ID_OB))
	return (0);

    id = *pd;
    is = *ps;
    *ps = ID_SP;
    if ((is == IDP_W) || (is == IDK_W))
	*pd = i0 = is;
    else
	*pd = i0 = (is | 8);
    if (check_b ()) {
	*ps = is;
	*pd = id;
	return (0);
    }
    if (depth >= level_depth) {
	if ((id == ID_SP) || ((val[id] <= val[is]) && under_b (pd))
		|| (evaluate_advw (ps,pd,depth,alpha,pbeta) > *pbeta)) {
	    *ps = is;
	    *pd = id;
	    return (0);
	}
    }
    if (mask) {
    /* locate (65, depth + 2); */
	wmove (aho, 65, depth + 2);
	wprintw (aho, "%c", 'A' + (ps - &board[0][0]) % 12 - 2);
	wprintw (aho, "%c", '8' - (ps - &board[0][0]) / 12 + 2);
	wprintw (aho, "%c", 'A' + (pd - &board[0][0]) % 12 - 2);
	wprintw (aho, "%c", '8' - (pd - &board[0][0]) / 12 + 2);
    /* locate (65, depth + 3); */
	wmove (aho, 65, depth + 3);
	wprintw (aho, "----");
	wrefresh (aho);
    }
    al = max_black (depth + 1, alpha, *pbeta);

    *ps = is;
    *pd = id;

    if (*pbeta > al) {
	*pbeta = al;
	pmv[depth].p_src = ps;
	pmv[depth].p_des = pd;
	pmv[depth].i_des = i0;
	if (!depth) {
	    for (i0 = 0; i0 < think_depth; i0++) {
		pmv0[i0].p_src = pmv[i0].p_src;
		pmv0[i0].p_des = pmv[i0].p_des;
		pmv0[i0].i_des = pmv[i0].i_des;
		if (mask && (max_depth > 1)) {
		/* locate (71, i0 + 2); */
		    wmove (aho, 71, i0 + 2);
		    wprintw (aho, "%c", 'A' + (pmv[i0].p_src - &board[0][0]) % 12 - 2);
		    wprintw (aho, "%c", '8' + (pmv[i0].p_src - &board[0][0]) / 12 + 2);
		    wprintw (aho, "%c", 'A' + (pmv[i0].p_des - &board[0][0]) % 12 - 2);
		    wprintw (aho, "%c", '8' - (pmv[i0].p_des - &board[0][0]) / 12 + 2);
		/* locate(71,i0+3); */
		    wmove (aho, 71, i0 + 3);
		    wprintw (aho, "----");
		    wrefresh (aho);
		}
	    }
	}
	if (*pbeta <= alpha)
	    return (1);
    }
    return (0);
}

int     min_white (depth, alpha, beta)/* try white move */
int     depth,
        alpha,
        beta;			/* return (bete) */
{
    long    t1;
    int     x,
            y,
           *ps,
           *p,
            be;

    evalprint_mess("min_white",depth,alpha,beta);

    think_depth = depth;
    if (depth >= max_depth)
	return (evaluate_min_white (1,depth,alpha,beta));

    be = beta;

    for (x = X_a; x <= X_h; x++)
	if (board[Y_4][x] == IDP_W1)
	    board[Y_4][x] = IDP_W;

    if (depth == 1) {
	time (&t1);

	disp_time (com_time + (t1 - com0time), !man_white);
    /* locate(X_MOVE,((man_white)? Y_B_MOVE : Y_W_MOVE)); */
    }
    for (x = X_e; x <= X_h; x = ((x > X_d) ? 11 : 12) - x) {
	for (y = Y_8; y <= Y_1; y++) {
	    ps = &board[y][x];
	    if (*ps > ID_SP) {
		switch (*ps) {
		    case IDP_W: 
			if (y == Y_2) {
			    if ((*(ps - 12) == ID_SP) && (*(ps - 24) == ID_SP)) {
				*ps = IDP_W1;
				if (advw (ps, ps - 24, depth, alpha, &be)) {
				    *ps = IDP_W;
				    return (be);
				}
				/* *ps = IDP_B;*/
				*ps = IDP_B;
			    }
			}
			else
			/* next is revised */
			    if (y == Y_5) {
			/* next is revised */
				if ((*(p = ps - 1) == IDP_B1)
			/* next is revised */
					|| (*(p = ps + 1) == IDP_B1)) {
				    *p = ID_SP;
				/* if (advb(ps,p+12,depth,&al,beta)) { */
			/* next is revised */
				    if (advw (ps, p + 12, depth,alpha, &be)) {
					*p = IDP_W1;
				    /*       return(al); */
					return (be);
				    }
				    *p = IDP_W1;
				}
			    }
			    else
			/* next is revised */
				if (y == Y_7) {
				    /* *ps = IDQ_B1;*/
				    *ps = IDQ_W1;
				    /* if (((*(ps + 11) > ID_SP)*/
				    if (((*(ps - 11) > ID_SP)
						&& advw (ps, ps - 11, depth, alpha, &be))
					    || ((*(ps - 13) < ID_SP)
						&& advw (ps, ps - 13, depth, alpha, &be))
					    || ((*(ps - 12) == ID_SP)
						&& advw (ps, ps - 12, depth, alpha, &be))) {
					*ps = IDP_W;
					return (be);
				    }
				    *ps = IDN_W1;
				    if (((*(ps - 11) < ID_SP)
						&& advw (ps, ps - 11, depth, alpha, &be))
					    || ((*(ps - 13) < ID_SP)
						&& advw (ps, ps - 13, depth, alpha, &be))
					    || ((*(ps - 12) == ID_SP)
						&& advw (ps, ps - 12, depth, alpha, &be))) {
					*ps = IDP_W;
					return (be);
				    }
				    *ps = IDP_W;
				}

			if ((*(ps - 11) < ID_SP)
				&& advw (ps, ps - 11, depth, alpha, &be))
			    return (be);
			if ((*(ps - 13) < ID_SP)
				&& advw (ps, ps - 13, depth, alpha, &be))
			    return (be);
			if ((*(ps - 12) == ID_SP)
				&& advw (ps, ps - 12, depth, alpha, &be))
			    return (be);
			break;

		    case IDN_W: 
		    case IDN_W1: 
			if (advw (ps, ps - 25, depth, alpha, &be))
			    return (be);
			if (advw (ps, ps - 23, depth, alpha, &be))
			    return (be);
			if (advw (ps, ps - 14, depth, alpha, &be))
			    return (be);
			if (advw (ps, ps - 10, depth, alpha, &be))
			    return (be);
			if (advw (ps, ps + 10, depth, alpha, &be))
			    return (be);
			if (advw (ps, ps + 14, depth, alpha, &be))
			    return (be);
			if (advw (ps, ps + 23, depth, alpha, &be))
			    return (be);
			if (advw (ps, ps + 25, depth, alpha, &be))
			    return (be);
			break;

		    case IDB_W: 
		    case IDB_W1: 
		    case IDQ_W: 
		    case IDQ_W1: 
			p = ps;
			do {
			    if (advw (ps, p -= 13, depth, alpha, &be))
				return (be);
			} while (*p == ID_SP);
			p = ps;
			do {
			    if (advw (ps, p -= 11, depth, alpha, &be))
				return (be);
			} while (*p == ID_SP);
			p = ps;
			do {
			    if (advw (ps, p += 11, depth, alpha, &be))
				return (be);
			} while (*p == ID_SP);
			p = ps;
			do {
			    if (advw (ps, p += 13, depth, alpha, &be))
				return (be);
			} while (*p == ID_SP);
			if ((*ps & ID_SIDE) == IDB_W)
			    break;
		    case IDR_W: 
		    case IDR_W1: 
			p = ps;
			do {
			    if (advw (ps, p -= 12, depth, alpha, &be))
				return (be);

			} while (*p == ID_SP);
			p = ps;
			do {
			    if (advw (ps, --p, depth, alpha, &be))
				return (be);
			} while (*p == ID_SP);
			p = ps;
			do {
			    if (advw (ps, ++p, depth, alpha, &be))
				return (be);
			} while (*p == ID_SP);
			p = ps;
			do {
			    if (advw (ps, p += 12, depth, alpha, &be))
				return (be);
			} while (*p == ID_SP);
			break;

		    case IDK_W: 
			if ((*(ps - 4) == IDR_W) && (*(ps - 3) == ID_SP)
				&& (*(ps - 2) == ID_SP) && (*(ps - 1) == ID_SP)
				&& (!under_b (ps - 2)) && (!under_b (ps - 1))
				&& (!under_b (ps))) {
			    *(ps - 1) = IDR_W1;
			    *(ps - 4) = ID_SP;
			    if (advw (ps, ps - 2, depth, alpha, &be)) {
				*(ps - 1) = ID_SP;
				*(ps - 4) = IDR_W;
				return (be);
			    }
			    *(ps - 1) = ID_SP;
			    *(ps - 4) = IDR_W;
			}

			if ((*(ps + 3) == IDR_W && (*(ps + 2) == ID_SP)
				    && (*(ps + 1) == ID_SP) && (!under_b (ps))
				    && (!under_b (ps + 1)) && (!under_b (ps + 2)))) {
			    *(ps + 1) = IDR_W1;
			    *(ps + 3) = ID_SP;
			    if (advw (ps, ps + 2, depth, alpha, &be)) {
				*(ps + 1) = ID_SP;
				*(ps + 3) = IDR_W;
				return (be);
			    }
			    *(ps + 1) = ID_SP;
			    *(ps + 3) = IDR_W;
			}
		    case IDK_W1: 
			if (advw (ps, ps - 11, depth, alpha, &be))
			    return (be);
			if (advw (ps, ps - 12, depth, alpha, &be))
			    return (be);
			if (advw (ps, ps - 13, depth, alpha, &be))
			    return (be);
			if (advw (ps, ps - 1, depth, alpha, &be))
			    return (be);
			if (advw (ps, ps + 1, depth, alpha, &be))
			    return (be);
			if (advw (ps, ps + 11, depth, alpha, &be))
			    return (be);
			if (advw (ps, ps + 12, depth, alpha, &be))
			    return (be);
			if (advw (ps, ps + 13, depth, alpha, &be))
			    return (be);
		}		/* switch */
	    }			/* if */
	}			/* for */
    }				/* for */
    if (be < beta)
	return (be);
    if (depth >= level_depth)
	return (evaluate_min_white (2,depth,alpha,beta));
    return (beta);
}


void beep () {
    putchar (7);
}

/* void disp_position()*/
/*{*/
 /* int  i ; */
 /* setviewport(0.,0.,767.,479.); */
 /* setwindow(0.,0.,767.,479.); */
 /* i=0; */
/*     move(9.,55.+35.*(++i)); text("1");*/
/*     move(9.,40.+50.*(++i)); text("2");*/
/*     move(9.,40.+50.*(++i)); text("3");*/
/*     move(9.,40.+50.*(++i)); text("4");*/
/*     move(9.,40.+50.*(++i)); text("5");*/
/*     move(9.,40.+50.*(++i)); text("6");*/
/*     move(9.,40.+50.*(++i)); text("7");*/
/*     move(9.,40.+50.*(++i)); text("8");*/
/*     i=1;*/
/*     move(50.*i++,49.); text("a");*/
/*     move(50.*i++,49.); text("b");*/
/*     move(50.*i++,49.); text("c");*/
/*     move(50.*i++,49.); text("d");*/
/*     move(50.*i++,49.); text("e");*/
/*     move(50.*i++,49.); text("f");*/
/*     move(50.*i++,49.); text("g");*/
/*     move(50.*i++,49.); text("h");*/
/*     i=74;*/
/*      cursor(4,i); */
/*     printf("[CTRL.E]");*/
/*     cursor(5,i);*/
/*     printf("exchange");*/
/*     cursor(7,i); */
/*     printf("[CTRL.R]");*/
/*      cursor(8,i); */
/*     printf("initial ");*/
/*      cursor(10,i);*/
/*     printf("[CTRL.T]");*/
/*      cursor(11,i);*/
/*     printf("forward");*/
/*      cursor(13,i);*/
/*     printf("[CTRL.Y]");*/
/*      cursor(14,i);*/
/*     printf("backboard");*/
/*      cursor(16,i);*/
/*     printf("[CTRL.U]");*/
/*      cursor(17,i);*/
/*     printf("load");*/
/*      cursor(19,i);*/
/*     printf("[CTRL.O]");*/
/*      cursor(20,i);*/
/*     printf("save");*/
/*      cursor(24,1);*/
/*}*/

/*void disp_box (x,y,id)*/
/*int  x,y,id ;*/
/*{*/
/*    int iro,koma,store ;*/

/*    id &= ID_SIDE;*/
/*    x -= 1;*/
/*    y -= 1;*/
/*    koma = store = id      ;*/
/*    iro  = (id >>= 4);*/
/*    store >>= 3;*/
/*    store <<= 3; */
/*    koma -= store ;*/
/*    */
/*    if (iro != 1) setcolor("red",1); */
/*             else setcolor("white",1);*/
/*        setviewport(29.0+50.0*(x-1),455.0-50.0*y,*/
/*                    29.0+50.0* x   ,455.0-50.0*(y-1));*/
/*   cess(koma);*/
/*}*/
void disp_board () {
    int     x,
            y,
           *p;

 /* cur_erase(); */

    p = &board[Y_8][X_a];
    for (y = Y_8; y <= Y_1; y++, p += 4)
	for (x = X_a; x <= X_h; x++, p++)
	   /* disp_box (x, y, *p);*/
	    disp_box (x, y, board[y][x]);
}

void disp_position () {
    int     i;

 /* cur_erase(); */
    for (i = 0; i < 8; i++) {
	wmove (aho, 6, 2 * i + 3);
	wprintw (aho, "%c", '8' - i);
    }
    for (i = 0; i < 8; i++) {
	wmove (aho, 4 * i + 9, 19);
	wprintw (aho, "%c", 'a' + i);
    }
}
void disp_box (x, y, id)
int     x,
        y,
        id;
{
    int     h,
            v,
            k;

    id &= ID_SIDE;
    h = id >> 4;		/* player */
    v = id & 7;			/* sita 3bit */
    v <<= 2;			/* koma sitee */
    switch (v) {
	case 4: 
	    put_koma2(x, y, 1, h);
	    break;
	case 8: 
	    put_koma2(x, y, 2, h);
	    break;
	case 12: 
	    put_koma2(x, y, 3, h);
	    break;
	case 16: 
	    put_koma2(x, y, 4, h);
	    break;
	case 20: 
	    put_koma2(x, y, 5, h);
	    break;
	case 24: 
	    put_koma2(x, y, 6, h);
	    break;
	default: 
	    put_koma2(x, y, 0, h);
    }
}


/*shori1()
{
int a,aa,i;
for(a = 0; a < 2; a++)
	for(aa = 0; aa < 16; aa++)
	{
		ko[aa].x = 1;  
		ko[aa].y = 1; 
		ko[aa].f = 0; 
		ko[aa].k = 8;  */


#define V_P8   180
#define V_P1   224
#define V_N    590
#define V_B    590
#define V_R    990
#define V_Q    1570
#define V_K    5000
#define V_Kc   255
#define VMN    15
#define VMB    24
#define VMR    30
#define VMQ    63
#define VMK    77

extern int  num_move,
            board[12][12];

static int  E_B[] = {
    -10, 1, 3, 3, 5, 7, 2, 0, 0, 1, 3, 3, 5, 7, 2, 0, 14, 6, 17, 17, 26, 39, 45, 0, 0, 6, 17, 17, 26, 39, 45, 0
};
static int  E_W[] = {
    -10, 6, 17, 17, 26, 39, 45, 0, 0, 6, 17, 17, 26, 39, 45, 0, 14, 1, 3, 3, 5, 7, 2, 0, 0, 1, 3, 3, 5, 7, 2, 0
};

#define BB1(d) value += E_B[*(pb + (d))];

#define WW1(d) value -= E_W[*(pb + (d))];

#define BB2(d) p=pb; do v += E_B[*(p+=(d))];while(*p == ID_SP);

#define WW2(d) p=pb; do v += E_W[*(p+=(d))]; while(*p == ID_SP);

int     evaluate () {		/* get piece value & effect */
    int     value,
            vpb,
            vpw,
            nxx,
            v;
    int     x,
            y,
           *pb,
           *p;

    value = 0;

    nxx = (num_move << 3);

    pb = &board[Y_8][X_a];
    for (y = Y_8; y <= Y_1; y++, pb += 4) {
	vpb = nxx + (y << 2);
	vpw = nxx - (y << 2);
	for (x = X_a; x <= X_h; x++, pb++) {
	    if (*pb == ID_SP)
		continue;
	    if (*pb < ID_SP) {
		switch (*pb) {
		    case IDP_B1: 
		    case IDP_B: 
			value += V_P8 + vpb;
			BB1 (11)
			    BB1 (13)
			    break;

		    case IDN_B1: 
			value += VMN;
		    case IDN_B: 
			value += V_N;
			BB1 (-25)
			    BB1 (-23)
			    BB1 (-14)
			    BB1 (-10)
			    BB1 (25)
			    BB1 (23)
			    BB1 (14)
			    BB1 (10)
			    break;

		    case IDB_B1: 
			value += VMB;
		    case IDB_B: 
			value += V_B;
			v = 0;
			BB2 (-13)
			    BB2 (-11)
			    BB2 (11)
			    BB2 (13)
			    value += v;
			break;

		    case IDR_B: 
			value += VMR;
		    case IDR_B1: 
			value += V_R;
			v = 0;
			BB2 (1)
			    BB2 (-1)
			    BB2 (12)
			    BB2 (-12)
			    value += v / 2;
			break;
		    case IDQ_B: 
			value += VMQ;
		    case IDQ_B1: 
			value += V_Q;
			v = 0;
			BB2 (-13)
			    BB2 (-12)
			    BB2 (-11)
			    BB2 (-1)
			    BB2 (13)
			    BB2 (12)
			    BB2 (11)
			    BB2 (1)
			    value += v / 4;
			break;

		    case IDK_B: 
			value += VMK;
			if ((pb == &board[Y_8][X_c])
				|| (pb == &board[Y_8][X_g]))
			    value += V_Kc;
		    case IDK_B1: 
			value += V_K;
			BB1 (-1)
			    BB1 (-11)
			    BB1 (-12)
			    BB1 (-13)
			    BB1 (1)
			    BB1 (11)
			    BB1 (12)
			    BB1 (13)
		}		/* switch */
	    }			/* if */
	    else {
		switch (*pb) {
		    case IDP_W1: 
		    case IDP_W: 
			value -= V_P1 + vpw;
			WW1 (-11)
			    WW1 (-13)
			    break;

		    case IDN_W1: 
			value -= VMN;
		    case IDN_W: 
			value -= V_N;
			WW1 (-25)
			    WW1 (-23)
			    WW1 (-14)
			    WW1 (-10)
			    WW1 (25)
			    WW1 (23)
			    WW1 (14)
			    WW1 (10)
			    break;

		    case IDB_W1: 
			value -= VMB;
		    case IDB_W: 
			value -= V_B;
			v = 0;
			WW2 (-13)
			    WW2 (-11)
			    WW2 (11)
			    WW2 (13)
			    value -= v;
			break;

		    case IDR_W: 
			value -= VMR;
		    case IDR_W1: 
			value -= V_R;
			v = 0;
			WW2 (1)
			    WW2 (-1)
			    WW2 (12)
			    WW2 (-12)
			    value -= v / 2;
			break;


		    case IDQ_W: 
			value -= VMQ;
		    case IDQ_W1: 
			value -= V_Q;
			v = 0;
			WW2 (-13)
			    WW2 (-12)
			    WW2 (-11)
			    WW2 (-1)
			    WW2 (13)
			    WW2 (12)
			    WW2 (11)
			    WW2 (1)
			    value -= v / 4;
			break;

		    case IDK_W: 
			value -= VMK;
			if ((pb == &board[Y_1][X_c])
				|| (pb == &board[Y_1][X_g]))
			    value -= V_Kc;
		    case IDK_W1: 
			value -= V_K;
			WW1 (-1)
			    WW1 (-11)
			    WW1 (-12)
			    WW1 (-13)
			    WW1 (1)
			    WW1 (11)
			    WW1 (12)
			    WW1 (13)
		}		/* switch */
	    }			/* else */
	}			/* for(x) */
    }				/* for(y) */

    return (value);
}


void game_end (em)
int     em;
{
    g_end = 1;

 /* cursor(15,49); */
    wmove (aho, 15, 49);
    switch (em) {
	case 1: 
	    wprintw (aho, "CHECKMATE !!  ");
	    break;
	case 2: 
	    wprintw (aho, "STALEMATE !!  ");
	    break;
	case 3: 
	    wprintw (aho, "COMPUT LOSE !!");
	    break;
    }
    wrefresh (aho);
    wait (600);
    endwin ();			/* henkou 1032 */
}

void initial () {		/* initialize */
    init_board ();

    disp_board (); 

    max_depth = MAX_DEPTH;
    g_end = 0;
    man_white = 1;
    num_move = 0;
    com_time = man_time = 0L;
}

void main (argc, argv)		/* main routine */
int     argc;
char   *argv[];
{
    int     em;
    void ogata ();
    signal (SIGINT, ogata);
    initscr ();
    aho = newwin (0, 0, 0, 0);
    wrefresh (aho);
/*	crmode(); */

 /* onerror (1); */

 /* initgraphics(); */
 /* setup("aw0"); */
 /* setup("sm0"); */
 /* clearscreen(); */
    mask = ((*argv[1] == 'M') ? 1 : 0);

    evalmask = 0;

    rmv[0].x_src = 0;
    level = 2;

 /* next ... initialization for board windows */
    pass = newwin (20, 37, 2, 4);
    drow_ban ();
    wrefresh (pass);
    disp_board ();
    wrefresh (pass);



    initial ();
 /* disp_position(); */
 /* cursor(12,49); */
    wmove (aho, 12, 55);
    wprintw (aho, "LEVEL = %1d", level);
    wmove (aho, 13, 50);
    wrefresh (aho);
 /* cursor(24,1); */


    while (1) {
	switch (em = man_move ()) {
	    case 0: 
		num_move++;
		break;
	    case 1: 
		game_end (em);
		break;
	    case 2: 
		game_end (em);
		break;
	    case 3: 
		break;
	}
	if (g_end)
	    continue;
	switch (em = com_move ()) {
	    case 0: 
		num_move++;
		break;
	    case 1: 
		game_end (em);
		break;
	    case 2: 
		game_end (em);
		break;
	    case 3: 
		game_end (em);
		break;
	}
    }
/* setup("aw1"); */
/* setup("sm1"); */
}
#define ABS(a,b)  (((a)>(b))? (a)-(b):(b)-(a))

extern int  level,
            level_depth,
            max_depth;
extern int  man_white,
            num_move,
            board[12][12],
            board0[12][12];
int     up_grade () {		/* upgrade PAWN */
    int     c;

    while (1) {
	beep ();
	wmove (aho, Y_MESSAGE, X_MESSAGE);
	wprintw (aho, "input(Q,N,B,R)=     ");
	wmove (aho, Y_MESSAGE, X_MESSAGE + 15);
	if (isalpha (c = cget ())) {
	    wprintw (aho, "%c", (c = toupper (c)));
	    switch (c) {
		case 'Q': 
		    return (ID_QUEE1);
		case 'N': 
		    return (ID_KNIG1);
		case 'R': 
		    return (ID_ROOK1);
		case 'B': 
		    return (ID_BISH1);
	    }
	}
	beep ();
	wait (100);
	beep ();
	wait (100);
	beep ();
	wait (100);
    }
 /* return (0);			dummy */
}
int     man_move () {		/* man side move */
 /* 0: continue,  1:man win,  2:stalemate, 3:exchange sides */
    char    str[8];
    MOVE mv;
    int     em;

    reset2pawn (man_white);

    while (1) {

	disp_prompt (man_white, PLAYER, num_move);

	if (em = str_get (str))
	    return (em);

	mv.x_src = str[0] - 'A' + 2;
	mv.y_src = '8' - str[1] + 2;
	mv.x_des = str[2] - 'A' + 2;
	mv.y_des = '8' - str[3] + 2;
	mv.i_src = board0[mv.y_src][mv.x_src];

	if (((mv.i_src & ID_MOVE) == ID_PAWN)
		&& (ABS (mv.y_src, mv.y_des) != 2))
	    mv.i_des = mv.i_src;
	else
	    mv.i_des = mv.i_src | 8;

	b2board (&board[Y_8][X_a], &board0[Y_8][X_a]);

	if (move_check (&mv, man_white)) {
	    up_board (&mv);
	    if (man_white && !check_b ())
		break;
	    if (!man_white && !check_w ())
		break;
	}

	wmove (aho, Y_MESSAGE, X_MESSAGE);
	wprintw (aho, "invalid move (%s) ", str);
	wrefresh (aho);
	beep ();
	wait (100);
	beep ();
	wait (100);
	beep ();
	wait (100);
    }				/* while */

    if (((mv.i_src & ID_PART) == ID_PAWN)
	    && ((mv.y_des == Y_8) || (mv.y_des == Y_1)))
	mv.i_des = (up_grade () | (man_white ? 16 : 0));

    disp_move (&mv, man_white);

    disp_piece (&mv);

    b2board (&board0[Y_8][X_a], &board[Y_8][X_a]);

    up_board (&mv);

    b2board (&board[Y_8][X_a], &board0[Y_8][X_a]);

    disp_check (man_white);

    record_move (&mv, num_move);

    em = ((man_white) ? check_w () : check_b ());
    max_depth = 1;
    level_depth = level;
    if (((man_white) && (max_black (0, -10000, 10000) == -10000))
	    || ((!man_white) && (min_white (0, -10000, 10000) == 10000))) {
	max_depth = MAX_DEPTH;
	if (em)
	    return (1);
	else
	    return (2);
    }
    max_depth = MAX_DEPTH;

    return (0);
}
extern int  g_end,
            level,
            man_white,
            num_move;
extern long man_time,
            com_time;
extern  MOVE rmv[];

void disp_message (n)		/* display key in message */
int     n;
{
    wmove (aho, Y_MESSAGE, X_MESSAGE);
    switch (n) {
	case 0: 
	    wprintw (aho, "                     ");
	    break;
	case 1: 
	    wprintw (aho, "exchange sides       ");
	    break;
	case 2: 
	    wprintw (aho, "forward              ");
	    break;
	case 3: 
	    wprintw (aho, "backward             ");
	    break;
	case 4: 
	    wprintw (aho, "change      ");
	    break;
	case 5: 
	    wprintw (aho, "can't open file !    ");
	    break;
	case 6: 
	    wprintw (aho, "       loading       ");
	    break;
	case 7: 
	    wprintw (aho, "end of loading       ");
	    break;
	case 8: 
	    wprintw (aho, "       saving        ");
	    break;
	case 9: 
	    wprintw (aho, "end of saving        ");
	    break;
	case 10: 
	    wprintw (aho, "invalid key !        ");
	    beep ();
	    beep ();
	    beep ();
	    break;
	case 11: 
	    wprintw (aho, "eval print begins    ");
	    break;
    }
    wrefresh (aho);
}

int     are_you_sure () {	/* confirm key in */
    int     c;

    beep ();
    wmove (aho, Y_MESSAGE, X_MESSAGE);
    wprintw (aho, "Are you sure ? (Y)  ");
    wmove (aho, Y_MESSAGE, X_MESSAGE + 18);

    /* next modified */
    wrefresh (aho);

    c = toupper (cget ());

    disp_message (0);

    return (c);
}

/*next for I/O using integer */
mywrite(fp,i)
FILE *fp;
int i;
{
	fprintf(fp,"%d ",i);
}

isnum(c)
char c;
{
	if ((c>='0') && (c<='9')) return(1);
	else return(0);
}

myread(fp)
FILE *fp;
{
	int i;
	char c;

	c=getc(fp);
	while (!(isnum(c))) c=getc(fp);
	i=c-'0';
	while (isnum(c=getc(fp))) i = 10 *i +c-'0';
	return(i);
}


int     str_get (ps0)		/* get string, display & update
				   comsumption time */
char   *ps0;			/* 0: normal input,   3: exchange sides */
{
    FILE * fp;
    char   *ps;
    int    *prmv,
            c,
	    i,j,
            n = 0,
            ii = 0;
    long    t0,
            t1,
            t2;

    while (cready ())
	c = cget ();
    ps = ps0;
    time (&t0);
    t1 = t0;

    while (1) {
	wmove (aho, (man_white ? Y_W_MOVE : Y_B_MOVE), X_MOVE + n);
	wrefresh (aho);
	do {
	    time (&t2);
	    if (t1 != t2) {
		t1 = t2;
		disp_time (man_time + (t1 - t0), man_white);
		wmove (aho, (man_white ? Y_W_MOVE : Y_B_MOVE), X_MOVE + n);
		wrefresh (aho);
	    }
	} while (!cready ());

	c = toupper (cget ());
	switch (c) {
	    case KEY_CR: 
		if (!g_end && (n == 4)) {
		    *ps = 0;
		    man_time += (t1 - t0);
		    disp_message (0);
		/* baka(ps-4); */
		    return (0);
		}
		else
		    disp_message (10);
		break;

	    case KEY_BS: 
		if (!g_end && (n > 0)) {
		    wprintw (aho, "%c %c", c, c);
		    wrefresh (aho);
		    ps--;
		    n--;
		    disp_message (0);
		}
		else
		    disp_message (10);
		break;

	    case CTRL_E: 
	    case 'Z': 
		if (!g_end) {
		    disp_message (1);
		    man_white = !man_white;
		    disp_prompt (BLACK, !man_white, num_move);
		    disp_time (com_time, !man_white);
		    disp_time (man_time, man_white);
		    disp_prompt (WHITE, man_white, num_move);
		    return (3);
		}
		else
		    disp_message (10);
		break;

	    case CTRL_R: 
	    case 'R': 
		if (are_you_sure () == 'Y') {

		    n = 0;
		    ps = ps0;
		    g_end = 0;
		    t0 = t1;
		    initial ();
		}
		break;

	    case CTRL_T: 
	    case 'T': 
		if (!g_end && (rmv[num_move].x_src)) {
		    disp_message (2);
		    n = 0;
		    ps = ps0;
		    man_white = !man_white;
		    disp_last (num_move);
		    disp_prompt (man_white, PLAYER, ++num_move);
		}
		else
		    disp_message (10);
		break;

	    case CTRL_Y: 
	    case 'Y': 
		if (num_move) {
		    disp_message (3);
		    n = 0;
		    ps = ps0;
		    g_end = 0;
		    man_white = !man_white;
		    disp_last (num_move -= 2);
		    disp_prompt (man_white, PLAYER, ++num_move);
		}
		else
		    disp_message (10);
		break;

	    case CTRL_U: 
	    case 'U': 
		disp_message (4);
		level++;
		if (level > 3)
		    level = 1;
	    /* locate(X_LEVEL,Y_LEVEL); */
		wmove (aho, X_LEVEL, Y_LEVEL);
		wprintw (aho, "level = %1d", level);
		wrefresh (aho);
		break;

	    case 'X': 
		if (are_you_sure () == 'Y') {
		    if ((fpev = fopen ("CHESS.EVAL", "w")) == NULL) {
			disp_message (5);
			beep ();
			beep ();
			beep ();
			beep ();
		    }
		    else {
			disp_message (11);
			fprintf(fpev,"*****************************\n");
			fprintf(fpev,"*   Evalution begins .....  *\n");
			fprintf(fpev,"*****************************\n");
			evalmask=1;
			fclose(fpev);
		    }
		}
		break;


	    case 'J': 
		if (are_you_sure () == 'Y') {
		    if ((fp = fopen ("CHESS.BOARD", "w")) == NULL) {
			disp_message (5);
			beep ();
			beep ();
			beep ();
			beep ();
		    }
		    else {
			disp_message (8);

			mywrite(fp,man_white);
			putc('\n',fp);
			for (i=0;i<12;i++)
				{
				for (j=0;j<12;j++)
				/* next board0 not board00 */
					mywrite(fp,board0[i][j]);
				putc('\n',fp);
				}
			fclose (fp);
			disp_message (9);
		    }
		}
		break;


	    case 'K': 
		if (are_you_sure () == 'Y') {
		    if ((fp = fopen ("CHESS.BOARD", "r")) == NULL) {
			disp_message (5);
			beep ();
			beep ();
			beep ();
			beep ();
		    }
		    else {
			disp_message (6);

			man_white=myread(fp);
			for (i=0;i<12;i++)
				{
				for (j=0;j<12;j++)
					board00[i][j]=myread(fp);
				}
			fclose (fp);
			disp_message (7);
		    }
		}
		

	    case CTRL_I: 
	    case 'I': 
		/* if ((fp = fopen ("CHESS.DAT", "rb")) == NULL) {*/
		if ((fp = fopen ("CHESS.DAT", "r")) == NULL) {
		    disp_message (5);
		    beep ();
		    beep ();
		    beep ();
		    beep ();
		}
		else {
		    disp_message (6);
		    prmv = &rmv[0].x_src;
		    do
			/* *prmv++ = c = getc (fp);*/
			*prmv++ = c = myread (fp);
		    while (c);
		    fclose (fp);
		    disp_message (7);
		}
		break;

	    case CTRL_O: 
	    case 'O': 
		if (are_you_sure () == 'Y') {
		    /* if ((fp = fopen ("CHESS.DAT", "wb")) == NULL) {*/
		    if ((fp = fopen ("CHESS.DAT", "w")) == NULL) {
			disp_message (5);
			beep ();
			beep ();
			beep ();
			beep ();
		    }
		    else {
			disp_message (8);

			prmv = &rmv[0].x_src;
			do
			    /* putc (c = *prmv++, fp);*/
			    mywrite (fp,c = *prmv++);
			while (c);
			fclose (fp);
			disp_message (9);
		    }
		}
		break;

	    case 'A': 
	    case 'B': 
	    case 'C': 
	    case 'D': 
	    case 'E': 
	    case 'F': 
	    case 'G': 
	    case 'H': 
		if (!g_end && ((n == 0) || (n == 2))) {
		    wprintw (aho, "%c", c);
		    wrefresh (aho);
		    *ps++ = c;
		    n++;
		    disp_message (0);
		}
		else
		    disp_message (10);
		break;

	    case '1': 
	    case '2': 
	    case '3': 
	    case '4': 
	    case '5': 
	    case '6': 
	    case '7': 
	    case '8': 
		if (!g_end && ((n == 1) || (n == 3))) {
		    wprintw (aho, "%c", c);
		    wrefresh (aho);
		    *ps++ = c;
		    n++;
		    disp_message (0);
		}
		else
		    disp_message (10);
		break;

	    default: 
		disp_message (10);
	}			/* switch */
    }				/* while */

 /* return (0); *//* dummy */
}

extern int  board[12][12],
            board0[12][12];

int     move_check (mv, white)	/* examine man move on board0[] */
        MOVE * mv;		/* 0; illegal,  1: legal */
int     white;
{
    int     xs,
            ys,
            xd,
            yd,
            is,
            id,
            dx,
            dy,
            x,
            y;

    xs = mv -> x_src;
    ys = mv -> y_src;
    xd = mv -> x_des;
    yd = mv -> y_des;

    if ((xs < X_a) || (xs > X_h) || (ys < Y_8) || (ys > Y_1)
	    || (xd < X_a) || (xd > X_h) || (yd < Y_8) || (yd > Y_1))
	return (0);
    is = board0[ys][xs];
    id = board0[yd][xd];

    if (white) {
	if ((is <= ID_SP) || (id > ID_SP))
	    return (0);
    }
    else {
	if ((is >= ID_SP) || (id < ID_SP))
	    return (0);
    }

    switch (is & ID_MOVE) {
	case ID_PAWN: 
	case ID_PAWN1: 
	    if (white) {
		if (yd == ys - 1) {
		    if ((xd == xs) && (id == ID_SP))
			return (1);
		    if (((id < ID_SP)
				|| (board0[ys][xd] == IDP_B1))
			    && ((xd == xs - 1) || (xd == xs + 1)))
			return (1);
		    return (0);
		}
		if ((yd == Y_4) && (ys == Y_2)
			&& (xd == xs) && (id == ID_SP)
			&& (board0[ys - 1][xs] == ID_SP))
		    return (1);
	    }
	    else {
		if (yd == ys + 1) {
		    if ((xd == xs) && (id == ID_SP))
			return (1);
		    if (((id > ID_SP)
				|| (board0[ys][xd] == IDP_W1))
			    && ((xd == xs - 1) || (xd == xs + 1)))
			return (1);
		    return (0);
		}
		if ((yd == Y_5) && (ys == Y_7)
			&& (xd == xs) && (id == ID_SP)
			&& (board0[ys + 1][xs] == ID_SP))
		    return (1);
	    }
	    return (0);

	case ID_KNIGHT: 
	case ID_KNIG1: 
	    if (((yd == ys - 2) || (yd == ys + 2))
		    && ((xd == xs - 1) || (xd == xs + 1)))
		return (1);
	    if (((yd == ys - 1) || (yd == ys + 1))
		    && ((xd == xs - 2) || (xd == xs + 2)))
		return (1);
	    return (0);

	case ID_BISHOP: 
	case ID_BISH1: 
	case ID_QUEEN: 
	case ID_QUEE1: 
	    dx = xd - xs;
	    dy = yd - ys;
	    x = xs;
	    y = ys;

	    if ((dy == dx) && (dx < 0)) {
		while (board0[--y][--x] == ID_SP);
		return (xd >= x);
	    }
	    if ((dy == -dx) && (dx > 0)) {
		while (board0[--y][++x] == ID_SP);
		return (xd <= x);
	    }
	    if ((dy == dx) && (dx > 0)) {
		while (board0[++y][++x] == ID_SP);
		return (xd <= x);
	    }
	    if ((dy == -dx) && (dx < 0)) {
		while (board0[++y][--x] == ID_SP);
		return (xd >= x);
	    }
	    if ((is & ID_PART) == ID_BISHOP)
		return (0);

	case ID_ROOK1: 
	case ID_ROOK: 
	    x = xs;
	    y = ys;

	    if ((yd == ys) && (xd > xs)) {
		while (board0[ys][++x] == ID_SP);
		return (xd <= x);

	    }
	    if ((yd == ys) && (xd < xs)) {
		while (board0[ys][--x] == ID_SP);
		return (xd >= x);
	    }

	    if ((xd == xs) && (yd > ys)) {
		while (board0[++y][xs] == ID_SP);
		return (yd <= y);
	    }
	    if ((xd == xs) && (yd < ys)) {
		while (board0[--y][xs] == ID_SP);
		return (yd >= y);
	    }
	    return (0);

	case ID_KING: 
	    if ((xd == X_c) && (yd == ys)
		    && (board0[ys][X_b] == ID_SP)
		    && (board0[ys][X_c] == ID_SP)
		    && (board0[ys][X_d] == ID_SP)) {
		if (white) {
		    if ((board0[ys][X_a] == IDR_W)
			    && (!under_b (&board[ys][X_c]))
			    && (!under_b (&board[ys][X_d]))
			    && (!under_b (&board[ys][X_e])))
			return (1);
		}


		else {
		    if ((board0[ys][X_a] == IDR_B)
			    && (!under_w (&board[ys][X_c]))
			    && (!under_w (&board[ys][X_d]))
			    && (!under_w (&board[ys][X_e])))
			return (1);
		}
	    }
	    else
		if ((xd == X_g) && (yd == ys)
			&& (board0[ys][X_f] == ID_SP)
			&& (board0[ys][X_g] == ID_SP)) {
		    if (white) {
			if ((board0[ys][X_h] == IDR_W)
				&& (!under_b (&board[ys][X_e]))
				&& (!under_b (&board[ys][X_f]))
				&& (!under_b (&board[ys][X_g])))
			    return (1);
		    }
		    else {
			if ((board0[ys][X_h] == IDR_B)
				&& (!under_w (&board[ys][X_e]))
				&& (!under_w (&board[ys][X_f]))
				&& (!under_w (&board[ys][X_g])))
			    return (1);
		    }
		}
	case ID_KING1: 
	    if (((yd == ys - 1) || (yd == ys) || (yd == ys + 1))
		    && ((xd == xs + 1) || (xd == xs) || (xd == xs - 1)))
		return (1);
	    return (0);
    }				/* switch */

    return (0);			/* dummy */
}
/* WINDOW * pass;*/
char    koma[7] = {
    /* ' ', 'L', 'N', 'B', 'K', 'Q', 'P'*/
    /* next is modified using disp_board */
    ' ', 'P', 'N', 'B', 'L', 'Q', 'K'
};

/* next init is not used */
int     init[2][9] = {{
	0, 1, 2, 3, 4, 5, 3, 2, 1
},
{
    0, 6, 6, 6, 6, 6, 6, 6, 6
}
};

void init_board () {
/* next is modified */
    int x,y;
    for (y=0;y<12;y++) {
    for (x=0;x<12;x++) {
	board[y][x]=board0[y][x]=board00[y][x];
    }
    }

    /* next is commented for using disp_board */
    /* those initialization of window is into main */
/*    pass = newwin (20, 37, 2, 4);
    drow_ban ();
    wrefresh (pass);
    drow_shoki ();
    wrefresh (pass);*/
}


drow_ban () {
    int     x,
            y,
            yy = 0;

    for (x = 3; x < 36; x += 4)
	for (y = 2; y < 17; y++) {
	    wmove (pass, y, x);
	    wprintw (pass, "|");
	}
    for (y = 1; y < 18; y += 2)
	for (x = 4; x < 35; x++)
	    if (!((x + 1) % 4) && y > 1 && y < 17) {
		wmove (pass, y, x);
		wprintw (pass, "+");
	    }
	    else {
		wmove (pass, y, x);
		wprintw (pass, "-");
	    }
    wmove (pass, 18, 5);
    wprintw (pass, "a   b   c   d   e   f   g   h");
    for (y = 8; y > 0; y--) {
	wmove (pass, yy += 2, 1);
	wprintw (pass, "%d", y);
    }
}

put_koma2(i, j, id, play)
int     i,
        j,
        id,
        play;
{
/* next is modified on 90-1-9 */
/* 	put_koma(i-1,j-1,id,play);*/
	put_koma(i-1,10-j,id,play);
}

put_koma (i, j, id, play)
int     i,
        j,
        id,
        play;
{
    if (i > 0 && i < 9 && j > 0 && j < 9)
	if (play == 1) {
	    wmove (pass, 16 - (j - 1) * 2, (i - 1) * 4 + 5);
	    /* next modified using (id <> 0) */
	    if (id != 0) wstandout (pass);
	    wprintw (pass, "%c", koma[id]);
	    /* next modified using (id <> 0) */
	    if (id != 0) wstandend (pass);
	}
	else {
	    wmove (pass, 16 - (j - 1) * 2, (i - 1) * 4 + 5);
	    wprintw (pass, "%c", koma[id]);
	}
    wrefresh (pass);
}

drow_shoki () {
    int     x,
            y;

    for (y = 8; y > 0; y--)
	for (x = 1; x < 9; x++) {
	/* next modified on 1 <--> 0 in fourth argument */
	    if (y == 8)
		put_koma (x, y, init[0][x], 0);
	    if (y == 7)
		put_koma (x, y, init[1][x], 0);
	    if (y == 2)
		put_koma (x, y, init[1][x], 1);
	    if (y == 1)
		put_koma (x, y, init[0][x], 1);
	}
}

/* #define ABS(a,b)    (((a)>(b))?(a)-(b):(b)-(a)) */

extern int  board[12][12],
            board0[12][12];
extern  MOVE rmv[];

void disp_piece (mv)		/* display move piece */
MOVE * mv;
{
    int     xs,
            ys,
            is,
            xd,
            yd,
            id,
            yy,
            ii;
    MOVE mv1;

    xs = mv -> x_src;
    ys = mv -> y_src;
    is = mv -> i_src;
    xd = mv -> x_des;
    yd = mv -> y_des;
    id = mv -> i_des;


    disp_box (xs, ys, ID_SP);
    wait (1000);
    disp_box (xs, ys, is);
    wait (1000);
    disp_box (xs, ys, ID_SP);
    wait (1000);
    disp_box (xs, ys, is);
    wait (1000);
    disp_box (xs, ys, ID_SP);

    disp_box (xd, yd, id);
    wait (1000);
    disp_box (xd, yd, ID_SP);
    wait (1000);
    disp_box (xd, yd, id);
    wait (1000);
    disp_box (xd, yd, ID_SP);
    wait (1000);
    disp_box (xd, yd, id);

    if (((is == IDP_B) && (board0[yy = yd - 1][xd] == (ii = IDP_W1)))
	    || ((is == IDP_W) && (board0[yy = yd + 1][xd] == (ii = IDP_B1)))) {
	disp_box (xd, yy, ID_SP);
	wait (1000);
	disp_box (xd, yy, ii);
	wait (1000);
	disp_box (xd, yy, ID_SP);
    }

    if (((is & ID_PART) == ID_KING) && (ABS (xd, xs) == 2)) {
	mv1.x_src = ((xd == X_c) ? X_a : X_h);
	mv1.y_src = ((is == IDK_B) ? Y_8 : Y_1);
	mv1.i_src = ((is == IDK_B) ? IDR_B : IDR_W);
	mv1.x_des = ((xd == X_c) ? X_d : X_f);
	mv1.y_des = ((is == IDK_B) ? Y_8 : Y_1);
	mv1.i_des = ((is == IDK_B) ? IDR_B1 : IDR_W1);

	disp_piece (&mv1);
    }
}
void up_board (mv)		/* update board[] */
MOVE * mv;
{
    switch (mv -> i_src & ID_SIDE) {
	case IDP_B: 
	    if (board[mv -> y_des][mv -> x_des] == ID_SP)
		board[mv -> y_des - 1][mv -> x_des] = ID_SP;
	    break;
	case IDP_W: 
	    if (board[mv -> y_des][mv -> x_des] == ID_SP)
		board[mv -> y_des + 1][mv -> x_des] = ID_SP;
	    break;
	case IDK_B: 
	    if (ABS (mv -> x_des, mv -> x_src) == 2) {
		if (mv -> x_des == X_c) {
		    board[Y_8][X_a] = ID_SP;
		    board[Y_8][X_d] = IDR_B1;
		}
		else
		    if (mv -> x_des == X_g) {
			board[Y_8][X_h] = ID_SP;
			board[Y_8][X_f] = IDR_B1;
		    }
	    }
	    break;
	case IDK_W: 
	    if (ABS (mv -> x_des, mv -> x_src) == 2) {
		if (mv -> x_des == X_c) {
		    board[Y_1][X_a] = ID_SP;
		    board[Y_1][X_d] = IDR_W1;
		}
		else
		    if (mv -> x_des == X_g) {
			board[Y_1][X_h] = ID_SP;
			board[Y_1][X_f] = IDR_W1;
		    }
	    }
    }				/* switch */

    board[mv -> y_src][mv -> x_src] = ID_SP;
    board[mv -> y_des][mv -> x_des] = mv -> i_des;
}





void disp_move (mv, white)	/* display move */
MOVE * mv;
int     white;
{

    wmove(aho, (white ? Y_W_MOVE:Y_B_MOVE), X_MOVE); 

    if (((mv -> i_src & ID_PART) == ID_KING)
	    && (ABS (mv -> x_des, mv -> x_src) == 2)) {
	if (mv -> x_des == X_c)
	    /* printw ("0-0-0");*/
	    wprintw (aho,"0-0-0");
	else
	    /* printw (" 0-0");*/
	    wprintw (aho," 0-0");
	/* refresh ();*/
	wrefresh (aho);
    }
    else {
	wprintw (aho, "%c", 'A' + mv -> x_src - 2);
	wprintw (aho, "%c", '8' - mv -> y_src + 2);
	wprintw (aho, "%c", (board0[mv -> y_des][mv -> x_des] == ID_SP) ? '-' : 'x');
	wprintw (aho, "%c", 'A' + mv -> x_des - 2);
	wprintw (aho, "%c", '8' - mv -> y_des + 2);
	wrefresh (aho);
    }
}

void record_move (mv, nmove)	/* record move */
MOVE * mv;
int     nmove;
{
    rmv[nmove].x_src = mv -> x_src;
    rmv[nmove].y_src = mv -> y_src;
    rmv[nmove].i_src = mv -> i_src;
    rmv[nmove].x_des = mv -> x_des;
    rmv[nmove].y_des = mv -> y_des;
    rmv[nmove].i_des = mv -> i_des;

    rmv[nmove + 1].x_src = 0;
}


extern int  board[12][12],
            board0[12][12],
            board00[12][12];
extern int  man_white;
extern long com_time;
extern  MOVE rmv[];

void wait (t)			/* wait */
int     t;
{
    int     i,
            j;

    for (i = 0; i < t; i++)
	for (j = 0; j < 16; j++);
}

void b2board (p0, p1)		/* copy board to board */
int    *p0,
       *p1;			/* &board[Y_8][X_a] or &board0[Y_8][X_a] 
				*/
{
    int     i;

    for (i = 0; i < 92; i++)
	*p0++ = *p1++;
}

void disp_time (tm, white)	/* display comsumption time */
long    tm;
int     white;
{
    int     h,
            m,
            s;


    wmove (aho, (white ? Y_W_TIME : Y_B_TIME), X_TIME);

    s = tm % 60;
    h = tm / 60;
    m = h % 60;
    h = h / 60;
    wprintw (aho, "%02d:%02d:%02d", h, m, s);
}

void reset2pawn (white)		/* reset 2 steps PAWN on boardo[] */
int     white;
{
    int     x;

    if (white) {
	for (x = X_a; x <= X_h; x++)
	    if (board0[Y_4][x] == IDP_W1)
		board0[Y_4][x] == IDP_W;
    }
    else {
	for (x = X_a; x <= X_h; x++)
	    if (board0[Y_5][x] == IDP_B1)
		board0[Y_5][x] == IDP_B;
    }
}

void disp_check (white)		/* display check message */
int     white;
{

    wmove (aho, Y_MESSAGE, X_MESSAGE);

    if ((white && check_w ()) || (!white && check_b ())) {
	wprintw (aho, ("Check !	            "));
	wmove (aho, X_MOVE + 5, (white ? Y_W_MOVE : Y_B_MOVE));
	wprintw (aho, "+");
	wrefresh (aho);
	wait (600);
	wait (600);
    }
    else {
	wprintw (aho, ("		 "));
	wrefresh (aho);
    }
}

void disp_prompt (white, player, nmove)/* display prompt */
int     white,
        player,
        nmove;
{

    wmove (aho, (white ? Y_W_MOVE : Y_B_MOVE), X_MOVE - 12);

    if (nmove < 0)
	nmove = 0;

    if (player)
	wprintw (aho, "%3d  PLAYER=          ", nmove / 2 + 1);
    else
	wprintw (aho, "%3d  COMPUT=          ", nmove / 2 + 1);
    wrefresh (aho);

 /* locate (X_MOVE, (white ? Y_W_MOVE : Y_B_MOVE)); */
    wmove (aho, (white ? Y_W_MOVE : Y_B_MOVE), X_MOVE);
}

void disp_last (nmove)		/* display last board */
int     nmove;			/* & make board[] & board0[] */
{
    int     i;

    init_board ();

    for (i = 0; i < nmove; i++)
	up_board (&rmv[i]);

    b2board (&board0[Y_8][X_a], &board[Y_8][X_a]);
 /* make last board0[] */

    disp_prompt (!(nmove & 1), COMPUTER, nmove);
    disp_time (com_time, !(nmove & 1));

    if (nmove >= 0) {
	disp_move (&rmv[nmove], !(nmove & 1));

	up_board (&rmv[nmove]);
    }
    b2board (&board0[Y_8][X_a], &board[Y_8][X_a]);

    disp_board ();
}



#define BRQ0(dp)   for(p=pb; *(p += (dp)) == ID_SP; ); i0 = *p & ID_SIDE;
#define BRQ1(i1,i2)   if ((i0 == i1) || (i0 == i2)) return(1);

extern int  board[12][12];

int     under_b (pb)		/* under black piece effect on board[] */
int    *pb;			/* 0:no effect, 1:under effect */
{
    int     i0,
           *p;

    if (((*(pb - 13) & ID_SIDE) == IDP_B) || ((*(pb - 11) & ID_SIDE) == IDP_B))
	return (1);

    if (((*(pb - 25) & ID_SIDE) == IDN_B) || ((*(pb - 23) & ID_SIDE) == IDN_B)
	    || ((*(pb - 14) & ID_SIDE) == IDN_B) || ((*(pb - 10) & ID_SIDE) == IDN_B)
	    || ((*(pb + 14) & ID_SIDE) == IDN_B) || ((*(pb + 10) & ID_SIDE) == IDN_B)
	    || ((*(pb + 25) & ID_SIDE) == IDN_B) || ((*(pb + 23) & ID_SIDE) == IDN_B))
	return (1);

    BRQ0 (-13) BRQ1 (IDQ_B, IDB_B)
	BRQ0 (-11) BRQ1 (IDQ_B, IDB_B)
	BRQ0 (11) BRQ1 (IDQ_B, IDB_B)
	BRQ0 (13) BRQ1 (IDQ_B, IDB_B)
	BRQ0 (-12) BRQ1 (IDQ_B, IDR_B)
	BRQ0 (-1) BRQ1 (IDQ_B, IDR_B)
	BRQ0 (1) BRQ1 (IDQ_B, IDR_B)
	BRQ0 (12) BRQ1 (IDQ_B, IDR_B)
	if (((*(pb - 13) & ID_SIDE) == IDK_B) || ((*(pb - 12) & ID_SIDE) == IDK_B)
	    || ((*(pb - 11) & ID_SIDE) == IDK_B) || ((*(pb - 1) & ID_SIDE) == IDK_B)
	    || ((*(pb + 1) & ID_SIDE) == IDK_B) || ((*(pb + 11) & ID_SIDE) == IDK_B)
	    || ((*(pb + 12) & ID_SIDE) == IDK_B) || ((*(pb + 13) & ID_SIDE) == IDK_B))
	return (1);

    return (0);
}


int     under_w (pb)		/* under white piece effect on board[] */
int    *pb;			/* 0:no effect, 1:under effect */
{
    int     i0,
           *p;

    if (((*(pb + 13) & ID_SIDE) == IDP_W) || ((*(pb + 11) & ID_SIDE) == IDP_W))
	return (1);

    if (((*(pb + 25) & ID_SIDE) == IDN_W) || ((*(pb + 23) & ID_SIDE) == IDN_W)
	    || ((*(pb + 14) & ID_SIDE) == IDN_W) || ((*(pb + 10) & ID_SIDE) == IDN_W)
	    || ((*(pb - 14) & ID_SIDE) == IDN_W) || ((*(pb - 10) & ID_SIDE) == IDN_W)
	    || ((*(pb - 25) & ID_SIDE) == IDN_W) || ((*(pb - 23) & ID_SIDE) == IDN_W))
	return (1);

    BRQ0 (13) BRQ1 (IDQ_W, IDB_W)
	BRQ0 (11) BRQ1 (IDQ_W, IDB_W)
	BRQ0 (-11) BRQ1 (IDQ_W, IDB_W)
	BRQ0 (-13) BRQ1 (IDQ_W, IDB_W)
	BRQ0 (12) BRQ1 (IDQ_W, IDR_W)
	BRQ0 (1) BRQ1 (IDQ_W, IDR_W)
	BRQ0 (-1) BRQ1 (IDQ_W, IDR_W)
	BRQ0 (-12) BRQ1 (IDQ_W, IDR_W)
	if (((*(pb + 13) & ID_SIDE) == IDK_W) || ((*(pb + 12) & ID_SIDE) == IDK_W)
	    || ((*(pb + 11) & ID_SIDE) == IDK_W) || ((*(pb + 1) & ID_SIDE) == IDK_W)
	    || ((*(pb - 1) & ID_SIDE) == IDK_W) || ((*(pb - 11) & ID_SIDE) == IDK_W)
	    || ((*(pb - 12) & ID_SIDE) == IDK_W) || ((*(pb - 13) & ID_SIDE) == IDK_W))
	return (1);

    return (0);
}

int     check_w () {		/* Eexamine white check *//* out :  0: no
				   check,    1: check */
    int    *p;

    for (p = &board[Y_8][X_a]; (*p & ID_SIDE) != IDK_B; p++);
    return (under_w (p));
}

int     check_b () {		/* Eexamine black check *//* out :   0: no
				   check,  1: check */
    int    *p;

    for (p = &board[Y_1][X_h]; (*p & ID_SIDE) != IDK_W; p--);
    return (under_b (p));
}
void ogata () {
    erase ();
    move (0, 0);
    refresh ();
    endwin ();
    exit (0);
}

setblock (fd, on)		/* set non-blocking terminal */
int     fd,
        on;
{
    static int  blockf,
                nonblockf;
    static int  first = 1;
    int     flags;

    if (first == 1) {
	first = 0;
	if ((flags = fcntl (fd, F_GETFL, 0)) == -1) {
	    fprintf (stderr, "setblock err\n");
	    exit (1);
	}
	blockf = flags & ~O_NDELAY;
	nonblockf = flags | O_NDELAY;
    }
    if (fcntl (fd, F_SETFL, on ? blockf : nonblockf) == -1) {
	fprintf (stderr, "file control err\n");
	exit (1);
    }
}

static char cbuf = '\0';

cready () {
    int     n;

 /* nl(); noraw(); noecho(); crmode(); */
    if (cbuf != '\0')
	return (1);
    setblock (0, 0);
    n = read (0, &cbuf, 1);
    setblock (0, 1);
    switch (n) {
	case -1: 
	    return (0);
	default: 
	    return (1);
    }
}

cget () {
    char    c;

    noecho ();
    crmode ();
    if (cbuf != '\0') {
	c = cbuf;
	cbuf = '\0';
	return (c & 0377);
    }
    else
	return (getch ());
}

baka (o)
char   *o;
{
    wmove (aho, 1, 40);
    wprintw (aho, "baka%s", o);
    wrefresh (aho);
}
void disp_board_fp (depth,fp) 
    FILE *fp;
    {
    int     x,
            y,
           *p;


    p = &board[Y_8][X_a];
    for (y = Y_8; y <= Y_1; y++, p += 4){
	depthindent(depth,fpev);
	for (x = X_a; x <= X_h; x++, p++)
		fprintf(fp," %d",*p);
	fprintf(fp,"\n");
	}
    fprintf(fp,"\n");

}

evaluate_advb(ps,pd,depth,palpha,beta)
int *ps,*pd,depth,*palpha,beta;
{
FILE *fpev;
int ev;

ev=evaluate();
if (evalmask){
   if ((fpev = fopen ("CHESS.EVAL", "a")) == NULL) {
		disp_message (5);
		beep ();
		beep ();
		beep ();
		beep ();
	    }
	    else {
		disp_message (11);
		depthindent(depth,fpev);
		fprintf(fpev,"Evaluation in advb\n");
		depthindent(depth,fpev);
		fprintf(fpev,"  *ps=%d *pd=%d depth=%d *palpha=%d beta=%d\n",*ps,pd,depth,*palpha,beta);
		disp_board_fp(depth,fpev);
		depthindent(depth,fpev);
		fprintf(fpev,"  Eval=%d\n\n",ev);
		fclose(fpev);
		}
	}
return(ev);
}

evaluate_advw(ps,pd,depth,alpha,pbeta)
int *ps,*pd,depth,alpha,*pbeta;
{
FILE *fpev;
int ev;

ev=evaluate();
if (evalmask){
   if ((fpev = fopen ("CHESS.EVAL", "a")) == NULL) {
		disp_message (5);
		beep ();
		beep ();
		beep ();
		beep ();
	    }
	    else {
		disp_message (11);
		depthindent(depth,fpev);
		fprintf(fpev,"Evaluation in advw\n");
		depthindent(depth,fpev);
		fprintf(fpev,"  *ps=%d *pd=%d depth=%d alpha=%d *pbeta=%d\n",*ps,pd,depth,alpha,*pbeta);
		disp_board_fp(depth,fpev);
		depthindent(depth,fpev);
		fprintf(fpev,"  Eval=%d\n\n",ev);
		fclose(fpev);
		}
	}
return(ev);
}

evaluate_max_black(num,depth,alpha,beta)
int num,depth,alpha,beta;
{
FILE *fpev;
int ev;

ev=evaluate();
if (evalmask){
   if ((fpev = fopen ("CHESS.EVAL", "a")) == NULL) {
		disp_message (5);
		beep ();
		beep ();
		beep ();
		beep ();
	    }
	    else {
		disp_message (11);
		depthindent(depth,fpev);
		fprintf(fpev,"Evaluation in max_black case of %d\n",num);
		depthindent(depth,fpev);
		fprintf(fpev,"  depth=%d alpha=%d beta=%d\n",depth,alpha,beta);
		disp_board_fp(depth,fpev);
		depthindent(depth,fpev);
		fprintf(fpev,"  Eval=%d\n\n",ev);
		fclose(fpev);
		}
	}
return(ev);
}

evaluate_min_white(num,depth,alpha,beta)
int num,depth,alpha,beta;
{
FILE *fpev;
int ev;

ev=evaluate();
if (evalmask){
   if ((fpev = fopen ("CHESS.EVAL", "a")) == NULL) {
		disp_message (5);
		beep ();
		beep ();
		beep ();
		beep ();
	    }
	    else {
		disp_message (11);
		depthindent(depth,fpev);
		fprintf(fpev,"Evaluation in min_white case of %d\n",num);
		depthindent(depth,fpev);
		fprintf(fpev,"  depth=%d alpha=%d beta=%d\n",depth,alpha,beta);
		disp_board_fp(depth,fpev);
		depthindent(depth,fpev);
		fprintf(fpev,"  Eval=%d\n\n",ev);
		fclose(fpev);
		}
	}
return(ev);
}

evalprint_mess(mess,depth,alpha,beta)
int depth,alpha,beta;
char *mess;
{
FILE *fpev;

if (evalmask){
   if ((fpev = fopen ("CHESS.EVAL", "a")) == NULL) {
		disp_message (5);
		beep ();
		beep ();
		beep ();
		beep ();
	    }
	    else {
		disp_message (11);
		depthindent(depth,fpev);
		fprintf(fpev," %s ==> ",mess);
		fprintf(fpev,"  depth=%d alpha=%d beta=%d\n",depth,alpha,beta);
		fclose(fpev);
		}
	}
}

depthindent(depth,fpev)
int depth;
FILE *fpev;
{
int i;
for (i=0;i<depth;i++) fprintf(fpev,"    ");
}

disp_move_fp(depth,ps,pd)
int depth,*ps,*pd;
{
FILE *fpev;

if (evalmask){
   if ((fpev = fopen ("CHESS.EVAL", "a")) == NULL) {
		disp_message (5);
		beep ();
		beep ();
		beep ();
		beep ();
	    }
	    else {
		disp_message (11);
		depthindent(depth,fpev);
		fprintf (fpev,"Move:%c", 'A' + (ps - &board[0][0]) % 12 - 2);
		fprintf (fpev,"%c", '8' - (ps - &board[0][0]) / 12 + 2);
		fprintf (fpev," %c", 'A' + (pd - &board[0][0]) % 12 - 2);
		fprintf (fpev,"%c\n", '8' - (pd - &board[0][0]) / 12 + 2);
		fclose(fpev);
		}
	}
}

