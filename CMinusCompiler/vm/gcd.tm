* C-Minus Compilation to TM Code
* File: gcd.tm
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
* processing function: gcd
* jump around function body here
 12:     ST  0,-1(5) 	store return
* -> compound statement
* -> if
* -> op
* -> id
* looking up id: v
 13:     LD  0,-3(5) 	load id value
* <- id
 14:     ST  0,-4(5) 	op: push left
* -> constant
 15:    LDC  0,0(0) 	load const
* <- constant
 16:     LD  1,-4(5) 	op: load left
 17:    SUB  0,1,0 	op ==
 18:    JEQ  0,2(7) 	br if true
 19:    LDC  0,0(0) 	false case
 20:    LDA  7,1(7) 	unconditional jmp
 21:    LDC  0,1(0) 	true case
* <- op
* if: jump to else belongs here
* -> return
* -> id
* looking up id: u
 23:     LD  0,-2(5) 	load id value
* <- id
 24:     LD  7,-1(5) 	return to caller
* <- return
* if: jump to end belongs here
 22:    JEQ  0,3(7) 	if: jmp to else
* -> return
* -> call of function: gcd
* -> id
* looking up id: v
 26:     LD  0,-3(5) 	load id value
* <- id
 27:     ST  0,-6(5) 	store arg val
* -> op
* -> id
* looking up id: u
 28:     LD  0,-2(5) 	load id value
* <- id
 29:     ST  0,-7(5) 	op: push left
* -> op
* -> op
* -> id
* looking up id: u
 30:     LD  0,-2(5) 	load id value
* <- id
 31:     ST  0,-8(5) 	op: push left
* -> id
* looking up id: v
 32:     LD  0,-3(5) 	load id value
* <- id
 33:     LD  1,-8(5) 	op: load left
 34:    DIV  0,1,0 	op /
* <- op
 35:     ST  0,-8(5) 	op: push left
* -> id
* looking up id: v
 36:     LD  0,-3(5) 	load id value
* <- id
 37:     LD  1,-8(5) 	op: load left
 38:    MUL  0,1,0 	op *
* <- op
 39:     LD  1,-7(5) 	op: load left
 40:    SUB  0,1,0 	op -
* <- op
 41:     ST  0,-7(5) 	store arg val
 42:     ST  5,-4(5) 	push ofp
 43:    LDA  5,-4(5) 	push frame
 44:    LDA  0,1(7) 	load ac with ret ptr
 45:    LDA  7,-34(7) 	jump to fun loc
 46:     LD  5,0(5) 	pop frame
* <- call
 47:     LD  7,-1(5) 	return to caller
* <- return
 25:    LDA  7,22(7) 	jmp to end
* <- if
* <- compound statement
 48:     LD  7,-1(5) 	return to caller
 11:    LDA  7,37(7) 	jump around fn body
* <- fundecl
* processing function: main
* jump around function body here
 50:     ST  0,-1(5) 	store return
* -> compound statement
* processing local var: x
* processing local var: y
* -> op
* -> id
* looking up id: x
 51:    LDA  0,-2(5) 	load id address
* <- id
 52:     ST  0,-4(5) 	op: push left
* -> call of function: input
 53:     ST  5,-5(5) 	push ofp
 54:    LDA  5,-5(5) 	push frame
 55:    LDA  0,1(7) 	load ac with ret ptr
 56:    LDA  7,-53(7) 	jump to fun loc
 57:     LD  5,0(5) 	pop frame
* <- call
 58:     LD  1,-4(5) 	op: load left
 59:     ST  0,0(1) 	assign: store value
* <- op
* -> op
* -> id
* looking up id: y
 60:    LDA  0,-3(5) 	load id address
* <- id
 61:     ST  0,-4(5) 	op: push left
* -> call of function: input
 62:     ST  5,-5(5) 	push ofp
 63:    LDA  5,-5(5) 	push frame
 64:    LDA  0,1(7) 	load ac with ret ptr
 65:    LDA  7,-62(7) 	jump to fun loc
 66:     LD  5,0(5) 	pop frame
* <- call
 67:     LD  1,-4(5) 	op: load left
 68:     ST  0,0(1) 	assign: store value
* <- op
* -> call of function: output
* -> call of function: gcd
* -> id
* looking up id: x
 69:     LD  0,-2(5) 	load id value
* <- id
 70:     ST  0,-8(5) 	store arg val
* -> id
* looking up id: y
 71:     LD  0,-3(5) 	load id value
* <- id
 72:     ST  0,-9(5) 	store arg val
 73:     ST  5,-6(5) 	push ofp
 74:    LDA  5,-6(5) 	push frame
 75:    LDA  0,1(7) 	load ac with ret ptr
 76:    LDA  7,-65(7) 	jump to fun loc
 77:     LD  5,0(5) 	pop frame
* <- call
 78:     ST  0,-6(5) 	store arg val
 79:     ST  5,-4(5) 	push ofp
 80:    LDA  5,-4(5) 	push frame
 81:    LDA  0,1(7) 	load ac with ret ptr
 82:    LDA  7,-76(7) 	jump to fun loc
 83:     LD  5,0(5) 	pop frame
* <- call
* <- compound statement
 84:     LD  7,-1(5) 	return to caller
 49:    LDA  7,35(7) 	jump around fn body
* <- fundecl
 85:     ST  5,0(5) 	push ofp
 86:    LDA  5,0(5) 	push frame
 87:    LDA  0,1(7) 	load ac with ret ptr
 88:    LDA  7,-39(7) 	jump to main loc
 89:     LD  5,0(5) 	pop frame
* End of execution.
 90:   HALT  0,0,0 	
