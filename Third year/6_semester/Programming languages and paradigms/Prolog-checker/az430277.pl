% Adam Zembrzuski 430277

% Reprezentacja stanu: state(Vars, Arrs, CIRs)
% --------------------------------------------
% Vars - przechowuje wartości zmiennych,
% Arrs - przechowuje wartości zmiennych tablicowych,
% CIRs - przechowuje licznik rozkazów dla procesów.
:- ensure_loaded(library(lists)).
:- op(700, xfx, '<>').

% verify(+N, +Program)
verify(N, File) :-
    \+ checkN(N),
    \+ openFile(File),
    readProgram(Program),
    seen,
    initState(Program, N, State),
    checkState(Program, State, [], [], _),
    % Wszystkie testy zwróciły true.
    format('Program jest poprawny (bezpieczny).~n', []), !.

% initState(+Program, +N, -StanPoczątkowy)
initState(termprog(VIds,AIds,_), N, state(Vars, Arrs, CIRs)) :-
    initMap(VIds, 0, Vars),
    initList(N, 0, L),
    initMap(AIds, L, Arrs),
    initList(N, 1, CIRs).

% step(+Program, +StanWe, ?PrId, -StanWy)
step(termprog(_, _, Inss), SIn, PrId, SOut) :-
    checkCIR(PrId, SIn, CIR),
    nth1(CIR, Inss, Ins),
    evalIns(Ins, SIn, PrId, SOut).

% checkState(+Program, +State, +VisitedIn, +Path, -VisitedOut)
% -------------------------------------------------------------
% Sprawdza czy z obecnego stanu można dojść tylko do poprawnych
% stanów.
checkState(_, S, VIn, _, VIn) :- member(S, VIn).
checkState(P, S, VIn, Path, VOut) :-
    \+ notSafe(P, S, Path),
    \+ member(S, VIn),
    nextStates(P, S, Nexts),
    checkNexts(P, S, [S|VIn], Nexts, Path, VOut).

% checkNexts(+Program, +State, +VisitedIn, +Nexts, +Path, -VisitedOut)
checkNexts(_, _, V, [], _, V).
checkNexts(P, S, VIn, [N-CIR-SOut|Nexts], Path ,VOut) :-
    checkState(P, SOut, VIn, [N-CIR|Path], V),
    checkNexts(P, S, V, Nexts, Path, VOut).

% notSafe(+Program, +State, +Path)
% --------------------------------
% Sprawdza czy program jest bezpieczny
% przy obecnym stanie.
notSafe(termprog(_,_,Inss), state(_,_,CIRs), Path) :-
    inSekcja(0, Inss, CIRs, [X, Y|Arr]),
    reverse([X, Y|Arr], Arr2),
    printNiePoprawny(Arr2, Path).

% printNiePoprawny(+Arr, +Path)
printNiePoprawny([X|Arr], Path1) :-
    format('Program jest niepoprawny.~n', []),
    format('Niepoprawny przeplot:~n', []),
    reverse(Path1, Path2),
    printPath(Path2),
    format('Procesy w sekcji: ~w', X),
    printInSekcja(Arr),
    format('.~n', []). 

% printPath(+Path)
printPath([]).
printPath([N-CIR|Path]) :-
    format('    Proces ~d: ~d~n', [N, CIR]),
    printPath(Path).

% printInSekcja(+Arr)
printInSekcja([]).
printInSekcja([X|Arr]) :- format(', ~w', [X]), printInSekcja(Arr).

% nextStates(+Program, +State, -Nexts)
nextStates(P, state(Vars, Arrs, CIRs), Nexts) :-
    nextStates(0, P, state(Vars, Arrs, CIRs), CIRs, Nexts).
nextStates(_, _, _, [], []).
nextStates(N1, P, SIn, [CIR|CIRs], [N1-CIR-SOut|Nexts]) :-
    step(P, SIn, N1, SOut),
    N2 is N1 + 1,
    nextStates(N2, P, SIn, CIRs, Nexts).

% inSekcja(+N, +Inss, +CIRs, -Arr)
% --------------------------------
% Sprawdza, które procesy wejdą do
% sekcji krytycznej.
inSekcja(_,_,[],[]).
inSekcja(N1, Inss, [CIR|CIRs], [N1|Arr]) :-
    nth1(CIR,Inss,sekcja),
    N2 is N1 + 1,
    inSekcja(N2, Inss, CIRs, Arr).
inSekcja(N1, Inss, [CIR|CIRs], Arr) :-
    nth1(CIR, Inss, Elem),
    Elem \= sekcja,
    N2 is N1 + 1,
    inSekcja(N2, Inss, CIRs, Arr).

% checkN(+N)
checkN(N) :-
    \+ (integer(N), N >= 1), 
    format('Error: parametr ~w powinien byc liczba > 0~n', [N]). 

% openFile(+File)
openFile(File) :-
    set_prolog_flag(fileerrors, off), 
    \+ (atom(File), see(File)), 
    format('Error: brak pliku o nazwie - ~w~n', [File]).

% readProgram(-Program)
readProgram(termprog(VIds, AIds, Inss)) :-
    read(variables(VIds)),
    read(arrays(AIds)),
    read(program(Inss)).

% initList(+N, +Value, -List)
initList(0, _, []) :- !.
initList(N, V, [V|L]) :- N > 0, N1 is N - 1, initList(N1, V, L).

% initMap(+Ids, +Value, -Map)
initMap([], _, []).
initMap([Id|Ids], V, [Id-V|M]) :- initMap(Ids, V, M).

