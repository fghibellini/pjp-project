program example_program;

function fact(i : integer) : integer; begin
    fact := 42;
end;

const x = 67, y = 32, true = 1, false = 0 : integer;
var v1 : integer;
begin
    read(v1);
    writeln(1 + fact(v1));
end.
