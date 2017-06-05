; ModuleID = 'mila compiler'
source_filename = "mila compiler"

declare void @write(i32)

declare void @writeln(i32)

declare i32 @read()

define i32 @main() {
main_entry:
  br label %scope

scope:                                            ; preds = %main_entry
  %v1 = alloca i32
  %calltmp = call i32 @fact(i32 2)
  ret i32 0
}

define i32 @fact(i32 %n1) {
fact_entry:
  %fact = alloca i32
  %n = alloca i32
  store i32 %n1, i32* %n
  br label %scope

scope:                                            ; preds = %fact_entry
  %n2 = load i32, i32* %n
  %cmple_res = icmp sle i32 %n2, 1
  %cmple_res8b = bitcast i1 %cmple_res to i32
  %ifcond = icmp ne i32 %cmple_res8b, 0
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %scope
  store i32 10, i32* %fact
  br label %ifcont

else:                                             ; preds = %scope
  store i32 20, i32* %fact
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %fact3 = load i32, i32* %fact
  %calltmp = call void @writeln(i32 %fact3)
  %fact4 = load i32, i32* %fact
  ret i32 %fact4
}
