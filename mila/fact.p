program example_program;

function fact(n : integer) : integer;
begin
    if (n <= 1) then begin
        fact := 1;
    end else begin
        fact := n * fact(n - 1);
    end;
end;

const x = 67, y = 32, true = 1, false = 0 : integer;
var v1 : integer;
begin
    writeln(fact(1));
    writeln(fact(2));
    writeln(fact(3));
    writeln(fact(4));
    writeln(fact(5));
    writeln(fact(6));
    writeln(fact(7));
    writeln(fact(8));
    writeln(fact(9));
    writeln(fact(10));
end.
