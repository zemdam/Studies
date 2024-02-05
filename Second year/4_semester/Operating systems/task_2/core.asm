global core

; uint64_t get_value(uint64_t n)
extern get_value

; void put_value(uint64_t n, uint64_t w)
extern put_value

section .data

align 8
; visit[i] = x - core i wants to exchange with core x.
visit: times N dq N

section .bss

align 8
; value[i] = x - core i offers value x to exchange.
value: resq N

section .text

; ========================================
; uint64_t core(uint64_t n, char const *p)
; ========================================
;
; Performs operations on the stack.
;
; ENTRY: n - core number
;        p - pointer to string
;
; RETURN: Top of the stack.
;
; REGISTER: rdi - n
;           rsi - p
core:
        ; Saves registers on the stack.
        push    rbx
        push    rbp
        push    r12
        push    r14

        mov     rbx, rsp                        ; rbx saves rsp for exit.
        mov     rbp, rdi                        ; rbp holds n.
        mov     r12, rsi                        ; r12 holds p.
        xor     eax, eax

.start:
        mov     al, [r12]                       ; al holds current char from string.

        cmp     al, `\0`
        jne     .skip_end

        pop     rax                             ; rax now holds value to return.
        mov     rsp, rbx                        ; Removes all putted values on the stack.

        ; Restores registers values.
        pop     r14
        pop     r12
        pop     rbp
        pop     rbx
        ret

.skip_end:
        cmp     al, `+`
        jne     .skip_plus

        ; Adds first two values on the stack.
        pop     rcx
        add     [rsp], rcx

.skip_plus:
        cmp     al, `*`
        jne     .skip_multiply

        ; Multiplies first two values on the stack.
        pop     rcx
        pop     rdi
        imul    rcx, rdi
        push    rcx

.skip_multiply:
        cmp     al, `-`
        jne     .skip_negative

        neg     qword [rsp]

.skip_negative:
        cmp     al, `n`
        jne     .skip_core_number

        push    rbp                             ; Pushes n to the stack.

.skip_core_number:
        cmp     al, `B`
        jne     .skip_drop_with_jump

        pop     rdi                             ; rdi holds string shift value.
        
        ; Checks if top of the stack is zero.
        pop     rcx                             
        test    rcx, rcx
        push    rcx

        jz      .skip_drop_with_jump
        lea     r12, [r12 + rdi]                ; Shifts current char in string.

.skip_drop_with_jump:
        cmp     al, `C`
        jne     .skip_drop

        pop     rcx                             ; rcx is only used as trash for top of the stack.

.skip_drop:
        cmp     al, `D`
        jne     .skip_duplicate

        push    qword [rsp]                     ; Duplicates top of the stack.

.skip_duplicate:
        cmp     al, `E`
        jne     .skip_exchange

        ; Swaps first two values on the stack.
        pop     rcx
        pop     rdi
        push    rcx
        push    rdi

.skip_exchange:
        cmp     al, `P`
        jne     .skip_put

        lea     rcx, [rel put_value]            ; rcx holds address of function put_value.

        pop     rsi                             ; Top of the stack will be used as w in put_value.
        jmp     .function_call   

.skip_put:
        cmp     al, `G`
        jne     .skip_get

        lea     rcx, [rel get_value]            ; rcx holds address of function get_value.

.function_call:
        push    r13                             ; Saves r13.
        mov     r13, rsp                        ; Saves top of the stack in r13.
        and     rsp, -16                        ; Makes sure that rsp is dividable by 16.

        mov     rdi, rbp                        ; rdi holds value of n.
        mov     r14, rax                        ; Saves rax;
        call    rcx                             ; Calls put_value or get_value.
        mov     rsp, r13                        ; Restores top of the stack.
        pop     r13                             ; Restores r13.

        ; Checks which function was called.
        cmp     r14b, `G`
        jne     .skip_push

        push    rax                             ; Puts return value of get_value on the stack.

.skip_push:
        mov     rax, r14                        ; Restores rax.

.skip_get:
        cmp     al, `S`
        jne     .skip_sync

        mov     rcx, N
        lea     r8, [rel visit]

.free_value:
        cmp     [r8 + rbp*8], rcx               ; Checks if my exchange finished.
        jne     .free_value
        pop     rsi                             ; rsi holds core to exchange partner.
        pop     rdx                             ; rdx holds value to offer.
        lea     rdi, [rel value]
        mov     [rdi + rbp*8], rdx              ; Loads offer value to value array.
        mov     [r8 + rbp*8], rsi               ; Loads partner to visit array.

.not_my_turn:
        cmp     [r8 + rsi*8], rbp               ; Checks if can i exchange.
        jne     .not_my_turn
        push    qword [rdi + rsi*8]             ; Pushes partner offered value on the stack.
        mov     [r8 + rsi*8], rcx               ; Mark exchange as finished.

.skip_sync:
        ; Checks if char represents a digit.
        cmp     al, `0`
        jb      .skip_number
        cmp     al, `9`
        ja      .skip_number

        sub     al, `0`                         ; Converts char to digit.

        push    rax                             ; Push digit with zero extend on the stack.
        
.skip_number:
        inc     r12
        jmp     .start