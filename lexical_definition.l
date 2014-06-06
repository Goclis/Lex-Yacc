
cmm

%
ABCDEFGHIJKLMNOPQRSTUVWXYZ
abcdefghijklmnopqrstuvwxyz_
0123456789
\t\n\s\r
+-*/
.|&
^
\%
<>=!
;,()[]{}
"'
#


%
	digit~0		1|2|3|4|5|6|7|8|9
	digit		0|@digit~0@
	letter_		A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|_
	
$	integer		0|@digit~0@.@digit@*
$	NUMBER		@integer@|@integer@*.\..@integer@*
$	IDENTIFIER	@letter_@.(@letter_@|@digit@)*


?	=	=

?	+	+
?	-	-
?	*	\*
?	/	/
?	+=	+.=
?	-=	-.=
?	*=	\*.=
?	/=	/.=
?	==	=.=

?	&&	&.&
?	||	\|.\|
?	!	!


?	<	<
?	>	>
?	<=	<.=
?	>=	>.=



?	(	\(
?	)	\)
?	[	[
?	]	]
?	{	{
?	}	}

?	'	'
?	"	"

?	,		,
?	;		;

?	if		i.f
?	else		e.l.s.e
?	while		w.h.i.l.e
?	do		d.o
?	return		r.e.t.u.r.n

?	int		i.n.t
?	char		c.h.a.r
?	bool		b.o.o.l
?	float		f.l.o.a.t
?	double		d.o.u.b.l.e
?	void		v.o.i.d

?	true		t.r.u.e
?	false		f.a.l.s.e




~	white_space	(\t|\n|\s|\r)*

%