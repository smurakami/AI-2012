/*  notice modified on 2-17 1988 */
/*  move1 -> mmmove1   move2 -> mmmove2	*/
struct mmmove1 {
		int x_src;
		int y_src;
		int i_src;
		int x_des;
		int y_des;
		int i_des;
	};
struct mmmove2 { 
		int *p_src;
		int *p_des;
		int i_des;
	};

typedef struct mmmove1 MOVE;
typedef struct mmmove2 PMOVE;
