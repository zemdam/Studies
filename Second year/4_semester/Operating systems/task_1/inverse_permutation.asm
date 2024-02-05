section .data
        SGN_BIT equ 80000000H                   ; Bit used for checking if number was visited.
        STEP    equ 4                           ; Number of bytes per each element in array.
        MAX_N   equ 2147483647                  ; n - 1 must be lower or equal to this value.

section .text

global inverse_permutation

; ==========================================
; bool inverse_permutation(size_t n, int *p)
; ==========================================
;
; Inverts permutations.
;
; ENTRY: n - size of an array
;        p - pointer to an array
; 
; EXIT:  true  - if n > 0 and n <= 2^31 and p[] contains permutation from 0 to n-1 
;        false - otherwise
;
; REGISTER: rdi - n
;           rsi - p
inverse_permutation:
        dec     rdi                             ; Now rdi holds n - 1.
        js      .false                          ; Checks if edi is lower than zero.

        ; Makes sure that n is lower than 2^31 + 1
        cmp     rdi, MAX_N
        ja      .false

        mov     ecx, edi                        ; ecx holds current index i = n - 1.

.check_loop:
        ; Makes sure that element in array is lower than n.
        mov     r8d, [rsi+rcx*STEP]             ; Now r8d holds a[i].
        and     r8d, MAX_N                      ; Unset 32bit if set.
        cmp     edi, r8d                        ; Compare n - 1 with a[i].
        jb      .false_over_n                   

        ; Checks if number was already visited.
        xor     dword [rsi+r8*STEP], SGN_BIT 
        jns     .false_visited

        dec     ecx
        jns     .check_loop

        mov     ecx, edi                        ; ecx holds current index i = n - 1.

.reverse_loop:
        ; Skips visited cycles.
        mov     r9d, [rsi+rcx*STEP]             ; r9 holds next index in cycle.
        xor     r9d, SGN_BIT                    ; Checks if visited.
        js      .next_index

        xor     dword [rsi+rcx*STEP], SGN_BIT   ; Marks as visited.
        
.cycle_loop:
        ; Inverts elements in a cycle.
        mov     r10d, [rsi+r9*STEP]             ; r10 holds next index in cycle.
        mov     [rsi+r9*STEP], ecx; 
        mov     ecx, r9d                        ; Change current index to next index in cycle.
        mov     r9d, r10d                       ; r9 again holds index in cycle.
        xor     r9d, SGN_BIT                    ; Checks if visited.
        jns     .cycle_loop

.next_index:
        dec     ecx
        jns     .reverse_loop

.true:
        ; Returns true.
        stc
        setc    al
        ret

.false_loop:
        ; Reverts changes.
        mov     r8d, dword [rsi+rcx*STEP]       ; Gets next index of number to unmark.
        and     r8d, MAX_N                      ; Unset 32bit if was set.
.false_visited:
        xor     dword [rsi+r8*STEP], SGN_BIT    ; Unmark number. 
.false_over_n:
        inc     ecx
        cmp     ecx, edi                        ; Checks if current index is out of range.
        jbe     .false_loop

.false:
        ; Returns false.
        xor     eax, eax
        ret   