program example_program;

function f1(n : integer) : integer;
begin
    if (n = 1) then begin
        f1 := 2 * f2(2);
    end else begin
        f1 := 7 + n;
    end;
end;

function f2(n : integer) : integer;
begin
    f2 := f1(n + 1);
end;

begin
    writeln(f1(1));
end.
