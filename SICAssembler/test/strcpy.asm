STRCPY:	START	0
	LDX	ZERO
L1:	LDCH	DATA1,X
	STCH	DATA2,X
	USE	INIT
ZERO:	WORD	0
DATA1:	BYTE	C'HI'
COUNT:	WORD	2
DATA2:	RESB	2
	USE
	TIX	COUNT
	JLT	L1
	RSUB
	END

