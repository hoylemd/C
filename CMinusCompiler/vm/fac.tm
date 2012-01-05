* C-Minus Compilation to TM Code
* File: fac.tm
* Standard prelude:
  0:     LD  6,0(0) 	load gp with maxaddress
  1:    LDA  5,0(6) 	copy to gp to fp
  2:     ST  0,0(0) 	clear location 0
* Jump around i/o routines here
* code for input routine
  4:     ST  0,-1(5) 	store return
  5:     IN  0,0,0 	input
  6:     LD  7,-1(5) 	return to caller
* code for output routine
  7:     ST  0,-1(5) 	store return
  8:     LD  0,-2(5) 	load output value
  9:    OUT  0,0,0 	output
 10:     LD  7,-1(5) 	return to caller
  3:    LDA  7,7(7) 	jump around i/o code
* End of standard prelude.
* processing function: main
* jump around function body here
 12:     ST  0,-1(5) 	store return
* -> compound statement
* processing local var: x
* processing local var: fact
* -> op
* -> id
* looking up id: x
 13:    LDA  0,-2(5) 	load id address
* <- id
 14:     ST  0,-4(5) 	op: push left
* -> call of function: input
 15:     ST  5,-5(5) 	push ofp
 16:    LDA  5,-5(5) 	push frame
 17:    LDA  0,1(7) 	load ac with ret ptr
 18:    LDA  7,-15(7) 	jump to fun loc
 19:     LD  5,0(5) 	pop frame
* <- call
 20:     LD  1,-4(5) 	op: load left
 21:     ST  0,0(1) 	assign: store value
* <- op
* -> if
* -> op
* -> id
* looking up id: x
 22:     LD  0,-2(5) 	load id value
* <- id
 23:     ST  0,-4(5) 	op: push left
* -> constant
 24:    LDC  0,0(0) 	load const
* <- constant
 25:     LD  1,-4(5) 	op: load left
 26:    SUB  0,1,0 	op >=
 27:    JGE  0,2(7) 	br if true
 28:    LDC  0,0(0) 	false case
 29:    LDA  7,1(7) 	unconditional jmp
 30:    LDC  0,1(0) 	true case
* <- op
* if: jump to else belongs here
* -> compound statement
* -> op
* -> id
* looking up id: fact
 32:    LDA  0,-3(5) 	load id address
* <- id
 33:     ST  0,-4(5) 	op: push left
* -> constant
 34:    LDC  0,1(0) 	load const
* <- constant
 35:     LD  1,-4(5) 	op: load left
 36:     ST  0,0(1) 	assign: store value
* <- op
* -> while
* while: jump after body comes back here
* -> op
* -> id
* looking up id: x
 37:     LD  0,-2(5) 	load id value
* <- id
 38:     ST  0,-4(5) 	op: push left
* -> constant
 39:    LDC  0,0(0) 	load const
* <- constant
 40:     LD  1,-4(5) 	op: load left
 41:    SUB  0,1,0 	op >
 42:    JGT  0,2(7) 	br if true
 43:    LDC  0,0(0) 	false case
 44:    LDA  7,1(7) 	unconditional jmp
 45:    LDC  0,1(0) 	true case
* <- op
* while: jump to end belongs here
* -> compound statement
* -> op
* -> id
* looking up id: fact
 47:    LDA  0,-3(5) 	load id address
* <- id
 48:     ST  0,-4(5) 	op: push left
* -> op
* -> id
* looking up id: fact
 49:     LD  0,-3(5) 	load id value
* <- id
 50:     ST  0,-5(5) 	op: push left
* -> id
* looking up id: x
 51:     LD  0,-2(5) 	load id value
* <- id
 52:     LD  1,-5(5) 	op: load left
 53:    MUL  0,1,0 	op *
* <- op
 54:     LD  1,-4(5) 	op: load left
 55:     ST  0,0(1) 	assign: store value
* <- op
* -> op
* -> id
* looking up id: x
 56:    LDA  0,-2(5) 	load id address
* <- id
 57:     ST  0,-4(5) 	op: push left
* -> op
* -> id
* looking up id: x
 58:     LD  0,-2(5) 	load id value
* <- id
 59:     ST  0,-5(5) 	op: push left
* -> constant
 60:    LDC  0,1(0) 	load const
* <- constant
 61:     LD  1,-5(5) 	op: load left
 62:    SUB  0,1,0 	op -
* <- op
 63:     LD  1,-4(5) 	op: load left
 64:     ST  0,0(1) 	assign: store value
* <- op
* <- compound statement
 65:    LDA  7,-29(7) 	while: absolute jmp to test
 46:    JEQ  0,19(7) 	while: jmp to end
* <- while
* -> call of function: output
* -> id
* looking up id: fact
 66:     LD  0,-3(5) 	load id value
* <- id
 67:     ST  0,-6(5) 	store arg val
 68:     ST  5,-4(5) 	push ofp
 69:    LDA  5,-4(5) 	push frame
 70:    LDA  0,1(7) 	load ac with ret ptr
 71:    LDA  7,-65(7) 	jump to fun loc
 72:     LD  5,0(5) 	pop frame
* <- call
* <- compound statement
* if: jump to end belongs here
 31:    JEQ  0,42(7) 	if: jmp to else
 73:    LDA  7,0(7) 	jmp to end
* <- if
* <- compound statement
 74:     LD  7,-1(5) 	return to caller
 11:    LDA  7,63(7) 	jump around fn body
* <- fundecl
 75:     ST  5,0(5) 	push ofp
 76:    LDA  5,0(5) 	push frame
 77:    LDA  0,1(7) 	load ac with ret ptr
 78:    LDA  7,-67(7) 	jump to main loc
 79:     LD  5,0(5) 	pop frame
* End of execution.
 80:   HALT  0,0,0 	