% updateMap(+Id, +OldMap, +Value, -NewMap)
updateMap(Id, OM, V, NM) :- select(Id-_, OM, Id-V, NM).

% updateVar(+Id, +StanWe, +Value, -StanWy)
updateVar(Id, state(Vars1, Arrs, CIRs), V, state(Vars2, Arrs, CIRs)) :-
    updateMap(Id, Vars1, V, Vars2).

% checkMap(+Id, +Map, -Value)
checkMap(Id, M, V) :- member(Id-V, M).

% checkVar(+Id, +Stan, -Value)
checkVar(Id, state(Vars, _, _), V) :- checkMap(Id, Vars, V).

% updateList(+Index, +ListWe, +Value, -ListWy)
updateList(0, [_|L], V, [V|L]) :- !.
updateList(I1, [E|L1], V, [E|L2]) :- 
    I1 > 0, I2 is I1 - 1, 
    updateList(I2, L1, V, L2).

% checkCIR(+Id, +Stan, -Value)
checkCIR(Id, state(_,_,CIRs), CIR) :- nth0(Id, CIRs, CIR).

% updateCIR(+Id, +StanWe, +V, -StanWy)
updateCIR(Id, state(Vars, Arrs, CIRs1), V, state(Vars, Arrs, CIRs2)) :-
    updateList(Id, CIRs1, V, CIRs2).

% checkArr(+Id, +Stan, -Arr)
checkArr(Id, state(_,Arrs,_), Arr) :- checkMap(Id, Arrs, Arr).

% updateArr(+Id, +StanWe, +Arr, -StanWy)
updateArr(Id, state(Vars, Arrs1, CIRs), Arr, state(Vars, Arrs2, CIRs)) :-
    updateMap(Id, Arrs1, Arr, Arrs2).

% evalIns(+Ins, +StanWe, +PrId, -StanWy)
evalIns(assign(Var, Expr), SIn, PrId, SOut) :-
    evalWyrArytm(Expr, SIn, PrId, V),
    updateZmienna(Var, SIn, V, PrId, S),
    move(S, PrId, SOut).
evalIns(goto(N), SIn, PrId, SOut) :-
    updateCIR(PrId, SIn, N, SOut).
evalIns(condGoto(BExpr, N), SIn, PrId, SOut) :-
    (evalWyrLogiczne(BExpr, SIn, PrId) ->
        updateCIR(PrId, SIn, N, SOut)
    ;
        move(SIn, PrId, SOut)
    ).
evalIns(sekcja, SIn, PrId, SOut) :-
    move(SIn, PrId, SOut).

% evalWyrLogiczne(+BExpr, +Stan, +PrId)
evalWyrLogiczne(E1 < E2, S, PrId) :-
    evalWyrProste(E1, S, PrId, N1),
    evalWyrProste(E2, S, PrId, N2),
    N1 < N2.
evalWyrLogiczne(E1 = E2, S, PrId) :-
    evalWyrProste(E1, S, PrId, N1),
    evalWyrProste(E2, S, PrId, N2),
    N1 = N2.
evalWyrLogiczne(E1 <> E2, S, PrId) :-
    evalWyrProste(E1, S, PrId, N1),
    evalWyrProste(E2, S, PrId, N2),
    N1 \= N2.

% updateZmienna(+Zmienna, +StaneWe, +Value, +PrId, -StanWy)
updateZmienna(Var, SIn, V, _, SOut) :-
    atom(Var),
    updateVar(Var, SIn, V, SOut).
updateZmienna(array(Var, Expr), SIn, V, PrId, SOut) :-
    evalWyrArytm(Expr, SIn, PrId, N),
    checkArr(Var, SIn, Arr1),
    updateList(N, Arr1, V, Arr2),
    updateArr(Var, SIn, Arr2, SOut).

% move(+StanWe, +PrId, -StanWy)
% -----------------------------
% Przesuwa licznik rozkazu procesu
% o jeden.
move(S1, Id, S2) :-
    checkCIR(Id, S1, N1),
    N2 is N1 + 1,
    updateCIR(Id, S1, N2, S2).

% evalWyrArytm(+Expr, +Stan, +PrId, -Value)
evalWyrArytm(E, S, PrId, N) :- atomic(E), evalWyrProste(E, S, PrId, N).
evalWyrArytm(E1 + E2, S, PrId, N) :- 
    evalWyrProste(E1, S, PrId, N1),
    evalWyrProste(E2, S, PrId, N2),
    N is N1 + N2.
evalWyrArytm(E1 - E2, S, PrId, N) :- 
    evalWyrProste(E1, S, PrId, N1),
    evalWyrProste(E2, S, PrId, N2),
    N is N1 - N2.
evalWyrArytm(E1 * E2, S, PrId, N) :- 
    evalWyrProste(E1, S, PrId, N1),
    evalWyrProste(E2, S, PrId, N2),
    N is N1 * N2.
evalWyrArytm(E1 / E2, S, PrId, N) :- 
    evalWyrProste(E1, S, PrId, N1),
    evalWyrProste(E2, S, PrId, N2),
    N is N1 / N2.

% evalWyrProste(+Expr, +Stan, +PrId, -Value)
evalWyrProste(pid, _, PrId, PrId) :- !.
evalWyrProste(N, _, _, N) :- number(N).
evalWyrProste(Var, S, _, V) :- atom(Var), checkVar(Var, S, V).
evalWyrProste(array(Var, Expr), S, PrId, V) :-
    evalWyrArytm(Expr, S, PrId, N),
    checkArr(Var, S, Arr),
    nth0(N, Arr, V).
