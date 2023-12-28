section .data
    const_1 dq 1
    const_1_m dq -1
    const_2 dq 2
    const_3 dq 3
    const_5 dq 5

section .text

global f_1
f_1: ;2^x + 1
    push ebp
    mov ebp, esp
    
    finit
    
    fld1 ;st0->1
    fld qword[ebp + 8] ;st0->x st1->1
    fprem ;st0->ost st1->1
    f2xm1 ;st0->2^ost - 1 st1->1
    faddp ;st0->2^ost
    fld qword[ebp + 8] ;st0->x st1->2^ost
    fld1;st0->1 st1->x st2->2^ost
    fscale;st0->2^[x] st1->x st2->2^ost
    fxch st1
    fxch st2
    fmulp
    fld1
    faddp
    fstp st1

    mov esp, ebp
    pop ebp
    ret

global f_2  
f_2: ;x^5
    push ebp
    mov ebp, esp
    
    finit
    
    fld qword[ebp + 8]
    fld qword[ebp + 8]
    fmulp 
    fld qword[ebp + 8]
    fmulp
    fld qword[ebp + 8]
    fmulp
    fld qword[ebp + 8]
    fmulp
    
    mov esp, ebp
    pop ebp
    ret

global f_3
f_3: ;(1-x)/3
    push ebp
    mov ebp, esp
    
    finit
    
    fild qword[const_1]
    fld qword[ebp + 8]
    fsubp
    
    fild qword[const_3]
    fdivp
    
    mov esp, ebp
    pop ebp
    ret
 
global df_1
df_1: ;ln2*2^x
    push ebp
    mov ebp, esp
    
    finit
    
    fld1 ;st0->1
    fld qword[ebp + 8] ;st0->x st1->1
    fprem ;st0->ost st1->1
    f2xm1 ;st0->2^ost - 1 st1->1
    faddp ;st0->2^ost
    fld qword[ebp + 8] ;st0->x st1->2^ost
    fld1;st0->1 st1->x st2->2^ost
    fscale;st0->2^[x] st1->x st2->2^ost
    fxch st1
    fxch st2
    fmulp
    fldln2
    fmulp

    mov esp, ebp
    pop ebp
    ret

global df_2  
df_2: ;5*x^4
    push ebp
    mov ebp, esp
    
    finit
    
    fild qword[const_5]
    fld qword[ebp + 8]
    fmulp
    fld qword[ebp + 8]
    fmulp 
    fld qword[ebp + 8]
    fmulp
    fld qword[ebp + 8]
    fmulp
    
    mov esp, ebp
    pop ebp
    ret

global df_3
df_3: ;-1/3
    push ebp
    mov ebp, esp
    
    finit
    
    fild qword[const_1_m]    
    fild qword[const_3]
    fdivp
    
    mov esp, ebp
    pop ebp
    ret
