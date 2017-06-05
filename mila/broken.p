program example_program;

function fact(n : integer) : integer; begin
    if (n <= 1) then begin
        fact := 10;
    end else begin
        fact := 20;
    end;
    writeln(fact);
end;

const x = 67, y = 32, true = 1, false = 0 : integer;
var v1 : integer;
begin
    fact(2);
end.
