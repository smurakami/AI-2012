#define ID_PAWN         1	/* PAWN */
#define ID_KNIGHT       2	/* KNIGHT */
#define ID_BISHOP       3	/* BISHOP */
#define ID_ROOK         4	/* ROOK */
#define ID_QUEEN        5	/* QUEEN */
#define ID_KING         6	/* KING */

#define ID_PART         7	/* ID without side,moved mark */
#define ID_MOVE         8 + ID_PART/* ID with moved mark, without side */
#define ID_SIDE         16+ ID_PART/* ID with side mark, without moved */
#define ID_PAWN1        8 + ID_PAWN/* 2 stops moved PAWN */
#define ID_KNIG1        8 + ID_KNIGHT/* moved KNIGHT */
#define ID_BISH1        8 + ID_BISHOP/* moved BISHOP */
#define ID_ROOK1        8 + ID_ROOK/* moved ROOK */
#define ID_QUEE1        8 + ID_QUEEN/* moved QUEEN */
#define ID_KING1        8 + ID_KING/* moved KING */

#define ID_OB           0	/* out of board */

#define IDP_B           ID_PAWN	/* black PAWN:    7634-7637 */
#define IDN_B           ID_KNIGHT/* black KNIGHT:  7638-763b */
#define IDB_B           ID_BISHOP/* black BISHOP:  763c-763f */
#define IDR_B           ID_ROOK	/* black ROOK:    7640-7643 */
#define IDQ_B           ID_QUEEN/* black QUEEN:   7644-7647 */
#define IDK_B           ID_KING	/* black KING:    7648-764b */

#define ID_SP           16	/* space:         7630-7633 */

#define IDP_W           16 + ID_PAWN/* white PAWN:    764c-764f */
#define IDN_W           16 + ID_KNIGHT/* white KNIGHT:  7650-7653 */
#define IDB_W           16 + ID_BISHOP/* white BISHOP:  7654-7657 */

#define IDR_W   16 + ID_ROOK	/* white ROOK:   7658-765b */
#define IDQ_W   16 + ID_QUEEN	/* white QUEEN:  765c-765f */
#define IDK_W   16 + ID_KING	/* white KING:   7660-7663 */

#define IDP_B1  8 + IDP_B	/* 2 steps moved PAWN */
#define IDN_B1  8 + IDN_B	/* moved KNIGHT */
#define IDB_B1  8 + IDB_B	/* moved BISHOP */
#define IDR_B1  8 + IDR_B	/* moved ROOK */
#define IDQ_B1  8 + IDQ_B	/* moved QUEEN */
#define IDK_B1  8 + IDK_B	/* moved KING */
#define IDP_W1  8 + IDP_W	/* 2 steps moved PAWN */
#define IDN_W1  8 + IDN_W	/* moved KNIGHT */
#define IDB_W1  8 + IDB_W	/* moved BISHOP */
#define IDR_W1  8 + IDR_W	/* moved ROOK */
#define IDQ_W1  8 + IDQ_W	/* moved QUEEN */
#define IDK_W1  8 + IDK_W	/* moved KING */

#define X_a     2		/* board position */
#define X_b     3 
#define X_c     4
#define X_d     5
#define X_e     6
#define X_f     7
#define X_g     8
#define X_h     9

#define Y_8     2
#define Y_7     3
#define Y_6     4

#define Y_5		5
#define Y_4		6
#define Y_3		7
#define Y_2		8
#define Y_1		9

#define CTRL_E	        5	/* key code */
#define CTRL_R		18
#define CTRL_T		20
#define CTRL_Y		25
#define CTRL_U		21
#define CTRL_I		9
#define CTRL_O		15
#define KEY_BS		8
/* further revised  2012 7 4 */
#define KEY_CR		13    /*  revised on 3/12  from  13 */

#define X_MESSAGE	45
#define Y_MESSAGE	12
#define X_TIME		44
#define X_MOVE		53
#define Y_B_TIME	5
#define Y_B_MOVE	3
#define Y_W_TIME	16
#define Y_W_MOVE	18
#define X_LEVEL		45
#define Y_LEVEL		9

#define BLACK		0	/* board sides */
#define WHITE		1
#define COMPUTER	0
#define PLAYER		1

#define MAX_DEPTH	12	/* limit of depth */
