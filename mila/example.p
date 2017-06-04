program example_program;

function fact(i : integer; j : integer) : integer; begin
    writeln(1);
    writeln(i);
    writeln(2);
    writeln(j);
    fact := i + j;
end;

const x = 67, y = 32, true = 1, false = 0 : integer;
var v1 : integer;
begin
    v1 := fact(y, x);
    writeln(3);
    writeln(v1);
end.
