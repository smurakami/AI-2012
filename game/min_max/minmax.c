#include "defs.h"

main()
{

    struct state *init(),*bag1;
    struct state *st0;
    
    st0=init();


/*
    st0->ban[0][1]=MINE;
    st0->ban[1][1]=YOURS;
*/

    bag1=init();
    expand(st0,MAXDEPTH,MAXMODE,1,bag1);
    free_bags(bag1); 
    minimax(st0,MAXMODE);
    printf("Showing ... \n");
    show(st0,0);
    printf("Playing ... \n");
    playhand(st0,MAXMODE);
}

show(st,d)
struct state *st;
int d;
{
    struct state *w;

    printf("Depth -> %d\n",d);
    print(st);
    if (st->daut!=NULL) 
	for (w=st->daut; w!=NULL; w=w->sist)
	        show(w,d+1);

}

expand(st,depth,mode,judge,bags)
struct state *st,*bags;
int depth, mode,judge;
{
    register int i,j;
    struct state *w, *copy(),*init();
    struct state *newbags;
    struct state *newbags2;


    if (st==NULL) return;
    if (depth<=0 || check(st)!=CONT) return;


    printf("Expanding depth=%d mode=%d... \n",depth,mode);
    print(st);

    if (st->daut==NULL)
	for(i=0; i<3; i++) 
	    for(j=0; j<3; j++) {
		switch (move(i,j,w=copy(st),mode,depth,judge,bags)) {
		case DISABLE:
		    stfree(w);
		    continue;
		case ENABLE:
		    printf("ENABLE with i=%d,j=%d\n",i,j); 
		    print(w);
		    newbags=init();
		    expand(w,depth-1,TOGGLE(mode),judge,newbags);
		    free_bags(newbags); 
		}
		w->sist = st->daut;
		st->daut = w;
	    }
    else {
	for (w=st->daut; w!=NULL; w=w->sist) {
	    newbags2=init();
/*	    expand(w,depth-1,TOGGLE(mode),judge,newbags2); */
	    expand(w,1,TOGGLE(mode),judge,newbags2);
	    free_bags(newbags2); 
	}
    }


}

playhand(st,mode)
struct state *st;
int mode;
{
    struct state *p,*w,*t;
    register int i=0;

    for (p=st; p!=NULL; p=p->daut) {
	if (check(p)!=CONT) {
	    switch(check(p)) {
		case MYWIN: printf("I win\n"); break;
		case YOUWIN: printf("You win\n"); break;
		case DRAWN: printf("Draw\n"); break;
		}
	    break;
	}
	minimax(p,mode);
	for (w=p->daut, p->daut=NULL; w!=NULL; w=t) {
	    t=w->sist;
	    w->sist = NULL;
	    if (w->eval==p->eval && p->daut==NULL)
		p->daut =w;
	    else stfree(w);
	}
	mode = TOGGLE(mode);
	printf("EXP:\n"); 
	if (p->daut==NULL) printf("NULL"); 
	else print(p->daut); 
	expand(p->daut,1,mode,0,NULL);
    }
    printf("Answering...\n",i);
    for (p=st; p!=NULL; p=p->daut) {
	printf("(%d)\n",i);
	print(p);
	i++;
    }
}

minimax(st,mode)
struct state *st;
int mode;
{
    register struct state *w;

    if (st==NULL) return;
    if (st->daut==NULL) {
/*	st->eval = evaluate(st); */
/*	printf("Eval=%d\n",st->eval); */
    }
    else {
	st-> eval = (mode==MAXMODE)?MININT:MAXINT;
	for (w=st->daut; w!=NULL; w=w->sist) {
	    minimax(w,TOGGLE(mode));
	    if ((mode==MAXMODE && w->eval > st->eval) ||
		(mode==MINMODE && w->eval < st->eval))
		st->eval = w->eval;
	}
    }
}

