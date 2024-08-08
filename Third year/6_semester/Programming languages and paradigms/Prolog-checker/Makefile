verify: verify.sav
	spld --static --exechome=/opt/sicstus/sicstus4.8.0/bin/ verify.sav -o verify
verify.sav: az430277.pl
	echo "compile('az430277.pl'). save_program('verify.sav')." | sicstus
